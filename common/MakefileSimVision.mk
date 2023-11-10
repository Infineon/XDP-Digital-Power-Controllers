# =================================================================================================
# File:     MakefileSimVision.mk
# Project:  Shasta Firmware
# -------------------------------------------------------------------------------------------------
# Brief:    Makefile plugin for SimVision images.
#           Include this file before MakefileCommon.mk to create a dedicated target for SimVision.
#           SimVision requires a flat build directory structure, where source-, header- and output
#           files are mixed.
# =================================================================================================

###################################################################################################
# Extend the help menue
###################################################################################################

# Extend "help" with the information about the Simvision specific targets
help: sim_help
.PHONY: sim_help
sim_help:
	@$(ECHO) 'make simvision        Create image (in $(SIMVISION_BUILD_DIR))'
	@$(ECHO) 'make clean_simvision  Remove $(SIMVISION_BUILD_DIR)'


###################################################################################################
# Building the firmware image for Simvision
###################################################################################################

# Variables =======================================================================================

ifeq ($(BUILD_DIR), ) # if empty
    BUILD_DIR = ./build
endif

# Determine the OS based on standard environment variables.
# Note that if you are running under BASH, make sure that the BASH variable is
# marked for export in your .bashrc file in your home directory.
ifeq ("$(PROGRAMFILES)$(ProgramFiles)", "")
    OS := Linux
else
    OS := Windows
endif

SIMVISION_BUILD_DIR := $(BUILD_DIR)/simvision
# List of source files. For the constrained lenght of the Windows command line the list is
# seperated.
C_SOURCE_FILES := $(foreach dir, $(CCOMPILER_DIRS:%/=%), $(wildcard $(dir)/*.c))
H_SOURCE_FILES := $(foreach dir, $(CCOMPILER_DIRS:%/=%), $(wildcard $(dir)/*.h))
OTHER_SOURCE_FILES := $(foreach dir, $(CCOMPILER_DIRS:%/=%),\
                        $(wildcard $(dir)/*.s) $(wildcard $(dir)/*.sct $(dir)/sym_keep))

# This flag is set with the target "simvision" and "clean_simvision" (see below).
# It leads to overwriting the build directory structure.
ifdef SIMVISION
    
    # Overwrite the path information of the target image for having the flat
    # build directory structure.
    # Note: The subsequent assignments or appends to an 'override' variable which are not marked
    # 'override' will be ignored. Thus, MakefileSimvision.mk has to be included before
    # MakefileCommon.mk.
    override IMAGE_NAME = simvision
    override IMAGE_BUILD_DIR := $(SIMVISION_BUILD_DIR)
    override OBJECT_BUILD_DIR := $(SIMVISION_BUILD_DIR)
    
else
    # On the first run substitute the directory slashes for copying the files under Windows.
    ifeq ("$(OS)", "Windows")
        SIMVISION_BUILD_DIR := $(subst /,\,$(SIMVISION_BUILD_DIR))
        C_SOURCE_FILES := $(subst /,\,$(C_SOURCE_FILES))
        H_SOURCE_FILES := $(subst /,\,$(H_SOURCE_FILES))
        OTHER_SOURCE_FILES := $(subst /,\,$(OTHER_SOURCE_FILES))
    endif
endif # SIMVISION

# Build & Cleanup labels ==========================================================================

# Calling make again is necessary for copying the source files and then using them as prerequesites.
# GNUmake cannot reference prerequesites that are created during runtime.
.PHONY: simvision
simvision:
	@$(ECHO) '$(___TARGET_SEPARATOR_LINE___)'
	@$(ECHO) Copy \(or update\) all source files to "$(SIMVISION_BUILD_DIR)"
	@$(MKDIR) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(C_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(H_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(OTHER_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(MAKE) SIMVISION=1 image

.PHONY: clean_simvision
clean_simvision:
	@$(MAKE) SIMVISION=1 clean_image

.PHONY: simvision_patch
simvision_patch:
	@$(ECHO) '$(___TARGET_SEPARATOR_LINE___)'
	@$(ECHO) Copy \(or update\) all source files to "$(SIMVISION_BUILD_DIR)"
	@$(MKDIR) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(C_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(H_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(OTHER_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(MAKE) SIMVISION=1 PATCH=1 PATCH_BUILD_DIR=$(SIMVISION_BUILD_DIR) image

.PHONY: simvision_patch_full
simvision_patch_full:
	@$(ECHO) '$(___TARGET_SEPARATOR_LINE___)'
	@$(ECHO) Copy (or update) all source files to "$(SIMVISION_BUILD_DIR)"
	@$(MKDIR) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(C_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(H_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(COPY) $(OTHER_SOURCE_FILES) $(SIMVISION_BUILD_DIR)
	@$(MAKE) SIMVISION=1 PATCH_FULL=1 PATCH_FULL_BUILD_DIR=$(SIMVISION_BUILD_DIR) image
	