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
 *      Author: dlewis4
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
//extern ARM_DRIVER_USART * UARTdrv;
/*lint -e752 */
//extern  uint8_t  src_buf[N_ROWS];
//extern  uint8_t  dest_buf[N_ROWS];
/**
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_FAN_CONFIG_1_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef fan_en
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t data = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_FAN_CONFIG_1_2);
		COMMON_EN_REG3__EN_FAN1__SET(((data&0x80u)==0x80u) ? 1 : 0);  // bit 7 fan 1
		COMMON_EN_REG3__EN_FAN2__SET(((data&0x08u)==0x08u) ? 1 : 0);  // bit 3 fan 2
		FAN_FAN_CONFIG__FAN_TACH_PPR__SET(FAN1, (data&0x30u)>>4);
		FAN_FAN_CONFIG__FAN_TACH_PPR__SET(FAN2, (data&0x3u));
	}
#endif
}

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Linear11 format fan1 command of either duty or rpm depending on fan config
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_FAN_COMMAND_1(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef fan_en
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
#endif
}

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Linear11 format fan2 command of either duty or rpm depending on fan config
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_FAN_COMMAND_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef fan_en
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
#endif
}


/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Current sharing threshold in linear11 format Amps resolution.
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_MFR_ISHARE_THRESHOLD(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t data = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD);
		int32_t exponent = LINEAR11_TO_EXPONENT((uint16_t)data);
		data = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)data);  // grab mantissa
		uint32_t cpa = COMMON_ISHARE__ISHR_SCALE__GET();  // cpa is codes per amp

		data = (uint32_t)SHIFT_EXPONENT((int32_t)(data * cpa), exponent);  // multiply together and shift back down by exponent to get to exp=0
		// set the adc code absolute value for hysteresis
		if (data>0xff) data = 0xff;  // cap at register max 255 codes
		COMMON_ISHARE_DZ__ISHARE_DEAD_ZONE__SET(data);
	}

}

/*lint -e715 */
void PMBUS_HANDLE_MFR_ADDED_DROOP_DURING_RAMP(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	// update only if a read:
	if(direction==PMBUS_WRITE)
	{

	}
}

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


void PMBUS_HANDLE_MFR_IOUT_OFFSET_CORR_SLOPE_FACTOR(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		user_data.iout_telem_offset_correction_slope_factor = (PMBUS_Get_Command_Word(page,
				PMBUS_CMDCODE_MFR_IOUT_OFFSET_CORR_SLOPE_FACTOR));
	}
}

void PMBUS_HANDLE_MFR_IOUT_OFFSET_CORR_FACTOR(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		user_data.iout_telem_offset_correction_factor = (PMBUS_Get_Command_Word(page,
				PMBUS_CMDCODE_MFR_IOUT_OFFSET_CORR_FACTOR));
	}
}







