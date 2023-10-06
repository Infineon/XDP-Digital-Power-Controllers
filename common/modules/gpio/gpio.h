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
 * gpio.h
 *
 * Created on:
 * Author: A. Liu
 */

/*****************************************************************************/
/**
* This driver supports the GPIO Controller.
*
* The GPIO Controller supports the following features:
*	- 2 banks
*	- Masked writes (There are no masked reads)
*	- Bypass mode
*	- Configurable Interrupts (Level/Edge)
*
* <b>Driver Description</b>
* The device driver enables higher layer software (e.g., an application) to
* communicate to the GPIO.
*
* <b>Interrupts</b>
* The driver provides interrupt management functions and an interrupt handler.
* Users of this driver need to provide callback functions. An interrupt handler
* example is available with the driver.
*
* <b>Threads</b>
* This driver is not thread safe. Any needs for threads or thread mutual
* exclusion must be satisfied by the layer above this driver.
*
* <b>Building the driver</b>
* The Gpio driver is composed of several source files. This allows the user
* to build and link only those parts of the driver that are necessary.
*/

#ifndef GPIO_H_
#define GPIO_H_

/***************************** Include Files *********************************/

#include <stddef.h>         // NULL pointer for public structs
#include "driver_common.h"
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
#include "shasta_hal_gpio.h"

/************************** Constant Definitions *****************************/

#define GPIO_IRQ_TYPE_EDGE_BOTH	    0  /**< Interrupt on both edges */
#define GPIO_IRQ_TYPE_EDGE_RISING	1  /**< Interrupt on Rising edge */
#define GPIO_IRQ_TYPE_EDGE_FALLING	2  /**< Interrupt Falling edge */
#define GPIO_IRQ_TYPE_LEVEL_HIGH	3  /**< Interrupt on high level */
#define GPIO_IRQ_TYPE_LEVEL_LOW	    4  /**< Interrupt on low level */

#define GPIO_MAX_BANKS		        2  /**< Max banks in a GPIO device */
#define GPIO_BANK_MAX_PINS          8  /**< Max pins in a GPIO bank */
#define GPIO_DTIMER_MAX             6  /**< Max dtimers for de-glitch */

typedef enum GPIO_ERROR_TYPE_e
{
    GPIO_ERROR_NONE               = 0u,
    GPIO_ERROR                    = 1u
}GPIO_ERROR_TYPE;

/**************************** Type Definitions *******************************/

/*****************************************************************************/
/**
 * This handler data type allows the user to define a callback function to
 * handle the interrupts for the GPIO device. The application using this
 * driver is expected to define a handler of this type, to support interrupt
 * driven mode. The handler executes in an interrupt context such that minimal
 * processing should be performed.
 *
 * @param	Bank is the bank for which the interrupt status has changed.
 * @param	Status is the Interrupt status of the GPIO bank.
 *
 *****************************************************************************/
typedef void (*Gpio_Handler) (gpio_idx_t Bank, uint32_t Status);

/*
 * The Gpio driver instance data. The user is required to allocate a
 * variable of this type for the GPIO device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
STATIC struct Gpio_drv_app_s
{
    Gpio_Handler Handler;                       /**< status handlers for all banks */
    GPIO_ERROR_TYPE error_status;               /**< gpio error status */
	uint8_t      GpioCnfgDone[GPIO_MAX_BANKS];	/**< pins configured */
    uint32_t     IntrStatus[GPIO_MAX_BANKS];    /**< interrupt status */
	uint8_t      dti[GPIO_DTIMER_MAX];          /**< dtimer instance used for interrupt */
} Gpio_drv_app;




#endif /**GPIO_H_ */
