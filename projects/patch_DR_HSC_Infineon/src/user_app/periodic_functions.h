/*
 * periodic_functions.h
 *
 *  Created on: Nov 8, 2017
 *      Author: dlewis4
 */

#ifndef SRC_USER_APP_PERIODIC_FUNCTIONS_H_
#define SRC_USER_APP_PERIODIC_FUNCTIONS_H_

void frequency_dither_enable(PMBUS_PAGE_t loop);
void frequency_dither_disable(PMBUS_PAGE_t loop);
void frequency_dither_irq_callback(PMBUS_PAGE_t loop);
void ramp_switching_frequency_pcmc_startup_enable(PMBUS_PAGE_t loop);
void ramp_switching_frequency_pcmc_startup_disable(PMBUS_PAGE_t loop);
void ramp_switching_frequency_pcmc_startup_irq_handle(PMBUS_PAGE_t loop);

#endif /* SRC_USER_APP_PERIODIC_FUNCTIONS_H_ */
