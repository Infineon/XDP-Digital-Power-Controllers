/*
 * regulation_state_machine_callbacks.c
 *
 *  Created on: Jun 12, 2017
 *      Author: dlewis4
 */
#include "periodic_functions.h"
#include "pmbus_autogen.h"      					// PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"
#include "dtimer_drv.h"
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
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_isen.h"
#include "regulation_state_machine_callbacks.h"
#include "log_app.h"            					 /* Logging of errors */
#include "periodic_functions.h"
#include "patch_rom_table.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_fault_common.h"
#include "add_on_features.h"
#include "vin_startup_sense.h" 					  //VenkatG addition


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

#ifdef FREQUENCY_DITHER 
	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN,
			frequency_dither_disable);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			frequency_dither_disable);
#endif	

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_ENABLE,
			TON_RISE_ENABLE);

//VenkatG addition
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
			AT_PRESTARTUP);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);																//Droop Enable

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN); 

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);
   //Ivan addition
	Regulation_set_regulation_event_cb(REGULATION_STATE_FAULT, REGULATION_CONTROLS_FAULT,
			AT_FAULT_SHUTDOWN);

	// ------------------------------------------------------------------------------------------------------------


	// register the callback functions for fsw interrupts
#ifdef FREQUENCY_DITHER
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_3, frequency_dither_irq_callback);
#endif


#ifdef FSW_RAMP_PCMC_STARTUP
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_4, ramp_switching_frequency_pcmc_startup_irq_handle);
#endif

	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_2, remove_added_droop_irq_callback);


}

/*
 * Function executes when the target vid window is reached during soft start.
 */
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{
	//venkatG addition
	added_droop_enable(loop);
}


/*
 * Function executes when the ton_rise state when given an enable command.
 * Put things here that modify soft start.
 */
void TON_RISE_ENABLE(PMBUS_PAGE_t loop)
{
	//venkatG addition
	#ifdef en_ishare
		enable_ishare(loop);
	#endif
		CAL_DROOP(loop); // droop calculation change by Ivan
}

/*
 * Function to execute only droop calculation command
*/
void CAL_DROOP (PMBUS_PAGE_t loop)
{
	calculate_droop(loop);
}


/*
 * Measuring Vin from PRISEN in off mode
 */
void AT_PRESTARTUP(PMBUS_PAGE_t loop)
{
#ifdef FSW_RAMP_PCMC_STARTUP
	ramp_switching_frequency_pcmc_startup_enable(loop); // change by Ivan
#endif
	//added by VenkatG
	#ifdef VIN_SEL_EN
		Calculate_Vin_from_PRISEN(loop);
	#endif
}


/*
 * Function executes when the target vid reached on enable.
 */
void AT_TARGET_ENABLE(PMBUS_PAGE_t loop)
{
	#ifdef FREQUENCY_DITHER
		frequency_dither_enable(loop);
	#endif
}

/*
 * Function executes whenever the shutdown that was requested is completed.
 */
void AT_SHUTDOWN(PMBUS_PAGE_t loop)
{
	//venkatG addition
	#ifdef VIN_SEL_EN
		Enable_PRISEN_Sensing(loop);
	#endif

	#ifdef FSW_RAMP_PCMC_STARTUP
		ramp_switching_frequency_pcmc_startup_disable(loop);
	#endif 
	
	//venkatG addition
	#ifdef en_ishare
		disable_ishare(loop);
	#endif	
		added_droop_disable(loop);
}  


void AT_FAULT_SHUTDOWN(PMBUS_PAGE_t loop)
{
	//Ivan addition
	#ifdef VIN_SEL_EN
		Enable_PRISEN_Sensing(loop);
	#endif
}



