/*
 * open_loop_LLC.c
 *
 *  Created on: 15 Jun 2020
 *      Author: halimand
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
//#include "shasta_hal_regulation.h"
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
//#include "user_ntc_temperature_lut.h"
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"
#include "uart_drv.h"                // uart driver functions
#include "pmbus_mfr_specific_handlers.h"

#include "open_loop_LLC.h"


void UpdateForceDuty(PMBUS_PAGE_t loop, uint8_t duty) {
	if(loop == PMBUS_PAGE_0){
		PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY__SET(duty);	//U0.8 // FORCE_DUTY
	}
	else{
		PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY__SET(duty);	//U0.8 // FORCE_DUTY
	}
}

void UpdateSwitchingPeriod(PMBUS_PAGE_t loop, uint16_t period){
	if (loop == PMBUS_PAGE_0){
		PWM_RAMP0_TSWITCH0__TSWITCH0__SET(period);
	}
	else{
		PWM_RAMP1_TSWITCH1__TSWITCH1__SET(period);
	}
}

void OpenLoopLLC_SoftStart_Enable(PMBUS_PAGE_t loop){

	uint16_t max_freq_khz, steady_freq_khz;
	int32_t max_freq_exp, steady_freq_exp;

	/* ---------------------------------------- modified on 27 Aug 2020 ---------------------------------------- */

	// 1. Get Maximum Switching Frequency from MFR_SOFTSTART_MAX_FREQUENCY command
	max_freq_khz = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_SOFTSTART_MAX_FREQUENCY);
	max_freq_khz = (uint16_t)LINEAR11_TO_MANTISSA(max_freq_khz);  // since we know exponent is 0 we can just grab mantissa
	max_freq_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_SOFTSTART_MAX_FREQUENCY));
	max_freq_khz = (uint16_t)SHIFT_EXPONENT(max_freq_khz, max_freq_exp);

	// 2. Get Steady State Switching Frequency from FREQUENCY_SWITCH command
	steady_freq_khz = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_FREQUENCY_SWITCH);
	steady_freq_khz = (uint16_t)LINEAR11_TO_MANTISSA(steady_freq_khz);  // since we know exponent is 0 we can just grab mantissa
	steady_freq_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_FREQUENCY_SWITCH));
	steady_freq_khz = (uint16_t)SHIFT_EXPONENT(steady_freq_khz, steady_freq_exp);

	if (max_freq_khz == 0) {
		max_freq_khz = steady_freq_khz;
	}

	// 3. Calculate the period of the Max Switching Frequency
	uint32_t max_freq_period = 1000000/(max_freq_khz*SWITCHING_PERIOD_LSB_NS);
	if (max_freq_period > 0x1ff) max_freq_period = 0x1ff;

	// 4. Calculate the period of the Steady State Switching Frequency
	uint32_t steady_freq_period = 1000000/(steady_freq_khz*SWITCHING_PERIOD_LSB_NS);
	if (steady_freq_period > 0x1ff) steady_freq_period = 0x1ff;

	// 5. Assign max_freq_period to user_data
	user_data.freq_max_period = (uint16_t) max_freq_period;

	// 6. Also assign freq_ramp_counter to user_data
	user_data.freq_ramp_counter = user_data.freq_max_period;

	// 7. Assign steady_state_frequency_period to user_data
	user_data.freq_switch_period = (uint16_t) steady_freq_period;

	UpdateSwitchingPeriod(loop, user_data.freq_max_period);
	/* ---------------------------------------- modified on 27 Aug 2020 ---------------------------------------- */


	// Enable FORCE DUTY
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY_EN__SET(1);
	PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY_EN__SET(1);

	// Force (update) duty
	UpdateForceDuty(loop, 0);

	// SET initial Phase to 0
	user_data.softstart_current_phase = 0;

	// SET sofstart counter
	user_data.softstart_counter = 0;
	user_data.softstart_pwm_duty = 0;

	// Get MAX_DUTY
	int32_t max_duty_man = LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(loop,PMBUS_CMDCODE_MAX_DUTY));
	int32_t max_duty_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(loop,PMBUS_CMDCODE_MAX_DUTY));
	uint32_t max_duty = (uint32_t)SHIFT_EXPONENT(max_duty_man, (max_duty_exp - RAMP_DC_MAX_EXPONENT));
	if (max_duty > 0xFF) max_duty = 0xFF;

//	user_data.softstart_max_duty = (uint16_t) max_duty;
	user_data.softstart_max_duty = (max_duty * NUM_FORMAT_DIFF_RATIO) >> NUM_FORMAT_DIFF_RATIO_SCALE;

	// Enable switching Interrupt and irq rate
