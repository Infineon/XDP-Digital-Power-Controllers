/* ============================================================================
** Copyright (C) 2017 Infineon Technologies AG
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
 * telemetry_api.h
 *
 *  Created on: Mar 16, 2017
 *      Author: agrishi1
 */

#ifndef COMMON_MODULES_TELEMETRY_TELEMETRY_API_H_
#define COMMON_MODULES_TELEMETRY_TELEMETRY_API_H_

#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
/* Data types */
#include <stddef.h>             // NULL pointer for public structs
#include <stdint.h>              /* uint*_t */
#include "driver_common.h"
#include "log_app.h"            // Logging of errors
#include "shasta_hal_telem_common.h"
#include "telemetry_drv.h"
#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "shasta_hal_fan.h"

#define Telemetry_VOUT_EXP 0   //Telemetry_TLM_VOUT_FSW   u12.3, but we scale the -3 later by DAC_STEP_SIZE_VOLTS_TO_MV
#define Telemetry_IOUT_EXP -4   //Telemetry_TLM_IOUT_FSW   s9.4
#define Telemetry_VIN_EXP  -4   //Telemetry_TLM_VIN_LPF    u7.4
#define Telemetry_IIN_EXP  -7   //Telemetry_TLM_IIN_LPF    u6.7
#define Telemetry_TEMP_EXP  0   //TLMCOM_TLM_xTSEN_LPF u10.0
#define Telemetry_DUTY_EXP -16  //Telemetry_TLM_DUTY_FSW   u0.16
#define Telemetry_FSW_EXP  0   //Telemetry_TLM_FREQ       u11.0
#define Telemetry_GAIN_EXP -9  // gain is in s1.6

typedef struct {
	int8_t vout[NUM_LOOPS];
	int8_t vin[NUM_LOOPS];
	int8_t iout[NUM_LOOPS];
	int8_t iin[NUM_LOOPS];
	int8_t temp[NUM_LOOPS];
	int8_t duty[NUM_LOOPS];
	int8_t power[NUM_LOOPS];
	int8_t fan[NUM_LOOPS];
} _TELEM_READ_EXP_s ;

extern _TELEM_READ_EXP_s _TELEM_READ_EXP;

typedef enum TELEMETRY_VIN_SOURCE_e
{
	TELEMETRY_VIN_SOURCE_VRECT1,
	TELEMETRY_VIN_SOURCE_VRECT2,
	TELEMETRY_VIN_SOURCE_LOOP0_VOUT,
	TELEMETRY_VIN_SOURCE_VIN_TSP,
	TELEMETRY_VIN_SOURCE_VIN_FORCE,
	TELEMETRY_VIN_SOURCE_VRECT1_VRS_INIT_ZERO,
	TELEMETRY_VIN_SOURCE_VRECT1_VIN_PRISEN,
	TELEMETRY_VIN_SOURCE_VRECT2_VIN_PRISEN
} TELEMETRY_VIN_SOURCE;

typedef enum TELEMETRY_TYPE_e
{
	VOUT_TELEM,
	IOUT_TELEM,
	VIN_TELEM,
	IIN_TELEM,
	ATSEN_TELEM,
	BTSEN_TELEM,
	ITSEN_TELEM,
	DUTY_TELEM,
	FSW_TELEM,
	XADDR1_TELEM,
	XADDR2_TELEM
} TELEMETRY_TYPE_t;

/*
 * This is used to decode the fields in FW_CONFIG_REGULATION PMBUS command:
 */
