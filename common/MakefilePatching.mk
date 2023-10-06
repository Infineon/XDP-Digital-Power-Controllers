# =================================================================================================
# File:     MakefilePatching.mk
# Project:  Shasta Firmware
# -------------------------------------------------------------------------------------------------
# Brief:    Makefile plugin for patch images.
#           Include this file before MakefileCommon.mk to create patch images or full patch images.
#           A patch image is linked against the most parts of a ROM image.
#           A full patch image is similar to a RAM image with the exception that it may reference
#           some libraries from the ROM.
# =================================================================================================

###################################################################################################
# Extend the help menue
###################################################################################################

# Extend "help" with the information about the patch targets
help: patch_help
.PHONY: patch_help
patch_help:
	@$(ECHO) 'make patch            Create the patch image in $(PATCH_BUILD_DIR)'
	@$(ECHO) 'make clean_patch      Remove $(PATCH_BUILD_DIR)'
	@$(ECHO) 'make patch_full       Create the full patch image in $(PATCH_FULL_BUILD_DIR)'
	@$(ECHO) 'make clean_patch_full Remove $(PATCH_FULL_BUILD_DIR)'


###################################################################################################
# Generation of patch images for released versions
# A patch needs other (and probably much less) objects than a full image. Therefore a patch target
# needs its own build directory instead of reusing the objects of the full image.
###################################################################################################

# Variables =======================================================================================

ifeq ($(BUILD_DIR), ) # if empty
    BUILD_DIR = ./build
endif

PATCH_BUILD_DIR = $(BUILD_DIR)/patch
PATCH_FULL_BUILD_DIR = $(BUILD_DIR)/patch_full

