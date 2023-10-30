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
 *      Author: kotvytsr
 */

#include "buck_boost.h"

#include "shasta_hal_telem.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_tsen.h"
#include "pmbus_mfr_autogen.h"

#include "telemetry_api.h"
#include "regulation_api.h"
#include "faults_drv.h"
//#include "stdint.h"

#include "regulation_state_machine_callbacks.h"
#include "light_load_features.h"
#include "zvs_feature.h"

#include "pmbus_api.h"

#ifdef eff_table_feature_en

#include "efficiency_table.h"

/* Helper function to calculate hysteresis threshold */

uint8_t calculate_i(uint32_t vin)
{
	uint8_t i;

	// triggering for unsign format
	if(user_data.vin_p1_adjust <= 0 || user_data.vin_p1_adjust > 0xAA)
	{
		user_data.vin_p1_adjust = vin_table[0];
	}
	if(user_data.vin_p2_adjust <= 0 || user_data.vin_p2_adjust > 0xAA)
	{
		user_data.vin_p2_adjust = vin_table[1];
	}

	if(vin <= user_data.vin_p1_adjust)
	{
		user_data.vin_p1_adjust = vin_table[0] + user_data.vin_hysteresis;
		user_data.vin_p2_adjust = vin_table[1] + user_data.vin_hysteresis;
		i = 0;
	}
	else if(vin <= user_data.vin_p2_adjust)
	{
		user_data.vin_p1_adjust = vin_table[0] - user_data.vin_hysteresis;
		user_data.vin_p2_adjust = vin_table[1] + user_data.vin_hysteresis;
		i = 1;
	}
	else
	{
		user_data.vin_p1_adjust = vin_table[0] - user_data.vin_hysteresis;
		user_data.vin_p2_adjust = vin_table[1] - user_data.vin_hysteresis;
		i = 2;
	}

	return i;
}


/*
 * Function below represents output current selection
 * from the iout_table with hardcode of the hysteresis
 */

uint8_t calculate_j(int32_t iout)
{
	uint8_t j;

	// triggering for unsign format
	if(user_data.iout_p1_adjust <= 0 || user_data.iout_p1_adjust > 0xAA)
	{
		user_data.iout_p1_adjust = iout_table[0];
	}
	if(user_data.iout_p2_adjust <= 0 || user_data.iout_p2_adjust > 0xAA)
	{
		user_data.iout_p2_adjust = iout_table[1];
	}
	if(user_data.iout_p3_adjust <= 0 || user_data.iout_p3_adjust > 0xAA)
	{
		user_data.iout_p3_adjust = iout_table[2];
	}
    if(user_data.iout_p4_adjust <= 0 || user_data.iout_p4_adjust > 0xAA) //220504 changes
    {
        user_data.iout_p4_adjust = iout_table[3];
    }

	if(iout <= user_data.iout_p1_adjust)
	{
		user_data.iout_p1_adjust = iout_table[0] + user_data.iout_hysteresis;
		user_data.iout_p2_adjust = iout_table[1] + user_data.iout_hysteresis;
		user_data.iout_p3_adjust = iout_table[2] + user_data.iout_hysteresis;
        user_data.iout_p4_adjust = iout_table[3] + user_data.iout_hysteresis;
		j = 0;
	}
	else if(iout <= user_data.iout_p2_adjust)
	{
		user_data.iout_p1_adjust = iout_table[0] - user_data.iout_hysteresis;
		user_data.iout_p2_adjust = iout_table[1] + user_data.iout_hysteresis;
		user_data.iout_p3_adjust = iout_table[2] + user_data.iout_hysteresis;
        user_data.iout_p4_adjust = iout_table[3] + user_data.iout_hysteresis;
		j = 1;
	}
	else if(iout <= user_data.iout_p3_adjust)
	{
		user_data.iout_p1_adjust = iout_table[0] - user_data.iout_hysteresis;
		user_data.iout_p2_adjust = iout_table[1] - user_data.iout_hysteresis;
		user_data.iout_p3_adjust = iout_table[2] + user_data.iout_hysteresis;
        user_data.iout_p4_adjust = iout_table[3] + user_data.iout_hysteresis;
		j = 2;
	}
    else if(iout <= user_data.iout_p4_adjust) //220504 changes
    {
        user_data.iout_p1_adjust = iout_table[0] - user_data.iout_hysteresis;
        user_data.iout_p2_adjust = iout_table[1] - user_data.iout_hysteresis;
        user_data.iout_p3_adjust = iout_table[2] - user_data.iout_hysteresis;
        user_data.iout_p4_adjust = iout_table[3] + user_data.iout_hysteresis;
        j = 3;
    }
	else
	{
		user_data.iout_p1_adjust = iout_table[0] - user_data.iout_hysteresis;
		user_data.iout_p2_adjust = iout_table[1] - user_data.iout_hysteresis;
		user_data.iout_p3_adjust = iout_table[2] - user_data.iout_hysteresis;
        user_data.iout_p4_adjust = iout_table[3] - user_data.iout_hysteresis; //220504 changes
		j = 4;
	}

	return j;
}


