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
 * @file    bits.h
 * @brief   This header file contains a collection of generic macros for
 *          bit access & manipulation of arbitrary values.<br>
 *          It also contains macros for creating and accessing sets of bits
 *          which are used for an efficient memory footprint of storing a
 *          large amount of bits.
 */

#ifndef BITS_H
#define BITS_H

/* ============================================================================
 * Bit access & manipulation of arbitrary values
 * ========================================================================= */

/** Creates a constant with a bit set to 1 at @a pos (count from 0 onwards). */
#define BIT(pos)                        (uint32_t)( (uint32_t)1 << (uint8_t)(pos) )
/** Creates a bit mask of @a length. Bits 0..length-1 will be set to 1. */
#define BIT_MASK(length)                ( BIT(length) - 1 )
/** Creates a bit mask of @a length starting at bit @a start. */
#define BIT_MASK_SHIFT(length, start)   ( BIT_MASK(length) << (start) )

/** Returns the bit at @a pos in @a value. */
#define BIT_GET(value, pos)             ( ((uint32_t)(value) >> (pos)) & BIT_MASK(1) )
/** Sets the bit at @a pos in @a value. */
#define BIT_SET(value, pos)             ( (uint32_t)(value) | BIT(pos) )
/** Clears the bit at @a pos in @a value. */
#define BIT_CLEAR(value, pos)           ( (uint32_t)(value) & ~BIT(pos) )

/** Returns all bits within @a value between @a low and @a low + (@a length - 1). */
#define BITS_GET(value, low, length)    ( ((uint32_t)(value) >> (low)) & BIT_MASK(length) )
/** Sets all bits within @a value between @a low and @a low + (@a length - 1). */
#define BITS_SET(value, low, length)    ( (uint32_t)(value) | BIT_MASK_SHIFT(length, low) )
/** Clears all bits within @a value between @a low and @a low + (@a length - 1). */
#define BITS_CLEAR(value, low, length)  ( (uint32_t)(value) & ~BIT_MASK_SHIFT(length, low) )


/* ============================================================================
 * Declaration and access of bit sets
 * ========================================================================= */

/** Defines the constant @a name as a bit set made of @a length bits. */
#define BITSET_DECLARE(name, length)    uint8_t name[((uint32_t)length + 7) / 8]
/** Returns the bit at @a pos in the bit set @a name. */
#define BITSET_GET(name, pos)           BIT_GET(name[(uint32_t)pos / 8], (uint32_t)pos % 8)
/*lint -esym(773, BITSET_*) : Expression-like macro 'BITSET_SET' and 'BITSET_CLEAR' not parenthesized. This macro assigns a variable! */
/** Sets the bit at @a pos in the bit set @a name. */
#define BITSET_SET(name, pos)           name[(uint32_t)pos / 8] = (uint8_t) BIT_SET(name[(uint32_t)pos / 8], (uint32_t)pos % 8)
/** Clears the bit at @a pos in the bit set @a name. */
#define BITSET_CLEAR(name, pos)         name[(uint32_t)pos / 8] = (uint8_t) BIT_CLEAR(name[(uint32_t)pos / 8], (uint32_t)pos % 8)

#endif /* BITS_H */
