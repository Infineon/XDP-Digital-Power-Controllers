/*
 * regulation_state_machine_callbacks.c
 *
 *  Created on: Jun 12, 2017
 *      Author: dlewis4
 */
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

#include "open_loop_LLC.h"
#include "vin_trim.h"


USER_DATA_t user_data;




/**
 * Examples of how to register callback functions for the regulation state machine states.  The user can decide
 * whether it makes sense to enable their feature at these particular points in time.  Anything that one would
 * want to call on soft start or shutdown should be registered here.
 */
void regulation_sm_callbacks_init(void)
{

	// Set the callback functions for the regulation state machine.
	// ------------------------------------------------------------------------------------------------------------

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_ENABLE,
			AT_TARGET_ENABLE);


	/* ------------------------------- modified on 2 Jul 2020 ------------------------------- */
	// Enable FORCE DUTY
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY_EN__SET(1);
	PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY_EN__SET(1);

	// Force (update) duty
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY__SET(0);
	PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY__SET(0);
	/* ------------------------------- modified on 2 Jul 2020 ------------------------------- */


	/* ------------------------------- modified on 16 Jun 2020 ------------------------------- */
	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);

	/* ------------------------------- modified on 16 Jun 2020 ------------------------------- */


	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_ENABLE,
			TON_RISE_ENABLE);


	/* ------------------------------- modified on 16 Jun 2020 ------------------------------- */
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
			AT_TON_DELAY);

	/* ========================================================== VIN_TRIM  ========================================================== */
		Regulation_set_regulation_event_cb(REGULATION_STATE_FAULT, REGULATION_CONTROLS_SHUTDOWN, AT_SHUTDOWN);
		Regulation_set_regulation_event_cb(REGULATION_STATE_FAULT, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, AT_SHUTDOWN);
		Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN,
					AT_SHUTDOWN);

			Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
					AT_SHUTDOWN);
		/* ========================================================== VIN_TRIM  ========================================================== */
	/* ------------------------------- modified on 16 Jun 2020 ------------------------------- */
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);

	// ----------------------- DELTA patch: OpenLoop LLC Softstart  -----------------------
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_5, OpenLoopLLC_SoftStart_IRQ_Handler);
	// ----------------------- DELTA patch: OpenLoop LLC Softstart  -----------------------


}

/**
 * Function executes when the target vid window is reached during soft start.
 */
/*lint -e715 */
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{
	// ++++++++++++++++++++++++ Enable VOUT_UV Fault ++++++++++++++++++++++++
//	Faults_Mask_Fault(loop, 1, // hw fault
//							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN)), // set_mask
//							0	// clear_mask
//							);
	// ++++++++++++++++++++++++ Enable VOUT_UV Fault ++++++++++++++++++++++++

	// Set SCP Threshold to Startup SCP Thres
	ISEN_ISP_FAULT1__ISP_SCP_THRESH__SET(loop, user_data.steady_scp_thres);
}

/**
 * Function executes when the ton_rise state when given an enable command.
 * Put things here that modify soft start.
 */
/*lint -e715 */
void TON_RISE_ENABLE(PMBUS_PAGE_t loop)
{
#ifdef vin_trim
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
	Update_Vin_Trim(user_data.active_vin_trim_slope, user_data.active_vin_trim_offset);
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
#endif
}

/**
 * Function executes when the target vid reached on enable.
 */
/*lint -e715 */
void AT_TARGET_ENABLE(PMBUS_PAGE_t loop)
{
#ifdef vin_trim
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
	Update_Vin_Trim(user_data.active_vin_trim_slope, user_data.active_vin_trim_offset);
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
#endif

}

/**
 * Function executes when the ton_delay.
 */
/*lint -e715 */
void AT_TON_DELAY(PMBUS_PAGE_t loop){

	// ++++++++++++++++++++++++ Disable VOUT_UV Fault ++++++++++++++++++++++++
//	Faults_Mask_Fault(loop, 1, // hw fault
//							0, // set_mask
//							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN))	// clear_mask
//							);
	// ++++++++++++++++++++++++ Disable VOUT_UV Fault ++++++++++++++++++++++++

	OpenLoopLLC_SoftStart_Enable(loop);

//	user_data.steady_scp_thres = ISEN_ISP_FAULT1__ISP_SCP_THRESH__GET(0);


	// Set SCP Threshold to Startup SCP Thres
	ISEN_ISP_FAULT1__ISP_SCP_THRESH__SET(loop, user_data.startup_scp_thres);
}


/**
 * Function executes whenever the shutdown that was requested is completed.
 */
/*lint -e715 */
void AT_SHUTDOWN(PMBUS_PAGE_t loop)
{

	// ++++++++++++++++++++++++ Disable VOUT_UV faults ++++++++++++++++++++++++
	Faults_Mask_Fault(loop, 1, // hw fault
							0, // set_mask
							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN))	// clear_mask
							);
	// ++++++++++++++++++++++++ Disable VOUT_UV faults ++++++++++++++++++++++++


	ISEN_ISP_FAULT1__ISP_SCP_THRESH__SET(loop, user_data.startup_scp_thres);

	OpenLoopLLC_SoftStart_Disable(loop);
#ifdef vin_trim
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
	Update_Vin_Trim(user_data.standby_vin_trim_slope, user_data.standby_vin_trim_offset);
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
#endif
}


