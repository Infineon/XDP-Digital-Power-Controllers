/*
 * pmbus_mfr_specific_handlers.h
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

#ifndef PMBUS_MFR_SPECIFIC_HANDLERS_H_
#define PMBUS_MFR_SPECIFIC_HANDLERS_H_

#include <stdint.h>              /* uint*_t */
#include "pmbus_autogen.h"

void patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif /* PMBUS_MFR_SPECIFIC_HANDLERS_H_ */
