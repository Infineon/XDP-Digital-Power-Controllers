
/**
 * @file    ./src/user_app/pmbus_mfr_autogen.h
 * @brief   Automatically generated from output of pmbus compile script.
 */

/* ============================================================================
** Copyright (C) 2021 Infineon. All rights reserved.
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
** Created on:  2021-03-30
*/
/*lint -save -e960 */

#include <stdint.h>              /* uint*_t */
#include "fw_bool.h"             /* boolean type */
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"




/* instantiate the command structure for FAN_CONFIG_1_2.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_FAN_CONFIG_1_2;
uint8_t	PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP0[1];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP1;    // data array for command loop1 (pointer if common)

/* instantiate the command structure for FAN_COMMAND_1.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_FAN_COMMAND_1;
uint8_t	PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP1;    // data array for command loop1 (pointer if common)
uint8_t	PMBUS_CMD_FAN_COMMAND_1_RANGE[2];  	// data array for range (common for loops)

/* instantiate the command structure for FAN_COMMAND_2.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_FAN_COMMAND_2;
uint8_t	PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP1;    // data array for command loop1 (pointer if common)
uint8_t	PMBUS_CMD_FAN_COMMAND_2_RANGE[2];  	// data array for range (common for loops)

/* instantiate the command structure for MFR_48V_ON_THRESH.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_MFR_48V_ON_THRESH;
uint8_t	PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP1;    // data array for command loop1 (pointer if common)
uint8_t	PMBUS_CMD_MFR_48V_ON_THRESH_RANGE[2];  	// data array for range (common for loops)

/* instantiate the command structure for MFR_48V_OFF_THRESH.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_MFR_48V_OFF_THRESH;
uint8_t	PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP1;    // data array for command loop1 (pointer if common)
uint8_t	PMBUS_CMD_MFR_48V_OFF_THRESH_RANGE[2];  	// data array for range (common for loops)

/* instantiate the command MFR_FREQUENCY_DITHER.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_FREQUENCY_DITHER;
uint8_t	PMBUS_CMD_MFR_FREQUENCY_DITHER_DATA_LOOP0[1];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_FREQUENCY_DITHER_DATA_LOOP1[1];    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_BOARD_TRIM.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_BOARD_TRIM;
uint8_t	PMBUS_CMD_MFR_BOARD_TRIM_DATA_LOOP0[2];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_BOARD_TRIM_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_SNAPSHOT_DATA.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_SNAPSHOT_DATA;
uint8_t	PMBUS_CMD_MFR_SNAPSHOT_DATA_DATA_LOOP0[31];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_SNAPSHOT_DATA_DATA_LOOP1[31];    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_VDD_SCALE.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_VDD_SCALE;
uint8_t	PMBUS_CMD_MFR_VDD_SCALE_DATA_LOOP0[2];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_VDD_SCALE_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_VIN_SCALE.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_VIN_SCALE;
uint8_t	PMBUS_CMD_MFR_VIN_SCALE_DATA_LOOP0[2];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_VIN_SCALE_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_SS_RAMP_FSW.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_SS_RAMP_FSW;
uint8_t	PMBUS_CMD_MFR_SS_RAMP_FSW_DATA_LOOP0[1];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_SS_RAMP_FSW_DATA_LOOP1[1];    // data array for command loop1 (pointer if common)

/* instantiate the command structure for MFR_ISHARE_THRESHOLD.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_MFR_ISHARE_THRESHOLD;
uint8_t	PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1;    // data array for command loop1 (pointer if common)

/* instantiate the command structure for MFR_CONFIG_THRESHOLD.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_MFR_CONFIG_THRESHOLD;
uint8_t	PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP1;    // data array for command loop1 (pointer if common)

/* instantiate the command structure for MFR_CONFIG_DELAY.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_MFR_CONFIG_DELAY;
uint8_t	PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP1;    // data array for command loop1 (pointer if common)

/* instantiate the command structure for MFR_MAX_DUTY.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_MFR_MAX_DUTY;
uint8_t	PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP1;    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_ADDED_DROOP_DURING_RAMP.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP;
uint8_t	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_DATA_LOOP0[2];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)
uint8_t	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_RANGE[4];  	// data array for range (common for loops)

/*
 * Initializes all of our pmbus structs that were generated by the script
 * Function gets called to setup command support array, initialize pointers to command data, and describe commands 
 */
