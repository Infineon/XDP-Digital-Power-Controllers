/*
 * pmbus_mfr_specific_handlers.c
 */

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
//#include "regulation_state_machine_callbacks.h"
#include "add_on_features.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "uart_drv.h"
#include "user_app.h"

void PMBUS_HANDLE_MFR_VDD_SCALE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t scale = (PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VDD_SCALE));
		REGULATION_APP.vdd_scale_factor[page] = scale;
	}
}

void PMBUS_HANDLE_MFR_VIN_SCALE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t scale = (PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VIN_SCALE));
		REGULATION_APP.vin_scale_factor[page] = scale;
	}
}

void PMBUS_HANDLE_FAN_CONFIG_1_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t data = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_FAN_CONFIG_1_2);
		COMMON_EN_REG3__EN_FAN1__SET(((data&0x80u)==0x80u) ? 1 : 0);  // bit 7 fan 1
		COMMON_EN_REG3__EN_FAN2__SET(((data&0x08u)==0x08u) ? 1 : 0);  // bit 3 fan 2
		FAN_FAN_CONFIG__FAN_TACH_PPR__SET(FAN1, (data&0x30u)>>4);
		FAN_FAN_CONFIG__FAN_TACH_PPR__SET(FAN2, (data&0x3u));
	}
}

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Linear11 format fan1 command of either duty or rpm depending on fan config
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_FAN_COMMAND_1(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t config = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_FAN_CONFIG_1_2);
		uint32_t data = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_FAN_COMMAND_1);
		int32_t exponent = LINEAR11_TO_EXPONENT((uint16_t)data);
		data = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)data);
		//RGU_SPARE_FF__SET(0xacd);
		//RGU_SPARE_FF__SET(data);
		//RGU_SPARE_FF__SET(config);

		if ((config&0x40u)==0x40u)  // fan1 is commanded in RPM
		{
			/*
			 * fan_speed[15:0]: Fan speed in rotations per minute (RPM) as measured at the fan_tach_i input
			 * (U16.0, range = 0 to 65535 RPM)
			 */
			// use a linear relationship between 0-65535 rpm assume duty is linear
			// note that fans are not linear and this relationship is loose
			// duty*65535 ~= rpm
			data = (uint32_t)SHIFT_EXPONENT((int32_t)data,(exponent-PMBUS_Q_EXP));  // convert to PMBUS_Q_EXP for division
			data = (uint32_t)SHIFT_EXPONENT((int32_t)(data/65535),(PMBUS_Q_EXP+8));  // convert to -8 format for register
			//RGU_SPARE_FF__SET(0xbabe);
			//RGU_SPARE_FF__SET(data);
			FAN_FAN_DUTY__SET(FAN1, data);

		}
		else
		{
			/*
			fan_duty[7:0]: Fan commanded duty cycle when operating in duty cycle mode (U0.8). Note fan_duty=0xFF overrides to 100%.
			Computed from PMBus command as follows:
			fan_duty(U0.8) = FAN_COMMAND(U6.2) * 256 / 400
			*/
			data = (uint32_t)SHIFT_EXPONENT((int32_t)data,(exponent+8));  // convert to U0.8
			//RGU_SPARE_FF__SET(0xbabe);
			//RGU_SPARE_FF__SET(data);
			if (data>0xff) data = 0xff; // cap at max
			FAN_FAN_DUTY__SET(FAN1, data);

		}
	}
}

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Linear11 format fan2 command of either duty or rpm depending on fan config
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_FAN_COMMAND_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t config = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_FAN_CONFIG_1_2);
		uint32_t data = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_FAN_COMMAND_2);
		int32_t exponent = (int32_t)LINEAR11_TO_EXPONENT((uint16_t)data);
		data = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)data);
		//RGU_SPARE_FF__SET(0xdac);
		//RGU_SPARE_FF__SET(data);
		//RGU_SPARE_FF__SET(config);

		if ((config&0x4u)==0x4u)  // fan2 is commanded in RPM
		{
			/*
			 * fan_speed[15:0]: Fan speed in rotations per minute (RPM) as measured at the fan_tach_i input
			 * (U16.0, range = 0 to 65535 RPM)
			 */
			// use a linear relationship between 0-65535 rpm assume duty is linear
			// note that fans are not linear and this relationship is loose
			// duty*65535 ~= rpm
			data = (uint32_t)SHIFT_EXPONENT((int32_t)data,(exponent-PMBUS_Q_EXP));  // convert to PMBUS_Q_EXP for division
			data = (uint32_t)SHIFT_EXPONENT((int32_t)(data/65535),(PMBUS_Q_EXP+8));  // convert to -8 format for register
			//RGU_SPARE_FF__SET(0xbabe);
			//RGU_SPARE_FF__SET(data);
			FAN_FAN_DUTY__SET(FAN2, data);

		}
		else
		{
			/*
			fan_duty[7:0]: Fan commanded duty cycle when operating in duty cycle mode (U0.8). Note fan_duty=0xFF overrides to 100%.
			Computed from PMBus command as follows:
			fan_duty(U0.8) = FAN_COMMAND(U6.2) * 256 / 400
			*/
			data = (uint32_t)SHIFT_EXPONENT((int32_t)data,(exponent+8));  // convert to U0.8
			//RGU_SPARE_FF__SET(0xbabe);
			//RGU_SPARE_FF__SET(data);
			if (data>0xff) data = 0xff; // cap at max
			FAN_FAN_DUTY__SET(FAN2, data);

		}
	}
}

void patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	PMBUS_HANDLE_MFR_FIRMWARE_COMMAND(page, direction); // first call the ROM handle

	// Now add new code here:
	if (direction == PMBUS_WRITE)
	{
		// get the command code here:
		uint8_t command = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_FIRMWARE_COMMAND);
		//uint32_t input_data = PMBUS_Get_Command_Dword(page, PMBUS_CMDCODE_MFR_FIRMWARE_COMMAND_DATA);
		uint32_t output_data = 0;  // if command doesn't exist we write command_data to 0

		// here we assume there are no other commands added by patch so first
		// free command code is PMBUS_MFR_FIRMWARE_COMMAND_END_OF_ENUM
		if((PMBUS_MFR_FIRMWARE_COMMAND_SET_e)command  == PMBUS_MFR_FIRMWARE_COMMAND_END_OF_ENUM)
		{
			output_data = 0;
			output_data |= (uart_info.status.tx_busy &1) << 0;
			output_data |= (uart_info.status.rx_overflow &1) << 3;
			output_data |= (uart_info.status.rx_break &1) << 4;
			output_data |= (uart_info.status.rx_framing_error &1) << 5;
			output_data |= (uart_info.status.rx_parity_error &1) << 6;

			// set the output data in MFR_FIRMWARE_COMMAND_DATA
			PMBUS_Set_Command_Dword((PMBUS_PAGE_W_BROADCAST_t)page, PMBUS_CMDCODE_MFR_FIRMWARE_COMMAND_DATA, output_data);
		}

	}
}
