/**
 * @file     shasta_hal_crc32.h
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


#ifndef _SHASTA_HAL_CRC32_H_
#define _SHASTA_HAL_CRC32_H_

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


/** @addtogroup crc32
 * @{
 */

#if defined ( __CC_ARM )
#define __CRC32_INLINE  static __inline __attribute__((always_inline))
#elif defined ( __GNUC__ )
#define __CRC32_INLINE  static inline __attribute__((always_inline))
#elif defined ( _MSC_VER )
#define __CRC32_INLINE  static __forceinline
#else
#warning Unsupported Compiler
#define __CRC32_INLINE  static inline
#endif

/** register file of the crc32 block.
 */
typedef struct _CRC32_s
{
    volatile        uint32_t        DATA                        ; /*!< (@0x00000000) CRC DATA IN & OUT register */
    volatile        uint32_t        INIT                        ; /*!< (@0x00000004) CRC Initialization Value register */
    volatile        uint32_t        POLY                        ; /*!< (@0x00000008) CRC Polynomial Configuration Register */
    volatile        uint32_t        XORE                        ; /*!< (@0x0000000C) CRC Final XOR Configuration Register */
    volatile        uint32_t        CNFG                        ; /*!< (@0x00000010) CRC Control Register */
} CRC32_t;


#define CRC32_BASE (0x70090000u)                                                          /*!< crc32 base address */

/** __CRC32_BASE_PTR returns the base address of the peripheral.
 *
 * @return base address of peripheral's instance
 */
__CRC32_INLINE uint32_t* __CRC32_BASE_PTR(void)
{
    return (uint32_t*)(CRC32_BASE);
}


/* -------------------------------------------------------------------------- */
/*                           DATA member of CRC32_t                           */
/* -------------------------------------------------------------------------- */

/** __CRC32_DATA__ADDRESS returns the address of member DATA.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @return address of DATA
 */
__CRC32_INLINE uint32_t* __CRC32_DATA__ADDRESS(void)
{
    return (uint32_t*)((uint32_t)__CRC32_BASE_PTR() + (uint32_t)(offsetof(CRC32_t, DATA)));
}

/** CRC32_DATA__SET() sets DATA to given value.
 *
 * When writing to DATA, this is mandatory to use.
 *
 * @param value new value for DATA
 */
__CRC32_INLINE void CRC32_DATA__SET(uint32_t value)
{
    HAL_SET32(__CRC32_DATA__ADDRESS(), value);
}

/** CRC32_DATA__GET() gets DATA's current value.
 *
 * When reading from DATA, this is mandatory to use.
 *
 * @return current value of DATA
 */
__CRC32_INLINE uint32_t CRC32_DATA__GET(void)
{
    return HAL_GET32(__CRC32_DATA__ADDRESS());
}

/* ----------------------------- CRC32.DATA.VAL ----------------------------- */


#define CRC32_DATA__VAL__Pos                                    0                                                                      /*!< Right-most bit position of VAL in CRC32.DATA */
#define CRC32_DATA__VAL__Msk                                    (0xFFFFFFFFUL << CRC32_DATA__VAL__Pos)                                 /*!< Mask for VAL in CRC32.DATA */
#define CRC32_DATA__VAL__Shift(v)                               (((v) << CRC32_DATA__VAL__Pos) & CRC32_DATA__VAL__Msk)                 /*!< Shift value 'v' into VAL and mask 'v' to fit it into VAL field */

/** CRC32_DATA__VAL__SET() sets DATA.VAL to given value.
 *
 * When writing to DATA.VAL, this is recommended to use.
 *
 * @param bf_value new value for DATA.VAL
 */
__CRC32_INLINE void CRC32_DATA__VAL__SET(uint32_t bf_value)
{
    uint32_t reg = CRC32_DATA__GET();
    reg = (reg & ~CRC32_DATA__VAL__Msk);
    reg = (reg | (CRC32_DATA__VAL__Shift((uint32_t)bf_value)));
    CRC32_DATA__SET(reg);
}

