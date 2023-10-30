/*
 * TELEM_DATA.c
 *
 *  Created on: Jun 12, 2017
 *      Author: dlewis4
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
//#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
//#include "cmsis_os_ext.h"                   // ARM::CMSIS:RTOS:Keil RTX ext
#include "log_app.h"             /* Logging of errors */
#include "periodic_functions.h"
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
USER_DATA_t user_data;

//DRIVER_DTIMER *drv4_info1;

//#define FAULT_TYPE_EXTERNAL fw_spare1

void loadline_adjustment(PMBUS_PAGE_t loop, uint32_t mohms);

DTIMER_CONFIG_t dtimer4_cfg1;
DRIVER_DTIMER *drv4_info1;

//void DTIMER4_Timer_IRQHandler(void);

void regulation_sm_callbacks_init(void)
{
	//Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN, pwm_boot_cap_startup);

	// Set the callback functions for the regulation state machine.
	// ------------------------------------------------------------------------------------------------------------
	/*
	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_ENABLE,
			AT_TARGET_ENABLE);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN,
			frequency_dither_disable);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			frequency_dither_disable);
			*/
#ifdef en_ishare
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_ENABLE,
			TON_RISE_ENABLE);
#endif

	//Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
	//		ramp_switching_frequency_pcmc_startup_enable);


	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,
			AT_TON_RISE_Efuse_tracking_enable);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_DELAY, REGULATION_CONTROLS_OPERATION_ENABLE,
				AT_TON_DELAY_Efuse_tracking_enable);

	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);

	//Regulation_set_regulation_event_cb(REGULATION_STATE_FAULT, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
	//			AT_SHUTDOWN);

	// Shutdwon immediately for all the cases
	Regulation_set_regulation_event_cb(REGULATION_STATE_TOFF_FALL, REGULATION_CONTROLS_SHUTDOWN ,
				AT_TOFF_FALL);

	Regulation_set_regulation_event_cb(REGULATION_STATE_AT_TARGET_VID, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE ,
				AT_TOFF_FALL);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE ,
				AT_TOFF_FALL);

#ifdef fsw_int_48v_thr
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_5, MFR_48V_OFF_THRESH_IRQ_Handler);
#else


#endif

	#ifdef en_ishare
	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN,
			AT_SHUTDOWN);

	Regulation_set_regulation_event_cb(REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,
			AT_SHUTDOWN);
	#endif
	#ifdef en_ishare
	Regulation_set_regulation_event_cb(REGULATION_STATE_TON_RISE, REGULATION_CONTROLS_VID_REACHED,
			TON_RISE_VID_REACHED);
	#endif


	// ------------------------------------------------------------------------------------------------------------

	// register the callback functions for fsw interrupts
