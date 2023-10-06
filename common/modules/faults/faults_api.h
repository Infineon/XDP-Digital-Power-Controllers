/* ============================================================================
** Copyright (C) 2015 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/*
 * faults_api.h
 *
 *  Created on: Oct 26, 2016
 *      Author: dlewis4
 */

#ifndef FAULTS_API_H_
#define FAULTS_API_H_

/* Compiler configurations first */
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
/* Data types */
#include <stddef.h>             // NULL pointer for public structures
#include <driver_common.h>
#include "pmbus_autogen.h"
#include "pmbus_api.h"
//#include "faults_drv.h"


// hals:
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_common.h"
//#include "shasta_hal_isen.h"
#include "regulation_api.h"
#include "regulation_drv.h"

__FAULT_INLINE uint32_t* __FAULT__THRESHOLD__ADDRESS(fault_idx_t idp, uint8_t ida)
{
    return (uint32_t*)((uint32_t)__FAULT_BASE_PTR(idp) + (uint32_t)(offsetof(FAULT_t, FAULT_VOUT_OV_FLT)) + (uint32_t)(sizeof(uint32_t)*(uint32_t)(ida-1))); //Fault types start at 1 thus -1
}

__FAULT_INLINE void FAULT_THRESHOLD__SET(fault_idx_t idp, uint8_t ida, uint32_t value)
{
    HAL_SET32(__FAULT__THRESHOLD__ADDRESS(idp, ida), value);
}

/*
 * This is used to decode the fields in FW_CONFIG_FAULTS PMBUS command:
 */
typedef enum FAULTS_CONFIG_e
{
	FAULTS_CONFIG_TEMPERATURE_DELAY_UNIT = 0u,
	FAULTS_CONFIG_IOUT_DELAY_UNIT = 2u,
	FAULTS_CONFIG_VIN_DELAY_UNIT = 4u,
	FAULTS_CONFIG_VOUT_DELAY_UNIT = 6u,
	FAULTS_CONFIG_FAULT_PIN_MASK_HW = 8u,
	FAULTS_CONFIG_FAULT_PIN_MASK_FW = 40u,
	FAULTS_CONFIG_FAULT_ENABLE_MASK_LOOP_HW = 72u,
	FAULTS_CONFIG_FAULT_ENABLE_MASK_LOOP_FW = 104u,
	FAULTS_CONFIG_FAULT_ENABLE_MASK_LOOP_COMMON = 136u,
	FAULTS_CONFIG_FAULT_T2_SHUT_MASK_LOOP_HW = 168u
} FAULTS_CONFIG;

/**
 * This is used to decode response command byte. Voltage(Temp, TON MAX) type or CURRENT type response
 */
typedef enum RESPONSE_CONFIG_e
{
	VOLTAGE_RESPONSE_TYPE = 0u,
	CURRENT_RESPONSE_TYPE = 1u
} RESPONSE_CONFIG_t;

/** enumeration of FAULT_TYPE_ member index
 */
/*lint -parent(uint32_t, FAULT_TYPE__t) -strong(AJX)*/
/** @todo should make these explicitly defined as well **/
typedef enum _FAULT_TYPE__e
{
	RESERVED_HW_NO_FAULT,	// HW FAULT BIT 00 /*NO FAULT*/
	// HW controlled faults
	FAULT_TYPE_VOUT_OV_FAULT,
	FAULT_TYPE_VOUT_OV_WARN,
	FAULT_TYPE_VOUT_UV_FAULT,
	FAULT_TYPE_VOUT_UV_WARN,
	FAULT_TYPE_VIN_OV_FAULT,
	FAULT_TYPE_VIN_OV_WARN,
	FAULT_TYPE_VIN_UV_FAULT,
	FAULT_TYPE_VIN_UV_WARN,
	FAULT_TYPE_IOUT_OC_FAULT,
	FAULT_TYPE_IOUT_OC_LV_FAULT,
	FAULT_TYPE_IOUT_OC_WARN,
	FAULT_TYPE_IOUT_UC_FAULT,
	FAULT_TYPE_MFR_IOUT_OC_FAST,
	FAULT_TYPE_IIN_OC_FAULT,
	FAULT_TYPE_IIN_OC_WARN,
	FAULT_TYPE_OT_FAULT,
	FAULT_TYPE_OT_WARN,
	FAULT_TYPE_UT_FAULT,
	FAULT_TYPE_UT_WARN,
	FAULT_TYPE_IN_POWER_LIMITING_MODE,
	FAULT_TYPE_CURRENT_SHARE_FAULT,
	FAULT_TYPE_VOUT_MAX_MIN_WARN,
	FAULT_TYPE_SYNC_FAULT ,
	//FAULT_TYPE_FLUX_BALANCE_FAULT,
	hw_spare1,
	hw_spare2,
	hw_spare3,
	hw_spare4,
	hw_spare5,
	hw_spare6,
	hw_spare7,
	FAULT_TYPE__END_OF_HW_FAULT_TYPES,


	// common HW faults:
	RESERVED_FW_NO_FAULT,  // FW FAULT BIT 00 /*NO FAULT*/
	FAULT_TYPE_COMMON_SHUTDOWN,

	// FW controlled faults:
	FAULT_TYPE_TON_MAX_FAULT,
	FAULT_TYPE_TOFF_MAX_WARN,
	FAULT_TYPE_PIN_OP_WARN,
	FAULT_TYPE_POUT_OP_WARN,
	FAULT_TYPE_VIN_INSUFFICIENT,

	// spares for user:
	fw_spare1,
	fw_spare2,
	fw_spare3,
	fw_spare4,
	fw_spare5,
	fw_spare6,
	fw_spare7,
	fw_spare8,
	fw_spare9,
	fw_spare10,
	fw_spare11,
	fw_spare12,
	fw_spare13,
	fw_spare14,
	fw_spare15,
	fw_spare16,
	fw_spare17,
	fw_spare18,
	fw_spare19,
	fw_spare20,
	fw_spare21,
	fw_spare22,
	fw_spare23,
	fw_spare24,
	FAULT_TYPE__END_OF_FAULT_TYPES

} FAULT_TYPE_t;


