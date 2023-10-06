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
 * @file    mmu_drv_doxy.h
 * @brief   Doxygen overview of mmu FW drivers.
 */

#ifndef DTIMER_DRV_DOXY_H
#define DTIMER_DRV_DOXY_H

/** @page pgShasta_mmu_drv   MMU Driver module
 *
 * @section sctShasta_mmu_drv_Intro   MMU Driver
 * The Memory Management Unit supports two main functionalities.
 * - Memory Mapping:
 * <br/> The MMU driver firmware module provides a memory mapping functionality.
 * The System memory comprising of ROM, OTP, RAM1, RAM2 is segmented in 1kbyte
 * pages. Each page of each block can be re-mapped to any other memory block.
 * - Write protection:
 * <br/> Additionally each page can be write protected.
 * A write to a protected memory cell
 * generates a Hardfault Interrupt which captures the status information.
 * The status information contains the block and the page information of the affected memory cell.
 * - Register Write protection:
 * <br/> This is a specific protection for the configuration registers of the device.
 *
 * The memory blocks, the start address, block index and number of pages are shown below.
 * <br/> The RAM is segmented in 2 blocks for layout purposes.
 *
 * | Memory | Start Address | Block  |  Pages  |
 * | :----: | :-----------: | :----: | :----:  |
 * | ROM    | 0x1000_0000   |   0    |   64    |
 * | OTP    | 0x1001_0000   |   1    |   64    |
 * | RAM1   | 0x2002_C000   |   2    |   16    |
 * | RAM2   | 0x2003_0000   |   3    |   16    |
 *
 * The MMU allows mapping according to the table below. Any mapping is allowed.
 *
 * |       |  ROM  |  OTP  |  RAM1 |  RAM2 |
 * | :--:  | :---: | :---: | :---: | :---: |
 * | ROM   |   x   |   x   |   x   |   x   |
 * | OTP   |   x   |   x   |   x   |   x   |
 * | RAM1  |   x   |   x   |   x   |   x   |
 * | RAM2  |   x   |   x   |   x   |   x   |
 *
 * The register protection is organized as follows.
 *
 * | Register | Start Address | Index  |  Comment                  |
 * | :------: | :-----------: | :----: |                           |
 * |   NONE   |               |   1    |   Dummy entry not used    |
 * |   CPUP   | 0x6000_0000   |   2    |   CPU registers           |
 * |   TRIM   | 0x7000_0000   |   4    |   Regulation registers    |
 * |   ANA    | 0x7000_0400   |   4    |   Regulation registers    |
 * |   VDPS0  | 0x7000_0800   |   4    |   Regulation registers    |
 * |   VDPS1  | 0x7000_0C00   |   4    |   Regulation registers    |
 * |   VDPS2  | 0x7000_1000   |   4    |   Regulation registers    |
 * |   VCTRL0 | 0x7000_1400   |   4    |   Regulation registers    |
 * |   VCTRL1 | 0x7000_1800   |   4    |   Regulation registers    |
 * |   PID0   | 0x7000_1C00   |   4    |   Regulation registers    |
 * |   PID1   | 0x7000_2000   |   4    |   Regulation registers    |
 * |   ISP0   | 0x7000_2400   |   4    |   Regulation registers    |
 * |   ISP1   | 0x7000_2800   |   4    |   Regulation registers    |
 * |   PWM    | 0x7000_2C00   |   4    |   Regulation registers    |
 * |   COMM   | 0x7000_3000   |   4    |   Regulation registers    |
 * |   TELEM0 | 0x7000_3400   |   4    |   Regulation registers    |
 * |   TELEM1 | 0x7000_3800   |   4    |   Regulation registers    |
 * |   FAULT0 | 0x7000_3C00   |   4    |   Regulation registers    |
 * |   FAULT1 | 0x7000_4000   |   4    |   Regulation registers    |
 * |   FAN1   | 0x7000_4400   |   4    |   Regulation registers    |
 * |   FAN2   | 0x7000_4800   |   4    |   Regulation registers    |
 * |   TS     | 0x7000_4C00   |   4    |   Regulation registers    |
 * |   TLMCOM | 0x7000_5000   |   4    |   Regulation registers    |
 * | FAULTCOM | 0x7000_5400   |   4    |   Regulation registers    |
 * |   BIFP   | 0x7008_0000   |   8    |   Peripheral registers    |

 *
 *  @section sctShasta_mmu_drv_init   MMU Initialization
 *  During initialization the protection is disabled and the mapping is set to default.
 *  <br/> The behavior after initialization is the same as with mapping disabled.
 *
 *
 * @section sctShasta_mmu_drv_example   Example application
 *
 * The example shows the protection and mapping feature of the MMU.
\code

// function to read and write from memory
#define MMIO32(x)   (*(volatile uint32_t *)(x))

STATIC CONST DRIVER_MMU *drv_mmu;

void test_execute(void) {
	// Get the API Handler
    drv_mmu = &Driver_MMU;

	// Initialize the mmu with 1:1 mapping
    drv_mmu->Initialize();

	//Protection Test
	// Set protection
    drv_mmu->SetMemProtection(ROM, 63);
	//write to memory
    MMIO32(0x1000FF00)=0x1000;
	//read from memory
    x = MMIO32(0x1000FF00);
    // Clear protection
    drv_mmu->ClearMemProtection(ROM, 63);

    // Map block 0 of OTP (0x10010000) to Block 0 of RAM2 (0x20030000)
    drv_mmu->Configure(OTP, 0, RAM2, 0);
    // Map block 63 of ROM (0x1000FC00) to Block 4 of RAM2 (0x20031000)
    drv_mmu->Configure(ROM, 63, RAM2, 4);
    // Map block 31 of RAM1 (0x2000FC00) to Block 4 of RAM2 (0x20032000)
    drv_mmu->Configure(RAM1, 31, RAM2, 4);

    // Regulation Register protection
    drv_mmu->SetRegProtection(BIFR);
    // Un-protects Regulation Register
    drv_mmu->ClearRegProtection(BIFR);

    // Peripheral Register protection
    drv_mmu->SetRegProtection(BIFP);
    // Un-protects Peripheral Register
    drv_mmu->ClearRegProtection(BIFP);

    // CPU Register protection
    drv_mmu->SetRegProtection(CPUP);
    // Un-protects CPU Register
    drv_mmu->ClearRegProtection(CPUP);

\endcode

See
- mmu_drv.h for details on the API
- CPUS ITS for details on the HW Interface
    https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/03_System/CPUS.pdf
 */

#endif /* DTIMER_DRV_DOXY_H */