typedef enum TELEMETRY_CONFIG_e
{
	TELEMETRY_CONFIG_READ_POWER_EXP = 0u,
	TELEMETRY_CONFIG_FW_FILTER_EN = 3u,
	TELEMETRY_CONFIG_VOUT_GAIN = 4u,
	TELEMETRY_CONFIG_IOUT_GAIN = 8u,
	TELEMETRY_CONFIG_VIN_GAIN = 12u,
	TELEMETRY_CONFIG_IIN_GAIN = 16u,
	TELEMETRY_CONFIG_TEMPA_GAIN = 20u,
	TELEMETRY_CONFIG_TEMPB_GAIN = 24u,
	TELEMETRY_CONFIG_TEMPI_GAIN = 28u,
	TELEMETRY_CONFIG_VOUT_OFFSET = 32u,
	TELEMETRY_CONFIG_IOUT_OFFSET = 40u,
	TELEMETRY_CONFIG_VIN_OFFSET = 48u,
	TELEMETRY_CONFIG_IIN_OFFSET = 56u,
	TELEMETRY_CONFIG_TEMPA_OFFSET = 64u,
	TELEMETRY_CONFIG_TEMPB_OFFSET = 72u,
	TELEMETRY_CONFIG_TEMPI_OFFSET = 80u,
	TELEMETRY_CONFIG_READ_VIN_EXP = 88u,
	TELEMETRY_CONFIG_READ_IOUT_EXP = 92u,
	TELEMETRY_CONFIG_READ_IIN_EXP = 96u,
	TELEMETRY_CONFIG_READ_DUTY_EXP = 100u,
	TELEMETRY_CONFIG_READ_FAN_EXP = 104u,
	TELEMETRY_CONFIG_GAIN_OFFSET_FAULT_THRESH_EN = 108u,
	TELEMETRY_CONFIG_RESERVED1 = 109u,
	TELEMETRY_CONFIG_RESERVED2 = 110u,
	TELEMETRY_CONFIG_RESERVED3 = 111u,
	TELEMETRY_CONFIG_VOUT_GAIN_BASE = 112u,
	TELEMETRY_CONFIG_IOUT_GAIN_BASE = 120u,
	TELEMETRY_CONFIG_VIN_GAIN_BASE = 128u,
	TELEMETRY_CONFIG_IIN_GAIN_BASE = 136u,
	TELEMETRY_CONFIG_TEMPA_GAIN_BASE = 144u,
	TELEMETRY_CONFIG_TEMPB_GAIN_BASE = 152u,
	TELEMETRY_CONFIG_TEMPI_GAIN_BASE = 160u
} TELEMETRY_CONFIG;

/*
 * Enumeration of TELEM IRQ interrupts
 */
typedef enum _Telemetry_irq_src_e
{
    IOUT_0,
	IOUT_1,
    VOUT_0,
    VOUT_1,
    IIN_0,
    IIN_1,
    VIN_0,
    VIN_1,
    DUTY_0,
    DUTY_1,
    FSW_0,
    FSW_1,
    ATSEN,
    BTSEN,
	ITSEN,
    IMON,
	PRISEN,
	XADDR1,
	XADDR2,
	Telemetry_IRQ_SRC__END_OF_Telemetry__SRC_TYPES

} Telemetry_irq_src_t;

/* ============================================================================
 * Public data types
 * ========================================================================= */
typedef struct {
	PMBUS_PAGE_t loop;
	uint8_t polarity;
	uint8_t thresh_count;
	Telemetry_irq_src_t source;
	int32_t mantissa;
	int8_t  exponent;
	int32_t hysteresis_mantissa;
	int8_t  hysteresis_exponent;
} _Telemetry_s;

/*
 * Array for storage of _Telemetry_s type for every telem irq registered in firmware
 */

extern _Telemetry_s Telemetry_ARRAY [Telemetry_IRQ_TYPE__END_OF_Telemetry_IRQ_TYPES];
extern const int16_t * ntc_temp;

/** Callback type for TELEM IRQ event **/
typedef void (* Telemetry_handle)(Telemetry_IRQ_TYPE_t Telemetry_type);

void Telemetry_Disable_IRQ_Type(Telemetry_IRQ_TYPE_t Telemetry_type);
void Telemetry_Enable_IRQ_Type(Telemetry_IRQ_TYPE_t Telemetry_type);

/** Set callback function to TELEM IRQ event **/
void Telemetry_IRQ_Event_Handle_Set(Telemetry_IRQ_TYPE_t Telemetry_type, Telemetry_handle Telemetry_IRQ_HANDLE);

void Telemetry_Set_Irq_Threshold(Telemetry_IRQ_TYPE_t Telemetry_type, int32_t mantissa, int8_t exponent);
void Telemetry_Set_Irq_Hysteresis_Threshold(Telemetry_IRQ_TYPE_t Telemetry_type, int32_t mantissa, int8_t exponent);
void Telemetry_Setup_Irq(PMBUS_PAGE_t page, Telemetry_irq_src_t source, Telemetry_IRQ_TYPE_t Telemetry_type, Telemetry_handle handle);
void Telemetry_Generic_Irq_Threshold_Set(Telemetry_IRQ_TYPE_t index, Telemetry_irq_src_t source, int32_t mantissa, int32_t exponent);
void Telemetry_Set_Irq_Polarity(Telemetry_IRQ_TYPE_t Telemetry_type, uint8_t polarity);

