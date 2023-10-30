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

#ifndef COMMON_MODULES_USER_APP_USER_APP_H_
#define COMMON_MODULES_USER_APP_USER_APP_H_
#define N_ROWS   (128)

#include "uart_drv.h"

void user_drv_init(void);
//
//uint8_t  src_buf[N_ROWS];
uint8_t  dest_buf[N_ROWS];
ARM_DRIVER_USART * UARTdrv;

#endif /* COMMON_MODULES_USER_APP_USER_APP_H_ */
