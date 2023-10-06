# =================================================================================================
# File:     MakefileCommon.mk
# Project:  Shasta Firmware
# -------------------------------------------------------------------------------------------------
# Brief:
# This file is the core of the firmware development environment.
# It must be included by a build project specific Makefile which defines the needed source files.
# -------------------------------------------------------------------------------------------------
# Requirements for the Makefile:
#
# 1.) Usage with mulitple build projects:
# It is assumed that throughout the development process there will be more than a single build
# project (e.g. one for functional verification, one for building the image, one for simply testing
# a new feature, etc.).
# This leads to a directory structure with a common pool of source code and multiple build
# projects. The latter will include different parts of the common source and hence one Makefile per
# project which defines the source files is needed. MakefileCommon.mk is then included by these
# project specific Makefiles.
#
# 2.) Reusability:
# The Makefile shall be resuable within other firmware development projects.
#
# 3.) Portability:
# The Makefile shall be portable between Windows (without Cygwin) and Linux.
#
# 4.) Restrictions:
# - Only C, C++ and Assembler sources are used (.c, .cpp, .s).
# - There are no duplicate basenames of sources (e.g. ./src.c, ./src.s, dir/src.c).
# -------------------------------------------------------------------------------------------------
# Content:
# 1.) Information about the environment (operating system, shell, paths)
# 2.) Configuration of the tools (executables, parameters)
# 3.) Targets
# -------------------------------------------------------------------------------------------------
# Conventions for target rules:
#
# 1.) First define the target's variables for creating (lists of) file names.
# - *_FILE variables contain a file's name and its path from the project's root.
# - *_DIR variables contain the path WITHOUT a slash '/' at the end.
#
# 2.) Then start with the label target (.PHONY) at the top, create a dedicated label target
# 'clean_TARGETNAME' and define all sub-targets (for creating the actual files and directories)
# below.
#
# 3.) If a target needs to be extended (e.g. the output of target 'image' is the image but a
# report file shall be generated from the image with the same target) then separate the extension
# from the original target but put it into the same section. Note that the corresponding
# 'clean_TARGETNAME' might have to be extended as well.
# By using this separation one can easily cut out the single extension target without affecting
# the normal build process.
#
# Example:
# image: $(IMAGE_BUILD_DIR) $(IMAGE_FILE)
# $(IMAGE_FILE): prerequesites
#     original image recipe
#
# image: $(EXTENSION_FILE)
# $(EXTENSION_FILE): $(IMAGE_FILE) or other prerequesites
#     extended recipe
#
# 4.) For each set of the above targets create a separate section (analog to a word document).
# -------------------------------------------------------------------------------------------------
# Recommended reading:
# www.conifersystems.com/whitepapers/gnu-make
# www.scons.org
# =================================================================================================


# =================================================================================================
#
#
# 1.) Information about the environment (operating system, shell, paths)
#
#
# =================================================================================================

# Repo root directory, related to the project directory. This variable
# is used as reference when generating the absolute path information
ifeq ($(REPO_ROOT_DIR), ) # if empty
    REPO_ROOT_DIR = ../..
endif

# Check the essential SOURCE_DIRS which contain the files for building anything
ifeq ($(SOURCE_DIRS), ) # if empty
    $(error SOURCE_DIRS is empty)
endif

# Each target creates its build output in an own sub-directory below this directory.
ifeq ($(BUILD_DIR), ) # if empty
    BUILD_DIR = ./build
endif

# Each target creates its build output in an own sub-directory below this directory.
ifeq ($(ROOT_DIR), ) # if empty
    ROOT_DIR = C:
endif
# sofit repodir,
ifeq ($(SOFIT_PATH), ) # if empty
	SOFIT_PATH = $(ROOT_DIR)/sofit/shasta_fw/
endif

# General separator for print-outs
___TARGET_SEPARATOR_LINE___ := --------------------------------------------------------------------------------

# Determine the OS based on standard environment variables.
# Note that if you are running under BASH, make sure that the BASH variable is
# marked for export in your .bashrc file in your home directory.
ifeq ("$(PROGRAMFILES)$(ProgramFiles)", "")
    OS = Linux
else
    OS = Windows
endif

