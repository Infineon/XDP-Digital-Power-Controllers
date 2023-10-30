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

#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"
#include "dtimer_drv.h"
//#include "telemetry_drv.h"
#include "telemetry_api.h"
#include "telemetry_irq_handlers.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "pmbus_api.h"
#include "pmbus_gpio_control.h"
#include "shasta_hal_vsen.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
//#include "shasta_hal_regulation.h"
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_isen.h"
#include "regulation_state_machine_callbacks.h"

#include "patch_rom_table.h"
//#include "rt_TypeDef.h"
//#include "rt_Mailbox.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_fault_common.h"
#include "add_on_features.h"
#include "dtimer_drv.h"
#include "shasta_hal_vcontrol.h"
//#include "ramp_data.h"
#include "user_app.h"
#include "string.h"
USER_DATA_t user_data;

DTIMER_CONFIG_t dtimer4_cfg1;
DRIVER_DTIMER *drv4_info1;


void regulation_sm_callbacks_init(void)
{

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
			AT_TON_RISE_Efuse_tracking_enable);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_OPERATION_ENABLE,
				AT_TON_DELAY_Efuse_tracking_enable);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_ENABLE,
			TON_RISE_IIN_OC_THRESHOLD_MODIFY);

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
				AT_TOFF_FALL);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN,
				AT_TOFF_FALL);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
				AT_TOFF_FALL);

	// register interrupt handler
	/*
	2.	Set 2-level OCP protection threshold.
	-	In ZSC startup, the second stage should stay off until 12V bus voltage reach PWRGD threshold. But if ZSC FETs was damaged or the second stage false turn-on while ZSC not fully powered up. It could cause large inrush current. We will set a low OCP threshold in ZSC startup to prevent damage of the e-fuse during startup. Change the OCP threshold to normal value when VOUT reaching ¼ VIN.
	-	Per Christian, the startup input current is around 1A in normal condition. With a factor of 4, we can set the startup IOUT_OC_FAULT_LIMIT = 5A. set IOUT_OC_WARN_LIMIT =2A. I think hard code the threshold is fine. @Chen Cheng-Wei (IFAM PSS DCDC PD TM AE 2 TCOM), you can verify the startup current level at no load condition. Update Venu the threshold based on experiment result.
	-	Use the low OCP threshold when enabling e-fuse.
	-	Restore normal OCP threshold (set by configuration, restore from OTP config), when VOUT > (¼ VIN – Vout_target_window).
	 */

#ifdef EFUSE_OCP_THRESHOLD
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_6, VOUT_VIN_tracking_IRQ_Handler);
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_7, vout_to_threshold_prebias_IRQ_Handler);
#endif

	patch_VOUT_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);

#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
	patch_VIN_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);
#endif

}

void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{
	Faults_Mask_Fault(loop, 1, // hw fault
							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN)), // set_mask
							0	// clear_mask
							);
}

void TON_RISE_IIN_OC_THRESHOLD_MODIFY(PMBUS_PAGE_t loop)
{
	int32_t exponent;
	//uint32_t value;
	uint16_t mantissa;
#ifdef EFUSE_OCP_THRESHOLD
	exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_IOUT_OC_WARN_LIMIT));
	mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_IOUT_OC_WARN_LIMIT));
	Fault_Threshold_Set(loop, FAULT_TYPE_IOUT_OC_WARN, (int32_t)mantissa, (int32_t)exponent);

	exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_IOUT_OC_FAULT_LIMIT));
	mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_IOUT_OC_FAULT_LIMIT));
	Fault_Threshold_Set(loop, FAULT_TYPE_IOUT_OC_FAULT, (int32_t)mantissa, (int32_t)exponent);

#endif
}

