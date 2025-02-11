
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

/* instantiate the command MFR_VIN_TRIMMING_ACTIVE.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE;
uint8_t	PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE_DATA_LOOP0[4];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE_DATA_LOOP1[4];    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_VIN_TRIMMING_STANDBY.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY;
uint8_t	PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY_DATA_LOOP0[4];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY_DATA_LOOP1[4];    // data array for command loop1 (pointer if common)

/* instantiate the command structure for MFR_ISHARE_THRESHOLD.
 */
// instances of the struct one per command
PMBUS_CMD_t PMBUS_CMD_MFR_ISHARE_THRESHOLD;
uint8_t	PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP0[2];  	// data array for command loop0
/*lint -esym(552,PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1) */
/*lint -esym(844,PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1) */
uint8_t	* PMBUS_CMD_MFR_ISHARE_THRESHOLD_DATA_LOOP1;    // data array for command loop1 (pointer if common)

/* instantiate the command MFR_ADJ_TURN_RATIO.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_ADJ_TURN_RATIO;
uint8_t	PMBUS_CMD_MFR_ADJ_TURN_RATIO_DATA_LOOP0[2];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_ADJ_TURN_RATIO_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)
uint8_t	PMBUS_CMD_MFR_ADJ_TURN_RATIO_RANGE[4];  	// data array for range (common for loops)

/* instantiate the command MFR_ESTIMATE_EFFICIENCY.
 */
PMBUS_CMD_t PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY;
uint8_t	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_DATA_LOOP0[2];  	// data array for command loop0
uint8_t	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_DATA_LOOP1[2];    // data array for command loop1 (pointer if common)
uint8_t	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_RANGE[2];  	// data array for range (common for loops)

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

	/* initialize command structure for MFR_VIN_TRIMMING_ACTIVE. */
	// MFR_VIN_TRIMMING_ACTIVE COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE] = PMBUS_HANDLE_MFR_VIN_TRIMMING_ACTIVE;
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.OPCODE = PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE;
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_BLOCK_WRITE;
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_BLOCK_READ;
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.NUM_BYTES = 4; // data only not including slave address and command
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.NUM_PROTOCOL_BYTES = 3;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.SUPPORTED = 1;
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.CMD_CONFIG = 0xc1c456b1;
	PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.DATA0 = &PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE.DATA1 = &PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_VIN_TRIMMING_ACTIVE COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE] = &PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE] = &PMBUS_CMD_MFR_VIN_TRIMMING_ACTIVE;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE&31u));

	/* initialize command structure for MFR_VIN_TRIMMING_STANDBY. */
	// MFR_VIN_TRIMMING_STANDBY COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY] = PMBUS_HANDLE_MFR_VIN_TRIMMING_STANDBY;
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.OPCODE = PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY;
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_BLOCK_WRITE;
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_BLOCK_READ;
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.NUM_BYTES = 4; // data only not including slave address and command
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.NUM_PROTOCOL_BYTES = 3;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.SUPPORTED = 1;
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.RANGE_TYPE = (uint8_t) PMBUS_RANGE_ALL;
	PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.CMD_CONFIG = 0xc1c456b2;
	PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.DATA0 = &PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY.DATA1 = &PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_VIN_TRIMMING_STANDBY COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY] = &PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY] = &PMBUS_CMD_MFR_VIN_TRIMMING_STANDBY;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY&31u));

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

	/* initialize command structure for MFR_ADJ_TURN_RATIO. */
	// MFR_ADJ_TURN_RATIO COMMAND:
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO] = PMBUS_HANDLE_MFR_ADJ_TURN_RATIO;
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.OPCODE = PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO;
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.SUPPORTED = 1;
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_ADJ_TURN_RATIO.RANGE_TYPE = (uint8_t) PMBUS_RANGE_LINEAR11_UNSIGNED_ARRAY;
	PMBUS_CMD_MFR_ADJ_TURN_RATIO.RANGE = &PMBUS_CMD_MFR_ADJ_TURN_RATIO_RANGE[0];
	PMBUS_CMD_MFR_ADJ_TURN_RATIO_RANGE[0] = 1;
	PMBUS_CMD_MFR_ADJ_TURN_RATIO_RANGE[1] = 9;
	PMBUS_CMD_MFR_ADJ_TURN_RATIO_RANGE[2] = 2;
	PMBUS_CMD_MFR_ADJ_TURN_RATIO_RANGE[3] = 8;
	PMBUS_CMD_MFR_ADJ_TURN_RATIO.CMD_CONFIG = 0xc58223e5;
	PMBUS_CMD_MFR_ADJ_TURN_RATIO.DATA0 = &PMBUS_CMD_MFR_ADJ_TURN_RATIO_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_ADJ_TURN_RATIO.DATA1 = &PMBUS_CMD_MFR_ADJ_TURN_RATIO_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_ADJ_TURN_RATIO COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO] = &PMBUS_CMD_MFR_ADJ_TURN_RATIO;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO] = &PMBUS_CMD_MFR_ADJ_TURN_RATIO;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ADJ_TURN_RATIO&31u));

	/* initialize command structure for MFR_ESTIMATE_EFFICIENCY. */
	// MFR_ESTIMATE_EFFICIENCY COMMAND:
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.OPCODE = PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY;
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.WRITE_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_WRITE;
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.READ_PROTOCOL = (uint8_t) TRANSACTION_PROTOCOL_READ;
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.NUM_BYTES = 2; // data only not including slave address and command
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.NUM_PROTOCOL_BYTES = 2;  // slave address command and any other non-data bytes
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.SUPPORTED = 1;
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.PROTECTED0 = 0;
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.PROTECTED1 = 0;
	//PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.RANGE_TYPE = (uint8_t) PMBUS_RANGE_LINEAR11_UNSIGNED;
	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.RANGE = &PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_RANGE[0];
	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_RANGE[0] = 0;
	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_RANGE[1] = 8;
	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.CMD_CONFIG = 0xc38223ed;
	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.DATA0 = &PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_DATA_LOOP0[0];  // set the pointer to the data array
	PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY.DATA1 = &PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY_DATA_LOOP1[0];  // set the pointer to the data array
	// RANGE SUPPORT for MFR_ESTIMATE_EFFICIENCY COMMAND:
	// now populate pointers to the supported commands for each page
	PMBUS_CMD_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY] = &PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY;
	//PMBUS_CMD_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY] = &PMBUS_CMD_MFR_ESTIMATE_EFFICIENCY;
	//lint -e{778} suppress "Constant expression evaluates to 0 in operation '&'" 
	PMBUS_CMD_SUPPORT_ARRAY_LOOP[PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY&31u));
	//PMBUS_CMD_SUPPORT_ARRAY_LOOP1[PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY/32] |= (uint32_t) ((uint32_t)1u << ((uint32_t)PMBUS_CMDCODE_MFR_ESTIMATE_EFFICIENCY&31u));

	/* initialize command structure for MFR_ADDED_DROOP_DURING_RAMP. */
	// MFR_ADDED_DROOP_DURING_RAMP COMMAND:
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