/** CRC32_DATA__VAL__GET() gets DATA.VAL's current value.
 *
 * When reading from DATA.VAL, this is recommended to use.
 *
 * @return current value of DATA.VAL
 */
__CRC32_INLINE uint32_t CRC32_DATA__VAL__GET(void)
{
    uint32_t reg = CRC32_DATA__GET();
    reg = (reg & CRC32_DATA__VAL__Msk);
    reg = (reg >> CRC32_DATA__VAL__Pos);
    return (uint32_t)reg;
}

/** CRC32_DATA__VAL__MODIFY() modifies DATA.VAL.
 *
 * This is recommended to use when setting more than one bitfield of CRC32.DATA.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of CRC32.DATA
 * @param bf_value new value for DATA.VAL
 * @return new value of CRC32.DATA
 */
__CRC32_INLINE uint32_t CRC32_DATA__VAL__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~CRC32_DATA__VAL__Msk);
    reg = (reg | (CRC32_DATA__VAL__Shift((uint32_t)bf_value)));
    return reg;
}

/** CRC32_DATA__VAL__EXTRACT() extracts DATA.VAL.
 *
 * This is recommended to use when extracting more than one bitfield from CRC32.DATA.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of CRC32.DATA
 * @return current value of DATA.VAL
 */
__CRC32_INLINE uint32_t CRC32_DATA__VAL__EXTRACT(uint32_t reg)
{
    reg = (reg & CRC32_DATA__VAL__Msk);
    reg = (reg >> CRC32_DATA__VAL__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           INIT member of CRC32_t                           */
/* -------------------------------------------------------------------------- */

/** __CRC32_INIT__ADDRESS returns the address of member INIT.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @return address of INIT
 */
__CRC32_INLINE uint32_t* __CRC32_INIT__ADDRESS(void)
{
    return (uint32_t*)((uint32_t)__CRC32_BASE_PTR() + (uint32_t)(offsetof(CRC32_t, INIT)));
}

/** CRC32_INIT__SET() sets INIT to given value.
 *
 * When writing to INIT, this is mandatory to use.
 *
 * @param value new value for INIT
 */
__CRC32_INLINE void CRC32_INIT__SET(uint32_t value)
{
    HAL_SET32(__CRC32_INIT__ADDRESS(), value);
}


/* ----------------------------- CRC32.INIT.VAL ----------------------------- */


#define CRC32_INIT__VAL__Pos                                    0                                                                      /*!< Right-most bit position of VAL in CRC32.INIT */
#define CRC32_INIT__VAL__Msk                                    (0xFFFFFFFFUL << CRC32_INIT__VAL__Pos)                                 /*!< Mask for VAL in CRC32.INIT */
#define CRC32_INIT__VAL__Shift(v)                               (((v) << CRC32_INIT__VAL__Pos) & CRC32_INIT__VAL__Msk)                 /*!< Shift value 'v' into VAL and mask 'v' to fit it into VAL field */

/** CRC32_INIT__VAL__SET() sets INIT.VAL to given value.
 *
 * When writing to INIT.VAL, this is recommended to use.
 *
 * @param bf_value new value for INIT.VAL
 */
__CRC32_INLINE void CRC32_INIT__VAL__SET(uint32_t bf_value)
{
    uint32_t reg = CRC32_INIT__VAL__Shift((uint32_t)bf_value);
    CRC32_INIT__SET(reg);
}


/** CRC32_INIT__VAL__MODIFY() modifies INIT.VAL.
 *
 * This is recommended to use when setting more than one bitfield of CRC32.INIT.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of CRC32.INIT
 * @param bf_value new value for INIT.VAL
 * @return new value of CRC32.INIT
 */
__CRC32_INLINE uint32_t CRC32_INIT__VAL__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~CRC32_INIT__VAL__Msk);
    reg = (reg | (CRC32_INIT__VAL__Shift((uint32_t)bf_value)));
    return reg;
}

