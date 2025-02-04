
/**
 * @file    ./src/user_app/pmbus_mfr_autogen.h
 * @brief   Automatically generated from output of pmbus compile script.
 */

/* ============================================================================
** Copyright (C) 2025 Infineon. All rights reserved.
**               Infineon Technologies, PMM SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorisation.
** ============================================================================
**
** Automatically generated by:  pmbus_xlsx2c.py shasta_pmbus.xlsx
** Created on:  2025-02-04
*/
/*lint -save -e960 */


#ifndef PMBUS_MFR_AUTOGEN_H
#define PMBUS_MFR_AUTOGEN_H

#include <stdint.h>              /* uint*_t */
#include "fw_bool.h"             /* boolean type */
#include "pmbus_autogen.h"




#define PMBUS_CMDCODE_FAN_CONFIG_1_2 (0x3A)

/* instantiate the command structure for FAN_CONFIG_1_2.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_FAN_CONFIG_1_2;
extern uint8_t	PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP0[1];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP1;    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_FAN_COMMAND_1 (0x3B)

/* instantiate the command structure for FAN_COMMAND_1.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_FAN_COMMAND_1;
extern uint8_t	PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP1;    // data array for command loop1 (pointer if common)
extern uint8_t	PMBUS_CMD_FAN_COMMAND_1_RANGE[2];  	// data array for range (common for loops)

#define PMBUS_CMDCODE_FAN_COMMAND_2 (0x3C)

/* instantiate the command structure for FAN_COMMAND_2.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_FAN_COMMAND_2;
extern uint8_t	PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP1;    // data array for command loop1 (pointer if common)
extern uint8_t	PMBUS_CMD_FAN_COMMAND_2_RANGE[2];  	// data array for range (common for loops)

#define PMBUS_CMDCODE_MFR_FREQUENCY_DITHER (0xC7)

/* instantiate the command MFR_FREQUENCY_DITHER.
 */
extern PMBUS_CMD_t PMBUS_CMD_MFR_FREQUENCY_DITHER;
extern uint8_t	PMBUS_CMD_MFR_FREQUENCY_DITHER_DATA_LOOP0[1];  	// data array for command loop0
extern uint8_t	PMBUS_CMD_MFR_FREQUENCY_DITHER_DATA_LOOP1[1];    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_SS_RAMP_FSW (0xD5)

/* instantiate the command MFR_SS_RAMP_FSW.
 */
extern PMBUS_CMD_t PMBUS_CMD_MFR_SS_RAMP_FSW;
extern uint8_t	PMBUS_CMD_MFR_SS_RAMP_FSW_DATA_LOOP0[1];  	// data array for command loop0
extern uint8_t	PMBUS_CMD_MFR_SS_RAMP_FSW_DATA_LOOP1[1];    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD (0xDA)

/* instantiate the command structure for MFR_ISHARE_THRESHOLD.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_MFR_ISHARE_THRESHOLD;
extern uint8_t	PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1;    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP (0xFC)

/* instantiate the command MFR_ADDED_DROOP_DURING_RAMP.
 */
extern PMBUS_CMD_t PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP;
extern uint8_t	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)
extern uint8_t	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_RANGE[4];  	// data array for range (common for loops)


void pmbus_mfr_autogen_init(void);

// external handle functions
extern void PMBUS_HANDLE_FAN_CONFIG_1_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_FAN_COMMAND_1(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_FAN_COMMAND_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_MFR_FREQUENCY_DITHER(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_MFR_ISHARE_THRESHOLD(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif   //#endif /* _PMBUS_MFR_AUTOGEN_H_ */


