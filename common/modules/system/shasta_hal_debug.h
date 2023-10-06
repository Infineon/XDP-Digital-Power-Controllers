/**
 * @file     shasta_hal_debug.h
 *
 * @brief    Definitions for the SHASTA Hardware Access Layer (HAL).
 *
 * @version  v0.0
 * @date     date
 *
 * @note
 */

/* ============================================================================
** Copyright (C) 2019 Infineon. All rights reserved.
**               Infineon Technologies, PMM SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorisation.
** ============================================================================
*
*/
/* This file is automatically generated. Do not edit! */
/*lint -save -e960 */


#ifndef _SHASTA_HAL_DEBUG_H_
#define _SHASTA_HAL_DEBUG_H_

#include    <stdint.h>

#include    <stddef.h>
#include    "fw_config.h"
#include    "hal_api.h"

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup SHASTA
 * @{
 */


/** @addtogroup debug
 * @{
 */

#if defined ( __CC_ARM )
#define __DEBUG_INLINE  static __inline __attribute__((always_inline))
#elif defined ( __GNUC__ )
#define __DEBUG_INLINE  static inline __attribute__((always_inline))
#elif defined ( _MSC_VER )
#define __DEBUG_INLINE  static __forceinline
#else
#warning Unsupported Compiler
#define __DEBUG_INLINE  static inline
#endif

/** register file of the debug block.
 */
typedef struct _DEBUG_s
{
    volatile        uint32_t        DATA_LUT[16]                ; /*!< (@0x00000000)  */
} DEBUG_t;

/** enumeration of DEBUG.DATA_LUT member index
 */
/*lint -parent(uint32_t, debug_data_lut_idx_t) -strong(AJX)*/
typedef enum _debug_data_lut_idx_e
{
    debug_data_lut_0 = 0u,    //!< Index 0
    debug_data_lut_1 = 1u,    //!< Index 1
    debug_data_lut_2 = 2u,    //!< Index 2
    debug_data_lut_3 = 3u,    //!< Index 3
    debug_data_lut_4 = 4u,    //!< Index 4
    debug_data_lut_5 = 5u,    //!< Index 5
    debug_data_lut_6 = 6u,    //!< Index 6
    debug_data_lut_7 = 7u,    //!< Index 7
    debug_data_lut_8 = 8u,    //!< Index 8
    debug_data_lut_9 = 9u,    //!< Index 9
    debug_data_lut_10 = 10u,    //!< Index 10
    debug_data_lut_11 = 11u,    //!< Index 11
    debug_data_lut_12 = 12u,    //!< Index 12
    debug_data_lut_13 = 13u,    //!< Index 13
    debug_data_lut_14 = 14u,    //!< Index 14
    debug_data_lut_15 = 15u     //!< Index 15
} debug_data_lut_idx_t;


#define DEBUG_BASE (0x40003000u)                                                          /*!< debug base address */

/** __DEBUG_BASE_PTR returns the base address of the peripheral.
 *
 * @return base address of peripheral's instance
 */
__DEBUG_INLINE uint32_t* __DEBUG_BASE_PTR(void)
{
    return (uint32_t*)(DEBUG_BASE);
}


/* -------------------------------------------------------------------------- */
/*                         DATA_LUT member of DEBUG_t                         */
/* -------------------------------------------------------------------------- */

/** __DEBUG_DATA_LUT__ADDRESS returns the address of member DATA_LUT.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @param ida index in DATA_LUT array
 * @return address of DATA_LUT
 */
__DEBUG_INLINE uint32_t* __DEBUG_DATA_LUT__ADDRESS(debug_data_lut_idx_t ida)
{
    return (uint32_t*)((uint32_t)__DEBUG_BASE_PTR() + (uint32_t)(offsetof(DEBUG_t, DATA_LUT)) + (uint32_t)(sizeof(uint32_t)*(uint32_t)ida));
}

/** DEBUG_DATA_LUT__SET() sets DATA_LUT to given value.
 *
 * When writing to DATA_LUT, this is mandatory to use.
 *
 * @param ida index in DATA_LUT array
 * @param value new value for DATA_LUT
 */
__DEBUG_INLINE void DEBUG_DATA_LUT__SET(debug_data_lut_idx_t ida, uint32_t value)
{
    HAL_SET32(__DEBUG_DATA_LUT__ADDRESS(ida), value);
}

/** DEBUG_DATA_LUT__GET() gets DATA_LUT's current value.
 *
 * When reading from DATA_LUT, this is mandatory to use.
 *
 * @param ida index in DATA_LUT array
 * @return current value of DATA_LUT
 */
__DEBUG_INLINE uint32_t DEBUG_DATA_LUT__GET(debug_data_lut_idx_t ida)
{
    return HAL_GET32(__DEBUG_DATA_LUT__ADDRESS(ida));
}

/* -------------------------- DEBUG.DATA_LUT.DATA3 -------------------------- */


#define DEBUG_DATA_LUT__DATA3__Pos                              24                                                                     /*!< Right-most bit position of DATA3 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA3__Msk                              (0x000000FFUL << DEBUG_DATA_LUT__DATA3__Pos)                           /*!< Mask for DATA3 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA3__Shift(v)                         (((v) << DEBUG_DATA_LUT__DATA3__Pos) & DEBUG_DATA_LUT__DATA3__Msk)     /*!< Shift value 'v' into DATA3 and mask 'v' to fit it into DATA3 field */

/** DEBUG_DATA_LUT__DATA3__SET() sets DATA_LUT.DATA3 to given value.
 *
 * When writing to DATA_LUT.DATA3, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @param bf_value new value for DATA_LUT.DATA3
 */
__DEBUG_INLINE void DEBUG_DATA_LUT__DATA3__SET(debug_data_lut_idx_t ida, uint32_t bf_value)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & ~DEBUG_DATA_LUT__DATA3__Msk);
    reg = (reg | (DEBUG_DATA_LUT__DATA3__Shift((uint32_t)bf_value)));
    DEBUG_DATA_LUT__SET(ida, reg);
}