#ifdef en_ishare
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_2, remove_added_droop_irq_callback);
#endif
	//Regulation_set_fsw_irq_event_cb(fsw_irq_idx_3, frequency_dither_irq_callback);
	/*
	// HSC patch
	Regulation_set_fsw_irq_event_cb(fsw_irq_idx_5, ramp_switching_frequency_pcmc_startup_irq_handle);

	// Setup and enable generic telemetry interrupt example
	user_data.VIN_Thresh_Exponent = -2;   //Set the threshold exponent
	Telemetry_Set_Irq_Threshold(TELEM_IRQ7, 96, user_data.VIN_Thresh_Exponent); //24V  in U11.2 - 96
	Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ7, 80, user_data.VIN_Thresh_Exponent); //20V  in U11.2 - 96

	// page - loop 0 Selects the loop for reporting. PMBUS_PAGE_t
	// telemetry irq source - VIN loop 0 Telemetry_irq_src_t
	// generic telemetry irq number - TELEM_IRQ7 (11) Telemetry_IRQ_TYPE_t
	// set callback function - Telemetry_IRQ_VIN_HANDLE
	Telemetry_Setup_Irq(PMBUS_PAGE_0, VIN_0, TELEM_IRQ7, Telemetry_IRQ_VIN_HANDLE);
	//Setup and enable interrupt
	// page - loop 1 Selects the loop for reporting. PMBUS_PAGE_t
	// telemetry irq source - VIN loop 1 Telemetry_irq_src_t
	// generic telemetry irq number - TELEM_IRQ8 (12) Telemetry_IRQ_TYPE_t
	// set callback function - Telemetry_IRQ_VIN_HANDLE
	Telemetry_Setup_Irq(PMBUS_PAGE_1, VIN_1, TELEM_IRQ8, Telemetry_IRQ_VIN_HANDLE);


	//Example of setting up the response on external fault to FAULT_RESPONSE_RETRY_SHUTDOWN with 6 hiccups
	//with no hysteresis

	FAULTS_SETUP_FAULT(
			PMBUS_PAGE_0 ,  //PMBUS_PAGE_t pmbus page to update status on
			FAULT_TYPE_EXTERNAL,
			0xB0, //fault response byte: Response[7:6], Retry_Setting{5:3] and Delay_Time[2:0]
			FAULTS_CONFIG_VOUT_DELAY_UNIT, //Config_Delay_Unit This is used to decode the fields in FW_CONFIG_FAULTS PMBUS command (Time unit for retry responses)
			VOLTAGE_RESPONSE_TYPE, //response_config This is used to decode response command byte. Voltage(Temp, TON MAX) type or CURRENT type response
			(uint8_t)PMBUS_CMDCODE_STATUS_MFR_SPECIFIC, //pmbus_status_command status command number STATUS_XXX used for reporting the fault on pmbus
			(uint8_t)PMBUS_STATUS_MFR_SPECIFIC_0, //pmbus_status_bit_position bit position in command STATUS_XXX used for reporting the fault on pmbus
			(FAULT_TYPE_t) FAULT_TYPE_EXTERNAL); //hysteresis sets the hysteresis FAULT_TYPE_t

*/

	patch_VOUT_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);
#if 0
	patch_VIN_UV_FAULT((PMBUS_PAGE_t)0, PMBUS_WRITE);
#endif

}

#ifdef en_ishare
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{

		added_droop_enable(loop);
}

void TON_RISE_ENABLE(PMBUS_PAGE_t loop)
{
	//ramp_switching_frequency_pcmc_startup_enable(loop);
		enable_ishare(loop);
}
#endif


/**
* Function executes when the target vid window is reached during soft start.
*/
/*lint -e715 */
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop)
{
	// ++++++++++++++++++++++++ Enable VOUT_UV Fault ++++++++++++++++++++++++
	Faults_Mask_Fault(loop, 1, // hw fault
							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN)), // set_mask
							0	// clear_mask
							);
	// ++++++++++++++++++++++++ Enable VOUT_UV Fault ++++++++++++++++++++++++
#ifdef fsw_int_48v_thr
	if (loop == 1)
	{
		MFR_48V_OFF_THRESH_enable(loop);
	}
#else
	boolean polarity;
	uint16_t en_thr_manntisa;
	int8_t en_thr_exponent;
	int32_t en_hys_manntisa;
	int8_t en_thys_exponent;

	// Setup and enable generic telemetry interrupt example
	en_hys_manntisa = LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_48V_OFF_THRESH));
	en_thys_exponent = (int8_t)LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_48V_OFF_THRESH));	// -1 or -2

	en_thr_manntisa = LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_48V_ON_THRESH));
	en_thr_exponent = (int8_t)LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_48V_ON_THRESH));	// -1 or -2

	Telemetry_Set_Irq_Threshold(TELEM_IRQ7, en_thr_manntisa, en_thr_exponent); //24V  in U11.2 - 96
	Telemetry_Set_Irq_Hysteresis_Threshold(TELEM_IRQ7, en_hys_manntisa, en_thys_exponent); //20V  in U11.2 - 96
	polarity = 1;		// waiting for below hyst
	Telemetry_Setup_Irq_with_polarity(loop, VIN_1, TELEM_IRQ7, Telemetry_IRQ_MFR_48V_Threshold_HANDLE,polarity);
	Telemetry_Enable_IRQ_Type(TELEM_IRQ7);

#endif
}





