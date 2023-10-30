/*
 * scp_thres.h
 *
 *  Created on: 30 Sep 2020
 *      Author: halimand
 */

#ifndef SRC_USER_APP_SCP_THRES_H_
#define SRC_USER_APP_SCP_THRES_H_


void PMBUS_HANDLE_MFR_STARTUP_SCP_THRESH(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void PMBUS_HANDLE_MFR_STEADY_SCP_THRESH(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif /* SRC_USER_APP_SCP_THRES_H_ */
