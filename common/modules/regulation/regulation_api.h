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
 * regulation_api.h
 *
 *  Created on: Oct 31, 2016
 *      Author: dlewis4
 */

#ifndef REGULATION_API_H_
#define REGULATION_API_H_

/* Standard library */
#include <stdint.h>              /* uint*_t */
#include "fw_bool.h"             /* boolean type */
#include "faults_api.h"
#include "pmbus_autogen.h"      // PMBUS_PAGE_t

/*
 * Hardware enumerated resolutions for registers
 */
/** @todo we should move these to hal level and make enumerations in the particular registers for these **/
#define CLAMP_LSB_MVOLTS (50)
#define LOADLINE_EXP (-7)
#define SWITCHING_PERIOD_LSB_NS (20)
#define PW_MIN_LSB_NS (5)
#define DAC_STEP_SIZE_VOLTS_TO_0_156MV (6400)  // 1000mv/1V / 1.25mv / 8
#define DAC_INTERNAL_PRECISION_0_156MV (-13)  // this is the number of bits to capture DAC_STEP_SIZE_VOLTS_TO_0_156MV
#define DAC_STEP_SIZE_VOLTS_TO_1_25MV (800)  // 1000mv/1V / 1.25mv
#define DAC_INTERNAL_PRECISION_1_25MV (-10)  // this is the number of bits to capture DAC_STEP_SIZE_VOLTS_TO_1_25MV
#define DAC_INTERNAL_CLAMP_MAX_0_156MV (0x7FFF)
#define DAC_INTERNAL_CLAMP_MAX_1_25MV (0x3FF)
#define VC_VCONTROL_DELTA_CLAMP_MAX (4095)  // max value of VC_VCONTROL_DELTA register
#define VC_VCONTROL_DELTA_CLAMP_MIN (-4096) // min value of VC_VCONTROL_DELTA register
#define IOUT_CAL_OFFSET_EXPONENT (-3)
#define RAMP_DC_MAX_EXPONENT (-1)
#define ISEN_APC_EXPONENT (-9)
#define NUMBER_OF_PWMS (12)

// API:
void Regulation_Set_VID(PMBUS_PAGE_t loop, uint32_t voltage_mantissa, uint32_t vout_scale, int32_t vout_exp, uint32_t window, boolean immediate);
void Regulation_Voltage_Ramp_Rate_Set(PMBUS_PAGE_t loop, uint32_t mv_per_ms);
void Regulation_Switching_Frequency_Set(PMBUS_PAGE_t loop, uint16_t freq_khz);
void Regulation_PWM_Deadtime_Set(uint8_t phase, uint8_t rising_edge_delay, uint8_t falling_edge_delay);


typedef enum _fsw_irq_idx_e
{
    fsw_irq_idx_0 = 0u,    //!< Index 0
    fsw_irq_idx_1 = 1u,    //!< Index 1
    fsw_irq_idx_2 = 2u,    //!< Index 2
    fsw_irq_idx_3 = 3u,    //!< Index 3
    fsw_irq_idx_4 = 4u,    //!< Index 4
    fsw_irq_idx_5 = 5u,    //!< Index 5
    fsw_irq_idx_6 = 6u,    //!< Index 6
    fsw_irq_idx_7 = 7u,    //!< Index 7
	fsw_irq_idx_size
} fsw_irq_idx_t;

typedef enum _fsw_irq_rate_sel_e
{
    fsw_irq_rate_sel_disabled = 0u,    	//!< Index 0
    fsw_irq_rate_sel_1 = 1u,    		//!< Index 1
    fsw_irq_rate_sel_2 = 2u,    		//!< Index 2
    fsw_irq_rate_sel_4 = 3u,    		//!< Index 3
    fsw_irq_rate_sel_8 = 4u,    		//!< Index 4
    fsw_irq_rate_sel_16 = 5u,    		//!< Index 5
    fsw_irq_rate_sel_32 = 6u,    		//!< Index 6
    fsw_irq_rate_sel_64 = 7u,    		//!< Index 7
} fsw_irq_rate_sel_t;

/* API's for regulation switching frequency sync'ed interrupts */
typedef void (* regulation_fsw_irq_callback)(PMBUS_PAGE_t loop);
extern regulation_fsw_irq_callback ptr_fsw_irq_callback[fsw_irq_idx_size];
/* Set callback function to fsw IRQ event */
void Regulation_set_fsw_irq_event_cb(fsw_irq_idx_t fsw_irq_index, regulation_fsw_irq_callback regulation_fsw_irq_cb);
/* Call callback function for fsw IRQ event */
void Regulation_call_fsw_irq_event_cb(fsw_irq_idx_t fsw_irq_index, PMBUS_PAGE_t loop);
void Regulation_setup_fsw_irq(PMBUS_PAGE_t loop, fsw_irq_idx_t fsw_irq_index, fsw_irq_rate_sel_t  fsw_irq_rate_sel);
void Regulation_fsw_irq_disable(void);
void Regulation_fsw_irq_enable(void);

#endif /* REGULATION_API_H_ */
