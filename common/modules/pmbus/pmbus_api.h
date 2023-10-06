/* ============================================================================
** Copyright (C) 2014 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/**
 * @file    pmbus_API.h
 * @brief   Top level API for pmbus driver
 */

#ifndef PMBUS_API_H
#define PMBUS_API_H

/** Compiler configurations first */
#include "fw_config.h"           /** Essential fw-wide configuration for e.g. buffer size #defines */
#include "environment.h"         /** STATIC, INLINE */
#include "shasta.h"
#include "system_shasta.h"
#include <stdint.h>              /** uint*_t */
#include "fw_bool.h"             /** boolean type */
#include "log.h"
#include "pmbus_autogen.h"
#include "pmbus_api.h"
#include "pmbus_drv.h"
#include "pmbus_gpio_control.h"
#include "shasta_hal_pmbus.h"
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "configurator.h"

// These are function specific feature defines, they should only be used in OPERATION function
#define PMBUS_Q_EXP (-16)
#define PMBUS_SMBALERT_MASK_SIZE (9)


/**
 * this is a memory value of the on_off_state from pmbus used to determine to send queue pushes
 */
typedef struct {
	boolean On_Off_State[NUM_LOOPS];     // this is a memory value of the on_off_state from pmbus used to determine to send queue pushes
	boolean Vin_On_Off_State[NUM_LOOPS]; // this is a memory value of the on_off_state from pmbus used to determine to send
	boolean Retry_Operation_State[NUM_LOOPS]; // this is a memory value of the fault in the middle of retry operation
	boolean Constant_Current_State[NUM_LOOPS]; // this is a memory value of the fault in constant current state
	boolean Under_Voltage_State[NUM_LOOPS]; // this is an indicator of below output voltage (for clearing of retry count)
	boolean Negative_Current_State[NUM_LOOPS]; // this is an indicator of below output voltage (for clearing of retry count)
	boolean Shutdown_State[NUM_LOOPS]; // this is a memory of the fault shutdown
} _ON_OFF_STATE_s ;

extern _ON_OFF_STATE_s ON_OFF_STATE; //Loop0 HW, Loop1 HW

typedef enum  _FW_COMMAND_t {
	FW_COMMAND_NONE	= 0u,			// NOP
	FW_COMMAND_GET_DRIVER_VERSION,  // fw_data = Modules_t, returns ARM_DRIVER_VERSION type

} FW_COMMAND_t;

typedef enum  _PMBUS_STATUS_e {
	PMBUS_STATUS_NONE_OF_THE_ABOVE	= 0u,
	PMBUS_STATUS_CML				= 1u,
	PMBUS_STATUS_TEMPERATURE		= 2u,
	PMBUS_STATUS_VIN_UV				= 3u,
	PMBUS_STATUS_IOUT_OC			= 4u,
	PMBUS_STATUS_VOUT_OV 			= 5u,
	PMBUS_STATUS_OFF 				= 6u,
	PMBUS_STATUS_BUSY 				= 7u,
	PMBUS_STATUS_UNKNOWN 			= 8u,
	PMBUS_STATUS_OTHER 				= 9u,
	PMBUS_STATUS_FANS 				= 10u,
	PMBUS_STATUS_POWER_GOOD 		= 11u,
	PMBUS_STATUS_MFR 				= 12u,
	PMBUS_STATUS_INPUT 				= 13u,
	PMBUS_STATUS_IOUT_POUT 			= 14u,
	PMBUS_STATUS_VOUT 				= 15u
} PMBUS_STATUS_e;

typedef enum  _PMBUS_VOUT_STATUS_e {
	PMBUS_STATUS_VOUT_TRACKING_ERROR	= 0u,
	PMBUS_STATUS_VOUT_TOFF_MAX_WARNING	= 1u,
	PMBUS_STATUS_VOUT_TON_MAX_WARNING	= 2u,
	PMBUS_STATUS_VOUT_MAX_MIN_WARNING	= 3u,
	PMBUS_STATUS_VOUT_UV_FAULT			= 4u,
	PMBUS_STATUS_VOUT_UV_WARNING 		= 5u,
	PMBUS_STATUS_VOUT_OV_WARNING 		= 6u,
	PMBUS_STATUS_VOUT_OV_FAULT 			= 7u
} PMBUS_VOUT_STATUS_e;

