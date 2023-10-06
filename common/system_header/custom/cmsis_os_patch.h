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
 * @file    cmsis_os_patch.h
 * @brief   patched cmsis API calls which are compatible with GNU and ARM.
 */

#ifndef CMSIS_OS_PATCH_H
#define CMSIS_OS_PATCH_H

/** @page pgLib_Rtos_patch  Custom RTOS Patches
 *
 * This header file provides an API for RTOS functions which require the @c __value_in_regs attribute.<br>
 * The attribute allows structures to be returned in a non-eabi fashion via registers instead of by reference.<br>
 * Unfortunately this feature is not supported by the GNUC C-compiler. Thus it is not possible to call
 * RTOS functions compiled with ARM C-compiler from code which is compiled with gcc.
 *
 * Following functions are affected:
 * - os_InRegs osEvent osWait (uint32_t millisec); // if enabled
 * - os_InRegs osEvent osSignalWait (int32_t signals, uint32_t millisec);
 * - os_InRegs osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec);
 * - os_InRegs osEvent osMailGet (osMailQId queue_id, uint32_t millisec);
 *
 * This API appends @c _patch to the respective function. External code using this API can be compiled either with the
 * ARM or the GNU C-Compiler.<br>
 * For ARM compiled code the call is transparent, for GNU compiled code a wrapper is applied to the call.
 *
 * @section pgLib_Rtos_patch_wrapper  Wrapper for RTOS Functions
 *
 * The wrapper is implemented in Assembler.
 *
 *  - @c R0 contains the pointer where the return values shall be stored.
 *  - @c R7 is used as a scratch register to hold the pointer address.
 *
 *  @b Constraints:
 *  - Output has no constraints because we are using register @c R7 as scratch register.
 *  - Input has the two input variables as constraints.
 *  - @c R7 is defined as clobber register
 *
@code
    asm volatile (
        "movs r7, r0\n\t"
        "movs r0, %0\n\t"
        "movs r1, %1\n\t"
        "bl osMessageGet\n\t"
        "str r0, [r7]\n\t"
        "str r1, [r7, #4]\n\t"
        "str r2, [r7, #8]\n\t"
        :
        : "r" (queue_id), "r" (millisec)
        : "r7"
    );
@endcode
 *
 * GNU Inline Assembly documentation can be found in
 * <a href="http://www.ethernut.de/en/documents/arm-inline-asm.html">ARM GCC Inline Assembler Cookbook</a>.
 *
 */

/* APIs */
#include "cmsis_os.h"            // Access to all CMSIS-RTOS functions and data types
#include "environment.h"        /* INLINE */

/* ============================================================================
 * Function declarations
 * ========================================================================= */

 /**
 * Wrapper for osMessageGet (osMessageQId queue_id, uint32_t millisec).
 */

#if defined (__CC_ARM)

INLINE osEvent osMessageGet_patch (osMessageQId queue_id, uint32_t millisec)
{
    return osMessageGet (queue_id, millisec);
}
#else

osEvent osMessageGet_patch (osMessageQId queue_id, uint32_t millisec)
{
    osEvent evt;

    asm volatile (
        "movs r7, r0\n\t"
        "movs r0, %0\n\t"
        "movs r1, %1\n\t"
        "bl osMessageGet\n\t"
        "str r0, [r7]\n\t"
        "str r1, [r7, #4]\n\t"
        "str r2, [r7, #8]\n\t"
        :
        : "r" (queue_id), "r" (millisec)
        : "r7"
    );
    return evt;
}
	
#endif //__CC_ARM

/**
 * Wrapper for os_InRegs osEvent osMailGet (osMailQId queue_id, uint32_t millisec).
 */

#if defined (__CC_ARM)

/*
 * Wrapper for the ARM compiler. Due to the INLINE instruction the
 * osMessageGet is called directly.
 */
INLINE osEvent osMailGet_patch (osMailQId queue_id, uint32_t millisec)
{
    return osMailGet (queue_id, millisec);
}
#else

osEvent osMailGet_patch (osMailQId queue_id, uint32_t millisec)
{
    osEvent evt;

    asm volatile (
        "movs r7, r0\n\t"
        "movs r0, %0\n\t"
        "movs r1, %1\n\t"
        "bl osMessageGet\n\t"
        "str r0, [r7]\n\t"
        "str r1, [r7, #4]\n\t"
        "str r2, [r7, #8]\n\t"
        :
        : "r" (queue_id), "r" (millisec)
        : "r7"
    );
    return evt;
}

#endif //__CC_ARM

#if (defined (osFeature_Wait)  &&  (osFeature_Wait != 0))     // Generic Wait available

/**
 * Wrapper for os_InRegs osEvent osWait (uint32_t millisec)
 */

#if defined (__CC_ARM)

INLINE osEvent osWait_patch (uint32_t millisec)
{
    return osWait(millisec);
}
#else

osEvent osWait_patch (uint32_t millisec)
{
    osEvent evt;

    asm volatile (
        "movs r7, r0\n\t"
        "movs r0, %0\n\t"
        "bl osWait\n\t"
        "str r0, [r7]\n\t"
        "str r1, [r7, #4]\n\t"
        "str r2, [r7, #8]\n\t"
        :
        : "r" (millisec)
        : "r7"
    );
    return evt;
}

#endif //__CC_ARM

#endif // Generic Wait available
/**
* Wrapper os_InRegs osEvent osSignalWait (int32_t signals, uint32_t millisec);.
*/

#if defined (__CC_ARM)

INLINE osEvent osSignalWait_patch (int32_t signals, uint32_t millisec)
{
   return osSignalWait_patch (signals, millisec);
}
#else

osEvent osSignalWait_patch (int32_t signals, uint32_t millisec)
{
   osEvent evt;

   asm volatile (
       "movs r7, r0\n\t"
       "movs r0, %0\n\t"
       "movs r1, %1\n\t"
       "bl osMessageGet\n\t"
       "str r0, [r7]\n\t"
       "str r1, [r7, #4]\n\t"
       "str r2, [r7, #8]\n\t"
       :
       : "r" (signals), "r" (millisec)
       : "r7"
   );
   return evt;
}

#endif //__CC_ARM

#endif /* CMSIS_OS_PATCH_H */
