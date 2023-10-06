/*
 * TELEM_DATA.c
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

//#include "ramp_data.h"
USER_DATA_t user_data;


void regulation_sm_callbacks_init(void)
{
	// Set the callback functions for the regulation state machine.
	// ------------------------------------------------------------------------------------------------------------

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN);
	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);

	// ------------------------------------------------------------------------------------------------------------

	// register the callback functions for fsw interrupts

	// Setup and enable generic telemetry interrupt example


}



void AT_SHUTDOWN(PMBUS_PAGE_t loop)
{
	VCONTROL_VC_VRAMP3__VC_VCONTROL_DELTA__SET((vcontrol_idx_t) loop, 0);
}



