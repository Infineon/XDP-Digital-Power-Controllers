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
 * @file    otp_doxy.h
 * @brief   Doxygen overview of the OTP FW Module.
 */

#ifndef OTP_DOXY_H
#define OTP_DOXY_H

/** @page pgShasta_Otp   OTP Driver Module
 *
 * @section sctShasta_Otp_Intro Introduction
 *
 * This section deals with OTP drivers and the otp hardware access layer(HAL). OTP stands for
 * "One time programmable" memory. As its name implies, the OTP device can only be written
 * once. Its physical characteristics is similar to flash memory except former can only be written once
 * while latter can be written multiple times.<br>
 * In Shasta, there is one OTP, with up to four partitions. OTP driver
 * and the HAL handle both read/write access for all partitions. The first partition is used to store
 * FW, Trim, Register and PMBus configuration data. See @ref pgShasta_Configurator.<br>
 * The other partitions are used FW patches. See @ref pgShasta_Patch.
 * <br>
 * The OTP device is organized as 4 32-bit words or 16 8-bit bytes or 128 bit per memory location named "otp row".
 * For a virgin device, even rows contain all 0s while odd rows contain all 1s when row is counting from 0.
 * FW needs not care about this bit flips it just sees a virgin device as all 0s.
 *
 * @par OTP Read
 * The OTP Wrapper is build such that data can be read with a direct access to AHB.
 *
 * @par OTP Write
 * Writing and programming means exactly the same in this write up and is used interchangeably throughout this document.
 * Writing operates on two different modes, the poll and interrupt modes. In Shasta FW,
 * interrupt mode is used. Poll mode is used for building a Lauterbach mem_loader driver and will
 * not discussed here. Under interrupt mode, RTX message is used to wait for the OTP "write completion"
 * message being sent from the interrupt service handler. Caller write one chunk of data, then wait for "write completion" message.
 * Upon getting that message, issue another chunk of data to write. Please read @ref sctShasta_Otp_Write for steps
 * of OTP writing.

 * See
 * - otp_drv.h for details on the API
 * - otp_fs.h for details on the file system API
 * - CPUS ITS for details on the HW Interface
 * https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/03_System/CPUS.pdf
 *
 * @section sctShasta_otp_driver_init OTP Driver Initialization
 *
 * This functions enables clk, disables reset and power the OTP which takes roughly 400ms. Therefore the OTP
 * keeps powered for the whole activation of the system. Only in sleep mode it will be switched off if needed.
 * Since the OTP driver works closely with the RTX,
 * the RTOS's message queue and message box
 * should be initialized here. It also enables OTP interrupt services. Since writing
 * to OTP takes a long time, interrupt service is used for OTP write.
 *
 * @section sctShasta_Otp_Write OTP Data Write
 * This is the routine to write data into the otp memory.
 * The mechanic of OTP writing is as follow:
 *  - Caller provides a 4 32-bit word chunk.
 *  - OTP driver loads up the "cache line"
 *  - Set up the address register that the data will be written to
 *  - Prepare the interrupt mask
 *  - Issue an otp write instruction
 *  - Wait for OTP "write completion" message from the OTP interrupt service handler
 *
 * @section sctShasta_Otp_read OTP Data Read
 * This function just returns a pointer to the data which can be read directly via the AHB Interface.
 *
 * @section sctShasta_Otp_file_structure OTP File Structure
 * The OTP is organized as a file structure, @ref Otp_File_System_Cmd_Types_t.
 *
 */


#endif /* OTP_DOXY_H */