typedef enum  _PMBUS_IOUT_STATUS_e {
	PMBUS_STATUS_IOUT_POUT_OP_WARNING			= 0u,
	PMBUS_STATUS_IOUT_POUT_OP_FAULT				= 1u,
	PMBUS_STATUS_IOUT_IN_POWER_LIMITING_MODE	= 2u,
	PMBUS_STATUS_IOUT_CURRENT_SHARE_FAULT		= 3u,
	PMBUS_STATUS_IOUT_UC_FAULT					= 4u,
	PMBUS_STATUS_IOUT_OC_WARNING 				= 5u,
	PMBUS_STATUS_IOUT_OC_FAULT_W_LV_SD 			= 6u,
	PMBUS_STATUS_IOUT_OC_FAULT 					= 7u
} PMBUS_IOUT_STATUS_e;

typedef enum  _PMBUS_TEMPERATURE_STATUS_e {
	PMBUS_STATUS_TEMPERATURE_UT_FAULT	= 4u,
	PMBUS_STATUS_TEMPERATURE_UT_WARNING = 5u,
	PMBUS_STATUS_TEMPERATURE_OT_WARNING = 6u,
	PMBUS_STATUS_TEMPERATURE_OT_FAULT 	= 7u
} PMBUS_TEMPERATURE_STATUS_e;

typedef enum  _PMBUS_CML_STATUS_e {
	PMBUS_STATUS_CML_OTHER_MEM_FAULT	= 0u,
	PMBUS_STATUS_CML_OTHER_COMM_FAULT	= 1u,
	PMBUS_STATUS_CML_PROCESSOR_FAULT	= 3u,
	PMBUS_STATUS_CML_MEMORY_FAULT		= 4u,
	PMBUS_STATUS_CML_PACKET_ERROR 		= 5u,
	PMBUS_STATUS_CML_INVALID_DATA 		= 6u,
	PMBUS_STATUS_CML_INVALID_COMMAND	= 7u
} PMBUS_CML_STATUS_e;

typedef enum  _PMBUS_INPUT_STATUS_e {
	PMBUS_STATUS_INPUT_PIN_OP_WARNING				= 0u,
	PMBUS_STATUS_INPUT_IIN_OC_WARNING				= 1u,
	PMBUS_STATUS_INPUT_IIN_OC_FAULT					= 2u,
	PMBUS_STATUS_INPUT_UNIT_OFF_LOW_INPUT_VOLTS		= 3u,
	PMBUS_STATUS_INPUT_VIN_UV_FAULT					= 4u,
	PMBUS_STATUS_INPUT_VIN_UV_WARNING 				= 5u,
	PMBUS_STATUS_INPUT_VIN_OV_WARNING 				= 6u,
	PMBUS_STATUS_INPUT_VIN_OV_FAULT 				= 7u
} PMBUS_INPUT_STATUS_e;

typedef enum  _PMBUS_MFR_SPECIFIC_STATUS_e {
	PMBUS_STATUS_MFR_SPECIFIC_0		= 0u,
	PMBUS_STATUS_MFR_SPECIFIC_1		= 1u,
	PMBUS_STATUS_MFR_SPECIFIC_2		= 2u,
	PMBUS_STATUS_MFR_SPECIFIC_3		= 3u,
	PMBUS_STATUS_MFR_SPECIFIC_4		= 4u,
	PMBUS_STATUS_MFR_SPECIFIC_5 	= 5u,
	PMBUS_STATUS_MFR_SPECIFIC_6 	= 6u,
	PMBUS_STATUS_MFR_SPECIFIC_7 	= 7u
} PMBUS_MFR_SPECIFIC_STATUS_e;

