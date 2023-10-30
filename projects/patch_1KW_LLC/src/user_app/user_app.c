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
#include "uart_drv.h"                // uart driver functions
#include "pmbus_mfr_specific_handlers.h"

#include "open_loop_LLC.h"
#include "scp_retry.h"


/**
 * Example how to setup a user thread for RTX RTOS.  Note only 1 USER thread is allowed to memory allocation.
 */
STATIC void USER_Thread(void const* arg);
/*lint -esym(844, tid_USER_Thread) */
osThreadId tid_USER_Thread;
osThreadDef(USER_Thread, osPriorityNormal, 1, OS_THREAD_STACK_SIZE);

/*lint -e715 */
STATIC void USER_Thread(void const* arg)
{
	// Insert 1 time init code here:


	/* thread loop */
	for(;;)
	{
		// Insert user thread code here executed every 1ms RTOS tick time

	}
}



/**
 * Main entry point of the user application.  Good place to set breakpoint when debugging a patch.
 * This will only get executed a single time prior to configuration loading and before RTOS starts
 */
void user_drv_init(void)
{
	memset(&user_data, 0, sizeof(USER_DATA_t));  // ZI the user data
	//patch_rom_table.patch_FAULTS_Resolve_Shutdown_Priority = &FAULTS_User_Resolve_Shutdown_Priority;
	ntc_temp = &user_ntc_temp_lut[0];  // set the ntc lut to rom constant table


#ifdef SCP_RETRY
	// SCP_RETRY patch
	patch_rom_table.patch_COMMON_FAULT_HANDLE = patch_COMMON_FAULT_HANDLE;
#endif


	// this is the initialization of user pmbus commands autogenerated from pmbus spreadsheet
	ptr_mfr_specific_init = (mfr_specific_init_ptr) pmbus_mfr_autogen_init;

	set_module_init_cb(MODULE_REGULATION, regulation_sm_callbacks_init);  // set the pointer funtion in init of regulation state machine to user callback setup for each state
	/*lint -e522 */
	add_on_features_init();

	/* ------------------------------- modified on 2 Jul 2020 ------------------------------- */
	// Enable FORCE DUTY
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY_EN__SET(1);
	PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY_EN__SET(1);

	// Force Duty to 0
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY__SET(0);
	PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY__SET(0);
	/* ------------------------------- modified on 2 Jul 2020 ------------------------------- */

	pmbus_mfr_autogen_init();
	regulation_sm_callbacks_init();

	// create a user thread to execute the fan_pid function periodically
	//tid_USER_Thread = osExtThreadCreate(osThread(USER_Thread), NULL);
	//THROW_IF((tid_USER_Thread == NULL), OUT_OF_RESOURCE); /* osErrorNoMemory */
}