//	Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, fsw_irq_rate_sel_64);
	Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, user_data.softstart_irq_rate_sel);



	/* ++++++++++++++++++++++++++++++ modified on 25 Sep 2020 ++++++++++++++++++++++++++++++ */
	// disable Interrupt if max frequency value < steady_state frequency.
	// Frequency is inversely proportional to period
	// if-else condition is max_freq_period > steady_state_freq_period
	if (user_data.freq_max_period > user_data.freq_switch_period) {
		// Force (update) duty
		UpdateForceDuty(loop, 0);

		// Disable switching Interrupt
		Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, fsw_irq_rate_sel_disabled);
	}
	/* ++++++++++++++++++++++++++++++ modified on 25 Sep 2020 ++++++++++++++++++++++++++++++ */
}

void OpenLoopLLC_SoftStart_Disable(PMBUS_PAGE_t loop){

	// SET initial Phase to 0
	user_data.softstart_current_phase = 0;

	// SET sofstart counter
	user_data.softstart_counter = 0;
	user_data.softstart_pwm_duty = 0;

	/* ------------------------------- modified on 2 Jul 2020 ------------------------------- */
	// Enable FORCE DUTY
	PWM_RAMP0_FORCE_DUTY__RAMP0_FORCE_DUTY_EN__SET(1);
	PWM_RAMP1_FORCE_DUTY__RAMP1_FORCE_DUTY_EN__SET(1);

	// Force (update) duty
	UpdateForceDuty(loop, 0);
	/* ------------------------------- modified on 2 Jul 2020 ------------------------------- */


	// Disable switching Interrupt
	Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, fsw_irq_rate_sel_disabled);
}

void OpenLoopLLC_SoftStart_IRQ_Handler(PMBUS_PAGE_t loop){
	/* ++++++++++++++++++++++++++++++ modified on 25 Sep 2020 ++++++++++++++++++++++++++++++ */
	// Frequency is inversely proportional to period
	// if-else condition is max_freq_period > steady_state_freq_period
	if (!(user_data.freq_max_period > user_data.freq_switch_period)) {

		if(user_data.softstart_current_phase < SOFTSTART_MAX_STAGE){
			// Increment softstart cycle counter
			user_data.softstart_counter++;

			// Make sure PWM duty is below Max Duty
			if (user_data.softstart_pwm_duty < user_data.softstart_max_duty) {
				// Increment duty cycle based on the specified stepsize
				user_data.softstart_pwm_duty += user_data.softstart_pwm_stepsize[user_data.softstart_current_phase];
			}

			// Update duty cycle with Force Duty
			UpdateForceDuty(loop, user_data.softstart_pwm_duty);

			// Check if softstart cycle counter hits the assigned cycle count at each phase
			if (user_data.softstart_counter >= user_data.softstart_cycle_count[user_data.softstart_current_phase]){
				user_data.softstart_current_phase++;	// Increase softstart current phase
				user_data.softstart_counter = 0;		// Reset softstart counter
			}
		}

		// Check if current phase exceed the maximum stage possible
		else {
			// if need for frequency modulation, this should be the place to program.
	//		Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, fsw_irq_rate_sel_disabled);		// Disable switching Interrupt

			// For frequency ramp down, the period of Freq_Switch must be more than the Freq_Max.
			if (user_data.freq_switch_period > user_data.freq_ramp_counter) { // freq_ramp_counter already set as Freq_Max during initialization.
				user_data.freq_ramp_counter++;  // freq_ramp_counter is incremented until it reaches freq_switch_period
				UpdateSwitchingPeriod(loop, user_data.freq_ramp_counter);
			}
			else {
				Regulation_setup_fsw_irq(loop, fsw_irq_idx_5, fsw_irq_rate_sel_disabled);		// Disable switching Interrupt
			}
		}

	}	/* ++++++++++++++++++++++++++++++ modified on 25 Sep 2020 ++++++++++++++++++++++++++++++ */
}

