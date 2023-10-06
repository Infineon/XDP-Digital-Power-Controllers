/*
 * regulation_state_machine_callbacks.c
 *
 *  Created on: Jun 12, 2017
 *      Author: dlewis4
 */
#include "periodic_functions.h"
#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"
#include "dtimer_drv.h"
//#include "telemetry_drv.h"
#include "telemetry_api.h"
#include "telemetry_irq_handlers.h"
#include "regulation_api.h"
#include "regulation_drv.h"
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
//#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
//#include "cmsis_os_ext.h"                   // ARM::CMSIS:RTOS:Keil RTX ext
#include "log_app.h"             /* Logging of errors */
#include "patch_rom_table.h"
//#include "rt_TypeDef.h"
//#include "rt_Mailbox.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_fault_common.h"
#include "add_on_features.h"
#include "log_debug_data_examples.h"
#include "add_on_features.h"

//#include "ramp_data.h"
USER_DATA_t user_data;

#define FAULT_TYPE_EXTERNAL fw_spare1

#ifdef FREQUENCY_DITHER
void frequency_dither_disable_call(PMBUS_PAGE_t loop);
#endif

/**
 * Examples of how to register callback functions for the regulation state machine states.  The user can decide
 * whether it makes sense to enable their feature at these particular points in time.  Anything that one would
 * want to call on soft start or shutdown should be registered here.
 */
void regulation_sm_callbacks_init(void)
{
	//Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN, pwm_boot_cap_startup);

	// Set the callback functions for the regulation state machine.
	// ------------------------------------------------------------------------------------------------------------
	#ifdef retry_if_removed_bug_fix
	/**
	 * Errata bug fix for retry when removed fault response.  Not needed if response is not required.
	 * https://jirapmmprod.intra.infineon.com/browse/SHASTA-674
	 */
	sys_irq_handler_set((IRQn_Type) VC0_VCONTROL_IRQn,
			(Irq_Handler) patch_Regulation_VC0_VCONTROL_IRQHandler); /* Put interrupt handler into vector table */
	#endif

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_ENABLE,
			AT_TARGET_ENABLE);

#ifdef FREQUENCY_DITHER
	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN,
			frequency_dither_disable_call);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			frequency_dither_disable_call);
#endif

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_ENABLE,
			TON_RISE_ENABLE);

#ifdef FSW_RAMP_PCMC_STARTUP
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
			ramp_switching_frequency_pcmc_startup_enable);
#endif

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN, AT_SHUTDOWN);
	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);


	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, AT_SHUTDOWN);
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_SHUTDOWN, AT_SHUTDOWN);
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_FAULT, AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, AT_SHUTDOWN);
	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_FAULT, AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_FAULT, REGULATION_CONTROLS_SHUTDOWN, AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_DELAY, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, AT_SHUTDOWN);
	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_DELAY, REGULATION_CONTROLS_FAULT, AT_SHUTDOWN);
	//Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN, AT_SHUTDOWN);
	//Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, AT_SHUTDOWN);

	// ------------------------------------------------------------------------------------------------------------

	// register the callback functions for fsw interrupts