void AT_TON_RISE_Efuse_tracking_enable(PMBUS_PAGE_t loop)
{
	///int32_t exponent;
	//uint16_t mantissa;
	//If 48Vin voltage > VIN_ON (create a MFR command to config threshold), force pwm output by writing ramp0_force_duty= 246 (calculate the force_duty based on MAX_DUTY ). We want to start switching. Enable the PWM output. PWM_RISE, PWM_FALL and LOOP. PWM_ON
	//uint16_t duty;
	int32_t exponent;
	//uint32_t value;
	//uint16_t mantissa;
	uint16_t duty;
	//int32_t exponent;
	uint32_t value;

	// force the duty cycle based on the max_duty
	duty = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MAX_DUTY);
	duty = (uint16_t)LINEAR11_TO_MANTISSA(duty);  // since we know exponent is 0 we can just grab mantissa
	exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MAX_DUTY));
	value = (uint32_t)SHIFT_EXPONENT(duty, (exponent));  // Value is percentage
	value = (uint32_t)(value * 256) / 100 ;		// Convert this to a register of LSB
	if (value > 0xff) value = 0xff;  // cap to field max

	//Regulation_Max_Duty_Set(page, duty, exponent);
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY__SET(value);
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY_EN__SET(1);

   	// reset pwm_force_hi to 0x00. reset the pwm_force to default values.
   	//COMMON_EN_REG1__EN_PWM0__SET(0x1);
	//PWM_PWM_FORCE_HI_LO__SET(0x00);

	// restore the VIN UV FAULT limits by writing the VIN UV fault limit which checks the vina nd sets the thresholds
	//patch_VOUT_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);
	patch_VIN_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);

#ifdef EFUSE_OCP_THRESHOLD
	// Check E fuse Health
	//-	If vsp1_vout_fs < 80, execute pwm_on_set and  GpioBit_DatWr(GPIO_0, pwm1_bit_index, 0xFF)
	if (VSEN_VSP5__VSP_VOUT_FS__GET((vsen_idx_t)1) < 254)		// 80
	{
		// start switching by turning on the PWM on
		PWM_PWM_ON__PWM_ON__SET(0x3C);					// Turn on PWM 3,4 and 5,6

		GpioBit_DatWr((gpio_idx_t)GPIO_0, (uint8_t)SYNC_PIN_GPIO_BIT_INDEX, 0);
		if (loop == PMBUS_PAGE_0)
			vout_to_on_threshold_prebias_IRQ(loop);

	}
	else	//If vsp1_vout_fs > 80,
	{
		//keep PWM 3~6 off, skip enabling e-fuse. Set bit [6] of STATUS_MFR_SPECIFIC to 1.
		// The STATUS_BYTE should show OFF state.
		// send a GPIO signal  indicating e-fuse was shorted. We can use SYNC pin. Set SYNC high
		//PWM_PWM_ON__PWM_ON__SET(0x00);					// Turn off
		Regulation_PWM_Control(loop, REGULATION_PWMS_OFF);  // ALL FET's OFF


		uint32_t status_byte = PMBUS_Get_Command_Byte(loop, PMBUS_CMDCODE_STATUS_MFR_SPECIFIC);
		status_byte = status_byte | 0x40U;
		//PMBUS_Set_Command_Byte((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_STATUS_MFR_SPECIFIC, status_byte);


	   	GpioBit_DatWr((gpio_idx_t)GPIO_0, (uint8_t)SYNC_PIN_GPIO_BIT_INDEX, 0xFF);

	   	Regulation_Shutdown_Sequence (loop, REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE);
		REGULATION_INTER_THREAD_SET(loop, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, 0);
	}

    /* -----S-----imon IRQ---------- */

    	//}

#else

		// start switching by turning on the PWM on
		PWM_PWM_ON__PWM_ON__SET(0x3C);					// Turn on PWM 3,4 and 5,6

		GpioBit_DatWr((gpio_idx_t)GPIO_0, (uint8_t)SYNC_PIN_GPIO_BIT_INDEX, 0);
		// waiting for the Efuse delay before turning on the Efuse output GPIO to
		command = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_CONFIG_DELAY);
		delay = (uint16_t)LINEAR11_TO_MANTISSA((uint16_t)command);
		exponent = LINEAR11_TO_EXPONENT((uint16_t)command);
		load = (uint32_t)SHIFT_EXPONENT((1000*delay) , (exponent));  // LSB for load is 10ns * 1000 = 10 us - the input to the command is multiples of 10 us.
		//Regulation_TON_TOFF_DELAY_Timer_Activation(REGULATION_APP.loop, load);  // use loop number as the timer number
		Dtimer4_Timer_Activation(loop, load + 2000, 0); // LSB for load is 10ns  200us
    #endif

}
void AT_TON_DELAY_Efuse_tracking_enable(PMBUS_PAGE_t loop)
{

	Faults_Mask_Fault(loop, 1, // hw fault
							0, // set_mask
							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN))	// clear_mask
							);

	if (loop==PMBUS_PAGE_0)
	{
	    COMMON_EN_REG1__EN_PWM0__SET(0x1);
	}
	else
	{
		COMMON_EN_REG2__EN_PWM1__SET(0x1);
	}
}