typedef enum  _PMBUS_FANS_1_2_STATUS_e {
	PMBUS_STATUS_FANS_1_2_AIR_FLOW_WARNING		= 0u,
	PMBUS_STATUS_FANS_1_2_AIR_FLOW_FAULT		= 1u,
	PMBUS_STATUS_FANS_1_2_FAN2_SPEED_OVERRIDE	= 2u,
	PMBUS_STATUS_FANS_1_2_FAN1_SPEED_OVERRIDE	= 3u,
	PMBUS_STATUS_FANS_1_2_FAN2_WARNING			= 4u,
	PMBUS_STATUS_FANS_1_2_FAN1_WARNING 			= 5u,
	PMBUS_STATUS_FANS_1_2_FAN2_FAULT 			= 6u,
	PMBUS_STATUS_FANS_1_2_FAN1_FAULT			= 7u
} PMBUS_FANS_1_2_STATUS_e;

typedef enum  _PMBUS_FANS_3_4_STATUS_e {
	PMBUS_STATUS_FANS_3_4_FAN4_SPEED_OVERRIDE	= 2u,
	PMBUS_STATUS_FANS_3_4_FAN3_SPEED_OVERRIDE	= 3u,
	PMBUS_STATUS_FANS_3_4_FAN4_WARNING			= 4u,
	PMBUS_STATUS_FANS_3_4_FAN3_WARNING 			= 5u,
	PMBUS_STATUS_FANS_3_4_FAN4_FAULT 			= 6u,
	PMBUS_STATUS_FANS_3_4_FAN3_FAULT 			= 7u
} PMBUS_FANS_3_4_STATUS_e;

typedef enum  _PMBUS_OTHER_STATUS_e {
	PMBUS_STATUS_OTHER_FIRST_TO_ASSERT_SMBALERT	= 0u,
	PMBUS_STATUS_OTHER_OUTPUT_ORING_FAULT		= 1u,
	PMBUS_STATUS_OTHER_INPUTB_ORING_FAULT		= 2u,
	PMBUS_STATUS_OTHER_INPUTA_ORING_FAULT		= 3u,
	PMBUS_STATUS_OTHER_INPUTB_FUSE_FAULT		= 4u,
	PMBUS_STATUS_OTHER_INPUTA_FUSE_FAULT		= 5u
} PMBUS_OTHER_STATUS_e;

typedef enum  _PMBUS_MFR_FIRMWARE_COMMAND_SET_e {
	PMBUS_MFR_FIRMWARE_COMMAND_NOP	= 0u,
	PMBUS_MFR_FIRMWARE_COMMAND_GET_REV_ID,
	PMBUS_MFR_FIRMWARE_COMMAND_TESTMODE_ENTER,
	PMBUS_MFR_FIRMWARE_COMMAND_STORE_TRIM,
	PMBUS_MFR_FIRMWARE_COMMAND_STORE_CONFIG,
	PMBUS_MFR_FIRMWARE_COMMAND_CONFIGURATOR_STATUS_GET,
	PMBUS_MFR_FIRMWARE_COMMAND_STORE_FW_PATCH,
	PMBUS_MFR_FIRMWARE_COMMAND_RESTORE_TRIM,
	PMBUS_MFR_FIRMWARE_COMMAND_RESTORE_CONFIG,
	PMBUS_MFR_FIRMWARE_COMMAND_RESTORE_USER,
	PMBUS_MFR_FIRMWARE_COMMAND_RESTORE_FW_PATCH,
	PMBUS_MFR_FIRMWARE_COMMAND_UNINITIALIZE_DRIVER,
	PMBUS_MFR_FIRMWARE_COMMAND_INITIALIZE_DRIVER,
	PMBUS_MFR_FIRMWARE_COMMAND_FORCE_FIRMWARE_FAULTS,
	PMBUS_MFR_FIRMWARE_COMMAND_SYSTEM_RESET,
	PMBUS_MFR_FIRMWARE_COMMAND_DISABLE_I2C_BUS,
	PMBUS_MFR_FIRMWARE_COMMAND_CONFIGURATOR_REMAINING_SIZE,
	PMBUS_MFR_FIRMWARE_COMMAND_OTP_FS_COMMAND_STORE,
	PMBUS_MFR_FIRMWARE_COMMAND_OTP_FS_INVALIDATE,
	PMBUS_MFR_FIRMWARE_COMMAND_TRIM_PROTECT,
	PMBUS_MFR_FIRMWARE_COMMAND_STORE_PARTIAL_CONFIG,
	PMBUS_MFR_FIRMWARE_COMMAND_RESTORE_PARTIAL_CONFIG,
	PMBUS_MFR_FIRMWARE_COMMAND_EXECUTE_PATCH_FROM_RAM,
	PMBUS_MFR_FIRMWARE_COMMAND_READ_COMMON_FAULTS_STATUS,
	PMBUS_MFR_FIRMWARE_COMMAND_GET_LOOP_OTP_SIZE,
	PMBUS_MFR_FIRMWARE_COMMAND_READ_OPEN_SENSE_COMMON_FAULTS_STATUS,
	PMBUS_MFR_FIRMWARE_COMMAND_END_OF_ENUM
} PMBUS_MFR_FIRMWARE_COMMAND_SET_e;