/* Function to adjust input current according to efficiency table */

uint32_t input_current_correction(int8_t vin_read_exp, int32_t OutputCurrent, int8_t iout_read_exp, int8_t iin_read_exp, int8_t power_read_exp, int8_t loop, uint32_t vin, uint32_t pout)
{

	int32_t iout = SHIFT_EXPONENT_UNSIGNED( OutputCurrent , iout_read_exp + 2 );			// to get integer (uX.2 format) real value of Output Current => LSB = 0.25

	uint32_t vin_read = SHIFT_EXPONENT_UNSIGNED( vin , vin_read_exp );	// to get Input Voltage in u7.0 format

	uint8_t i = calculate_i(vin_read);		// condition searching
	uint8_t j = calculate_j(iout);

	uint8_t efficiency = efficiency_table[i][j];		// U0.8 format

	uint32_t prod_vin_eff;
	prod_vin_eff = vin*efficiency;	//u7.vin_exp*u0.8 = u7.8
	prod_vin_eff = SHIFT_EXPONENT_UNSIGNED( prod_vin_eff  , vin_read_exp );   // u7.8

	uint32_t one_div_prod_vin_eff;

	one_div_prod_vin_eff = (1U<<24) / (prod_vin_eff);  //24 also includes the 2^8 accounted form efficiency, so this will be X.16

	uint32_t prod_pout_one_div_vin_eff;

	uint16_t reqrd_shift = 16;
	uint16_t iin_exp_shift = 0;

	if(iin_read_exp > 0x80)
	{
	iin_exp_shift = (0x100 - iin_read_exp); // 128 is a dummy number to avoid implementing this for positive exp
	reqrd_shift = (reqrd_shift - iin_exp_shift);
	}
	else
	{
	iin_exp_shift = iin_read_exp; // 128 is a dummy number to avoid implementing this for positive exp
	reqrd_shift = reqrd_shift + iin_exp_shift;
	}
	prod_pout_one_div_vin_eff = one_div_prod_vin_eff*pout;
	prod_pout_one_div_vin_eff = prod_pout_one_div_vin_eff >> reqrd_shift;   //for Iin exp = -2, must be right shifted by 14, shift based on exp selected
//	uint16_t iin_corr = 0x1FFF;
	uint32_t iin_corr = prod_pout_one_div_vin_eff;

	PMBUS_Set_Command_Byte((PMBUS_PAGE_W_BROADCAST_t) loop, PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY, efficiency);		// set value of efficiency to MFR_ESTIMATE_EFFICIENCY

	if (iin_corr>0x3FF) iin_corr = 0x3FF;

	return iin_corr; 	// max absolute value = 1023

}

