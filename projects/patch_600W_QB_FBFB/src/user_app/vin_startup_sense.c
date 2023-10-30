/*
 * vin_startup_sense.c
 *
 *  Created on: 15 Jul 2019
 *      Author: halimand
 */
/*
 * regulation_state_machine_callbacks.c
 *
 *  Created on: Jun 12, 2017
 *      Author: dlewis4
 */
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "dtimer_drv.h"
#include "telemetry_api.h"
#include "telemetry_irq_handlers.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "pmbus_api.h"
#include "pmbus_gpio_control.h"
#include "shasta_hal_vsen.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"
#include "shasta_hal_telem.h"
#include "shasta_hal_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_isen.h"
#include "regulation_state_machine_callbacks.h"
#include "log_app.h"             /* Logging of errors */
#include "periodic_functions.h"
#include "patch_rom_table.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_fault_common.h"
#include "add_on_features.h"
#include "shasta_hal_tsen.h"



/* BEFORE STARTUP -> state = OFF */
void Enable_PRISEN_Sensing(PMBUS_PAGE_t loop)
{
	// Input voltage telemetry source select to VRSEN with TS-ADC VIN
	TELEM_TLM_CFG1__TLM_VIN_SRC_SEL__SET(loop, 3);
}


/* BEFORE STARTUP -> state = TON_DELAY */
void Calculate_Vin_from_PRISEN(PMBUS_PAGE_t loop)
{
	// Low pass filtered input voltage telemetry
	uint16_t temp_vin = TELEM_TLM_VIN_LPF__TLM_VIN_LPF__GET((telem_idx_t) loop); 										// U7.4. LSB = 0.0625. Min = 0. Max = 127.9375
	// need to scale down by 4


	// Get VRECT (a.k.a. VRSEN) scale --> Read from Register
	uint16_t vrect_scale_value = TELEM_TLM_VRECT_SCALE_LOOP__TLM_VRECT_SCALE_LOOP__GET((telem_idx_t) loop); 			// U0.12. LSB = 2.4414e-04. Min = 0. Max = 0.99976
	// scale down by 12

	// Get Transformer scale --> Read from Register
	uint16_t transformer_turns_ratio = TELEM_TLM_TRNS_SCALE_LP__TLM_TRANSFORMER_SCALE_LOOP__GET((telem_idx_t) loop); 	// U0.12. LSB = 2.4414e-4. Min = 0. Max = 0.99976
	// scale down by 12

	// Calculate Input Voltage
	// Input Voltage = VRSEN Voltage Divider * Referenced Vin at secondary-side
	// Input Voltage = VRSEN Voltage Divider * {(Nsec/Npri) * Actual Pri-side Vin}
	uint32_t vin = (vrect_scale_value * transformer_turns_ratio) >> 12; 												// multiply two U0.12 numbers and scale down by 12 bits.
	uint32_t vin_1 = (vin * temp_vin * 50) >> 12;
	uint32_t vin_2 = vin_1 >> 4;

	// Assign VIN value to vrs_voltage_init. 1/LSB = 1/20mV = 1/0.02V --> multiply by 50 and shift down by 14
	COMMON_VRS_VOLTAGE_INIT__VRS_VOLTAGE_INIT__SET(vin_2); 																// U12.-4. LSB = 0.02. Min = 0. Max = 2.1V.

	// Input voltage Telemetry source select to VRSEN with vrs_init
	TELEM_TLM_CFG1__TLM_VIN_SRC_SEL__SET((telem_idx_t) loop, 0);
}


