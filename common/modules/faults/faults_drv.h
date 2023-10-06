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
 * faults_drv.h
 *
 *  Created on: Oct 31, 2016
 *      Author: dlewis4
 */

#ifndef FAULTS_DRV_H_
#define FAULTS_DRV_H_

/* Compiler configurations first */
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
/* Data types */
#include <stddef.h>             // NULL pointer for public structs
#include <faults_api.h>
#define MAX_RETRY_COUNT 16
#include <stdint.h>
/*Fault Driver Structure*/
/*NUM_LOOPS + COMMON*/

typedef struct {
	uint32_t fault_enable;
	uint32_t enable_mask;       /** @todo add comments **/
	uint32_t enable_mask_pmbus;
	uint32_t enable_mask_driver;
	uint32_t fault_status;
    uint32_t fault_polarity;
	uint32_t fault_shut_mask;
    uint32_t fault_status_event_latch;
    FAULT_TYPE_t last_high_priority_fault;
} _FAULT_DRV_s ;

typedef struct {
	uint32_t fault_enable;
	uint32_t enable_mask;
	uint32_t enable_mask_pmbus;
	uint32_t enable_mask_driver;
	uint32_t fault_status;
	uint32_t fault_status_event_latch;
	uint8_t open_sense_fault;
} _FAULT_DRV_COMMON_s ;

extern _FAULT_DRV_COMMON_s FAULT_COMMON_App;

extern _FAULT_DRV_s FAULT_HW_App[NUM_LOOPS]; //Loop0 HW, Loop1 HW
extern _FAULT_DRV_s FAULT_FW_App[NUM_LOOPS]; //Loop0 FW, Loop1 FW

#define FAULTS_API_VERSION   DRIVER_VERSION_MAJOR_MINOR(1,00) /* API version */
#define FAULTS_DRV_VERSION   DRIVER_VERSION_MAJOR_MINOR(1,00) /* API version */

typedef struct {
	int8_t vout;
	int8_t vin;
	int8_t iout;
	int8_t iin;
	int8_t temp;

} _FAULT_THRESH_EXP_s ;

extern _FAULT_THRESH_EXP_s _FAULT_THRESH_EXP;

typedef struct _FAULTS_CAPABILITIES_s
{
	uint32_t FAULTS_CAPABLE;
} FAULTS_CAPABILITIES;

typedef enum _FAULTS_STATUS_e
{
	FAULTS_STATUS_OK,
} FAULTS_STATUS;

DRIVER_VERSION FAULTS_GetVersion (void);
FAULTS_CAPABILITIES FAULTS_GetCapabilities (void);
int32_t FAULTS_Initialize (void);
int32_t FAULTS_Uninitialize (void);
int32_t FAULTS_PowerControl (POWER_STATE state);
int32_t FAULTS_Control (uint32_t control, uint32_t arg);

typedef struct _DRIVER_FAULTS {
  DRIVER_VERSION      (*GetVersion)      (void);
  FAULTS_CAPABILITIES (*GetCapabilities) (void);
  int32_t                 (*Initialize)      (void);
  int32_t                 (*Uninitialize)    (void);
  int32_t                 (*PowerControl)    (POWER_STATE state);
} const DRIVER_FAULTS;

// access functions for FAULTS interface
extern DRIVER_FAULTS Driver_FAULTS;

typedef enum FAULT_CONTROLS_e
{
	FAULT_CONTROL_FAULT_CLEARED_RETRY = 0x0100,
	REGULATION_CONTROL_DELAYED_SHUTDOWN = 0x0200
} FAULT_CONTROLS;

typedef struct {
	FAULT_RESPONSE_TYPE_t fault_response_type;
	uint8_t retry_count;  	// 7:4 retry count max, 3:0 retry counter incremented by regulation driver
	uint32_t delay_time_ms;		// time in ms to delay between retry events
	uint8_t pmbus_status_command;
	uint8_t pmbus_status_bit_position;
	FAULT_TYPE_t hysteresis;
} _FAULT_DESC_s;


//Access to array for storage of _FAULT_s type for every fault registered in firmware
_FAULT_DESC_s * FAULTS_GET_ARRAY_POINTER(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type);