/** enumeration of FAULT_TYPE_COMMON_ member index
 */
/*lint -parent(uint32_t, FAULT_TYPE_COMMON__t) -strong(AJX)*/
/** @todo should make these explicitly defined as well **/
typedef enum _FAULT_TYPE_COMMON__e
{
	RESERVED_COMMON_NO_FAULT,	// HW FAULT BIT 00 /*NO FAULT*/
	common_spare0,
	IS1_TRACK_FAULT,
	IS2_TRACK_FAULT,
	FBAL1_FAULT,
	ISP0_IOUT_PCL_FAULT,
	ISP0_IOUT_SCP_FAULT,
	FBAL2_FAULT,
	ISP1_IOUT_PCL_FAULT,
	ISP1_IOUT_SCP_FAULT,
	common_spare1,
	VS0_VREF_OPEN_FAULT,
	VS0_VSEN_OPEN_FAULT,
	common_spare2,
	VS1_VREF_OPEN_FAULT,
	VS1_VSEN_OPEN_FAULT,
	common_spare3,
	VS2_VREF_OPEN_FAULT,
	VS2_VSEN_OPEN_FAULT,
	common_spare4,
	common_spare5,
	common_spare6,
	common_spare7,
	common_spare8,
	common_spare9,
	common_spare10,
	common_spare11,
	common_spare12,
	common_spare13,
	common_spare14,
	common_spare15,
	FAULT_TYPE_COMMON__END_OF_FAULT_TYPES
} FAULT_TYPE_COMMON_t;

typedef enum _OPEN_SENSE_FAULT__e
{
	VSEN_FAULT,
	VREF_FAULT,
	VRSEN_FAULT,
	VRREF_FAULT,
	BVSEN_FAULT,
	BVREF_FAULT

} OPEN_SENSE_FAULT_t;
/** enumeration of FAULT_RESPONSE_TYPE_ member index
 */
/*lint -parent(uint32_t, FAULT_RESPONSE_TYPE__t) -strong(AJX)*/
// Covers VOUT, temperature, and ton_max fault types:
typedef enum _FAULT_RESPONSE_TYPE__e
{
	FAULT_RESPONSE_IGNORE = 0u,
	FAULT_RESPONSE_RETRY_OPERATION = 1u,
	FAULT_RESPONSE_RETRY_SHUTDOWN = 2u,
	FAULT_RESPONSE_SHUTDOWN_RESTART_WHEN_REMOVED = 3u,
	FAULT_RESPONSE_IOUT_CONSTANT_CURRENT  = 4u,
	FAULT_RESPONSE_IOUT_CONSTANT_CURRENT_WITH_OC_LV = 5u,
	FAULT_RESPONSE_IOUT_DELAY_WITH_CC_RETRY = 6u,
	FAULT_RESPONSE_IOUT_SHUTDOWN_AND_RETRY = 7u
} FAULT_RESPONSE_TYPE_t;

/* Clear Faults */
void Faults_Clear_Faults(PMBUS_PAGE_t page);
void FAULTS_SETUP_FAULT(
		PMBUS_PAGE_t loop,
		FAULT_TYPE_t fault_type,
		uint8_t fault_response_cmd_byte, //Response[7:6], Retry_Setting{5:3] and Delay_Time[2:0]
		FAULTS_CONFIG Config_Delay_Unit,
		RESPONSE_CONFIG_t response_config,
		uint8_t pmbus_status_command,
		uint8_t pmbus_status_bit_position,
		FAULT_TYPE_t hysteresis);

uint32_t FAULTS_Get_PMBUS_Configuration(PMBUS_PAGE_t loop, FAULTS_CONFIG config);
FAULT_RESPONSE_TYPE_t FAULTS_Get_Fault_Response_Type(RESPONSE_CONFIG_t response_config, uint8_t fault_response_cmd_byte);

void Set_Firmware_Fault(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type);
void Clear_Firmware_Fault(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type);

#endif /* FAULTS_API_H_ */
