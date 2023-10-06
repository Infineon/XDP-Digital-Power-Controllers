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
 * @file    fw_config.h
 * @brief   Collection of constant configuration parameters for the whole firmware.
 *          This file must be included by all files that make up a FW module.
 *          General utility files should not include it.
 *          Note that changes to this file will mean a full re-compilation of the FW.
 */
#ifndef FW_CONFIG_H
#define FW_CONFIG_H

/* ============================================================================
 * Image location
 * ========================================================================= */
#include "image_types.h"

/**
 * The project specific @ref IMAGE_TYPE definition is set to @ref ROM_IMAGE
 * in case the project is compiled to be executed from the ROM.
 * This means that the code and constant variables
 * are stored in ROM, and the variables, stack and heap and kept in ROM.
 *
 * This definition is used inside the source code to include ROM code related
 * initialization parts that need to be included during the build.
 */
#define IMAGE_TYPE                                  ROM_IMAGE

/* ============================================================================
 * Configuration of the firmware modules
 * - The documentation can be found within the corresponding module
 * ========================================================================= */

/* --- Operation & Maintenance FW Module --- */
/** The maximum number of reboots due to #CRITICAL issues before the CPU Subsystem will
 * be disabled / powered down. */
#define SYS_MAX_CRITICAL_REBOOTS                    8
/** Time in seconds when the watchdog timer triggers a firmware reset if itself was not reset. */
#define SYS_WATCHDOG_EXPIRATION_MILLISECONDS             (4000)

/** Specific stack size for the application threads */
/* Note: Stack size equal to zero means that RTX uses the default stack size,
 * which is fix allocated during linker time.
 * The dynamic stack allocation requires that the RTX dynamic memory
 * allocation pool is well configured. */
#define OS_THREAD_STACK_SIZE                       (OS_STKSIZE * 4)

/* ============================================================================
 * RTOS configuration
 * - The documentation can be found within RTX_Conf_CM.c
 * - Commented lines contain the default value
 * ========================================================================= */

/* --- Threads --- */
/* Total amount of task (excluding main, os_idle_demon and timer) */
#define OS_TASKCNT                                  5
/* Stack size of the task which do not use dynamic allocated stack.
 * In our configuration this is only the idle demon. */
#define OS_STKSIZE                                  300
/* We do not use the main thread since it is defined with a fixed priority by RTX.
 * There was no mapping of our threads to be a main thread with osPriorityNormal yet. */
#define OS_MAINSTKSIZE                              100
/*lint -esym(755, OS_PRIVCNT)
 * This RTX configuration, not used by the linted code parts. */
/* all task should use dynamic allocate stack
 * (each might have a different stack size). */
#define OS_PRIVCNT                                  (OS_TASKCNT-1) /* Cannot be equal. -1 is a workaround for a part of OPUS-685. */
/*lint -esym(755, OS_PRIVSTKSIZE)
 * This RTX configuration, not used by the linted code parts. */
#define OS_PRIVSTKSIZE                              (OS_TASKCNT * 300)
/* #define OS_STKCHECK                                 1 */
/* #define OS_RUNPRIV                                  1 */

/* --- Timer Ticks --- */
/* #define OS_SYSTICK                                  1          */
/* #define OS_CLOCK                                    100000000  */
/* #define OS_TICK                                     1000       */

/* --- System --- */
/* #define OS_ROBIN                                    1    */
/* #define OS_ROBINTOUT                                5    */
#define OS_TIMERS                                   1
#define OS_TIMERPRIO                                5
#define OS_TIMERSTKSZ                               200
#define OS_TIMERCBQS                                4
#define OS_FIFOSZ                                   64
/* #define OS_MUTEXCNT                                 8    */

/* ============================================================================
 * Device specific constants
 * ========================================================================= */

/**
 * Determine the used core.
 * This is somewhat tricky (and not very satisfactory at the end, thx to Keil and
 * their weird way of configuring RTX):
 * 1. Using the RTX RTOS with the CMSIS-compliant API, you are required to compile
 * a source file rt_CMSIS.c. Looking into this file, you find that it wants to include
 * a HAL header file for the respective cortex core you are using: M0 or M3 or whatever.
 * Including the correct HAL file (e.g. core_cm0.h) is done by evaluating a predefined
 * macro, e.g. __CORTEX_M0. At this point in time, in the original rt_CMSIS.c, @b NO
 * other include file has been included. That means, __CORTEX_M0 is either defined
 * on the command line of the compiler or it is a predefined macro of the compiler.
 * In the case of the IAR tool chain, it is defined on the command line - you can check this
 * when you rebuild the RTX library with their tools: The project file defines
 * __CORTEX_M0 as part of the options and it appears on the command line.
 * In case of the Keil tools, I would assume that this part of the predefined macros -
 * they wrote RTX and this macros has 2 leading underscores, so it should be defined
 * by the compiler itself. Looking at Keil's compiler manual, it is not. Thank you ...
 *
 * 2. We could do the same thing here and simply add __CORTEX_M0 to our makefile so that
 * it becomes part of the compiler's command line. But that does not allow Eclipse to
 * understand our code - it does not parse our makefile for -D macros.
 *
 * Therefore, we add it here as part of our configuration file and include fw_config.h
 * in rt_CMSIS.h
 */
#define __CORTEX_M0

/** Fixed frequency in MHz of the primary clock source (hosc_clk) */
#define FREQ_PRIMARY_CLOCK                          ((uint32_t) 100)

/** Expiration time in ms for the watchdog timer if SYS_BD_WD_TM_ENABLE is set. */
#define SYS_WATCHDOG_SIM_EXPIRATION_MS              40

#define UVM_DEBUG

/* --- OTP FW Module --- */
/** Maximum number of OTP write messages in the OTP driver */
#define OTP_QUEUE_LENGTH 5
#define PMBUS_QUEUE_LENGTH (32)
#define REGULATION_QUEUE_LENGTH (32)

#define NUM_LOOPS (2)
#define SCRATCHPAD_SIZE_IN_BYTES (4096)
//#define PMBUS_DRIVER_DEBUG
//#define DISABLE_RANGE_CHECKS
//#define DISABLE_ERROR_CHECKS
//#define CHECK_PMBUS_ADDRESS_ON_RESTART

#endif /* FW_CONFIG_H */
