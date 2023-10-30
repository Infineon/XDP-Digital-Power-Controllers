/*
 * user_app.c
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

#include "dtimer_drv.h"
#include "telemetry_drv.h"
#include "regulation_api.h"
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
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "user_app.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "cmsis_os_ext.h"                   // ARM::CMSIS:RTOS:Keil RTX ext
#include "log_app.h"             /* Logging of errors */
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"
#include "uart_drv.h"                // uart driver functions
#include "pmbus_mfr_specific_handlers.h"
#include "shasta_hal_fault_common.h"
#include "gpio.h"

void patch_LOOP_FAULT_HANDLE(PMBUS_PAGE_t loop, FAULT_TYPE_t  fault_type);
void patch_PMBUS_HANDLE_MAX_DUTY(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
uint8_t patch_PMBUS_Get_On_Off_State (PMBUS_PAGE_t loop);
void patch_PMBUS_GPIO_CB (gpio_idx_t Bank, uint32_t Status);
void patch_PMBUS_HANDLE_FW_CONFIG_FAULTS(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void patch_GPIO_Drv_init(gpio_idx_t Bank);

void Telemetry_Setup_Irq_with_polarity(PMBUS_PAGE_t page, Telemetry_irq_src_t source, Telemetry_IRQ_TYPE_t Telemetry_type, Telemetry_handle handle, boolean polarity);
// VOUT UV fault -
//void patch_VOUT_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void patch_PMBUS_HANDLE_VOUT_UV_FAULT_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void patch_PMBUS_HANDLE_VOUT_UV_WARN_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void Telemetry_IRQ_VOUT_UV_FAULT_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type);

#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
// VIN UV fault
//void patch_VIN_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void patch_PMBUS_HANDLE_VIN_UV_FAULT_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void patch_PMBUS_HANDLE_VIN_UV_WARN_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void Telemetry_IRQ_VIN_UV_FAULT_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type);
#endif

//DTIMER_CONFIG_t dtimer6_cfg;
//DRIVER_DTIMER *drv6_info;

/**
 * Main entry point of the user application.  Good place to set breakpoint when debugging a patch.
 */
//void debug_example(void);
//void cb_test (uint32_t event);
void patch_pmbus_mfr_autogen_init(void);

void patch_pmbus_mfr_autogen_init(void)
{
	pmbus_mfr_autogen_init();

	ptr_pmbus_callback[PMBUS_CMDCODE_MAX_DUTY] = patch_PMBUS_HANDLE_MAX_DUTY;

	// patch UVP fault response, OCP fault response, and OVP fault response
	ptr_pmbus_callback[PMBUS_CMDCODE_VOUT_UV_FAULT_RESPONSE] = patch_PMBUS_HANDLE_VOUT_UV_FAULT_LIMIT;
	ptr_pmbus_callback[PMBUS_CMDCODE_VOUT_UV_FAULT_LIMIT] = patch_PMBUS_HANDLE_VOUT_UV_FAULT_LIMIT;
	ptr_pmbus_callback[PMBUS_CMDCODE_VOUT_UV_WARN_LIMIT] = patch_PMBUS_HANDLE_VOUT_UV_WARN_LIMIT;
	//ptr_pmbus_callback[PMBUS_CMDCODE_VOUT_SCALE_LOOP] = patch_PMBUS_HANDLE_VOUT_SCALE_LOOP;
#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
	ptr_pmbus_callback[PMBUS_CMDCODE_VIN_UV_FAULT_RESPONSE] = patch_PMBUS_HANDLE_VIN_UV_FAULT_LIMIT;
	ptr_pmbus_callback[PMBUS_CMDCODE_VIN_UV_FAULT_LIMIT] = patch_PMBUS_HANDLE_VIN_UV_FAULT_LIMIT;
	ptr_pmbus_callback[PMBUS_CMDCODE_VIN_UV_WARN_LIMIT] = patch_PMBUS_HANDLE_VIN_UV_WARN_LIMIT;
#endif
	ptr_pmbus_callback[PMBUS_CMDCODE_FW_CONFIG_FAULTS] = patch_PMBUS_HANDLE_FW_CONFIG_FAULTS;

	// patch the MFR_FIRMWARE_COMMAND handle:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_FIRMWARE_COMMAND] = patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND;


}

void Telemetry_Setup_Irq_with_polarity(PMBUS_PAGE_t page, Telemetry_irq_src_t source, Telemetry_IRQ_TYPE_t Telemetry_type, Telemetry_handle handle, boolean polarity)
{
	uint16_t telem;
	telem = (uint16_t)TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET();
	//Clear hardware interrupt
	TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(telem &~(1u<<(uint32_t)Telemetry_type));

	_Telemetry_s * Telemetry_str = Telemetry_GET_ARRAY_POINTER(Telemetry_type) ;
	Telemetry_str->loop = page;
	Telemetry_str->source = source;
	//Set the handle
	Telemetry_IRQ_Event_Handle_Set(Telemetry_type, handle);
	//Reset to initial polarity
	//Set the threshold
	//Telemetry_Generic_Irq_Threshold_Set(Telemetry_type, source, Telemetry_str->mantissa, Telemetry_str->exponent);
	Telemetry_Set_Irq_Polarity(Telemetry_type, polarity);
	TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(telem);
}


void patch_PMBUS_HANDLE_FW_CONFIG_FAULTS(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction){
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		//SET ENABLE LOOP_HW
		FAULT_HW_App[page].enable_mask_pmbus = ~(FAULTS_Get_PMBUS_Configuration(page, FAULTS_CONFIG_FAULT_ENABLE_MASK_LOOP_HW));
		FAULT_HW_App[page].fault_enable = FAULT_HW_App[page].enable_mask &
											FAULT_HW_App[page].enable_mask_driver &
											FAULT_HW_App[page].enable_mask_pmbus;

		//Apply the fault mask
		FAULT_FAULT_ENABLE_LOOP__SET((fault_idx_t) page, FAULT_HW_App[page].fault_enable);

		//SET ENABLE LOOP_FW
		FAULT_FW_App[page].enable_mask_pmbus = ~(FAULTS_Get_PMBUS_Configuration(page, FAULTS_CONFIG_FAULT_ENABLE_MASK_LOOP_FW));
		FAULT_FW_App[page].fault_enable = FAULT_FW_App[page].enable_mask &
											FAULT_FW_App[page].enable_mask_driver &
											FAULT_FW_App[page].enable_mask_pmbus;

		//SET ENABLE COMMON
		FAULT_COMMON_App.enable_mask_pmbus = ~(FAULTS_Get_PMBUS_Configuration(PMBUS_PAGE_0, FAULTS_CONFIG_FAULT_ENABLE_MASK_LOOP_COMMON));
		FAULT_COMMON_App.fault_enable = FAULT_COMMON_App.enable_mask &
											FAULT_COMMON_App.enable_mask_driver &
												FAULT_COMMON_App.enable_mask_pmbus;
		FAULTCOM_FAULT_ENABLE_COM__SET(FAULT_COMMON_App.fault_enable);

		PMBUS_HANDLE_VIN_ON(page, PMBUS_WRITE);
		PMBUS_HANDLE_VOUT_MIN(page, PMBUS_WRITE);
		PMBUS_HANDLE_VOUT_MAX(page, PMBUS_WRITE);
		PMBUS_HANDLE_VOUT_OV_FAULT_LIMIT(page, PMBUS_WRITE);
		patch_PMBUS_HANDLE_VOUT_UV_FAULT_LIMIT(page, PMBUS_WRITE);
		PMBUS_HANDLE_VIN_OV_FAULT_LIMIT(page, PMBUS_WRITE);
#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
		patch_PMBUS_HANDLE_VIN_UV_FAULT_LIMIT(page, PMBUS_WRITE);
#else
		PMBUS_HANDLE_VIN_UV_FAULT_LIMIT(page, PMBUS_WRITE);
#endif
		PMBUS_HANDLE_IOUT_OC_FAULT_LIMIT(page, PMBUS_WRITE);
		PMBUS_HANDLE_MFR_IOUT_OC_FAST_FAULT_LIMIT(page, PMBUS_WRITE);
		PMBUS_HANDLE_IOUT_OC_LV_FAULT_LIMIT(page, PMBUS_WRITE);  // do after PMBUS_HANDLE_MFR_IOUT_OC_FAST_FAULT_LIMIT and PMBUS_HANDLE_IOUT_OC_FAULT_LIMIT
		PMBUS_HANDLE_IOUT_UC_FAULT_LIMIT(page, PMBUS_WRITE);
		PMBUS_HANDLE_IIN_OC_FAULT_LIMIT(page, PMBUS_WRITE);
		PMBUS_HANDLE_TON_MAX_FAULT_LIMIT(page, PMBUS_WRITE);


		PMBUS_HANDLE_MFR_SELECT_TEMPERATURE_SENSOR(page, PMBUS_WRITE);  // do UT and OT faults here to mask them while changing limits
		//PMBUS_HANDLE_OT_FAULT_LIMIT(page, PMBUS_WRITE); //this are done inside PMBUS_HANDLE_MFR_SELECT_TEMPERATURE_SENSOR
		//PMBUS_HANDLE_OT_WARN_LIMIT(page, PMBUS_WRITE); //this are done inside PMBUS_HANDLE_MFR_SELECT_TEMPERATURE_SENSOR
		//PMBUS_HANDLE_UT_WARN_LIMIT(page, PMBUS_WRITE); //this are done inside PMBUS_HANDLE_MFR_SELECT_TEMPERATURE_SENSOR
		//PMBUS_HANDLE_UT_FAULT_LIMIT(page, PMBUS_WRITE); //this are done inside PMBUS_HANDLE_MFR_SELECT_TEMPERATURE_SENSOR

		//if (Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)page, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_GAIN_OFFSET_FAULT_THRESH_EN))
		//{
			PMBUS_HANDLE_VOUT_OV_WARN_LIMIT(page, PMBUS_WRITE);  // warns are not using fw_config_faults for delay time so no need to call here
			PMBUS_HANDLE_VOUT_UV_WARN_LIMIT(page, PMBUS_WRITE);
			PMBUS_HANDLE_VIN_OV_WARN_LIMIT(page, PMBUS_WRITE);
			PMBUS_HANDLE_VIN_UV_WARN_LIMIT(page, PMBUS_WRITE);
			PMBUS_HANDLE_IOUT_OC_WARN_LIMIT(page, PMBUS_WRITE);
			PMBUS_HANDLE_IIN_OC_WARN_LIMIT(page, PMBUS_WRITE);
			PMBUS_HANDLE_POUT_OP_WARN_LIMIT(page, PMBUS_WRITE);
			PMBUS_HANDLE_PIN_OP_WARN_LIMIT(page, PMBUS_WRITE);
		//}
		//Faults_Clear_Faults(page);
	}

}
void patch_PMBUS_HANDLE_VOUT_UV_FAULT_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	PMBUS_HANDLE_VOUT_UV_FAULT_LIMIT(page,  direction);
	patch_VOUT_UV_FAULT(page,  direction);


}
void patch_PMBUS_HANDLE_VOUT_UV_WARN_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	PMBUS_HANDLE_VOUT_UV_WARN_LIMIT(page,  direction);
	if (direction == PMBUS_WRITE)
		patch_VOUT_UV_FAULT(page,  direction);
}

