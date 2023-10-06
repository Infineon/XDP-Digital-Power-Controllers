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
 * @file    cmsis_os_ext.h
 * @brief   API for using a CMSIS-RTOS with custom extensions.
 */

#ifndef CMSIS_OS_EXT_H
#define CMSIS_OS_EXT_H

/** @page pgLib_Rtos_Ext   Custom RTOS extensions
 *
A CMSIS-RTOS is held as small as possible and does not foresee any extensions by nature.
Though, in some cases it does make sense to extend it, especially for modularization purposes.
The highest priority extension is the Structured Exception Handling mechanism for multiple
threads which makes it possible to centralize the exception handling in a single module.

This CMSIS-RTOS API extension encapsulates the actual CMSIS-RTOS API so that the latter
is not affected by the custom extensions. This in turn leads to the necessity to exchange
those functions from the actual CMSIS-RTOS API that need extended functionality by the ones
from this API. The essential function for this purpose is osThreadCreate() which must be
exchanged by osExtThreadCreate(). In addition where necessary substitute all includes of
cmsis_os.h with cmsis_os_ext.h

@todo Add queue priority description for regulation state machine, move cmsis extentions to this file.

@note Nomenclature: All extensions shall follow the coding style of the CMSIS-RTOS API to stay
independent from a varying application coding style.
The prefix of each extension is @c os<Moduleprefix>. For instance, the function descriptors
of the extension for exception handling look like @c osExcFunctionName.

The implemented extensions are:
- @subpage pgLib_StructExc
- @subpage pgLib_Itc
 */

/* Compiler configurations first */
//#include "fw_config.h"  // Essential fw-wide configuration for e.g. buffer size #defines

/* Data types */
#include <stddef.h>     // NULL pointer

/* APIs */
#include "cmsis_os.h"               // Access to all CMSIS-RTOS functions and data types
#include "cmsis_os_exceptions.h"    // Extension for Structured Exception Handling


/* ============================================================================
 * Function declarations
 * ========================================================================= */

/**
 * Extends osKernelInitialize() with the initialization of all kernel extension modules.
 * @return see osKernelInitialize().
 */
osStatus osExtKernelInitialize(void);

/**
 * Extends osThreadCreate() with the initialization of all thread context extension.
 * @param thread_def    see osThreadCreate()
 * @param argument      see osThreadCreate()
 * @return thread ID from osThreadCreate() which now holds a reference to its context extension.
 */
osThreadId osExtThreadCreate(const osThreadDef_t* thread_def, void* argument);

/**
 * Extends the context of an already existing thread.
 * @param thread_id see osThreadCreate()
 */
void osThreadContextExtend(osThreadId thread_id);

#endif /* CMSIS_OS_EXT_H */
