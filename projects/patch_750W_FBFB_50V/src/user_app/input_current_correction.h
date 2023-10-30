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

#ifndef SRC_USER_APP_INPUT_CURRENT_CORRECTION_H_
#define SRC_USER_APP_INPUT_CURRENT_CORRECTION_H_

#include "driver_common.h"
#include "telemetry_api.h"
#include "pmbus_api.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_fault.h"

//#include "efficiency_table.h"

uint16_t calculate_i(uint32_t InputVoltage);
uint16_t calculate_j(uint32_t OutputPower);

uint32_t input_current_modification(int8_t vin_read_exp, int8_t iin_read_exp, int8_t power_read_exp, int8_t loop, uint32_t vin, uint32_t pout);

void patch_Telemetry_Sample(void);

#endif /* SRC_USER_APP_INPUT_CURRENT_CORRECTION_H_ */
