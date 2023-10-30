/* ============================================================================
z** Copyright (C) 2020 Infineon. All rights reserved.
**               Infineon Technologies, PSS SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorisation.
** ============================================================================
**
** Created on:  2020-08-19
*
 *      Author: kotvytsr
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

#include "buck_boost.h"
#include "deadtime.h"
#include "light_load_features.h"
#include "vin_trim.h"
#include "zvs_feature.h"
#include "pwm_skip.h"

USER_DATA_t user_data;



void regulation_sm_callbacks_init(void)
{
	// Set the callback functions for the regulation state machine.
	// ------------------------------------------------------------------------------------------------------------
	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_ENABLE,
			AT_TARGET_ENABLE);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_ENABLE,
			TON_RISE_ENABLE);


	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
			TON_DELAY_ENABLE);


	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
			AT_SHUTDOWN);

//	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_SHUTDOWN,
//			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_DELAY, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_FAULT, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_FAULT, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);


	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);

	// ------------------------------------------------------------------------------------------------------------

#ifdef ishare_feature_en
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_2, remove_added_droop_irq_callback);
#endif


#ifdef light_load_features_en
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_4, pwm_skip_irq_handle); // high frequency IRQ for PWM skip feature
#endif

}

void TON_DELAY_ENABLE(PMBUS_PAGE_t loop)
{
#ifdef eff_table_feature_en
	patch_Telemetry_Sample();
#endif

#ifdef deadtime_feature_en
	Set_StartUp_Deadtime(loop);
#endif

}


void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{
#ifdef deadtime_feature_en
	Set_SteadyState_Deadtime(loop);
#endif

#ifdef ishare_feature_en
	added_droop_enable(loop);
#endif
}

void TON_RISE_ENABLE(PMBUS_PAGE_t loop)
{
	user_data.shutdown_status = 0;

#ifdef ishare_feature_en
	enable_ishare(loop);
#endif

#if defined(feed_forward_feature_en) || defined(zvs_feature_en) || defined(light_load_features_en)
	// buck boost feed-forward IRQ 1 -> XDPP1100 will crashed if FF_IRQ < 8
	Regulation_setup_fsw_irq(loop, fsw_irq_idx_1, fsw_irq_rate_sel_8); // it is 8 by default
#endif

#ifdef vin_trim_feature_en
	Update_Vin_Trim(user_data.active_vin_trim_slope, user_data.active_vin_trim_offset);
#endif
}

void AT_TARGET_ENABLE(PMBUS_PAGE_t loop)
{

#ifdef vin_trim_feature_en
	Update_Vin_Trim(user_data.active_vin_trim_slope, user_data.active_vin_trim_offset);
#endif


#ifdef zvs_feature_en
	iout_zvs_threshold_irq_handle_enable(); // change date 220405 enable zvs to turn off an light load in irq checking
#endif

#ifdef light_load_features_en        //added by venkatg
	light_load_features_threshold_irq_handle_enable(); //added by venkatg
#endif                       //added by venkatg
}

void AT_SHUTDOWN(PMBUS_PAGE_t loop)
{
	if (user_data.shutdown_status == 0)	// for one time execution
	{
		user_data.shutdown_status = 1;

	#ifdef zvs_feature_en
		iout_zvs_threshold_irq_handle_disable();
	#endif

	#ifdef light_load_features_en
		light_load_features_threshold_irq_handle_disable();
	#endif

	#ifdef ishare_feature_en
		disable_ishare(loop);
	#endif

// If this is done here, execution in TON_DELAY_ENABLE is blocked by user_data.deadtime_flag==1,
// and if the MFR_DEADTIME has been changed in between, the new dead time value will not be used for the next start-up.
/*
	#ifdef deadtime_feature_en
		Set_StartUp_Deadtime(loop);
	#endif
*/

	#ifdef vin_trim_feature_en
		Update_Vin_Trim(user_data.standby_vin_trim_slope, user_data.standby_vin_trim_offset);
	#endif

	#if defined (feed_forward_feature_en) || defined(zvs_feature_en) || defined(light_load_features_en)
		// buck boost feed-forward IRQ 1 -> XDPP1100 will crashed if FF_IRQ < 8
		Regulation_setup_fsw_irq(loop, fsw_irq_idx_1, fsw_irq_rate_sel_disabled); // it is 8 by default
	#endif

	}
}