void PMBUS_HANDLE_MFR_SOFTSTART_MAX_FREQUENCY(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	uint16_t max_freq_khz, steady_freq_khz;
	int32_t max_freq_exp, steady_freq_exp;

	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))  // on writes change the switching frequency of the loop
	{
		// 1. Get Maximum Switching Frequency from MFR_SOFTSTART_MAX_FREQUENCY command
		max_freq_khz = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_SOFTSTART_MAX_FREQUENCY);
		max_freq_khz = (uint16_t)LINEAR11_TO_MANTISSA(max_freq_khz);  // since we know exponent is 0 we can just grab mantissa
		max_freq_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_SOFTSTART_MAX_FREQUENCY));
		max_freq_khz = (uint16_t)SHIFT_EXPONENT(max_freq_khz, max_freq_exp);

		// 2. Get Steady State Switching Frequency from FREQUENCY_SWITCH command
		steady_freq_khz = PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_FREQUENCY_SWITCH);
		steady_freq_khz = (uint16_t)LINEAR11_TO_MANTISSA(steady_freq_khz);  // since we know exponent is 0 we can just grab mantissa
		steady_freq_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_FREQUENCY_SWITCH));
		steady_freq_khz = (uint16_t)SHIFT_EXPONENT(steady_freq_khz, steady_freq_exp);

		if (max_freq_khz == 0) {
			max_freq_khz = steady_freq_khz;
		}

		// 3. Calculate the period of the Max Switching Frequency
		uint32_t max_freq_period = 1000000/(max_freq_khz*SWITCHING_PERIOD_LSB_NS);
		if (max_freq_period > 0x1ff) max_freq_period = 0x1ff;

		// 4. Calculate the period of the Steady State Switching Frequency
		uint32_t steady_freq_period = 1000000/(steady_freq_khz*SWITCHING_PERIOD_LSB_NS);
		if (steady_freq_period > 0x1ff) steady_freq_period = 0x1ff;

		// 5. Assign max_freq_period to user_data
		user_data.freq_max_period = (uint16_t) max_freq_period;

		// 6. Also assign freq_ramp_counter to user_data
		user_data.freq_ramp_counter = user_data.freq_max_period;

		// 7. Assign steady_state_frequency_period to user_data
		user_data.freq_switch_period = (uint16_t) steady_freq_period;

		// 8. Update Switching Period to TSWITCH register
		UpdateSwitchingPeriod(page, max_freq_period); // Use Max Freq as startup switching frequency

		// commands dependent on frequency switch:
		PMBUS_HANDLE_MAX_DUTY(page, direction);
	}
}

void PMBUS_HANDLE_MFR_SOFTSTART_IRQ_RATE_SEL(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction){
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE)) {

		uint8_t irq_rate_sel = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_SOFTSTART_IRQ_RATE_SEL);

		if (irq_rate_sel != 0) {
			user_data.softstart_irq_rate_sel = irq_rate_sel;
		}
		else {
			user_data.softstart_irq_rate_sel = 0x07;
		}
	}
}

void PMBUS_HANDLE_MFR_SOFTSTART_PWM_STEPSIZE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction){
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE)) {
		// 1. Get cycle counts from pmbus array. Format is direct 8-bit.
		uint8_t *pPWMStepSizeArrayPtr = PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_SOFTSTART_PWM_STEPSIZE);

		if (pPWMStepSizeArrayPtr != NULL) {
			// 2. Assign to user_data structure for storage.
			for (uint8_t i=0; i < SOFTSTART_MAX_STAGE; i++){
				if (pPWMStepSizeArrayPtr[i] != 0 || pPWMStepSizeArrayPtr[i] != 0xFF ) {
					user_data.softstart_pwm_stepsize[i] = pPWMStepSizeArrayPtr[i];
				}
				else {
					user_data.softstart_pwm_stepsize[i] = 0x01;
				}
			}
		}
	}
}

void PMBUS_HANDLE_MFR_SOFTSTART_CYCLE_COUNT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction){
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE)) {
		// 1. Get cycle counts from pmbus array. Format is direct 8-bit.
		uint8_t *pCycleCountArrayPtr = PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_SOFTSTART_CYCLE_COUNT);

		if (pCycleCountArrayPtr != NULL) {
			// 2. Assign to user_data structure for storage.
			for (uint8_t i=0; i < SOFTSTART_MAX_STAGE; i++){
				if (pCycleCountArrayPtr[i] != 0 || pCycleCountArrayPtr[i] != 0xFF) {
					user_data.softstart_cycle_count[i] = pCycleCountArrayPtr[i];
				}
				else {
					user_data.softstart_cycle_count[i] = 0x55;
				}
			}
		}
	}
}

//void PMBUS_HANDLE_MFR_DBG(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction){
//	if (direction == PMBUS_READ) {
//		// for testing. modify later. remove once patch is verified
//
//		PMBUS_Set_Command_Word(page, PMBUS_CMDCODE_MFR_DBG, user_data.freq_max_period);
//	}
//}