/** CRC32_INIT__VAL__EXTRACT() extracts INIT.VAL.
 *
 * This is recommended to use when extracting more than one bitfield from CRC32.INIT.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of CRC32.INIT
 * @return current value of INIT.VAL
 */
__CRC32_INLINE uint32_t CRC32_INIT__VAL__EXTRACT(uint32_t reg)
{
    reg = (reg & CRC32_INIT__VAL__Msk);
    reg = (reg >> CRC32_INIT__VAL__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           POLY member of CRC32_t                           */
/* -------------------------------------------------------------------------- */

/** __CRC32_POLY__ADDRESS returns the address of member POLY.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @return address of POLY
 */
__CRC32_INLINE uint32_t* __CRC32_POLY__ADDRESS(void)
{
    return (uint32_t*)((uint32_t)__CRC32_BASE_PTR() + (uint32_t)(offsetof(CRC32_t, POLY)));
}

/** CRC32_POLY__SET() sets POLY to given value.
 *
 * When writing to POLY, this is mandatory to use.
 *
 * @param value new value for POLY
 */
__CRC32_INLINE void CRC32_POLY__SET(uint32_t value)
{
    HAL_SET32(__CRC32_POLY__ADDRESS(), value);
}

/** CRC32_POLY__GET() gets POLY's current value.
 *
 * When reading from POLY, this is mandatory to use.
 *
 * @return current value of POLY
 */
__CRC32_INLINE uint32_t CRC32_POLY__GET(void)
{
    return HAL_GET32(__CRC32_POLY__ADDRESS());
}

/* ----------------------------- CRC32.POLY.VAL ----------------------------- */


#define CRC32_POLY__VAL__Pos                                    0                                                                      /*!< Right-most bit position of VAL in CRC32.POLY */
#define CRC32_POLY__VAL__Msk                                    (0xFFFFFFFFUL << CRC32_POLY__VAL__Pos)                                 /*!< Mask for VAL in CRC32.POLY */
#define CRC32_POLY__VAL__Shift(v)                               (((v) << CRC32_POLY__VAL__Pos) & CRC32_POLY__VAL__Msk)                 /*!< Shift value 'v' into VAL and mask 'v' to fit it into VAL field */

/** CRC32_POLY__VAL__SET() sets POLY.VAL to given value.
 *
 * When writing to POLY.VAL, this is recommended to use.
 *
 * @param bf_value new value for POLY.VAL
 */
__CRC32_INLINE void CRC32_POLY__VAL__SET(uint32_t bf_value)
{
    uint32_t reg = CRC32_POLY__GET();
    reg = (reg & ~CRC32_POLY__VAL__Msk);
    reg = (reg | (CRC32_POLY__VAL__Shift((uint32_t)bf_value)));
    CRC32_POLY__SET(reg);
}

/** CRC32_POLY__VAL__GET() gets POLY.VAL's current value.
 *
 * When reading from POLY.VAL, this is recommended to use.
 *
 * @return current value of POLY.VAL
 */
__CRC32_INLINE uint32_t CRC32_POLY__VAL__GET(void)
{
    uint32_t reg = CRC32_POLY__GET();
    reg = (reg & CRC32_POLY__VAL__Msk);
    reg = (reg >> CRC32_POLY__VAL__Pos);
    return (uint32_t)reg;
}

/** CRC32_POLY__VAL__MODIFY() modifies POLY.VAL.
 *
 * This is recommended to use when setting more than one bitfield of CRC32.POLY.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of CRC32.POLY
 * @param bf_value new value for POLY.VAL
 * @return new value of CRC32.POLY
 */
__CRC32_INLINE uint32_t CRC32_POLY__VAL__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~CRC32_POLY__VAL__Msk);
    reg = (reg | (CRC32_POLY__VAL__Shift((uint32_t)bf_value)));
    return reg;
}

