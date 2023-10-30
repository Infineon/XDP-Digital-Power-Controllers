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

#ifndef ADD_ON_FEATURES_H_
#define ADD_ON_FEATURES_H_

//void add_on_features_init (void);
void zsc_gpio_config (void);

#include "regulation_drv.h"
#include "telemetry_api.h"

// ADD-ON Features:
// add your features.
// Features that user can enable via a define
#define patch_hsc


#endif