void AT_TON_RISE_Efuse_tracking_enable(PMBUS_PAGE_t loop)
{
    uint16_t delay;
    uint32_t load;
	uint32_t command;

	///int32_t exponent;
	//uint16_t mantissa;
	//If 48Vin voltage > VIN_ON (create a MFR command to config threshold), force pwm output by writing ramp0_force_duty= 246 (calculate the force_duty based on MAX_DUTY ). We want to start switching. Enable the PWM output. PWM_RISE, PWM_FALL and LOOP. PWM_ON
	//uint16_t duty;
	int32_t exponent;
	//uint32_t value;

	uint16_t duty;
	//int32_t exponent;
	uint32_t value;

    if(loop == PMBUS_PAGE_1)
    {
		// force the duty cycle based on the max_duty
		duty = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_MAX_DUTY);
		duty = (uint16_t)LINEAR11_TO_MANTISSA(duty);  // since we know exponent is 0 we can just grab mantissa
		exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_MAX_DUTY));
		value = (uint32_t)SHIFT_EXPONENT(duty, (exponent));  // Value is percentage
		value = (uint32_t)(value * 256) / 100 ;		// Convert this to a register of LSB
		if (value > 0xff) value = 0xff;  // cap to field max as high 99%
		if (value < 0xE6) value = 0xE6;  // cap to a low value of - 90 % - narrow range of duty is allowed 99% to 90 %


		//Regulation_Max_Duty_Set(page, duty, exponent);
		PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY__SET(value);
		PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY_EN__SET(1);

		// reset pwm_force_hi to 0x00. reset the pwm_force to default values.
		//COMMON_EN_REG1__EN_PWM0__SET(0x1);
		PWM_PWM_FORCE_HI_LO__SET(0x00);

		// start switching by turning on the PWM on
	#ifdef SYNC_PIN_ENABLED
		PWM_PWM_ON__PWM_ON__SET(0x03);					// Turn on PWM 1,2. PWM3 , 4, 5, and 6 are used in Loop1.
	#else
		PWM_PWM_ON__PWM_ON__SET(0x3C);					// Turn on PWM 3,4 and 5,6
	#endif

		//Regulation_Set_VID(loop, 0, 0, 0, 0, 1);		// immediatly turn on the PWM's by setting the Vc ramp registers
	   //   VCONTROL_VC_VRAMP1__VC_VRAMP_ON__SET((vcontrol_idx_t) loop, 1);
		//VCONTROL_VC_VRAMP1__SET((vcontrol_idx_t) loop, 1);
	   //   VCONTROL_VC_VRAMP1__VC_VRAMP_LOAD_IMMEDIATE__SET((vcontrol_idx_t) loop, 1);
		//PWM_PWM12_DEADTIME__SET(PWM_PWM12_DEADTIME__GET());  // required to trigger shadow registers to internal


		// waiting for the Efuse delay before turning on the Efuse output GPIO to
		command = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_CONFIG_DELAY);
		delay = (uint16_t)LINEAR11_TO_MANTISSA((uint16_t)command);
		exponent = LINEAR11_TO_EXPONENT((uint16_t)command);
		load = (uint32_t)SHIFT_EXPONENT((1000*delay) , (exponent));  // LSB for load is 10ns * 1000 = 10 us - the input to the command is multiples of 10 us.
		//Regulation_TON_TOFF_DELAY_Timer_Activation(REGULATION_APP.loop, load);  // use loop number as the timer number
		Dtimer4_Timer_Activation(loop, load + 2000, 0); // LSB for load is 10ns  200us
	/*
		if (loop==PMBUS_PAGE_0)
		{
			Telemetry_Enable_IRQ_Type(TELEM_IRQ14);
		}
		else
		{
			Telemetry_Enable_IRQ_Type(TELEM_IRQ15);
		}
		*/
    }
}
void AT_TON_DELAY_Efuse_tracking_enable(PMBUS_PAGE_t loop)
{
/*
	uint8_t pwm2_bit_index = 7; //bit5
	uint32_t pwm2_read;

	COMMON_MP_FUNC3__PWM2_FUNC__SET(1);  //GPIO0[5]
	COMMON_MP_FUNC3__PWM2_PD__SET(0); 	 //pulldown disabled
	COMMON_MP_FUNC3__PWM2_PU_N__SET(1);  //pullup disabled
	COMMON_MP_FUNC3__PWM2_PPEN__SET(0);  //Open Drain

	GpioBit_SetCnfg (GPIO_0, pwm2_bit_index);
	GpioBit_SetDir (GPIO_0, pwm2_bit_index, DIRECTION_INPUT);
	pwm2_read = GpioBit_DatRd(GPIO_0,pwm2_bit_index); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
	//1.  If PWM2 is high, make tlm0_vin_src_sel = 3 to measure 48V input voltage

	if (pwm2_read != 0)
	{
		// change the input source to measure PRISEN
		TELEM_TLM_CFG1__TLM_VIN_SRC_SEL__SET((telem_idx_t)0 , 3);
	}
	Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, fsw_irq_rate_sel_32);
	// delay by 10
	 *
	 */
    //uint16_t delay;
    //uint32_t load;
	//uint32_t command;

	///int32_t exponent;
	//uint16_t mantissa;
	//If 48Vin voltage > VIN_ON (create a MFR command to config threshold), force pwm output by writing ramp0_force_duty= 246 (calculate the force_duty based on MAX_DUTY ). We want to start switching. Enable the PWM output. PWM_RISE, PWM_FALL and LOOP. PWM_ON


   	// reset pwm_force_hi to 0x00. reset the pwm_force to default values.
/*
	if (loop==PMBUS_PAGE_0)
	{
		COMMON_EN_REG1__EN_PWM0__SET(0x1);
	}
	else
	{
		COMMON_EN_REG2__EN_PWM1__SET(0x1);
	}
*/
	VSEN_VSP2__VSP_ADC_BLANK__SET(1,1);
	// ++++++++++++++++++++++++ Disable VOUT_UV Fault ++++++++++++++++++++++++
	Faults_Mask_Fault(loop, 1, // hw fault
							0, // set_mask
							((1u<<(uint32_t)FAULT_TYPE_VOUT_UV_FAULT) |(1u<<(uint32_t)FAULT_TYPE_VOUT_UV_WARN))	// clear_mask
							);
	// ++++++++++++++++++++++++ Disable VOUT_UV Fault ++++++++++++++++++++++++


#ifdef SYNC_PIN_ENABLED
	//PWM_PWM_FORCE_HI_LO__SET(0x00);		//not forcing any PWM4 and PWM5 are on
#else
	PWM_PWM_FORCE_HI_LO__SET(0x18);
#endif

}