// Error Logging:
void PMBUS_Log_Error(Log_Source_t source, uint32_t program_counter, PMBUS_CML_STATUS_e cml_status);  // set to PROGRAM_COUNTER macro

// Thread queue access:
void PMBUS_Inter_Thread_Set_Command(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, PMBUS_DIRECTION_e direction );

// PMBUS data manipulation API:
__DRIVER_COMMON_INLINE void PMBUS_SET_COMMAND(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint8_t * data );
void PMBUS_Set_Command_Byte(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint8_t data );
void PMBUS_Set_Command_Word(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint16_t data );
void PMBUS_Set_Command_Dword(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint32_t data );
__DRIVER_COMMON_INLINE void PMBUS_COMMAND_OR_BYTE(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint8_t data );
__DRIVER_COMMON_INLINE void PMBUS_COMMAND_OR_WORD(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint16_t data );
__DRIVER_COMMON_INLINE uint8_t * PMBUS_GET_DATA_ARRAY_POINTER(PMBUS_PAGE_t loop, uint8_t cmd);
uint8_t PMBUS_Get_Command_Byte(PMBUS_PAGE_t loop, uint8_t cmd);
uint16_t PMBUS_Get_Command_Word(PMBUS_PAGE_t loop, uint8_t cmd);
uint32_t PMBUS_Get_Command_Dword(PMBUS_PAGE_t loop, uint8_t cmd);
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_COMMAND_SIZE(uint8_t cmd);
uint32_t PMBUS_Calculate_Size_Check(void);

// PMBUS command properties:
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_SUPPORTED(uint8_t cmd);
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_OTP_STORE(uint8_t cmd);
__DRIVER_COMMON_INLINE void PMBUS_Set_Command_PROTECTION(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint8_t data );
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_COMMAND_PROTECTION(PMBUS_PAGE_t loop, uint8_t cmd );
//uint8_t * PMBUS_INTER_THREAD_GET_COMMAND(PMBUS_PAGE_t loop, uint8_t cmd);

// OTP API:
uint32_t PMBUS_Copy_Command_To_RAM(PMBUS_PAGE_t loop, uint8_t *ram_location, uint8_t command);
uint32_t PMBUS_Copy_To_RAM(PMBUS_PAGE_t loop, uint8_t *ram_location);
void PMBUS_Copy_From_Memory(PMBUS_PAGE_t loop, const uint8_t *ram_location);