int32_t Telemetry_get(PMBUS_PAGE_t loop, TELEMETRY_TYPE_t telem_type);
uint32_t Telemetry_get_power(uint32_t vout, int8_t vout_exp, int32_t iout, int8_t iout_exp, int8_t pout_exp);

int16_t Telemetry_get_ntc_temp(uint16_t adc_code);
uint16_t Telemetry_get_adc_code(int16_t temp);
int16_t Telemetry_get_tempi(void);
uint16_t Telemetry_get_fan_speed(fan_idx_t fan);
void Telemetry_set_vin_source(PMBUS_PAGE_t loop, TELEMETRY_VIN_SOURCE source);
TELEMETRY_VIN_SOURCE Telemetry_get_vin_source(PMBUS_PAGE_t loop);
void Telemetry_change_scales(PMBUS_PAGE_t loop);
uint8_t Telemetry_Get_PMBUS_Configuration(PMBUS_PAGE_t loop, TELEMETRY_CONFIG config);
uint16_t Telemetry_Get_Vout_Scale(PMBUS_PAGE_t loop);
uint8_t Telemetry_Get_Xvalent_Pinset(boolean xaddr);
//int32_t Telemetry_Low_Pass_Filter(int32_t LPF_Shift, int32_t LPF_input, int32_t LPF_Filt);

/*
 * This function is used to grab the configuration of the firmware module from PMBUS command
 * @param loop specifies which pmbus loop to read the config from
 * @param fault_type is an enum in faults_api.h to define which type of fault to lookup in the array
 * @return pointer to _FAULT_s struct from array
 */
__TLMCOM_INLINE _Telemetry_s * Telemetry_GET_ARRAY_POINTER(Telemetry_IRQ_TYPE_t Telemetry_type)
{
	// if fault type is 0-31 it is a loop fault which index in the array from 0-31 for loop0 and 32-63 for loop1
	if (Telemetry_type < Telemetry_IRQ_TYPE__END_OF_Telemetry_IRQ_TYPES)
	{
		return (&Telemetry_ARRAY[Telemetry_type]);

	}
	else // in the fw faults and hw common cases we are using the fault types above 32 so just add 32 to the type to get an index between 64-95
	{
		LOG_ERROR_TYPE(TELEMETRY, WARNING, ILLEGAL_PARAMETER);
		return NULL;
	}
}

__TLMCOM_INLINE uint32_t* __THRESHOLD__ADDRESS(uint8_t ida)
{
    return (uint32_t*)((uint32_t)__TLMCOM_BASE_PTR() + (uint32_t)(offsetof(TLMCOM_t, TLM_IRQ_THR_0)) + (uint32_t)(sizeof(uint32_t)*(uint32_t)ida));
}

__TLMCOM_INLINE void TLMCOM_THRESHOLD__SET(uint8_t ida, uint32_t value)
{
    HAL_SET32(__THRESHOLD__ADDRESS(ida), value);
}

__TLMCOM_INLINE int32_t Telemetry_Get_Irq_Source_Exponent(Telemetry_irq_src_t source)
{
	int32_t Telemetry_exponent;
	switch (source)
	{
	case VOUT_0:
	case VOUT_1:
		Telemetry_exponent = Telemetry_VOUT_EXP;
		break;
	case IOUT_0:
	case IOUT_1:
		Telemetry_exponent = Telemetry_IOUT_EXP;
		break;
	case VIN_0:
	case VIN_1:
		Telemetry_exponent = Telemetry_VIN_EXP;
		break;
	case IIN_0:
	case IIN_1:
		Telemetry_exponent = Telemetry_IIN_EXP;
		break;
	case ATSEN:
	case BTSEN:
	case ITSEN:
		Telemetry_exponent = Telemetry_TEMP_EXP;
		break;
	case DUTY_0:
	case DUTY_1:
		Telemetry_exponent = Telemetry_DUTY_EXP;
		break;
	case FSW_0:
	case FSW_1:
		Telemetry_exponent = Telemetry_FSW_EXP;
		break;
	default:
		//LOG_ERROR_TYPE(TELEMETRY, WARNING, ILLEGAL_PARAMETER);
		Telemetry_exponent = 0;
	}
	return Telemetry_exponent;
}

#endif /* COMMON_MODULES_TELEMETRY_TELEMETRY_API_H_ */