/** DEBUG_DATA_LUT__DATA3__GET() gets DATA_LUT.DATA3's current value.
 *
 * When reading from DATA_LUT.DATA3, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @return current value of DATA_LUT.DATA3
 */
__DEBUG_INLINE uint32_t DEBUG_DATA_LUT__DATA3__GET(debug_data_lut_idx_t ida)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & DEBUG_DATA_LUT__DATA3__Msk);
    reg = (reg >> DEBUG_DATA_LUT__DATA3__Pos);
    return (uint32_t)reg;
}

/* -------------------------- DEBUG.DATA_LUT.DATA2 -------------------------- */


#define DEBUG_DATA_LUT__DATA2__Pos                              16                                                                     /*!< Right-most bit position of DATA2 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA2__Msk                              (0x000000FFUL << DEBUG_DATA_LUT__DATA2__Pos)                           /*!< Mask for DATA2 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA2__Shift(v)                         (((v) << DEBUG_DATA_LUT__DATA2__Pos) & DEBUG_DATA_LUT__DATA2__Msk)     /*!< Shift value 'v' into DATA2 and mask 'v' to fit it into DATA2 field */

/** DEBUG_DATA_LUT__DATA2__SET() sets DATA_LUT.DATA2 to given value.
 *
 * When writing to DATA_LUT.DATA2, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @param bf_value new value for DATA_LUT.DATA2
 */
__DEBUG_INLINE void DEBUG_DATA_LUT__DATA2__SET(debug_data_lut_idx_t ida, uint32_t bf_value)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & ~DEBUG_DATA_LUT__DATA2__Msk);
    reg = (reg | (DEBUG_DATA_LUT__DATA2__Shift((uint32_t)bf_value)));
    DEBUG_DATA_LUT__SET(ida, reg);
}

/** DEBUG_DATA_LUT__DATA2__GET() gets DATA_LUT.DATA2's current value.
 *
 * When reading from DATA_LUT.DATA2, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @return current value of DATA_LUT.DATA2
 */
__DEBUG_INLINE uint32_t DEBUG_DATA_LUT__DATA2__GET(debug_data_lut_idx_t ida)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & DEBUG_DATA_LUT__DATA2__Msk);
    reg = (reg >> DEBUG_DATA_LUT__DATA2__Pos);
    return (uint32_t)reg;
}