void patch_VOUT_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{

	boolean polarity;
	//uint32_t vin;
	int8_t vout_read_exp[NUM_LOOPS];
	uint32_t vout,vout_shift,vout_02v,vout_warn,vout_fault;

	if ((direction == PMBUS_WRITE))
	{
		uint16_t warn_mantissa = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VOUT_UV_WARN_LIMIT);
		int32_t  exponent = PMBUS_Get_Vout_Mode_Exponent(page);
		uint16_t fault_mantissa = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VOUT_UV_FAULT_LIMIT);

		int32_t threshold_exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VOUT_OFF_THRESHOLD));
		uint16_t threshold_mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VOUT_OFF_THRESHOLD));
		//int32_t threshold_exponent =  PMBUS_Get_Vout_Mode_Exponent(page);
		//uint16_t threshold_mantissa = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VOUT_OFF_THRESHOLD);
		if (page==PMBUS_PAGE_0)
		{
			/*
			Telemetry_Set_Irq_Threshold(TELEM_IRQ14, (int32_t)warn_mantissa, (int8_t)exponent);
			Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ14, (int32_t)(fault_mantissa), (int8_t)exponent);
			Telemetry_Setup_Irq(page, VOUT_0, TELEM_IRQ14, Telemetry_IRQ_VOUT_UV_FAULT_HANDLE);
			Telemetry_Enable_IRQ_Type(TELEM_IRQ14);*/

			// set up the polarity and thresholds based on the output voltage
			vout_read_exp[page] = (int8_t)PMBUS_Get_Vout_Mode_Exponent((PMBUS_PAGE_t)page);
			vout_shift = (uint32_t)Telemetry_get((PMBUS_PAGE_t)page, VOUT_TELEM);
			vout = (uint32_t)SHIFT_EXPONENT((int32_t)vout_shift,  PMBUS_Q_EXP - vout_read_exp[page]);  // convert to vout_mode format
			if (vout>0xFFFF) vout = 0xFFFF;

			//vout = (uint32_t)SHIFT_EXPONENT((int32_t)vout_shift,  exponent - PMBUS_Q_EXP );
			vout_02v = (uint32_t)SHIFT_EXPONENT((int32_t)threshold_mantissa,  threshold_exponent - vout_read_exp[page] );
			vout_warn = (uint32_t)SHIFT_EXPONENT((int32_t)warn_mantissa, exponent - vout_read_exp[page]  );
			vout_fault = (uint32_t)SHIFT_EXPONENT((int32_t)fault_mantissa,  exponent - vout_read_exp[page] );

			Telemetry_Set_Irq_Threshold(TELEM_IRQ14, (int32_t)warn_mantissa, (int8_t)exponent);

			if (vout > vout_warn)
			{
				polarity = 1;		// waiting for below the fault limit - look out for below fault , if the current vout is greater than earn limit
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ14, (int32_t)(fault_mantissa), (int8_t)exponent);
			}
			else if ((vout < vout_fault) && (vout > vout_02v))		// if vout is below fault limit and greater than 0.2v then still look out for the below update the hys
			{
				polarity = 1;		// waiting for below the 0.2v
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ14, (int32_t)(threshold_mantissa), (int8_t)threshold_exponent);
			}
			else if (vout < vout_02v)			// if vout is below 0.2v  look for above warn limit and update the hyst back to fault limit
			{
				polarity = 0;		//waiting for above the warn limit
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ14, (int32_t)(fault_mantissa), (int8_t)exponent);
			}
			else
			{
				polarity = 0;		//waiting for above the warn limit
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ14, (int32_t)(fault_mantissa), (int8_t)exponent);
			}

			Telemetry_Setup_Irq_with_polarity(page, VOUT_0, TELEM_IRQ14, Telemetry_IRQ_VOUT_UV_FAULT_HANDLE,polarity);
			Telemetry_Enable_IRQ_Type(TELEM_IRQ14);
		}/*
		else
		{
			Telemetry_Set_Irq_Threshold(TELEM_IRQ15, (int32_t)warn_mantissa, (int8_t)exponent);
			Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ15, (int32_t)(fault_mantissa), (int8_t)exponent);
			Telemetry_Setup_Irq(page, VOUT_1, TELEM_IRQ15, Telemetry_IRQ_VOUT_UV_FAULT_HANDLE);
			Telemetry_Enable_IRQ_Type(TELEM_IRQ15);
		}*/
	}
}

