# =================================================================================================
# File:     Makefile
# Project:  Shasta Firmware
# -------------------------------------------------------------------------------------------------
# Brief:    The purpose of this Makefile is to define the build for project specific directories.
# =================================================================================================

# Repo root directory, related to the project directory. This variable
# is used as reference when generating the absolute path information 
REPO_ROOT_DIR := ../..

# Overwrite help as the first target so that this one is called when no argument is given to GNUmake.
default: simvision

# Toolchain defines the tools used for compiling. 
# Toolchain can be ARM_GCC, the GNU Compiler or ARMV5, the Keil-ARM compiler
TOOLCHAIN          ?= ARM_GCC
# Defines the target hardware architecture
CPU                = Cortex-M0
PATCH=1
# Specify ALL source directories of this build project including those which contain tool
# configuration files. These paths will be included by the tools of all targets.
# Note: SOURCE_DIRS is essential to be set.
SOURCE_DIRS := \
    ./src/  ../version/  \
    ./src/user_app/ \
    $(filter %/, $(wildcard $(REPO_ROOT_DIR)/common/modules/*/)) \
    $(REPO_ROOT_DIR)/common/system_header/ \
    $(REPO_ROOT_DIR)/common/system_header/cmsis/ \
    $(REPO_ROOT_DIR)/common/system_header/custom/ \
    $(REPO_ROOT_DIR)/common/utilities/


ifeq ($(TOOLCHAIN), ARM_GCC)
	# GCC Compiler
	LINKER_CONFIG_FILE = ./src/linker_config.ld
	# In case an entry point is needed or a linker symbol need to be kept (e.g. a debugger)
	LINKER_PARAMS =
	SOURCE_DIRS += \
		$(REPO_ROOT_DIR)/common/system_header/gnu/ \
		$(REPO_ROOT_DIR)/common/system_header/gnu/sys/ \
		$(REPO_ROOT_DIR)/common/modules/rtx/GCC/ \
		$(REPO_ROOT_DIR)/common/modules/startup/GCC/ 
else
	# ARM Compiler
	LINKER_CONFIG_FILE = ./src/linker_config.sct
	# In case an entry point is needed or a linker symbol need to be kept (e.g. a debugger)
	SOURCE_DIRS += \
		$(REPO_ROOT_DIR)/common/system_header/c_keil/ \
		$(REPO_ROOT_DIR)/common/modules/rtx/ARM/ \
		$(REPO_ROOT_DIR)/common/modules/startup/ARM/
endif

# Specify the directories which are excluded from the build process.
# Exclude those directories from SOURCE_DIRS that contain implementation files
# (e.g. *.s, *.c) which shall not be compiled.
# Note: If CCOMPILER_DIRS_FILTER is empty it will be set to SOURCE_DIRS
CCOMPILER_DIRS_FILTER := \
    ../version/ \
    $(filter %/, $(wildcard $(REPO_ROOT_DIR)/common/modules/*/)) \
    $(REPO_ROOT_DIR)/common/utilities/ \
	$(REPO_ROOT_DIR)/common/system_header/gnu/sys/ \
	$(REPO_ROOT_DIR)/common/system_header/c_keil/ \
	$(REPO_ROOT_DIR)/common/modules/configurator/ \
    $(REPO_ROOT_DIR)/common/modules/driver_common/ \
    $(REPO_ROOT_DIR)/common/modules/job_agency/ \
    $(REPO_ROOT_DIR)/common/modules/logger/ \
    $(REPO_ROOT_DIR)/common/modules/oam/ \
    $(REPO_ROOT_DIR)/common/modules/otp/ \
    $(REPO_ROOT_DIR)/common/modules/patcher/ \
    $(REPO_ROOT_DIR)/common/modules/pmbus/ \
    $(REPO_ROOT_DIR)/common/modules/regulation/ \
    $(REPO_ROOT_DIR)/common/modules/rtx/ \
    $(REPO_ROOT_DIR)/common/modules/startup/ \
    $(REPO_ROOT_DIR)/common/modules/user_app/

CCOMPILER_DIRS := $(filter-out $(CCOMPILER_DIRS_FILTER), $(SOURCE_DIRS))

# Specifiy the directories which are excluded from C code analysis.
CCODEANALYZER_DIRS_FILTER := \
    $(filter %/, $(wildcard $(REPO_ROOT_DIR)/common/modules/*/)) \
    $(REPO_ROOT_DIR)/common/utilities/

#    $(REPO_ROOT_DIR)/common/modules/rtx/ \
#    $(REPO_ROOT_DIR)/common/modules/driver_common/ \
#    $(REPO_ROOT_DIR)/common/modules/pmbus/ \
#    $(REPO_ROOT_DIR)/common/modules/telemetry/ \
#    $(REPO_ROOT_DIR)/common/modules/regulation/ \
#    $(REPO_ROOT_DIR)/common/modules/vdac/ \
#    $(REPO_ROOT_DIR)/common/modules/gpio/ \
#    $(REPO_ROOT_DIR)/common/modules/user_app/ \
#    $(REPO_ROOT_DIR)/common/modules/faults/

# Specifiy the directories which are included in C Code analysis.
CCODEANALYZER_DIRS := $(filter-out $(CCODEANALYZER_DIRS_FILTER), $(CCOMPILER_DIRS))

# Reference Image for the patch targets. Not relevant for the image target.
#PATCH_REF_IMAGE_NAMES := $(REPO_ROOT_DIR)/versions/Shasta.A11.ROM.v4_20180124_124011_rom_image
#PATCH_REF_IMAGE_NAMES := $(REPO_ROOT_DIR)/projects/rom_image/build/image/image
#PATCH_REF_IMAGE_NAMES := $(REPO_ROOT_DIR)/projects/rom_image/build/simvision/simvision
PATCH_REF_IMAGE_NAMES := $(REPO_ROOT_DIR)/versions/Shasta.A23.ROM.v12_20191118_093946_rom_image

# Reference Image for the patch targets. Not relevant for the image target.
PATCH_LINK_RANGE_CONFIG_FILE := ./src/partial_patch.cfg

# In case an entry point is needed by e.g. a debugger
LINKER_PARAMS += --entry=patch_entry

include $(REPO_ROOT_DIR)/common/MakefilePatching.mk

ifeq ($(TOOLCHAIN), ARM_GCC)
# Plugin for the targets "patch".
	include $(REPO_ROOT_DIR)/common/MakefileCommonGCC.mk
endif
# Plugin for pmbus auto generation of c code
include $(REPO_ROOT_DIR)/common/MakefilePmbus.mk
