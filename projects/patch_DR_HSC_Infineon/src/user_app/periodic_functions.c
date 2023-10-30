/*
 * periodic_functions.c
 *
 *  Created on: Nov 8, 2017
 *      Author: dlewis4
 */

#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"
#include "dtimer_drv.h"
#include "telemetry_drv.h"
#include "telemetry_irq_handlers.h"
#include "telemetry_api.h"
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
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "log_app.h"             /* Logging of errors */
#include "periodic_functions.h"

/**
 * Function to enable the frequency dither (spreading) feature
 * @param loop regulation loop to operate on.
 */
void frequency_dither_enable(PMBUS_PAGE_t loop)
{
	// if the pmbus command is 0 or we didn't calculate the limits disable feature
	if ((user_data.control[loop] != 0)&&(user_data.period_min[loop] != 0)&&(user_data.period_max[loop] != 0))
	{
		Regulation_setup_fsw_irq(loop, fsw_irq_idx_3, fsw_irq_rate_sel_16);
	}
}

/**
 * Function to disable the frequency dither (spreading) feature
 * @param loop regulation loop to operate on.
 */
void frequency_dither_disable(PMBUS_PAGE_t loop)
{
	// if config bits 0 or 1 are set then we need to not disable the fsw irq
	// otherwise we are interfering with ROM features
	Regulation_setup_fsw_irq(loop, fsw_irq_idx_3, fsw_irq_rate_sel_disabled);

	// restore the pmbus switching frequency of the loop
	PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)loop,
			PMBUS_CMDCODE_FREQUENCY_SWITCH, PMBUS_WRITE);
}

/**
 * Interrupt service callback for FSW_IRQn
 * This is used for current limited startup for cycle by cycle adjustment of limit
 * @param loop regulation loop to operate on.
 */
void frequency_dither_irq_callback(PMBUS_PAGE_t loop)
{
	uint32_t num_period_units;
	uint32_t current_period;

	// [7:4] of control is number of periods to step at where period is fsw/64
	num_period_units = user_data.control[loop] >> 4;

	// first we modulo divide the counter by the unit count to see if we are
	// a multiple of the base unit of 64 periods or not
	// we use num_period_units+1 to ensure that we can't divide by zero
	// figure out if we are a multiple of the num_period_units+1 value
	// otherwise we do nothing increment count and exit isr
	if ((user_data.counter[loop] % (num_period_units+1)) == 0)
	{
		// now grab the current period we are working from
		if (loop == PMBUS_PAGE_0)
			current_period = PWM_RAMP0_TSWITCH0__TSWITCH0__GET();
		else
			current_period = PWM_RAMP1_TSWITCH1__TSWITCH1__GET();

		// figure out which direction to go
		if ((user_data.up0_down1[loop] == 0) && (current_period >= user_data.period_max[loop]))
			user_data.up0_down1[loop] = 1;  // change to downward direction
		else if ((user_data.up0_down1[loop] == 1) && (current_period <= user_data.period_min[loop]))
			user_data.up0_down1[loop] = 0;

		// now do the actual step up or down
		if (user_data.up0_down1[loop]==0)
			current_period++;  // 20ns step size multiplied by number of steps in pmbus command
		else
			current_period--;

		// write the register in hal:
		if (loop == PMBUS_PAGE_0)
			PWM_RAMP0_TSWITCH0__TSWITCH0__SET(current_period);
		else
			PWM_RAMP1_TSWITCH1__TSWITCH1__SET(current_period);
	}
	user_data.counter[loop]++;  // keep track of interrupt count
}

/**
 * Function to enable the ramp_switching_frequency_pcmc_startup feature
 * @param loop regulation loop to operate on.
 */