typedef struct
{
	uint16_t pout_op_warn_thresh; //storage for fw limit
	uint16_t pin_op_warn_thresh;  //storage for fw limit
} POWER_WARN_THRESH_s;

extern POWER_WARN_THRESH_s POWER_WARN_THRESH[NUM_LOOPS];

void faults_drv_init(void);

/**
* This function does a lookup on the fault type to determine where to report in pmbus memory
* this is useful for a single fault type, but is slow for a group of faults since it needs to be
* called for x number of concurrent faults.
* @param loop pmbus page to update status on
* @param fault_type fault type to perform lookup from
*/

__FAULT_INLINE void FAULTS_REPORT_PMBUS(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type)
{
	_FAULT_DESC_s * fault = FAULTS_GET_ARRAY_POINTER(loop, fault_type);
	if (fault == NULL)
	{
		PMBUS_Log_Error(FAULTS, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
		return;
	}


	uint8_t latch_data, data;
	data = 0;
	uint32_t * iout_addr = __PMBUS_STATUS_IOUT__ADDRESS((pmbus_status_loop_idx_t)loop);
	uint32_t * base_addr = __PMBUS_STATUS_VOUT__ADDRESS((pmbus_status_loop_idx_t)loop);  // base offset to start from
	uint32_t * register_offset = base_addr + (fault->pmbus_status_command - PMBUS_CMDCODE_STATUS_VOUT) * 	// index from cmd argument
		/*lint -e(946, 947) */
		(iout_addr - __PMBUS_STATUS_VOUT__ADDRESS((pmbus_status_loop_idx_t)loop));  // offset between registers

	latch_data = (uint8_t)HAL_GET32(register_offset);
	// set internal latch
	data = (latch_data | (uint8_t)(1u<<(fault->pmbus_status_bit_position)));
	//Update the latched status
	HAL_SET32(register_offset, data);

	//Check that it's a status command.
	if ((fault->pmbus_status_command >= PMBUS_CMDCODE_STATUS_VOUT) &&
		  (fault->pmbus_status_command <=PMBUS_CMDCODE_STATUS_FANS_1_2))
	PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)loop, fault->pmbus_status_command, (uint8_t)(1u<<(fault->pmbus_status_bit_position)));
	PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)loop, fault->pmbus_status_command, PMBUS_READ);
}

/* ----------------------------------------------------------------------------
 * Forward declaration and documentation of private functions
 * ------------------------------------------------------------------------- */
FAULT_TYPE_t FAULTS_Resolve_Shutdown_Priority(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type_current, FAULT_TYPE_t fault_type_previous);
void faults_drv_irq_enable(void);
void faults_drv_irq_disable(void);
void Faults_Drv_Clear_latched_fault_struct(PMBUS_PAGE_t page);
void COMMON_FAULT_HANDLE(FAULT_TYPE_t fault_code);
void LOOP_FAULT_HANDLE(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type);
void Faults_Mask_Fault(PMBUS_PAGE_t loop , uint8_t hw_fault, uint32_t set_mask, uint32_t clear_mask);
void Faults_Enable_Constant_Current_Mode (PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type, boolean enable);
void Faults_Set_Shutdown_Mask_Hardware_Faults(PMBUS_PAGE_t loop, uint32_t shutdown_mask);
void Faults_Enable_Hardware_Faults(PMBUS_PAGE_t loop, uint32_t enable);
void Faults_Enable_Common_Hardware_Faults(uint32_t enable);
void Faults_Set_Polarity_Hardware_Faults(PMBUS_PAGE_t loop, uint32_t polarity);
void Faults_Clear_Hardware_Faults(PMBUS_PAGE_t loop, uint32_t clear_mask);
void Faults_Clear_Common_Hardware_Faults(uint32_t enable);
void Fault_Threshold_Set(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type, int32_t mantissa, int32_t exponent);
int32_t FAULTS_Get_Threshold_Source_Exponent(FAULT_TYPE_t fault_type);
void Faults_IRQHandler(void);


#endif /* FAULTS_DRV_H_ */
