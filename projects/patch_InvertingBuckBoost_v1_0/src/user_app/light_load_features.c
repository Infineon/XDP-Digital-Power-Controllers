/* ============================================================================
** Copyright (C) 2020 Infineon. All rights reserved.
**               Infineon Technologies, PSS SIS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorization.
** ============================================================================
**
** Created on:  2021-01-06
*
 *      Author: kotvytsr
 */


#include "fw_config.h"           /* Essential fw-wide configuration for e.g. buffer size #defines */
#include "environment.h"         /* STATIC, */
#include <stdint.h>              /* uint*_t */
#include <string.h>				 /* memcpy */
#include "fw_bool.h"             /* boolean type */
#include "pmbus_mfr_specific_handlers.h"
#include "pmbus_api.h"
#include "regulation_drv.h"
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"

// debug headers:
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_fan.h"
#include "shasta_hal_trim.h"
#include "shasta_hal_vsen.h"
#include "shasta_hal_pid.h"
#include "regulation_api.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "regulation_drv.h"
#include "telemetry_drv.h"
#include "pmbus_drv.h"
#include "driver_common.h"
#include "log_app.h"
#include "regulation_state_machine_callbacks.h"
#include "add_on_features.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "uart_drv.h"
#include "user_app.h"

#include "buck_boost.h"
#include "pwm_skip.h"

#ifdef light_load_features_en

#include "light_load_features.h"
#include "telemetry_api.h"


/*
 * light_load_features initialization at AT_VID
 */

void light_load_features_threshold_irq_handle_enable(void)
{
	user_data.light_load_features_initialization_status = 0;

	iout_telemetry_get_high_frequence_limited(0);

	uint16_t iout_local = 0;

	if (user_data.iout[PMBUS_PAGE_0] > 0)
	{
		iout_local = (uint16_t)user_data.iout[PMBUS_PAGE_0];
	}

	if (iout_local < user_data.light_load_features_threshold_current)
	{
		light_load_features_enable(ON);
	}
	else
	{
		light_load_features_enable(OFF);
	}

	// no IRQ enabling here, moved to FF IRQ

	user_data.light_load_features_initialization_status = 1;
}

void light_load_features_threshold_irq_handle_disable(void)
{
    // no IRQ disabling here, moved to FF IRQ

    light_load_features_enable(OFF); // disable the light load functions
    user_data.light_load_features_initialization_status = 0;
}

/*
 * light_load_features_threshold_irq_handle() is
 * hysteresis based light_load threshold IRQ callback
 * to enable / disable light_load.
 * If iout < light_load_features_threshold, then light_load -> ON
 * else light_load -> OFF
 */

void light_load_features_threshold_irq_handle(PMBUS_PAGE_t loop)
{
	uint16_t iout_local = 0;

	if (user_data.iout[PMBUS_PAGE_0] > 0)
	{
		iout_local = (uint16_t)user_data.iout[PMBUS_PAGE_0];
	}

	if (iout_local < user_data.light_load_features_threshold_current)
	{
		if (user_data.light_load_features_status == OFF)
		{
			light_load_features_enable(ON);
		}
	}
	else
	{
		if (user_data.light_load_features_status == ON)
		{
			light_load_features_enable(OFF);
		}
	}

}

/*
 * light_load_features_enable() does light load mode en/dis (ON / OFF)
 */

