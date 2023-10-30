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
//#include "shasta_hal_regulation.h"
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
#include "user_ntc_temperature_lut.h"
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"
#include "pmbus_mfr_specific_handlers.h"

/*
 * Initialize any additional PMBus MFR commands
 * Ivan Addition
 */
void patch_pmbus_mfr_autogen_init(void)
{
	pmbus_mfr_autogen_init();
}

/**
 * Main entry point of the user application.  Good place to set breakpoint when debugging a patch.
 * This will only get executed a single time prior to configuration loading and before RTOS starts
 */
void user_drv_init(void)
{
	memset(&user_data, 0, sizeof(USER_DATA_t));  									// ZI the user data
	ntc_temp = &user_ntc_temp_lut[0]; 													        	 // set the ntc lut to rom constant table

	ptr_mfr_specific_init = (mfr_specific_init_ptr) pmbus_mfr_autogen_init;

	set_module_init_cb(MODULE_REGULATION, regulation_sm_callbacks_init);  								// set the pointer function in init of regulation state machine to user callback setup for each state
	user_data.count_test = 0;																		// Need to confirm what is this count_test variable used for

	add_on_features_init();

	// Function to be call when running in RAM
	regulation_sm_callbacks_init();   																//Release 2022 : venkat, Ivan, Punit Changes/patch/additions are included
	patch_pmbus_mfr_autogen_init(); //Ivan addition
}