# The objective of the following lines is to make sure that the actual build
# is executed within a suitable, well-defined shell along with a usable, but minimal path.
# Doing this, we avoid any interference of the make process with a shell that is
# inherited from the actual user.
ifeq ("$(OS)", "Linux")

    SHELL := /bin/bash

    # Installation directory of the build tools for the image (see target "image")
   	ARM_DIR     = /opt/gccarm/gcc-arm-none-eabi-5_4-2016q3
    ARM_BIN_DIR = $(ARM_DIR)/bin/

    # Installation directory for the image build (see target "image")
    TOOLCHAIN_DIR := $(ARM_BIN_DIR)

    # Python Interpretor
    PYTHON_DIR :=

    # Doxygen documentation tool (see target "doc")
    # Under Linux, doxygen is loaded into the user's environment, so we assume it is
    # available anywhere and the path to the executable is empty.
    # @todo We need to make sure that 'module load doxygen' has been done before
    # running doxygen. I tried to integrate it into this makefile, but failed and
    # did not follow up.
    DOXYGEN_DIR :=

    # Defines operating system specific helper macros
    MKDIR   := mkdir -p
    RMDIR   := rm -fr
    RMALL   := rm -f
    ECHO    := echo
    COPY    := cp --update # copy when source is newer than destination
    COPY_REGMAP_DOCS := cp -rf /home/irdigcom/projects/regmap_docs/shasta/SHASTA_R0C/regmap_customer/ $(BUILD_DIR)/doc/regmap

    export PATH := $(PATH):$(TOOLCHAIN_DIR):$(PYTON_DIR)

else # Windows

    # Make sure that any commands initiated by make are done within the default
    # windows shell. This makes sure that make does not use e.g. a cygwin shell
    # that it finds in the path by chance.
    SHELL := C:/Windows/system32/cmd.exe


    # Doxygen documentation tool (see target "doc")
    # Under Windows, doxygen comes with a clone of the repo, we don't want to bother
    # the user to have it generally available on his machine. Thus, we provide the
    # path to this 'local' copy of doxygen.
    DOXYGEN_DIR := $(REPO_ROOT_DIR)/tools/Doxygen

    # SOFIT tool repository directory. As temporary solution, the SOFIT tools are 
    # installed in the "C:\soft\<sofit-project-name>" directory. 
    # The long term solution will combine both repos (source code repo + SOFIT repo) 
    # where bold repositories are cloned together and the their versions 
    # are synchronized during checkout.
    # 
    # For Jenkins build tasks, this variable will be pre-defined (overwritten) 
    # to the build directory where sofit clone is placed.
	SOFIT_DIR =  $(ROOT_DIR)/sofit/shasta_fw

    # Installation directory of the build tools for the image (see target "image")
   	#ARM_DIR     ?= $(ROOT_DIR)/sofit/shasta_fw/armgnu
   	ARM_DIR     ?= $(REPO_ROOT_DIR)/tools/armgnu
   	ARM_BIN_DIR     = $(ARM_DIR)/bin

    # Installation directory for the image build (see target "image")
    TOOLCHAIN_DIR := $(ARM_BIN_DIR)

    # Unit test framework tool directories
    UNITTEST_DIR := $(SOFIT_DIR)/ruby/bin;$(SOFIT_DIR)/gcc/bin
	GCOVR := $(SOFIT_DIR)/gcovr/scripts/gcovr

    # Python Interpretor
    PYTHON_DIR := $(REPO_ROOT_DIR)/tools/python/Portable_Python_2.7.6.1
    #export PYTHONPATH := $(REPO_ROOT_DIR)/tools/python/intelhex-1.5;$(REPO_ROOT_DIR)/tools/python

    # Directory of additional GNUmake scripts under windows (batch files like
    # windows_gmake_mkdir.bat, usually in the same directory as GNUmake).

    # Defines operating system specific helper macros
    WIN_MAKE_DIR := $(REPO_ROOT_DIR)/tools/GNUmake
    CMD_DIR := C:/Windows/system32/
    MKDIR   := cmd /c $(subst /,\,$(WIN_MAKE_DIR))\windows_gmake_mkdir.bat  # mkdir
    RMDIR   := cmd /c $(subst /,\,$(WIN_MAKE_DIR))\windows_gmake_rmdir.bat  # rmdir /s /q
    RMALL   := cmd /c $(subst /,\,$(WIN_MAKE_DIR))\windows_gmake_rm.bat     # del /F /Q
    COPY    := cmd /c $(subst /,\,$(WIN_MAKE_DIR))\windows_gmake_cp.bat
    ECHO    := cmd /c echo

    export PATH := $(PATH);$(TOOLCHAIN_DIR);$(UNITTEST_DIR);$(PYTHON_DIR);$(CMD_DIR)

