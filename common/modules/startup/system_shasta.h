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
 * @file    system_shasta.h
 * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer Header File for Device Shasta.
 *          This file contains the API functions for the CMSIS system & clock configuration.
 */

#ifndef SYSTEM_SHASTA_H
#define SYSTEM_SHASTA_H

#include <stdint.h>             // uint*_t
#include "shasta.h"             // Cortex_M0_Stack_Frame_t
#include "shasta_hal_cgu.h"     // Access to HAL: Clock Generation Unit
#include "cmsis_os_exceptions.h"// osException_t
#include "fw_bool.h"

/* ============================================================================
 * Public data types
 * ========================================================================= */

/** These states are used as a special logging mechanism of the boot sequence in addition to
 * the general log buffer. */
typedef enum
{
    /** The boot sequence was just started. */
    BOOT_STARTED    = 0x0,
    /** The boot descriptor was read. */
    BD_READ         = 0x1,
    /** The end of SystemInit was reached. */
    SYSTEM_INIT     = 0x2,
    /** The memory was initialized. */
    MEM_INIT        = 0x3,
    /** RTOS is initialized. */
    RTOS_INIT       = 0x4,
    /** All modules are initialized. */
    MODULES_INIT    = 0x5,
    /** RTOS was successfully started. Scheduling is running.
     * Last state which has a fixed 4-bit value for RTL-simulation checks. */
    RTOS_STARTED    = 0x8,
    /** The boot sequence was unsuccessful (e.g. due to a missing boot descriptor). */
    BOOT_FAILED     = 0xF

} Osys_Boot_State_t;

/** Callback type for the IRQ handlers from the vector table. */
typedef void (*Irq_Handler)(void);


/* ============================================================================
 * Public data
 * ========================================================================= */

/** @defgroup grpShasta_System_Globals   System global boot resistant data
 * System global boot resistant variables, which are cross module shared all over the system.
 * They are not initialized during system startup.
 * @{
 */

/** Used in case of errors in the HW modules of the CPU subsystem. It contains information about
 * which modules (HW + corresponding FW) of the CPU subsystem shall be disabled. Furthermore,
 * it contains information about how clocking and timing shall be set.
 * See sys_boot_descr_format.h for details. */
extern uint32_t sys_boot_descriptor;

/** State of the boot sequence. */
extern Osys_Boot_State_t sys_boot_state;

/** Counter which is increased for each reboot that was caused by a #CRITICAL error. */
extern uint8_t sys_num_critical_reboots;

/** System Clock defines the clk of the CPU in Hz. */
extern uint32_t SystemCoreClock;

/** @} */ //grpShasta_System_Globals


/* ============================================================================
 * API functions
 * ========================================================================= */

/** Macro to keep track of the boot sequence's progress.
 * Note: Only the lower 4 bits of the CGU_SPARE_FF are allowed to be used. */
#define SYS_BOOT_STATE_TRACK(state) \
    sys_boot_state = (Osys_Boot_State_t)state; \
    CGU_SPARE_FF__SET( (CGU_SPARE_FF__GET() & (uint32_t)0xFFFFFFF0) \
                       | ((uint32_t)0xF & (uint32_t)state) )

/**
SystemInit() initializes the CPU subsystem (CPUS) upon reset. It is called as part of Reset_Handler(), the assembler routine executed on startup.
 
The initialization of the CPU subsystem (CPUS) broadly comprises the following steps:

- Initialize the reset-resistant log buffer if booting from power on. See log_buffer_reset().
- Enable dynamic clock gating for some internal modules (but not PMBus, SVID or OTP) of the CPU subsystem. This allows the relevant clocks to be gated when the processor executes a 'WFI' (Wait For Interrupt) or 'WFE' (Wait For Event) instruction. The hardware requires some time before becoming active. The WFI or WFE instruction notifies the Shasta FW of the readiness of the hardware. See CGU_CLK_SLEEP_MSK_CNFG__SET().
- Enable all wakeup sources for the CPU. See SCU_CPUS_CFG__GET().
- Disable PMBus interrupts by masking. See sys_restore_pmbus_interrupt_mask().
- Read the boot descriptor from SCU_SPARE_FF[29:16] into sys_boot_descriptor and wait endlessly (through 'wfi') until the boot descriptor becomes non-zero (see also sys_boot_descr_format.h).
- Check sys_boot_descriptor whether the CPUS is supposed to be disabled. If this is the case, the CPUS is disabled (sys_power_down()).
- Assign interrupt priorities. See sys_interrupt_priorities_configure().
- Configure the system clock according to the boot descriptor setting. See SystemCoreClockUpdate().
- Activate the block interface (BIF) peripheral clocks to allow FW drivers access to the BIF registers.
- Activate the watchdog timer (WDT) unless otherwise specified.
- Put the master state machine (MSM) on hold before the DC/DC module is started to implement the DC/DC configuration.
 
In addition to the above sequence, SystemInit() keeps track of the boot sequence by logging the boot progress through SYS_BOOT_STATE_TRACK.
 
@note Nothing other than the initialization of hardware is done here. All software initialization follows in a different function of a different file (__main() in __main.c).
 
@note CamelCase due to CMSIS naming convention.
 */
void SystemInit(void);

/**
 * Returns after a minimum of @p delay_us microseconds.
 *
 * The accuracy is better for higher delays:
 * - 1 us delay actually yields 158 cycles @100MHz ==> 1.58 us
 * - 10us delay actually yields 1058 cycles @100MHZ ==> 10.58 us
 *
 * @note This function is delaying to a lower limit, it cannot guarantee to
 * exactly return after a given delay. In an RTOS environment, an accurate delay below
 * the actual scheduling period of the RTOS is not possible. Context switching of RTOS-controlled
 * threads prevents any accuracy below the scheduling period.
 *
 * @param delay_us delay in aggregates of (integer) microseconds
 */
void sys_delay_by_us(uint32_t delay_us);

/**
 * Resets the CPU Subsystem. Program execution will start from the vector table again.
 */
void sys_reset(void);

/**
 * Powers the CPU Subsystem down. Also the wakeup sources will be disabled to prevent any
 * peripheral to power the CPUS up again.
 */
void sys_power_down(void);

/**
 * Updates the variable SystemCoreClock and must be called whenever the core clock is changed
 * during program execution. The function evaluates the clock register settings and calculates the
 * current core clock.
 *
 * @note CamelCase due to CMSIS naming convention.
 */
void SystemCoreClockUpdate(void);

/**
 * Determines if an IRQ is pending and enabled.
 * @param irq_id    Indicator of the IRQ
 * @return @c true if the IRQ is pending and enabled, otherwise @c false.
 */
boolean sys_irq_is_pending(IRQn_Type irq_id);

/**
 * Returns the IRQ handler from the vector table that corresponds to the indicated IRQ.
 * @param irq_id    IRQ for which the handler shall be retrieved
 * @return the IRQ handler from the vector table for @p irq_id
 */
Irq_Handler sys_irq_handler_get(IRQn_Type irq_id);

/**
 * Set the IRQ handler inside the vector table that corresponds to the indicated IRQ.
 * @param irq_id    IRQ of the handler that shall be installed
 * @param handler IRQ handler to install in the vector table for @p irq_id
 * @return none
 */
void sys_irq_handler_set(IRQn_Type irq_id, Irq_Handler handler);

#endif /* SYSTEM_SHASTA_H */