void Telemetry_IRQ_VOUT_UV_FAULT_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type)
{
	_Telemetry_s * Telemetry_str = Telemetry_GET_ARRAY_POINTER(Telemetry_type);
	//uint16_t telem;
	PMBUS_PAGE_t page;
	//Telemetry_IRQ_TYPE_t Telemetry_type_vin;

	if(Telemetry_type == TELEM_IRQ14){
		page = PMBUS_PAGE_0;
		//Telemetry_type_vin = TELEM_IRQ12;
	}
	else
	{
		page = PMBUS_PAGE_1;
		//Telemetry_type_vin = TELEM_IRQ13;
	}

	uint16_t mantissa = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VOUT_UV_FAULT_LIMIT);
	int32_t  exponent = PMBUS_Get_Vout_Mode_Exponent(page);
	/*
#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
	uint16_t mantissa_vin_fault = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_FAULT_LIMIT));
	int32_t  exponent_vin_fault = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_FAULT_LIMIT));
	uint16_t mantissa_vin_warn = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_WARN_LIMIT));
	int32_t  exponent_vin_warn = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_WARN_LIMIT));
#endif
*/
	//int32_t threshold_exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VOUT_OFF_THRESHOLD));
	//int32_t threshold_exponent =  PMBUS_Get_Vout_Mode_Exponent(page);
	int32_t threshold_exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VOUT_OFF_THRESHOLD));
	uint16_t threshold_mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VOUT_OFF_THRESHOLD));
	//uint16_t hysteresis_mantissa = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VOUT_UV_WARN_LIMIT);

   	if (~(Telemetry_str->polarity)&1) 				// if is executed vout is below the fault limit
   	{
   		if (Telemetry_str->hysteresis_mantissa == threshold_mantissa && Telemetry_str->hysteresis_exponent == threshold_exponent)		// check if the threshold is below 200 mv then shutdown
   		{
   			//telem = (uint16_t)TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET();
   			//Clear hardware interrupt
   			//TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(telem &~(1u<<(uint32_t)Telemetry_type));
   			Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_type, (int32_t)mantissa, (int8_t)exponent);
   			Telemetry_Generic_Irq_Threshold_Set(Telemetry_type, VOUT_0, (int32_t)mantissa, (int8_t)exponent);
   			Telemetry_Set_Irq_Polarity(Telemetry_type, 0);		// look for above and this sets the threshold as  threshold - Warn limt
   			//TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(telem);
 /*
#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
   			// while turn off vout - also restore the thresholds of the VIN back to original
   			Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_type_vin, (int32_t)mantissa_vin_fault, (int8_t)exponent_vin_fault);
   			Telemetry_Generic_Irq_Threshold_Set(Telemetry_type_vin, VIN_0, (int32_t)mantissa_vin_warn, (int8_t)exponent_vin_warn);
   			Telemetry_Set_Irq_Polarity(Telemetry_type_vin, 0);		// look for above and this sets the threshold as  threshold - Warn limt
#endif
*/
   			// Mask UV fault - untill the En fuse is turned on
   			Faults_Mask_Fault(page, 1, // hw fault
   									0, // set_mask
   									((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN))	// clear_mask
   									);
   			Regulation_Shutdown_Sequence (page, REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE);
			REGULATION_INTER_THREAD_SET(page, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, 0);



   		}
   		else		// for the fist time vout is below the fault limit . change the lower threshold to 200 mv threshold.
   		{
   			// Send the GPIO EFUSE output signal
   			//telem = (uint16_t)TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET();
   			//Clear hardware interrupt
   			Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_type, (int32_t)threshold_mantissa, (int8_t)threshold_exponent);
   			Telemetry_Generic_Irq_Threshold_Set(Telemetry_type, VOUT_0, (int32_t)threshold_mantissa, (int8_t)threshold_exponent);
   			Telemetry_Set_Irq_Polarity(Telemetry_type, 1);		// look for below 200 mv . if it goes below 200 mv - shutdown.
   			//TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(telem);

   			// Send the GPIO EFUSE output signal
		   	GpioBit_DatWr(GPIO_0, PWM1_BIT_INDEX, 0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
   		}
   	}
   	else		//else is executed when threshold is above the Warn limit
   	{
   		Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_type, (int32_t)mantissa, (int8_t)exponent);
   		Telemetry_Generic_Irq_Threshold_Set(Telemetry_type, VOUT_0, (int32_t)mantissa, (int8_t)exponent);
		Telemetry_Set_Irq_Polarity(Telemetry_type, 1);			//look for below
   	}
}
#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
void patch_PMBUS_HANDLE_VIN_UV_WARN_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	PMBUS_HANDLE_VIN_UV_WARN_LIMIT(page,  direction);
	if (direction == PMBUS_WRITE)
		patch_VIN_UV_FAULT(page,  direction);
}

void patch_PMBUS_HANDLE_VIN_UV_FAULT_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	PMBUS_HANDLE_VIN_UV_FAULT_LIMIT(page,  direction);
	patch_VIN_UV_FAULT(page,  direction);
}

