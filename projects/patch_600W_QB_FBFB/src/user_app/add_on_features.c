/*
 * ADD_ON_FEATURES.c
 *
 *  Created on: Mar 7, 2018
 *      Author: ashendar
 */

#include "telemetry_api.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "shasta_hal_tsen.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_analog.h"
#include "shasta_hal_common.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_isen.h"
#include "shasta_hal_fault_common.h"
#include "shasta_hal_fault.h"

#include "shasta_hal_vcontrol.h"
#include "shasta_hal_tstctrl.h"  	// ate test state
#include "log_app.h"             /* Logging of errors */
/* Standard library */
//#include <stdint.h>             /* uint*_t */
#include <string.h>             /* memset */
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"
#include "mmu_drv.h"                /* MMU_Initialization */ 
#include "driver_common.h"
#include "pmbus_mfr_autogen.h"
#include "regulation_state_machine_callbacks.h"   //venkatG addition

void add_on_features_init (void)
{

//venkat addition
#ifdef en_ishare
	patch_rom_table.patch_Regulation_Shutdown_Sequence = &patch_Regulation_Shutdown_Sequence;
#endif

}

//venkat addition
#ifdef en_ishare
void enable_ishare(PMBUS_PAGE_t loop)
{

	uint16_t data = PMBUS_Get_Command_Word(PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD);
	data = (uint16_t)LINEAR11_TO_MANTISSA(data);  									// grab mantissa
	uint8_t pwm11_bit_index = 6; 													//Macro for PWM11

	if (data !=0)
	{
		//when enabled Low
		COMMON_MP_FUNC5__PWM11_FUNC__SET(1);  										//GPIO0[6], This  is setting PWM11 as a function 1 = GPIO0[6]
		COMMON_MP_FUNC5__PWM11_PD__SET(0); 	 										//pulldown disabled
		COMMON_MP_FUNC5__PWM11_PU_N__SET(1);  										//pullup disabled

		GpioBit_SetCnfg (GPIO_0, pwm11_bit_index);									// Here it's setting GPIO0 to value 6 = GPIO[6]
		GpioBit_SetDir (GPIO_0, pwm11_bit_index, DIRECTION_OUTPUT);					//Direction is set here as O/P

		TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(1);									// IMON output current DAC enable
		COMMON_EN_REG3__EN_ISHARE__SET(1);											//Enable IMON based current sharing function

		// Setting PWM11 Low	// PWM11 will be high when converter is not switching

		GpioBit_DatWr(GPIO_0, pwm11_bit_index, 0x0); 								// gpio driver requires a value of 1 on bus to be 0xff in index of that data location
		COMMON_MP_FUNC5__PWM11_PPEN__SET(1);  										//CMOS output 0 -	open drain output

	}

	else

	{
		TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(0);									// IMON output current DAC disable
		COMMON_EN_REG3__EN_ISHARE__SET(0);											//disable for IMON based current sharing function
	}

	// This function is for passive current sharing. Added droop is calculated here
	calculate_droop(loop);

}

/*lint -e715 */
void disable_ishare(PMBUS_PAGE_t loop)
{

	uint16_t data = PMBUS_Get_Command_Word(PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD);
	data = (uint16_t)LINEAR11_TO_MANTISSA(data); 																		// grab mantissa of Ishare threshold

	TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(0);																			// IMON output current DAC disable
	COMMON_EN_REG3__EN_ISHARE__SET(0);																					//disable for IMON based current sharing function

	if (data !=0)
	{
		uint8_t pwm11_bit_index = 6; 																					//Micro for pwm 11
		COMMON_MP_FUNC5__PWM11_PD__SET(0); 	 																			//pulldown disabled
		COMMON_MP_FUNC5__PWM11_PU_N__SET(1);  																			//pullup disabled


		// PMW11 setting it High		// PWM11 will be high when converter is not switching
		GpioBit_SetCnfg (GPIO_0, pwm11_bit_index);
		GpioBit_SetDir (GPIO_0, pwm11_bit_index, DIRECTION_OUTPUT);
		COMMON_MP_FUNC5__PWM11_PPEN__SET(0);  																			//CMOS output 0 -	open drain output
		GpioBit_DatWr(GPIO_0, pwm11_bit_index, 0xFF); 																	//Gpio driver requires a value of 1 on bus to be 0xff in index of that data location

		COMMON_MP_FUNC5__PWM11_FUNC__SET(1);

	}
}


/**
 * Function to disable the added droop feature
 * @param loop regulation loop to operate on.
 */
void added_droop_disable(PMBUS_PAGE_t loop)
{
	Regulation_setup_fsw_irq(loop, fsw_irq_idx_2, fsw_irq_rate_sel_disabled);
}

/**
 * Function to enable the added droop feature
 * @param loop regulation loop to operate on.
 */

void added_droop_enable(PMBUS_PAGE_t loop)
{
	if (user_data.enable_added_droop != 0)
	{
		if(loop==PMBUS_PAGE_0)
		{
			user_data.remove_added_droop_counter = 0;														//reset counter
			Regulation_setup_fsw_irq(loop, fsw_irq_idx_2, fsw_irq_rate_sel_8);								// enalbe the irq every 8 cycles
		}
	}
}


