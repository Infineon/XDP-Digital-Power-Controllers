/**
 * @file     shasta_hal_vsen.h
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


#ifndef _SHASTA_HAL_VSEN_H_
#define _SHASTA_HAL_VSEN_H_

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


/** @addtogroup vsen
 * @{
 */

#if defined ( __CC_ARM )
#define __VSEN_INLINE  static __inline __attribute__((always_inline))
#elif defined ( __GNUC__ )
#define __VSEN_INLINE  static inline __attribute__((always_inline))
#elif defined ( _MSC_VER )
#define __VSEN_INLINE  static __forceinline
#else
#warning Unsupported Compiler
#define __VSEN_INLINE  static inline
#endif

/** register file of the vsen block.
 */
typedef struct _VSEN_s
{
    volatile        uint32_t        VSP1                        ; /*!< (@0x00000000) Verrn clamp threshold at PID input where verrn is defined as the control voltage minus the sensed output voltage.  Ignoring droop,  Verrn = VOUT_COMMAND*VOU...(more) */
    volatile        uint32_t        VSP2                        ; /*!< (@0x00000004) When high, holds the previous output from the Voltage Sense ADC.  This may be used by FW to stall but not zero out the ADC output for further down stream pr...(more) */
    volatile        uint32_t        VSP3                        ; /*!< (@0x00000008) Not used */
    volatile        uint32_t        VSP4                        ; /*!< (@0x0000000C) Open sense protection fault threshold. */
    volatile        uint32_t        VSP5                        ; /*!< (@0x00000010) Gain and offset trimmed voltage sense ADC output result. */
} VSEN_t;


/** enumeration of implemented vsen blocks
 */
typedef enum _vsen_idx_e
{
    VSEN0 = 0u,    //!< VSEN0
    VSEN1 = 1u,    //!< VSEN1
    VSEN2 = 2u     //!< VSEN2
} vsen_idx_t;

#define VSEN_BASE (0x70000800u)                                                          /*!< vsen base address */
#define VSEN_STRIDE (0x00000400u)                                                         

/** __VSEN_BASE_PTR returns the base address of the peripheral.
 *
 * @param idp index of peripheral
 * @return base address of peripheral's instance
 */
__VSEN_INLINE uint32_t* __VSEN_BASE_PTR(vsen_idx_t idp)
{
    return (uint32_t*)(VSEN_BASE + (uint32_t)((uint32_t)idp * VSEN_STRIDE));
}


/* -------------------------------------------------------------------------- */
/*                           VSP1 member of VSEN_t                           */
/* -------------------------------------------------------------------------- */

/** __VSEN_VSP1__ADDRESS returns the address of member VSP1.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @param idp index of peripheral
 * @return address of VSP1
 */
__VSEN_INLINE uint32_t* __VSEN_VSP1__ADDRESS(vsen_idx_t idp)
{
    return (uint32_t*)((uint32_t)__VSEN_BASE_PTR(idp) + (uint32_t)(offsetof(VSEN_t, VSP1)));
}

/** VSEN_VSP1__SET() sets VSP1 to given value.
 *
 * When writing to VSP1, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @param value new value for VSP1
 */
__VSEN_INLINE void VSEN_VSP1__SET(vsen_idx_t idp, uint32_t value)
{
    HAL_SET32(__VSEN_VSP1__ADDRESS(idp), value);
}

/** VSEN_VSP1__GET() gets VSP1's current value.
 *
 * When reading from VSP1, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP1
 */
__VSEN_INLINE uint32_t VSEN_VSP1__GET(vsen_idx_t idp)
{
    return HAL_GET32(__VSEN_VSP1__ADDRESS(idp));
}

/* ----------------------- VSEN.VSP1.VSP_COUNT_FE_CMP ----------------------- */

/**
<pre>
  VSEN.VSP1.VSP_COUNT_FE_CMP:

    Defines the number of consecutive voltage sense front end stress compensation (FESC) cycles with the comparator in the same polarity required to increment or decrement the compensation DAC.  In general, the FESC settings will be defined by Infineon are recommended to be left unchanged.
    Cycles = vsp_count_fe_cmp + 1

</pre> */

#define VSEN_VSP1__VSP_COUNT_FE_CMP__Pos                        18                                                                     /*!< Right-most bit position of vsp_count_fe_cmp in VSEN.VSP1 */
#define VSEN_VSP1__VSP_COUNT_FE_CMP__Msk                        (0x0000001FUL << VSEN_VSP1__VSP_COUNT_FE_CMP__Pos)                     /*!< Mask for vsp_count_fe_cmp in VSEN.VSP1 */
#define VSEN_VSP1__VSP_COUNT_FE_CMP__Shift(v)                   (((v) << VSEN_VSP1__VSP_COUNT_FE_CMP__Pos) & VSEN_VSP1__VSP_COUNT_FE_CMP__Msk) /*!< Shift value 'v' into vsp_count_fe_cmp and mask 'v' to fit it into vsp_count_fe_cmp field */

