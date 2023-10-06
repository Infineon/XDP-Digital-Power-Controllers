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
 * @file    hash_generator.h
 * @brief   API for creating a hash.
 */

#ifndef COMMON_UTILITIES_HASH_GENERATOR_H_
#define COMMON_UTILITIES_HASH_GENERATOR_H_

#include <stdint.h>     /*  uint*_t */

uint32_t hash_code_create(const uint8_t* ptr, uint8_t size);

#endif /* COMMON_UTILITIES_HASH_GENERATOR_H_ */
