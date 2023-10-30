/*
 * pwm_skip.h
 *
 *  Created on: Feb 11, 2021
 *      Author: kotvytsr
 */

#ifndef SRC_USER_APP_PWM_SKIP_H_
#define SRC_USER_APP_PWM_SKIP_H_

void pwm_skip(void);
void pwm_unskip(void);
void pwm_skip_irq_handle(PMBUS_PAGE_t loop);
void pwm_skip_irq_enable(void);
void pwm_skip_irq_disable(void);
void PMBUS_HANDLE_MFR_BURST_CONFIG(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif /* SRC_USER_APP_PWM_SKIP_H_ */