void patch_Telemetry_Sample(void)
{
    /*python automated patch entry*/
//    VOID_PATCH_TABLE_RUN(Telemetry_Sample)

	uint8_t loop;
	uint32_t vout, vin, iin, pin, pout, duty, fsw;

	int8_t vout_read_exp[NUM_LOOPS];
	int8_t vin_read_exp[NUM_LOOPS];
	int8_t iout_read_exp[NUM_LOOPS];
	int8_t iin_read_exp[NUM_LOOPS];
	int8_t duty_read_exp[NUM_LOOPS];
	int8_t power_read_exp[NUM_LOOPS];
	int8_t fan_read_exp[NUM_LOOPS];
	int8_t fsw_read_exp[NUM_LOOPS];

	int32_t iout;
	int32_t tempa, tempb, tempi;
	int32_t temp_1, temp_2;
	// update the telemetry data at the bandwidth period:

//	for(loop=0; loop<(uint8_t)NUM_LOOPS; loop++)  // update for the number of loops in the system
//	{
	//==================loop update START
	loop = PMBUS_PAGE_0;	// assuming we have only 1 regulation loop
	//GET_EXPONENTS
		//int32_t vout_exp = PMBUS_Q_EXP;
		vout_read_exp[loop] = (int8_t)PMBUS_Get_Vout_Mode_Exponent((PMBUS_PAGE_t)loop);
		fsw_read_exp[loop]  = (int8_t)LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_FREQUENCY_SWITCH));
		vin_read_exp[loop]  = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_VIN_EXP));
		iout_read_exp[loop] = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IOUT_EXP));
		iin_read_exp[loop]  = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IIN_EXP));
		duty_read_exp[loop] = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_DUTY_EXP));
		power_read_exp[loop]= (int8_t)TWOS_UNCOMPLEMENT(3, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_POWER_EXP));
		fan_read_exp[loop]  = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)PMBUS_PAGE_0, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_FAN_EXP));
	//VOUT_TELEM
		vout = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, VOUT_TELEM);
		vout = (uint32_t)SHIFT_EXPONENT((int32_t)vout,  PMBUS_Q_EXP - vout_read_exp[loop]);  // convert to vout_mode format
		if (vout>0xFFFF) vout = 0xFFFF;
		TELEM_DATA.vout[loop] = vout;
	//IOUT_TELEM
		iout = Telemetry_get((PMBUS_PAGE_t)loop, IOUT_TELEM);
		if (iout>1023) iout = 1023;
		if (iout<-1023) iout = -1023;

		TELEM_DATA.iout[loop] = iout;
	//VIN_TELEM
		vin = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, VIN_TELEM);
		if (vin>0x3FF) vin = 0x3FF;
		TELEM_DATA.vin[loop] = vin;

		// for 2v9 modification
		user_data.vin_telem_in_iout_exp = SHIFT_EXPONENT_UNSIGNED(vin, vin_read_exp[loop] - iout_read_exp[loop]);

	//DUTY_TELEM
		duty = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, DUTY_TELEM);
		if (duty>0x3FF) duty = 0x3FF;
		TELEM_DATA.duty[loop] = duty;
	//FSW_TELEM
		fsw = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, FSW_TELEM);
		if (fsw>0x3FF)  fsw = 0x3FF;
		TELEM_DATA.fsw[loop] = fsw;
	//POUT_TELEM
		pout = (uint32_t)Telemetry_get_power(vout, vout_read_exp[loop], iout, iout_read_exp[loop], power_read_exp[loop]); //in read_power_exp
		if (pout>0x3ff) pout=0x3ff;
		TELEM_DATA.pout[loop] = pout;

//----------------------patch modification Start
	//IIN_TELEM updated calculation
//		iin = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, IIN_TELEM);

		iin = (uint32_t)input_current_correction(vin_read_exp[loop], iout, iout_read_exp[loop], iin_read_exp[loop], power_read_exp[loop], loop, vin, pout);	// correction of Input Current
		if (iin>0x3FF) iin = 0x3FF;
		TELEM_DATA.iin[loop] = iin;

	//PIN_TELEM put after IIN calculation
		pin = (uint32_t)Telemetry_get_power(vin, vin_read_exp[loop], (int32_t)iin, iin_read_exp[loop], power_read_exp[loop]);
		if (pin > 0x3ff) pin = 0x3ff;  // cap value
		TELEM_DATA.pin[loop] = pin;
