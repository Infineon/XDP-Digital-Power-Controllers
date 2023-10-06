/* ============================================================================
** Copyright (C) 2015 Infineon Technologies AG
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
 * @file    hal_dma.h
 * @brief   HW abstraction layer for ARM's PL230 uDMA, private for DMA driver source files.
 */

#ifndef HAL_DMA_H
#define HAL_DMA_H

/// Maximum Number of DMA channels
#define DMA_CHANNELS_MAX (16)

/** In the official ARM PL230 uDMA documentation, DMA_ChCtrl_t is also called the
 * 'Channel Control Data Structure', see https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/03_System/CPUS.pdf chap 8.2.2.
 *
 *  I tried to make the whole typedef as much as CMSIS compliant as possible,
 *  the only deviation is that this implementation uses an unused uint32_t
 *  word in the data structure to keep a (constant) copy of the 'control data
 *  configuration'.
 */
typedef struct {
    /** pointer to location where DMA reads from.
     */
    uint32_t*           src_data_end;
    /** pointer to location where DMA writes to.
     */
    uint32_t*           dest_data_end;
    /** current 'control data configuration', for meaning of
     * each bitfield see @ref sctOpus_Ref_CPUS_ITS, figure 8-8.
     * and respective macros and enums in hal_dma.h.
     *
     * Note that the DMA changes cur_config while it is servicing
     * DMA requests - it needs to be volatile.
     */
    volatile uint32_t   cur_config;
    /** shadow copy of cur_config.
     */
    uint32_t            config;
} DMA_ChCtrl_t;


/** operating mode of the DMA cycle
 */
typedef enum DMA_Ch_Cfg_Cycle_Ctrl_Enum_e
{
  STOP = 0,                   //!< STOP: this descriptor is invalid
  BASIC = 1,                  /*!< BASIC: DMA must receive a new request,
                                   prior to it entering the arbitration
                                   process, to enable the DMA cycle to complete
                               */
  AUTO_REQUEST = 2,           /*!< AUTO_REQUEST: DMATautomatically inserts a
                                   request for the appropriate channel during the
                                   arbitration process. This means that the
                                   initial request is sufficient to enable
                                   the DMA cycle to complete.
                               */
  PING_PONG = 3,              /*!< PING_PONG: The controller performs a DMA cycle
                                   using one of the data structures. After the
                                   DMA cycle completes, it performs a DMA cycle
                                   using the other data structure. After the DMA
                                   cycle completes and provided that the host
                                   processor has updated the original data structure,
                                   it performs a DMA cycle using the original data structure.
                                   DMA continues to perform DMA cycles until it either reads
                                   an invalid data structure or the host processor
                                   changes the cycle_ctrl bits to 001B or 010B
                               */
  MEM_SCATTER_GATHER_PRIM = 4,//!< MEM_SCATTER_GATHER_PRIM
  MEM_SCATTER_GATHER_ALT = 5, //!< MEM_SCATTER_GATHER_ALT
  PER_SCATTER_GATHER_PRIM = 6,//!< PER_SCATTER_GATHER_PRIM
  PER_SCATTER_GATHER_ALT = 7  //!< PER_SCATTER_GATHER_ALT
} Ch_Cfg_Cycle_Ctrl_Enum_t;

#define DMA_CFG_CYCLE_CTRL_SET_Pos      0    //*!< Right-most bit position of Ch_Cfg_Cycle_Ctrl_Enum_t in ::DMA_ChCtrl_t::config */
#define DMA_CFG_CYCLE_CTRL_SET_Msk      (0x7UL << DMA_CFG_CYCLE_CTRL_SET_Pos)   /*!< Mask for DMA_CFG_CYCLE_CTRL_SET_Pos in ::DMA_ChCtrl_t::config */
#define DMA_CFG_CYCLE_CTRL_SET_Shift(v) (((v) << DMA_CFG_CYCLE_CTRL_SET_Pos) & DMA_CFG_CYCLE_CTRL_SET_Msk) /*!< Shift value 'v' into ::DMA_ChCtrl_t::config and mask 'v' to fit it into ::DMA_ChCtrl_t::config */


#define DMA_CFG_N_MINUS_1_SET_Pos           4    /*!< Right-most bit position of 'n_minus_1' (total transfers per DMA cycle) in ::DMA_ChCtrl_t::config */
#define DMA_CFG_N_MINUS_1_SET_Msk           (0x3FFUL << DMA_CFG_N_MINUS_1_SET_Pos)   /*!< Mask for DMA_CFG_SRC_DATA_SIZE_SET_Pos in ::DMA_ChCtrl_t::config */
#define DMA_CFG_N_MINUS_1_SET_Shift(v)      (((v) << DMA_CFG_N_MINUS_1_SET_Pos) & DMA_CFG_N_MINUS_1_SET_Msk) /*!< Shift value 'v' into ::DMA_ChCtrl_t::config and mask 'v' to fit it into ::DMA_ChCtrl_t::config */