endif


# =================================================================================================
#
#
# 2.) Configuration of the tools (executables, parameters)
#
#
# =================================================================================================

ARM_GCC_PREFIX := arm-none-eabi-
ARM_GCC_VERSION_PARAM := --version
CPU_INTERN = $(CPU)
IMAGE_EXTENSION := elf

CCOMPILER    := $(ARM_GCC_PREFIX)gcc
ASSEMBLER    := $(ARM_GCC_PREFIX)gcc
LINKER       := $(ARM_GCC_PREFIX)gcc
HEXFILEGEN   := $(ARM_GCC_PREFIX)objcopy
LISTING_TOOL := $(ARM_GCC_PREFIX)objdump
SIZE_TOOL    := $(ARM_GCC_PREFIX)size
NM_TOOL      := $(ARM_GCC_PREFIX)nm

CCOMPILER_VERSION_CMD = $(CCOMPILER) $(ARM_GCC_VERSION_PARAM)
CCOMPILER_HELP_CMD    = $(CCOMPILER) --help

#DEBUG = 1
ADDCLFAGS = -DROM_IMAGE
CCOMPILER_PARAMS += -DUC_ID=1101007 \
					-ffunction-sections \
					-fdata-sections \
					-std=gnu99 \
					-mfloat-abi=soft \
					-c \
					-fmessage-length=0 \
					-MD \
					-MP \
					-mcpu=$(CPU_INTERN) \
					-mthumb \
					-g3 \
					-gdwarf-2 \
					-fomit-frame-pointer \
					-I. $(addprefix -I, $(SOURCE_DIRS)) 

ifdef DEBUG
    CCOMPILER_PARAMS += -O1
else
    CCOMPILER_PARAMS += -O1
endif

ASSEMBLER_PARAMS += -x assembler-with-cpp $(CCOMPILER_PARAMS)

LINKER_FILES       += $(LINKER_CONFIG_FILE)

LINKER_PARAMS +=  -T"$(LINKER_CONFIG_FILE)" \
				  -nostartfiles \
				  $(addprefix -L,$(LIB_DIRS)) \
				  $(addprefix -l ,$(LIBS)) \
				  -Wl,-Map,"$(IMAGE_BUILD_DIR)/$(IMAGE_NAME).map"  \
				  -Wl,--gc-sections \
				  -mcpu=$(CPU_INTERN) \
				  -mthumb \
				  -g3 \
				  -gdwarf-2
				  
LINKER_CMD       = $(LINKER) $(LINKER_PARAMS)  $(LINKER_FILES)  -o $@
				                 
HEXFILE_PREFIX  = 
HEXEFILE_PARAMS = -O ihex
HEXFILEGEN_CMD  = $(HEXFILEGEN) -O ihex $< $@
SREFILEGEN_CMD  = $(HEXFILEGEN) -O srec --srec-forceS3 $< $@
BINFILEGEN_CMD  = $(HEXFILEGEN) -O binary $< $@
SYMFILEGEN_CMD   = $(NM_TOOL) -s $< > $@

LISTING_PARAMS   = -h -S 
LISTING_TOOL_CMD =  $(LISTING_TOOL) $(LISTING_PARAMS) $< > $@

SIZE_TOOL_CMD    = $(SIZE_TOOL) --format=berkeley $(IMAGE_FILE)

ROMFILEGEN_CMD   = $(ECHO) Skipped

###################################################################################################
# Define the linker
###################################################################################################

###################################################################################################
# Define the document generator
###################################################################################################

# Search for a single DoxyFile in all source directories
DOCGENERATOR_CONFIG_FILE := $(strip $(foreach dir,$(SOURCE_DIRS:%/=%),$(wildcard $(dir)/DoxyFile)))
ifeq ($(words $(DOCGENERATOR_CONFIG_FILE)), 0)
    # Generates an dependencies error in the target if the file is not here
    DOCGENERATOR_CONFIG_FILE := ./DoxyFile
else
    ifneq ($(words $(DOCGENERATOR_CONFIG_FILE)), 1)
        $(error Too many DoxyFiles found ($(DOCGENERATOR_CONFIG_FILE)).)
    endif
endif

# Optional layout file
DOCGENERATOR_LAYOUT_FILE := $(strip $(foreach dir,$(SOURCE_DIRS:%/=%),$(wildcard $(dir)/DoxygenLayout.xml)))
ifneq ($(words $(DOCGENERATOR_CONFIG_FILE)), 1)
    $(error Too many layout files found ($(DOCGENERATOR_CONFIG_FILE)).)
