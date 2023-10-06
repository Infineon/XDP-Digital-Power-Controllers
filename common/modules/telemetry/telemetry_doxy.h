/* ============================================================================
** Copyright (C) 2014 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorization.
** ============================================================================
*/
/**
 * @file    TELEMETRY_doxy.h
 * @brief   Doxygen overview of TELEMETRY module.
 */


#ifndef TELEMETRY_DOXY_H_
#define TELEMETRY_DOXY_H_

 /** @page pgShasta_TELEMETRY   Telemetry FW Module
*
* @section sctShasta_TELEMETRY_Intro   Introduction
*
* TELEMETRY driver handles version 1.0.
* It auto-generates its command support tables from:
* https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/00_Requirements/TELEMETRY/Shasta_TELEMETRY.xlsx
*
* The Telemetry firmware module is a CMSIS style driver.  See @ref ARM_DRIVER_TELEM for driver struct.
*
* Terminology Assumptions:
* API -  Application layer interface used by user for access to firmware module.
* Driver - Shortened to drv in file naming.  Tends to wrap the hardware abstraction layer for accessing a particular hardware block.
*
* @section sctShasta_Telemetry_IRQ   Telemetry Generic IRQ module
* Telemetry has 16 generic irq types @ref Telemetry_IRQ_TYPE_t with selectable sources @ref Telemetry_irq_src_t. Thresholds are set by @ ref Telemetry_Set_Irq_Threshold API
* Source, loop and handle for each @ref Telemetry_IRQ_TYPE_t is configured by @ref TELEMETRY_SETUP_IRQ.
* Each IRQ has a firmware hysteresis set by @ ref Telemetry_Set_Irq_Hysteresis_Threshold API
*
* When the selected source signal exceeds the threshold @ref Telemetry_IRQ_TYPE_t bit is set in tlm_irq_stat (if enabled by tlm_irq_en). Telemetry uses level type comparator so
* to ensure the tlm_irq_stat is set only once tlm_irq_polarity is used. tlm_irq_polarity is controlled by telemetry driver. The value of 0 assumes above the threshold
* polarity of comparator and the value of 1 - below the threshold comparator polarity.  Telemetry driver clears the status tlm_irq_stat by setting tlm_irq_en to 0, flips the polarity, changes the
* threshold to hysteresis threshold and calls the handle that was set by @ref TELEMETRY_SETUP_IRQ.
*
* @image html telemetry_hw_IRQ.png "TELEMETRY_IRQ Hardware Module Diagram"
* @image latex telemetry_hw_IRQ.png "TELEMETRY_IRQ Hardware Module Diagram"
* @image html Telemetry_irq_driver.png "TELEMETRY_IRQ Driver Diagram"
* @image latex Telemetry_irq_driver.png "TELEMETRY_IRQ Driver Diagram"
*
* @section sctShasta_Telemetry_THREAD   Telemetry Thread
*
* List of telemetry inputs:
* vout_fs_i
* ispx_iout_i
* temp_atsen_i
* temp_btsen_i
* duty_fs_i
* vin_tsp_i
* vrectx_fs_i
* ispx_iin_i
* iin_est_o
*
* Each telemetry has a low pass filter with configurable bandwidth and a hi/low detect.
*
* @image html Telemetry_LPF.png "Telemetry Low Pass Filter High/Low Diagram"
* @image latex Telemetry_LPF.png "Telemetry Low Pass Filter High/Low Diagram"
* @ref TELEMETRY_Thread samples the telemetry values for voltage, current and power every 1ms, updates pmbus read telemetry commands and sends the message REGULATION_CONTROLS_TELEMETRY_UPDATED  @ref REGULATION_CONTROLS to regulation state machine . The thread @ref REGULATION_Thread
* has a 16 deep message queue that stacks up with @ref regulation_inter_thread_message_t type messages.
*
 */
#endif /* TELEMETRY_DOXY_H_ */
