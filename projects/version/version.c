/* ============================================================================
** Copyright (C) 2019 Infineon Technologies AG
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
 * @file    version.c
 * @brief   Contains the version information of the firmware image.
 */

/* Always compare to the associated interface */
#include "version.h"

/* Standard library */
#include <stdint.h>     // uint*_t

/** This variable describes the current firmware version number. */
const uint32_t version_number = 9;

/** This variable describes the current firmware version timestamp. This 
    number might be modified by applied patches. */
uint32_t version_timestamp = 0x5C843EFAu; /* UTC time: 2019-03-09 17:32:26 */