#ifdef FREQUENCY_DITHER
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_3, frequency_dither_irq_callback);
#endif
#ifdef FSW_RAMP_PCMC_STARTUP
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_4, ramp_switching_frequency_pcmc_startup_irq_handle);
#endif

	// Setup and enable generic telemetry interrupt example
	user_data.VIN_Thresh_Exponent = -2;   //Set the threshold exponent
	Telemetry_Set_Irq_Threshold(TELEM_IRQ7, 96, user_data.VIN_Thresh_Exponent); //24V  in U11.2 - 96
	Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ7, 80, user_data.VIN_Thresh_Exponent); //20V  in U11.2 - 96

	// page - loop 0 Selects the loop for reporting. PMBUS_PAGE_t
	// telemetry irq source - VIN loop 0 Telemetry_irq_src_t
	// generic telemetry irq number - TELEM_IRQ7 (11) Telemetry_IRQ_TYPE_t
	// set callback function - Telemetry_IRQ_VIN_HANDLE
	Telemetry_Setup_Irq(PMBUS_PAGE_0, VIN_0, TELEM_IRQ7, Telemetry_IRQ_VIN_HANDLE);
	//Setup and enable interrupt
	// page - loop 1 Selects the loop for reporting. PMBUS_PAGE_t
	// telemetry irq source - VIN loop 1 Telemetry_irq_src_t
	// generic telemetry irq number - TELEM_IRQ8 (12) Telemetry_IRQ_TYPE_t
	// set callback function - Telemetry_IRQ_VIN_HANDLE
	Telemetry_Setup_Irq(PMBUS_PAGE_1, VIN_1, TELEM_IRQ8, Telemetry_IRQ_VIN_HANDLE);


	//Example of setting up the response on external fault to FAULT_RESPONSE_RETRY_SHUTDOWN with 6 hiccups
	//with no hysteresis

	FAULTS_SETUP_FAULT(
			PMBUS_PAGE_0 ,  //PMBUS_PAGE_t pmbus page to update status on
			FAULT_TYPE_EXTERNAL,
			0xB0, //fault response byte: Response[7:6], Retry_Setting{5:3] and Delay_Time[2:0]
			FAULTS_CONFIG_VOUT_DELAY_UNIT, //Config_Delay_Unit This is used to decode the fields in FW_CONFIG_FAULTS PMBUS command (Time unit for retry responses)
			VOLTAGE_RESPONSE_TYPE, //response_config This is used to decode response command byte. Voltage(Temp, TON MAX) type or CURRENT type response
			(uint8_t)PMBUS_CMDCODE_STATUS_MFR_SPECIFIC, //pmbus_status_command status command number STATUS_XXX used for reporting the fault on pmbus
			(uint8_t)PMBUS_STATUS_MFR_SPECIFIC_0, //pmbus_status_bit_position bit position in command STATUS_XXX used for reporting the fault on pmbus
			(FAULT_TYPE_t) FAULT_TYPE_EXTERNAL); //hysteresis sets the hysteresis FAULT_TYPE_t

	/*lint -e522 */
	log_debug_data_init();

}

/**
 * Function executes when the target vid window is reached during soft start.
 */
/*lint -e715 */
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{

}

/**
 * Function executes when the ton_rise state when given an enable command.
 * Put things here that modify soft start.
 */
/*lint -e715 */
void TON_RISE_ENABLE(PMBUS_PAGE_t loop)
{
	//ramp_switching_frequency_pcmc_startup_enable(loop);

}

/**
 * Function executes when the target vid reached on enable.
 */
/*lint -e715 */
void AT_TARGET_ENABLE(PMBUS_PAGE_t loop)
{
#ifdef FREQUENCY_DITHER
	frequency_dither_enable(loop);
#endif
}

/**
 * Function executes whenever the shutdown that was requested is completed.
 */
/*lint -e715 */
void AT_SHUTDOWN(PMBUS_PAGE_t loop)
{
#ifdef FSW_RAMP_PCMC_STARTUP
	ramp_switching_frequency_pcmc_startup_disable(loop);
#endif

#ifdef JIRA_752_Disable_I2C_EN
	if(user_data.i2c_Flag == 0) //wwf20211123
	{
		user_data.i2c_Flag = 1; //wwf20211123
		PMBUS_Inter_Thread_Set_Command(PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_DISABLE_I2C, PMBUS_WRITE); //wwf20211123
	}
#endif

#ifdef JIRA_739_VCONTROL_DELTA_SET
	VCONTROL_VC_VRAMP3__VC_VCONTROL_DELTA__SET( (vcontrol_idx_t) loop,0);
#endif

}
#ifdef FREQUENCY_DITHER
/**
 * Function executes whenever the shutdown that was requested is completed.
 */
/*lint -e715 */
void frequency_dither_disable_call(PMBUS_PAGE_t loop)
{
	frequency_dither_disable(loop);

#ifdef JIRA_739_VCONTROL_DELTA_SET
	VCONTROL_VC_VRAMP3__VC_VCONTROL_DELTA__SET( (vcontrol_idx_t) loop,0);
#endif
}
#endif

/**
 * Callback function for generic telemetry interrupt on VIN source.
 * Can be used to adjust deadtime or pid settings based on changes in VIN
 * @param Telemetry_type @ref Telemetry_IRQ_TYPE_t telemetry type to operate on
 */
void Telemetry_IRQ_VIN_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type) {

	_Telemetry_s * Telemetry_str = Telemetry_GET_ARRAY_POINTER(Telemetry_type) ;
	//Grab the loop on which interrupt occurred
	//uint8_t loop = Telemetry_str->loop;

	if ((~Telemetry_str->polarity)&1) {
        //threshold
	}
	else {
		//Hysteresis threshold
	}

}