endif

# Add as dependency for the document.
DOCGENERATOR_FILES += $(DOCGENERATOR_CONFIG_FILE) $(DOCGENERATOR_LAYOUT_FILE)

ifeq ("$(OS)", "Windows")
    # The exotic command below essentially pipes the DoxyFile to the cmdline for configuring
    # doxygen (see doxygen FAQ). Furthermore, "-" tells doxygen to read its configuration from the
    # cmdline (instead of a config file). This way selected parts of the configuration can be
    # overwritten.
    # Note: Instead of the prerequisite DOCGENERATOR_FILES, DOCGENERATOR_DIRS is used for INPUT
    # since otherwise the cmdline for doxygen quickly gets too long.
    DOCGENERATOR = \
    ( type $(subst /,\,$(DOCGENERATOR_CONFIG_FILE)) \
        & echo INPUT=$(DOCGENERATOR_DIRS) \
        & echo IMAGE_PATH=$(DOCGENERATOR_DIRS) \
        & echo OUTPUT_DIRECTORY=$(DOC_BUILD_DIR) \
        & echo WARN_LOGFILE=$(DOC_BUILD_DIR)/warnings.log \
        & echo LAYOUT_FILE=$(subst /,\,$(DOCGENERATOR_LAYOUT_FILE)) \
    ) | cmd /c $(subst /,\,$(DOXYGEN_DIR))\doxygen -
else
    # As described in the Windows version above, we need to overrule a number of parameters for
    # doxygen: We cat the complete DoxyFile into the build directory and append the settings that
    # depend on the actual build. Moreover, this scheme gives us the opportunity to inspect the
    # final DoxyFile - above solution does not keep the final DoxyFile.
    DOCGENERATOR = \
    cat $(DOCGENERATOR_CONFIG_FILE) > $(DOC_BUILD_DIR)/DoxyFile &&\
    echo "INPUT=$(DOCGENERATOR_DIRS)" >> $(DOC_BUILD_DIR)/DoxyFile &&\
    echo "IMAGE_PATH=$(DOCGENERATOR_DIRS)" >> $(DOC_BUILD_DIR)/DoxyFile &&\
    echo "OUTPUT_DIRECTORY=$(DOC_BUILD_DIR)" >> $(DOC_BUILD_DIR)/DoxyFile &&\
    echo "WARN_LOGFILE=$(DOC_BUILD_DIR)/warnings.log" >> $(DOC_BUILD_DIR)/DoxyFile &&\
    echo "PROJECT_NUMBER=\"Build project: $(PROJECT_NAME)\"" >> $(DOC_BUILD_DIR)/DoxyFile &&\
    echo "LAYOUT_FILE=$(DOCGENERATOR_LAYOUT_FILE)" >> $(DOC_BUILD_DIR)/DoxyFile &&\
    doxygen $(DOC_BUILD_DIR)/DoxyFile
endif


###################################################################################################
# Define the tool for static code analysis
###################################################################################################
UNITTESTER := rake
UNITTESTER_TARGET := delta

UNITTESTCOV_PARAMS :=gcov:$(UNITTESTER_TARGET)  #Run unit testing for changed files (update)
UNITTESTER_PARAMS  := test:$(UNITTESTER_TARGET)
UNITTESTER_HTML    := utils:gcov


# =================================================================================================
#
#
# 3.) Targets
#
#
# =================================================================================================

# GNUmake syntax
#
# .PHONY:
# Targets are declared as phony if they are used only as jump label.
# GNUmake will not search for a file with the phony target's name since it is
# not used to create a file.
# For example a file in the current directory is named 'help'. If a rule for a
# target 'help' (lets say without any prerequisites) exists, it will not be
# executed since the file 'help' already exists (it is up to date without
# prerequisites). Since the target shall display makefile information it
# actually has nothing to do with the file. Declaring the target as phony
# solves this issue.
#
# Automatic variables:
# $@  name of the target
# $<  name of the first prerequesite
# %   wildcard for any string


###################################################################################################
# Display the help menue
###################################################################################################

