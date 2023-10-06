/**
 * @file     hal_api.h
 *
 * @brief    Low-level memory/register access used by the FW image.
 *
 */
/* ===================================================================
 ** Copyright (C) 2013 Infineon Technologies AG
 ** All rights reserved.
 ** ===================================================================
 **
 ** ===================================================================
 ** This document contains proprietary information. Passing on and
 ** copying of this document, and communication of its contents is not
 ** permitted without prior written authorization.
 ** ===================================================================
 **
 **
 */

#ifndef __HAL_API_H__
#define __HAL_API_H__

#include    <stdint.h>

#ifdef _lint
// PCLint does not process any __attribute__ statements, so we take it out.
#define HAL_INLINE                  static __inline

#else
#define HAL_INLINE  static __inline __attribute__((always_inline))
#endif

//#pragma message("Using HAL for FW image")

/** HAL_SET32() provides the lowest level of memory access:
 * It allows to write a uint32_t value to some address.
 *
 * This version of HAL_SET32() is meant for the FW image on the
 * actual target silicon - a simple assignment does the job.
 *
 * @param addr pointer to address, needs to be volatile because there might
 * HW 'behind' this address, changing the address' content.
 * @param val value
 */
HAL_INLINE void HAL_SET32(volatile uint32_t* addr, uint32_t val);

/* lint requires us to have both a declaration and a definition ... we
 * could probably bail out through some local lint, but her we simply
 * play to the rules.
 */
HAL_INLINE void HAL_SET32(volatile uint32_t* addr, uint32_t val) {
    *addr = val;
}

/** HAL_SET32() provides the lowest level of memory access:
 * It allows to read a uint32_t value from some address.
 *
 * This version of HAL_SET32() is meant for the FW image on the
 * actual target silicon - a simple assignment does the job.
 *
 * @param addr pointer to address, needs to be volatile because there might
 * HW 'behind' this address, changing the address' content.
 */
HAL_INLINE uint32_t HAL_GET32(volatile uint32_t* addr);

HAL_INLINE uint32_t HAL_GET32(volatile uint32_t* addr) {
    return *addr;
}

/** HAL_SET16() provides the lowest level of memory access:
 * It allows to write a uint16_t value to some address.
 *
 * This version of HAL_SET16() is meant for the FW image on the
 * actual target silicon - a simple assignment does the job.
 *
 * @param addr pointer to address, needs to be volatile because there might
 * HW 'behind' this address, changing the address' content.
 * @param val value
 */
HAL_INLINE void HAL_SET16(volatile uint16_t* addr, uint16_t val);

/* lint requires us to have both a declaration and a definition ... we
 * could probably bail out through some local lint, but her we simply
 * play to the rules.
 */
HAL_INLINE void HAL_SET16(volatile uint16_t* addr, uint16_t val)
{
    *addr = val;
}

/** HAL_SET16() provides the lowest level of memory access:
 * It allows to read a uint16_t value from some address.
 *
 * This version of HAL_SET16() is meant for the FW image on the
 * actual target silicon - a simple assignment does the job.
 *
 * @param addr pointer to address, needs to be volatile because there might
 * HW 'behind' this address, changing the address' content.
 */
HAL_INLINE uint16_t HAL_GET16(volatile uint16_t* addr);

HAL_INLINE uint16_t HAL_GET16(volatile uint16_t* addr)
{
    return *addr;
}

/** HAL_SET8() provides the lowest level of memory access:
 * It allows to write a uint8_t value to some address.
 *
 * This version of HAL_SET8() is meant for the FW image on the
 * actual target silicon - a simple assignment does the job.
 *
 * @param addr pointer to address, needs to be volatile because there might
 * HW 'behind' this address, changing the address' content.
 * @param val value
 */
HAL_INLINE void HAL_SET8(volatile uint8_t* addr, uint8_t val);

/* lint requires us to have both a declaration and a definition ... we
 * could probably bail out through some local lint, but her we simply
 * play to the rules.
 */
HAL_INLINE void HAL_SET8(volatile uint8_t* addr, uint8_t val)
{
    *addr = val;
}

/** HAL_SET8() provides the lowest level of memory access:
 * It allows to read a uint8_t value from some address.
 *
 * This version of HAL_SET8() is meant for the FW image on the
 * actual target silicon - a simple assignment does the job.
 *
 * @param addr pointer to address, needs to be volatile because there might
 * HW 'behind' this address, changing the address' content.
 */
HAL_INLINE uint8_t HAL_GET8(volatile uint8_t* addr);

HAL_INLINE uint8_t HAL_GET8(volatile uint8_t* addr)
{
    return *addr;
}

#endif /* __HAL_API_H__ */
