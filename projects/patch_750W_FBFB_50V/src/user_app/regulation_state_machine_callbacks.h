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


#ifndef COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_
#define COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_

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

	uint32_t calculated_droop;
	uint32_t current_droop;
	uint32_t enable_read_iout_adc_codes[NUM_LOOPS];
	uint8_t enable_added_droop;
	uint8_t remove_added_droop_counter;

	uint16_t active_vin_trim_slope;
	uint16_t active_vin_trim_offset;
	uint16_t standby_vin_trim_slope;
	uint16_t standby_vin_trim_offset;

} USER_DATA_t;

extern USER_DATA_t user_data;

void regulation_sm_callbacks_init(void);
void TON_RISE_VID_REACHED(PMBUS_PAGE_t loop);
void TON_RISE_ENABLE(PMBUS_PAGE_t loop);
void AT_TARGET_ENABLE(PMBUS_PAGE_t loop);
void AT_SHUTDOWN(PMBUS_PAGE_t loop);

#endif /* COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_ */