/** VSEN_VSP1__VSP_COUNT_FE_CMP__SET() sets VSP1.VSP_COUNT_FE_CMP to given value.
 *
 * When writing to VSP1.VSP_COUNT_FE_CMP, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP1.VSP_COUNT_FE_CMP
 */
__VSEN_INLINE void VSEN_VSP1__VSP_COUNT_FE_CMP__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & ~VSEN_VSP1__VSP_COUNT_FE_CMP__Msk);
    reg = (reg | (VSEN_VSP1__VSP_COUNT_FE_CMP__Shift((uint32_t)bf_value)));
    VSEN_VSP1__SET(idp, reg);
}

/** VSEN_VSP1__VSP_COUNT_FE_CMP__GET() gets VSP1.VSP_COUNT_FE_CMP's current value.
 *
 * When reading from VSP1.VSP_COUNT_FE_CMP, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP1.VSP_COUNT_FE_CMP
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_COUNT_FE_CMP__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & VSEN_VSP1__VSP_COUNT_FE_CMP__Msk);
    reg = (reg >> VSEN_VSP1__VSP_COUNT_FE_CMP__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP1__VSP_COUNT_FE_CMP__MODIFY() modifies VSP1.VSP_COUNT_FE_CMP.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP1.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP1
 * @param bf_value new value for VSP1.VSP_COUNT_FE_CMP
 * @return new value of VSEN.VSP1
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_COUNT_FE_CMP__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP1__VSP_COUNT_FE_CMP__Msk);
    reg = (reg | (VSEN_VSP1__VSP_COUNT_FE_CMP__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP1__VSP_COUNT_FE_CMP__EXTRACT() extracts VSP1.VSP_COUNT_FE_CMP.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP1.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP1
 * @return current value of VSP1.VSP_COUNT_FE_CMP
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_COUNT_FE_CMP__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP1__VSP_COUNT_FE_CMP__Msk);
    reg = (reg >> VSEN_VSP1__VSP_COUNT_FE_CMP__Pos);
    return (uint32_t)reg;
}

/* ---------------------- VSEN.VSP1.VSP_MODE_FE_1P25M ---------------------- */

/**
<pre>
  VSEN.VSP1.VSP_MODE_FE_1P25M:

    Defines voltage sense front end stress compensation (FESC) DAC LSB weight.  In general, the FESC settings will be defined by Infineon are recommended to be left unchanged.
    <start table>
    0 "0.625 mV"
    1 "1.250 mV"
    <end table>

</pre> */

#define VSEN_VSP1__VSP_MODE_FE_1P25M__Pos                       17                                                                     /*!< Right-most bit position of vsp_mode_fe_1p25m in VSEN.VSP1 */
#define VSEN_VSP1__VSP_MODE_FE_1P25M__Msk                       (0x00000001UL << VSEN_VSP1__VSP_MODE_FE_1P25M__Pos)                    /*!< Mask for vsp_mode_fe_1p25m in VSEN.VSP1 */
#define VSEN_VSP1__VSP_MODE_FE_1P25M__Shift(v)                  (((v) << VSEN_VSP1__VSP_MODE_FE_1P25M__Pos) & VSEN_VSP1__VSP_MODE_FE_1P25M__Msk) /*!< Shift value 'v' into vsp_mode_fe_1p25m and mask 'v' to fit it into vsp_mode_fe_1p25m field */

/** VSEN_VSP1__VSP_MODE_FE_1P25M__SET() sets VSP1.VSP_MODE_FE_1P25M to given value.
 *
 * When writing to VSP1.VSP_MODE_FE_1P25M, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP1.VSP_MODE_FE_1P25M
 */
__VSEN_INLINE void VSEN_VSP1__VSP_MODE_FE_1P25M__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & ~VSEN_VSP1__VSP_MODE_FE_1P25M__Msk);
    reg = (reg | (VSEN_VSP1__VSP_MODE_FE_1P25M__Shift((uint32_t)bf_value)));
    VSEN_VSP1__SET(idp, reg);
}

/** VSEN_VSP1__VSP_MODE_FE_1P25M__GET() gets VSP1.VSP_MODE_FE_1P25M's current value.
 *
 * When reading from VSP1.VSP_MODE_FE_1P25M, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP1.VSP_MODE_FE_1P25M
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_MODE_FE_1P25M__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & VSEN_VSP1__VSP_MODE_FE_1P25M__Msk);
    reg = (reg >> VSEN_VSP1__VSP_MODE_FE_1P25M__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP1__VSP_MODE_FE_1P25M__MODIFY() modifies VSP1.VSP_MODE_FE_1P25M.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP1.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP1
 * @param bf_value new value for VSP1.VSP_MODE_FE_1P25M
 * @return new value of VSEN.VSP1
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_MODE_FE_1P25M__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP1__VSP_MODE_FE_1P25M__Msk);
    reg = (reg | (VSEN_VSP1__VSP_MODE_FE_1P25M__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP1__VSP_MODE_FE_1P25M__EXTRACT() extracts VSP1.VSP_MODE_FE_1P25M.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP1.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP1
 * @return current value of VSP1.VSP_MODE_FE_1P25M
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_MODE_FE_1P25M__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP1__VSP_MODE_FE_1P25M__Msk);
    reg = (reg >> VSEN_VSP1__VSP_MODE_FE_1P25M__Pos);
    return (uint32_t)reg;
}

/* ----------------------- VSEN.VSP1.VSP_FE_COUNT_THR ----------------------- */

