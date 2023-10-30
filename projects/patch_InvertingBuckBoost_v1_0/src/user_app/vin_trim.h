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
 *  Created on: 28 May 2020
 *      Author: halimand
 */

#ifndef SRC_USER_APP_VIN_TRIM_H_
#define SRC_USER_APP_VIN_TRIM_H_

#include "buck_boost.h"

void PMBUS_HANDLE_MFR_FLEX_VIN_TRIMMING_ACTIVE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void PMBUS_HANDLE_MFR_FLEX_VIN_TRIMMING_STANDBY(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);


#ifdef vin_trim_feature_en
void Update_Vin_Trim(uint16_t slope, uint16_t offset);
#endif


#endif /* SRC_USER_APP_VIN_TRIM_H_ */
