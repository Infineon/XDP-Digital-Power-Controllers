# =================================================================================================
# File:     MakefilePmbusDefault.mk
# Project:  Shasta Firmware
# -------------------------------------------------------------------------------------------------
# Brief:    Makefile plugin for pmbus code generation
# =================================================================================================

###################################################################################################
# Extend the help menue
###################################################################################################

# Extend "help" with the information about the patch targets
help: patching_help
.PHONY: patching_help
patching_help:
	@$(ECHO) 'make patching            Create patch bin with version using gcc'

BUILD_TYPE = test-patch
PROJECT_FILE:= project_name.txt
test = $(if $(filter $(OS),Windows),type,cat)
PROJECT_NAME=$(shell $(test) $(PROJECT_FILE))
PATCH_VERSION = --versionoption=1
PATCH_ZIP = --zipoption=0
patching:
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating patch bin with version using gcc
	python $(REPO_ROOT_DIR)/tools/scripts/build_project.py $(BUILD_TYPE) $(PROJECT_NAME) $(PATCH_VERSION) $(PATCH_ZIP)

