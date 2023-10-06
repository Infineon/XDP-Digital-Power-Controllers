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
/*
 * telemetry_irq_handlers.h
 *
 *  Created on: May 23, 2017
 *      Author: agrishi1
 */

#ifndef TELEMETRY_IRQ_HANDLERS_H_
#define TELEMETRY_IRQ_HANDLERS_H_

#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
/* Data types */
#include <stddef.h>             // NULL pointer for public structs
#include "driver_common.h"
#include "shasta_hal_telem.h"

typedef enum _Telemetry_IRQ_TYPE__e
{
	VOUT_POWER_GOOD_ON_OFF_0           	= 0u,
	VOUT_POWER_GOOD_ON_OFF_1           	= 1u,
	VIN_ON_OFF_0                       	= 2u,
	VIN_ON_OFF_1                       	= 3u,
	VOUT_UV_TON_MAX_DISABLE_0          	= 4u,
	VOUT_UV_TON_MAX_DISABLE_1          	= 5u,
	TELEM_IRQ6                     	 	= 6u,
	TELEM_IRQ7                      	= 7u,
	TELEM_IRQ8                       	= 8u,
	TELEM_IRQ9                       	= 9u,
	TELEM_IRQ10          				= 10u,
	TELEM_IRQ11  		   				= 11u,
	TELEM_IRQ12                   		= 12u,
	TELEM_IRQ13               			= 13u,
	TELEM_IRQ14               			= 14u,
	TELEM_IRQ15                         = 15u,

	Telemetry_IRQ_TYPE__END_OF_Telemetry_IRQ_TYPES 	= 16u  //Total 16 generic interrupts supported

} Telemetry_IRQ_TYPE_t;

void Telemetry_IRQ_STUB_HANDLE(void);
void Telemetry_IRQ_VOUT_POWER_GOOD_ON_OFF_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type);
void Telemetry_IRQ_VIN_ON_OFF_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type);
void Telemetry_VOUT_UV_TON_MAX_DISABLE_Irq_Threshold_Set(PMBUS_PAGE_t loop, uint32_t mantissa, int8_t exponent);

#endif /* TELEMETRY_IRQ_HANDLERS_H_ */