/**
<pre>
  VSEN.VSP1.VSP_FE_COUNT_THR:

    Defines variable part of voltage sense front end stress compensation (FESC) period.   In general, the FESC settings will be defined by Infineon are recommended to be left unchanged.
    LSB = 10ns, Range = 0 to 10230ns

</pre> */

#define VSEN_VSP1__VSP_FE_COUNT_THR__Pos                        7                                                                      /*!< Right-most bit position of vsp_fe_count_thr in VSEN.VSP1 */
#define VSEN_VSP1__VSP_FE_COUNT_THR__Msk                        (0x000003FFUL << VSEN_VSP1__VSP_FE_COUNT_THR__Pos)                     /*!< Mask for vsp_fe_count_thr in VSEN.VSP1 */
#define VSEN_VSP1__VSP_FE_COUNT_THR__Shift(v)                   (((v) << VSEN_VSP1__VSP_FE_COUNT_THR__Pos) & VSEN_VSP1__VSP_FE_COUNT_THR__Msk) /*!< Shift value 'v' into vsp_fe_count_thr and mask 'v' to fit it into vsp_fe_count_thr field */

/** VSEN_VSP1__VSP_FE_COUNT_THR__SET() sets VSP1.VSP_FE_COUNT_THR to given value.
 *
 * When writing to VSP1.VSP_FE_COUNT_THR, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP1.VSP_FE_COUNT_THR
 */
__VSEN_INLINE void VSEN_VSP1__VSP_FE_COUNT_THR__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & ~VSEN_VSP1__VSP_FE_COUNT_THR__Msk);
    reg = (reg | (VSEN_VSP1__VSP_FE_COUNT_THR__Shift((uint32_t)bf_value)));
    VSEN_VSP1__SET(idp, reg);
}

/** VSEN_VSP1__VSP_FE_COUNT_THR__GET() gets VSP1.VSP_FE_COUNT_THR's current value.
 *
 * When reading from VSP1.VSP_FE_COUNT_THR, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP1.VSP_FE_COUNT_THR
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_FE_COUNT_THR__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & VSEN_VSP1__VSP_FE_COUNT_THR__Msk);
    reg = (reg >> VSEN_VSP1__VSP_FE_COUNT_THR__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP1__VSP_FE_COUNT_THR__MODIFY() modifies VSP1.VSP_FE_COUNT_THR.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP1.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP1
 * @param bf_value new value for VSP1.VSP_FE_COUNT_THR
 * @return new value of VSEN.VSP1
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_FE_COUNT_THR__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP1__VSP_FE_COUNT_THR__Msk);
    reg = (reg | (VSEN_VSP1__VSP_FE_COUNT_THR__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP1__VSP_FE_COUNT_THR__EXTRACT() extracts VSP1.VSP_FE_COUNT_THR.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP1.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP1
 * @return current value of VSP1.VSP_FE_COUNT_THR
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_FE_COUNT_THR__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP1__VSP_FE_COUNT_THR__Msk);
    reg = (reg >> VSEN_VSP1__VSP_FE_COUNT_THR__Pos);
    return (uint32_t)reg;
}

/* ---------------------- VSEN.VSP1.VSP_EN_FE_COMP_LP ---------------------- */

/**
<pre>
  VSEN.VSP1.VSP_EN_FE_COMP_LP:

    Voltage sense front end stress compensation (FESC) enable.   The FESC circuit is used to reduce long term offset drift at the input to the voltage sense ADC.  In general, the FESC settings will be defined by Infineon are recommended to be left unchanged.
    <start table>
    0 "disabled"
    1 "enabled"
    <end table>

</pre> */

#define VSEN_VSP1__VSP_EN_FE_COMP_LP__Pos                       6                                                                      /*!< Right-most bit position of vsp_en_fe_comp_lp in VSEN.VSP1 */
#define VSEN_VSP1__VSP_EN_FE_COMP_LP__Msk                       (0x00000001UL << VSEN_VSP1__VSP_EN_FE_COMP_LP__Pos)                    /*!< Mask for vsp_en_fe_comp_lp in VSEN.VSP1 */
#define VSEN_VSP1__VSP_EN_FE_COMP_LP__Shift(v)                  (((v) << VSEN_VSP1__VSP_EN_FE_COMP_LP__Pos) & VSEN_VSP1__VSP_EN_FE_COMP_LP__Msk) /*!< Shift value 'v' into vsp_en_fe_comp_lp and mask 'v' to fit it into vsp_en_fe_comp_lp field */

