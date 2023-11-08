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
help: pmbus_default_array_help
.PHONY: pmbus_default_array_help
pmbusdefault_help:
	@$(ECHO) 'make pmbus_default_array            Create the pmbus_default_array c table code'

PMBUS_DEFAULT = ./src/shasta_pmbus.xlsx
#PMBUS_ROM_AUTO_DIR = ../../common/modules/pmbus
PMBUS_USER_DIR = ./src/user_app
pmbus_default_array:
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating pmbus_default_array c table code
	python $(REPO_ROOT_DIR)/tools/scripts/otp_file_gen/config2otp_patch.py $(PMBUS_DEFAULT) $(PMBUS_USER_DIR)