/**
 * Interrupt service callback for FSW_IRQn
 * This is used removed the added droop at startup
 * by slowly decreasing it syncronized with fsw
 * @param loop regulation loop to operate on.
 */
void remove_added_droop_irq_callback(PMBUS_PAGE_t loop) 													// this should be done in Regulation state
{

	uint32_t mohms = ((user_data.current_droop - user_data.calculated_droop)>>1U) -1;
	user_data.current_droop -= mohms;

	if (user_data.current_droop > 0x7ff) user_data.current_droop = 0x7ff;  									// cap at max

	if ((user_data.current_droop <= user_data.calculated_droop) || (mohms == 0))
	{
		user_data.current_droop = user_data.calculated_droop;

		user_data.enable_added_droop = 0; 																	//enable periodic irq at target to remove droop
		Regulation_setup_fsw_irq(PMBUS_PAGE_0, fsw_irq_idx_2, fsw_irq_rate_sel_disabled);					// disable the interrupt
	}

	user_data.remove_added_droop_counter++;  																// keep track of interrupt count
	VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) PMBUS_PAGE_0, (uint32_t)user_data.current_droop);

}

void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control)
{
	//////////////SHUTDOWN SEQUENCE//////////////
	//Mask faults
	Regulation_Fault_Mask (loop, state, control);
	Regulation_PWM_Control(loop, REGULATION_PWMS_OFF); 														// ALL FET's OFF
	Regulation_Set_VID(loop, 0, 0, 0, 0, 1);
	VCONTROL_VC_VRAMP1__VC_VRAMP_ON__SET((vcontrol_idx_t)loop, 0);  										// turn off vramp_on signal to allow pwms to stop regulation
	VCONTROL_VC_IRQ__VC_VCONTROL_IRQ_EN__SET((vcontrol_idx_t) loop, 0);										// clear the interrupt here in case it was left over from a fault

	Regulation_Power_Mode (loop, state);																	//disable the hardware blocks: (telemetry and faults are always active but masked at different times)

	// disable power good and ton_max interrupts
	TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET() &
																	~((1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_0) |
																			(1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_1)));


	PMBUS_STATUS_OFF__SET((pmbus_status_loop_idx_t)loop, 1);												// update pmbus status off bit:
	PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_STATUS_WORD, PMBUS_READ);	// update status word from result:
	FAULT_FAULT_SHUT_CLR_LOOP__FAULT_SHUT_CLR_LOOP__SET((fault_idx_t) REGULATION_APP.loop, 1) ;				// Clear HW Fault Shutdown mask

	if (loop == PMBUS_PAGE_0)
	{
	FAULTCOM_FAULT_SHUT_CLR_COM__FAULT_SHUT_CLR_COM__SET(1) ;
	}
	///////////////////////////////

	disable_ishare(PMBUS_PAGE_0);
}


/**
 * Function calculate_added_droop
 * @param loop : Loop 0 or Loop 1
 * @return : void
 * Feature : Calculate the added droop at the ramp. This function uses PMBUS_MFR_ADDED_DROOP_DURING_RAMP command and Vout_Droop command and
 * gets value from user and calculate the required mohms.
 *
 * If value from user is non zero, it will grab mantissa and exponent of both the PMBUS command and calculate mohm from it and assign to user data
 * Also, enables "enable_added_droop" which is used to remove droop in "enable_droop" function.
 *
 *Punit Edit
 */
void calculate_droop(PMBUS_PAGE_t loop)
{

		uint32_t mfr_added_vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP);				// Read mfr_added_droop
				 mfr_added_vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)mfr_added_vout_droop);							// Align VOUT_DROOP to PMBUS_Q_EXP precision

		if (mfr_added_vout_droop !=0)
		{
			user_data.enable_added_droop = 1; 																					//enable periodic irq at target to remove added droop


			// Read vout_scale and vout_droop
			uint32_t vout_scale = PMBUS_Get_Vout_Scale(loop, PMBUS_Q_EXP);
			uint32_t vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_DROOP);
			int32_t vout_droop_exp = LINEAR11_TO_EXPONENT((uint16_t)vout_droop);
			vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)vout_droop);													// Grab mantissa of vout droop

			mfr_added_vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP);
			int32_t mfr_added_vout_droop_exp = LINEAR11_TO_EXPONENT((uint16_t)mfr_added_vout_droop);
			mfr_added_vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)mfr_added_vout_droop);								// Grab mantissa of vout droop


			uint32_t vout_droop_mohms 			= (uint32_t)SHIFT_EXPONENT((int32_t)(vout_droop * vout_scale) , (-LOADLINE_EXP+vout_droop_exp+PMBUS_Q_EXP));								// multiply droop by vout scale and shift to LOADLINE_EXP
			uint32_t mfr_added_vout_droop_mohms = (uint32_t)SHIFT_EXPONENT((int32_t)(mfr_added_vout_droop * vout_scale) , (-LOADLINE_EXP+mfr_added_vout_droop_exp+PMBUS_Q_EXP));			// multiply droop by vout scale and shift to LOADLINE_EXP


			uint32_t mohms = vout_droop_mohms + mfr_added_vout_droop_mohms;
			user_data.calculated_droop = vout_droop_mohms;
			user_data.current_droop = mohms;

			if (mohms > 0x7ff) mohms = 0x7ff; 																					// cap at max
			VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) loop, mohms);
		}
}

#endif