void pmbus_mfr_autogen_init(void)
{


	/* initialize command structure for FAN_CONFIG_1_2. */
	// FAN_CONFIG_1_2 COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_FAN_CONFIG_1_2] = PMBUS_HANDLE_FAN_CONFIG_1_2;
	//PMBUS_CMD_FAN_CONFIG_1_2.OPCODE = PMBUS_CMDCODE_FAN_CONFIG_1_2;
	//PMBUS_CMD_FAN_CONFIG_1_2.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_FAN_CONFIG_1_2.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_FAN_CONFIG_1_2.NUM_BYTES = 1; // data only not including slave address and command
	//PMBUS_CMD_FAN_CONFIG_1_2.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_FAN_CONFIG_1_2.SUPPORTED = 1;
	//PMBUS_CMD_FAN_CONFIG_1_2.PROTECTED0 = 0;
	//PMBUS_CMD_FAN_CONFIG_1_2.PROTECTED1 = 0;
	//PMBUS_CMD_FAN_CONFIG_1_2.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_FAN_CONFIG_1_2.CMD_CONFIG = 0xc181233a;
	PMBUS_CMD_FAN_CONFIG_1_2.DATA0 = &PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_FAN_CONFIG_1_2.DATA1 = &PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP1 = &PMBUS_CMD_FAN_CONFIG_1_2_DATA_LOOP0[0];
	// RANGE SUPPORT for FAN_CONFIG_1_2 COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_FAN_CONFIG_1_2] = &PMBUS_CMD_FAN_CONFIG_1_2;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_FAN_CONFIG_1_2] = &PMBUS_CMD_FAN_CONFIG_1_2;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_FAN_CONFIG_1_2/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_FAN_CONFIG_1_2&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_FAN_CONFIG_1_2/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_FAN_CONFIG_1_2&31u));

	/* initialize command structure for FAN_COMMAND_1. */
	// FAN_COMMAND_1 COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_FAN_COMMAND_1] = PMBUS_HANDLE_FAN_COMMAND_1;
	//PMBUS_CMD_FAN_COMMAND_1.OPCODE = PMBUS_CMDCODE_FAN_COMMAND_1;
	//PMBUS_CMD_FAN_COMMAND_1.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_FAN_COMMAND_1.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_FAN_COMMAND_1.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_FAN_COMMAND_1.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_FAN_COMMAND_1.SUPPORTED = 1;
	//PMBUS_CMD_FAN_COMMAND_1.PROTECTED0 = 0;
	//PMBUS_CMD_FAN_COMMAND_1.PROTECTED1 = 0;
	//PMBUS_CMD_FAN_COMMAND_1.RANGE_TYPE = (uint8_t) PMBUS_RANGE_LINEAR11_UNSIGNED;
	PMBUS_CMD_FAN_COMMAND_1.RANGE = &PMBUS_CMD_FAN_COMMAND_1_RANGE[0];
	PMBUS_CMD_FAN_COMMAND_1_RANGE[0] = 6;
	PMBUS_CMD_FAN_COMMAND_1_RANGE[1] = 2;
	PMBUS_CMD_FAN_COMMAND_1.CMD_CONFIG = 0xc382233b;
	PMBUS_CMD_FAN_COMMAND_1.DATA0 = &PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_FAN_COMMAND_1.DATA1 = &PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP1 = &PMBUS_CMD_FAN_COMMAND_1_DATA_LOOP0[0];
	// RANGE SUPPORT for FAN_COMMAND_1 COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_FAN_COMMAND_1] = &PMBUS_CMD_FAN_COMMAND_1;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_FAN_COMMAND_1] = &PMBUS_CMD_FAN_COMMAND_1;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_FAN_COMMAND_1/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_FAN_COMMAND_1&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_FAN_COMMAND_1/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_FAN_COMMAND_1&31u));

	/* initialize command structure for FAN_COMMAND_2. */
	// FAN_COMMAND_2 COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_FAN_COMMAND_2] = PMBUS_HANDLE_FAN_COMMAND_2;
	//PMBUS_CMD_FAN_COMMAND_2.OPCODE = PMBUS_CMDCODE_FAN_COMMAND_2;
	//PMBUS_CMD_FAN_COMMAND_2.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_FAN_COMMAND_2.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_FAN_COMMAND_2.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_FAN_COMMAND_2.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_FAN_COMMAND_2.SUPPORTED = 1;
	//PMBUS_CMD_FAN_COMMAND_2.PROTECTED0 = 0;
	//PMBUS_CMD_FAN_COMMAND_2.PROTECTED1 = 0;
	//PMBUS_CMD_FAN_COMMAND_2.RANGE_TYPE = (uint8_t) PMBUS_RANGE_LINEAR11_UNSIGNED;
	PMBUS_CMD_FAN_COMMAND_2.RANGE = &PMBUS_CMD_FAN_COMMAND_2_RANGE[0];
	PMBUS_CMD_FAN_COMMAND_2_RANGE[0] = 6;
	PMBUS_CMD_FAN_COMMAND_2_RANGE[1] = 2;
	PMBUS_CMD_FAN_COMMAND_2.CMD_CONFIG = 0xc382233c;
	PMBUS_CMD_FAN_COMMAND_2.DATA0 = &PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_FAN_COMMAND_2.DATA1 = &PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP1 = &PMBUS_CMD_FAN_COMMAND_2_DATA_LOOP0[0];
	// RANGE SUPPORT for FAN_COMMAND_2 COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_FAN_COMMAND_2] = &PMBUS_CMD_FAN_COMMAND_2;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_FAN_COMMAND_2] = &PMBUS_CMD_FAN_COMMAND_2;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_FAN_COMMAND_2/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_FAN_COMMAND_2&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_FAN_COMMAND_2/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_FAN_COMMAND_2&31u));

	/* initialize command structure for MFR_48V_ON_THRESH. */
	// MFR_48V_ON_THRESH COMMAND:
	//PMBUS_CMD_MFR_48V_ON_THRESH.OPCODE = PMBUS_CMDCODE_MFR_48V_ON_THRESH;
	//PMBUS_CMD_MFR_48V_ON_THRESH.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_48V_ON_THRESH.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_48V_ON_THRESH.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_48V_ON_THRESH.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_48V_ON_THRESH.SUPPORTED = 1;
	//PMBUS_CMD_MFR_48V_ON_THRESH.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_48V_ON_THRESH.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_48V_ON_THRESH.RANGE_TYPE = (uint8_t) PMBUS_RANGE_LINEAR11_UNSIGNED;
	PMBUS_CMD_MFR_48V_ON_THRESH.RANGE = &PMBUS_CMD_MFR_48V_ON_THRESH_RANGE[0];
	PMBUS_CMD_MFR_48V_ON_THRESH_RANGE[0] = 9;
	PMBUS_CMD_MFR_48V_ON_THRESH_RANGE[1] = 2;
	PMBUS_CMD_MFR_48V_ON_THRESH.CMD_CONFIG = 0xc38223b1;
	PMBUS_CMD_MFR_48V_ON_THRESH.DATA0 = &PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_48V_ON_THRESH.DATA1 = &PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP1 = &PMBUS_CMD_MFR_48V_ON_THRESH_DATA_LOOP0[0];
	// RANGE SUPPORT for MFR_48V_ON_THRESH COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_48V_ON_THRESH] = &PMBUS_CMD_MFR_48V_ON_THRESH;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_48V_ON_THRESH] = &PMBUS_CMD_MFR_48V_ON_THRESH;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_48V_ON_THRESH/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_48V_ON_THRESH&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_48V_ON_THRESH/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_48V_ON_THRESH&31u));

	/* initialize command structure for MFR_48V_OFF_THRESH. */
	// MFR_48V_OFF_THRESH COMMAND:
	//PMBUS_CMD_MFR_48V_OFF_THRESH.OPCODE = PMBUS_CMDCODE_MFR_48V_OFF_THRESH;
	//PMBUS_CMD_MFR_48V_OFF_THRESH.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_48V_OFF_THRESH.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_48V_OFF_THRESH.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_48V_OFF_THRESH.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_48V_OFF_THRESH.SUPPORTED = 1;
	//PMBUS_CMD_MFR_48V_OFF_THRESH.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_48V_OFF_THRESH.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_48V_OFF_THRESH.RANGE_TYPE = (uint8_t) PMBUS_RANGE_LINEAR11_UNSIGNED;
	PMBUS_CMD_MFR_48V_OFF_THRESH.RANGE = &PMBUS_CMD_MFR_48V_OFF_THRESH_RANGE[0];
	PMBUS_CMD_MFR_48V_OFF_THRESH_RANGE[0] = 9;
	PMBUS_CMD_MFR_48V_OFF_THRESH_RANGE[1] = 2;
	PMBUS_CMD_MFR_48V_OFF_THRESH.CMD_CONFIG = 0xc38223b2;
	PMBUS_CMD_MFR_48V_OFF_THRESH.DATA0 = &PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_48V_OFF_THRESH.DATA1 = &PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP1 = &PMBUS_CMD_MFR_48V_OFF_THRESH_DATA_LOOP0[0];
	// RANGE SUPPORT for MFR_48V_OFF_THRESH COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_48V_OFF_THRESH] = &PMBUS_CMD_MFR_48V_OFF_THRESH;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_48V_OFF_THRESH] = &PMBUS_CMD_MFR_48V_OFF_THRESH;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_48V_OFF_THRESH/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_48V_OFF_THRESH&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_48V_OFF_THRESH/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_48V_OFF_THRESH&31u));

	/* initialize command structure for MFR_FREQUENCY_DITHER. */
	// MFR_FREQUENCY_DITHER COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_FREQUENCY_DITHER] = PMBUS_HANDLE_MFR_FREQUENCY_DITHER;
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.OPCODE = PMBUS_CMDCODE_MFR_FREQUENCY_DITHER;
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.NUM_BYTES = 1; // data only not including slave address and command
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.SUPPORTED = 1;
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_FREQUENCY_DITHER.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_FREQUENCY_DITHER.CMD_CONFIG = 0xc18123c7;
	PMBUS_CMD_MFR_FREQUENCY_DITHER.DATA0 = &PMBUS_CMD_MFR_FREQUENCY_DITHER_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_FREQUENCY_DITHER.DATA1 = &PMBUS_CMD_MFR_FREQUENCY_DITHER_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_FREQUENCY_DITHER COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_FREQUENCY_DITHER] = &PMBUS_CMD_MFR_FREQUENCY_DITHER;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_FREQUENCY_DITHER] = &PMBUS_CMD_MFR_FREQUENCY_DITHER;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_FREQUENCY_DITHER/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_FREQUENCY_DITHER&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_FREQUENCY_DITHER/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_FREQUENCY_DITHER&31u));

	/* initialize command structure for MFR_BOARD_TRIM. */
	// MFR_BOARD_TRIM COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_BOARD_TRIM] = PMBUS_HANDLE_MFR_BOARD_TRIM;
	//PMBUS_CMD_MFR_BOARD_TRIM.OPCODE = PMBUS_CMDCODE_MFR_BOARD_TRIM;
	//PMBUS_CMD_MFR_BOARD_TRIM.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_BOARD_TRIM.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_BOARD_TRIM.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_BOARD_TRIM.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_BOARD_TRIM.SUPPORTED = 1;
	//PMBUS_CMD_MFR_BOARD_TRIM.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_BOARD_TRIM.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_BOARD_TRIM.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_BOARD_TRIM.CMD_CONFIG = 0xc18223cc;
	PMBUS_CMD_MFR_BOARD_TRIM.DATA0 = &PMBUS_CMD_MFR_BOARD_TRIM_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_BOARD_TRIM.DATA1 = &PMBUS_CMD_MFR_BOARD_TRIM_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_BOARD_TRIM COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_BOARD_TRIM] = &PMBUS_CMD_MFR_BOARD_TRIM;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_BOARD_TRIM] = &PMBUS_CMD_MFR_BOARD_TRIM;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_BOARD_TRIM/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_BOARD_TRIM&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_BOARD_TRIM/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_BOARD_TRIM&31u));

	/* initialize command structure for MFR_SNAPSHOT_DATA. */
	// MFR_SNAPSHOT_DATA COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_SNAPSHOT_DATA] = PMBUS_HANDLE_MFR_SNAPSHOT_DATA;
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.OPCODE = PMBUS_CMDCODE_MFR_SNAPSHOT_DATA;
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_BLOCK_WRITE;
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_BLOCK_READ;
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.NUM_BYTES = 31; // data only not including slave address and command
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.NUM_PROTOCOL_BYTES = 3;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.SUPPORTED = 1;
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_SNAPSHOT_DATA.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_SNAPSHOT_DATA.CMD_CONFIG = 0x41df56d0;
	PMBUS_CMD_MFR_SNAPSHOT_DATA.DATA0 = &PMBUS_CMD_MFR_SNAPSHOT_DATA_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_SNAPSHOT_DATA.DATA1 = &PMBUS_CMD_MFR_SNAPSHOT_DATA_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_SNAPSHOT_DATA COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_SNAPSHOT_DATA] = &PMBUS_CMD_MFR_SNAPSHOT_DATA;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_SNAPSHOT_DATA] = &PMBUS_CMD_MFR_SNAPSHOT_DATA;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_SNAPSHOT_DATA/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_SNAPSHOT_DATA&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_SNAPSHOT_DATA/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_SNAPSHOT_DATA&31u));

	/* initialize command structure for MFR_VDD_SCALE. */
	// MFR_VDD_SCALE COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_VDD_SCALE] = PMBUS_HANDLE_MFR_VDD_SCALE;
	//PMBUS_CMD_MFR_VDD_SCALE.OPCODE = PMBUS_CMDCODE_MFR_VDD_SCALE;
	//PMBUS_CMD_MFR_VDD_SCALE.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_VDD_SCALE.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_VDD_SCALE.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_VDD_SCALE.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_VDD_SCALE.SUPPORTED = 1;
	//PMBUS_CMD_MFR_VDD_SCALE.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_VDD_SCALE.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_VDD_SCALE.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_VDD_SCALE.CMD_CONFIG = 0xc18223d2;
	PMBUS_CMD_MFR_VDD_SCALE.DATA0 = &PMBUS_CMD_MFR_VDD_SCALE_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_VDD_SCALE.DATA1 = &PMBUS_CMD_MFR_VDD_SCALE_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_VDD_SCALE COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VDD_SCALE] = &PMBUS_CMD_MFR_VDD_SCALE;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VDD_SCALE] = &PMBUS_CMD_MFR_VDD_SCALE;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VDD_SCALE/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VDD_SCALE&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VDD_SCALE/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VDD_SCALE&31u));

	/* initialize command structure for MFR_VIN_SCALE. */
	// MFR_VIN_SCALE COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_VIN_SCALE] = PMBUS_HANDLE_MFR_VIN_SCALE;
	//PMBUS_CMD_MFR_VIN_SCALE.OPCODE = PMBUS_CMDCODE_MFR_VIN_SCALE;
	//PMBUS_CMD_MFR_VIN_SCALE.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_VIN_SCALE.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_VIN_SCALE.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_VIN_SCALE.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_VIN_SCALE.SUPPORTED = 1;
	//PMBUS_CMD_MFR_VIN_SCALE.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_VIN_SCALE.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_VIN_SCALE.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_VIN_SCALE.CMD_CONFIG = 0xc18223d3;
	PMBUS_CMD_MFR_VIN_SCALE.DATA0 = &PMBUS_CMD_MFR_VIN_SCALE_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_VIN_SCALE.DATA1 = &PMBUS_CMD_MFR_VIN_SCALE_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_VIN_SCALE COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VIN_SCALE] = &PMBUS_CMD_MFR_VIN_SCALE;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VIN_SCALE] = &PMBUS_CMD_MFR_VIN_SCALE;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VIN_SCALE/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VIN_SCALE&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VIN_SCALE/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VIN_SCALE&31u));

	/* initialize command structure for MFR_SS_RAMP_FSW. */
	// MFR_SS_RAMP_FSW COMMAND:
	//PMBUS_CMD_MFR_SS_RAMP_FSW.OPCODE = PMBUS_CMDCODE_MFR_SS_RAMP_FSW;
	//PMBUS_CMD_MFR_SS_RAMP_FSW.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_SS_RAMP_FSW.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_SS_RAMP_FSW.NUM_BYTES = 1; // data only not including slave address and command
	//PMBUS_CMD_MFR_SS_RAMP_FSW.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_SS_RAMP_FSW.SUPPORTED = 1;
	//PMBUS_CMD_MFR_SS_RAMP_FSW.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_SS_RAMP_FSW.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_SS_RAMP_FSW.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_SS_RAMP_FSW.CMD_CONFIG = 0xc18123d5;
	PMBUS_CMD_MFR_SS_RAMP_FSW.DATA0 = &PMBUS_CMD_MFR_SS_RAMP_FSW_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_SS_RAMP_FSW.DATA1 = &PMBUS_CMD_MFR_SS_RAMP_FSW_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_SS_RAMP_FSW COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_SS_RAMP_FSW] = &PMBUS_CMD_MFR_SS_RAMP_FSW;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_SS_RAMP_FSW] = &PMBUS_CMD_MFR_SS_RAMP_FSW;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_SS_RAMP_FSW/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_SS_RAMP_FSW&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_SS_RAMP_FSW/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_SS_RAMP_FSW&31u));

	/* initialize command structure for MFR_ISHARE_THRESHOLD. */
	// MFR_ISHARE_THRESHOLD COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD] = PMBUS_HANDLE_MFR_ISHARE_THRESHOLD;
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.OPCODE = PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD;
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.SUPPORTED = 1;
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_ISHARE_THRESHOLD.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_ISHARE_THRESHOLD.CMD_CONFIG = 0xc18223da;
	PMBUS_CMD_MFR_ISHARE_THRESHOLD.DATA0 = &PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_ISHARE_THRESHOLD.DATA1 = &PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1 = &PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP0[0];
	// RANGE SUPPORT for MFR_ISHARE_THRESHOLD COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD] = &PMBUS_CMD_MFR_ISHARE_THRESHOLD;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD] = &PMBUS_CMD_MFR_ISHARE_THRESHOLD;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD&31u));

	/* initialize command structure for MFR_CONFIG_THRESHOLD. */
	// MFR_CONFIG_THRESHOLD COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD] = PMBUS_HANDLE_MFR_CONFIG_THRESHOLD;
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.OPCODE = PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD;
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.SUPPORTED = 1;
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_CONFIG_THRESHOLD.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_CONFIG_THRESHOLD.CMD_CONFIG = 0xc18223e5;
	PMBUS_CMD_MFR_CONFIG_THRESHOLD.DATA0 = &PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_CONFIG_THRESHOLD.DATA1 = &PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP1 = &PMBUS_CMD_MFR_CONFIG_THRESHOLD_DATA_LOOP0[0];
	// RANGE SUPPORT for MFR_CONFIG_THRESHOLD COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD] = &PMBUS_CMD_MFR_CONFIG_THRESHOLD;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD] = &PMBUS_CMD_MFR_CONFIG_THRESHOLD;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_CONFIG_THRESHOLD&31u));

	/* initialize command structure for MFR_CONFIG_DELAY. */
	// MFR_CONFIG_DELAY COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_CONFIG_DELAY] = PMBUS_HANDLE_MFR_CONFIG_DELAY;
	//PMBUS_CMD_MFR_CONFIG_DELAY.OPCODE = PMBUS_CMDCODE_MFR_CONFIG_DELAY;
	//PMBUS_CMD_MFR_CONFIG_DELAY.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_CONFIG_DELAY.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_CONFIG_DELAY.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_CONFIG_DELAY.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_CONFIG_DELAY.SUPPORTED = 1;
	//PMBUS_CMD_MFR_CONFIG_DELAY.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_CONFIG_DELAY.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_CONFIG_DELAY.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_CONFIG_DELAY.CMD_CONFIG = 0xc18223e8;
	PMBUS_CMD_MFR_CONFIG_DELAY.DATA0 = &PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_CONFIG_DELAY.DATA1 = &PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP1 = &PMBUS_CMD_MFR_CONFIG_DELAY_DATA_LOOP0[0];
	// RANGE SUPPORT for MFR_CONFIG_DELAY COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_CONFIG_DELAY] = &PMBUS_CMD_MFR_CONFIG_DELAY;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_CONFIG_DELAY] = &PMBUS_CMD_MFR_CONFIG_DELAY;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_CONFIG_DELAY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_CONFIG_DELAY&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_CONFIG_DELAY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_CONFIG_DELAY&31u));

	/* initialize command structure for MFR_MAX_DUTY. */
	// MFR_MAX_DUTY COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_MAX_DUTY] = PMBUS_HANDLE_MFR_MAX_DUTY;
	//PMBUS_CMD_MFR_MAX_DUTY.OPCODE = PMBUS_CMDCODE_MFR_MAX_DUTY;
	//PMBUS_CMD_MFR_MAX_DUTY.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_MAX_DUTY.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_MAX_DUTY.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_MAX_DUTY.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_MAX_DUTY.SUPPORTED = 1;
	//PMBUS_CMD_MFR_MAX_DUTY.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_MAX_DUTY.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_MAX_DUTY.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_MAX_DUTY.CMD_CONFIG = 0xc18223ed;
	PMBUS_CMD_MFR_MAX_DUTY.DATA0 = &PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_MAX_DUTY.DATA1 = &PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP1 = &PMBUS_CMD_MFR_MAX_DUTY_DATA_LOOP0[0];
	// RANGE SUPPORT for MFR_MAX_DUTY COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_MAX_DUTY] = &PMBUS_CMD_MFR_MAX_DUTY;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_MAX_DUTY] = &PMBUS_CMD_MFR_MAX_DUTY;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_MAX_DUTY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_MAX_DUTY&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_MAX_DUTY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_MAX_DUTY&31u));

	/* initialize command structure for MFR_ADDED_DROOP_DURING_RAMP. */
	// MFR_ADDED_DROOP_DURING_RAMP COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP] = PMBUS_HANDLE_MFR_ADDED_DROOP_DURING_RAMP;
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.OPCODE = PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP;
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.SUPPORTED = 1;
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.RANGE_TYPE = (uint8_t) PMBUS_RANGE_LINEAR11_UNSIGNED_ARRAY;
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.RANGE = &PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_RANGE[0];
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_RANGE[0] = 3;
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_RANGE[1] = 7;
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_RANGE[2] = 4;
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_RANGE[3] = 6;
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.CMD_CONFIG = 0xc58223fc;
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.DATA0 = &PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP.DATA1 = &PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_ADDED_DROOP_DURING_RAMP COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP] = &PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP] = &PMBUS_CMD_MFR_ADDED_DROOP_DURING_RAMP;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP&31u));

}


