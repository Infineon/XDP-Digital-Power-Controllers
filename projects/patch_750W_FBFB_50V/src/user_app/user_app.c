/* ============================================================================
** Copyright (C) 2020 Infineon. All rights reserved.
**               Infineon Technologies, PSS SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorisation.
** ============================================================================
**
** Created on:  2020-03-05
*
 *      Author: kotvytsr
 */


#include "dtimer_drv.h"
#include "telemetry_drv.h"
#include "regulation_api.h"

#include "pmbus_api.h"
#include "pmbus_gpio_control.h"

#include "regulation_state_machine_callbacks.h"
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "user_app.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "cmsis_os_ext.h"                   // ARM::CMSIS:RTOS:Keil RTX ext

#include "user_ntc_temperature_lut.h"
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"

#include "pmbus_mfr_specific_handlers.h"
#include "input_current_correction.h"
#include "transformer_scaling.h"

void patch_pmbus_mfr_autogen_init(void)
{
	pmbus_mfr_autogen_init();
}

void user_drv_init(void)
{
	memset(&user_data, 0, sizeof(USER_DATA_t));  // ZI the user data

	ntc_temp = &user_ntc_temp_lut[0];  // set the ntc lut to rom constant table

	patch_rom_table.patch_Telemetry_Sample = patch_Telemetry_Sample;
	patch_rom_table.patch_Telemetry_change_scales = patch_Telemetry_change_scales;

	patch_Telemetry_change_scales(PMBUS_PAGE_0);
	patch_Telemetry_change_scales(PMBUS_PAGE_1);

	// this is the initialization of user pmbus commands autogenerated from pmbus spreadsheet
	ptr_mfr_specific_init = (mfr_specific_init_ptr) patch_pmbus_mfr_autogen_init;
	set_module_init_cb(MODULE_REGULATION, regulation_sm_callbacks_init);  // set the pointer funtion in init of regulation state machine to user callback setup for each state
	add_on_features_init();

	regulation_sm_callbacks_init();
	patch_pmbus_mfr_autogen_init();
}