# If this is the first target (not overruled in the Makefile) and it will be called when no
# argument is given to GNUmake.
.PHONY: help
help:
	@$(ECHO) 'make all              Call the targets image and doc'
	@$(ECHO) 'make clean            Remove $(BUILD_DIR)'
	@$(ECHO) 'make image            Create the firmware image in $(IMAGE_BUILD_DIR)'
	@$(ECHO) 'make clean_image      Remove $(IMAGE_BUILD_DIR)'
	@$(ECHO) 'make doc              Create the documentation in $(DOC_BUILD_DIR)'
	@$(ECHO) 'make clean_doc        Remove $(DOC_BUILD_DIR)'


###################################################################################################
# Generation of firmware images (ROM or RAM)
###################################################################################################

# Variables =======================================================================================

# Set the directory variables to default if they were not specified
ifeq ($(CCOMPILER_DIRS), )
    CCOMPILER_DIRS := $(SOURCE_DIRS)
endif

# Setting of names
IMAGE_NAME := image
IMAGE_BUILD_DIR = $(BUILD_DIR)/$(IMAGE_NAME)
IMAGE_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).elf
OBJECT_BUILD_DIR := $(IMAGE_BUILD_DIR)/objects

# Collect the input files for the compiler, assembler and code analyzer from the source directories.
CCOMPILER_FILES := $(foreach dir, $(CCOMPILER_DIRS:%/=%), $(wildcard  $(dir)/*.c  $(dir)/*.cpp) )
ASSEMBLER_FILES := $(foreach dir, $(CCOMPILER_DIRS:%/=%), $(wildcard  $(dir)/*.s) )

# Create the filenames including the path for the .o files out of the source files' names.
LINKER_FILES = $(foreach file, $(notdir $(CCOMPILER_FILES)) $(notdir $(ASSEMBLER_FILES)), \
                  $(OBJECT_BUILD_DIR)/$(basename $(file)).o)

# Create the filenames for the dependency files out of the .o files' names.
# The dependency files contain the .h files as prerequesites for the .o files
# The according rules look like "$(OBJECT_BUILD_DIR)/%.o: path/header.h".
# "-include" is used to make sure that GNUmake does not complain if a *.d file does not (yet) exist.
-include $(filter %.d, $(patsubst %.o, %.d, $(LINKER_FILES)))

# Use VPATH to locate the source and header files for the wildcard prerequesites (e.g. %.c).
VPATH := $(SOURCE_DIRS)


# Build & Cleanup labels ==========================================================================

.PHONY: image
image: $(IMAGE_BUILD_DIR) $(IMAGE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Finished building "$(IMAGE_FILE)".

.PHONY: clean_image
clean_image:
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Removing "$(IMAGE_BUILD_DIR)".
	@$(RMDIR) $(IMAGE_BUILD_DIR)


# Targets =========================================================================================

# Create the build output directories.
# Note: Do not add them as prerequesites for the object files or image file since if you do so
# these files will be rebuild as soon as the directories are marked as changed. And this will
# happen as soon as one of the files within the directories is rebuilt.
$(IMAGE_BUILD_DIR):
	@$(MKDIR) $@
	@$(MKDIR) $(OBJECT_BUILD_DIR)

# Link the object files together and generate the executable file.
# Afterwards perform a static C code analysis.
# Note: No duplicate basenames are allowed. If desired the prerequesites of
# the source files have to be separated for their respective targets.
$(IMAGE_FILE): $(LINKER_FILES) $(LINKER_CONFIG_FILE)
	@$(ECHO) Linking object files and building "$@"
	$(LINKER) $(LINKER_PARAMS)  $(LINKER_FILES)  -o $@

# Compile every single source file into an object file with the same basename.
# Note: Keil ARM compiler requires a valid Infineon site specific license file '$(KEIL_LICENSE_FILE)'.
# The compiles output file is pre-linked to mangle the global symbols by a Keil 
# specific steering configuration. This is needed for patching to hide compiled symbols and 
# use the ROM reference instead.
$(OBJECT_BUILD_DIR)/%.o: %.c $(KEIL_LICENSE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Compiling "$<"
	$(CCOMPILER) $(CCOMPILER_PARAMS)  $<  -o $@

$(OBJECT_BUILD_DIR)/%.o: %.cpp $(KEIL_LICENSE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Compiling "$<"
	$(CCOMPILER) $(CCOMPILER_PARAMS)  $<  -o $@

$(OBJECT_BUILD_DIR)/%.o: %.s $(KEIL_LICENSE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Assembling "$<"
	$(ASSEMBLER) $(ASSEMBLER_PARAMS)  $<  -o $@

# Extension targets ===============================================================================
#IMAGE_HEXCRC32_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).hex
IMAGE_HEXFILE  := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).hex
IMAGE_LISTING  := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).lst
#IMAGE_BINFILE  := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).bin
IMAGE_SREFILE  := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).sre
IMAGE_SYMFILE  := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).sym
IMAGE_MAP_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).map
IMAGE_SIZECODE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME)


image: $(IMAGE_HEXFILE)
$(IMAGE_HEXFILE): $(IMAGE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Creating an intel hex file $(IMAGE_HEXFILE) from "$<"
	@$(HEXFILEGEN_CMD)
    
image: $(IMAGE_SYMFILE)
$(IMAGE_SYMFILE): $(IMAGE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Creating a symbol file $(IMAGE_SYMFILE) from "$<"
	@$(SYMFILEGEN_CMD)    

image: $(IMAGE_LISTING)
$(IMAGE_LISTING): $(IMAGE_FILE) 
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating listing file $(IMAGE_LISTING) from "$(IMAGE_FILE)"
	@$(LISTING_TOOL_CMD)
	
image: $(IMAGE_SIZECODE)
$(IMAGE_SIZECODE): $(IMAGE_FILE) 
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Calculating size from "$(IMAGE_FILE)"
	@$(SIZE_TOOL_CMD)

###################################################################################################
# Generate the documentation
###################################################################################################

# Variables =======================================================================================

# Set the directory variable to default if it was not specified
ifeq ($(DOCGENERATOR_DIRS), )
    DOCGENERATOR_DIRS := $(SOURCE_DIRS)
endif

# Output directory for the documentation
DOC_BUILD_DIR := $(BUILD_DIR)/doc
DOC_FILE := $(DOC_BUILD_DIR)/html/index.html

# Obtain a list of all files from those directories that hold source files and
# use this list as a dependency for the document build.
DOCGENERATOR_FILES += $(foreach dir, $(DOCGENERATOR_DIRS:%/=%), $(wildcard $(dir)/*.*) )

# Build & Cleanup labels ==========================================================================

.PHONY: doc
doc: $(DOC_BUILD_DIR) $(DOC_FILE)

.PHONY: clean_doc
clean_doc:
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Removing "$(DOC_BUILD_DIR)".
	@$(RMDIR) $(DOC_BUILD_DIR)

# Targets =========================================================================================

$(DOC_BUILD_DIR):
	@$(MKDIR) $@

$(DOC_FILE): $(DOCGENERATOR_FILES)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating the documentation
	@$(DOCGENERATOR)
	@$(COPY_REGMAP_DOCS)
	@$(ECHO) Check the log "$(DOC_BUILD_DIR)/warnings.log"


###################################################################################################
# Firmware unit testing
###################################################################################################
# Variables =======================================================================================

UNITTEST_BUILD_DIR := $(BUILD_DIR)/unittest
CEEDLING_GCOV_OUT := $(BUILD_DIR)/unittest/gcov/out
GCOV_BUILD_DIR := $(UNITTEST_BUILD_DIR)/coverage
ROOT_DIR := $(realpath ../../)
PYTHON_TOOLS := /tools/python/Portable_Python_2.7.6.1/
# Output directory for the unit test results

# Build & Cleanup labels ==========================================================================
.PHONY: unittest

unittest: $(UNITTEST_BUILD_DIR)
	@$(UNITTESTER) $(UNITTESTER_PARAMS)
ceedling_buildcov: $(GCOV_BUILD_DIR)
	@$(UNITTESTER) $(UNITTESTER_PARAMS) $(UNITTESTCOV_PARAMS)
ceedling_cov_html: ceedling_buildcov
	cd $(CEEDLING_GCOV_OUT) & \
	$(ROOT_DIR)$(PYTHON_TOOLS)python $(GCOVR) -b -r ../../../../../../ -p --html --html-details -e "^modules/system.*" -e "^projects.*" -o "../../coverage/-coverage.html"
ceedling_coverage: ceedling_cov_html

# Targets =========================================================================================
$(UNITTEST_BUILD_DIR):
	@$(MKDIR) $@
$(GCOV_BUILD_DIR):
	@$(MKDIR) $@


###################################################################################################
# Define the meaning of "build everything"
###################################################################################################

.PHONY: all
all: patch

.PHONY: clean
clean:
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Removing "$(BUILD_DIR)".
	@$(RMDIR) $(BUILD_DIR)