/* -------------------------- DEBUG.DATA_LUT.DATA1 -------------------------- */


#define DEBUG_DATA_LUT__DATA1__Pos                              8                                                                      /*!< Right-most bit position of DATA1 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA1__Msk                              (0x000000FFUL << DEBUG_DATA_LUT__DATA1__Pos)                           /*!< Mask for DATA1 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA1__Shift(v)                         (((v) << DEBUG_DATA_LUT__DATA1__Pos) & DEBUG_DATA_LUT__DATA1__Msk)     /*!< Shift value 'v' into DATA1 and mask 'v' to fit it into DATA1 field */

/** DEBUG_DATA_LUT__DATA1__SET() sets DATA_LUT.DATA1 to given value.
 *
 * When writing to DATA_LUT.DATA1, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @param bf_value new value for DATA_LUT.DATA1
 */
__DEBUG_INLINE void DEBUG_DATA_LUT__DATA1__SET(debug_data_lut_idx_t ida, uint32_t bf_value)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & ~DEBUG_DATA_LUT__DATA1__Msk);
    reg = (reg | (DEBUG_DATA_LUT__DATA1__Shift((uint32_t)bf_value)));
    DEBUG_DATA_LUT__SET(ida, reg);
}

/** DEBUG_DATA_LUT__DATA1__GET() gets DATA_LUT.DATA1's current value.
 *
 * When reading from DATA_LUT.DATA1, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @return current value of DATA_LUT.DATA1
 */
__DEBUG_INLINE uint32_t DEBUG_DATA_LUT__DATA1__GET(debug_data_lut_idx_t ida)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & DEBUG_DATA_LUT__DATA1__Msk);
    reg = (reg >> DEBUG_DATA_LUT__DATA1__Pos);
    return (uint32_t)reg;
}

/* -------------------------- DEBUG.DATA_LUT.DATA0 -------------------------- */


#define DEBUG_DATA_LUT__DATA0__Pos                              0                                                                      /*!< Right-most bit position of DATA0 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA0__Msk                              (0x000000FFUL << DEBUG_DATA_LUT__DATA0__Pos)                           /*!< Mask for DATA0 in DEBUG.DATA_LUT */
#define DEBUG_DATA_LUT__DATA0__Shift(v)                         (((v) << DEBUG_DATA_LUT__DATA0__Pos) & DEBUG_DATA_LUT__DATA0__Msk)     /*!< Shift value 'v' into DATA0 and mask 'v' to fit it into DATA0 field */

/** DEBUG_DATA_LUT__DATA0__SET() sets DATA_LUT.DATA0 to given value.
 *
 * When writing to DATA_LUT.DATA0, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @param bf_value new value for DATA_LUT.DATA0
 */
__DEBUG_INLINE void DEBUG_DATA_LUT__DATA0__SET(debug_data_lut_idx_t ida, uint32_t bf_value)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & ~DEBUG_DATA_LUT__DATA0__Msk);
    reg = (reg | (DEBUG_DATA_LUT__DATA0__Shift((uint32_t)bf_value)));
    DEBUG_DATA_LUT__SET(ida, reg);
}

/** DEBUG_DATA_LUT__DATA0__GET() gets DATA_LUT.DATA0's current value.
 *
 * When reading from DATA_LUT.DATA0, this is recommended to use.
 *
 * @param ida index in DATA_LUT array
 * @return current value of DATA_LUT.DATA0
 */
__DEBUG_INLINE uint32_t DEBUG_DATA_LUT__DATA0__GET(debug_data_lut_idx_t ida)
{
    uint32_t reg = DEBUG_DATA_LUT__GET(ida);
    reg = (reg & DEBUG_DATA_LUT__DATA0__Msk);
    reg = (reg >> DEBUG_DATA_LUT__DATA0__Pos);
    return (uint32_t)reg;
}

/** @} */ /* End of group debug block */


/** @} */ /* End of group SHASTA */

/** @} */ /* End of group Infineon */

#endif /* _SHASTA_HAL_DEBUG_H_ */
