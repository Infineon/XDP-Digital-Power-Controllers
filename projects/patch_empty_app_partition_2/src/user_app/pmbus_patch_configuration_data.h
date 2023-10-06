/*
/*
 * pmbus_patch_configuration_data.h
 *
 *  Created on: 21 Jul 2022
 *      Author: seetweireniv
 */

#ifndef SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_
#define SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_

#define Patch_Pmbus_Table

#ifdef Patch_Pmbus_Table
extern void PMBUS_Restore_All(PMBUS_PAGE_t page, uint8_t * p);
void patch_PMBUS_HANDLE_RESTORE_DEFAULT_ALL(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

#endif /* SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_ */
        

