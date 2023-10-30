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
                // ARM::CMSIS:RTOS:Keil RTX
#include "uart_drv.h"
#include "user_app.h"

#include "telemetry_api.h"
#include "pwm_skip.h"
#include "light_load_features.h"


void pwm_skip(void)
{
	PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__SET(0x3FFu);
	user_data.pwm_skip_flag = 1;
}

void pwm_unskip(void)
{
	PWM_PWM_FORCE_HI_LO__PWM_FORCE_LO__SET(user_data.zvs_current_value); // remove all PWM LO force except ZVS
	user_data.pwm_skip_flag = 0;
}


void pwm_skip_irq_handle(PMBUS_PAGE_t loop)
{
	if (user_data.pwm_skip_max <= 1)
	{
		pwm_skip_irq_disable();
	}
	else
	{
		if (user_data.pwm_skip_counter >= user_data.pwm_skip_max-1)
		{
			user_data.pwm_skip_counter = 0;
			pwm_skip();
		}
		else
		{
			user_data.pwm_skip_counter++;
			if (user_data.pwm_skip_flag == 1)
			{
				pwm_unskip();
			}
		}
	}

}

void pwm_skip_irq_enable(void)
{
	Regulation_setup_fsw_irq(PMBUS_PAGE_0, fsw_irq_idx_4, user_data.pwm_skip_irq_rate);
	user_data.pwm_skip_counter = 0;
	pwm_skip();
}

void pwm_skip_irq_disable(void)
{
	Regulation_setup_fsw_irq(PMBUS_PAGE_0, fsw_irq_idx_4, fsw_irq_rate_sel_disabled);
	pwm_unskip();
}

/*
 * [7:4] - pwm_skip_max -> if 0, then pwm skip is disabled. if 3 - skip every 3rd (cycle * pwm_skip_irq_rate)
 * [3:0] - pwm_skip_irq_rate -> if 2 then fsw_irq = 2, or 3 => 4, 4 => 8, etc.
 */

void PMBUS_HANDLE_MFR_BURST_CONFIG(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if (direction == PMBUS_WRITE)
	{
		uint8_t irq_rate_sel = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_BURST_CONFIG);

		user_data.pwm_skip_max = (irq_rate_sel >> 4);

		irq_rate_sel &= 0xFu;
		if (irq_rate_sel > 0 && irq_rate_sel < 8)
		{
			user_data.pwm_skip_irq_rate = irq_rate_sel;
		}
		else {
			user_data.pwm_skip_irq_rate = 0x07;
		}
	}
}

