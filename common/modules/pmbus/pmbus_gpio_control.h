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
 * pmbus_gpio_control.h
 *
 *  Created on: Jan 19, 2017
 *      Author: dlewis4
 */

#ifndef PMBUS_GPIO_CONTROL_H_
#define PMBUS_GPIO_CONTROL_H_

#include "pmbus_autogen.h"      // PMBUS_PAGE_t

#define DIRECTION_INPUT 0
#define DIRECTION_OUTPUT 1

/*
 * This is used to decode the fields in FW_CONFIG_REGULATION PMBUS command:
 */
typedef enum PMBUS_GPIO_CONFIG_e
{
	PMBUS_GPIO_CONFIG_POLARITY = 32u,  				// polarity of pmbus gpio driver
	PMBUS_GPIO_CONFIG_DIRECTION = 40u,				// direction of given pin
	PMBUS_GPIO_CONFIG_ENABLE_SELECT = 48u,  		// gpio bit to map to enable
	PMBUS_GPIO_CONFIG_POWER_GOOD_SELECT = 51u,  	// gpio bit to map to power good
	PMBUS_GPIO_CONFIG_FAULT_SELECT = 54u,			// gpio bit to map to fault
} PMBUS_GPIO_CONFIG;


// One time initialization for GPIO configuration
void PMBUS_Gpio_Configure(void);

// GPIO access methods by function:
uint8_t PMBUS_Gpio_Get_PMBUS_Configuration(PMBUS_PAGE_t loop, PMBUS_GPIO_CONFIG config);
boolean PMBUS_Gpio_Get_Feature(PMBUS_PAGE_t loop, PMBUS_GPIO_CONFIG config);
void PMBUS_Gpio_Set_Feature(PMBUS_PAGE_t loop, PMBUS_GPIO_CONFIG config, boolean value);

#endif /* PMBUS_GPIO_CONTROL_H_ */