/*lint -e{715}   Suppress: loop is not referenced*/
void AT_TOFF_FALL(PMBUS_PAGE_t loop)
{
    // Send the GPIO EFUSE output signal
   	GpioBit_DatWr(GPIO_0, PWM1_BIT_INDEX, 0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

   	//	// restore the VIN UV FAULT limits by writing the VIN UV fault limit which checks the vina nd sets the thresholds
	//patch_VOUT_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);
}

void DTIMER4_Timer_IRQHandler(void)
{
    drv4_info1->ClearIRQ(DTIMER4);
    drv4_info1->Uninitialize(DTIMER4);
    // Send the GPIO EFUSE output signal
    GpioBit_DatWr(GPIO_0, PWM1_BIT_INDEX, 0xFF); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

    // Enable UV fault once the Efuse turned on
	Faults_Mask_Fault(PMBUS_PAGE_0, 1, // hw fault
							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN)), // set_mask
							0	// clear_mask
							);
    NVIC_ClearPendingIRQ((IRQn_Type) ((uint32_t) DTIMER1_0_IRQn + ((uint32_t)DTIMER4)) );
}

void Dtimer4_Timer_Activation (PMBUS_PAGE_t loop, uint32_t load, uint32_t bgload)
{
      if(loop == PMBUS_PAGE_0)
      {
		   drv4_info1 = &Driver_DTIMER;
		   memset(&dtimer4_cfg1, 0, sizeof(dtimer4_cfg1));
		   dtimer4_cfg1.mode = ONE_SHOT_INT;
		   dtimer4_cfg1.load = load; //first interrupt
		   dtimer4_cfg1.bgload = bgload; //consecutive periodic interrupts
		   dtimer4_cfg1.dtimer_ISR = DTIMER4_Timer_IRQHandler;
          drv4_info1->Initialize(DTIMER4, dtimer4_cfg1);
      }
}


// return 1, if VOUT > (¼ VIN – Vout_target_window)
// else returns 0

uint8_t vin_to_vout_ratio_condition(PMBUS_PAGE_t loop)
{
	//uint32_t vin_vout_ration;
	uint32_t vout;
	int8_t vout_read_exp[NUM_LOOPS];
	uint32_t vout_target;
	vout_read_exp[loop] = (int8_t)PMBUS_Get_Vout_Mode_Exponent((PMBUS_PAGE_t)loop);


	int32_t threshold_exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_VOUT_TARGET));
	uint16_t threshold_mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_VOUT_TARGET));
	vout_target = (uint32_t)SHIFT_EXPONENT((int32_t)threshold_mantissa,  threshold_exponent - vout_read_exp[loop] );
	if (vout_target>0xFFFF) vout_target = 0xFFFF;
	if(vout_target ==0) vout_target = 0;

	//uint32_t vout_target_window = Regulation_Get_PMBUS_Configuration(loop, REGULATION_CONFIG_VOUT_TARGET_WINDOW);
	//vout_target_window = SHIFT_EXPONENT_UNSIGNED(vout_target_window, -8 - vout_read_exp[loop]);  // treat target window as -8 format convert to vout_mode



	vout = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, VOUT_TELEM);
	vout = (uint32_t)SHIFT_EXPONENT((int32_t)vout,  PMBUS_Q_EXP - vout_read_exp[loop]);  // convert to vout_mode format
	//vout = (uint32_t)SHIFT_EXPONENT((int32_t)vout,  PMBUS_Q_EXP);  // vout
	//vout = (uint16_t)PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_COMMAND);	//in VOUT_MODE format
	if (vout>0xFFFF) vout = 0xFFFF;
	if(vout ==0) vout = 1;

	if  ( vout > vout_target)
		return 1;
	else
		return 0;
}


void VOUT_VIN_tracking_MONITOR_IRQ(PMBUS_PAGE_t loop)
{
	if (loop == PMBUS_PAGE_0)
		Regulation_setup_fsw_irq((PMBUS_PAGE_t) loop, fsw_irq_idx_6, fsw_irq_rate_sel_64);

	//Regulation_fsw_irq_enable();  // enable NVIC
}

