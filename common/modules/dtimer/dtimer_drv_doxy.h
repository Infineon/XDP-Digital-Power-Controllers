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
 * @file    dtimer_drv_doxy.h
 * @brief   Doxygen overview of dtimer FW drivers.
 */

#ifndef DTIMER_DRV_DOXY_H
#define DTIMER_DRV_DOXY_H

/** @page pgShasta_dtimer_drv   DTIMER Driver module
 *
 * @section sctShasta_dtimer_drv_Intro   DTIMER Driver
 * The DTIMER driver firmware module provides programmable timer functionality.
 * <br/>The timers are down-counting and generate interrupts when reaching zero.
 * <br/>Two modes are provided:
 * - Single Shot mode: Only a single interrupt is provided when the counter reaches zero.
 * - Periodic mode: The timer generates an interrupt on zero and restarts automatically.
 *
 * <br/>The counter is working with \p SystemCoreClock which is normally 100MHz.
 *
 * As soon as a timer is initialized the status bit is set indicating that
 * <br/> the timer is busy. Trying to initialize a busy timer will cause an exception.
 * <br/>It has to be uninitialized before it can be initialized again.
 *
 * @section sctShasta_dtimer_drv_single   Single shot mode
 *
 * See figure below. In single shot mode the timer is activated after initialization of the module.
 * <br/>The timer generates one interrupt after the number of \p %load cycles..
 *
 * @image html dtimer_single_shot.svg
 *
 * @section sctShasta_dtimer_drv_periodic   Periodic mode
 *
 * See figure below. In single shot mode the timer is activated after initialization of the module.
 * <br/>The timer generates one interrupt after the number of \p %load cycles.
 * The timer is loaded now with \p %bgload and generate an interrupt continuously after \p %bgload cycles until
 * it is uninitialized.
 *
 *
 * @image html dtimer_periodic.svg
 *
 * @section sctShasta_dtimer_drv_example   Example application
 *
 * The example uses DTIMER0
@code

// APIs
#include "shasta_hal_scu.h"         // write to CGU spare FF
#include "shasta_hal_rgu.h"         // write to CGU spare FF
#include "dtimer_drv.h"             // DTimer driver modules

//  Configuration variable for 1 timer
STATIC DTIMER_CONFIG_t dtimer_cfg;
//Handler for the driver functions
STATIC DRIVER_DTIMER *drv_info;

typedef struct test_register_t
{

    uint32_t x[6];               // counter variable
    uint32_t y[6];               // limit variable for deactivating the timer
    uint32_t reset_register[7];  // store reset status
    uint32_t clk_en_register[7]; // store clk status
} test_register_t;

STATIC test_register_t test_register;

//Interrupt service routine for DTIMER
STATIC void DTIMER_ISR(void)
{
    drv_info->ClearIRQ(DTIMER0);
    test_register.x[0]++;
    RGU_SPARE_FF__SET((uint32_t)0x0UL) ;
    SCU_SPARE_FF__SET((uint32_t)test_register.x[0]) ;
    if (test_register.x[0] > test_register.y[0])
    {
        drv_info->Uninitialize(DTIMER0);
    }
}

// Initialization of DTIMER0 for 100us periodic interrupts
// using DTIMER_ISR
void dtimer0_activation (void)
{
    drv_info = &Driver_DTIMER;

    // Initialize test registers ()
    test_register.x[i] = 0;
    // Count 3 times before stopping the timer
    test_register.y[i] = 3;

    // Initialize the timer variable to zero
    memset(&dtimer_cfg, 0, sizeof(dtimer_cfg));

    dtimer_cfg.mode   = PERIODIC_INT;

    //Period of 10000 clock cycles (a clock cycle is 10ns thus the time is 100us)
    dtimer_cfg.load   = 10000;
    dtimer_cfg.bgload = 10000;

    dtimer_cfg.dtimer_ISR = DTIMER_ISR;

    drv_info->Initialize(DTIMER0, dtimer_cfg);

    // wait until the timer is finished
    while (drv_info->GetStatus(DTIMER0) == DTIMER_STATUS_BUSY)
    {
    }
    // end of cycles reached
}
@endcode

See
- dtimer_drv.h for details on the API
- CPUS ITS for details on the HW Interface
    https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/03_System/CPUS.pdf
 */

#endif /* DTIMER_DRV_DOXY_H */
