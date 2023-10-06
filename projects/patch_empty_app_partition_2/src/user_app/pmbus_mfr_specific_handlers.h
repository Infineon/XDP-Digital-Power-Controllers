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
#define READ_OTP_CRC
void pmbus_mfr_init (void);

#ifdef READ_OTP_CRC
uint32_t configurator_get_section_crc32_xv(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Types_t cmd_type, uint8_t xv);
#endif

#endif /* PMBUS_MFR_SPECIFIC_HANDLERS_H_ */
