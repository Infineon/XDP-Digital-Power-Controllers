/*
 * zvs_feature.h
 *
 *  Created on: Nov 12, 2020
 *      Author: kotvytsr
 */

#ifndef SRC_USER_APP_ZVS_FEATURE_H_
#define SRC_USER_APP_ZVS_FEATURE_H_



void iout_zvs_threshold_irq_handle_enable(void);
//void iout_zvs_threshold_irq_handle_enable(uint16_t threshold );
void iout_zvs_threshold_irq_handle_disable(void);
void iout_zvs_threshold_irq_handle(PMBUS_PAGE_t loop);

void zvs_enable(mode_t mode);

void PMBUS_HANDLE_MFR_ZVS_DISABLE_THRESHOLD(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif /* SRC_USER_APP_ZVS_FEATURE_H_ */
