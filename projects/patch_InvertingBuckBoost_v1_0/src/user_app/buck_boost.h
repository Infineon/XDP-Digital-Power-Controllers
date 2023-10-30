/* ============================================================================
** Copyright (C) 2020 Infineon. All rights reserved.
**               Infineon Technologies, PSS SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorisation.
** ============================================================================
**
** Created on:  2020-08-19
*
 *      Author: kotvytsr
 */

#ifndef SRC_USER_APP_BUCK_BOOST_H_
#define SRC_USER_APP_BUCK_BOOST_H_

/*
 * All buck boost features can be enabled from this file.
 * All feature are listed below.
 * Un/comment feature out to dis/enable it.
 *
 * Take a note: the body box
 * 		//================================
 * 			some settings = some value;
 * 		//================================
 * shows that user can modify the code/settings inside
 */


//#define ishare_feature_en			// current sharing feature

#define eff_table_feature_en		// refer to efficiency_table.h for efficiency threshold settings
                                    // refer to "add_on_features.c / add_on_features_init() to set efficiency hysteresis ranges

/*
 * iout_correction_feature_en needs eff_table_feature_en to work correctly!
 */
#define iout_correction_feature_en

#define feed_forward_feature_en
#define deadtime_feature_en
//#define vin_trim_feature_en

/*
 * zvs_feature_en and light_load_features_en need iout_correction_feature_en to work correctly!
 */
#define zvs_feature_en				// refer to "add_on_features.c / add_on_features_init() to set your hysteresis range
#define light_load_features_en		// phase shedding or diode emulation or burst modes at light load

//#define fan_en					// FAN control



#ifdef light_load_features_en

typedef enum light_load_features_mode_e
{
	NO_MODE = 0u,
	PHASE_SHEDDING = 1u,
	DIODE_EMULATION = 2u,
	BURST = 3u,
} light_load_features_mode_t;

#endif

#include "stdint.h"


#include "telemetry_api.h"

#ifdef eff_table_feature_en
	uint8_t calculate_i(uint32_t InputVoltage);
	uint8_t calculate_j(int32_t OutputCurrent);
	uint32_t input_current_correction(int8_t vin_read_exp, int32_t OutputCurrent, int8_t iout_read_exp, int8_t iin_read_exp, int8_t power_read_exp, int8_t loop, uint32_t vin, uint32_t pout);
	void patch_Telemetry_Sample(void);
#endif

#ifdef iout_correction_feature_en
	int32_t patch_Telemetry_get(PMBUS_PAGE_t loop, TELEMETRY_TYPE_t telem_type);
#endif

#if defined (feed_forward_feature_en) || defined(zvs_feature_en) || defined(light_load_features_en)
	void patch_Regulation_Compute_Feed_Forward(PMBUS_PAGE_t loop);
#endif

typedef enum mode_e
{
	OFF = 0u,
	ON = 1u,
} mode_t;

void iout_telemetry_get_high_frequence_limited(PMBUS_PAGE_t loop);
void light_load_features_core_function(PMBUS_PAGE_t loop);
int32_t iout_telemetry_get_high_frequence(PMBUS_PAGE_t loop);

#endif /* SRC_USER_APP_BUCK_BOOST_H_ */
