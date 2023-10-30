/* ============================================================================
** Copyright (C) 2020 Infineon. All rights reserved.
**               Infineon Technologies, PSS SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorization.
** ============================================================================
**
** Created on:  2020-11-12
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

#ifdef zvs_feature_en

#include "zvs_feature.h"
#include "telemetry_api.h"


/*
 * ZVS feature initialization at AT_TARGET_ENABLE
 */
//void iout_zvs_threshold_irq_handle_enable(uint16_t threshold )
void iout_zvs_threshold_irq_handle_enable(void)
{
	user_data.zvs_initialization_status = 0;

	iout_telemetry_get_high_frequence_limited(0);

	uint16_t iout_local = 0;

	if (user_data.iout[PMBUS_PAGE_0] > 0)
	{
		iout_local = (uint16_t)user_data.iout[PMBUS_PAGE_0];
	}

	if (iout_local < user_data.iout_zvs_threshold_current)
	{
		zvs_enable(OFF);
	}
	else
	{
		zvs_enable(ON);
	}

	// no IRQ enabling here, moved to feed-forward IRQ

	user_data.zvs_initialization_status = 1;

}

void iout_zvs_threshold_irq_handle_disable(void)
{
    // no IRQ disabling here, moved to feed-forward IRQ

    zvs_enable(ON); //enable the ZVS MOSFETs
    user_data.zvs_initialization_status = 0;
}

/*
 * iout_zvs_threshold_irq_handle() is
 * hysteresis based ZVS threshold IRQ callback
 * to enable / disable ZVS.
 * If iout < iout_zvs_threshold, then ZVS -> OFF
 * else ZVS -> ON
 */

void iout_zvs_threshold_irq_handle(PMBUS_PAGE_t loop)
{
	uint16_t iout_local = 0;

	if (user_data.iout[PMBUS_PAGE_0] > 0)
	{
		iout_local = (uint16_t)user_data.iout[PMBUS_PAGE_0];
	}

	if (iout_local < user_data.iout_zvs_threshold_current)
	{
		if (user_data.zvs_status == ON)
		{
			zvs_enable(OFF);
		}
	}
	else
	{
		if (user_data.zvs_status == OFF)
		{
			zvs_enable(ON);
		}
	}
}

/*
 * zvs_enable() does en/dis pwm3 and pwm4 once output
 * load exceed ZVS threshold with 1 A hysteresis
 */

void zvs_enable(mode_t mode)	// ON / OFF
{
	/*
	 * forcing PWM3 and PWM4 to LOW
	 */
	uint32_t LO = PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__GET();
	if (mode == ON)
	{
        if ( (LO & 3u) != 3u) // enable phase 2 ZVS MOSFET only if phase 2 high and low MOSFETs are not disabled (no phase shedding)
        {
            LO &= ~(4u);
        }
		LO &= ~(8u);// enable phase 1 ZVS MOSFET always
		user_data.zvs_current_value = 0; //this always has to be 0u, so that both ZVS MOSFETs are enabled if phase shedding is deactivated
        user_data.iout_zvs_threshold_current = user_data.iout_zvs_threshold_low;
//		user_data.debug4 = 1;
	}
	else
	{
		LO |= 12u;
		user_data.zvs_current_value = 12u;
        user_data.iout_zvs_threshold_current = user_data.iout_zvs_threshold_high;
//		user_data.debug4 = 0;
	}
	PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__SET(LO);
    user_data.zvs_status = mode;

}
#endif


/*
 * ZVS feature MFR handler to set threshold and update hysteresis settings
 */

void PMBUS_HANDLE_MFR_ZVS_DISABLE_THRESHOLD(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef zvs_feature_en
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint16_t iout_zvs_threshold = LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_ZVS_DISABLE_THRESHOLD));
		user_data.iout_zvs_threshold_exp = (int8_t)LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_ZVS_DISABLE_THRESHOLD));

		// Hysteresis levels updates:
		int8_t exp_diff;
		int16_t iout_zvs_hysteresis;

		exp_diff = user_data.iout_zvs_hysteresis_exp - user_data.iout_exp[PMBUS_PAGE_0];
		iout_zvs_hysteresis = SHIFT_EXPONENT_UNSIGNED(user_data.iout_zvs_hysteresis, exp_diff);	// in iout format
		uint8_t half_of_hysteresis = iout_zvs_hysteresis >> 1;

		exp_diff = user_data.iout_zvs_threshold_exp - user_data.iout_exp[PMBUS_PAGE_0];
		iout_zvs_threshold = SHIFT_EXPONENT_UNSIGNED(iout_zvs_threshold, exp_diff);	// in iout format

		// avoiding case when iout_zvs_threshold < half_of_hysteresis
		if (iout_zvs_threshold > half_of_hysteresis)
		{
			user_data.iout_zvs_threshold_low = iout_zvs_threshold - half_of_hysteresis;
		}
		else
		{
			user_data.iout_zvs_threshold_low = 0;
		}
		user_data.iout_zvs_threshold_high = iout_zvs_threshold + half_of_hysteresis;
		user_data.iout_zvs_threshold_current = iout_zvs_threshold;

        // if threshold == 0, IRQ will not do the update. Therefore do it now.
		if (user_data.zvs_initialization_status == 1)
		{
			iout_zvs_threshold_irq_handle(page);
		}
	}
#endif
}

