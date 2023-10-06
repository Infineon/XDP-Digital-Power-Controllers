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

    # Installation directory for the image build (see target "image")
    TOOLCHAIN_DIR := /opt/armrvds/4.1b631/standard-linux-pentium-rel/linux-pentium/

    # Python Interpretor
    PYTHON_DIR :=

    UNITTEST_DIR := /home/herbi/.rbenv/shims
	GCOVR := gcovr
    
    
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

    # export PATH := $(PATH):$(TOOLCHAIN_DIR):$(PYTON_DIR)
    export PATH := $(PATH):$(TOOLCHAIN_DIR):$(UNITTEST_DIR)

else # Windows

    # Make sure that any commands initiated by make are done within the default
    # windows shell. This makes sure that make does not use e.g. a cygwin shell
    # that it finds in the path by chance.
    SHELL := C:/Windows/system32/cmd.exe

    # Installation directory of the build tools for the image (see target "image")
    TOOLCHAIN_DIR := $(REPO_ROOT_DIR)/tools/arm
    export ARMLIB := $(TOOLCHAIN_DIR)

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
	SOFIT_DIR =  C:/sofit/shasta_fw

    # Unit test framework tool directories
    UNITTEST_DIR := $(SOFIT_DIR)/ruby/bin;$(SOFIT_DIR)/gcc/bin
	GCOVR_DIR := $(SOFIT_DIR)/gcovr/scripts
	GCOVR := python $(GCOVR_DIR)/gcovr
	
	# MIKTEX tools
	# Miktex is used to create pdf from latex
	MIKTEX_DIR = C:/Progloc/MiKTeX 2.9/miktex/bin/x64
	
	GRAPHVIZ_DIR = C:\Program Files\Graphviz\bin

    # Python Interpretor
    PYTHON_DIR := $(REPO_ROOT_DIR)/tools/python/Portable_Python_2.7.6.1
    export PYTHONPATH := $(REPO_ROOT_DIR)/tools/python/intelhex-1.5;$(REPO_ROOT_DIR)/tools/python

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
    PDFMAKE := cmd /c make_short.bat 
    COPY_REGMAP_DOCS := 

    export PATH := $(TOOLCHAIN_DIR);$(UNITTEST_DIR);$(PYTHON_DIR);$(MIKTEX_DIR);$(CMD_DIR)

endif


# =================================================================================================
#
#
# 2.) Configuration of the tools (executables, parameters)
#
#
# =================================================================================================

###################################################################################################
# Define the C/C++ compiler
###################################################################################################
CCOMPILER := armcc
# cmd line options
# --cpu                           Specify Cortex-M0 as target core
# --thumb                         Select thumb mode for functions
# --c99                           Enables C99 source code and enforces C only
# --depend                        Create .d file in the same dir as the .o file
# --asm --interleave              List file: Assembler with C/C++ source as comments
# --split_sections                Place each function into a separate sector, which allows the
#                                 linker to remove code on function level (linker section level).
# --list                          Generates raw listing information for a source file.
#                                 The name of the raw listing file defaults to the name of the
#                                 input file with the filename extension .lst.
# --list_dir=directory_name       Specifies a directory for --list output.
# -c                              Suppress the linking function of the compiler
# --debug                         Generates debug tables
# -Otime                          Optimize for maximum performance
# -O1                             Optimization step
# -J                              Empty path for system headers prevents the compilers to search
#                                 its internal memory for <include>
# -I                              Directory for include files
#
# Note: Make sure that COMPILER_PARAMS is only expanded when used ("=") and not when
# defined (":=") so that $@ will be evaluated correctly.
# FIXME: can change to O2 for smaller code size, leaving at O1 for debug on A1 silicon
CCOMPILER_PARAMS += --cpu=Cortex-M0 --thumb --c99 --depend=$(basename $@).d --asm --interleave \
                    --list --list_dir=$(OBJECT_BUILD_DIR) \
                    --split_sections -c --debug -O1 -J. $(addprefix -I, $(SOURCE_DIRS))

