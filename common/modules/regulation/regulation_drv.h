/* ============================================================================
** Copyright (C) 2015 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/*
 * regulation_drv.h
 *
 *  Created on: Oct 31, 2016
 *      Author: dlewis4
 */

#ifndef REGULATION_DRV_H_
#define REGULATION_DRV_H_

#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
/* Data types */
#include <stddef.h>             // NULL pointer for public structs
#include "driver_common.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "dtimer_drv.h"
#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "telemetry_irq_handlers.h"  // Telemetry_IRQ_TYPE_t
#include "shasta_hal_pwm.h"
extern osThreadId tid_Regulation_Thread;


typedef enum REGULATION_SRFET_e
{
	REGULATION_SRFET_OFF = 0u,	//!< Disable SRFET's for given loop (primary FET's active)
	REGULATION_SRFET_ON = 1u,	//!< Enable SRFET's for given loop (primary FET's active)
	REGULATION_PWMS_OFF = 2u	//!< Disable all FET's
} REGULATION_SRFET;

typedef enum REGULATION_STATE_e
{
	REGULATION_STATE_OFF,				//!< OFF state.  Controller is not regulating.  Most blocks are powered down for power savings.
	REGULATION_STATE_TON_DELAY,			//!< Execution of ton_delay timer and waiting until timer has expired.  Some blocks like telemetry enabled for filter settle time.
	REGULATION_STATE_TON_RISE,			//!< Prebias voltage loading, vid setting, ramp time determination, most blocks enabled.
	REGULATION_STATE_AT_TARGET_VID,		//!< Voltage programmed has been reached.  Enable SRFET if above threshold.  Maintain target voltage.
	REGULATION_STATE_TOFF_DELAY,		//!< Execution of toff_delay timer and waiting until timer has expired.  Regulation is still active at target.
	REGULATION_STATE_TOFF_FALL,			//!< Program vid to 0, calculate ramp settings for soft off if required.  Blocks disabled for power savings.
	REGULATION_STATE_FAULT				//!< Fault state used for any type of regulation/pmbus fault detected that has non-ignore response.  Handles retry, delay, hiccups.
} REGULATION_STATE;

typedef enum REGULATION_CONTROLS_e
{
	REGULATION_CONTROLS_NOP,					//!< No operation control.  State machine possibly moves state based on next state but does not execute a control sequence.
	REGULATION_CONTROLS_SHUTDOWN_IMMEDIATE,		//!< Hard shutdown control. From fault responses or pmbus enable state depending on configuration.
	REGULATION_CONTROLS_SHUTDOWN,				//!< Soft shutdown control.  From pmbus enable state.
	REGULATION_CONTROLS_FAULT,					//!< Fault control that requires regulation state machine to act.  Driven by fault ISR
	REGULATION_CONTROLS_TELEMETRY_IRQ,			//!< Generic telemetry irq indication of threshold exceeded of various fast telemetry.
	REGULATION_CONTROLS_FAULT_TIMER,			//!< Fault hiccup or delay timer has expired. Driven by dtimer ISR
	REGULATION_CONTROLS_TON_TOFF_DELAY_TIMER,	//!< TON_DELAY or TOFF_DELAY timer has expired.  Driven by dtimer ISR
	REGULATION_CONTROLS_TON_TOFF_MAX_TIMER,		//!< TON_MAX or TOFF_MAX timer has expired.  Driven by dtimer ISR
	REGULATION_CONTROLS_OPERATION_ENABLE,		//!< Enable state change from pmbus detected.  Can be from operation, on_off_config, enable pins, vin_on/off
	REGULATION_CONTROLS_ENABLE,					//!< Enable state change from regulation state machine itself.  Used by faults for retries
	REGULATION_CONTROLS_VID_CHANGE,				//!< Vid change from pmbus thread detected
	REGULATION_CONTROLS_VID_REACHED,			//!< Vid target reached by vcontrol hardware block, driven in ISR
	REGULATION_CONTROLS_TELEMETRY_UPDATED		//!< Used by telemetry thread to indicate to regulation state machine new telemetry data is available
} REGULATION_CONTROLS;

/*
 * This is used to decode the fields in FW_CONFIG_REGULATION PMBUS command:
 */
