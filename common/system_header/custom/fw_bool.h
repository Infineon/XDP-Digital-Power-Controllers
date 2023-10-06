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
 * @file    fw_bool.h
 * @brief   This header file defines a specific boolean data type for C.
 *
 *          The usage of the built-in bool-type variables is discouraged. Using
 *          uint8_t-type is encouraged. There are 2 main reasons
 *          - C compilers tend to create code- and runtime overhead for
 *            implementing boolean variables. The gain in data memory footprint
 *            is usually massively offset by the loss in code memory footpint.
 *          - Having a mixture of both bool and uint8_t-type variables for
 *            implementing boolean data and return values in one code base is
 *            not helping readability and maintainability. You want only one
 *            method for handling boolean data and return values.
 */

#ifndef FW_BOOL_H
#define FW_BOOL_H

#include <stdint.h>     // uint*_t

#define boolean uint8_t

#define boolean_true (boolean)1
#define boolean_false (boolean)0

/* Signal that all the definitions are present. */
#define __bool_true_false_are_defined 1

#endif /* FW_BOOL_H */
