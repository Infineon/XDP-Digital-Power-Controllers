/*
 * regulation_state_machine_callbacks.h
 *
 *  Created on: Jun 12, 2017
 *      Author: dlewis4
 */


#ifndef COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_
#define COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_


/**
 * Driver global variable structure for easy debug
 */

/**
 * Local queue for priority reordering
 */

typedef struct {
	uint32_t vout[NUM_LOOPS];	//!< current telemetry value for output voltage of each loop
	uint32_t iout[NUM_LOOPS];	//!< current telemetry value for output current of each loop
	uint32_t iin[NUM_LOOPS];	//!< current telemetry value for input current of each loop
	uint32_t vin[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t pout[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t pin[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t fsw[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t duty[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t prebias_voltage[NUM_LOOPS];	//!< current telemetry value for output voltage at startup
	uint32_t leakage_current[NUM_LOOPS];	//!< current telemetry value for output current at startup
	uint32_t cout[NUM_LOOPS];	//!< current telemetry value for output capacitance of each loop
	uint32_t lout[NUM_LOOPS];	//!< current telemetry value for output inductance of each loop
	uint32_t temperature[3];  	//!< current telemetry value for temperature, index: tempa = 0, tempb = 1, tempi = 2


	uint32_t otp_osc_trim;
	uint8_t otp_adc_offset[NUM_LOOPS];
	uint8_t password_locked;
	int8_t   VIN_Thresh_Exponent;
	uint32_t VIN_Thresh;
	uint32_t hash;



} USER_DATA_t;

extern USER_DATA_t user_data;

void regulation_sm_callbacks_init(void);
void AT_SHUTDOWN(PMBUS_PAGE_t loop);


#endif /* COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_ */
