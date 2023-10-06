/* ============================================================================
** Copyright (C) 2017 Infineon Technologies AG
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
 * @file    telemetry_drv.h
 * @brief   API of the hardware-level part of the Telemetry FW Module.
 */

#ifndef TELEMETRY_DRV_H_
#define TELEMETRY_DRV_H_

/* Compiler configurations first */
#include "fw_config.h"          // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
/* Data types */
#include <stddef.h>             // NULL pointer for public structs
#include "cmsis_os.h"           // ARM::CMSIS:RTOS:Keil RTX
#include "driver_common.h"
#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "telemetry_irq_handlers.h"
#include "telemetry_api.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_telem_common.h"

#define Telemetry_API_VERSION   DRIVER_VERSION_MAJOR_MINOR(1,00) /* API version */
#define Telemetry_DRV_VERSION   DRIVER_VERSION_MAJOR_MINOR(1,00) /* API version */

typedef enum _Telemetry_STATUS_e
{
    Telemetry_STATUS_OK,
} Telemetry_STATUS;
void Telemetry_irq_enable(void);
void Telemetry_irq_disable(void);
void Telemetry_IRQHandler(void);
DRIVER_VERSION Telemetry_GetVersion (void);
int32_t Telemetry_Initialize (void);
int32_t Telemetry_Uninitialize (void);
int32_t Telemetry_PowerControl (POWER_STATE state);
int32_t Telemetry_Control (uint32_t control, uint32_t arg);
// irq declarations:



typedef struct _DRIVER_TELEM {
  DRIVER_VERSION     (*GetVersion)      (void);
  int32_t                (*Initialize)      (void);
  int32_t                (*Uninitialize)    (void);
  int32_t                (*PowerControl)    (POWER_STATE state);
} const DRIVER_TELEM;

typedef struct {
	uint32_t vout[NUM_LOOPS];	//!< current telemetry value for output voltage of each loop
	int32_t iout[NUM_LOOPS];	//!< current telemetry value for output current of each loop
	uint32_t iin[NUM_LOOPS];	//!< current telemetry value for input current of each loop
	uint32_t vin[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t pout[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t pin[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t fsw[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t duty[NUM_LOOPS];	//!< current telemetry value for input voltage of each loop
	uint32_t prebias_voltage[NUM_LOOPS];	//!< current telemetry value for output voltage at startup
	int32_t leakage_current[NUM_LOOPS];	//!< current telemetry value for output current at startup
	uint32_t cout[NUM_LOOPS];	//!< current telemetry value for output capacitance of each loop
	uint32_t lout[NUM_LOOPS];	//!< current telemetry value for output inductance of each loop
	int32_t temperature[3];  	//!< current telemetry value for temperature, index: tempa = 0, tempb = 1, tempi = 2
} TELEM_DATA_t;

// telemetry fast access data structure shared with user
extern TELEM_DATA_t TELEM_DATA;

// Hardware block parameters:
#define DUTY_CYCLE_LSB (2^-16)
//#define SWITCHING_PERIOD_KHZ_LSB (1)      // khz
#define INPUT_CURRENT_AMPS_LSB (0.0078125)  // 1/128
#define VOUT_DIV_VIN_LSB (2^12)             // V/V
#define IOUT_CURRENT_AMPS_LSB (0.0625)      // Amps
#define TEMPERATURE_C_LSB (0.125)           // degrees C, good for A,B and I sources
#define VIN_VOLTAGE_VOLTS_LSB (0.0625)      // Volts
#define VOUT_VOLTAGE_VOLTS_LSB (0.00125)    // Volts
#define ONE_DIV_VOUT_VOLTAGE_VOLTS_LSB (800)

/* ============================================================================
 * API functions
 * ========================================================================= */
/* TELEM Driver */
extern osThreadId tid_Telemetry_Thread;

void Telemetry_drv_init(void);
void Telemetry_Sample(void);
void Telemetry_Thread(void const* arg);

#endif /* TELEMETRY_DRV_H_ */
