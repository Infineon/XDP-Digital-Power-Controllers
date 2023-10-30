/*
 * user_app.h
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

#ifndef COMMON_MODULES_USER_APP_USER_APP_H_
#define COMMON_MODULES_USER_APP_USER_APP_H_

#include "pmbus_autogen.h"

#define PWM1_BIT_INDEX 5
#define PWM2_BIT_INDEX 7
#define SYNC_PIN_GPIO_BIT_INDEX 7
void user_drv_init(void);

#define VIN_UV_FAULT_THREE_THRESHOLDS

void patch_VOUT_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
//void patch_PMBUS_HANDLE_VOUT_SCALE_LOOP(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#ifdef VIN_UV_FAULT_THREE_THRESHOLDS
void patch_VIN_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

#endif /* COMMON_MODULES_USER_APP_USER_APP_H_ */