// PMBUS state functional API:
__DRIVER_COMMON_INLINE PMBUS_PAGE_W_BROADCAST_t PMBUS_GET_PAGE(void);
int32_t PMBUS_Get_Vout_Mode_Exponent(PMBUS_PAGE_t loop);
uint8_t PMBUS_Get_On_Off_State (PMBUS_PAGE_t loop);
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_VID_SOURCE (PMBUS_PAGE_t loop);
uint32_t PMBUS_Get_Vout_Scale(PMBUS_PAGE_t loop, int32_t vout_exp);

// pointer for store/restore functions, firmware download etc
extern uint8_t firmware_data_array[2048];  // max of 1024 data bytes per loop
extern uint8_t * pmbus_data_array_p;

/**
 * Returns the pointer to the data array of a given command (cmd)
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @return Returns the pointer to the uint8_t data array.
 */
__DRIVER_COMMON_INLINE uint8_t * PMBUS_GET_DATA_ARRAY_POINTER(PMBUS_PAGE_t loop, uint8_t cmd)  // worth 5k code size to inline
{
	if (loop==PMBUS_PAGE_0)
		return PMBUS_CMD_ARRAY_LOOP[cmd]->DATA0;
	else //if (loop==PMBUS_PAGE_1)
		return PMBUS_CMD_ARRAY_LOOP[cmd]->DATA1;
}



/**
 * Sets the data array values of a pmbus command
 * Use num_bytes member of command to determine data
 * size.  Ensure that *data pointer is same size as
 * command data or else bogus data will be pulled in!
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @param data Pointer to data to write to command.
 * @return None
 */
__DRIVER_COMMON_INLINE void PMBUS_SET_COMMAND(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint8_t * data )
{
	if (PMBUS_GET_DATA_ARRAY_POINTER(PMBUS_PAGE_0, cmd) == NULL)  // check and see if we have data for the incoming command (both loops are same)
	{
		PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
		return;
	}
	if (loop == PMBUS_PAGE_WB_BROADCAST)
	{
		for (int32_t i = 0; i < ((PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & NUMBYTES_MASK) >> NUMBYTES_POSITION); i++)
		{
			PMBUS_GET_DATA_ARRAY_POINTER(PMBUS_PAGE_0, cmd)[i] = data[i];
			PMBUS_GET_DATA_ARRAY_POINTER(PMBUS_PAGE_1, cmd)[i] = data[i];
		}
	}
	else
	{
		for (int32_t i = 0; i < ((PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & NUMBYTES_MASK) >> NUMBYTES_POSITION); i++)
		{
			PMBUS_GET_DATA_ARRAY_POINTER((PMBUS_PAGE_t)loop, cmd)[i] = data[i];
		}
	}
}

/**
 * Or in current byte sized value of a command (cmd) with (data)
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @param data Data to write to command.
 * @return None
 */
__DRIVER_COMMON_INLINE void PMBUS_COMMAND_OR_BYTE(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint8_t data ) // worth about 1k code size to inline
{
	uint8_t data_temp;
	if (loop == PMBUS_PAGE_WB_BROADCAST)
	{
		data_temp = PMBUS_Get_Command_Byte(PMBUS_PAGE_0, cmd);
		data_temp |= data;
		PMBUS_Set_Command_Byte(PMBUS_PAGE_WB_0, cmd, data_temp);
		data_temp = PMBUS_Get_Command_Byte(PMBUS_PAGE_1, cmd);
		data_temp |= data;
		PMBUS_Set_Command_Byte(PMBUS_PAGE_WB_1, cmd, data_temp);
	}
	else
	{
		data_temp = PMBUS_Get_Command_Byte((PMBUS_PAGE_t)loop, cmd);
		data_temp |= data;
		PMBUS_Set_Command_Byte(loop, cmd, data_temp);
	}

}

/**
 * Or in current word sized value of a command (cmd) with (data)
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @param data Data to write to command.
 * @return None
 */