/** CRC32_POLY__VAL__EXTRACT() extracts POLY.VAL.
 *
 * This is recommended to use when extracting more than one bitfield from CRC32.POLY.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of CRC32.POLY
 * @return current value of POLY.VAL
 */
__CRC32_INLINE uint32_t CRC32_POLY__VAL__EXTRACT(uint32_t reg)
{
    reg = (reg & CRC32_POLY__VAL__Msk);
    reg = (reg >> CRC32_POLY__VAL__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           XORE member of CRC32_t                           */
/* -------------------------------------------------------------------------- */

/** __CRC32_XORE__ADDRESS returns the address of member XORE.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @return address of XORE
 */
__CRC32_INLINE uint32_t* __CRC32_XORE__ADDRESS(void)
{
    return (uint32_t*)((uint32_t)__CRC32_BASE_PTR() + (uint32_t)(offsetof(CRC32_t, XORE)));
}

/** CRC32_XORE__SET() sets XORE to given value.
 *
 * When writing to XORE, this is mandatory to use.
 *
 * @param value new value for XORE
 */
__CRC32_INLINE void CRC32_XORE__SET(uint32_t value)
{
    HAL_SET32(__CRC32_XORE__ADDRESS(), value);
}

/** CRC32_XORE__GET() gets XORE's current value.
 *
 * When reading from XORE, this is mandatory to use.
 *
 * @return current value of XORE
 */
__CRC32_INLINE uint32_t CRC32_XORE__GET(void)
{
    return HAL_GET32(__CRC32_XORE__ADDRESS());
}

/* ----------------------------- CRC32.XORE.VAL ----------------------------- */


#define CRC32_XORE__VAL__Pos                                    0                                                                      /*!< Right-most bit position of VAL in CRC32.XORE */
#define CRC32_XORE__VAL__Msk                                    (0xFFFFFFFFUL << CRC32_XORE__VAL__Pos)                                 /*!< Mask for VAL in CRC32.XORE */
#define CRC32_XORE__VAL__Shift(v)                               (((v) << CRC32_XORE__VAL__Pos) & CRC32_XORE__VAL__Msk)                 /*!< Shift value 'v' into VAL and mask 'v' to fit it into VAL field */

/** CRC32_XORE__VAL__SET() sets XORE.VAL to given value.
 *
 * When writing to XORE.VAL, this is recommended to use.
 *
 * @param bf_value new value for XORE.VAL
 */
__CRC32_INLINE void CRC32_XORE__VAL__SET(uint32_t bf_value)
{
    uint32_t reg = CRC32_XORE__GET();
    reg = (reg & ~CRC32_XORE__VAL__Msk);
    reg = (reg | (CRC32_XORE__VAL__Shift((uint32_t)bf_value)));
    CRC32_XORE__SET(reg);
}

/** CRC32_XORE__VAL__GET() gets XORE.VAL's current value.
 *
 * When reading from XORE.VAL, this is recommended to use.
 *
 * @return current value of XORE.VAL
 */
__CRC32_INLINE uint32_t CRC32_XORE__VAL__GET(void)
{
    uint32_t reg = CRC32_XORE__GET();
    reg = (reg & CRC32_XORE__VAL__Msk);
    reg = (reg >> CRC32_XORE__VAL__Pos);
    return (uint32_t)reg;
}

/** CRC32_XORE__VAL__MODIFY() modifies XORE.VAL.
 *
 * This is recommended to use when setting more than one bitfield of CRC32.XORE.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of CRC32.XORE
 * @param bf_value new value for XORE.VAL
 * @return new value of CRC32.XORE
 */
__CRC32_INLINE uint32_t CRC32_XORE__VAL__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~CRC32_XORE__VAL__Msk);
    reg = (reg | (CRC32_XORE__VAL__Shift((uint32_t)bf_value)));
    return reg;
}