typedef enum REGULATION_CONFIG_e
{
	REGULATION_CONFIG_EN_DE_STARTUP = 0u,  				//!< Diode emulation (SR's off) on startup ramp
	REGULATION_CONFIG_EN_PRIM_ISENSE = 1u,				//!< Enable primary current sensing (Peak Current Mode Control)
	REGULATION_CONFIG_EN_PID_ADJ = 2u,  				//!< Margined startup (pid adjustments)
	REGULATION_CONFIG_EN_ILIM_STARTUP = 3u,  			//!< Current Limited Startup
	REGULATION_CONFIG_EN_C_ESTIMATE = 4u,				//!< Cout estimation, Iout leakage estimation
	REGULATION_CONFIG_EN_L_ESTIMATE = 5u,   			//!< Lout estimation
	REGULATION_CONFIG_EN_VSEN_OPEN_PROTECT = 6u,		//!< Remote sense detect
	REGULATION_CONFIG_EN_DEADTIME_ADJ = 7u,  			//!< Deadtime adjustment algorithm.  Adjust pwm deadtime at startup.
	REGULATION_CONFIG_INTERLEAVE_ENABLE = 8u,  			//!< Ramp to fixed duty cycle for open loop testing (done in HW completely need I2C to force it, testmode only)
	REGULATION_CONFIG_EN_IOUT_APC_TEMP_COMP = 9u,  		//!< Current sense gain adjustment based on temperature
	REGULATION_CONFIG_EN_DE_SHUTDOWN = 10u,  			//!< Diode emulation (SR's off) on shutdown ramp
	REGULATION_CONFIG_EN_BOOST_FEED_FORWARD = 11u,  			//!< Sync FET ramp
	REGULATION_CONFIG_CURRENT_DOUBLER_ENABLE = 12u, 	//!< Enable current doubling mode (2 inductors)
	REGULATION_CONFIG_TOPOLOGY = 13u,  					//!< 3 bits  see REGULATION_TOPOLOGY type
	REGULATION_CONFIG_CURRENT_LIMIT_AT_STARTUP = 16u,	//!< 8bits  (inrush current limit u8.0)
	REGULATION_CONFIG_VOUT_TARGET_WINDOW = 24u,  		//!< 8 bits  (vout target window used to enable sr fets vout_mode resolution limited to 8 bits)
	REGULATION_CONFIG_MFR_RDROOP_RLL_SEG2 = 32u,		//!< linear 11 format loadline droop for second segment of 3 segement piecewise linear curve
	REGULATION_CONFIG_MFR_RDROOP_RLL_SEG3 = 48u,		//!< linear 11 format loadline droop for third segment of 3 segement piecewise linear curve
	REGULATION_CONFIG_MFR_RDROOP_RLL_NEG = 64u,			//!< linear 11 format loadline droop for neg segment of 3 segement piecewise linear curve
	REGULATION_CONFIG_MFR_RDROOP_ITHR_SEG2 = 80u,		//!< linear 11 format loadline droop current threshold for second segment to kick in
	REGULATION_CONFIG_MFR_RDROOP_ITHR_SEG3 = 96u		//!< linear 11 format loadline droop current threshold for third segment to kick in
} REGULATION_CONFIG;

typedef enum REGULATION_TOPOLOGY_e
{
	REGULATION_TOPOLOGY_ACF = 0u,	//!< Active clamp forward topology
	REGULATION_TOPOLOGY_PS_CT = 1u,	//!< Phase shifted center tap topology
	REGULATION_TOPOLOGY_HB_CT = 2u,	//!< Half bridge center tap topology
	REGULATION_TOPOLOGY_HB_FW = 3u,	//!< Half bridge full wave topology
	REGULATION_TOPOLOGY_PS_FW = 4u,	//!< Phase shifted full wave topology
	REGULATION_TOPOLOGY_FB_CT = 5u,	//!< Full bridge center tap topology
	REGULATION_TOPOLOGY_FB_FW = 6u,	//!< Full bridge full wave topology
	REGULATION_TOPOLOGY_BUCK = 7u	//!< Buck topology
} REGULATION_TOPOLOGY;

typedef struct {
  PMBUS_PAGE_t loop;				//!< loop to select for message
  REGULATION_CONTROLS control;  	//!< using REGULATION_CONTROLS enumerations for definitions
  uint16_t control_data;			//!< fault case its reused for fault response
} regulation_inter_thread_message_t;

typedef struct {
	PMBUS_PAGE_t loop;							//!< current active loop executing a message
	REGULATION_STATE current_state[NUM_LOOPS];	//!< current state of each loop
	REGULATION_STATE next_state[NUM_LOOPS];		//!< next state of each loop
	REGULATION_STATE previous_state[NUM_LOOPS];	//!< prior state of each loop
	REGULATION_CONTROLS control[NUM_LOOPS];		//!< current control of each loop
    uint16_t control_data[NUM_LOOPS];			//!< current control data of each loop
    uint32_t ilim_current_stepsize[NUM_LOOPS];	//!< current limiting stepsize in apc format
    uint32_t ilim_current_otp_value[NUM_LOOPS];	//!< current limiting otp value to be applied at vid state
    uint32_t verr_entry_thresh[NUM_LOOPS];		//!< voltage error threshold of ata from otp config to be applied at vid state
    uint32_t ovs_entry_thresh[NUM_LOOPS];		//!< voltage error threshold of ata from otp config to be applied at vid state
    uint32_t rear;							//!< local queue write pointer, ie incoming messages to regulation sm
    uint32_t front;							//!< local queue read pointer, executed by regulation sm
    uint8_t mbx_size;						//!< local queue size/count of messages to execute
    uint32_t mbx_cnt;						//!< rtos queue size/count of messages to execute
    uint32_t config;
    uint32_t prebias_is_greater_than_vid[NUM_LOOPS];
    // boost topology feed forward scale factors
    // set by pmbus commands but need fast local access for BW
    uint32_t vdd_scale_factor[NUM_LOOPS]; // U7.7 (92.3)
    uint32_t vin_scale_factor[NUM_LOOPS]; // U7.7 (59.0)
    uint32_t vrect_scale_factor[NUM_LOOPS]; // 18.75 in U5.2
    uint32_t vout_scale_factor[NUM_LOOPS]; // U0.13 (256/VRECT_SCALE/800)
    uint8_t OSP_Fault_Detection_done[NUM_LOOPS];
} REGULATION_MODULE_t;