__DRIVER_COMMON_INLINE void PMBUS_COMMAND_OR_WORD(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint16_t data )
{
	uint16_t data_temp;
	if (loop == PMBUS_PAGE_WB_BROADCAST)
	{
		data_temp = PMBUS_Get_Command_Word(PMBUS_PAGE_0, cmd);
		data_temp |= data;
		PMBUS_Set_Command_Word(PMBUS_PAGE_WB_0, cmd, data_temp);
		data_temp = PMBUS_Get_Command_Word(PMBUS_PAGE_1, cmd);
		data_temp |= data;
		PMBUS_Set_Command_Word(PMBUS_PAGE_WB_1, cmd, data_temp);
	}
	else
	{
		data_temp = PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, cmd);
		data_temp |= data;
		PMBUS_Set_Command_Word(loop, cmd, data_temp);
	}
}



/**
 * Write protect a given command (cmd).  Non-zero values for data are considered protected
 * but data should be treated like a boolean
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @param data Protected if written with non-zero data.
 * @return None
 */
__DRIVER_COMMON_INLINE void PMBUS_Set_Command_PROTECTION(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, uint8_t data )
{
	if (loop == PMBUS_PAGE_WB_BROADCAST)
	{
		if (data == 0)
			PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG = (PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & ~(PROTECTED_MASK));
		else
			PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG = (PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & (PROTECTED_MASK));
	}
	else
	{
		PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG = (PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG &
				~(1<<(PROTECTED_POSITION+loop))) | ((data)<<(PROTECTED_POSITION+loop));
	}
}

/**
 * Returns the current write protection state of a given command (cmd).
 * Non-zero values for data are considered protected
 * but return vlaue should be treated like a boolean
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @return Protected if data is non-zero data.
 */
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_COMMAND_PROTECTION(PMBUS_PAGE_t loop, uint8_t cmd )
{
	return (((PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG&PROTECTED_MASK)>>(PROTECTED_POSITION+(uint32_t)loop))&1);
}

/**
 * Returns whether or not the command is supported
 * return value is non-zero when supported
 * treat it like a boolean
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @return Supported if data is non-zero data.
 */
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_SUPPORTED(uint8_t cmd)
{
	return ((PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & SUPPORTED_MASK)>>SUPPORTED_POSITION);
}

/**
 * Returns whether or not the command is stored in OTP
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @return Supported if data is non-zero data.
 */
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_OTP_STORE(uint8_t cmd)
{
	return ((PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & OTP_STORE_MASK)>>OTP_STORE_POSITION);
}

/**
 * Returns the size in bytes of a given pmbus command (cmd)
 * @param loop Selects the pmbus page to operate on.
 * @param cmd Selects the pmbus command to operate on.
 * @return Size in bytes of command data.
 */
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_COMMAND_SIZE(uint8_t cmd)
{
	return ((PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & NUMBYTES_MASK) >> NUMBYTES_POSITION);
}

/**
 * Returns the current active page for pmbus
 * @return Returns the current active page for pmbus
 */
__DRIVER_COMMON_INLINE PMBUS_PAGE_W_BROADCAST_t PMBUS_GET_PAGE(void)
{
	uint8_t * data = PMBUS_GET_DATA_ARRAY_POINTER(PMBUS_PAGE_0, PMBUS_CMDCODE_PAGE);
	return (PMBUS_PAGE_W_BROADCAST_t) data[0];
}

/**
 * Returns the current active driver of the voltage output
 * Return value decode:
 * VOUT_COMMAND = 0
 * VOUT_MARGIN_LOW = 1
 * VOUT_MARGIN_HIGH = 2
 * AVS = 3 ( unsupported in current device)
 * @param loop Selects the pmbus page to operate on.
 * @return Active driver of VID.
 */
__DRIVER_COMMON_INLINE uint8_t PMBUS_GET_VID_SOURCE (PMBUS_PAGE_t loop)
{
	uint8_t data;
	data = PMBUS_GET_DATA_ARRAY_POINTER((PMBUS_PAGE_t)loop,PMBUS_CMDCODE_OPERATION)[0];
	return ((data & 0x30)>>4);
}

#endif /* PMBUS_API_H */