#define DMA_CFG_R_POWER_SET_Pos           14    /*!< Right-most bit position of 'R_power' (arbitration rate) in ::DMA_ChCtrl_t::config */
#define DMA_CFG_R_POWER_SET_Msk           (0xFUL << DMA_CFG_R_POWER_SET_Pos)   /*!< Mask for DMA_CFG_SRC_DATA_SIZE_SET_Pos in ::DMA_ChCtrl_t::config */
#define DMA_CFG_R_POWER_SET_Shift(v)      (((v) << DMA_CFG_R_POWER_SET_Pos) & DMA_CFG_R_POWER_SET_Msk) /*!< Shift value 'v' into ::DMA_ChCtrl_t::config and mask 'v' to fit it into ::DMA_ChCtrl_t::config */


/** data transfer size and address increment mode
 */
typedef enum DMA_Ch_Cfg_Data_Width_Enum_e
{
    DATA_WIDTH_BYTE = 0,    //!< DATA_WIDTH_BYTE: byte transfer, byte address increment
    DATA_WIDTH_HALFWORD = 1,//!< DATA_WIDTH_HALFWORD: 16bit transfer, 16bit address increment
    DATA_WIDTH_WORD = 2,    //!< DATA_WIDTH_WORD: 32bit transfer, 32bit address increment
    DATA_WIDTH_NO_INCR = 3  /*!< DATA_WIDTH_NO_INCR: no increment of address
                             */
} DMA_Ch_Cfg_Data_Width_Enum_t;

#define DMA_CFG_SRC_DATA_SIZE_SET_Pos       24    /*!< Right-most bit position of source data size (::DMA_Ch_Cfg_Data_Width_Enum_t) in ::DMA_ChCtrl_t::config */
#define DMA_CFG_SRC_DATA_SIZE_SET_Msk       (0x3UL << DMA_CFG_SRC_DATA_SIZE_SET_Pos)   /*!< Mask for DMA_CFG_SRC_DATA_SIZE_SET_Pos in ::DMA_ChCtrl_t::config */
#define DMA_CFG_SRC_DATA_SIZE_SET_Shift(v)  (((v) << DMA_CFG_SRC_DATA_SIZE_SET_Pos) & DMA_CFG_SRC_DATA_SIZE_SET_Msk) /*!< Shift value 'v' into ::DMA_ChCtrl_t::config and mask 'v' to fit it into ::DMA_ChCtrl_t::config */

#define DMA_CFG_SRC_INC_SET_Pos             26    /*!< Right-most bit position of source addr increment (::DMA_Ch_Cfg_Data_Width_Enum_t) in ::DMA_ChCtrl_t::config */
#define DMA_CFG_SRC_INC_SET_Msk             (0x3UL << DMA_CFG_SRC_INC_SET_Pos)   /*!< Mask for DMA_CFG_SRC_DATA_SIZE_SET_Pos in ::DMA_ChCtrl_t::config */
#define DMA_CFG_SRC_INC_SET_Shift(v)        (((v) << DMA_CFG_SRC_INC_SET_Pos) & DMA_CFG_SRC_INC_SET_Msk) /*!< Shift value 'v' into ::DMA_ChCtrl_t::config and mask 'v' to fit it into ::DMA_ChCtrl_t::config */

#define DMA_CFG_DST_DATA_SIZE_SET_Pos       28    /*!< Right-most bit position of dest data size (::DMA_Ch_Cfg_Data_Width_Enum_t) in ::DMA_ChCtrl_t::config */
#define DMA_CFG_DST_DATA_SIZE_SET_Msk       (0x3UL << DMA_CFG_DST_DATA_SIZE_SET_Pos)   /*!< Mask for DMA_CFG_SRC_DATA_SIZE_SET_Pos in ::DMA_ChCtrl_t::config */
#define DMA_CFG_DST_DATA_SIZE_SET_Shift(v)  (((v) << DMA_CFG_DST_DATA_SIZE_SET_Pos) & DMA_CFG_DST_DATA_SIZE_SET_Msk) /*!< Shift value 'v' into ::DMA_ChCtrl_t::config and mask 'v' to fit it into ::DMA_ChCtrl_t::config */

#define DMA_CFG_DST_INC_SET_Pos             30    /*!< Right-most bit position of dest data increment (::DMA_Ch_Cfg_Data_Width_Enum_t) in ::DMA_ChCtrl_t::config */
#define DMA_CFG_DST_INC_SET_Msk             (0x3UL << DMA_CFG_DST_INC_SET_Pos)   /*!< Mask for DMA_CFG_SRC_DATA_SIZE_SET_Pos in ::DMA_ChCtrl_t::config */
#define DMA_CFG_DST_INC_SET_Shift(v)        (((v) << DMA_CFG_DST_INC_SET_Pos) & DMA_CFG_DST_INC_SET_Msk) /*!< Shift value 'v' into ::DMA_ChCtrl_t::config and mask 'v' to fit it into ::DMA_ChCtrl_t::config */


#endif /* HAL_DMA_H */

