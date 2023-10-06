/* ============================================================================
** Copyright (C) 2014 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/**
 * @file    oam_app.h
 * @brief   Public declarations and interface of the application for the
 *          Operation & Maintenance FW Module.
 */

#ifndef OAM_APP_H
#define OAM_APP_H

#include "shasta.h"

/* ============================================================================
 * Public data types
 * ========================================================================= */

/* ============================================================================
 * Function declarations
 * ========================================================================= */

void HardFault_Handler(const Cortex_M0_Stack_Frame_t* stack_frame);
void Default_IRQHandler(void);

#endif /* OAM_APP_H */
