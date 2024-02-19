/*
/*
 * pmbus_patch_configuration_data.h
 *
 ** Created on:  2024-02-19
 */
 /* ============================================================================
** Copyright (C) 2024 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/

#ifndef SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_
#define SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_

#define Patch_Pmbus_Table

#ifdef Patch_Pmbus_Table
extern void PMBUS_Restore_All(PMBUS_PAGE_t page, uint8_t * p);
void patch_PMBUS_HANDLE_RESTORE_DEFAULT_ALL(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

#endif /* SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_ */
        