void patch_VIN_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{

	if ((direction == PMBUS_WRITE))
	{
		int32_t warn_exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_WARN_LIMIT));
		uint16_t warn_mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_WARN_LIMIT));
		uint16_t fault_mantissa =  (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_FAULT_LIMIT));
		int32_t fault_exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_FAULT_LIMIT));
		boolean polarity;
		uint32_t vin,vin_shift;
		//int8_t vout_read_exp[NUM_LOOPS];
		uint32_t vin_02v,vin_warn,vin_fault;


		if (page==PMBUS_PAGE_0)
		{
			/*Telemetry_Set_Irq_Threshold(TELEM_IRQ12, (int32_t)warn_mantissa, (int8_t)exponent);
			Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ12, (int32_t)(fault_mantissa), (int8_t)exponent);
			Telemetry_Setup_Irq(page, VIN_0, TELEM_IRQ12, Telemetry_IRQ_VIN_UV_FAULT_HANDLE);
			Telemetry_Enable_IRQ_Type(TELEM_IRQ12);*/

			// set up the polarity and thresholds based on the output voltage
			//vout_read_exp[page] = (int8_t)PMBUS_Get_Vout_Mode_Exponent((PMBUS_PAGE_t)page);
			vin_shift = (uint32_t)Telemetry_get((PMBUS_PAGE_t)page, VIN_TELEM);
			if (vin_shift>0x3FF) vin_shift = 0x3FF;

			vin = (uint32_t)SHIFT_EXPONENT((int32_t)vin_shift, -3- PMBUS_Q_EXP );
			vin_02v = (uint32_t)SHIFT_EXPONENT((int32_t)20,  -2 - PMBUS_Q_EXP );
			vin_warn = (uint32_t)SHIFT_EXPONENT((int32_t)warn_mantissa,  warn_exponent - PMBUS_Q_EXP  );
			vin_fault = (uint32_t)SHIFT_EXPONENT((int32_t)fault_mantissa,  fault_exponent - PMBUS_Q_EXP );

			Telemetry_Set_Irq_Threshold(TELEM_IRQ12, (int32_t)warn_mantissa, (int8_t)warn_exponent);

			if (vin > vin_warn)
			{
				polarity = 1;		// waiting for below the fault limit - look out for below fault , if the current vout is greater than earn limit
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ12, (int32_t)(fault_mantissa), (int8_t)fault_exponent);
			}
			else if ((vin < vin_fault) && (vin > vin_02v))		// if vout is below fault limit and greater than 0.2v then still look out for the below update the hys
			{
				polarity = 1;		// waiting for below the 0.2v
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ12, (int32_t)(20), (int8_t)-2);
			}
			else if (vin < vin_02v)			// if vout is below 0.2v  look for above warn limit and update the hyst back to fault limit
			{
				polarity = 0;		//waiting for above the warn limit
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ12, (int32_t)(fault_mantissa), (int8_t)fault_exponent);
			}
			else
			{
				polarity = 0;		//waiting for above the warn limit
				Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ12, (int32_t)(fault_mantissa), (int8_t)fault_exponent);
			}

			Telemetry_Setup_Irq_with_polarity(page, VIN_0, TELEM_IRQ12, Telemetry_IRQ_VIN_UV_FAULT_HANDLE,polarity);
			Telemetry_Enable_IRQ_Type(TELEM_IRQ12);
		}/*
		else
		{
			Telemetry_Set_Irq_Threshold(TELEM_IRQ13, (int32_t)warn_mantissa, (int8_t)warn_exponent);
			Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ13, (int32_t)(fault_mantissa), (int8_t)fault_exponent);
			Telemetry_Setup_Irq(page, VIN_1, TELEM_IRQ13, Telemetry_IRQ_VIN_UV_FAULT_HANDLE);
			Telemetry_Enable_IRQ_Type(TELEM_IRQ13);
		}*/
	}
}

void Telemetry_IRQ_VIN_UV_FAULT_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type)
{
	_Telemetry_s * Telemetry_str = Telemetry_GET_ARRAY_POINTER(Telemetry_type);
	//uint16_t telem;
	PMBUS_PAGE_t page;

	if(Telemetry_type == TELEM_IRQ12)
		page = PMBUS_PAGE_0;
	else
		page = PMBUS_PAGE_1;

	uint16_t mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_FAULT_LIMIT));
	int32_t  exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VIN_UV_FAULT_LIMIT));

   	if (~(Telemetry_str->polarity)&1) 				// if is executed vout is below the fault limit
   	{

   		if (Telemetry_str->hysteresis_mantissa == 20 && Telemetry_str->hysteresis_exponent == -2)		// check if the threshold is below 2000 mv then shutdown
   		{
   			Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_type, (int32_t)mantissa, (int8_t)exponent);
   			Telemetry_Generic_Irq_Threshold_Set(Telemetry_type, VIN_0, (int32_t)mantissa, (int8_t)exponent);
   			Telemetry_Set_Irq_Polarity(Telemetry_type, 0);		// look for above and this sets the threshold as  threshold - Warn limt

   			// Mask UV fault - untill the En fuse is turned on
   		/*	Faults_Mask_Fault(page, 1, // hw fault
   									0, // set_mask
   									((1u<<(uint32_t)FAULT_TYPE_VIN_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VIN_UV_WARN))	// clear_mask
   									);
   			*/
   			Regulation_Shutdown_Sequence (page, REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE);
			REGULATION_INTER_THREAD_SET(page, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, 0);

   		}
   		else		// for the fist time vout is below the fault limit . change the lower threshold to 2000 mv threshold.
   		{
   			Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_type, (int32_t)20, (int8_t)-2);
   			Telemetry_Generic_Irq_Threshold_Set(Telemetry_type, VIN_0, (int32_t)20, (int8_t)-2);
   			Telemetry_Set_Irq_Polarity(Telemetry_type, 1);		// look for below 5v . if it goes below 5 v - shutdown.

   			// Send the GPIO EFUSE output signal
   		   	GpioBit_DatWr(GPIO_0, PWM1_BIT_INDEX, 0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
   		}
   	}
   	else		//else is executed when threshold is above the Warn limit
   	{
   		Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_type, (int32_t)mantissa, (int8_t)exponent);
   		Telemetry_Generic_Irq_Threshold_Set(Telemetry_type, VIN_0, (int32_t)mantissa, (int8_t)exponent);
		Telemetry_Set_Irq_Polarity(Telemetry_type, 1);			//look for below
   	}
}

#endif
void user_drv_init(void)
{
	memset(&user_data, 0, sizeof(USER_DATA_t));  // ZI the user data
	//patch_rom_table.patch_FAULTS_Resolve_Shutdown_Priority = &FAULTS_User_Resolve_Shutdown_Priority;
	//ntc_temp = &user_ntc_temp_lut[0];  // set the ntc lut to rom constant table
	// this is the initialization of user pmbus commands autogenerated from pmbus spreadsheet

	patch_rom_table.patch_PMBUS_GPIO_CB = &patch_PMBUS_GPIO_CB;
	patch_rom_table.patch_PMBUS_Get_On_Off_State = &patch_PMBUS_Get_On_Off_State;
	patch_rom_table.patch_LOOP_FAULT_HANDLE = &patch_LOOP_FAULT_HANDLE;
	patch_rom_table.patch_Regulation_Shutdown_Sequence = &patch_Regulation_Shutdown_Sequence;
    patch_rom_table.patch_GPIO_Drv_init = &patch_GPIO_Drv_init;

	ptr_mfr_specific_init = (mfr_specific_init_ptr) patch_pmbus_mfr_autogen_init;
	set_module_init_cb(MODULE_REGULATION, regulation_sm_callbacks_init);  // set the pointer funtion in init of regulation state machine to user callback setup for each state
	// Configure the the additional GPIO pins for ZSC (PWM1, PWM2). This will only work if the patch was started from RAM.
	// The zsc_gpio_config function is also called from patch_GPIO_Drv_init, which will work if the patch was started from OTP.
	zsc_gpio_config();

	regulation_sm_callbacks_init();
}


