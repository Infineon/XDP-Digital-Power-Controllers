/*
 * user_app.h
 *
 *  Created on: Jun 13, 2017
 *      Author: dlewis4
 */

#ifndef COMMON_MODULES_USER_APP_USER_APP_H_
#define COMMON_MODULES_USER_APP_USER_APP_H_
#define N_ROWS   (128)
#include "pmbus_autogen.h"

#define SYNC_PIN_ENABLED

void user_drv_init(void);
void Dtimer5_Timer_Activation  (PMBUS_PAGE_t loop, uint32_t load, uint32_t bgload);
void DTIMER5_Timer_IRQHandler(void);

void patch_VOUT_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void patch_VIN_UV_FAULT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

uint8_t enable_presence(PMBUS_PAGE_t page, uint8_t mode);
//void patch_PMBUS_HANDLE_VIN_UV_FAULT_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
/*

*/
#endif /* COMMON_MODULES_USER_APP_USER_APP_H_ */