//----------------------patch modification End

		if (Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_FW_FILTER_EN) == 1) {
			vout = (vout + (uint32_t)(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_VOUT)))>>1;
			/*lint -e702 */
			iout = (iout + (LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_IOUT))))>>1u;
			vin  = (vin  + (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_VIN))))>>1u;
			iin  = (iin  + (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_IIN))))>>1u;
			duty = (duty + (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_DUTY_CYCLE))))>>1u;
			fsw  = (fsw +  (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_FREQUENCY))))>>1u;
		}

		TELEMETRY_VIN_SOURCE source = Telemetry_get_vin_source((PMBUS_PAGE_t)loop);
		if ((ON_OFF_STATE.On_Off_State[loop]==0)&&
				((source == (TELEMETRY_VIN_SOURCE)TELEMETRY_VIN_SOURCE_VRECT1) || (source == (TELEMETRY_VIN_SOURCE)TELEMETRY_VIN_SOURCE_VRECT2)))
		{
			// if we have vrect sensing and not actively switching we need to set read_vin to 0v
			PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_VIN, (uint16_t)(((TWOS_COMPLEMENT (5, vin_read_exp[loop]))<<11)));
		}
		else
		{
			// otherwise we have real telemetry for vin from primary or forced values
			PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_VIN, (uint16_t)(((TWOS_COMPLEMENT (5, vin_read_exp[loop]))<<11) | (vin & 0x7ff))); // read_vin is u7.2
		}

		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_VOUT,      (uint16_t)vout);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_IOUT,      (uint16_t)( (TWOS_COMPLEMENT (5, iout_read_exp[loop] )<<11) | ((uint16_t)iout & 0x7ff))); // read_iout is s7.2
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_IIN,       (uint16_t)( (TWOS_COMPLEMENT (5, iin_read_exp[loop]  )<<11) | (iin  & 0x7ff))); // read_iin is u6.4
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_POUT,      (uint16_t)( (TWOS_COMPLEMENT (5, power_read_exp[loop])<<11) | (pout & 0x7ff))); // fixed at u10.0 format
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_PIN,       (uint16_t)( (TWOS_COMPLEMENT (5, power_read_exp[loop])<<11) | (pin  & 0x7ff)));
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_DUTY_CYCLE,(uint16_t)( (TWOS_COMPLEMENT (5, duty_read_exp[loop] )<<11) | (duty & 0x7ff)));  // u7.3 %
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_FREQUENCY, (uint16_t)( (TWOS_COMPLEMENT (5, fsw_read_exp[loop]  )<<11) | (fsw  & 0x7ff)));  // Return READ_FREQUENCY in FREQUENCY_SWITCH format

		//GET_POWER_Thresholds
			int32_t exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_POUT_OP_WARN_LIMIT));
			int16_t mantissa = (int16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_POUT_OP_WARN_LIMIT));
			POWER_WARN_THRESH[loop].pout_op_warn_thresh = (uint16_t) SHIFT_EXPONENT(mantissa, (- power_read_exp[loop] + exponent));
			exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_PIN_OP_WARN_LIMIT));
			mantissa = (int16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_PIN_OP_WARN_LIMIT));
			POWER_WARN_THRESH[loop].pin_op_warn_thresh = (uint16_t) SHIFT_EXPONENT(mantissa, (- power_read_exp[loop] + exponent));

		//POUT_OP Firmware Fault
		if (pout>POWER_WARN_THRESH[loop].pout_op_warn_thresh)
		{
			Set_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_POUT_OP_WARN);
		}
		else
		{
			Clear_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_POUT_OP_WARN);
		}

		//PIN_OP Firmware Fault
		if (pin>POWER_WARN_THRESH[loop].pin_op_warn_thresh)
		{
			Set_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_PIN_OP_WARN);
		}
		else
		{
			Clear_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_PIN_OP_WARN);
		}

