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


#ifndef COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_
#define COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_

//#include "add_on_features.h"
#include "faults_api.h"
//#include "buck_boost.h"

/**
 * Driver global variable structure for easy debug
 */
//extern REGULATION_MODULE_t REGULATION_APP;
/**
 * Local queue for priority reordering
 */
//extern regulation_inter_thread_message_t REGULATION_APP_QUEUE[REGULATION_QUEUE_LENGTH];

typedef struct {

	//===========
	// for high frequency iout
	uint16_t fs;;  // u11.0 format 1khz LSB
	uint8_t pwm1_deadtime_rise;;		// u6.2	or 	LSB = 1.25ns
	uint8_t pwm1_deadtime_fall;;
	int32_t iout_gain;
	int32_t iout_offset;
	uint16_t vin_telem_in_iout_exp;
	uint8_t iout_telem_offset_correction_slope_factor;
	int8_t iout_telem_offset_correction_slope_factor_exp;
	uint16_t iout_telem_offset_correction_factor;
	int8_t iout_telem_offset_correction_factor_exp;
	//===========

	uint8_t pwm_skip_counter;
	uint8_t pwm_skip_max;

	uint8_t pwm_skip_irq_rate;
	uint8_t pwm_skip_flag;

	int16_t iout[NUM_LOOPS];	//!< current telemetry value for output current of each loop

	uint32_t calculated_droop;
	uint32_t current_droop;
	uint32_t enable_read_iout_adc_codes[NUM_LOOPS];

	int8_t iout_exp[NUM_LOOPS];
	uint8_t en_buck_boost_feed_forward;
	uint8_t enable_added_droop;
	uint8_t remove_added_droop_counter;

	FAULT_TYPE_t last_high_priority_fault[NUM_LOOPS]; //for patch

	/*
	 * Parameters below are for buck-boost patch
	 */
	uint8_t shutdown_status;

	// +++++++++++++++++++++++++++++++ Deadtime  +++++++++++++++++++++++++++++++
	uint8_t Startup_PWM2_PWM6_dr;
	uint8_t SteadyState_PWM2_dr;
	uint8_t SteadyState_PWM6_dr;
	uint8_t deadtime_flag;
	// +++++++++++++++++++++++++++++++ Deadtime  +++++++++++++++++++++++++++++++

	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
	uint16_t active_vin_trim_slope;
	uint16_t active_vin_trim_offset;

	uint16_t standby_vin_trim_slope;
	uint16_t standby_vin_trim_offset;

//	uint16_t vin_trim_slope;
//	uint16_t vin_trim_offset;
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++

	// Set your hysteresis
	uint8_t vin_hysteresis;	// LSB = 1 V // Cap max = 170V
	uint8_t iout_hysteresis;	// LSB = 0.25A => for 3 is 0.75A A // Cap max = 42.5A

	// Do not change
	// initial input voltage threshold
	uint8_t vin_p1_adjust;
	uint8_t vin_p2_adjust;

	// initial output current threshold
	uint8_t iout_p1_adjust;
	uint8_t iout_p2_adjust;
	uint8_t iout_p3_adjust;
    uint8_t iout_p4_adjust; //220504 changes

	// zvs threshold feature
	int8_t iout_zvs_threshold_exp;
	uint16_t iout_zvs_threshold_current;
	uint16_t iout_zvs_threshold_low;
	uint16_t iout_zvs_threshold_high;

	int8_t iout_zvs_hysteresis_exp;
	uint8_t zvs_status;
	uint16_t iout_zvs_hysteresis;

	uint8_t zvs_initialization_status;

	// light_load_features
	int8_t light_load_features_threshold_exp;
	uint16_t light_load_features_threshold_current;
	uint16_t light_load_features_threshold_low;
	uint16_t light_load_features_threshold_high;

	uint8_t light_load_features_status;
	uint8_t light_load_features_hysteresis;
	int8_t light_load_features_hysteresis_exp;

	uint8_t light_load_features_initialization_status;
	uint8_t light_load_features_mode;


//
//	uint8_t debug1;
//	uint8_t debug2;
//	uint8_t debug3;
//	uint8_t debug4;

	uint16_t zvs_current_value;

} USER_DATA_t;

extern USER_DATA_t user_data;

void regulation_sm_callbacks_init(void);

void TON_DELAY_ENABLE(PMBUS_PAGE_t loop);
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop);
void TON_RISE_ENABLE(PMBUS_PAGE_t loop);
void AT_TARGET_ENABLE(PMBUS_PAGE_t loop);
void AT_SHUTDOWN(PMBUS_PAGE_t loop);


#endif /* COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_ */
