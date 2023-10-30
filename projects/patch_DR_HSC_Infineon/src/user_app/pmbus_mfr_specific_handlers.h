/*
 * pmbus_mfr_specific_handlers.h
 *
 *  Created on: Jan 24, 2017
 *      Author: dlewis4
 */

#ifndef PMBUS_MFR_SPECIFIC_HANDLERS_H_
#define PMBUS_MFR_SPECIFIC_HANDLERS_H_

#include <stdint.h>              /* uint*_t */
#include "pmbus_autogen.h"

void patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif /* PMBUS_MFR_SPECIFIC_HANDLERS_H_ */