void VOUT_VIN_tracking_IRQ_Handler(PMBUS_PAGE_t loop)
{

	uint8_t en_6v;
	en_6v = vin_to_vout_ratio_condition(loop);
	int32_t exponent;
	uint16_t mantissa;

	if (en_6v == 1)	//-	Restore normal OCP threshold (set by configuration, restore from OTP config), when VOUT > (¼ VIN – Vout_target_window).
	{
		// restore OCP thresholds
		exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_IOUT_OC_WARN_LIMIT));
		mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_IOUT_OC_WARN_LIMIT));
		Fault_Threshold_Set(loop, FAULT_TYPE_IOUT_OC_WARN, (int32_t)mantissa, (int32_t)exponent);


		exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_IOUT_OC_FAULT_LIMIT));
		mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_IOUT_OC_FAULT_LIMIT));
		Fault_Threshold_Set(loop, FAULT_TYPE_IOUT_OC_FAULT, (int32_t)mantissa, (int32_t)exponent);

		if (loop == PMBUS_PAGE_0)
			Regulation_setup_fsw_irq(loop, fsw_irq_idx_6, fsw_irq_rate_sel_disabled);
	}
	else
	{

	}
}

void vout_to_on_threshold_prebias_IRQ(PMBUS_PAGE_t loop)
{
	if (loop == PMBUS_PAGE_0)
		Regulation_setup_fsw_irq((PMBUS_PAGE_t) loop, fsw_irq_idx_7, fsw_irq_rate_sel_64);
	//Regulation_fsw_irq_enable();  // enable NVIC
}

//	Turn on eFuse only when ZSC output is below certain threshold voltage (MFR_CONFIG_THRESHOLD) at initial start-up
// continously monitor  vout telemetry with a threshold ( MFR_VOUT_ON_THRESHOLD) if VOUT telemetry  < VOUT_ON_THRESHOLD_TURNON
// once efuse PWM1 is on - stop the interrupt

void vout_to_threshold_prebias_IRQ_Handler(PMBUS_PAGE_t loop)
{

	uint8_t en_6v;
	int32_t exponent;
	//uint16_t mantissa;
    uint16_t delay;
    uint32_t load;
	uint32_t command;

	en_6v = vout_to_threshold_prebias_condition(PMBUS_PAGE_0);
	//-	Turn on eFuse only when ZSC output is below certain threshold voltage (PMBUS_CMDCODE_MFR_VOUT_ON_THRESHOLD) during pre-bias.
	if (en_6v == 1)
	{
		// waiting for the Efuse delay before turning on the Efuse output GPIO to
		//command = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_CONFIG_DELAY);
		//delay = (uint16_t)LINEAR11_TO_MANTISSA((uint16_t)command);
		//exponent = LINEAR11_TO_EXPONENT((uint16_t)command);
		//load = (uint32_t)SHIFT_EXPONENT((1000*delay) , (exponent));  // LSB for load is 10ns * 1000 = 10 us - the input to the command is multiples of 10 us.
		//Dtimer4_Timer_Activation(loop, load + 2000, 0); // LSB for load is 10ns  200us

	    // Send the GPIO EFUSE output signal
	    GpioBit_DatWr(GPIO_0, PWM1_BIT_INDEX, 0xFF); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

	    // Enable UV fault once the Efuse turned on
		Faults_Mask_Fault(PMBUS_PAGE_0, 1, // hw fault
								((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN)), // set_mask
								0	// clear_mask
								);

		if (loop == PMBUS_PAGE_0)
			Regulation_setup_fsw_irq(loop, fsw_irq_idx_7, fsw_irq_rate_sel_disabled);
			VOUT_VIN_tracking_MONITOR_IRQ(loop);
	}

}

uint8_t vout_to_threshold_prebias_condition(PMBUS_PAGE_t loop)
{
	uint32_t vout,vout_02v;
	int32_t threshold_exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_VOUT_ON_THRESHOLD));
	uint16_t threshold_mantissa = (uint16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_VOUT_ON_THRESHOLD));
	int32_t  exponent = PMBUS_Get_Vout_Mode_Exponent(loop);

	vout_02v = (uint32_t)SHIFT_EXPONENT((int32_t)threshold_mantissa,  threshold_exponent - exponent );

	vout = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, VOUT_TELEM);
	vout = (uint32_t)SHIFT_EXPONENT((int32_t)vout,  PMBUS_Q_EXP - exponent);  // convert to vout_mode format
	if (vout>0xFFFF) vout = 0xFFFF;
	if(vout ==0) vout = 1;


	//-	Turn on eFuse only when ZSC output is below certain threshold voltage (MFR_CONFIG_THRESHOLD) at initial start-up. (pre-bias).
	if (vout < vout_02v)
		return 1;
	else
		return 0;

}
