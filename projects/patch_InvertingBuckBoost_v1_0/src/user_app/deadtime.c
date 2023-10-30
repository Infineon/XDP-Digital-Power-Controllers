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
** Created on:  2020-08-19
*
 *      Author: halimand
 */

#include "fw_config.h"           /* Essential fw-wide configuration for e.g. buffer size #defines */
#include "log_app.h"

#include <stdint.h>              /* uint*_t */
#include "pmbus_autogen.h"

#include "regulation_state_machine_callbacks.h"
#include "pmbus_mfr_specific_handlers.h"
#include "pmbus_api.h"
#include "regulation_drv.h"
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"

#include "shasta_hal_pwm.h"


#include "deadtime.h"

#ifdef deadtime_feature_en

/**
 * Function to set the startup deadtime of PWM2 DR and PWM6 DR
 * @param loop regulation loop to operate on.
 */
void Set_StartUp_Deadtime(PMBUS_PAGE_t loop)
{
	if(user_data.deadtime_flag == 0)
	{
		user_data.deadtime_flag = 1;
		// Set both PWM2 DR and PWM6 DR with startup PWM2 DR and PWM6 DR
		PWM_PWM2_DEADTIME__PWM2_DR__SET(user_data.Startup_PWM2_PWM6_dr);
		PWM_PWM6_DEADTIME__PWM6_DR__SET(user_data.Startup_PWM2_PWM6_dr);
		// to add below PWM12 line for deadtime change to be effective.
		PWM_PWM12_DEADTIME__SET(PWM_PWM12_DEADTIME__GET()); //220504 changes

		uint8_t *data = PMBUS_GET_DATA_ARRAY_POINTER(0, PMBUS_CMDCODE_PWM_DEADTIME);
		user_data.SteadyState_PWM2_dr = data[1*2 + 1];	// PWM2 dr
		user_data.SteadyState_PWM6_dr = data[5*2 + 1];	// PWM6 dr
	}
}


/**
 * Function to set the steady state deadtime of PWM2 DR and PWM6 DR
 * @param loop regulation loop to operate on.
 */
void Set_SteadyState_Deadtime(PMBUS_PAGE_t loop)
{
	user_data.deadtime_flag = 0;
	// Set both PWM2 DR and PWM6 DR with the steadystate values
	PWM_PWM2_DEADTIME__PWM2_DR__SET(user_data.SteadyState_PWM2_dr);
	PWM_PWM6_DEADTIME__PWM6_DR__SET(user_data.SteadyState_PWM6_dr);
	// to add below PWM12 line for deadtime change to be effective.
	PWM_PWM12_DEADTIME__SET(PWM_PWM12_DEADTIME__GET());


	// DEBUG - to delete later - show PWM2 DR startup
//	PMBUS_Set_Command_Byte(loop, PMBUS_CMDCODE_MFR_DBG_DEADTIME, user_data.SteadyState_PWM2_dr);
}

#endif

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Function read the previous DeadTime of pwm2 dr and pwm6 dr and store new MFR DeadTime
 * @param page Selects the PMBus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_MFR_DEADTIME (PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef deadtime_feature_en

	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		// Get PMBus MFR DEADTIME value and put in user_data for storing
		user_data.Startup_PWM2_PWM6_dr = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_DEADTIME);

		// Get both PWM2 DR and PWM6 DR and store it in the user_data
		/* ---------------------------- Modified by Halim on 20 Aug 2020 ---------------------------- */
		uint8_t *data = PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_PWM_DEADTIME);

		user_data.SteadyState_PWM2_dr = data[1*2 + 1];	// PWM2 dr
		user_data.SteadyState_PWM6_dr = data[5*2 + 1];	// PWM6 dr
		if(user_data.Startup_PWM2_PWM6_dr == 0)
		{
			user_data.Startup_PWM2_PWM6_dr = user_data.SteadyState_PWM2_dr;
		}
		/* ---------------------------- Modified by Halim on 20 Aug 2020 ---------------------------- */

//		user_data.SteadyState_PWM2_dr = (uint8_t)PWM_PWM2_DEADTIME__PWM2_DR__GET();
//		user_data.SteadyState_PWM6_dr = (uint8_t)PWM_PWM6_DEADTIME__PWM6_DR__GET();


		// DEBUG - to delete later - show PWM2 DR startup
//		PMBUS_Set_Command_Byte(page, PMBUS_CMDCODE_MFR_DBG_DEADTIME, user_data.Startup_PWM2_PWM6_dr);
	}
#endif
}

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Function read the previous DeadTime of pwm2 dr and pwm6 dr and store new MFR dbg DeadTime
 * @param page Selects the PMBus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_MFR_DBG_DEADTIME (PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef deadtime_feature_en
	if (direction == PMBUS_READ)
	{
		// DEBUG - to delete later - show PWM2 DR startup
//		PMBUS_Set_Command_Byte(page, PMBUS_CMDCODE_MFR_DBG_DEADTIME, user_data.SteadyState_PWM2_dr);
//		PMBUS_Set_Command_Byte(page, PMBUS_CMDCODE_MFR_DBG_DEADTIME, user_data.SteadyState_PWM6_dr);
//		PMBUS_Set_Command_Byte(page, PMBUS_CMDCODE_MFR_DBG_DEADTIME, user_data.Startup_PWM2_PWM6_dr);

//		PMBUS_Set_Command_Byte(page, PMBUS_CMDCODE_MFR_DBG_DEADTIME, (uint8_t)PWM_PWM2_DEADTIME__PWM2_DR__GET());
	}
#endif
}


