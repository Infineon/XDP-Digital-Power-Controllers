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
 * @file    REGULATION_doxy.h
 * @brief   Doxygen overview of REGULATION module.
 */


#ifndef REGULATION_DOXY_H_
#define REGULATION_DOXY_H_

/** @page pgShasta_Regulation   Regulation FW Module
*
* @section sctShasta_REGULATION_Intro   Introduction
* The regulation module contains a firmware driven state machine.  The states are driven forward using a RTOS messaging system as the controls.
* The following state diagram illustrates the states of regulation.  Most of the startup behavior is controlled in the FW_CONFIG_REGULATION pmbus command.
*
* @image html FW_Regulation_State_Machine.png "FW Regulation State Machine"
* @image latex FW_Regulation_State_Machine.png "FW Regulation State Machine"
* See @ref REGULATION_STATE and @ref REGULATION_CONTROLS.  States can be monitored via @ref REGULATION_MODULE_t @ref REGULATION_APP.  Users can
* add their own startup functionality utilizing triggers from this state machine by registering callback functions.
* See @ref Regulation_set_regulation_event_cb.
*
* @section sctStartup  Startup Behavior
* Firmware is in control of soft startup behavior before it hands off to high speed control loop used for regulation.  The firmware uses
* pmbus command registers for turn on delays and ramp rates and calculates the best rate to program hardware ramp counters to based on
* voltage desired, prebias voltage on output, pmbus scaling and ramp rates desired.
* @image html startup_behavior.PNG "Startup Behavior"
* @image latex startup_behavior.PNG "Startup Behavior"
*
* @section sctFAULT Fault recovery
* The regulation state machine in conjunction with the fault module take actions based on pmbus configurations when faults occur.
* The state machine is required to keep track of the response, hiccup count, and fault maksing required in each state.  An example
* of a fault hiccup is shown below.  The responses can be configured in pmbus *_RESPONSE commands for desired behavior.
* @image html hiccup_recovery.PNG "Fault Hiccup Recovery"
* @image latex hiccup_recovery.PNG "Fault Hiccup Recovery"
*
* @section sctPrebias Prebias Response
* When the output voltage is presented with a prebias voltage at startup the firmware samples the output telemetry and sets the internal VID to the
* measured output voltage to avoid monotonic startup from PID adjustments due to error voltage.  The dotted blue line in the figure below illustrates how
* the ramp will look with a prebias voltage measured.  There is a similar response to prebias above the desired target.
*
* @image html prebias.png "Prebias"
* @image latex prebias.png "Prebias"
*
* @section sctCurrent_limited_startup Current Limited startup
* The inrush current may be uncontrolled depending on the boot voltage and the Ton_rise.  When EN_ILIM_STARTUP (bit3) of FW_CONFIG_REGULATION pmbus command is set
* the firmware will increase the current limit in steps from bits 23:16 CURRENT_LIMIT_AT_STARTUP until the voltage is reached.  This curbs the inrush current.
*
* @image html current_limited_startup.png "Current Limited Startup"
* @image latex current_limited_startup.png "Current Limited Startup"
*
*
* @section sctde_mode Diode emulation on startup
*
* Startup with the sync rectifiers disabled (at least until it hits a minimum voltage)
* may be more robust in presence of faults.  Using bit 0 EN_DE_STARTUP and bit 10 EN_DE_SHUTDOWN allow sync fets to turn off on startup until a voltage below vout target is reached.
* This is referred to as the target window and is programmable in bits 31:24 VOUT_TARGET_WINDOW.
*
*
*
*
*
*
*
*
*
 */

#endif /* REGULATION_DOXY_H_ */