//	} //==================loop update END

  //ATSEN_TELEM
	tempa = Telemetry_get(PMBUS_PAGE_0, ATSEN_TELEM);
	TELEM_DATA.temperature[0] = tempa;
  //BTSEN_TELEM
	tempb = Telemetry_get(PMBUS_PAGE_0, BTSEN_TELEM);
	TELEM_DATA.temperature[1] = tempb;
  //ITSEN_TELEM
	tempi = Telemetry_get_tempi();
	TELEM_DATA.temperature[2] = tempi;
	//tempa = Telemetry_get((PMBUS_PAGE_t)loop, ITSEN_TELEM);

	// Here we determine which READ_TEMPERATURE gets tempA, tempB, and tempI
	for(loop=0; loop<NUM_LOOPS; loop++)
	{
		switch ((PMBUS_Get_Command_Byte ((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_MFR_SELECT_TEMPERATURE_SENSOR)) & 0x7 )
		{
			case 0:
				temp_1 = tempa;
				temp_2 = tempb;
				break;
			case 1:
				temp_1 = tempa;
				temp_2 = tempi;
				break;
			case 2:
				temp_1 = tempb;
				temp_2 = tempa;
				break;
			case 3:
				temp_1 = tempb;
				temp_2 = tempi;
				break;
			case 4:
				temp_1 = tempi;
				temp_2 = tempa;
				break;
			case 5:
				temp_1 = tempi;
				temp_2 = tempb;
				break;
			default:
				temp_1 = tempa;
				temp_2 = tempb;
				break;
		}

		if (Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_FW_FILTER_EN) == 1) {
			/*lint -e702 */
			temp_1 = (temp_1 + (LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_1))))>>1u;
			/*lint -e702 */
			temp_2 = (temp_2 + (LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_2))))>>1u;
		}

		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_1, ((uint16_t)temp_1 & 0x7ff));
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_2, ((uint16_t)temp_2 & 0x7ff));
	}

	uint16_t fan_speed1 = Telemetry_get_fan_speed(FAN1);
	uint16_t fan_speed2 = Telemetry_get_fan_speed(FAN2);

	// Get fan speed, replicate on both loops
	// since the bottom 8 lsb's of fan_speed register are always zero we shift those off and use +6 exponent to get 64 to 65k rpm
	int32_t temp = SHIFT_EXPONENT(fan_speed1, -(fan_read_exp[PMBUS_PAGE_0]));
	if (temp > 0x3ff) temp = 0x3ff;  // clamp fan speed at mantissa size
	PMBUS_Set_Command_Word(PMBUS_PAGE_WB_BROADCAST, (uint8_t)PMBUS_CMDCODE_READ_FAN_SPEED_1, (uint16_t)(((TWOS_COMPLEMENT (5, fan_read_exp[PMBUS_PAGE_0]))<<11)
			| temp));
	temp = SHIFT_EXPONENT(fan_speed2, -(fan_read_exp[PMBUS_PAGE_0]));  // shift the fan speed by the exponent
	if (temp > 0x3ff) temp = 0x3ff;  // clamp fan speed at mantissa size
	PMBUS_Set_Command_Word(PMBUS_PAGE_WB_BROADCAST, (uint8_t)PMBUS_CMDCODE_READ_FAN_SPEED_2, (uint16_t)(((TWOS_COMPLEMENT (5, fan_read_exp[PMBUS_PAGE_0]))<<11)
			| temp));

	call_system_cb(TELEMETRY_SAMPLE);

	user_data.en_buck_boost_feed_forward = PMBUS_Get_Command_Byte(0, PMBUS_CMDCODE_MFR_EN_BUCK_BOOST_FEED_FORWARD);
}

#endif


#ifdef iout_correction_feature_en

