/*
 * regulation_state_machine_callbacks.h
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


#ifndef COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_
#define COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_

#define EFUSE_OCP_THRESHOLD
#define PREBIAS_CONDITION
//#define VOUT_Thresh_Count_Max 3 //Number of thresholds used (Thresh0..2)

typedef struct {
/*
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
	//uint32_t VOUT_Thresh[VOUT_Thresh_Count_Max];

	uint32_t otp_osc_trim;
	uint8_t otp_adc_offset[NUM_LOOPS];
	uint8_t password_locked;
	int8_t   VIN_Thresh_Exponent;
	uint32_t VIN_Thresh;
	uint32_t hash;

	FAULT_TYPE_t last_high_priority_fault[NUM_LOOPS]; //for patch

	uint32_t fsw_initial_period[NUM_LOOPS]; //!< startup value of period used by ramp_switching_frequency_pcmc_startup feature
*/
	uint8_t  board_trim_loaded;	//!< flag for board trim execution perfromed

	//int32_t OC_warn_exponent[2] ;
	//uint16_t OC_warn_mantissa[2];
	//uint16_t OC_fault_mantissa[2];
	//int32_t OC_fault_exponent[2];

} USER_DATA_t;

typedef enum mode_e
{
	OFF = 0u,
	ON = 1u,
} mode_t;

extern USER_DATA_t user_data;

void regulation_sm_callbacks_init(void);
void TON_DELAY_ENABLE(PMBUS_PAGE_t loop);
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop);
void TON_RISE_ENABLE(PMBUS_PAGE_t loop);
void TON_RISE_IIN_OC_THRESHOLD_MODIFY(PMBUS_PAGE_t loop);
//void AT_TARGET_ENABLE(PMBUS_PAGE_t loop);
//void AT_SHUTDOWN(PMBUS_PAGE_t loop);
void AT_TOFF_FALL(PMBUS_PAGE_t loop);

void AT_TON_DELAY_Efuse_tracking_enable(PMBUS_PAGE_t loop);
void Dtimer4_Timer_Activation (PMBUS_PAGE_t loop, uint32_t load, uint32_t bgload);
void DTIMER4_Timer_IRQHandler(void);
void AT_TON_RISE_Efuse_tracking_enable(PMBUS_PAGE_t loop);

uint8_t vin_to_vout_ratio_condition(PMBUS_PAGE_t loop);
void VOUT_VIN_tracking_MONITOR_IRQ(PMBUS_PAGE_t loop);
void VOUT_VIN_tracking_IRQ_Handler(PMBUS_PAGE_t loop);
uint8_t vout_to_threshold_prebias_condition(PMBUS_PAGE_t loop);

void vout_to_on_threshold_prebias_IRQ(PMBUS_PAGE_t loop);
void vout_to_threshold_prebias_IRQ_Handler(PMBUS_PAGE_t loop);


#endif /* COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_ */
