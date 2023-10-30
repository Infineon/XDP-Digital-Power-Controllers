/*
 * scp_retry.h
 *
 *  Created on: 23 Sep 2020
 *      Author: halimand
 */

#ifndef SRC_USER_APP_SCP_RETRY_H_
#define SRC_USER_APP_SCP_RETRY_H_

#define SCP_RETRY


#ifdef SCP_RETRY
#define FAULT_TYPE_SCP fw_spare1
#define SCP_COMMON_FAULT_BIT 6



void patch_COMMON_FAULT_HANDLE(FAULT_TYPE_t fault_code);
void PMBUS_HANDLE_MFR_SCP_FAULT_RESPONSE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif

#endif /* SRC_USER_APP_SCP_RETRY_H_ */
