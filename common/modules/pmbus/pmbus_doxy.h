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
 * @file    PMBUS_doxy.h
 * @brief   Doxygen overview of PMBUS module.
 */


#ifndef PMBUS_DOXY_H_
#define PMBUS_DOXY_H_

/** @page pgShasta_PMBUS   PMBUS FW Module
*
* @section sctShasta_PMBUS_Intro   Introduction
*
* PMBUS driver handles version 1.3.1 of the PMBUS specification:
* http://www.pmbus.org/Specifications/CurrentSpecifications
* It auto-generates its command support tables from:
* https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/00_Requirements/PMBus/Shasta_PMBus.xlsx
*
* See register map documentation for documented PMBUS commands included in ROM code.
*
* The PMBUS firmware module is a CMSIS style driver, but supports only slave access.  See @ref ARM_DRIVER_PMBUS for driver struct.
*
* Terminology Assumptions:
* Handler - Call back function used to do work on a given PMBUS command.
* API -  Application layer interface used by user for access to firmware module.
* Driver - Shortened to drv in file naming.  Tends to wrap the hardware abstraction layer for accessing a particular hardware block.
*
* @section sctShasta_PMBUS_HWFW   PMBUS HW/FW interface
* The PMBUS HW PHY is assisted by firmware module.  The firmware is expected in general to decode PMBUS protocol while the hardware de-serializes I2C traffic.
* A FIFO depth of 4 interrupts is used by HW block to signal to firmware when work needs to be done allowing firmware to fall behind hardware for upto 4 bytes.
* FW responds when slave address matches programmed address and pushes HW state machine forward when given valid PMBUS protocol.
* The firmware tends to work 1 byte ahead of the incoming PMBUS data to ensure that FW latency does not interfere with bus speed.
* Since PMBUS is a lower priority than FAULTS or WDT interrupts it can still be starved out if enough faults occur.  This is indicated via the busy bit of status
* word pmbus command if this occurs.
* The HW allows a 5us clock stretch in these cases then the packet is dropped (send all 1's/ nack until not busy).
* The PMBUS firmware is interrupt based for getting command data from the bus and putting into PMBUS RAM locations.  Any operation on the command after protocol
* is done in the @ref PMBUS_Thread.  The thread has a 32 deep message queue that stacks up with @ref pmbus_inter_thread_message_t type messages that tell the thread which
* command handles(callback functions from a bus command) to execute.  The thread is always lower priority than the interrupts so incoming commands can modify
* data that thread is executing on.
*
* @image html pmbus_byte_sequences.jpg "General PMBUS HW Flow"
* @image latex pmbus_byte_sequences.png "General PMBUS HW Flow"
*
* @section sctShasta_PMBUS_HWFW1   PMBUS HW/FW interface
* The firmware module is separated into a driver, auto-generated command structure, command handlers, and an API for other firmware blocks to access.
* The diagram below outlines the overview of the layering.  It is recommended that @ref pmbus_mfr_specific_handlers.c object is patched by customer for manufacturer
* specific commands using the @ref pmbus_api.c layer.
* @image html pmbus_fw_module.jpg "General PMBUS Firmware Module Flow"
* @image latex pmbus_fw_module.png "General PMBUS Firmware Module Flow"
*
* @section sctShasta_PMBUS_GPIO   PMBUS GPIO Controls
* The PMBUS firmware module has control of VR_ENABLE, POWER_GOOD#, FAULT, VRHOT, WARN, and TEST_IN_PROGRESS GPIO pins.
* Using FW_CONFIG_PMBUS one can control the mapping and polarity of these pins.
* The GPIO0 bank is associated with loop 0 of the controller and GPIO1 bank associated with the loop 1 of the controller.
* The GPIO mapping is seen in @ref pmbus_gpio_control.h.  The GPIO driver configuration, direction, polarity, and interrupt
* call-backs exist in @ref pmbus_gpio_control.c.  The GPIO banks/pins are multiplexed to pads of the device by the gpio_sel
* field in MP_EN_SEL register.  See HW register map for description.
*
* @section sctXaddrOffsets Using XADDR Pins to Offset PMBUS Address
* Using FW_CONFIG_PMBUS can enable xaddr resistor offsets.  The hardware is capable of both 8 and 16 offset option resistor ranges.
* After the base address is programmed in this register and the resistive offset is enabled the address will be the base +offset.
* This allows the same OTP configuration of multiple parts to coexist in the same system with different slave addresses.  If the
* resistive offset is disabled in this command the field for offset is used instead.  This needs to be programmed differently for
* each part in the same bus.  @ref FW_CONFIG_t xv_en field must also be programmed to allow different OTP images to be loaded based
* on resistor.
*
* @section sctShasta_PMBUS_Packets   PMBUS Packet Types
* Described in these FW/HW flow diagrams is the typical decision making process by the PMBUS interrupt handler for various types of PMBUS packets.
* @subsection sctShasta_PMBUS_Packets_Write_Word Write Word
* @image html write_word.jpg "Write Word/Byte"
* @image latex write_word.png "Write Word/Byte"
* @subsection sctShasta_PMBUS_Packets_Read_Word Read Word
* @image html read_word.jpg "Read Word/Byte"
* @image latex read_word.png "Read Word/Byte"
* @subsection sctShasta_PMBUS_Packets_Block_Write Block Write
* @image html block_write.jpg "Block Write"
* @image latex block_write.png "Block Write"
* @subsection sctShasta_PMBUS_Packets_Block_Read Block Read
* @image html block_read.jpg "Block Read"
* @image latex block_read.png "Block Read"
*
* @section sctShasta_I2C   PMBUS I2C AHB Bridge Interface
* A separate I2C address is supported for AHB access to the internal bus.  This allows for HW registers that are not controlled by PMBUS to be accessed
* by the user.  This bus is subject to MMU write protection, but no read protection.  See the HW memory map for register addressing/offsets.
* @subsection sctShasta_I2C_Read32 Read32
* @image html read32.jpg "I2C AHB Bridge Read"
* @image latex read32.png "I2C AHB Bridge Read"
* @subsection sctShasta_I2C_Write32 Write32
* @image latex write32.png "I2C AHB Bridge Write"
*
*
*
*
 */

#endif /* PMBUS_DOXY_H_ */