/** VSEN_VSP1__VSP_EN_FE_COMP_LP__SET() sets VSP1.VSP_EN_FE_COMP_LP to given value.
 *
 * When writing to VSP1.VSP_EN_FE_COMP_LP, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP1.VSP_EN_FE_COMP_LP
 */
__VSEN_INLINE void VSEN_VSP1__VSP_EN_FE_COMP_LP__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & ~VSEN_VSP1__VSP_EN_FE_COMP_LP__Msk);
    reg = (reg | (VSEN_VSP1__VSP_EN_FE_COMP_LP__Shift((uint32_t)bf_value)));
    VSEN_VSP1__SET(idp, reg);
}

/** VSEN_VSP1__VSP_EN_FE_COMP_LP__GET() gets VSP1.VSP_EN_FE_COMP_LP's current value.
 *
 * When reading from VSP1.VSP_EN_FE_COMP_LP, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP1.VSP_EN_FE_COMP_LP
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_EN_FE_COMP_LP__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & VSEN_VSP1__VSP_EN_FE_COMP_LP__Msk);
    reg = (reg >> VSEN_VSP1__VSP_EN_FE_COMP_LP__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP1__VSP_EN_FE_COMP_LP__MODIFY() modifies VSP1.VSP_EN_FE_COMP_LP.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP1.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP1
 * @param bf_value new value for VSP1.VSP_EN_FE_COMP_LP
 * @return new value of VSEN.VSP1
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_EN_FE_COMP_LP__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP1__VSP_EN_FE_COMP_LP__Msk);
    reg = (reg | (VSEN_VSP1__VSP_EN_FE_COMP_LP__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP1__VSP_EN_FE_COMP_LP__EXTRACT() extracts VSP1.VSP_EN_FE_COMP_LP.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP1.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP1
 * @return current value of VSP1.VSP_EN_FE_COMP_LP
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_EN_FE_COMP_LP__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP1__VSP_EN_FE_COMP_LP__Msk);
    reg = (reg >> VSEN_VSP1__VSP_EN_FE_COMP_LP__Pos);
    return (uint32_t)reg;
}

/* -------------------- VSEN.VSP1.VSP_VERRN_LARGE_THRESH -------------------- */

/**
<pre>
  VSEN.VSP1.VSP_VERRN_LARGE_THRESH:

    Not used

</pre> */

#define VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Pos                  3                                                                      /*!< Right-most bit position of vsp_verrn_large_thresh in VSEN.VSP1 */
#define VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Msk                  (0x00000007UL << VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Pos)               /*!< Mask for vsp_verrn_large_thresh in VSEN.VSP1 */
#define VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Shift(v)             (((v) << VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Pos) & VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Msk) /*!< Shift value 'v' into vsp_verrn_large_thresh and mask 'v' to fit it into vsp_verrn_large_thresh field */

/** VSEN_VSP1__VSP_VERRN_LARGE_THRESH__SET() sets VSP1.VSP_VERRN_LARGE_THRESH to given value.
 *
 * When writing to VSP1.VSP_VERRN_LARGE_THRESH, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP1.VSP_VERRN_LARGE_THRESH
 */
__VSEN_INLINE void VSEN_VSP1__VSP_VERRN_LARGE_THRESH__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & ~VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Msk);
    reg = (reg | (VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Shift((uint32_t)bf_value)));
    VSEN_VSP1__SET(idp, reg);
}

/** VSEN_VSP1__VSP_VERRN_LARGE_THRESH__GET() gets VSP1.VSP_VERRN_LARGE_THRESH's current value.
 *
 * When reading from VSP1.VSP_VERRN_LARGE_THRESH, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP1.VSP_VERRN_LARGE_THRESH
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_VERRN_LARGE_THRESH__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Msk);
    reg = (reg >> VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP1__VSP_VERRN_LARGE_THRESH__MODIFY() modifies VSP1.VSP_VERRN_LARGE_THRESH.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP1.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP1
 * @param bf_value new value for VSP1.VSP_VERRN_LARGE_THRESH
 * @return new value of VSEN.VSP1
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_VERRN_LARGE_THRESH__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Msk);
    reg = (reg | (VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP1__VSP_VERRN_LARGE_THRESH__EXTRACT() extracts VSP1.VSP_VERRN_LARGE_THRESH.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP1.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP1
 * @return current value of VSP1.VSP_VERRN_LARGE_THRESH
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_VERRN_LARGE_THRESH__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Msk);
    reg = (reg >> VSEN_VSP1__VSP_VERRN_LARGE_THRESH__Pos);
    return (uint32_t)reg;
}

/* -------------------- VSEN.VSP1.VSP_VERRN_CLAMP_THRESH -------------------- */