void patch_PMBUS_HANDLE_MAX_DUTY(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	uint16_t duty;
	int32_t exponent;
	uint32_t value;
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		// force the duty cycle based on the max_duty
		duty = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MAX_DUTY);
		duty = (uint16_t)LINEAR11_TO_MANTISSA(duty);  // since we know exponent is 0 we can just grab mantissa
		exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MAX_DUTY));
		value = (uint32_t)SHIFT_EXPONENT(duty, (exponent + 8));  // Value is percentage
		//value = (uint32_t)(value * 256) / 100 ;		// Convert this to a register of LSB
		value = (uint32_t)(value / 100) ;
		if (value > 0xff) value = 0xff;  // cap to field max

		//Regulation_Max_Duty_Set(page, duty, exponent);
		if(page == PMBUS_PAGE_0)		//PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY_EN__SET(1);
		{
			PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY__SET(value);
		}
		else
		{
			PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY__SET(value); //U7.1, percent
		}
		PMBUS_HANDLE_MAX_DUTY(page,direction);
	}
}

void patch_PMBUS_GPIO_CB (gpio_idx_t Bank, uint32_t Status)
{

	// This patch code should support two enable pins
	//1. VR enable pin of shasta
	//2. GPIO enable pin on PWM2

	int32_t vr_enable_bit_index = PMBUS_Gpio_Get_PMBUS_Configuration((PMBUS_PAGE_t)Bank, PMBUS_GPIO_CONFIG_ENABLE_SELECT);
	//RGU_SPARE_FF__SET(0xabcd0000 | (Bank<<16) | Status); // debug msg
	if (vr_enable_bit_index>=0)  // if enable is assgined
	{
		if ( (((Status)&(1u<<vr_enable_bit_index)) == (1u<<vr_enable_bit_index)) ||
				(((Status)&(1u<<PWM2_BIT_INDEX)) == (1u<<PWM2_BIT_INDEX)) )  // check if bit index is set in status to indicate enable interrupt
		{

			if ((GpioBit_DatRd(GPIO_0,PWM2_BIT_INDEX))==0)		// if power good aux input pin is set to low then shutdown
			{
				// Mask UV fault - untill the En fuse is turned on
				/*
				Faults_Mask_Fault(loop, 1, // hw fault
											0, // set_mask
											((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN))	// clear_mask
											);
				 */
				Regulation_Shutdown_Sequence ((PMBUS_PAGE_W_BROADCAST_t)Bank, REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE);
				REGULATION_INTER_THREAD_SET((PMBUS_PAGE_W_BROADCAST_t)Bank, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, 0);
			}
			else
			{
				// tell the pmbus thread that enable has changed
				//PWM_PWM_FORCE_HI_LO__SET(0x00);		// reset the PWM HI LO
				PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)Bank, PMBUS_CMDCODE_OPERATION, PMBUS_WRITE);
			}
		}

	}
}


uint8_t patch_PMBUS_Get_On_Off_State (PMBUS_PAGE_t loop)
{

	// This patch code should support two enable pins
	//1. VR enable pin of shasta
	//2. GPIO enable pin on PWM2

	uint8_t operation, on_off_config;
	uint8_t temp;
	boolean pwm2_gpio;
	boolean enable = PMBUS_Gpio_Get_Feature(loop, PMBUS_GPIO_CONFIG_ENABLE_SELECT);

	operation = PMBUS_Get_Command_Byte(loop, PMBUS_CMDCODE_OPERATION);
	on_off_config = PMBUS_Get_Command_Byte(loop, PMBUS_CMDCODE_ON_OFF_CONFIG);

    // GPIO - PWM2 input enable pin read - pwrgd_aux - input pin
	if ((GpioBit_DatRd(GPIO_0,PWM2_BIT_INDEX))!=0)
	{
		pwm2_gpio= 1;
	}
	else
	{
		pwm2_gpio = 0;
	}

	if ((on_off_config&0x10)==0x10)
		{
			switch ((on_off_config & 0xc)>>2)
			{
				case 1:
					// controlled by ENABLE/CONTROL pin		// Adding the and operation of the pwm gpio Enable
					if (((on_off_config & 0x2) == 0x2))
					{
						//PWM_PWM_FORCE_HI_LO__SET(0x18);		// pull up PWM4 and PWM5 if the pwm2 is pullup and convertor is on
						return (pwm2_gpio & enable);  // active high polarity
					}
					else
					{
						return (pwm2_gpio & (uint8_t)(!enable));  // active low polarity
					}
					//break;
				case 2:
					// controlled by OPERATION bit 7
					temp = operation & 0x80;
					temp = temp >> 7;
					return (temp);
					//break;
				case 3:
					// controlled by OPERATION bit 7 and ENABLE/CONTROL
					temp = operation & 0x80;
					temp = temp >> 7;
					if (((on_off_config & 0x2) == 0x2))
					{
						//PWM_PWM_FORCE_HI_LO__SET(0x18);		// pull up PWM4 and PWM5 if the pwm2 is pull up and convertor is on
						temp = temp & pwm2_gpio & (enable);  // active high polarity
					}
					else
					{
						/* lint -elib(514) */
						temp = temp & pwm2_gpio & (uint8_t)(!enable);  // active low polarity
					}
					//SCU_SPARE_FF__SET(0xcbbc0000 | temp);  // debug print
					return (temp);
					//break;
				default:
					return 0;  // weird combo meaning uncontrolled?
					//break;
			}
		}
	else
	{
		// unit powers up regardless of ENABLE pin or OPERATION bit7
		return 1;
	}


}


void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control)
{

    // Send the GPIO EFUSE output signal
   	GpioBit_DatWr(GPIO_0, PWM1_BIT_INDEX, 0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

	// turning off the PWM 3 to pwm6
	//PWM_PWM_ON__PWM_ON__SET(0x00);

	Regulation_Fault_Mask (loop, state, control);

	Regulation_PWM_Control(loop, REGULATION_PWMS_OFF);  // ALL FET's OFF
	Regulation_Set_VID(loop, 0, 0, 0, 0, 1);
	VCONTROL_VC_VRAMP1__VC_VRAMP_ON__SET((vcontrol_idx_t)loop, 0);  // turn off vramp_on signal to allow pwms to stop regulation
	// clear the interrupt here in case it was left over from a fault
	VCONTROL_VC_IRQ__VC_VCONTROL_IRQ_EN__SET((vcontrol_idx_t) loop, 0);

   	// restore the VIN UV FAULT limits by writing the VIN UV fault limit which checks the vina nd sets the thresholds
	//patch_VOUT_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);
	//patch_VIN_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);

	//disable the hardware blocks: (telemetry and faults are always active but masked at different times)
	Regulation_Power_Mode (loop, state);

	if (loop == PMBUS_PAGE_0)
	{
		// disable ton_max interrupts
		TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET() &
																	~((1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_0)));
	}
	else
	{
		// disable ton_max interrupts
		TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET() &
																		~((1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_1)));
	}

	// update pmbus status off bit:
	PMBUS_STATUS_OFF__SET((pmbus_status_loop_idx_t)loop, 1);
	// update status word from result:
	PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_STATUS_WORD, PMBUS_READ);

	// Clear HW Fault Shutdown mask
	FAULT_FAULT_SHUT_CLR_LOOP__FAULT_SHUT_CLR_LOOP__SET((fault_idx_t) REGULATION_APP.loop, 1) ;

	if (loop == PMBUS_PAGE_0)
	{
	FAULTCOM_FAULT_SHUT_CLR_COM__FAULT_SHUT_CLR_COM__SET(1) ;
	}
	///////////////////////////////


	call_system_cb(SHUTDOWN_SEQUENCE);

}



