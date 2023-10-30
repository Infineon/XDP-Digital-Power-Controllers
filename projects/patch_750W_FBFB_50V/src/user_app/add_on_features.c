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
 *      Author: kotvytsr
 */

#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
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
//#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_isen.h"

/* Standard library */

#include "patch_rom_table.h"

#include "add_on_features.h"

#include "shasta_hal_tstctrl.h"  	// ate test state
#include "mmu_drv.h"                /* MMU_Initialization */

#include "driver_common.h"

#include "shasta_hal_fault_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"

#ifdef en_ishare
#include "regulation_state_machine_callbacks.h"
#include "sys_boot_descr_format.h"  /* boot descriptor bitfield definitions */
#include "shasta.h"
#include <stdint.h>             /* uint*_t */
#endif


void add_on_features_init (void)
{

#ifdef en_ishare
	patch_rom_table.patch_Regulation_Shutdown_Sequence = &patch_Regulation_Shutdown_Sequence;
#endif

}

#ifdef en_ishare

void enable_ishare(PMBUS_PAGE_t loop)
{

	uint16_t data = PMBUS_Get_Command_Word(PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD);
	data = (uint16_t)LINEAR11_TO_MANTISSA(data);  // grab mantissa
	uint8_t pwm11_bit_index = 6; //bit6

	if (data !=0)
	{
		//when enabled Low
		COMMON_MP_FUNC5__PWM11_FUNC__SET(1);  //GPIO0[6]
		COMMON_MP_FUNC5__PWM11_PD__SET(0); 	 //pulldown disabled
		COMMON_MP_FUNC5__PWM11_PU_N__SET(1);  //pullup disabled
		GpioBit_SetCnfg (GPIO_0, pwm11_bit_index);
		GpioBit_SetDir (GPIO_0, pwm11_bit_index, DIRECTION_OUTPUT);

		TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(1);// IMON output current DAC enable
		COMMON_EN_REG3__EN_ISHARE__SET(1);//disable for IMON based current sharing function

		// PMW11

		GpioBit_DatWr(GPIO_0, pwm11_bit_index, 0x0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
		COMMON_MP_FUNC5__PWM11_PPEN__SET(1);  //CMOS output 0 -	open drain output

	}

	else

	{
		TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(0);// IMON output current DAC enable
		COMMON_EN_REG3__EN_ISHARE__SET(0);//disable for IMON based current sharing function
	}

	// Read mfr_added_droop
	uint32_t mfr_added_vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP);

	// Align VOUT_DROOP to PMBUS_Q_EXP precision
	mfr_added_vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)mfr_added_vout_droop);

	if (mfr_added_vout_droop !=0)
	{
		user_data.enable_added_droop = 1; //enable periodic irq at target to remove droop
		// Read vout_scale and vout_droop
		uint32_t vout_scale = PMBUS_Get_Vout_Scale(loop, PMBUS_Q_EXP);

		uint32_t vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_DROOP);
		int32_t vout_droop_exp = LINEAR11_TO_EXPONENT((uint16_t)vout_droop);

		mfr_added_vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP);
		int32_t mfr_added_vout_droop_exp = LINEAR11_TO_EXPONENT((uint16_t)mfr_added_vout_droop);

		// Grab mantissa of vout droop
		vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)vout_droop);
		// multiply droop by vout scale and shift to LOADLINE_EXP
		uint32_t vout_droop_mohms = (uint32_t)SHIFT_EXPONENT((int32_t)(vout_droop * vout_scale) , (-LOADLINE_EXP+vout_droop_exp+PMBUS_Q_EXP));


		// Grab mantissa of vout droop
		mfr_added_vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)mfr_added_vout_droop);
		// multiply droop by vout scale and shift to LOADLINE_EXP
		uint32_t mfr_added_vout_droop_mohms = (uint32_t)SHIFT_EXPONENT((int32_t)(mfr_added_vout_droop * vout_scale) , (-LOADLINE_EXP+mfr_added_vout_droop_exp+PMBUS_Q_EXP));


		uint32_t mohms = vout_droop_mohms + mfr_added_vout_droop_mohms;
		user_data.calculated_droop = vout_droop_mohms;
		user_data.current_droop = mohms;

		if (mohms > 0x7ff) mohms = 0x7ff;  // cap at max
		VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) loop, mohms);
	}
}