extern REGULATION_MODULE_t REGULATION_APP;

/* Declarations */
void REGULATION_INTER_THREAD_SET(PMBUS_PAGE_t loop, REGULATION_CONTROLS control, uint16_t control_data);
uint16_t Regulation_Get_Vid(PMBUS_PAGE_t loop);
void Regulation_Set_Vout(PMBUS_PAGE_t loop, uint32_t vout, uint32_t ramp_rate_mv_per_ms, boolean immediate, boolean trim);
uint32_t Regulation_Get_Vout_Transition_Rate(PMBUS_PAGE_t loop);
uint32_t Regulation_Get_Ramp_Rate(PMBUS_PAGE_t loop, uint32_t vout_target, uint32_t prebias_voltage, uint8_t cmd);
void Regulation_Update_Vid(PMBUS_PAGE_t loop, REGULATION_STATE state);
void Regulation_Generic_Telemetry_IRQ_VOUT_UV_DISABLE_TON_MAX_HANDLE(Telemetry_IRQ_TYPE_t Telemetry_type);
void Regulation_Power_Mode (PMBUS_PAGE_t loop, REGULATION_STATE state);
void Regulation_Fault_Mask (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
void Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
void Regulation_Drv_Init(void);
void Regulation_Initialize (void);
void Regulation_Uninitialize (void);
uint16_t Regulation_Get_PMBUS_Configuration(PMBUS_PAGE_t loop, REGULATION_CONFIG config);
void Regulation_Current_Limited_Startup_Fsw_IRQHandler(PMBUS_PAGE_t loop);
void Regulation_Compute_Feed_Forward (PMBUS_PAGE_t loop);
void iout_temperature_compensation(PMBUS_PAGE_t loop);

#define REGULATION_CALLBACK_ARRAY_SIZE (128)
/* Callback type for regulation IRQ event */
typedef void (* regulation_sm_callback)(PMBUS_PAGE_t loop);
/* Set callback function to regulation IRQ event */
void Regulation_set_regulation_event_cb(REGULATION_STATE state, REGULATION_CONTROLS control, regulation_sm_callback regulation_event_cb);
/* Call callback function for regulation IRQ event */
void Regulation_call_regulation_event_cb(REGULATION_STATE state, REGULATION_CONTROLS control, PMBUS_PAGE_t loop);
void Regulation_smooth_transition_feedforward (boolean enable_disable);
void Regulation_disable_ata(PMBUS_PAGE_t loop);
void Regulation_OSP_Fault_Detection_sequence(PMBUS_PAGE_t loop);

/**
 * Regulation_PWM_Control(PMBUS_PAGE_t loop, REGULATION_SRFET on_off)
 * on_off:
 * REGULATION_SRFET_OFF = disable SR FETS for a given loop
 * REGULATION_SRFET_ON = enable SR FETS for a give loop
 * REGULATION_PWMS_OFF = disable all FETS for a given loop
 * @param loop Page to operate on.
 * @param on_off @ref REGULATION_SRFET type on off control type for pwm's.  Selects primary and secondary fet on/off controls
 *
 */
__DRIVER_COMMON_INLINE void Regulation_PWM_Control(PMBUS_PAGE_t loop, REGULATION_SRFET on_off)
{
	uint32_t config;
	uint16_t srfet_mask;
	uint16_t fet_mask;

	config = PMBUS_Get_Command_Dword(loop, PMBUS_CMDCODE_FW_CONFIG_PWM);
	fet_mask = config & 0x0fff;
	srfet_mask = (config & 0x0fff0000)>>16;

	switch (on_off)
	{
		case REGULATION_SRFET_OFF:
			PWM_PWM_ON__SET(((PWM_PWM_ON__GET()) & (~srfet_mask) ) | fet_mask);  // zero out sr fets and or in q fets
			break;
		case REGULATION_SRFET_ON:
			PWM_PWM_ON__SET((PWM_PWM_ON__GET()) | fet_mask | srfet_mask);  // primary side PWM's on, SR's off (reset setting)
			break;
		case REGULATION_PWMS_OFF:
			PWM_PWM_ON__SET((PWM_PWM_ON__GET()) & (~srfet_mask) & (~fet_mask) );  // zero out all fets associated with the loop
			break;
		default:
			PMBUS_Log_Error(REGULATION, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
			break;
	}
}
#endif /* REGULATION_DRV_H_ */
