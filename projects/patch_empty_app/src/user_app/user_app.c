/*
 * user_app.c
 *
 *  Created on: Jun 13, 2017
 *      Author: dlewis4
 */



#include "dtimer_drv.h"
#include "telemetry_drv.h"
#include "regulation_api.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "pmbus_api.h"
#include "pmbus_gpio_control.h"
#include "shasta_hal_vsen.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_isen.h"
#include "regulation_state_machine_callbacks.h"
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "user_app.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "cmsis_os_ext.h"                   // ARM::CMSIS:RTOS:Keil RTX ext
#include "log_app.h"             /* Logging of errors */
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"
//#include "pmbus_patch_configuration_data.h" //uncomment to store PMBus Default(inclusive of MFR) in OTP FW replace ROM FW Default. The new Default array is generate by Build Target -> pmbusdefault

/**
 * Main entry point of the user application.  Good place to set breakpoint when debugging a patch.
 */

void patch_pmbus_mfr_autogen_init(void);


void patch_pmbus_mfr_autogen_init(void)
{
	//uncomment to store PMBus Default(inclusive of MFR) in OTP FW replace ROM FW Default.
	// The new Default array is generate by Build Target -> pmbusdefault
#ifdef Patch_Pmbus_Table
	ptr_pmbus_callback[PMBUS_CMDCODE_RESTORE_DEFAULT_ALL] = patch_PMBUS_HANDLE_RESTORE_DEFAULT_ALL;
#endif
	pmbus_mfr_autogen_init();

}

void user_drv_init(void)
{
	memset(&user_data, 0, sizeof(USER_DATA_t));  // ZI the user data
#ifdef pmbus_memory_error
	patch_rom_table.patch_PMBUS_Copy_From_Memory = &patch_PMBUS_Copy_From_Memory;
#endif
	// this is the initialization of user pmbus commands autogenerated from pmbus spreadsheet
	ptr_mfr_specific_init = (mfr_specific_init_ptr) patch_pmbus_mfr_autogen_init;
	set_module_init_cb(MODULE_REGULATION, regulation_sm_callbacks_init);  // set the pointer funtion in init of regulation state machine to user callback setup for each state

	//add for testing RAM
	patch_pmbus_mfr_autogen_init();
//	regulation_sm_callbacks_init();

	add_on_features_init();
}