PATCH_SRC_DIRS_FILTER := \
    ../version/ \
	$(REPO_ROOT_DIR)/common/system_header/gnu/sys/ \
	$(REPO_ROOT_DIR)/common/system_header/c_keil/ \
	$(filter %/, $(wildcard $(REPO_ROOT_DIR)/common/modules/*/)) 
    
PATCH_SRC_DIRS := $(filter-out $(PATCH_SRC_DIRS_FILTER), $(SOURCE_DIRS))

PATCH_C_SOURCE_FILES := $(foreach dir, $(PATCH_SRC_DIRS:%/=%), $(wildcard $(dir)/*.c))
PATCH_H_SOURCE_FILES := $(foreach dir, $(PATCH_SRC_DIRS:%/=%), $(wildcard $(dir)/*.h))
PATCH_OTHER_SOURCE_FILES := $(foreach dir, $(PATCH_SRC_DIRS:%/=%),\
                        $(wildcard $(dir)/*.s) $(wildcard $(dir)/*.ld) $(wildcard $(dir)/*.sct $(dir)/sym_keep))

ifeq ("$(OS)", "Windows")
    PATCH_BUILD_DIR := $(subst /,\,$(PATCH_BUILD_DIR))
    PATCH_C_SOURCE_FILES := $(subst /,\,$(PATCH_C_SOURCE_FILES))
    PATCH_H_SOURCE_FILES := $(subst /,\,$(PATCH_H_SOURCE_FILES))
    PATCH_OTHER_SOURCE_FILES := $(subst /,\,$(PATCH_OTHER_SOURCE_FILES))
endif
                            
# A patch image is linked against the ROM image.
ifdef PATCH
    ENABLE_PATCH_BUILD := 1
    PATCH_FLAG :=
    override IMAGE_NAME := patch
    override IMAGE_BUILD_DIR := $(PATCH_BUILD_DIR)
    override OBJECT_BUILD_DIR := $(PATCH_BUILD_DIR)
	ifneq ($(TOOLCHAIN), ARM_GCC)
	# ARM Compiler
	# For patches the linker gets the object files, which passes the ARM linker steering process.
    # This steering process hides or manipulates the global exported object file symbols.
    # It allows to hide compiled functions and global variables, and the linking process
    # uses the corresponding symbols of the patched ROM image.
    override LINKER_FILES = $(foreach file, $(notdir $(CCOMPILER_FILES)) $(notdir $(ASSEMBLER_FILES)), \
        $(OBJECT_BUILD_DIR)/$(basename $(file)).steering)

    # The patch compile process generates the "*.o" object files are intermediate input
    # for the ARM linker steering process to generate the "*.steering" files. Surpress that
    # make deleted such intermediate generated files.
    .SECONDARY:  $(foreach file, $(notdir $(CCOMPILER_FILES)) $(notdir $(ASSEMBLER_FILES)), \
        $(OBJECT_BUILD_DIR)/$(basename $(file)).o)
    endif
endif

# A full patch image is like a RAM image that stores code, data, stack and heap in RAM.
# Additionally it uses parts of the existing ROM code.
ifdef PATCH_FULL
    ENABLE_PATCH_BUILD := 1
    PATCH_FLAG := --full-patch
    override IMAGE_NAME := patch_full
    override IMAGE_BUILD_DIR := $(PATCH_FULL_BUILD_DIR)
endif

# The following lines are applicable to the target "patch".
ifdef ENABLE_PATCH_BUILD

    # For generating patches the linked output is made dependend on a filtered *.sym file 
    # and the reference *.elf files.
    # Note: 'PATCH_SYM_FILTERED_FILE' is not included for non-patch targets.

    ifeq ($(PATCH_REF_IMAGE_NAMES), ) # if empty
        $(error A patch must have at least one reference image PATCH_REF_IMAGE_NAMES where to apply)
    endif

   ifeq ($(PATCH_LINK_RANGE_CONFIG_FILE), ) # if empty
        $(error A patch must have at least one reference image PATCH_LINK_RANGE_CONFIG_FILE where to apply)
   endif

    # Check for the file which contains regular expressions for the symbols to keep
    ifeq ($(PATCH_SYM_KEEP_FILE), ) # if empty
        # Search for a single linker symbol file in all source directories
        PATCH_SYM_KEEP_FILE := $(strip $(foreach dir,$(SOURCE_DIRS:%/=%),$(wildcard $(dir)/sym_keep)))
        ifeq ($(words $(PATCH_SYM_KEEP_FILE)), 0)
            # Generates an dependencies error in the target if the file is not here
            PATCH_SYM_KEEP_FILE := ./sym_keep
        else
            ifneq ($(words $(PATCH_SYM_KEEP_FILE)), 1)
                $(error Too many symbol filter files found ($(PATCH_SYM_KEEP_FILE)).)
                $(error Specify a single file which contains the symbols to keep with PATCH_SYM_KEEP_FILE)
            endif
        endif
    endif

    # There should be one 'elf' file and one 'sym' file for each release reference image inside the
    # git root directory '/versions'.
    PATCH_SYM_IMPORT_FILES := $(foreach file, $(PATCH_REF_IMAGE_NAMES), $(file).sym)
    PATCH_ELF_IMPORT_FILES := $(foreach file, $(PATCH_REF_IMAGE_NAMES), $(file).elf)

	ifeq ($(TOOLCHAIN), ARM_GCC)
	  #GCC COMPILER
      # Define the filtered symbol file and add it to the linker input
      PATCH_SYM_FILTERED_FILE = $(IMAGE_BUILD_DIR)/patch_filtered.syml
      LINKER_PARAMS += -L $(IMAGE_BUILD_DIR)
      LINKER_PARAMS += -Xlinker "--just-symbols=$(PATCH_SYM_FILTERED_FILE)"
    else
 	  #ARM COMPILER 
      # Define the filtered symbol file and add it to the linker input
      PATCH_SYM_FILTERED_FILE = $(IMAGE_BUILD_DIR)/patch_filtered.sym
      LINKER_PARAMS += $(PATCH_SYM_FILTERED_FILE) 	    
    endif

    # Define the filtered symbol steering file to object file based partial linking
    STEERING_FILTERED_FILE = $(IMAGE_BUILD_DIR)/steering.cfg

    # Define the output header file which contains the address ranges for the target patch image.
    # The linker scatter file will include such header file to place the patch at the right place.
    ifeq ($(TOOLCHAIN), ARM_GCC)
	  #GCC COMPILER
      PATCH_LINKER_HEADER_FILE = $(IMAGE_BUILD_DIR)/patch_link_location.ld
    else
 	  #ARM COMPILER
      PATCH_LINKER_HEADER_FILE = $(IMAGE_BUILD_DIR)/patch_link_location.h
 	endif
 	
    # Will be defined below the patch targets but these variables need to be defined above them
    override IMAGE_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).elf
    override IMAGE2HEX_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).hex

    # Only ROM or RAM images need a CRC sum at their end. So do not build the file.
    override HEXCRC32_FILE :=
endif # ENABLE_PATCH_BUILD

# Build & Cleanup labels ==========================================================================

.PHONY: patch
patch:
	@$(ECHO) '$(___TARGET_SEPARATOR_LINE___)'
	@$(ECHO) Copy \(or update\) all source files to "$(PATCH_BUILD_DIR)"
	@$(MKDIR) $(PATCH_BUILD_DIR)
	@$(COPY) $(PATCH_C_SOURCE_FILES) $(PATCH_BUILD_DIR)
	@$(COPY) $(PATCH_H_SOURCE_FILES) $(PATCH_BUILD_DIR)
	@$(COPY) $(PATCH_OTHER_SOURCE_FILES) $(PATCH_BUILD_DIR)
	@$(MAKE) image
	
.PHONY: clean_patch
clean_patch:
	@$(MAKE) clean_image PATCH=1

.PHONY: patch_full
patch_full:
	@$(ECHO) '$(___TARGET_SEPARATOR_LINE___)'
	@$(MAKE) image PATCH_FULL=1

.PHONY: clean_patch_full
clean_patch_full:
	@$(MAKE) clean_image PATCH_FULL=1

# Targets =========================================================================================

ifdef ENABLE_PATCH_BUILD

# The patch output file might be linked against any other previous patch image version
# or against the ROM code image. The link process of such images generates linker symbol files
# (*.sym - files) which are used as input for the linking process.
#
# Using such sym-files in the linker might generate conflicts regarding symbol re-definition,
# because the patch might replace some of previous symbol implementations
# (e.g. patching the ROM features by an updated implementation).
# The following build target $(PATCH_SYM_FILTERED_FILE) takes the input sym-file and only keeps the necessary symbols for the
# linker process (based on a given configuration file).
ifeq ($(TOOLCHAIN), ARM_GCC)
  #GCC COMPILER
  SYM_OPTION = g
else
  #ARM COMPILER
  SYM_OPTION = m
endif
	  
$(PATCH_SYM_FILTERED_FILE): $(PATCH_SYM_IMPORT_FILES) $(PATCH_SYM_KEEP_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating Patch Symbol Filtered File "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/linker_sim_filter.py -$(SYM_OPTION) $@ -c $(PATCH_SYM_KEEP_FILE) $(PATCH_SYM_IMPORT_FILES)

# The following build target $(STEERING_FILTERED_FILE) takes the input sym-file and masks out the kept ROM symbol 
# by generating object file partial linking "HIDE" rule (based on a given configuration file). 
# This rules are applied on the compiled object files in a partial linking steering process to remove such symbols. 
# The partial linking object are then taken as input for the final image linking process. 
$(STEERING_FILTERED_FILE): $(PATCH_SYM_IMPORT_FILES) $(PATCH_SYM_KEEP_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating Patch Symbol Filtered File "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/linker_sim_filter.py -s $@ -c $(PATCH_SYM_KEEP_FILE) $(PATCH_SYM_IMPORT_FILES)

# The partial patch RAM location depends on the RAM usage of the reference images. Therefore
# we scan all given reference *.elf files and search the RAM for free space to place the 
# target patch file.
ifeq ($(TOOLCHAIN), ARM_GCC)
  #GCC COMPILER
  Patch_Linker_Location_OPTION = g
else
  #ARM COMPILER
  Patch_Linker_Location_OPTION = a
endif
	 
$(PATCH_LINKER_HEADER_FILE): $(PATCH_ELF_IMPORT_FILES)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating Patch Linker Location Header File "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/elf_ram_allocater.py -$(Patch_Linker_Location_OPTION) $@ -c $(PATCH_LINK_RANGE_CONFIG_FILE) $(PATCH_ELF_IMPORT_FILES)

# Changes on the symbol files for patches should result in rebuilding the image
$(IMAGE_FILE): $(PATCH_SYM_FILTERED_FILE) $(PATCH_LINKER_HEADER_FILE)

ifneq ($(TOOLCHAIN), ARM_GCC)
  #ARM COMPILER
# Generate the ARM linker steering files out of the compiled "*.o" files,
# which will be the input for the linking process.
# This steering process hides or manipulates the global exported object file symbols.
# It allows to hide compiled functions and global variables, and the linking process
# uses the corresponding symbols of the patched ROM image.
%.steering: %.o $(STEERING_FILTERED_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Steering symbols on "$@"
	$(LINKER) --partial $< --edit=$(STEERING_FILTERED_FILE) --diag_suppress=L6331W --output=$@
endif
# Extension targets ===============================================================================

OTP_CONFIG_FILE := $(REPO_ROOT_DIR)/tools/scripts/otp_file_gen/otp_files/config_otp_ACF_P5_3P3V_30A.hex

PATCH_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).bin
image: $(PATCH_FILE)
$(PATCH_FILE): $(IMAGE2HEX_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating Patch Binary File "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/hex2patch_bin.py --crc-header --crc-data $(PATCH_FLAG) -c $(PATCH_LINK_RANGE_CONFIG_FILE) -i $< -o $@

PATCH_HEX_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME)_otp.hex
image: $(PATCH_HEX_FILE)
$(PATCH_HEX_FILE): $(IMAGE2HEX_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating Patch Hex File "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/hex2patch_bin.py --crc-header --crc-data $(PATCH_FLAG) -c $(PATCH_LINK_RANGE_CONFIG_FILE) -i $< -p $@

PATCH_OTP_FILE = $(IMAGE_BUILD_DIR)/$(IMAGE_NAME)_otp.dat
image: $(PATCH_OTP_FILE)
$(PATCH_OTP_FILE): $(PATCH_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating OTP Patch File "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/patch_bin2sim_hex.py -i -c -o $@ $<

PATCH_PMBUS_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME)_pmbus.dat
image: $(PATCH_PMBUS_FILE)
$(PATCH_PMBUS_FILE): $(PATCH_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating PMBus downloadable Patch File "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/patch_bin2sim_hex.py -c -o $@ $<

endif # ENABLE_PATCH_BUILD
