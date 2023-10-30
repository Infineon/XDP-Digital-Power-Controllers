/*
 * vin_trim.h
 *
 *  Created on: 25 Nov 2021
 *      Author: seetweireniv
 */

#ifndef SRC_USER_APP_VIN_TRIM_H_
#define SRC_USER_APP_VIN_TRIM_H_

#define vin_trim_feature_en

void PMBUS_HANDLE_MFR_VIN_TRIMMING_ACTIVE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void PMBUS_HANDLE_MFR_VIN_TRIMMING_STANDBY(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#ifdef vin_trim_feature_en
void Update_Vin_Trim(uint16_t slope, uint16_t offset);
#endif

#endif /* SRC_USER_APP_VIN_TRIM_H_ */
