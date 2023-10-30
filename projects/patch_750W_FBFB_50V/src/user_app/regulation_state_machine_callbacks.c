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
** Created on:  2021-03-05
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
#include "shasta_hal_vcontrol.h"
#include "pmbus_mfr_specific_handlers.h"

#include "regulation_state_machine_callbacks.h"

#include "add_on_features.h"
#include "vin_trim.h"


USER_DATA_t user_data;


void regulation_sm_callbacks_init(void)
{

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_ENABLE,
			AT_TARGET_ENABLE);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_ENABLE,
			TON_RISE_ENABLE);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);



	// ------------------------------------------------------------------------------------------------------------

	// register the callback functions for fsw interrupts
#ifdef en_ishare
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_2, remove_added_droop_irq_callback);
#endif

}

void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{
#ifdef en_ishare
	added_droop_enable(loop);
#endif
}

void TON_RISE_ENABLE(PMBUS_PAGE_t loop)
{
#ifdef vin_trim_feature_en
	Update_Vin_Trim(user_data.active_vin_trim_slope, user_data.active_vin_trim_offset);
#endif
#ifdef en_ishare
	enable_ishare(loop);
#endif
}

void AT_TARGET_ENABLE(PMBUS_PAGE_t loop)
{
#ifdef vin_trim_feature_en
	Update_Vin_Trim(user_data.active_vin_trim_slope, user_data.active_vin_trim_offset);
#endif
}

void AT_SHUTDOWN(PMBUS_PAGE_t loop)
{

#ifdef en_ishare
	disable_ishare(loop);
#endif
#ifdef vin_trim_feature_en
	Update_Vin_Trim(user_data.standby_vin_trim_slope, user_data.standby_vin_trim_offset);
//	PMBUS_Set_Command_Dword(loop, PMBUS_CMDCODE_MFR_DBG_2, 0x01);
#endif
	VCONTROL_VC_VRAMP3__VC_VCONTROL_DELTA__SET((vcontrol_idx_t) loop, 0);
}
