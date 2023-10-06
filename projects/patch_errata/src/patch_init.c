/* ============================================================================
** Copyright (C) 2014 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/**
 * @file    patch_init.c
 * @brief   Template file for patches. Just contains functions for being
 *          compile-able.
 */

/* Compiler configurations first */
#include <stdint.h>                  /* uint * */
#include <string.h>                  /* memset() */
//#include "environment.h"        /* STATIC */
#include "fw_config.h"               /* Essential fw-wide configuration for e.g. buffer size #defines */
#include "linker_sections.h"         /* section definition to clear the BSS section. */
//#include "shasta_hal_rgu.h"
//#include "driver_common.h"          /* UVM reporting */
#include "add_on_features.h"          /* UVM reporting */
#include "linker_sections.h"
#include "shasta_hal_scu.h"
#include "pmbus_autogen.h"
#include "patch_pmbus_fifo.h"
/* ============================================================================
 * Private data
 * ========================================================================= */


/* ============================================================================
 * Functions
 * ========================================================================= */

/* ----------------------------------------------------------------------------
 * Forward declaration and documentation of (private) functions
 * ------------------------------------------------------------------------- */
void patch_entry(void);
//STATIC void patch_install(void);
void patch_pmbus_mfr_autogen_init(void);


void patch_pmbus_mfr_autogen_init(void)
{
	//pmbus_mfr_autogen_init();
	// patch the MFR_FIRMWARE_COMMAND handle:
#ifdef JIRA_785_REMAP_CLEAR_NEEDED_ON_RESET
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_FIRMWARE_COMMAND] = patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND;
#endif

#ifdef JIRA_769_SETUP_PASSWORD
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_SETUP_PASSWORD] = patch_PMBUS_HANDLE_MFR_SETUP_PASSWORD;
#endif

#ifdef JIRA_974_RESTORE_USER_CODE
	ptr_pmbus_callback[PMBUS_CMDCODE_RESTORE_USER_CODE] = patch_PMBUS_HANDLE_RESTORE_USER_CODE;
#endif
}

/* ------------------------------------------------------------------------- */
/*lint -esym(714, patch_entry)  Suppress: Symbol 'patch_entry' not referenced */
/*lint -e(957)  Suppress: Function 'patch_entry' defined without a prototype in scope. */
/*lint -esym(765, patch_entry)  Suppress: external 'patch_entry(void)' could be made static.
 * No, its called from startup_opus. */
/**
 * This is the patch entry point, which is called after the patch got
 * successfully loaded into RAM.
 */
void patch_entry(void)
{
    // Initialize the .bss and .data section
    memset(SECTION_BASE_ZI, 0, SECTION_LENGTH_ZI);
    memcpy(SECTION_BASE_RW_DESTINATION, SECTION_BASE_RW_SOURCE, SECTION_LENGTH_RW);
    // Initialize ram execution section
    memcpy(SECTION_BASE_RAM_EXEC_DESTINATION, SECTION_BASE_RAM_EXEC_SOURCE, SECTION_LENGTH_RAM_EXEC);


    SCU_SPARE_FF__SET(0x5F034EBFL);  // write the patch id to SCU spare for simple test that patch loaded

#ifdef JIRA_972_I2C_FIFO
    patch_pmbus_fifo();
#endif
#ifdef JIRA_973_I2C_FIFO
	set_module_init_cb(MODULE_PMBUS, pmbus_driver_init);
#endif
    add_on_features_init();
    set_module_init_cb(MODULE_REGULATION, add_on_features_init);
	ptr_mfr_specific_init = (mfr_specific_init_ptr) patch_pmbus_mfr_autogen_init;
}
