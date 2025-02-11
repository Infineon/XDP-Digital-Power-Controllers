/*
 * regulation_state_machine_callbacks.h
 *
 *  Created on: Jun 12, 2017
 *      Author: dlewis4
 */


#ifndef COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_
#define COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_

#include "open_loop_LLC.h"

//#include "add_on_features.h"
#include "faults_api.h"

#define VOUT_Thresh_Count_Max 3 //Number of thresholds used (Thresh0..2)

/**
 * Driver global variable structure for easy debug
 */
//extern REGULATION_MODULE_t REGULATION_APP;
/**
 * Local queue for priority reordering
 */
//extern regulation_inter_thread_message_t REGULATION_APP_QUEUE[REGULATION_QUEUE_LENGTH];

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

	uint32_t period_max[NUM_LOOPS];	//!< period used by frequency jitter feature
	uint32_t period_min[NUM_LOOPS];	//!< period used by frequency jitter feature
	uint32_t counter[NUM_LOOPS];	//!< counter used by frequency jitter feature
	uint8_t control[NUM_LOOPS];	//!< control used by frequency jitter feature
	uint8_t  up0_down1[NUM_LOOPS];	//!< flag for ramp up or down used by frequency jitter feature

	uint32_t calculated_droop;
	uint32_t current_droop;
	uint32_t enable_read_iout_adc_codes[NUM_LOOPS];
	uint8_t enable_added_droop;
	uint8_t remove_added_droop_counter;

	uint8_t  VOUT_Thresh_Count;
	int8_t   VOUT_Thresh_Exponent;
	uint32_t VOUT_Thresh[VOUT_Thresh_Count_Max];

	uint32_t otp_osc_trim;
	uint8_t otp_adc_offset[NUM_LOOPS];
	uint8_t password_locked;
	int8_t   VIN_Thresh_Exponent;
	uint32_t VIN_Thresh;
	uint32_t hash;

	FAULT_TYPE_t last_high_priority_fault[NUM_LOOPS]; //for patch

	uint32_t fsw_initial_period[NUM_LOOPS]; //!< startup value of period used by ramp_switching_frequency_pcmc_startup feature

	// ----------------------- OpenLoop LLC Softstart  -----------------------
	uint8_t softstart_irq_rate_sel;
	uint8_t softstart_pwm_stepsize[3];
	uint8_t softstart_pwm_duty;
	uint8_t softstart_max_duty;
	uint8_t softstart_counter;
	uint8_t softstart_cycle_count[3];
	uint8_t softstart_current_phase;

	uint16_t freq_max_period;
	uint16_t freq_switch_period;
	uint16_t freq_ramp_counter;
	// ----------------------- OpenLoop LLC Softstart  -----------------------


	// ----------------------- SCP Threshold Patch -----------------------
	uint8_t startup_scp_thres;
	uint8_t steady_scp_thres;
	// ----------------------- SCP Threshold Patch -----------------------
	// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++
		uint16_t active_vin_trim_slope;
		uint16_t active_vin_trim_offset;

		uint16_t standby_vin_trim_slope;
		uint16_t standby_vin_trim_offset;

	//	uint16_t vin_trim_slope;
	//	uint16_t vin_trim_offset;
		// +++++++++++++++++++++++++++++++ VIN TRIM  +++++++++++++++++++++++++++++++

} USER_DATA_t;

extern USER_DATA_t user_data;

//void user_regulation_cb_at_target(PMBUS_PAGE_t loop);
void regulation_sm_callbacks_init(void);
//void pwm_boot_cap_startup(void);
//void ton_rise_adjustment(PMBUS_PAGE_t loop, uint16_t ramp_rate_uv_per_us);
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop);
void TON_RISE_ENABLE(PMBUS_PAGE_t loop);
void AT_TARGET_ENABLE(PMBUS_PAGE_t loop);
void AT_TON_DELAY(PMBUS_PAGE_t loop);
void AT_SHUTDOWN(PMBUS_PAGE_t loop);
//FAULT_TYPE_t FAULTS_User_Resolve_Shutdown_Priority(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type_current, FAULT_TYPE_t fault_type_previous);

void Telemetry_IRQ_VIN_HANDLE(Telemetry_IRQ_TYPE_t telem_type);


#endif /* COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_ */