void light_load_features_enable(mode_t mode)	// ON / OFF
{
	uint8_t loop = REGULATION_APP.loop;

	if(mode == ON)
	{
        user_data.light_load_features_threshold_current = user_data.light_load_features_threshold_high;
	}
	else
	{
	    user_data.light_load_features_threshold_current = user_data.light_load_features_threshold_low;
	}
	/*
		-	In phase shedding mode, disable phase 2. The sequence matters.
		tlm_iout_src_sel = 0,
		Disable the pwm1, 2, 3 output. These are the pwm of phase 2.
		Could be done with pwm_on register.
		en_ibal=0.
		We will keep the same PID compensation as the 2-phase.
		-	Exist phase shedding, enable phase 2. The sequence matters.
		Enable the pwm1, 2, 3 output.
		tlm_iout_src_sel = 2.
		en_ibal=1.

	 */

	if (user_data.light_load_features_mode == PHASE_SHEDDING)
	{
		uint32_t LO = PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__GET();

		if (mode == ON)
		{
			LO |= 7u;

			TELEM_TLM_CFG1__TLM_IOUT_SRC_SEL__SET(PMBUS_PAGE_0, 0);
			PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__SET(LO);
			COMMON_EN_REG3__EN_IBAL__SET(0);
		}
		else
		{
			LO &= (~7u | user_data.zvs_current_value); //remove PWM LO force except ZVS

			PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__SET(LO);
			TELEM_TLM_CFG1__TLM_IOUT_SRC_SEL__SET(PMBUS_PAGE_0, 2);
			COMMON_EN_REG3__EN_IBAL__SET(1);
		}
	}


	/*
	 * DE mode at light load
	 *  -	When IOUT is lower than FW_CONFIG_DE_THRESH, turn off SR. The SR PWMs are defined by 0xC4 FW_CONFIG_PWM, bit 27:16
		-	When IOUT is higher than (FW_CONFIG_DE_THRESH + hysteresis), enable SR.
		The hysteresis can is hard coded.
	 *
	 */

	else if (user_data.light_load_features_mode == DIODE_EMULATION)
	{
		uint32_t LO = PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__GET();

		if (mode == ON)
		{
			LO |= 34u;	// 0x22 SR PWM
		}
		else
		{
			LO &= ~34u;
		}
		PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__SET(LO);
	}

	/*
	 * BURST mode - PWM skip. We do PWM skip (forcing to low) for
	 * certain configurable period of time.
	 *  -	When IOUT is lower than PWM skip is ON
		-	When IOUT is higher than PWM skip is OFF
	 */

	else if (user_data.light_load_features_mode == BURST)
	{
		if (user_data.pwm_skip_max != 0)	// check if that feature is enabled itself
		{
			if (mode == OFF)
			{
				pwm_skip_irq_disable();
			}
			else
			{
				pwm_skip_irq_enable();
			}
		}
	}
	else
	{
	    // disable pulse skipping, set current telemetry and phase balance back to standard values, remove all PWM LO force except ZVS
	    pwm_skip_irq_disable();
	    PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__SET(user_data.zvs_current_value);
	    TELEM_TLM_CFG1__TLM_IOUT_SRC_SEL__SET(PMBUS_PAGE_0, 2);
	    COMMON_EN_REG3__EN_IBAL__SET(1);
	}

    user_data.light_load_features_status = mode;

}

#endif

/*
 * light_load features MFR handler to set threshold and update hysteresis settings
 */

void PMBUS_HANDLE_MFR_LIGHT_LOAD_THRESH(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef light_load_features_en
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint16_t light_load_features_threshold = LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_LIGHT_LOAD_THRESH));
		user_data.light_load_features_threshold_exp = (int8_t)LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_LIGHT_LOAD_THRESH));

		// Hysteresis levels updates:
		int8_t exp_diff;
		int16_t light_load_features_hysteresis;

		exp_diff = user_data.light_load_features_hysteresis_exp - user_data.iout_exp[PMBUS_PAGE_0];
		light_load_features_hysteresis = SHIFT_EXPONENT_UNSIGNED(user_data.light_load_features_hysteresis, exp_diff);	// in iout format
		uint16_t half_of_hysteresis = light_load_features_hysteresis >> 1;	// hysteresis in iout format

		exp_diff = user_data.light_load_features_threshold_exp - user_data.iout_exp[PMBUS_PAGE_0];
		light_load_features_threshold = SHIFT_EXPONENT_UNSIGNED(light_load_features_threshold, exp_diff);	// in iout format

		// avoiding case when light_load_features_threshold < half_of_hysteresis
		if (light_load_features_threshold > half_of_hysteresis)
		{
			user_data.light_load_features_threshold_low = light_load_features_threshold - half_of_hysteresis;
		}
		else
		{
			user_data.light_load_features_threshold_low = 0;
		}

		user_data.light_load_features_threshold_high = light_load_features_threshold + half_of_hysteresis;
		user_data.light_load_features_threshold_current = light_load_features_threshold;

	}
#endif
}

/*
 * Set desired light load mode
 * Set '1' - phase shedding mode is enabled or
 * '2' - diode emulation mode is enabled; or
 * '0' to use nothing
 *
 */

void PMBUS_HANDLE_MFR_LIGHT_LOAD_MODE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef light_load_features_en
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
	    // First, disable any active light load mode to get to a known state.
	    user_data.light_load_features_mode = NO_MODE;
	    if (user_data.light_load_features_initialization_status == 1)
        {
            light_load_features_enable(OFF);
        }
	    // Now, set the new value. Update will be done automatically by IRQ.
	    user_data.light_load_features_mode = (uint8_t)PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_LIGHT_LOAD_MODE);
		if (user_data.light_load_features_mode > 3u)
		{
			user_data.light_load_features_mode = NO_MODE;
		}
	}
#endif
}
