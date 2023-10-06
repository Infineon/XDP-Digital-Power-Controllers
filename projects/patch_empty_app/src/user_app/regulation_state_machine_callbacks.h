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

} USER_DATA_t;

extern USER_DATA_t user_data;

void regulation_sm_callbacks_init(void);
void AT_SHUTDOWN(PMBUS_PAGE_t loop);


#endif /* COMMON_MODULES_REGULATION_REGULATION_STATE_MACHINE_CALLBACKS_H_ */