###################################################################################################
# Define the assembler
###################################################################################################
ASSEMBLER := armasm
# cmd line options
# --cpu                           Specify Cortex-M0 as target core
# --cpreproc                      This causes armasm to call armcc to preprocess the file before assembling it
# --depend                        Create .d file in the same dir as the .o file
# -I                              Directory for include files
ASSEMBLER_PARAMS += --cpu=Cortex-M0 --cpreproc --depend=$(basename $@).d \
                    $(addprefix -I, $(SOURCE_DIRS))

###################################################################################################
# Define the linker
###################################################################################################

# Search for a single linker configuration file in all source directories
ifdef SIMVISION
    
    LINKER_CONFIG_FILE := $(IMAGE_BUILD_DIR)/linker_config.sct
    
else
	LINKER_CONFIG_FILE := $(strip $(foreach dir,$(SOURCE_DIRS:%/=%),$(wildcard $(dir)/*.sct)))
	ifeq ($(words $(LINKER_CONFIG_FILE)), 0)
	    # Generates an dependencies error in the target if the file is not here
	    LINKER_CONFIG_FILE := ./linker_config.sct
	else
	    ifneq ($(words $(LINKER_CONFIG_FILE)), 1)
	        $(error Too many linker configuration files found ($(LINKER_CONFIG_FILE)).)
	    endif
	endif
endif
# Add as dependency for the image.
LINKER_FILES += $(LINKER_CONFIG_FILE)

LINKER := armlink
# cmd line options
# --cpu         Specify Cortex-M0 as target core
# --datacompressor Disables datacompression of RW sections since ARM library
#               startup including decompression is excluded.
# --list        File for the following diagnostics outputs
# --info,map,.. Options for the list file
# --symdefs     Generate a symbol text file which can be used as input file
#               for future link processes to link against this image
#               Note that the symbol file is only fresh regenerated in case
#               the old version got previously deleted.
# --no_startup  Do not auto-include __main and other symbols from the ARM C startup library code
# --scatter     Mapping of linker sections to memory addresses
# Note: Make sure that LINKER_PARAMS is only expanded when used ("=") and not when
# defined (":=") so that $@ will be evaluated.
LINKER_PARAMS += --cpu=Cortex-M0  --datacompressor=off  --list=$(IMAGE_BUILD_DIR)/linker.log \
                 --info=architecture,sizes,stack,totals,unused,inline --symbols --map \
                 --symdefs=$(basename $@).sym \
                 --no_startup \
                 --scatter=$(LINKER_CONFIG_FILE)

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

UNITTESTCOV_PARAMS := gcov:$(UNITTESTER_TARGET)  #Run unit testing for changed files (update)
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


# Target Tool License =============================================================================

# The target Keil ARM compiler requires a floating license file which is Infineon site specific
# A Python script uses the WINDOWS board tools to retrieve the local site and it generates the Keil
# license file inside the ARM tool folder.
KEIL_LICENSE_FILE := $(REPO_ROOT_DIR)/tools/arm/TOOLS.INI
DEFAULT_KEIL_LICENSE_FILE := $(REPO_ROOT_DIR)/tools/arm/Default_TOOLS.INI

$(KEIL_LICENSE_FILE):
ifeq ("$(OS)", "Linux")
	@$(ECHO) Take Keil default license server file for Linux
	$(COPY) $(DEFAULT_KEIL_LICENSE_FILE) $@
else # Windows
	@$(ECHO) Detecting Keil license server
	python $(REPO_ROOT_DIR)/tools/scripts/keil_license_detect.py -o $@
endif

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

# Get a textual representation of the executable image.
# cmd line options
# --cpu         Specify Cortex-M0 as target core
# --text        Decodes an ELF image or ELF object file and enables the following flags
# -c            This option disassembles code, alongside a dump of the original
#               binary data being disassembled and the addresses of the instructions.
# -d            Prints contents of the data sections.
# -e            Decodes exception table information for objects. Use with -c
#               when disassembling images.
# -g            Prints debug information.
# -r            Prints relocation information.
# -s            Prints the symbol and versioning tables.
# -t            Prints the string tables.
# -v            Prints detailed information on each segment and section header of the image.
# -w            Eliminates line wrapping.
# -y            Prints dynamic segment contents.
# -z            Prints the code and data sizes.
IMAGE2REPORT_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).txt
image: $(IMAGE2REPORT_FILE)
$(IMAGE2REPORT_FILE): $(IMAGE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Creating a textual representation of "$<"
	fromelf --cpu=Cortex-M0  --text -c -r -v -z  -o $@  $<

# Create an intel hex file from the executable image.
# cmd line options
# --i32combined Produces Intel Hex-32 format output. It generates one output file for an image
#               containing multiple load regions.
IMAGE2HEX_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME)_no_crc.hex
image: $(IMAGE2HEX_FILE)
$(IMAGE2HEX_FILE): $(IMAGE_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Creating an intel hex file from "$<"
	fromelf --i32combined  -o $@  $<

# Dummy action to create dependency to linker log
LINKER_LOG_FILE := $(IMAGE_BUILD_DIR)/linker.log
image: $(LINKER_LOG_FILE)
$(LINKER_LOG_FILE): $(IMAGE_FILE)

# Append the static symbols to the symbol table
IMAGE2HEX_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME)_no_crc.hex
SYM_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).sym
image: $(SYM_FILE)
$(SYM_FILE): $(LINKER_LOG_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Appending static symbols to "$@"
	python $(REPO_ROOT_DIR)/tools/scripts/sym_add_static_symbols.py  -o $@ -i $<

# Create an intel hex file from the executable image which contains a CRC-32 sum at ROM top address.
# cmd line options
# --crc=<address>         CRC-32 address where to store the calculated value
# --start=<address>       CRC-32 calculation start address where to start the CRC-32 calculation.
# --end=<address>         CRC-32 calculation end address where to stop the CRC-32 calculation
#                         (including this address).
# -v                      Verbose. Perform detailed debug printouts.
HEXCRC32_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NAME).hex
image: $(HEXCRC32_FILE)
$(HEXCRC32_FILE): $(IMAGE2HEX_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Creating an intel hex file with a CRC-32 sum
# @todo: Replace the hard coded addresses in the following Python script call by makefile variables or other generic mechanism
	python $(REPO_ROOT_DIR)/tools/scripts/inthex_crc_gen.py --crc=0x10013ffc --start=0x10000000 --end=0x10013ffc  -i $<  -o $@
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Starting static C code analysis
	$(CCODEANALYZER) $(CCODEANALYZER_PARAMS)  $(CCODEANALYZER_FILES)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)



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

.PHONY: doc
doc: $(DOC_BUILD_DIR) $(DOC_FILE)

###################################################################################################
# Generate pdf documentation
###################################################################################################
# Output directory for pdf documentation
PDF_BUILD_DIR := .\build\doc\latex
TEX_ORG_FILE := .\templates\tex\refman_short.tex
MAKE_SHORT_FILE := .\templates\tex\make_short.bat

# Build pdf document
.PHONY: pdf

pdf:  $(DOC_BUILD_DIR) $(DOC_FILE)
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Building "$(PDF_BUILD_DIR)\refman_short.pdf"
	@$(COPY) $(TEX_ORG_FILE) $(PDF_BUILD_DIR)
	@$(COPY) $(MAKE_SHORT_FILE) $(PDF_BUILD_DIR)
	@cd $(PDF_BUILD_DIR) && $(PDFMAKE)

.PHONY: clean_doc
clean_doc:
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Removing "$(DOC_BUILD_DIR)"
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
# Output directory for the unit test results

# Build & Cleanup labels ==========================================================================
.PHONY: unittest

unittest: $(UNITTEST_BUILD_DIR)
	@$(UNITTESTER) $(UNITTESTER_PARAMS)
ceedling_buildcov: $(GCOV_BUILD_DIR)
	@$(UNITTESTER) $(UNITTESTER_PARAMS) $(UNITTESTCOV_PARAMS)
ceedling_cov_html: ceedling_buildcov
	$(GCOVR) -b -r . -p --html --html-details -o $(GCOV_BUILD_DIR)/coverage.html
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