void ramp_switching_frequency_pcmc_startup_enable(PMBUS_PAGE_t loop)
{
	uint32_t mode_control;
	uint32_t enable_fsw_ramp = (PMBUS_Get_Command_Byte(loop, PMBUS_CMDCODE_MFR_SS_RAMP_FSW))&1;

	uint32_t prebias_voltage = (uint32_t)Telemetry_get(loop, VOUT_TELEM);  // PMBUS_Q_EXP format
	uint32_t vin = (uint32_t)TELEM_TLM_VIN_LPF__GET((telem_idx_t)loop); //u7.4

	// get the transformer scale in -10 Q format
	uint32_t transformer_scale = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_TRANSFORMER_SCALE);
	int32_t exponent = LINEAR11_TO_EXPONENT((uint16_t)transformer_scale);
	transformer_scale = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)transformer_scale);
	transformer_scale = SHIFT_EXPONENT_UNSIGNED(transformer_scale, exponent + 10);

	// figure out the switching frequency:
	uint32_t switching_freq_khz = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_FREQUENCY_SWITCH);
	exponent = LINEAR11_TO_EXPONENT((uint16_t)switching_freq_khz); // likely 0 or +1
	switching_freq_khz = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)switching_freq_khz);
	// note this will not work with negative exponents truncation will occur
	switching_freq_khz = SHIFT_EXPONENT_UNSIGNED(switching_freq_khz, exponent);

	uint32_t min_pw_ns = 10 * (PMBUS_Get_Command_Byte(loop, PMBUS_CMDCODE_MFR_MIN_PW));  // 5ns lsb * 2 for threshold
	// Pre-bias startup pulse width = Vout_prebias/ (Vin * Transformer_scale) /Frquency_switch
	uint32_t prebias_pw_ns = SHIFT_EXPONENT_UNSIGNED(vin * transformer_scale, -4);  // result in -10 format
	prebias_pw_ns = SHIFT_EXPONENT_UNSIGNED(prebias_voltage / prebias_pw_ns, 10);  // result in PMBUS_Q_EXP format
	prebias_pw_ns = 1000000 * (prebias_pw_ns / switching_freq_khz);
	prebias_pw_ns = SHIFT_EXPONENT_UNSIGNED(prebias_pw_ns, PMBUS_Q_EXP);

	if (prebias_pw_ns >= min_pw_ns)
	{
		enable_fsw_ramp = 0;  // disable fsw ramping when prebias > min pw
	}

	// if just want to always enable in PCMC and save pmbus command:
	if (loop == PMBUS_PAGE_0)
		mode_control = PWM_RAMP_CONFIG__MODE_CONTROL_LOOP0__GET();
	else
		mode_control = PWM_RAMP_CONFIG__MODE_CONTROL_LOOP1__GET();

	// check if in pcmc and we are enabled by bit 1 of PMBUS_CMDCODE_MFR_SS_RAMP_FSW
	if ((mode_control != 0) && (enable_fsw_ramp!=0))
	{
		// store the initial period to restore later
		if (loop == PMBUS_PAGE_0)
		{
			user_data.fsw_initial_period[0] = PWM_RAMP0_TSWITCH0__TSWITCH0__GET();
			PWM_RAMP0_TSWITCH0__TSWITCH0__SET(0x1FFU);  // set the switching period to max (lowest fsw)
		}
		else
		{
			user_data.fsw_initial_period[1] = PWM_RAMP1_TSWITCH1__TSWITCH1__GET();
			PWM_RAMP1_TSWITCH1__TSWITCH1__SET(0x1FFU);  // set the switching period to max (lowest fsw)
		}

		Regulation_setup_fsw_irq(loop, fsw_irq_idx_4, fsw_irq_rate_sel_4);
	}
}

/**
 * Function to disable the ramp_switching_frequency_pcmc_startup feature
 * @param loop regulation loop to operate on.
 */
void ramp_switching_frequency_pcmc_startup_disable(PMBUS_PAGE_t loop)
{
	uint32_t mode_control;
	uint32_t enable_fsw_ramp = (PMBUS_Get_Command_Byte(loop, PMBUS_CMDCODE_MFR_SS_RAMP_FSW))&1;

	// if just want to always enable in PCMC and save pmbus command:
	if (loop == PMBUS_PAGE_0)
		mode_control = PWM_RAMP_CONFIG__MODE_CONTROL_LOOP0__GET();
	else
		mode_control = PWM_RAMP_CONFIG__MODE_CONTROL_LOOP1__GET();

	// check if in pcmc and we are enabled by bit 1 of PMBUS_CMDCODE_MFR_SS_RAMP_FSW
	if ((mode_control != 0) && (enable_fsw_ramp!=0))
	{
		Regulation_setup_fsw_irq(loop, fsw_irq_idx_4, fsw_irq_rate_sel_disabled);

		// restore the pmbus switching frequency of the loop
		PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)loop,
				PMBUS_CMDCODE_FREQUENCY_SWITCH, PMBUS_WRITE);
	}
}

void ramp_switching_frequency_pcmc_startup_irq_handle(PMBUS_PAGE_t loop)
{
	uint32_t current_period;
	if (loop == PMBUS_PAGE_0)
		current_period = PWM_RAMP0_TSWITCH0__TSWITCH0__GET();
	else
		current_period = PWM_RAMP1_TSWITCH1__TSWITCH1__GET();
	current_period--;
	// if the current period becomes the same or less than target we stop:
	if (current_period > user_data.fsw_initial_period[loop])
	{
		if (loop == PMBUS_PAGE_0)
			PWM_RAMP0_TSWITCH0__TSWITCH0__SET(current_period);
		else
			PWM_RAMP1_TSWITCH1__TSWITCH1__SET(current_period);
	}
	else
		ramp_switching_frequency_pcmc_startup_disable(loop);
}
