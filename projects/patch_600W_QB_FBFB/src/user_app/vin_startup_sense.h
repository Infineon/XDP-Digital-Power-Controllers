/*
 * vin_startup_sense.h
 *
 *  Created on: 15 Jul 2019
 *      Author: halimand
 */

#ifndef SRC_USER_APP_VIN_STARTUP_SENSE_H_
#define SRC_USER_APP_VIN_STARTUP_SENSE_H_


#define VIN_SEL_EN

/* BEFORE STARTUP -> state = OFF */
void Enable_PRISEN_Sensing(PMBUS_PAGE_t loop);

/* BEFORE STARTUP -> state = TON_DELAY */
void Calculate_Vin_from_PRISEN(PMBUS_PAGE_t loop);
void Enable_VRSEN_Sensing(PMBUS_PAGE_t loop);

#endif /* SRC_USER_APP_VIN_STARTUP_SENSE_H_ */