void AT_TOFF_FALL(PMBUS_PAGE_t loop)
{
    if(loop == PMBUS_PAGE_1)
    {
		// Send the GPIO EFUSE output signal
		uint8_t imon_pin_gpio_bit_index = 3; //bit3
	#ifdef SYNC_PIN_ENABLED
		COMMON_MP_FUNC2__IMON_FUNC__SET(1);  //IMon pin - GPIO0[3] (mp imon pin )
		COMMON_MP_FUNC2__IMON_PD__SET(0); 	 //pulldown disabled
		COMMON_MP_FUNC2__IMON_PU_N__SET(1);  //pullup disabled
		COMMON_MP_FUNC2__IMON_PPEN__SET(1);  //CMOS output

		GpioBit_SetCnfg (GPIO_0, imon_pin_gpio_bit_index);
		GpioBit_SetDir (GPIO_0, imon_pin_gpio_bit_index, DIRECTION_OUTPUT);
		GpioBit_DatWr(GPIO_0, imon_pin_gpio_bit_index, 0x0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

	#else
		COMMON_MP_FUNC3__PWM1_FUNC__SET(1);  //GPIO0[5]


		COMMON_MP_FUNC3__PWM1_PD__SET(0); 	 //pulldown disabled
		COMMON_MP_FUNC3__PWM1_PU_N__SET(1);  //pullup disabled
		COMMON_MP_FUNC3__PWM1_PPEN__SET(1);  //CMOS output

		GpioBit_SetCnfg (GPIO_0, pwm1_bit_index);
		GpioBit_SetDir (GPIO_0, pwm1_bit_index, DIRECTION_OUTPUT);
		GpioBit_DatWr(GPIO_0, pwm1_bit_index, 0x0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
	#endif


		//check the fault type and perform PWM4 and pwm5 pull up for OCP and OVP  , not for UVP
		// Get pwm_force_hi to 0x18 - turn PWM 4 and pwm5 to 1 . after disable e-fuse.

	   //if((PMBUS_STATUS_IOUT__GET((pmbus_status_loop_idx_t)loop) & 0x80 ) || ((PMBUS_STATUS_VOUT__GET((pmbus_status_loop_idx_t)loop) & 0x80 )) || (PMBUS_STATUS_INPUT__GET((pmbus_status_loop_idx_t)loop) & 0x80 ))		// OCP || OVP || VINOV
	//	   	PWM_PWM_FORCE_HI_LO__SET(0x18);

		/*if (loop==PMBUS_PAGE_0)
		{
			COMMON_EN_REG1__EN_PWM0__SET(0x1);
		}
		else
		{
			COMMON_EN_REG2__EN_PWM1__SET(0x1);
		}*/

    }

}

void DTIMER4_Timer_IRQHandler(void)
{

    //if(loop == PMBUS_PAGE_1)
   // {
    	drv4_info1->ClearIRQ(DTIMER4);
    	drv4_info1->Uninitialize(DTIMER4);

    #ifdef SYNC_PIN_ENABLED
/*		uint8_t sync_pin_gpio_bit_index = 7; //bit5

		COMMON_MP_FUNC2__SYNC_FUNC__SET(1);  //SYNC pin - GPIO0[7]
    	COMMON_MP_FUNC2__SYNC_PD__SET(0); 	 //pulldown disabled
    	COMMON_MP_FUNC2__SYNC_PU_N__SET(1);  //pullup disabled
    	COMMON_MP_FUNC2__SYNC_PPEN__SET(1);  //CMOS output

    	GpioBit_SetCnfg (GPIO_0, sync_pin_gpio_bit_index);
    	GpioBit_SetDir (GPIO_0, sync_pin_gpio_bit_index, DIRECTION_OUTPUT);
    	GpioBit_DatWr(GPIO_0, sync_pin_gpio_bit_index, 0xFF); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
*/

		uint8_t imon_pin_gpio_bit_index = 3; //bit3
		COMMON_MP_FUNC2__IMON_FUNC__SET(1);  //IMon pin - GPIO0[3] (mp imon pin )
		COMMON_MP_FUNC2__IMON_PD__SET(0); 	 //pulldown disabled
		COMMON_MP_FUNC2__IMON_PU_N__SET(1);  //pullup disabled
		COMMON_MP_FUNC2__IMON_PPEN__SET(1);  //CMOS output

		GpioBit_SetCnfg (GPIO_0, imon_pin_gpio_bit_index);
		GpioBit_SetDir (GPIO_0, imon_pin_gpio_bit_index, DIRECTION_OUTPUT);
		GpioBit_DatWr(GPIO_0, imon_pin_gpio_bit_index, 0xFF); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location





    #else
    	uint8_t pwm1_bit_index = 5; //bit5

    	COMMON_MP_FUNC3__PWM1_FUNC__SET(1);  //GPIO0[5]
    	COMMON_MP_FUNC3__PWM1_PD__SET(0); 	 //pulldown disabled
    	COMMON_MP_FUNC3__PWM1_PU_N__SET(1);  //pullup disabled
    	COMMON_MP_FUNC3__PWM1_PPEN__SET(1);  //CMOS output

    	GpioBit_SetCnfg (GPIO_0, pwm1_bit_index);
    	GpioBit_SetDir (GPIO_0, pwm1_bit_index, DIRECTION_OUTPUT);
    	GpioBit_DatWr(GPIO_0, pwm1_bit_index, 0xFF); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
    #endif

    	NVIC_ClearPendingIRQ((IRQn_Type) ((uint32_t) DTIMER1_0_IRQn + ((uint32_t)DTIMER4)) );
    //}
}

void Dtimer4_Timer_Activation (PMBUS_PAGE_t loop, uint32_t load, uint32_t bgload)
{
      if(loop == PMBUS_PAGE_1)
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


void AT_TARGET_ENABLE(PMBUS_PAGE_t loop)
{
	frequency_dither_enable(loop);
}

void AT_SHUTDOWN(PMBUS_PAGE_t loop)
{
	ramp_switching_frequency_pcmc_startup_disable(loop);
#ifdef en_ishare
	disable_ishare(loop);
#endif

	// added by Halim 13 Feb 2020
	//VCONTROL_VC_VRAMP3__VC_VCONTROL_DELTA__SET((vcontrol_idx_t) loop, 0);
	// added by Halim 13 Feb 2020
}

/**
 * Callback function for generic telemetry interrupt on VIN source.
 * Can be used to adjust deadtime or pid settings based on changes in VIN
 * @param Telemetry_type @ref Telemetry_IRQ_TYPE_t telemetry type to operate on
 */
void Telemetry_IRQ_VIN_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type) {

	_Telemetry_s * Telemetry_str = Telemetry_GET_ARRAY_POINTER(Telemetry_type) ;
	//Grab the loop on which interrupt occurred
	//uint8_t loop = Telemetry_str->loop;

	if ((~Telemetry_str->polarity)&1) {
        //threshold
	}
	else {
		//Hysteresis threshold
	}

}

/**
 * example of loadline adjustment with PMBUS_Q_EXP
 * @param loop pmbus page to operate on
 * @param mohms @ref PMBUS_Q_EXP format number in mohms
 */
void loadline_adjustment(PMBUS_PAGE_t loop, uint32_t mohms)
{
	uint32_t data;
	// convert from PMBUS_Q_EXP to LOADLINE_EXP
	data = (uint32_t)SHIFT_EXPONENT((int32_t)mohms, (PMBUS_Q_EXP - LOADLINE_EXP));
	// convert to linear11 format, 5bit exponent, 11bit mantissa
	data = (uint32_t)(((uint32_t)TWOS_COMPLEMENT (5, LOADLINE_EXP)<<11) | (data & 0x3ff));  // unsigned is 0x3ff
	PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_VOUT_DROOP, (uint16_t)data);
	// send a pmbus command update to vout_command to execute callback on command:
	PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)loop,
			PMBUS_CMDCODE_VOUT_DROOP, PMBUS_WRITE);
}

