/**
 * @file    periodic_functions.c
 * @brief
 */

/* ============================================================================
** Copyright (C) 2020 Infineon. All rights reserved.
**               Infineon Technologies, PSS SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorisation.
** ============================================================================
**
** Created on:  2020-08-19
*/

#include "telemetry_api.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "shasta_hal_tsen.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_analog.h"
#include "shasta_hal_common.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_isen.h"
#include "log_app.h"             /* Logging of errors */
#include <string.h>             /* memset */
#include "patch_rom_table.h"
#include "string.h"
#include "shasta_hal_tstctrl.h"  	// ate test state
#include "mmu_drv.h"                /* MMU_Initialization */
#include "driver_common.h"
#include "shasta_hal_fault_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "periodic_functions.h"

/*
 * Put here your periodic functions.
 */

void periodic_function_init (void)
{

}