/*lint -e715 */
void disable_ishare(PMBUS_PAGE_t loop)
{

	uint16_t data = PMBUS_Get_Command_Word(PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD);
	data = (uint16_t)LINEAR11_TO_MANTISSA(data);  // grab mantissa

	TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(0);// IMON output current DAC disable
	COMMON_EN_REG3__EN_ISHARE__SET(0);//disable for IMON based current sharing function

	if (data !=0)
	{
		uint8_t pwm11_bit_index = 6; //bit6
		COMMON_MP_FUNC5__PWM11_PD__SET(0); 	 //pulldown disabled
		COMMON_MP_FUNC5__PWM11_PU_N__SET(1);  //pullup disabled


		// PMW11
		GpioBit_SetCnfg (GPIO_0, pwm11_bit_index);
		GpioBit_SetDir (GPIO_0, pwm11_bit_index, DIRECTION_OUTPUT);
		COMMON_MP_FUNC5__PWM11_PPEN__SET(0);  //CMOS output 0 -	open drain output
		GpioBit_DatWr(GPIO_0, pwm11_bit_index, 0xFF); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

		COMMON_MP_FUNC5__PWM11_FUNC__SET(1);  //GPIO0[5]

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
	// if the pmbus command is 0 disable feature
	if (user_data.enable_added_droop != 0)
	{
		if(loop==PMBUS_PAGE_0)
		{
			user_data.remove_added_droop_counter = 0;
			// enalbe the irq every 8 cycles
			Regulation_setup_fsw_irq(loop, fsw_irq_idx_2, fsw_irq_rate_sel_8);
		}
	}
	//else
	//{
	//	added_droop_disable(loop);
	//}
}

/**
 * Interrupt service callback for FSW_IRQn
 * This is used removed the added droop at startup
 * by slowly decreasing it syncronized with fsw
 * @param loop regulation loop to operate on.
 */
void remove_added_droop_irq_callback(PMBUS_PAGE_t loop)
{

	uint32_t mohms = ((user_data.current_droop - user_data.calculated_droop)>>1U) -1;
	user_data.current_droop -= mohms;

	if (user_data.current_droop > 0x7ff) user_data.current_droop = 0x7ff;  // cap at max
	//if (user_data.current_droop < 0x0) user_data.current_droop = 0x0;  // cap at min

	if ((user_data.current_droop <= user_data.calculated_droop) || (mohms == 0))
	{
		user_data.current_droop = user_data.calculated_droop;
		//VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) loop, user_data.current_droop);
		// disable the interrupt
		user_data.enable_added_droop = 0; //enable periodic irq at target to remove droop
		Regulation_setup_fsw_irq(PMBUS_PAGE_0, fsw_irq_idx_2, fsw_irq_rate_sel_disabled);
	}

	user_data.remove_added_droop_counter++;  // keep track of interrupt count
	VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) PMBUS_PAGE_0, (uint32_t)user_data.current_droop);

}

void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control)
{
	//////////////SHUTDOWN SEQUENCE//////////////
	//Mask faults
	Regulation_Fault_Mask (loop, state, control);

	Regulation_PWM_Control(loop, REGULATION_PWMS_OFF);  // ALL FET's OFF
	Regulation_Set_VID(loop, 0, 0, 0, 0, 1);
	VCONTROL_VC_VRAMP1__VC_VRAMP_ON__SET((vcontrol_idx_t)loop, 0);  // turn off vramp_on signal to allow pwms to stop regulation
	// clear the interrupt here in case it was left over from a fault
	VCONTROL_VC_IRQ__VC_VCONTROL_IRQ_EN__SET((vcontrol_idx_t) loop, 0);

	//disable the hardware blocks: (telemetry and faults are always active but masked at different times)
	Regulation_Power_Mode (loop, state);

	// disable power good and ton_max interrupts
	TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET() &
																	~((1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_0) |
																			(1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_1)));

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

	disable_ishare(PMBUS_PAGE_0);
}
#endif