int32_t patch_Telemetry_get(PMBUS_PAGE_t loop, TELEMETRY_TYPE_t telem_type)
{

	int32_t telem = 0;
    uint32_t telem_unsigned = 0;
	int32_t gain;
	int32_t offset;
	uint32_t vout_scale;
	int32_t Telemetry_exponent;
	int32_t read_exp;

	switch (telem_type)
	{
		case VOUT_TELEM:
			telem = (int32_t)TELEM_TLM_VOUT_FSW__GET((telem_idx_t)loop);  // lsb = 1.25mv u12.0
			//debug[0] = 0xAA;
			Telemetry_exponent = Telemetry_VOUT_EXP;
			read_exp = PMBUS_Q_EXP;
			break;
		case IOUT_TELEM :

			user_data.fs = TELEM_TLM_FREQ__TLM_FREQ__GET((telem_idx_t)loop);  // u11.0 format 1khz LSB
			user_data.pwm1_deadtime_rise = PWM_PWM1_DEADTIME__PWM1_DR__GET();		// u6.2	or 	LSB = 1.25ns
			user_data.pwm1_deadtime_fall = PWM_PWM1_DEADTIME__PWM1_DF__GET();		// u6.2	or 	LSB = 1.25ns

			if (REGULATION_APP.current_state[loop] != REGULATION_STATE_OFF)
			{
				telem = iout_telemetry_get_high_frequence((telem_idx_t)loop);

			}

			// additional iout update for light load features
			int32_t temp = telem;
			if (temp>1023) temp = 1023;
			if (temp<-1023) temp = -1023;
			user_data.iout[loop] = (int16_t)temp;

			Telemetry_exponent = Telemetry_IOUT_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IOUT_EXP));
			user_data.iout_exp[loop] = read_exp;
			break;
		case VIN_TELEM :
			telem = (int32_t)TELEM_TLM_VIN_LPF__GET((telem_idx_t)loop); //u7.4
			Telemetry_exponent = Telemetry_VIN_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_VIN_EXP));
			break;
		case IIN_TELEM :
			telem = (int32_t)TELEM_TLM_IIN_LPF__GET((telem_idx_t)loop);
			Telemetry_exponent = Telemetry_IIN_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IIN_EXP));
			break;
		case ATSEN_TELEM :
			telem = (int32_t)TLMCOM_TLM_ATSEN_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case BTSEN_TELEM :
			telem = (int32_t)TLMCOM_TLM_BTSEN_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case ITSEN_TELEM :
			telem = (int32_t)TLMCOM_TLM_ITSEN_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case XADDR1_TELEM :
			telem = (int32_t)TLMCOM_TLM_XADDR1_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case XADDR2_TELEM :
			telem = (int32_t)TLMCOM_TLM_XADDR2_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case DUTY_TELEM:
			telem = (int32_t)TELEM_TLM_DUTY_FSW__GET((telem_idx_t)loop);  // u0.16
			telem = telem*100;  // convert to percent in u0.16 format
			Telemetry_exponent = Telemetry_DUTY_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_DUTY_EXP));
			break;
		case FSW_TELEM :
			telem = (int32_t)TELEM_TLM_FREQ__GET((telem_idx_t)loop);  // u11.0 format 1khz lsb
			Telemetry_exponent = Telemetry_FSW_EXP;
			read_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_FREQUENCY_SWITCH));
			break;
		default:
			telem = 0;  // u11.0 format 1khz lsb
			read_exp = 0;
			Telemetry_exponent = 0;
			PMBUS_Log_Error(TELEMETRY, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
			break;
	}


	if (telem_type == VOUT_TELEM)
	{
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED((uint32_t)telem, Telemetry_exponent - read_exp); // convert vout fsw to Q=-16 (32 bit number)
		// see vout_target_setpoint_computations.xlsx for equations
		vout_scale = (PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_SCALE_LOOP));  // get scale in Q=-16 (16bit number)
		if (vout_scale == 0) vout_scale = 1;  // avoid divide by zero
		telem_unsigned = DIVIDE_WITH_ROUNDING((uint32_t)telem_unsigned , ((uint32_t)DAC_STEP_SIZE_VOLTS_TO_0_156MV));  // convert vout_fsw to volts Q=-16
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED(telem_unsigned, -read_exp-3);  // shift back up to Q=-13 to divide
		telem_unsigned = DIVIDE_WITH_ROUNDING(telem_unsigned , vout_scale);  // divide by vout_scale to get scaled volts
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED(telem_unsigned, 3);  // back to Q=-16
	}
	else
	{
		if (telem_type != IOUT_TELEM)
		{
			telem = SHIFT_EXPONENT(telem, Telemetry_exponent - read_exp); // convert vout fsw to Q=-16 (32 bit number)
		}
	}

	if ((uint32_t)telem_type < (uint32_t)ITSEN_TELEM) //Itsen, Duty and Freq do not have offset and gain
	{
		// grab the s-5.9 or format gain from telem config convert to int32
		// 1 + (gain_base[s4.4] << 5)[s4.9] + gain[s-5.9]
		/*lint -e701 */
		gain = 512 + (TWOS_UNCOMPLEMENT(8, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)((uint32_t)TELEMETRY_CONFIG_VOUT_GAIN_BASE + (8*(uint32_t)telem_type)))) << 5);
		gain += TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)((uint32_t)TELEMETRY_CONFIG_VOUT_GAIN + (4*(uint32_t)telem_type))));

		// offset in telem_type format already covert to int32
		offset = TWOS_UNCOMPLEMENT(8, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)((uint32_t)TELEMETRY_CONFIG_VOUT_OFFSET + (8*(uint32_t)telem_type))));

		if (telem_type == VOUT_TELEM)
		{
			offset = SHIFT_EXPONENT(offset, PMBUS_Get_Vout_Mode_Exponent((PMBUS_PAGE_t)loop) - read_exp); // convert to READ_Exponent
		}

	}
	else {
		gain = SHIFT_EXPONENT(1, -Telemetry_GAIN_EXP);
		offset = 0;
	}

	//=========patch modification
	if (telem_type == IOUT_TELEM)
	{
		user_data.iout_gain = gain;
		user_data.iout_offset = offset;
	}
	//=========patch modification


	// multiply the gain by the telem value and shift down by Telemetry_GAIN_EXP to
	// get read_exp Q format number post multiply
	if (telem_type == VOUT_TELEM)
	{
		telem_unsigned = telem_unsigned*(uint32_t)gain;  // multiply by gain ~512 in Q0 format
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED(telem_unsigned, Telemetry_GAIN_EXP);  // shift down by 9
		telem = offset + (int32_t)telem_unsigned;  // add in the offset
	}
	else
	{
		if (telem_type != IOUT_TELEM)
		{
			telem = SHIFT_EXPONENT(telem*gain, Telemetry_GAIN_EXP);
			telem = offset + telem;  // add in the offset
		}
	}

	/*lint -e845 */
	if ((telem_type ==  ATSEN_TELEM) || (telem_type == BTSEN_TELEM))
	{
		if (ntc_temp != NULL)
		{
			if (telem<0) telem = 0;
			if (telem>511) telem = 511;
			telem = ntc_temp[telem];
		}
		else
		{
			PMBUS_Log_Error(TELEMETRY, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
		}
	}
	return telem;
}
#endif

