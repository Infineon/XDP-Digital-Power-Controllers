/*
 * open_loop_LLC.h
 *
 *  Created on: 15 Jun 2020
 *      Author: halimand
 */

#ifndef SRC_USER_APP_OPEN_LOOP_LLC_H_
#define SRC_USER_APP_OPEN_LOOP_LLC_H_

#define SOFTSTART_MAX_STAGE 3

// Refer to 3_Open_Loop_Softstart_Halim.xlsx for detailed information
#define NUM_FORMAT_DIFF_RATIO_SCALE 5
#define NUM_FORMAT_DIFF_RATIO 41

void UpdateSwitchingPeriod(PMBUS_PAGE_t loop, uint16_t period);
void UpdateForceDuty(PMBUS_PAGE_t loop, uint8_t duty);

void OpenLoopLLC_SoftStart_Enable(PMBUS_PAGE_t loop);
void OpenLoopLLC_SoftStart_Disable(PMBUS_PAGE_t loop);
void OpenLoopLLC_SoftStart_IRQ_Handler(PMBUS_PAGE_t loop);

//void PMBUS_HANDLE_DELTA_FREQUENCY_SWITCH(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

//void PMBUS_HANDLE_MFR_DELTA_SOFTSTART_PWM_STEPSIZE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
//void PMBUS_HANDLE_MFR_DELTA_SOFTSTART_CYCLE_COUNT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
//void PMBUS_HANDLE_MFR_DELTA_DBG(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);




#endif /* SRC_USER_APP_OPEN_LOOP_LLC_H_ */
