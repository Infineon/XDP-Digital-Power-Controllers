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
*/

/* Standard library */
#include "telemetry_api.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "shasta_hal_tsen.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_analog.h"
#include "shasta_hal_common.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_isen.h"
#include "log_app.h"             /* Logging of errors */
#include <string.h>             /* memset */
#include "patch_rom_table.h"
#include "string.h"
#include "user_app.h"
#include "add_on_features.h"
#include "shasta_hal_tstctrl.h"  	// ate test state
#include "mmu_drv.h"                /* MMU_Initialization */
#include "driver_common.h"
#include "shasta_hal_fault_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"
#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"

/*
void add_on_features_init (void)
{

}
*/

void zsc_gpio_config (void)
{
	/*0. PWM1 is configured as GPIO output to control E-fuse, push-pull output.
	PWM2 should be configured as GPIO input and take PWRGD_aux input,
	tlm0_vin_src_sel = 6, ramp0_force_duty= 0 , ramp0_forec_duty_en=1. PWM1 output low. */
	//when enabled Low
	// PMW1 - Output pin - GPIO0[5] - Efuse enable output pin .
	COMMON_MP_FUNC3__PWM1_FUNC__SET(1);  //GPIO0[5]
	COMMON_MP_FUNC3__PWM1_PD__SET(0); 	 //pulldown disabled
	COMMON_MP_FUNC3__PWM1_PU_N__SET(1);  //pullup disabled
	COMMON_MP_FUNC3__PWM1_PPEN__SET(1);  //CMOS output

	GpioBit_SetCnfg((gpio_idx_t)GPIO_0, (uint8_t)PWM1_BIT_INDEX);
	GpioBit_SetDir((gpio_idx_t)GPIO_0, (uint8_t)PWM1_BIT_INDEX, DIRECTION_OUTPUT);
	GpioBit_DatWr((gpio_idx_t)GPIO_0, (uint8_t)PWM1_BIT_INDEX, 0x0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

	// PMW2 - Inputput pin - GPIO0[5]- Input to enable Shasta - along with actual enable pin.
	COMMON_MP_FUNC3__PWM2_FUNC__SET(1);  //GPIO0[5]
	COMMON_MP_FUNC3__PWM2_PD__SET(0); 	 //pulldown disabled
	COMMON_MP_FUNC3__PWM2_PU_N__SET(1);  //pullup disabled
	COMMON_MP_FUNC3__PWM2_PPEN__SET(0);  //open drain

	GpioBit_SetCnfg((gpio_idx_t)GPIO_0, (uint8_t)PWM2_BIT_INDEX);
	GpioBit_SetDir((gpio_idx_t)GPIO_0, (uint8_t)PWM2_BIT_INDEX, DIRECTION_INPUT);
	GpioBit_IntrType((gpio_idx_t)GPIO_0, (uint8_t)PWM2_BIT_INDEX, GPIO_IRQ_TYPE_EDGE_BOTH);  // interrupt on both edges
	GpioBit_IntrEnable((gpio_idx_t)GPIO_0, (uint8_t)PWM2_BIT_INDEX);
/*
	// sync pin
   	//uint8_t sync_pin_gpio_bit_index = 7; //bit7
   	COMMON_MP_FUNC2__SYNC_FUNC__SET(1);  //SYNC pin - GPIO0[7]
   	COMMON_MP_FUNC2__SYNC_PD__SET(0); 	 //pulldown disabled
   	COMMON_MP_FUNC2__SYNC_PU_N__SET(1);  //pullup disabled
   	COMMON_MP_FUNC2__SYNC_PPEN__SET(1);  //CMOS output

   	GpioBit_SetCnfg ((gpio_idx_t)GPIO_0, (uint8_t)SYNC_PIN_GPIO_BIT_INDEX);
   	GpioBit_SetDir ((gpio_idx_t)GPIO_0, (uint8_t)SYNC_PIN_GPIO_BIT_INDEX, DIRECTION_OUTPUT);
   	*/
}