/*
 * Buck boost version of feed-forward feature
 */
void patch_Regulation_Compute_Feed_Forward (PMBUS_PAGE_t loop)
{
#if defined(zvs_feature_en) || defined(light_load_features_en)
	light_load_features_core_function(loop);
#endif


#ifdef feed_forward_feature_en
	// do FF if 0xC5 en_buck_boost_feed_forward is enabled
//	user_data.debug3 = user_data.en_buck_boost_feed_forward;
	if (user_data.en_buck_boost_feed_forward == 1)
	{
//		user_data.debug4 = 4;
		uint32_t vin_shift = 16; // divide by 2^16 to set result at U7.4
		uint32_t vin_result; // U7.4
		uint32_t vout_tgt; // U8.8
		uint32_t dividend, divisor, quotient;

		vin_result = TELEM_TLM_VIN_LPF__GET((telem_idx_t)loop); //u7.4// get the current vin voltage
		vout_tgt = VCONTROL_VC_VCTRL_RD__GET((vcontrol_idx_t)loop);  // U12.0
		vout_tgt = vout_tgt * REGULATION_APP.vout_scale_factor[loop]; // U12.0 * U0.13 = U12.13
		vout_tgt = vout_tgt >> 5; // U8.8, some binary point magic here due to range limits
		dividend = vout_tgt << 10; // U8.8 << 10 = U18.-2
		divisor  = vout_tgt + (vin_result << 7); // U8.8 + U7.4 = U9.8

		quotient = dividend / divisor;

		PID_PID_FF_CFG4__SET((pid_idx_t)loop, PID_PID_FF_CFG4__PID_FF_OVERRIDE_SEL__Msk
				| PID_PID_FF_CFG4__PID_FF_OVERRIDE__Shift(quotient));
	}
	else
	{
		uint32_t quotient = 0;

		PID_PID_FF_CFG4__SET((pid_idx_t)loop, PID_PID_FF_CFG4__PID_FF_OVERRIDE_SEL__Msk
				| PID_PID_FF_CFG4__PID_FF_OVERRIDE__Shift(quotient));
	}

#endif
}


/*
 * Global iout telem bounding
 */

void iout_telemetry_get_high_frequence_limited(PMBUS_PAGE_t loop)
{
	int32_t temp = iout_telemetry_get_high_frequence(loop);
	if (temp>1023) temp = 1023;
	if (temp<-1023) temp = -1023;
	user_data.iout[loop] = (int16_t)temp;
}

void light_load_features_core_function(PMBUS_PAGE_t loop)
{
	iout_telemetry_get_high_frequence_limited(loop);

#ifdef zvs_feature_en
	// turn on ZVS feature once it was enabled and there is a zvs threshold != 0
	if (user_data.zvs_initialization_status == 1 && user_data.iout_zvs_threshold_current != 0)
	{
		iout_zvs_threshold_irq_handle(loop);
	}

#endif

#ifdef light_load_features_en
	// turn on light load modification once it was enabled and there is any mode of regulation
	if (user_data.light_load_features_initialization_status == 1 && user_data.light_load_features_mode != NO_MODE)
	{
		light_load_features_threshold_irq_handle(loop);
	}
#endif

}


