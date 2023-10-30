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
** Created on:  2021-03-05
*
 *      Author: kotvytsr
 */


#ifndef ADD_ON_FEATURES_H_
#define ADD_ON_FEATURES_H_

#include "regulation_drv.h"

#define en_ishare

void add_on_features_init (void);


#ifdef en_ishare
void added_droop_disable(PMBUS_PAGE_t loop);
void added_droop_enable(PMBUS_PAGE_t loop);
void remove_added_droop_irq_callback(PMBUS_PAGE_t loop);

void enable_ishare(PMBUS_PAGE_t loop);
void disable_ishare(PMBUS_PAGE_t loop);
//void mfr_remove_added_droop(PMBUS_PAGE_t loop);
void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
#endif

#endif