/**
 * Function for reporting both an error to log buffer and pmbus status.
 * This can only be used for non fatal runtime errors that can be recoverable.
 * source is always USER type @ref Log_Source_t.
 * @param program_counter Current program counter logged.  Usually pass PROGRAM_COUNTER
 * @param cml_status @ref PMBUS_CML_STATUS_e cml error type defined by pmbus spec, value
 * provided will be used as the bit to set in STATUS_CML pmbus command
 */
void USER_Log_Error(uint32_t program_counter, PMBUS_CML_STATUS_e cml_status)
{
	PMBUS_Log_Error(USER, program_counter, cml_status);
}


#ifdef fsw_int_48v_thr
void MFR_48V_OFF_THRESH_IRQ_Handler(PMBUS_PAGE_t loop)
{

	uint8_t en_efuse;

	if (loop == 1)
	{
		en_efuse = enable_presence(loop,1);		// In ON mode - for loop 1 to look for the

		if (en_efuse == 1)	// vin is less than VIN_OFF - // In on mode, compare the result with 48v_off, if lower than 48v_off, turn off eFuse, shutdown loop1 pwms
		{
			Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, fsw_irq_rate_sel_disabled);
			//REGULATION_APP.next_state[REGULATION_APP.loop] = REGULATION_STATE_OFF;
			//REGULATION_INTER_THREAD_SET(REGULATION_APP.loop, REGULATION_CONTROLS_SHUTDOWN, REGULATION_APP.control_data[REGULATION_APP.loop]);
			// resend enable control from pmbus
			//PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)REGULATION_APP.loop, PMBUS_CMDCODE_OPERATION, PMBUS_POKE);

			//PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY__SET(255);
			//PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY_EN__SET(1);////

			PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY__SET(255);
			PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY_EN__SET(1);////
/*
			uint8_t sync_pin_gpio_bit_index = 7; //bit7
			COMMON_MP_FUNC2__SYNC_FUNC__SET(1);  //SYNC pin - GPIO0[7]
			COMMON_MP_FUNC2__SYNC_PD__SET(0); 	 //pulldown disabled
			COMMON_MP_FUNC2__SYNC_PU_N__SET(1);  //pullup disabled
			COMMON_MP_FUNC2__SYNC_PPEN__SET(1);  //CMOS output

			GpioBit_SetCnfg (GPIO_0, sync_pin_gpio_bit_index);
			GpioBit_SetDir (GPIO_0, sync_pin_gpio_bit_index, DIRECTION_OUTPUT);
			GpioBit_DatWr(GPIO_0, sync_pin_gpio_bit_index, 0x00); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

			*/

			uint8_t imon_pin_gpio_bit_index = 3; //bit3
			COMMON_MP_FUNC2__IMON_FUNC__SET(1);  //IMon pin - GPIO0[3] (mp imon pin )
			COMMON_MP_FUNC2__IMON_PD__SET(0); 	 //pulldown disabled
			COMMON_MP_FUNC2__IMON_PU_N__SET(1);  //pullup disabled
			COMMON_MP_FUNC2__IMON_PPEN__SET(1);  //CMOS output

			GpioBit_SetCnfg (GPIO_0, imon_pin_gpio_bit_index);
			GpioBit_SetDir (GPIO_0, imon_pin_gpio_bit_index, DIRECTION_OUTPUT);
			GpioBit_DatWr(GPIO_0, imon_pin_gpio_bit_index, 0x0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

		}
		else
		{
				//open_loop_llc_softstart_modes(loop);
		}
	}
}

