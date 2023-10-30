
/**
 * @file    ./src/user_app/pmbus_mfr_autogen.h
 * @brief   Automatically generated from output of pmbus compile script.
 */

/* ============================================================================
** Copyright (C) 2023 Infineon. All rights reserved.
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
** Created on:  2023-10-30
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

#define PMBUS_CMDCODE_MFR_VDD_SCALE (0xB4)

/* instantiate the command MFR_VDD_SCALE.
 */
extern PMBUS_CMD_t PMBUS_CMD_MFR_VDD_SCALE;
extern uint8_t	PMBUS_CMD_MFR_VDD_SCALE_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	PMBUS_CMD_MFR_VDD_SCALE_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_VIN_SCALE (0xB5)

/* instantiate the command MFR_VIN_SCALE.
 */
extern PMBUS_CMD_t PMBUS_CMD_MFR_VIN_SCALE;
extern uint8_t	PMBUS_CMD_MFR_VIN_SCALE_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	PMBUS_CMD_MFR_VIN_SCALE_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_VOUT_OFF_THRESHOLD (0xB6)

/* instantiate the command structure for MFR_VOUT_OFF_THRESHOLD.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_MFR_VOUT_OFF_THRESHOLD;
extern uint8_t	PMBUS_CMD_MFR_VOUT_OFF_THRESHOLD_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_MFR_VOUT_OFF_THRESHOLD_DATA_LOOP1;    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_CONFIG_DELAY (0xB7)

/* instantiate the command structure for MFR_CONFIG_DELAY.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_MFR_CONFIG_DELAY;
extern uint8_t	PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP1;    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_IOUT_OC_FAULT_LIMIT (0xB8)

/* instantiate the command structure for MFR_IOUT_OC_FAULT_LIMIT.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_MFR_IOUT_OC_FAULT_LIMIT;
extern uint8_t	PMBUS_CMD_MFR_IOUT_OC_FAULT_LIMIT_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_MFR_IOUT_OC_FAULT_LIMIT_DATA_LOOP1;    // data array for command loop1 (pointer if common)
extern uint8_t	PMBUS_CMD_MFR_IOUT_OC_FAULT_LIMIT_RANGE[2];  	// data array for range (common for loops)

#define PMBUS_CMDCODE_MFR_IOUT_OC_WARN_LIMIT (0xB9)

/* instantiate the command structure for MFR_IOUT_OC_WARN_LIMIT.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_MFR_IOUT_OC_WARN_LIMIT;
extern uint8_t	PMBUS_CMD_MFR_IOUT_OC_WARN_LIMIT_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_MFR_IOUT_OC_WARN_LIMIT_DATA_LOOP1;    // data array for command loop1 (pointer if common)
extern uint8_t	PMBUS_CMD_MFR_IOUT_OC_WARN_LIMIT_RANGE[2];  	// data array for range (common for loops)

#define PMBUS_CMDCODE_MFR_VOUT_ON_THRESHOLD (0xBA)

/* instantiate the command structure for MFR_VOUT_ON_THRESHOLD.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_MFR_VOUT_ON_THRESHOLD;
extern uint8_t	PMBUS_CMD_MFR_VOUT_ON_THRESHOLD_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_MFR_VOUT_ON_THRESHOLD_DATA_LOOP1;    // data array for command loop1 (pointer if common)

#define PMBUS_CMDCODE_MFR_VOUT_TARGET (0xBB)

/* instantiate the command structure for MFR_VOUT_TARGET.
 */
// instances of the struct one per command
extern PMBUS_CMD_t PMBUS_CMD_MFR_VOUT_TARGET;
extern uint8_t	PMBUS_CMD_MFR_VOUT_TARGET_DATA_LOOP0[2];  	// data array for command loop0
extern uint8_t	* PMBUS_CMD_MFR_VOUT_TARGET_DATA_LOOP1;    // data array for command loop1 (pointer if common)


void pmbus_mfr_autogen_init(void);

// external handle functions
extern void PMBUS_HANDLE_FAN_CONFIG_1_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_FAN_COMMAND_1(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_FAN_COMMAND_2(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_MFR_VDD_SCALE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
extern void PMBUS_HANDLE_MFR_VIN_SCALE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif   //#endif /* _PMBUS_MFR_AUTOGEN_H_ */