int32_t iout_telemetry_get_high_frequence(PMBUS_PAGE_t loop)
{
	int32_t telem = (int32_t)TWOS_UNCOMPLEMENT(13, (TELEM_TLM_IOUT_FSW__GET((telem_idx_t)loop)));  // S9.4, LSB 62.5mA

	int32_t gain = user_data.iout_gain;
	int32_t offset = user_data.iout_offset;

	uint16_t fs = user_data.fs;  // u11.0 format 1khz LSB
	uint8_t pwm1_deadtime_rise = user_data.pwm1_deadtime_rise;		// u6.2	or 	LSB = 1.25ns
	uint8_t pwm1_deadtime_fall = user_data.pwm1_deadtime_fall;		// u6.2	or 	LSB = 1.25ns

	uint32_t actual_duty_cycle = TELEM_TLM_DUTY_FSW__TLM_DUTY_FSW__GET((telem_idx_t)loop);  // u0.16 & LSB = 0.000015259
	uint32_t dead_time;
	uint8_t sign;

	if ( pwm1_deadtime_fall >= pwm1_deadtime_rise)
	{
		dead_time = pwm1_deadtime_fall - pwm1_deadtime_rise;
		sign = 1;
	}
	else
	{
		dead_time = pwm1_deadtime_rise - pwm1_deadtime_fall;
		sign = 0;
	}

	/*
	 * Take a note, that there are different formats depends on LSB from ADC or LSB from bits
	 * As an example:
	 * 1) u16.0 with LSB = 2^(-16) - bits
	 * 2) u16.0 with LSB = 320 ns  - from ADC
	 */

	dead_time = dead_time * fs ; 	// u6.2 * u11.0 => u17.2 & LSB = (1.25ns / 1 000 000ns) = 0.000 001 25
	dead_time = (dead_time * 671u) >> 13u;    // LSB = 0.000 015 259
//	dead_time = (uint16_t)dead_time;
	if (sign == 1)
	{
		actual_duty_cycle = actual_duty_cycle + dead_time;
		if (actual_duty_cycle > UINT16_MAX)
		{
			actual_duty_cycle = UINT16_MAX;
		}
	}
	else
	{
		if (actual_duty_cycle >= dead_time)
		{
			actual_duty_cycle = actual_duty_cycle - dead_time;
		}
		else
		{
			actual_duty_cycle = 0;
		}
	}

	uint16_t duty_cycle_nom = (UINT16_MAX - ((uint16_t)actual_duty_cycle));	// (u0.16 - u0.16)
//	int32_t duty_cycle_nom_temp = (int32_t)duty_cycle_nom;
	int32_t duty_cycle_nom_temp = TWOS_UNCOMPLEMENT(32, (uint32_t)duty_cycle_nom);

	telem = (duty_cycle_nom_temp * telem);	// IOUT = (1 - DC) * IOUT or formats: s9.4 = ((u0.16 - u.16) >> 16) * s9.4
	telem >>= 16u;

	telem = SHIFT_EXPONENT(telem, Telemetry_IOUT_EXP - user_data.iout_exp[loop]); // convert into iout_read_exp

	telem = SHIFT_EXPONENT(telem*gain, Telemetry_GAIN_EXP);
	telem = offset + telem;  // add the offset

	// 2v9 modification===START===============================================
	// offset_corr = -factor * vin_telem + 30.5
	// iout_telem_corr = iout_telem + offset_corr	// in iout_read_exp

	// converting 30.5 to iout_exp as additional_value
//	uint16_t additional_value = 61;	// as 30.5 with -1 exp
//	int8_t additional_value_exp = -1;
	int32_t additional_value = SHIFT_EXPONENT(user_data.iout_telem_offset_correction_factor, user_data.iout_telem_offset_correction_factor_exp - user_data.iout_exp[loop]);

	// (factor * vin_telem) calculation
	int32_t offset_corr = (int32_t)(user_data.iout_telem_offset_correction_slope_factor * user_data.vin_telem_in_iout_exp);	// -8 * iout_exp
	offset_corr = SHIFT_EXPONENT(offset_corr, user_data.iout_telem_offset_correction_slope_factor_exp);

	// applying '-'
	offset_corr = -offset_corr;

	// adding 30.5
	offset_corr += additional_value;

	// adding new offset
	telem += offset_corr;
	// 2v9 modification===END===============================================

	return telem;
}

