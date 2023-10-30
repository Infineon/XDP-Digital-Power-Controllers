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
 *  Created on: 28 May 2020
 *      Author: halimand
 */


#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"
#include "dtimer_drv.h"
#include "telemetry_drv.h"
#include "telemetry_api.h"
#include "telemetry_irq_handlers.h"
#include "regulation_api.h"
#include "regulation_drv.h"
//#include "faults_api.h"
//#include "faults_drv.h"
#include "pmbus_api.h"
#include "pmbus_gpio_control.h"
#include "shasta_hal_vsen.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
//#include "shasta_hal_regulation.h"
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_isen.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_tsen.h"

#include "regulation_state_machine_callbacks.h"

#include "vin_trim.h"


// VIN_TRIM Active Mode Example values: B0A0E782
// VIN Slope:
// User wants to use 0.15625 as the VIN slope
// In LINEAR11:	0xB0A0 with exponent of -10
// In register U-2.14: 2560 (0xA00)	--> (0.15625/0.00006103515625) = 2560

// VIN Offset:
// User wants to use -7.875 as the VIN offset
// In LINEAR11: 0xE782 with exponent of -4
// In register S6.4: -126 (0x382) --> (-(7.875/0.0625) = -126) = 0x382

void PMBUS_HANDLE_MFR_VIN_TRIMMING_ACTIVE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef vin_trim
	if(direction == PMBUS_WRITE || direction == OTP_RESTORE)
	{

		uint8_t *pVinTrimArrayPtr = PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_VIN_TRIMMING_ACTIVE);

		uint16_t vin_offset_LINEAR11 	= pVinTrimArrayPtr[1] << 8 | pVinTrimArrayPtr[0];
		uint16_t vin_slope_LINEAR11 	= pVinTrimArrayPtr[3] << 8 | pVinTrimArrayPtr[2];

		int32_t vin_offset_exp = LINEAR11_TO_EXPONENT(vin_offset_LINEAR11);
		int32_t vin_offset_man = LINEAR11_TO_MANTISSA(vin_offset_LINEAR11);
		int32_t vin_offset = SHIFT_EXPONENT(vin_offset_man, vin_offset_exp + 4);


		int32_t vin_slope_exp = LINEAR11_TO_EXPONENT(vin_slope_LINEAR11);
		int32_t vin_slope_man = LINEAR11_TO_MANTISSA(vin_slope_LINEAR11);
		int32_t vin_slope = SHIFT_EXPONENT(vin_slope_man, vin_slope_exp + 14);

		user_data.active_vin_trim_slope = (uint16_t)vin_slope;
		user_data.active_vin_trim_offset = (uint16_t)vin_offset;
	}
	else if (direction == PMBUS_READ){

	}
#endif
}


// VIN_TRIM Standby Mode Example values: B080E762
// VIN Slope:
// User wants to use 0.125 as the VIN slope
// In LINEAR11:	0xB080 with exponent of -10
// In register U-2.14: 2048 (0x800)	--> (0.125/0.00006103515625) = 2048

// VIN Offset:
// User wants to use -9.875 as the VIN offset
// In LINEAR11: 0xE762 with exponent of -4
// In register S6.4: -158 (0x362) --> (-(9.875/0.0625) = -158) = 0x362

void PMBUS_HANDLE_MFR_VIN_TRIMMING_STANDBY(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef vin_trim
	if(direction == PMBUS_WRITE || direction == OTP_RESTORE)
	{
		uint8_t *pVinTrimArrayPtr = PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_VIN_TRIMMING_STANDBY);

		uint16_t vin_offset_LINEAR11 	= pVinTrimArrayPtr[1] << 8 | pVinTrimArrayPtr[0];
		uint16_t vin_slope_LINEAR11 	= pVinTrimArrayPtr[3] << 8 | pVinTrimArrayPtr[2];

		int32_t vin_offset_exp = LINEAR11_TO_EXPONENT(vin_offset_LINEAR11);
		int32_t vin_offset_man = LINEAR11_TO_MANTISSA(vin_offset_LINEAR11);
		int32_t vin_offset = SHIFT_EXPONENT(vin_offset_man, vin_offset_exp + 4);


		int32_t vin_slope_exp = LINEAR11_TO_EXPONENT(vin_slope_LINEAR11);
		int32_t vin_slope_man = LINEAR11_TO_MANTISSA(vin_slope_LINEAR11);
		int32_t vin_slope = SHIFT_EXPONENT(vin_slope_man, vin_slope_exp + 14);

		user_data.standby_vin_trim_slope = (uint16_t)vin_slope;
		user_data.standby_vin_trim_offset = (uint16_t)vin_offset;

	}
	else if (direction == PMBUS_READ){

	}
#endif
}

#ifdef vin_trim
void Update_Vin_Trim(uint16_t slope, uint16_t offset)
{
	TSEN_TSEN_VIN__VIN_PWL_SLOPE__SET((uint32_t)slope);		// Format: U-2.14. 	Length: 12-bit. LSB: 0.00006103515625	Min: 0		Max: 0.24993896484375
	TSEN_TSEN_VIN__VIN_TRIM__SET((uint32_t)offset);			// Format: S6.4.	Length: 10-bit.	LSB: 0.0625				Min: -32	Max: 31.9375

	// For debugging purpose. Can be deleted once verified
	PMBUS_Set_Command_Dword(0, PMBUS_CMDCODE_MFR_DBG_VIN_TRIMMING, (slope << 16 | offset));
	PMBUS_Set_Command_Dword(1, PMBUS_CMDCODE_MFR_DBG_VIN_TRIMMING, (slope << 16 | offset));
}
#endif



void PMBUS_HANDLE_MFR_DBG_VIN_TRIMMING(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef vin_trim
	if(direction == PMBUS_WRITE || direction == OTP_RESTORE)
	{

	}
	else if (direction == PMBUS_READ){

	}
#endif
}
