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
** Created on:  2021-03-05
*
 *      Author: halimand
 */


#include "transformer_scaling.h"

#include "shasta_hal_telem.h"
#include "pmbus_api.h"
#include "driver_common.h"
#include "regulation_drv.h"
#include "shasta_hal_pid.h"



void patch_Telemetry_change_scales(PMBUS_PAGE_t loop)
{
//    /*python automated patch entry*/
//    VOID_PATCH_TABLE_RUN(Telemetry_change_scales, loop)


	// Get MFR_ADJ_TURN_RATIO value
	uint16_t adj_turnratio_linear11 = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO);
	uint32_t adj_turnratio_value = (uint32_t)LINEAR11_TO_MANTISSA(adj_turnratio_linear11);
    int32_t adj_turnratio_exponent = LINEAR11_TO_EXPONENT(adj_turnratio_linear11);

    adj_turnratio_value = SHIFT_EXPONENT_UNSIGNED(adj_turnratio_value, (adj_turnratio_exponent+9));  // convert to u1.9 format if not already
    // Clamp to 1 to prevent dividing by zero
    if (adj_turnratio_value == 0) {
    	adj_turnratio_value = 1 << 9; // convert to u1.9 format if not already
    }


    // Get MFR_VRECT_SCALE
	uint16_t vrect_scale_linear11 = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_VRECT_SCALE);
	uint32_t vrect_value = (uint32_t)LINEAR11_TO_MANTISSA(vrect_scale_linear11);
    int32_t exponent = LINEAR11_TO_EXPONENT(vrect_scale_linear11);
	vrect_value = SHIFT_EXPONENT_UNSIGNED(vrect_value, (exponent+12));  // convert to u0.12 format if not already

	// ************** TLM_VRECT_SCALE compensation with ADJ_TURNRATIO  **************
	vrect_value = (vrect_value * adj_turnratio_value) >> 9;	// Shift by 9 due to adj_turnratio_value scaling (U1.9)


	TELEM_TLM_VRECT_SCALE_LOOP__TLM_VRECT_SCALE_LOOP__SET((telem_idx_t)loop, (vrect_value));  // LSB is 1/256


	// Get MFR_TRANSFORMER_SCALE
	uint16_t transformer_scale_linear11 = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_TRANSFORMER_SCALE);
	uint32_t transformer_turns_ratio = (uint32_t)LINEAR11_TO_MANTISSA(transformer_scale_linear11);
	int32_t transformer_scale_exponent = LINEAR11_TO_EXPONENT(transformer_scale_linear11);
	// if half bridge we need to divide the turns ratio by 2
	if ((Regulation_Get_PMBUS_Configuration(loop, REGULATION_CONFIG_TOPOLOGY) == (uint16_t)REGULATION_TOPOLOGY_HB_CT)
		|| (Regulation_Get_PMBUS_Configuration(loop, REGULATION_CONFIG_TOPOLOGY) == (uint16_t)REGULATION_TOPOLOGY_HB_FW))
		transformer_turns_ratio = transformer_turns_ratio >> 1;  // divide by 2

	transformer_turns_ratio = SHIFT_EXPONENT_UNSIGNED(transformer_turns_ratio, (transformer_scale_exponent+12));

	TELEM_TLM_TRNS_SCALE_LP__TLM_TRANSFORMER_SCALE_LOOP__SET((telem_idx_t)loop, transformer_turns_ratio);  // lsb = 1/1024

	uint32_t vin_convert = 32767;  // clamp to 2^15 - 1
	if ((vrect_value>0) && (transformer_turns_ratio>0))
	{
		vin_convert = vrect_value*transformer_turns_ratio;  // both -12 format
		vin_convert = SHIFT_EXPONENT_UNSIGNED(vin_convert, -10);  // convert to -14 format
		vin_convert = (1<<22) / (vin_convert);
		if (vin_convert > 32767) vin_convert = 32767;
	}

	vin_convert = vin_convert * 82;
	TELEM_TLM_VI_CNVRT_FACT__TLM_VIN_CONVERT_FACTOR__SET((telem_idx_t)loop, vin_convert);  // lsb = 1/1024

	//uint32_t vout_scale = LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_SCALE_LOOP));
	//int32_t vout_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_SCALE_LOOP));
	//vout_scale = (uint16_t)SHIFT_EXPONENT((int32_t)vout_scale, (vout_exp+10));  // convert to u0.10 format if not already
	uint32_t vout_scale = PMBUS_Get_Vout_Scale(loop, -16);  //  // convert to u0.16 format if not already

	//LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_SCALE_LOOP));
    //int32_t vout_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_SCALE_LOOP));
	//vout_scale = SHIFT_EXPONENT_UNSIGNED(vout_scale, (vout_exp+12));  // convert to u0.12 format if not already
	TELEM_TLM_VOUT_SCALE_LOOP__TLM_VOUT_SCALE_LOOP__SET((telem_idx_t)loop, (vout_scale));  // LSB is 1/256

	uint32_t vout_convert = 16383;  // clamp to 2^14 - 1
	if (vout_scale>0) {
		vout_convert = (1<<24) / (vout_scale);
		if (vout_convert > 16383) vout_convert = 16383;
	}
	vout_convert = vout_convert * 82;
	TELEM_TLM_VO_CNVRT_FACT__TLM_VOUT_CONVERT_FACTOR__SET((telem_idx_t)loop, vout_convert);


	//  ************** FF_ONE_DIV_VOUT_SCALE_LOOP compensation with ADJ_TURNRATIO  **************
	uint32_t pid_ff_one_div_vout_scale_loop = 0x1FF;//U6.3
	if (vout_scale>0) {
		pid_ff_one_div_vout_scale_loop = (1<<28) / (vout_scale) / (adj_turnratio_value); // U6.3
		if (pid_ff_one_div_vout_scale_loop > 0x1FF) pid_ff_one_div_vout_scale_loop = 0x1FF;
	}

	PID_PID_FF_CFG3__PID_FF_ONE_DIV_VOUT_SCALE_LOOP__SET((pid_idx_t)loop, pid_ff_one_div_vout_scale_loop); //

	uint32_t pid_ff_i82_div_trans_scale_loop = 0xFFF; //U5.7
	if (transformer_turns_ratio>0) {
		pid_ff_i82_div_trans_scale_loop =  (1<<19)/transformer_turns_ratio; //U5.7  quot1(U5.7) = 2^(-3-exp) / MFR_TRANSFORMER_SCALE[9:0], exp =-12
		if (pid_ff_i82_div_trans_scale_loop > 0xFFF) pid_ff_i82_div_trans_scale_loop = 0xFFF;
	}
	pid_ff_i82_div_trans_scale_loop = pid_ff_i82_div_trans_scale_loop * 82; //U-4.23
 	PID_PID_FF_CFG3__PID_FF_I82_DIV_TRANS_SCALE_LOOP__SET((pid_idx_t)loop, pid_ff_i82_div_trans_scale_loop); //U-4.23
}

