# =================================================================================================
# File:     MakefilePmbus.mk
# Project:  Shasta Firmware
# -------------------------------------------------------------------------------------------------
# Brief:    Makefile plugin for pmbus code generation
# =================================================================================================

###################################################################################################
# Extend the help menue
###################################################################################################

# Extend "help" with the information about the patch targets
help: pmbus_help
.PHONY: pmbus_help
pmbus_help:
	@$(ECHO) 'make pmbus            Create the pmbus c code'

PMBUS_SPREADSHET = ./src/shasta_pmbus.xlsx
PMBUS_ROM_AUTO_DIR = ../../common/modules/pmbus
PMBUS_USER_AUTO_DIR = ./src/user_app
pmbus:
	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
	@$(ECHO) Generating PMBus c code
	python $(REPO_ROOT_DIR)/tools/scripts/pmbus_xlsx2c.py $(PMBUS_SPREADSHET) $(PMBUS_ROM_AUTO_DIR)/pmbus_autogen.h $(PMBUS_ROM_AUTO_DIR)/pmbus_autogen.c $(PMBUS_USER_AUTO_DIR)/pmbus_mfr_autogen.h $(PMBUS_USER_AUTO_DIR)/pmbus_mfr_autogen.c