void MFR_48V_OFF_THRESH_enable(PMBUS_PAGE_t loop)
{
	Regulation_setup_fsw_irq((PMBUS_PAGE_t)loop, fsw_irq_idx_5, fsw_irq_rate_sel_32);
}

#else

/**
 * Callback function for generic telemetry interrupt on VIN source.
 * Can be used to adjust deadtime or pid settings based on changes in VIN
 * @param Telemetry_type @ref Telemetry_IRQ_TYPE_t telemetry type to operate on
 */
void Telemetry_IRQ_MFR_48V_Threshold_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type)
{
	_Telemetry_s * Telemetry_str = Telemetry_GET_ARRAY_POINTER(Telemetry_type);
	PMBUS_PAGE_t page= PMBUS_PAGE_1;

	uint16_t mantissa = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VOUT_UV_FAULT_LIMIT);
	int32_t  exponent = PMBUS_Get_Vout_Mode_Exponent(page);
	//uint16_t hysteresis_mantissa = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_VOUT_UV_WARN_LIMIT);

   	if (~(Telemetry_str->polarity)&1) 				// if is executed vin is below the off threshold
   	{
   			Telemetry_Set_Irq_Polarity(Telemetry_type, 0);		// look for above and this sets the threshold as  threshold - Warn limt
   			//Regulation_Shutdown_Sequence (page, REGULATION_STATE_OFF, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE);
			//REGULATION_INTER_THREAD_SET(page, REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE, 0);
/*
		   	uint8_t  = 7; //bit7
		   	COMMON_MP_FUNC2__SYNC_FUNC__SET(1);  //SYNC pin - GPIO0[7]
		   	COMMON_MP_FUNC2__SYNC_PD__SET(0); 	 //pulldown disabled
		   	COMMON_MP_FUNC2__SYNC_PU_N__SET(1);  //pullup disabled
		   	COMMON_MP_FUNC2__SYNC_PPEN__SET(1);  //CMOS output

		   	GpioBit_SetCnfg (GPIO_0, sync_pin_gpio_bit_index);
		   	GpioBit_SetDir (GPIO_0, sync_pin_gpio_bit_index, DIRECTION_OUTPUT);
		   	GpioBit_DatWr(GPIO_0, sync_pin_gpio_bit_index, 0x00); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
		   	*/

   			uint8_t imon_pin_gpio_bit_index = 3; //bit3
   			COMMON_MP_FUNC2__IMON_FUNC__SET(1);  //IMon pin - GPIO0[3] (mp imon pin )
   			COMMON_MP_FUNC2__IMON_PD__SET(0); 	 //pulldown disabled
   			COMMON_MP_FUNC2__IMON_PU_N__SET(1);  //pullup disabled
   			COMMON_MP_FUNC2__IMON_PPEN__SET(1);  //CMOS output

   			GpioBit_SetCnfg (GPIO_0, imon_pin_gpio_bit_index);
   			GpioBit_SetDir (GPIO_0, imon_pin_gpio_bit_index, DIRECTION_OUTPUT);
   			GpioBit_DatWr(GPIO_0, imon_pin_gpio_bit_index, 0x00); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
   	}
   	else		//else is executed when threshold is above the on threshold
   	{
   		// if it above the on threshold
		Telemetry_Set_Irq_Polarity(Telemetry_type, 1);			//look for below
		REGULATION_INTER_THREAD_SET(PMBUS_PAGE_1, REGULATION_CONTROLS_OPERATION_ENABLE, 0);
   	}
}
#endif
