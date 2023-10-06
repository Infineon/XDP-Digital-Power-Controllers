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
uint8_t* patch_configurator_pmbus_default_ptr_get(uint8_t loop);
#endif

#endif /* SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_ */
        

