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
 * gpio_drv.h
 *
 *  Created on:
 *  Author: A. Liu
 */

#ifndef GPIO_DRV_H_
#define GPIO_DRV_H_

/***************************** Include Files *********************************/

#include <stddef.h>         // NULL pointer for public structs
#include "driver_common.h"
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
#include "shasta_hal_gpio.h"
#include "gpio.h"
#include "dtimer_drv.h"
#include "clk_ctrl_drv.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

// Configuration variable for 1 timer
STATIC DTIMER_CONFIG_t glitch_cfg;

// Handler for the driver functions
STATIC DRIVER_DTIMER *glitch_drv_info;

/*****************************************************************************/

/*
 * Peripheral control functions in gpio_drv.c
 */
void GPIO_Drv_init(gpio_idx_t Bank);
void GPIO_Drv_uninit(gpio_idx_t Bank);

/*
 * Bank APIs in gpio_drv.c
 */
void GpioBank_SetCnfg (gpio_idx_t Bank);
void GpioBank_RstCnfg (gpio_idx_t Bank);
uint8_t GpioBank_GetCnfg (gpio_idx_t Bank);
uint32_t GpioBank_DatRd(gpio_idx_t Bank);
void GpioBank_DatWr(gpio_idx_t Bank, uint32_t Data);
uint32_t GpioBank_GetDirection(gpio_idx_t Bank);
void GpioBank_SetDirection(gpio_idx_t Bank, uint32_t Direction);

/*
 * Pin APIs in gpio_drv.c
 */
void GpioBit_CkCnfg (uint8_t Cnfg);
void GpioBit_SetCnfg (gpio_idx_t Bank, uint8_t Cnfg);
void GpioBit_RstCnfg (gpio_idx_t Bank, uint8_t Cnfg);
uint32_t GpioBit_DatRd(gpio_idx_t Bank, uint8_t Cnfg);
void GpioBit_DatWr(gpio_idx_t Bank, uint8_t Cnfg, uint32_t Data);
void GpioBit_SetDir (gpio_idx_t Bank, uint8_t Cnfg, uint8_t Direction);
uint8_t GpioBit_GetDir (gpio_idx_t Bank, uint8_t Cnfg);

/*
 * Interrupt Pin APIs in gpio_intr.c
 */
void GpioBit_IntrEnable(gpio_idx_t Bank, uint8_t Cnfg);
void GpioBit_IntrDisable(gpio_idx_t Bank, uint8_t Cnfg);
void GpioBit_IntrClr(gpio_idx_t Bank, uint8_t Cnfg);
void GpioBit_IntrType(gpio_idx_t Bank, uint8_t Cnfg, uint8_t Type);
void GpioBit_Dglitch_IntrEnable(gpio_idx_t Bank, uint8_t Cnfg);

/*
 * Interrupt Bank APIs in gpio_intr.c
 */
uint32_t GpioBank_IntrGetStatus(gpio_idx_t Bank);
void GpioBank_IntrClear(gpio_idx_t Bank, uint32_t Mask);
void Gpio_IntrHandler(void);
void GpioBank_SetCallbackHandler(Gpio_Handler FuncPtr);
void Gpio_Dglitch_activation (dtimer_instance_e dtie, uint32_t ticks);
void Gpio_RstDtimerCnfg(dtimer_instance_e dtie);
void Gpio_RstDtimerAllCnfg(void);
void Gpio_Dglitch_IntrHandler(void);
void Gpio_Dtimer_ISR(void);




#endif /* GPIO_DRV_H_ */