/** CRC32_XORE__VAL__EXTRACT() extracts XORE.VAL.
 *
 * This is recommended to use when extracting more than one bitfield from CRC32.XORE.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of CRC32.XORE
 * @return current value of XORE.VAL
 */
__CRC32_INLINE uint32_t CRC32_XORE__VAL__EXTRACT(uint32_t reg)
{
    reg = (reg & CRC32_XORE__VAL__Msk);
    reg = (reg >> CRC32_XORE__VAL__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           CNFG member of CRC32_t                           */
/* -------------------------------------------------------------------------- */

/** __CRC32_CNFG__ADDRESS returns the address of member CNFG.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @return address of CNFG
 */
__CRC32_INLINE uint32_t* __CRC32_CNFG__ADDRESS(void)
{
    return (uint32_t*)((uint32_t)__CRC32_BASE_PTR() + (uint32_t)(offsetof(CRC32_t, CNFG)));
}

/** CRC32_CNFG__SET() sets CNFG to given value.
 *
 * When writing to CNFG, this is mandatory to use.
 *
 * @param value new value for CNFG
 */
__CRC32_INLINE void CRC32_CNFG__SET(uint32_t value)
{
    HAL_SET32(__CRC32_CNFG__ADDRESS(), value);
}

/** CRC32_CNFG__GET() gets CNFG's current value.
 *
 * When reading from CNFG, this is mandatory to use.
 *
 * @return current value of CNFG
 */
__CRC32_INLINE uint32_t CRC32_CNFG__GET(void)
{
    return HAL_GET32(__CRC32_CNFG__ADDRESS());
}

/* --------------------------- CRC32.CNFG.REFOUT --------------------------- */


#define CRC32_CNFG__REFOUT__Pos                                 1                                                                      /*!< Right-most bit position of REFOUT in CRC32.CNFG */
#define CRC32_CNFG__REFOUT__Msk                                 (0x00000001UL << CRC32_CNFG__REFOUT__Pos)                              /*!< Mask for REFOUT in CRC32.CNFG */
#define CRC32_CNFG__REFOUT__Shift(v)                            (((v) << CRC32_CNFG__REFOUT__Pos) & CRC32_CNFG__REFOUT__Msk)           /*!< Shift value 'v' into REFOUT and mask 'v' to fit it into REFOUT field */

/** CRC32_CNFG__REFOUT__SET() sets CNFG.REFOUT to given value.
 *
 * When writing to CNFG.REFOUT, this is recommended to use.
 *
 * @param bf_value new value for CNFG.REFOUT
 */
__CRC32_INLINE void CRC32_CNFG__REFOUT__SET(uint32_t bf_value)
{
    uint32_t reg = CRC32_CNFG__GET();
    reg = (reg & ~CRC32_CNFG__REFOUT__Msk);
    reg = (reg | (CRC32_CNFG__REFOUT__Shift((uint32_t)bf_value)));
    CRC32_CNFG__SET(reg);
}

/** CRC32_CNFG__REFOUT__GET() gets CNFG.REFOUT's current value.
 *
 * When reading from CNFG.REFOUT, this is recommended to use.
 *
 * @return current value of CNFG.REFOUT
 */
__CRC32_INLINE uint32_t CRC32_CNFG__REFOUT__GET(void)
{
    uint32_t reg = CRC32_CNFG__GET();
    reg = (reg & CRC32_CNFG__REFOUT__Msk);
    reg = (reg >> CRC32_CNFG__REFOUT__Pos);
    return (uint32_t)reg;
}

/** CRC32_CNFG__REFOUT__MODIFY() modifies CNFG.REFOUT.
 *
 * This is recommended to use when setting more than one bitfield of CRC32.CNFG.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of CRC32.CNFG
 * @param bf_value new value for CNFG.REFOUT
 * @return new value of CRC32.CNFG
 */
__CRC32_INLINE uint32_t CRC32_CNFG__REFOUT__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~CRC32_CNFG__REFOUT__Msk);
    reg = (reg | (CRC32_CNFG__REFOUT__Shift((uint32_t)bf_value)));
    return reg;
}

