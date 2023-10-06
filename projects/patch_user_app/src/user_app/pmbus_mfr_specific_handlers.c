/*
 * pmbus_mfr_specific_handlers.c
 *
 *  Created on: Jan 24, 2017
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
//#include "user_app.h"
/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Used to configure the fans 1 and 2
 * The FAN_CONFIG_1_2 is used to configure up to two fans associated with one PMBus device.
 * The first part of the configuration byte tells the PMBus device whether or not a fan
 * associated with position 1 (or 2) is installed. Any combination of fan installation is
 * permitted (no fans, a fan in position 1 and no fan in position 2, no fan in position 1 and a
 * fan in position 2, fans in both positions).
 * The second part of the configuration tells the device whether the fan speed commands
 * are in RPM or PWM duty cycle (in percent). Section 14.11 describes the command for
 * setting fan speed. These settings do not have to be the same for Fan 1 and Fan 2.
 * The third part of the configuration data tells the PMBus device the number of tachometer
 * pulses per revolution each fan provides. This information is needed for commanding
 * and reporting fan speed in RPM. Two bits are provided for each fan. These settings do
 * not have to be the same for Fan 1 and Fan 2. The binary values of these bits map to
 * pulses per revolution as follows:
 * 00b = 1 pulse per revolution,
 * 01b = 2 pulses per revolution,
 * 10b = 3 pulses per revolution, and
 * 11b = 4 pulses per revolution.
 * This command has one data byte formatted as follows:
 * Table 12. FAN_CONFIG_1_2 Data Byte Format
 * Bit(s) Value Meaning
 * 7 1 A Fan Is Installed In Position 1
 * 0 No Fan Is Installed In Position 1
 * 6 1 Fan 1 Is Commanded In RPM
 * 0 Fan 1 Is Commanded In Duty Cycle
 * 5:4 00b-11b Fan 1 Tachometer Pulses Per Revolution
 * 3 1 A Fan Is Installed In Position 2
 * 0 No Fan Is Installed In Position 2
 * 2 1 Fan 2 Is Commanded In RPM
 * 0 Fan 2 Is Commanded In Duty Cycle
 * 1:0 00b-11b Fan 2 Tachometer Pulses Per Revolution
 *
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
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

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Byte format raw bit decode:
 * [7:4] > step rate.  The number of 64 fsw units to wait between steps
 * [3:0] > percentage. Percentage of frequency to modulate by. +/-15% max
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_MFR_FREQUENCY_DITHER(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		uint32_t data, percentage, freq_khz, period, delta;
		int32_t exponent;
		// update the user control data used by irq handle for feature
		user_data.control[page] = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_FREQUENCY_DITHER);
		percentage = user_data.control[page]&0xf;
		percentage = (uint32_t)SHIFT_EXPONENT((int32_t)percentage, (-PMBUS_Q_EXP));  // move to PMBUS_Q_EXP format
		percentage = percentage/100;  // convert from percentage to fractional number in PMBUS_Q_EXP format

		// grab the current frequency set point and calculate the min and max values
		// of the triangle wave
		data = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_FREQUENCY_SWITCH);
		freq_khz = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)data);
		exponent = LINEAR11_TO_EXPONENT((uint16_t)data);
		freq_khz = (uint16_t)SHIFT_EXPONENT((int32_t)freq_khz, (exponent));

		//delta = (uint32_t)SHIFT_EXPONENT((int32_t)(freq_khz*percentage), (exponent+PMBUS_Q_EXP));

		// convert frequency to period in ns for midpoint
		period = 1000000/(freq_khz*SWITCHING_PERIOD_LSB_NS);
		if (period > 0x1ff) period = 0x1ff;
		delta = (uint32_t)SHIFT_EXPONENT((int32_t)(period*percentage), (PMBUS_Q_EXP));

		user_data.period_max[page] = period + delta;
		if (user_data.period_max[page] > 0x1fff) user_data.period_max[page] = 0x1ff;
		user_data.period_min[page] = period - delta;
	}
}

/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * Word format raw bit decode:
 * [0] > trim_enable
 * [6:1] > mfr_vout_offset_trim[5:0] -40 to +38.75mV, LSB = 1.25mV, S6.0
 * [10:7]	mfr_lo_clk_trim_coarse[3:0]  S4.0, -12 to +10.5MHz, ~ +/- 5%
 * [15:11]	mfr_lo_clk_trim_fine[4:0] U5.0
 *
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void PMBUS_HANDLE_MFR_BOARD_TRIM(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		// FIXME: Add protection bypass here?
		uint32_t data = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_BOARD_TRIM);
		if ((data&1)!=0)  // if trim is enabled
		{
			// store the OTP value for future use if we are at OTP_RESTORE time
			// or if we got enable and the OTP_RESTORE had not occurred
			// in this scenario the current register value will be the "otp" value
			if ((direction == OTP_RESTORE) || (user_data.otp_osc_trim == 0))
			{
				// store otp value of vsp offset:
				if (page == PMBUS_PAGE_0)
				{
					user_data.otp_adc_offset[page] = (uint8_t)TRIM_VSP_OFF_TRIM__VSP0_ADC_OFFSET__GET();
				}
				else
				{
					user_data.otp_adc_offset[page] = (uint8_t)TRIM_VSP_OFF_TRIM__VSP2_ADC_OFFSET__GET();
				}
				user_data.otp_osc_trim = TRIM_LO_TRIM__GET();  // store otp value of osc trim
			}
			// vsp0_adc_offset_new[6:0] = trim.VSP0_TRIM.vsp0_adc_offset[6:0] + mfr_vout_offset_trim[5:0](S6.0)
			uint32_t offset = user_data.otp_adc_offset[page] + (uint32_t)(TWOS_UNCOMPLEMENT(6, ((data>>1)&0x3f)));
			if (page == PMBUS_PAGE_0)
				TRIM_VSP_OFF_TRIM__VSP0_ADC_OFFSET__SET(offset);
			else
				TRIM_VSP_OFF_TRIM__VSP2_ADC_OFFSET__SET(offset);

			// now do osc trim:
			// lo_clk_trim_coarse_new[6:0] = trim.OSCILLATOR.lo_clk_trim_coarse[6:0](U7.0) + mfr_lo_clk_trim_coarse[3:0](S4.0)
			uint32_t coarse = ((user_data.otp_osc_trim&TRIM_LO_TRIM__LO_CLK_TRIM_COARSE__Msk)
					>> TRIM_LO_TRIM__LO_CLK_TRIM_COARSE__Pos) + (uint32_t)(TWOS_UNCOMPLEMENT(4, ((data>>7)&0xf)));
			TRIM_LO_TRIM__LO_CLK_TRIM_COARSE__SET(coarse);  // set the register
			uint32_t fine = (data>>11)&0x1f;   //[15:11]	mfr_lo_clk_trim_fine[4:0] U5.0
			TRIM_LO_TRIM__LO_CLK_TRIM_FINE__SET(fine);
		}
		// FIXME: Restore protection here
	}
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

/*lint -e715 */
void PMBUS_HANDLE_MFR_FW_CONFIG_UART(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef uart_debug
	int32_t status;
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
		{
			uint32_t control, arg;
			uint8_t * data_p = PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_FW_CONFIG_UART);
			if (data_p!=NULL)
			{
				control = (uint32_t)data_p[3] << 24
						| (uint32_t)data_p[2] << 16
						| (uint32_t)data_p[1] << 8
						| (uint32_t)data_p[0];

				arg = (uint32_t)data_p[7] << 24
					| (uint32_t)data_p[6] << 16
					| (uint32_t)data_p[5] << 8
					| (uint32_t)data_p[4];
				status = UARTdrv->Control(control, arg);
				if (status != DRIVER_OK)
				{
					USER_Log_Error(PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
				}
				if (direction == PMBUS_WRITE)
				{
					status = UARTdrv->Receive(&dest_buf[0], N_ROWS);
					if (status != DRIVER_OK)
					{
						USER_Log_Error(PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
					}
				}
			}
		}


#endif
}
void PMBUS_HANDLE_MFR_SNAPSHOT_DATA(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{

}
/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
*/
void PMBUS_HANDLE_MFR_CNFG_DEBOUNCE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef DEBOUNCE_GLITCH
	uint8_t  config = PMBUS_Get_Command_Byte((PMBUS_PAGE_t)0, PMBUS_CMDCODE_MFR_CNFG_DEBOUNCE);
	uint32_t reg;

	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		if (config >= 7)
		{
			config = 7;
		}
		else if(config <= 1)
		{
			config = 1;
		}

		reg = PMBUS_CNFG__GET();
		reg = (reg & ~PMBUS_CNFG__DEBOUNCE_LENGTH__Msk);
		reg = (reg | (PMBUS_CNFG__DEBOUNCE_LENGTH__Shift((uint32_t)config)));  // LSB 40 ns

		if (config >3)
		{
			reg = (reg & ~PMBUS_CNFG__DS_FSM_FSM_CLK_G__Msk);
			reg = (reg | (PMBUS_CNFG__DS_FSM_FSM_CLK_G__Shift((uint32_t)1)));
		}
		PMBUS_CNFG__SET(reg);
	}
#endif
}

void PMBUS_HANDLE_MFR_DISABLE_I2C(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef JIRA_752_Disable_I2C_EN
	uint8_t input_data = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_DISABLE_I2C);
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		if (input_data==0)
		{
			PMBUS_ADDR_LUT__EN_ADDR__SET(pmbus_addr_lut_1, pmbus_addr_lut__en_addr_rw);  // enable i2c
		}
		else
		{
			PMBUS_ADDR_LUT__EN_ADDR__SET(pmbus_addr_lut_1, pmbus_addr_lut__en_addr_ds);   // disable i2c
		}
	}
#endif
}