/**
<pre>
  VSEN.VSP1.VSP_VERRN_CLAMP_THRESH:

    Verrn clamp threshold at PID input where verrn is defined as the control voltage minus the sensed output voltage.  Ignoring droop,
    Verrn = VOUT_COMMAND*VOUT_SCALE_LOOP - V(VSEN)
    <start table_with_header>
    [2:0] "Clamp Range" [2:0] "Clamp Range"
    0 "-40 to 40 mV" 4 "-200 to 200mV"
    1 "-80 to 80 mV" 5 "-240 to 240mV"
    2 "-120 to 120 mV" 6 "-280 to 280mV"
    3 "-160 to 160 mV" 4 "-318.75 to 318.75mV"
    <end table>

</pre> */

#define VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Pos                  0                                                                      /*!< Right-most bit position of vsp_verrn_clamp_thresh in VSEN.VSP1 */
#define VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Msk                  (0x00000007UL << VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Pos)               /*!< Mask for vsp_verrn_clamp_thresh in VSEN.VSP1 */
#define VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Shift(v)             (((v) << VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Pos) & VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Msk) /*!< Shift value 'v' into vsp_verrn_clamp_thresh and mask 'v' to fit it into vsp_verrn_clamp_thresh field */

/** VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__SET() sets VSP1.VSP_VERRN_CLAMP_THRESH to given value.
 *
 * When writing to VSP1.VSP_VERRN_CLAMP_THRESH, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP1.VSP_VERRN_CLAMP_THRESH
 */
__VSEN_INLINE void VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & ~VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Msk);
    reg = (reg | (VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Shift((uint32_t)bf_value)));
    VSEN_VSP1__SET(idp, reg);
}

/** VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__GET() gets VSP1.VSP_VERRN_CLAMP_THRESH's current value.
 *
 * When reading from VSP1.VSP_VERRN_CLAMP_THRESH, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP1.VSP_VERRN_CLAMP_THRESH
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP1__GET(idp);
    reg = (reg & VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Msk);
    reg = (reg >> VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__MODIFY() modifies VSP1.VSP_VERRN_CLAMP_THRESH.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP1.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP1
 * @param bf_value new value for VSP1.VSP_VERRN_CLAMP_THRESH
 * @return new value of VSEN.VSP1
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Msk);
    reg = (reg | (VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__EXTRACT() extracts VSP1.VSP_VERRN_CLAMP_THRESH.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP1.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP1
 * @return current value of VSP1.VSP_VERRN_CLAMP_THRESH
 */
__VSEN_INLINE uint32_t VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Msk);
    reg = (reg >> VSEN_VSP1__VSP_VERRN_CLAMP_THRESH__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           VSP2 member of VSEN_t                           */
/* -------------------------------------------------------------------------- */

/** __VSEN_VSP2__ADDRESS returns the address of member VSP2.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @param idp index of peripheral
 * @return address of VSP2
 */
__VSEN_INLINE uint32_t* __VSEN_VSP2__ADDRESS(vsen_idx_t idp)
{
    return (uint32_t*)((uint32_t)__VSEN_BASE_PTR(idp) + (uint32_t)(offsetof(VSEN_t, VSP2)));
}

/** VSEN_VSP2__SET() sets VSP2 to given value.
 *
 * When writing to VSP2, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @param value new value for VSP2
 */
__VSEN_INLINE void VSEN_VSP2__SET(vsen_idx_t idp, uint32_t value)
{
    HAL_SET32(__VSEN_VSP2__ADDRESS(idp), value);
}

/** VSEN_VSP2__GET() gets VSP2's current value.
 *
 * When reading from VSP2, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP2
 */
__VSEN_INLINE uint32_t VSEN_VSP2__GET(vsen_idx_t idp)
{
    return HAL_GET32(__VSEN_VSP2__ADDRESS(idp));
}

/* ------------------------ VSEN.VSP2.VSP_ADC_BLANK ------------------------ */

/**
<pre>
  VSEN.VSP2.VSP_ADC_BLANK:

    When high, holds the previous output from the Voltage Sense ADC.  This may be used by FW to stall but not zero out the ADC output for further down stream processing.

</pre> */

#define VSEN_VSP2__VSP_ADC_BLANK__Pos                           0                                                                      /*!< Right-most bit position of vsp_adc_blank in VSEN.VSP2 */
#define VSEN_VSP2__VSP_ADC_BLANK__Msk                           (0x00000001UL << VSEN_VSP2__VSP_ADC_BLANK__Pos)                        /*!< Mask for vsp_adc_blank in VSEN.VSP2 */
#define VSEN_VSP2__VSP_ADC_BLANK__Shift(v)                      (((v) << VSEN_VSP2__VSP_ADC_BLANK__Pos) & VSEN_VSP2__VSP_ADC_BLANK__Msk) /*!< Shift value 'v' into vsp_adc_blank and mask 'v' to fit it into vsp_adc_blank field */

/** VSEN_VSP2__VSP_ADC_BLANK__SET() sets VSP2.VSP_ADC_BLANK to given value.
 *
 * When writing to VSP2.VSP_ADC_BLANK, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP2.VSP_ADC_BLANK
 */
__VSEN_INLINE void VSEN_VSP2__VSP_ADC_BLANK__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP2__GET(idp);
    reg = (reg & ~VSEN_VSP2__VSP_ADC_BLANK__Msk);
    reg = (reg | (VSEN_VSP2__VSP_ADC_BLANK__Shift((uint32_t)bf_value)));
    VSEN_VSP2__SET(idp, reg);
}

