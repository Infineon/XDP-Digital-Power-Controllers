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
*
 *      Author: halimand
 */

#ifndef SRC_USER_APP_DEADTIME_H_
#define SRC_USER_APP_DEADTIME_H_

#include "buck_boost.h"

#ifdef deadtime_feature_en
void Set_StartUp_Deadtime(PMBUS_PAGE_t loop);
void Set_SteadyState_Deadtime(PMBUS_PAGE_t loop);
#endif

void PMBUS_HANDLE_MFR_DEADTIME (PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void PMBUS_HANDLE_MFR_DBG_DEADTIME (PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);


#endif /* SRC_USER_APP_DEADTIME_H_ */
