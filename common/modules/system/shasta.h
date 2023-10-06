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
 * @file    shasta.h
 * @brief   CMSIS Cortex-M0 Core Peripheral Access Layer Header File
 *          for Device Shasta. Also see <a href="http://www.keil.com/pack/doc/cmsis/Core/html/device_h_pg.html">CMSIS: Device Header File</a>.
 */


#ifndef SHASTA_H
#define SHASTA_H

/* Data types */
#include <stdint.h>         /* uint*_t */

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Shasta_Definitions Shasta Definitions
 * This file defines all structures and symbols for Shasta:
 *  - registers and bitfields
 *  - peripheral base address
 *  - peripheral ID
 *  - Peripheral definitions
 * @{
 */


/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/
/** @addtogroup Shasta_CMSIS Device CMSIS Definitions
  Configuration of the Cortex-M0 Processor and Core Peripherals
  @{
*/

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

 /** Number of System Exception Handler slots in the vector table for calculating the
  * IRQ# from an Exception#. */
#define CORTEX_M_NUM_SYSTEM_EXCEPTIONS   16

typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers ***************************************************/

    NonMaskableInt_IRQn         = -14,      /*!< IRQ#  2 Non Maskable Interrupt                        */
    HardFault_IRQn              = -13,      /*!< IRQ#  3 Hard Fault Interrupt                          */
    SVCall_IRQn                 = -5,       /*!< IRQ# 11 SV Call Interrupt                             */
    PendSV_IRQn                 = -2,       /*!< IRQ# 14 Pend SV Interrupt                             */
    SysTick_IRQn                = -1,       /*!< IRQ# 15 System Tick Interrupt                         */

/******  Device Specific Interrupt Numbers ********************************************************/

    /* From the CPU Subsystem ITS "CPUS.pdf".
     * Compare to startup_shasta.s "AREA VECTORS_ROM".
     * Note: In the ITS "external" source means from outside the CPU Subsystem,
     * e.g., SVID unit and "internal" means from within, e.g., Watchdog.
     */
    /* External sources */
	    RAMP0_T1_IRQn               = 0,        /*!<ext0>_IRQn < Interrupt on T1 crossing of pwm ramp0 > */
	    INT_SVID0_IRQn              = 1,        /*!< frameIRQ: A new SVID frame has been received */
	    INT_SVID1_IRQn              = 2,        /*!< sliceIRQ: A detection slice has detected a specific frame */
        CSC_IRQn                    = 3,        /*!< Event in Clock & System Control container */
	    PMBUS_IRQn                  = 4,        /*!< Event in PMBus module */
	    OTP1_W_IRQn                 = 5,        /*!< OPT1 finished operation */
	    RAMP0_T2_IRQn               = 6,        /*!<ext1>_IRQn < Interrupt on T2 crossing of pwm ramp0 */
		RAMP1_T1_IRQn               = 7,        /*!<ext2>_IRQn< Interrupt on T1 crossing of pwm ramp1 */
		RAMP1_T2_IRQn               = 8,        /*!<ext3>_IRQn< Interrupt on T2 crossing of pwm ramp1 */
	    CAL_ALL_DONE_IRQn           = 9,        /*!<ext4>_IRQn< External source is not used (yet) */
	//	<ext5>_IRQn                 = 10,       /*!<ext5>_IRQn< External source is not used (yet) */
	//	<ext6>_IRQn                 = 11,       /*!<ext6>_IRQn< External source is not used (yet) */
	    /* Internal sources */
	    WDT_IRQn                    = 12,       /*!< Event in ARM Watchdog Timer module */
	    DTIMER1_0_IRQn              = 13,       /*!< Event 1 in ARM Dual Timer module 1 */
	    DTIMER1_1_IRQn              = 14,       /*!< Event 2 in ARM Dual Timer module 1 */
	    DTIMER2_0_IRQn              = 15,       /*!< Event 1 in ARM Dual Timer module 2 */
	    DTIMER2_1_IRQn              = 16,       /*!< Event 2 in ARM Dual Timer module 2 */
        DTIMER3_0_IRQn              = 17,       /*!< Event 1 in ARM Dual Timer module 3 */
        DTIMER3_1_IRQn              = 18,       /*!< Event 2 in ARM Dual Timer module 3 */
	    GPIO0_IRQn                  = 19,       /*!< Event in GPIO-0 module */
	    GPIO1_IRQn                  = 20,       /*!< Event in GPIO-1 module */
	    /* More external sources */
        FH_FAULT1_IRQn              = 21,       /*!<ext7>_IRQn< Event 1 in Fault Module module */
        TEST_IRQn              		= 22,      	/*!<ext8>_IRQn < Event 2 in Fault Module module */
        VC0_VCONTROL_IRQn           = 23,       /*!< External source 9: for Voltage Control 0 Window Interrupt */
        VC1_VCONTROL_IRQn           = 24,       /*!< External source 10: for Voltage Control 1 Window Interrupt */
	    TLM_IRQn                    = 25,       /*!< External source 11: Telemetry Block Interrupt */
		FSW_IRQn                	= 26,       /*!< External source 12: Switching Frequency Loop 0 Interrupt */
	//	<ext13>_IRQn                = 27,       /*!< External source 13: Switching Frequency Loop 1 Interrupt */
	    DMA_IRQn                    = 28,       /*!< Event in Direct Memory Access module */
	    SSP_IRQn                    = 29,       /*!< Event in SPI Interface Module */
	    I2C_IRQn                    = 30,       /*!< Event in I2C Interface Module */
	    UART_IRQn                   = 31,       /*!< Event in UART Interface Module */

    /** Number of Device Specific IRQs of the Shasta.
     * Note: This does not have to be the full Cortex vector table with 32 entries.  */
    SHASTA_NUM_IRQS

} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __CM0_REV                 0x0201    /*!< Core Revision r2p1                               */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */

/** This is somewhat tricky (and not very satisfactory at the end, thx to Keil and
  * their predefined compiler macros):
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
  *
  * 2. We could do the same thing here and simply add __CORTEX_M0 to our makefile so that
  * it becomes part of the compiler's command line. But that does not allow Eclipse to
  * understand our code - it does not parse our makefile for -D macros.
  *
  * Therefore, we add it here as part of our configuration file and include fw_config.h
  * in rt_CMSIS.h
  */
#define __CORTEX_M0

/** M0's stack frame after interrupt 'stacking operation' (aka context save).
 * See also @ref sctRefDefinitiveGuideToArmCortexM0, chapter 8 - it explains
 * the details of the stacking procedure and especially the order of the
 * registers in memory.
 */
typedef struct
{
    volatile        uint32_t        r0;     /*!< Register r0 */
    volatile        uint32_t        r1;     /*!< Register r1 */
    volatile        uint32_t        r2;     /*!< Register r2 */
    volatile        uint32_t        r3;     /*!< Register r3 */
    volatile        uint32_t        r12;    /*!< Register r12 */
    volatile        uint32_t        lr;     /*!< Register r14, also known as link register (LR). <br>
                                                 After stacking it contains the address to where the interrupt routine would return to.
                                                 In other words, this is the address of the next instruction that is carried out after
                                                 the interrupt has been serviced. */
    volatile        uint32_t        pc;     /*!< Holds the address of the instruction that was executed when the hard fault (or other fault) occurred. */
    volatile        uint32_t        xpsr;   /*!< Processor status register */

} Cortex_M0_Stack_Frame_t;

/*@}*/ /* end of group Shasta_CMSIS */

#include "core_cm0.h"       /* Cortex-M0 processor and core peripherals */

/** @} */ /* End of group Shasta_Definitions */
/** @} */ /* End of group Infineon */


#endif /* SHASTA_H */