/** VSEN_VSP2__VSP_ADC_BLANK__GET() gets VSP2.VSP_ADC_BLANK's current value.
 *
 * When reading from VSP2.VSP_ADC_BLANK, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP2.VSP_ADC_BLANK
 */
__VSEN_INLINE uint32_t VSEN_VSP2__VSP_ADC_BLANK__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP2__GET(idp);
    reg = (reg & VSEN_VSP2__VSP_ADC_BLANK__Msk);
    reg = (reg >> VSEN_VSP2__VSP_ADC_BLANK__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP2__VSP_ADC_BLANK__MODIFY() modifies VSP2.VSP_ADC_BLANK.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP2.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP2
 * @param bf_value new value for VSP2.VSP_ADC_BLANK
 * @return new value of VSEN.VSP2
 */
__VSEN_INLINE uint32_t VSEN_VSP2__VSP_ADC_BLANK__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP2__VSP_ADC_BLANK__Msk);
    reg = (reg | (VSEN_VSP2__VSP_ADC_BLANK__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP2__VSP_ADC_BLANK__EXTRACT() extracts VSP2.VSP_ADC_BLANK.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP2.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP2
 * @return current value of VSP2.VSP_ADC_BLANK
 */
__VSEN_INLINE uint32_t VSEN_VSP2__VSP_ADC_BLANK__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP2__VSP_ADC_BLANK__Msk);
    reg = (reg >> VSEN_VSP2__VSP_ADC_BLANK__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           VSP3 member of VSEN_t                           */
/* -------------------------------------------------------------------------- */

/** __VSEN_VSP3__ADDRESS returns the address of member VSP3.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @param idp index of peripheral
 * @return address of VSP3
 */
__VSEN_INLINE uint32_t* __VSEN_VSP3__ADDRESS(vsen_idx_t idp)
{
    return (uint32_t*)((uint32_t)__VSEN_BASE_PTR(idp) + (uint32_t)(offsetof(VSEN_t, VSP3)));
}

/** VSEN_VSP3__SET() sets VSP3 to given value.
 *
 * When writing to VSP3, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @param value new value for VSP3
 */
__VSEN_INLINE void VSEN_VSP3__SET(vsen_idx_t idp, uint32_t value)
{
    HAL_SET32(__VSEN_VSP3__ADDRESS(idp), value);
}

/** VSEN_VSP3__GET() gets VSP3's current value.
 *
 * When reading from VSP3, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP3
 */
__VSEN_INLINE uint32_t VSEN_VSP3__GET(vsen_idx_t idp)
{
    return HAL_GET32(__VSEN_VSP3__ADDRESS(idp));
}

/* -------------------------- VSEN.VSP3.VSP_OSP_EN -------------------------- */

/**
<pre>
  VSEN.VSP3.VSP_OSP_EN:

    Not used

</pre> */

#define VSEN_VSP3__VSP_OSP_EN__Pos                              0                                                                      /*!< Right-most bit position of vsp_osp_en in VSEN.VSP3 */
#define VSEN_VSP3__VSP_OSP_EN__Msk                              (0x00000001UL << VSEN_VSP3__VSP_OSP_EN__Pos)                           /*!< Mask for vsp_osp_en in VSEN.VSP3 */
#define VSEN_VSP3__VSP_OSP_EN__Shift(v)                         (((v) << VSEN_VSP3__VSP_OSP_EN__Pos) & VSEN_VSP3__VSP_OSP_EN__Msk)     /*!< Shift value 'v' into vsp_osp_en and mask 'v' to fit it into vsp_osp_en field */

/** VSEN_VSP3__VSP_OSP_EN__SET() sets VSP3.VSP_OSP_EN to given value.
 *
 * When writing to VSP3.VSP_OSP_EN, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP3.VSP_OSP_EN
 */
__VSEN_INLINE void VSEN_VSP3__VSP_OSP_EN__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP3__GET(idp);
    reg = (reg & ~VSEN_VSP3__VSP_OSP_EN__Msk);
    reg = (reg | (VSEN_VSP3__VSP_OSP_EN__Shift((uint32_t)bf_value)));
    VSEN_VSP3__SET(idp, reg);
}

/** VSEN_VSP3__VSP_OSP_EN__GET() gets VSP3.VSP_OSP_EN's current value.
 *
 * When reading from VSP3.VSP_OSP_EN, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP3.VSP_OSP_EN
 */
__VSEN_INLINE uint32_t VSEN_VSP3__VSP_OSP_EN__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP3__GET(idp);
    reg = (reg & VSEN_VSP3__VSP_OSP_EN__Msk);
    reg = (reg >> VSEN_VSP3__VSP_OSP_EN__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP3__VSP_OSP_EN__MODIFY() modifies VSP3.VSP_OSP_EN.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP3.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP3
 * @param bf_value new value for VSP3.VSP_OSP_EN
 * @return new value of VSEN.VSP3
 */
__VSEN_INLINE uint32_t VSEN_VSP3__VSP_OSP_EN__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP3__VSP_OSP_EN__Msk);
    reg = (reg | (VSEN_VSP3__VSP_OSP_EN__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP3__VSP_OSP_EN__EXTRACT() extracts VSP3.VSP_OSP_EN.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP3.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP3
 * @return current value of VSP3.VSP_OSP_EN
 */
__VSEN_INLINE uint32_t VSEN_VSP3__VSP_OSP_EN__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP3__VSP_OSP_EN__Msk);
    reg = (reg >> VSEN_VSP3__VSP_OSP_EN__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           VSP4 member of VSEN_t                           */
/* -------------------------------------------------------------------------- */

/** __VSEN_VSP4__ADDRESS returns the address of member VSP4.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @param idp index of peripheral
 * @return address of VSP4
 */
__VSEN_INLINE uint32_t* __VSEN_VSP4__ADDRESS(vsen_idx_t idp)
{
    return (uint32_t*)((uint32_t)__VSEN_BASE_PTR(idp) + (uint32_t)(offsetof(VSEN_t, VSP4)));
}

/** VSEN_VSP4__SET() sets VSP4 to given value.
 *
 * When writing to VSP4, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @param value new value for VSP4
 */
__VSEN_INLINE void VSEN_VSP4__SET(vsen_idx_t idp, uint32_t value)
{
    HAL_SET32(__VSEN_VSP4__ADDRESS(idp), value);
}

/** VSEN_VSP4__GET() gets VSP4's current value.
 *
 * When reading from VSP4, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP4
 */
__VSEN_INLINE uint32_t VSEN_VSP4__GET(vsen_idx_t idp)
{
    return HAL_GET32(__VSEN_VSP4__ADDRESS(idp));
}

/* ------------------------ VSEN.VSP4.VSP_OSP_THRESH ------------------------ */

/**
<pre>
  VSEN.VSP4.VSP_OSP_THRESH:

    Open sense protection fault threshold.

</pre> */

#define VSEN_VSP4__VSP_OSP_THRESH__Pos                          0                                                                      /*!< Right-most bit position of vsp_osp_thresh in VSEN.VSP4 */
#define VSEN_VSP4__VSP_OSP_THRESH__Msk                          (0x00000FFFUL << VSEN_VSP4__VSP_OSP_THRESH__Pos)                       /*!< Mask for vsp_osp_thresh in VSEN.VSP4 */
#define VSEN_VSP4__VSP_OSP_THRESH__Shift(v)                     (((v) << VSEN_VSP4__VSP_OSP_THRESH__Pos) & VSEN_VSP4__VSP_OSP_THRESH__Msk) /*!< Shift value 'v' into vsp_osp_thresh and mask 'v' to fit it into vsp_osp_thresh field */

/** VSEN_VSP4__VSP_OSP_THRESH__SET() sets VSP4.VSP_OSP_THRESH to given value.
 *
 * When writing to VSP4.VSP_OSP_THRESH, this is recommended to use.
 *
 * @param idp index of peripheral
 * @param bf_value new value for VSP4.VSP_OSP_THRESH
 */
__VSEN_INLINE void VSEN_VSP4__VSP_OSP_THRESH__SET(vsen_idx_t idp, uint32_t bf_value)
{
    uint32_t reg = VSEN_VSP4__GET(idp);
    reg = (reg & ~VSEN_VSP4__VSP_OSP_THRESH__Msk);
    reg = (reg | (VSEN_VSP4__VSP_OSP_THRESH__Shift((uint32_t)bf_value)));
    VSEN_VSP4__SET(idp, reg);
}

/** VSEN_VSP4__VSP_OSP_THRESH__GET() gets VSP4.VSP_OSP_THRESH's current value.
 *
 * When reading from VSP4.VSP_OSP_THRESH, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP4.VSP_OSP_THRESH
 */
__VSEN_INLINE uint32_t VSEN_VSP4__VSP_OSP_THRESH__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP4__GET(idp);
    reg = (reg & VSEN_VSP4__VSP_OSP_THRESH__Msk);
    reg = (reg >> VSEN_VSP4__VSP_OSP_THRESH__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP4__VSP_OSP_THRESH__MODIFY() modifies VSP4.VSP_OSP_THRESH.
 *
 * This is recommended to use when setting more than one bitfield of VSEN.VSP4.
 * Multiple subsequent xxx__MODIFY inlines are aggregated into one
 * by the compiler's optimization stage.
 *
 * @param reg current value of VSEN.VSP4
 * @param bf_value new value for VSP4.VSP_OSP_THRESH
 * @return new value of VSEN.VSP4
 */
__VSEN_INLINE uint32_t VSEN_VSP4__VSP_OSP_THRESH__MODIFY(uint32_t reg, uint32_t bf_value)
{
    reg = (reg & ~VSEN_VSP4__VSP_OSP_THRESH__Msk);
    reg = (reg | (VSEN_VSP4__VSP_OSP_THRESH__Shift((uint32_t)bf_value)));
    return reg;
}

/** VSEN_VSP4__VSP_OSP_THRESH__EXTRACT() extracts VSP4.VSP_OSP_THRESH.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP4.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP4
 * @return current value of VSP4.VSP_OSP_THRESH
 */
__VSEN_INLINE uint32_t VSEN_VSP4__VSP_OSP_THRESH__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP4__VSP_OSP_THRESH__Msk);
    reg = (reg >> VSEN_VSP4__VSP_OSP_THRESH__Pos);
    return (uint32_t)reg;
}

/* -------------------------------------------------------------------------- */
/*                           VSP5 member of VSEN_t                           */
/* -------------------------------------------------------------------------- */

/** __VSEN_VSP5__ADDRESS returns the address of member VSP5.
 *
 * As indicated by the __ prefix, this is not intended for use by the FW. It is
 * here for readability only.
 *
 * @param idp index of peripheral
 * @return address of VSP5
 */
__VSEN_INLINE uint32_t* __VSEN_VSP5__ADDRESS(vsen_idx_t idp)
{
    return (uint32_t*)((uint32_t)__VSEN_BASE_PTR(idp) + (uint32_t)(offsetof(VSEN_t, VSP5)));
}

/** VSEN_VSP5__GET() gets VSP5's current value.
 *
 * When reading from VSP5, this is mandatory to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP5
 */
__VSEN_INLINE uint32_t VSEN_VSP5__GET(vsen_idx_t idp)
{
    return HAL_GET32(__VSEN_VSP5__ADDRESS(idp));
}

/* ------------------------- VSEN.VSP5.VSP_VOUT_FS ------------------------- */

/**
<pre>
  VSEN.VSP5.VSP_VOUT_FS:

    Gain and offset trimmed voltage sense ADC output result.

</pre> */

#define VSEN_VSP5__VSP_VOUT_FS__Pos                             0                                                                      /*!< Right-most bit position of vsp_vout_fs in VSEN.VSP5 */
#define VSEN_VSP5__VSP_VOUT_FS__Msk                             (0x00000FFFUL << VSEN_VSP5__VSP_VOUT_FS__Pos)                          /*!< Mask for vsp_vout_fs in VSEN.VSP5 */
#define VSEN_VSP5__VSP_VOUT_FS__Shift(v)                        (((v) << VSEN_VSP5__VSP_VOUT_FS__Pos) & VSEN_VSP5__VSP_VOUT_FS__Msk)   /*!< Shift value 'v' into vsp_vout_fs and mask 'v' to fit it into vsp_vout_fs field */

/** VSEN_VSP5__VSP_VOUT_FS__GET() gets VSP5.VSP_VOUT_FS's current value.
 *
 * When reading from VSP5.VSP_VOUT_FS, this is recommended to use.
 *
 * @param idp index of peripheral
 * @return current value of VSP5.VSP_VOUT_FS
 */
__VSEN_INLINE uint32_t VSEN_VSP5__VSP_VOUT_FS__GET(vsen_idx_t idp)
{
    uint32_t reg = VSEN_VSP5__GET(idp);
    reg = (reg & VSEN_VSP5__VSP_VOUT_FS__Msk);
    reg = (reg >> VSEN_VSP5__VSP_VOUT_FS__Pos);
    return (uint32_t)reg;
}

/** VSEN_VSP5__VSP_VOUT_FS__EXTRACT() extracts VSP5.VSP_VOUT_FS.
 *
 * This is recommended to use when extracting more than one bitfield from VSEN.VSP5.
 * It avoids multiple time-consuming bus accesses.
 *
 * @param reg current value of VSEN.VSP5
 * @return current value of VSP5.VSP_VOUT_FS
 */
__VSEN_INLINE uint32_t VSEN_VSP5__VSP_VOUT_FS__EXTRACT(uint32_t reg)
{
    reg = (reg & VSEN_VSP5__VSP_VOUT_FS__Msk);
    reg = (reg >> VSEN_VSP5__VSP_VOUT_FS__Pos);
    return (uint32_t)reg;
}

/** @} */ /* End of group vsen block */


/** @} */ /* End of group SHASTA */

/** @} */ /* End of group Infineon */

#endif /* _SHASTA_HAL_VSEN_H_ */