void patch_LOOP_FAULT_HANDLE(PMBUS_PAGE_t loop, FAULT_TYPE_t  fault_type)
{
	//FAULT_TYPE_t fault_type[32:0]  - hardware faults
	//FAULT_TYPE_t fault_type[64:32] - firmware faults
	//uint8_t fault_code[32:0] = (fault_type)      - hardware faults
	//uint8_t fault_code[32:0] = (fault_type - 32) - firmware faults

	//FAULT_TYPE_t hysteresis_type[32:0]  - hardware faults
	//FAULT_TYPE_t hysteresis_type[64:32] - firmware faults
	//uint8_t hysteresis_code[32:0] = (hysteresis_type)      - hardware faults
	//uint8_t hysteresis_code[32:0] = (hysteresis_type - 32) - firmware faults

    //Fault code is the number between 0..31 for FAULT_DRV_App
    //Firmware fault_types are 32..63 with fault_code 0..31
    uint32_t fault_code = (uint32_t)fault_type;


	_FAULT_DRV_s *FAULT_DRV_App;
    _FAULT_DESC_s * fault_str = FAULTS_GET_ARRAY_POINTER(loop, fault_type);
    // if we get a null pointer on the fault type we are enabling we need to abort and disable faults
    /** @todo graceful recovery might be to clear the enable of given fault and assume other faults are ok **/
    if (fault_str == NULL)
	{
    	LOG_ERROR_TYPE(FAULTS, ERROR, CORRUPT_MEMORY);
    	faults_drv_irq_disable();
		return;
	}

    FAULT_TYPE_t hysteresis_type = fault_str->hysteresis;
    uint32_t hysteresis_code = (uint32_t)fault_str->hysteresis;
    FAULT_RESPONSE_TYPE_t fault_response  = (fault_str->fault_response_type);

    boolean hw_fault;
	boolean fault_pin_mask;

	if ((uint32_t)fault_type< 32)
	{
		FAULT_DRV_App = &FAULT_HW_App[loop];
		hw_fault = 1;
		fault_pin_mask = (boolean)(((~FAULTS_Get_PMBUS_Configuration(loop,FAULTS_CONFIG_FAULT_PIN_MASK_HW)) >> fault_code)&1);
	}

	else
	{
		FAULT_DRV_App = &FAULT_FW_App[loop];
		fault_code = ((uint32_t)fault_type - 32);
		if(hysteresis_code == (uint32_t)RESERVED_HW_NO_FAULT) hysteresis_code = (uint32_t)RESERVED_FW_NO_FAULT;
		hysteresis_code = ((uint32_t)hysteresis_code - 32);
		hw_fault = 0;
		fault_pin_mask = (boolean)(((~FAULTS_Get_PMBUS_Configuration(loop,FAULTS_CONFIG_FAULT_PIN_MASK_FW)) >> fault_code)&1);
	}


	//Grab status, enable, shut mask and polarity from HW registers
	if (hw_fault)
	{
		//Read the Fault Status
		FAULT_DRV_App->fault_status = (FAULT_FAULT_STATUS_LOOP__GET((fault_idx_t) loop) | (FAULT_DRV_App->fault_status));
	}

	//Confirm that fault_code status bit is set
	if (FAULT_DRV_App->fault_status & (1u<<(uint32_t)fault_code))
	{

	  //   PROCESS LOOP:
	  //   Read Fault Status
	  //   Read Shutdown Masks
	  //   Update regulation thread due to shutdown event
	  //   Mask/Disable/polarity Fault interrupt source

	  //        Check which event happened:
	  //      warn_event_status - above warn limit  fault_status[x] = 1 and shutdown_mask[x] = 0 and polarity[x] = 0
	  //      warn_fall_event_status - below warn limit  fault_status[x] = 1 and shutdown_mask[x] = 0 and polarity[x] = 1
	  //      fault_event_status  - above fault limit fault_status[x] = 1 and shutdown_mask[x] = 1 and polarity[x] = 0
	  //

		uint32_t warn_event_status =          (1u<<(uint32_t)fault_code) & ~FAULT_DRV_App->fault_shut_mask & ~FAULT_DRV_App->fault_polarity;
		uint32_t warn_removed_event_status =  (1u<<(uint32_t)fault_code) & ~FAULT_DRV_App->fault_shut_mask &  FAULT_DRV_App->fault_polarity;
		uint32_t shutdown_event_status  =     (1u<<(uint32_t)fault_code) &  FAULT_DRV_App->fault_shut_mask & ~FAULT_DRV_App->fault_polarity;
		uint32_t fault_removed_event_status = (1u<<(uint32_t)fault_code) &  FAULT_DRV_App->fault_shut_mask &  FAULT_DRV_App->fault_polarity;

		uint32_t retry_event_status = 0;
		FAULT_TYPE_t fault_type_retry;
		uint32_t fault_code_retry;

		if (((hysteresis_code==fault_code) && (fault_type!=FAULT_TYPE_IOUT_UC_FAULT) &&
											  (fault_response!=FAULT_RESPONSE_RETRY_OPERATION) &&
											  (fault_response!=FAULT_RESPONSE_IOUT_DELAY_WITH_CC_RETRY)) ||
			((hysteresis_code==0) && (fault_response!=FAULT_RESPONSE_RETRY_OPERATION) &&
								     (fault_response!=FAULT_RESPONSE_IOUT_DELAY_WITH_CC_RETRY))          ||
			//IOUT_UC_FAULT is a special case
			((fault_type==FAULT_TYPE_IOUT_UC_FAULT) && (fault_response==FAULT_RESPONSE_RETRY_SHUTDOWN)))
		{
			retry_event_status = fault_removed_event_status;
			fault_code_retry = fault_code;
			fault_type_retry = fault_type;

		}
		else if ((((hysteresis_code==0) || (hysteresis_code==fault_code)) && ((fault_response==FAULT_RESPONSE_RETRY_OPERATION) ||
												 (fault_response==FAULT_RESPONSE_IOUT_DELAY_WITH_CC_RETRY)))  ||
				 ((fault_type==FAULT_TYPE_IOUT_UC_FAULT) && (fault_response==FAULT_RESPONSE_SHUTDOWN_RESTART_WHEN_REMOVED)))
		{
			retry_event_status = warn_removed_event_status;
			fault_code_retry = fault_code;
			fault_type_retry = fault_type;
		}
		else
		{
			retry_event_status = warn_removed_event_status;
			fault_code_retry = hysteresis_code;
			fault_type_retry = hysteresis_type;
		}

		////////////////////
		///REPORT ON PMBUS//
		////////////////////

		//do not grab status on falling below limit
		if (((1u<<(uint32_t)fault_code) & ~warn_removed_event_status & ~fault_removed_event_status)!=0)
		{
			//REPORT FAULT_TYPE on PMBUS
			FAULTS_REPORT_PMBUS(loop,fault_type);
			//Set fault pin if not masked by pmbus
			PMBUS_Gpio_Set_Feature((PMBUS_PAGE_t)loop, PMBUS_GPIO_CONFIG_FAULT_SELECT, fault_pin_mask);  // set fault pin for given loop

		}

		// SHUTDOWN EVENT
		if (shutdown_event_status)
		{
		   	//if(((PMBUS_STATUS_VOUT__GET((pmbus_status_loop_idx_t)loop) & 0x80 )) )		// for UVP - ignore  ENable efuse
		   	if( (fault_type!=FAULT_TYPE_VIN_UV_FAULT)  && (fault_type!=FAULT_TYPE_VIN_UV_WARN) &&  (fault_type!=FAULT_TYPE_VOUT_UV_FAULT)  && (fault_type!=FAULT_TYPE_VOUT_UV_WARN) )
		   	{
	            // Send the GPIO EFUSE output signal
		   		GpioBit_DatWr(GPIO_0, PWM1_BIT_INDEX, 0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
		   	}

			//Turn off vcontrol irq so that we don't get vid_reached signal
			VCONTROL_VC_IRQ__VC_VCONTROL_IRQ_EN__SET((vcontrol_idx_t) loop, 0);
			ON_OFF_STATE.Shutdown_State[loop] = 1; // Save shutdown from fault state

			//Save Current Status
			FAULT_DRV_App->fault_status_event_latch |=(uint32_t) 1<<fault_code ;

			if((hysteresis_code!=fault_code) && (hysteresis_code!=0))
			{
				//Disable the fault until it goes below warn
				FAULT_DRV_App->enable_mask_driver &= ~ (1u<<(uint32_t)fault_code);
				FAULT_DRV_App->fault_enable = FAULT_DRV_App->enable_mask_driver &
												FAULT_DRV_App->enable_mask &
													FAULT_DRV_App->enable_mask_pmbus;
				if (hw_fault)
				{
					FAULT_FAULT_ENABLE_LOOP__SET((fault_idx_t) loop, FAULT_DRV_App->fault_enable);
				}
			}

			//
			// COMMUNICATE SHUTDOWN TO REGULATION THREAD
			// PRIORITY IS GIVEN TO SHUTDOWN WITH MIN RETRY COUNT
			//

			FAULT_TYPE_t higher_priority_fault = FAULTS_Resolve_Shutdown_Priority(loop, fault_type, FAULT_DRV_App->last_high_priority_fault);

			//If current fault code is the priority one send it to regulation
			if (higher_priority_fault==fault_type)
			{
				FAULT_HW_App[loop].last_high_priority_fault = fault_type;
				FAULT_FW_App[loop].last_high_priority_fault = fault_type;
				//need to REGULATION_INTER_THREAD_SET call twice. one to move the state machine one to go to controls case
				//can be fixed by using RTX5.0 and RTX cmsis2.1 by reordering the queue
				REGULATION_INTER_THREAD_SET(loop, REGULATION_CONTROLS_FAULT, ((uint32_t)fault_type & 0xff));
				REGULATION_INTER_THREAD_SET(loop, REGULATION_CONTROLS_FAULT, ((uint32_t)fault_type & 0xff));
			}
		}
		else if (warn_event_status)
		{

			//
			// COMMUNICATE RETRY OPERATION TO REGULATION THREAD
			// PRIORITY IS GIVEN TO SHUTDOWN WITH MIN RETRY COUNT
			//

			fault_str = FAULTS_GET_ARRAY_POINTER((PMBUS_PAGE_t)loop, (FAULT_TYPE_t) fault_type) ;

			if ((fault_str->fault_response_type == FAULT_RESPONSE_RETRY_OPERATION) ||
						(fault_str->fault_response_type == FAULT_RESPONSE_IOUT_DELAY_WITH_CC_RETRY) ||
									(fault_str->fault_response_type == FAULT_RESPONSE_IOUT_CONSTANT_CURRENT_WITH_OC_LV) ||
									    	((fault_type==FAULT_TYPE_IOUT_UC_FAULT) && (fault_str->fault_response_type==FAULT_RESPONSE_SHUTDOWN_RESTART_WHEN_REMOVED)))
			{
				if((hysteresis_code!=fault_code) && (hysteresis_code!=0))
				{
					//Disable the fault until it goes below warn
					FAULT_DRV_App->enable_mask_driver &= ~(1u<<(uint32_t)fault_code);
					FAULT_DRV_App->fault_enable = FAULT_DRV_App->enable_mask_driver &
													FAULT_DRV_App->enable_mask &
															FAULT_DRV_App->enable_mask_pmbus;
					if (hw_fault)
					{
						FAULT_FAULT_ENABLE_LOOP__SET((fault_idx_t) loop, FAULT_DRV_App->fault_enable);
					}
				}
				FAULT_TYPE_t higher_priority_fault = FAULTS_Resolve_Shutdown_Priority(loop, fault_type, FAULT_DRV_App->last_high_priority_fault);

				//If current fault code is the priority one send it to regulation
				if (higher_priority_fault==fault_type)
				{
					FAULT_HW_App[loop].last_high_priority_fault = fault_type;
					FAULT_FW_App[loop].last_high_priority_fault = fault_type;

					//Save Current Status
					FAULT_DRV_App->fault_status_event_latch |=(uint32_t) 1<<fault_code ;

					//need to REGULATION_INTER_THREAD_SET call twice. one to move the state machine one to go to controls case
					//can be fixed by using RTX5.0 and RTX cmsis2.1 by reordering the queue
					REGULATION_INTER_THREAD_SET(loop, REGULATION_CONTROLS_FAULT, ((uint32_t) fault_type & 0xff));
					REGULATION_INTER_THREAD_SET(loop, REGULATION_CONTROLS_FAULT, ((uint32_t) fault_type & 0xff));
				}
			}
		}

		//RETRY EVENT //
		else if (retry_event_status)
		{
			//re-enable possibly disabled hw faults
			//When fault is set it's disabled till Cleared by corresponding warn condition
			//vout_uv_fault is re-enabled AT_TARGET
			FAULT_DRV_App->enable_mask_driver |= (1u<<(uint32_t)fault_code_retry);
			FAULT_DRV_App->fault_enable = FAULT_DRV_App->enable_mask_driver &
												FAULT_DRV_App->enable_mask &
													FAULT_DRV_App->enable_mask_pmbus;

			if (hw_fault)
			{
				FAULT_FAULT_ENABLE_LOOP__SET((fault_idx_t) loop, FAULT_DRV_App->fault_enable);
			}

			//higher_priority_fault is saved in fault code previous
			if (fault_type_retry == FAULT_DRV_App->last_high_priority_fault)
			{
				//
				// PRIORITY IS GIVEN TO SHUTDOWN WITH MIN RETRY COUNT
				// Priority is given to FAULT_RESPONSE_SHUTDOWN_RESTART_WHEN_REMOVED response type
				//

				if ((1u << fault_code_retry) & FAULT_DRV_App->fault_status_event_latch)
				{
					// Not Hysteresis type: There was a shutdown due to this fault//
					//Hysteresis type: There was a shutdown due to fault corresponding to this warn//
					// COMMUNICATE RETRY TO REGULATION THREAD
					REGULATION_INTER_THREAD_SET(loop, REGULATION_CONTROLS_FAULT, ((uint32_t)FAULT_CONTROL_FAULT_CLEARED_RETRY|((uint32_t)fault_type_retry & 0xff)));
				}
			}

			// Clear Latched Fault Event to detect another //
			FAULT_DRV_App->fault_status_event_latch &= ~((1u<<(uint32_t)fault_code_retry));
		}

		////////////////////////
		///POLARITY AND CLEAR///
		////////////////////////
		FAULT_DRV_App->fault_status &= ~(1u<<(uint32_t)fault_code);
		FAULT_DRV_App->fault_polarity ^= 1u<<(uint32_t)fault_code;
		if (hw_fault)
		{
			//Disable shutdown output while changing polarity and status
			FAULT_FAULT_SHUT_MASK_LOOP__SET((fault_idx_t) loop, (FAULT_DRV_App->fault_shut_mask & ~(1u<<(uint32_t)fault_code)));
			FAULT_FAULT_T2_SHUT_MASK__SET((fault_idx_t) loop, (FAULT_DRV_App->fault_shut_mask & ~(1u<<(uint32_t)fault_code)));

			FAULT_FAULT_POLARITY_LOOP__SET((fault_idx_t) loop, FAULT_DRV_App->fault_polarity);
			FAULT_FAULT_CLEAR_LOOP__SET((fault_idx_t) loop, (1u<<(uint32_t)fault_code));
			//Restore shutdown mask
			Faults_Set_Shutdown_Mask_Hardware_Faults(loop, FAULT_DRV_App->fault_shut_mask);
		}
	}
	//
	// END OF PROCESS LOOP.
	//
}

/*****************************************************************************/
/**
* This function initializes the GPIO instance/driver.
* All members of the GPIO instance bank are initialized.
* All interrupt pending for the GPIO instance bank are reset.
* In this patch, a call to zsc_gpio_config() is added to
* configure the additional GPIO pins for ZSC (PWM1, PWM2).
*
* @param    Bank points to the GPIO bank to be initialized.
*
* @return   None
*
******************************************************************************/
void patch_GPIO_Drv_init(gpio_idx_t Bank) {
    /*python automated patch entry*/

    /** Setup GPIO's pins for the active bank specified by Bank */
    if (Bank == GPIO_0)
    {
        /* enable the GPIO0 clock */
        ClkControl_EnableClockGate(GPIO0_CLK);

        /* Release GPIO0 from reset */
        ClkControl_ClrRSTMODS(GPIO0RST);

        /* Clear sleep masks for GPIO0 */
        ClkControl_RstSEMsk(GPIO0_CLK);
    }
    else
    {
        /* enable the GPIO1 clock */
        ClkControl_EnableClockGate(GPIO1_CLK);

        /* Release GPIO1 from reset */
        ClkControl_ClrRSTMODS(GPIO1RST);

        /* Clear sleep masks for GPIO1 */
        ClkControl_RstSEMsk(GPIO1_CLK);
    }

    /* Clear all interrupt pending */
    GPIO_GPIOIC__SET(Bank, 0xFF);

    /* Clear GPIO error status */
    /*lint -esym(551, Gpio_drv_app) Supress "Symbol Gpio_drv_app not accessed" warning, Gpio_drv_app is accessed by functions in ROM Code */
    Gpio_drv_app.error_status = (GPIO_ERROR_TYPE) GPIO_ERROR_NONE;

    // call the configuration of PWM1 and PWM2 GPIO pins
    if (Bank == GPIO_0)
    {
        zsc_gpio_config();
    }

}

void patch_PMBUS_HANDLE_VOUT_SCALE_LOOP(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	//uint8_t is_vout_scale_loop_command;

	//is_vout_scale_loop_command = 1;

	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))  // recompute everything that depends on vout_scale_loop on writes:
	{
		// PMBUS command handles for the recompute of anything scaled by VOUT_SCALE_LOOP
		PMBUS_HANDLE_VOUT_DROOP(page, PMBUS_WRITE);				// these handles don't have OTP_RESTORE direction so send PMBUS_WRITE on restore
		PMBUS_HANDLE_VOUT_OV_WARN_LIMIT(page, PMBUS_WRITE);
		patch_PMBUS_HANDLE_VOUT_UV_WARN_LIMIT(page, PMBUS_WRITE);
		PMBUS_HANDLE_FW_CONFIG_REGULATION(page, PMBUS_WRITE);
		PMBUS_HANDLE_POWER_GOOD_ON(page, PMBUS_WRITE);
		//PMBUS_HANDLE_POWER_GOOD_OFF(page, direction);  // this just calls PMBUS_HANDLE_POWER_GOOD_ON so no need to replicate

		// These two commands trim and cal offset are tied to vout_command already covered further down in function
		//PMBUS_HANDLE_VOUT_TRIM(page, direction);
		//PMBUS_HANDLE_VOUT_CAL_OFFSET(page, direction);
		// on OTP restore don't send the inter thread vid change in vout_command:
		if (direction == PMBUS_WRITE)
		{
			// change vout telemetry scale:
			// called in telmetry init so not restored
			Telemetry_change_scales(page);

			// these are restored by FW_CONFIG_FAULTS
			PMBUS_HANDLE_VOUT_MIN(page, direction);
			PMBUS_HANDLE_VOUT_MAX(page, direction);
			PMBUS_HANDLE_VOUT_OV_FAULT_LIMIT(page, direction);
			patch_PMBUS_HANDLE_VOUT_UV_FAULT_LIMIT(page, direction);

			PMBUS_HANDLE_VOUT_COMMAND(page, direction);  // finally issue a new vid here (margin high/low covered here)
		}
		// calculate the boost feed forward scales
		// vout_scale_factor (U0.13) = 2^24 / (VOUT_SCALE_LOOP(U0.16) * 25)
		uint32_t vout_scale = (PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VOUT_SCALE_LOOP));
		if (vout_scale==0) vout_scale=1;  // avoid divide by zero
		REGULATION_APP.vout_scale_factor[page] = (1<<24)/(vout_scale*25);

		// calculate the boost feed forward scales
		// vrect_scale_factor (U5.2) = 800 * MFR_VRECT_SCALE (U0.12) >> 10
		uint16_t vrect_cmd = (PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_VRECT_SCALE));
		int32_t vrect_scale_exponent = LINEAR11_TO_EXPONENT(vrect_cmd);
		uint32_t vrect_scale = (uint32_t)LINEAR11_TO_MANTISSA(vrect_cmd);
		vrect_scale = 800 * vrect_scale;
		vrect_scale = SHIFT_EXPONENT_UNSIGNED(vrect_scale, (vrect_scale_exponent+2));
		REGULATION_APP.vrect_scale_factor[page] = vrect_scale;
	}
}

