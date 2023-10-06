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
 * @file    pmbus_drv.h
 * @brief   API of the hardware-level part of the PMBus FW Module.
 */

#ifndef PMBUS_DRV_H
#define PMBUS_DRV_H

/* Compiler configurations first */
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
/* Data types */
#include <stddef.h>             // NULL pointer for public structs
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "driver_common.h"
#include "pmbus_autogen.h"



// macros:
#define NUM_PROTO_BYTES (pmbus_drv_state.NUM_PROTOCOL_BYTES-2)  // this typically will be 0 for regular commands and 1 for block
#define NUMBYTES (pmbus_drv_state.NUM_BYTES)  // number of data bytes in command
#define TOTAL_NUM_BYTES_WRITE (NUMBYTES + pmbus_drv_state.NUM_PROTOCOL_BYTES)  // total number of bytes including command number and protocol bytes
#define TOTAL_NUM_BYTES_WRITE_W_PEC (TOTAL_NUM_BYTES_WRITE+1)  // total number of bytes plus 1 byte for PEC
#define TOTAL_NUM_BYTES_READ_W_PEC (NUMBYTES+NUM_PROTO_BYTES+1)

#define PMBUS_MAX_NUM_BYTES_PER_COMMAND (256)

#define PMBUS_API_VERSION   DRIVER_VERSION_MAJOR_MINOR(1,00) /* API version */
#define PMBUS_DRV_VERSION   DRIVER_VERSION_MAJOR_MINOR(1,00) /* API version */

/* ============================================================================
 * Public data types
 * ========================================================================= */
typedef struct _PMBUS_ADDRESSING_s
{
	uint8_t i2c_slave_address;
	uint8_t pmbus_slave_address;
} PMBUS_ADDRESSING_t;

extern PMBUS_ADDRESSING_t PMBUS_ADDRESSING;

#define ARA_SLAVE_ADDRESS (0xc)
#define I2C_SLAVE_ADDRESS (PMBUS_ADDRESSING.i2c_slave_address)
#define PMBUS_SLAVE_ADDRESS (PMBUS_ADDRESSING.pmbus_slave_address)
#define ARP_SLAVE_ADDRESS (0xc2)
#define ZONE_SLAVE_ADDRESS (0x28)

DRIVER_VERSION PMBUS_GetVersion (void);
int32_t PMBUS_Initialize (void);
int32_t PMBUS_Uninitialize (void);
int32_t PMBUS_PowerControl (POWER_STATE state);
int32_t PMBUS_Control (uint32_t control, uint32_t arg);

typedef struct _DRIVER_PMBUS {
  DRIVER_VERSION   (*GetVersion)      (void);
  int32_t              (*Initialize)      (void);
  int32_t              (*Uninitialize)    (void);
  int32_t              (*PowerControl)    (POWER_STATE state);
} const DRIVER_PMBUS;

/* ============================================================================
 * API functions
 * ========================================================================= */
/* PMBUS Driver */
extern osThreadId tid_PMBUS_Thread;
extern osPoolId pmbus_inter_thread_mpool;
extern osMessageQId  pmbus_inter_thread_queue;
extern mfr_specific_init_ptr ptr_pmbus_driver_user_error_handle;

#ifdef UVM_DEBUG
extern uint32_t PMBUS_HANDLER_STATE;
#endif

typedef struct {
	PMBUS_PAGE_W_BROADCAST_t page;
	uint_least8_t command;
	PMBUS_DIRECTION_e direction;
} pmbus_inter_thread_message_t;

void PMBUS_drv_init(void);

#endif /* PMBUS_DRV_H */