/** CRC32_CNFG__REFOUT__EXTRACT() extracts CNFG.REFOUT.
 *
 * This is recommended to use when extracting more than one bitfield from CRC32.CNFG.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of CRC32.CNFG
 * @return current value of CNFG.REFOUT
 */
__CRC32_INLINE uint32_t CRC32_CNFG__REFOUT__EXTRACT(uint32_t reg)
{
    reg = (reg & CRC32_CNFG__REFOUT__Msk);
    reg = (reg >> CRC32_CNFG__REFOUT__Pos);
    return (uint32_t)reg;
}

/* ---------------------------- CRC32.CNFG.REFIN ---------------------------- */


#define CRC32_CNFG__REFIN__Pos                                  0                                                                      /*!< Right-most bit position of REFIN in CRC32.CNFG */
#define CRC32_CNFG__REFIN__Msk                                  (0x00000001UL << CRC32_CNFG__REFIN__Pos)                               /*!< Mask for REFIN in CRC32.CNFG */
#define CRC32_CNFG__REFIN__Shift(v)                             (((v) << CRC32_CNFG__REFIN__Pos) & CRC32_CNFG__REFIN__Msk)             /*!< Shift value 'v' into REFIN and mask 'v' to fit it into REFIN field */

/** CRC32_CNFG__REFIN__SET() sets CNFG.REFIN to given value.
 *
 * When writing to CNFG.REFIN, this is recommended to use.
 *
 * @param bf_value new value for CNFG.REFIN
 */
__CRC32_INLINE void CRC32_CNFG__REFIN__SET(uint32_t bf_value)
{
    uint32_t reg = CRC32_CNFG__GET();
    reg = (reg & ~CRC32_CNFG__REFIN__Msk);
    reg = (reg | (CRC32_CNFG__REFIN__Shift((uint32_t)bf_value)));
    CRC32_CNFG__SET(reg);
}

/** CRC32_CNFG__REFIN__GET() gets CNFG.REFIN's current value.
 *
 * When reading from CNFG.REFIN, this is recommended to use.
 *
 * @return current value of CNFG.REFIN
 */
__CRC32_INLINE uint32_t CRC32_CNFG__REFIN__GET(void)
{
    uint32_t reg = CRC32_CNFG__GET();
    reg = (reg & CRC32_CNFG__REFIN__Msk);
    reg = (reg >> CRC32_CNFG__REFIN__Pos);
    return (uint32_t)reg;
}

/** CRC32_CNFG__REFIN__MODIFY() modifies CNFG.REFIN.
 *
 * This is recommended to use when setting more than one bitfield of CRC32.CNFG.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of CRC32.CNFG
 * @param bf_value new value for CNFG.REFIN
 * @return new value of CRC32.CNFG
 */
__CRC32_INLINE uint32_t CRC32_CNFG__REFIN__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~CRC32_CNFG__REFIN__Msk);
    reg = (reg | (CRC32_CNFG__REFIN__Shift((uint32_t)bf_value)));
    return reg;
}

/** CRC32_CNFG__REFIN__EXTRACT() extracts CNFG.REFIN.
 *
 * This is recommended to use when extracting more than one bitfield from CRC32.CNFG.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of CRC32.CNFG
 * @return current value of CNFG.REFIN
 */
__CRC32_INLINE uint32_t CRC32_CNFG__REFIN__EXTRACT(uint32_t reg)
{
    reg = (reg & CRC32_CNFG__REFIN__Msk);
    reg = (reg >> CRC32_CNFG__REFIN__Pos);
    return (uint32_t)reg;
}

/** @} */ /* End of group crc32 block */


/** @} */ /* End of group SHASTA */

/** @} */ /* End of group Infineon */

#endif /* _SHASTA_HAL_CRC32_H_ */
