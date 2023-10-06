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
 * @file    dma_drv.h
 * @brief   DMA driver header file
 */

#ifndef DMA_DRV_H
#define DMA_DRV_H

#include <stdint.h>         // uint*_t
#include "driver_common.h" // cmsis definitions
#include "hal_dma.h"


/* ============================================================================
 * Public data types
 * ========================================================================= */
/** Version definitions for the DMA Module */
#define DMA_API_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,02) // API version
#define DMA_DRV_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,02) // API version

/** Capabilities \p 1 indicates that the dma driver is available */
typedef struct _DMA_CAPABILITIES_s
{
    uint32_t DMA_CAPABLE;
} DMA_CAPABILITIES;


/** dma_ChIRQHandler defines the call back function for the channel specific DMA
 *  Interrupt
 */
typedef  void (*DMA_ChIRQHandler) (void);


/** DMA_CONFIGURATION defines the characteristics of a data transfer for a specific channel */
typedef struct _DMA_CONFIGURATION_s
{
    /** Data width of data 8, 16 or 32 bytes to transfer */
    DMA_Ch_Cfg_Data_Width_Enum_t data_width;

    /** is source address incremented after each transfer
     *  1 .. yes
     *  0 .. no */
    uint8_t is_src_port_inc;
    /** is destination address incremented after each transfer
     *  1 .. yes
     *  0 .. no */
    uint8_t is_dest_port_inc;

    /** Transfer 2**r_pow number of items per request */
    uint8_t r_pow;

    /** DMA transfer type e.g ping pong*/
    Ch_Cfg_Cycle_Ctrl_Enum_t cycle_control;

    /** Source pointer */
    uint32_t * src;

    /** Destination pointer */
    uint32_t * dest;

    /** Number of items to transfer */
    uint32_t size_m1;

    /** Channel specific IRQ Handler */
    DMA_ChIRQHandler dma_ChIRQ;

} DMA_CONFIGURATION;

/** To indicate the status of the dma */

typedef enum _DMA_STATUS_e
{
    /** dma is not in use, it is uninitialized */
    DMA_STATUS_FREE=0,
    /** dma is use means, it is initialized */
    DMA_STATUS_BUSY
} DMA_STATUS;

/** DMA Channel Enumerator */
/*lint -parent(uint32_t, DMA_Channel_e) -strong(AJX)*/
typedef enum _DMA_Channel_e
{
    /* External Requests */
    SREQ0 = 0,                       /*!< External Request 0                       */
    SREQ1 = 1,                       /*!< External Request 1                       */
    SREQ2 = 2,                       /*!< External Request 2                       */
    SREQ3 = 3,                       /*!< External Request 3                       */
    SREQ4 = 4,                       /*!< External Request 4                       */
    SREQ5 = 5,                       /*!< External Request 5                       */
    /* Internal Requests */
    DTIMER1_REQ1 = 6,                /*!< DTIMER1_1 Request                        */
    DTIMER1_REQ2 = 7,                /*!< DTIMER1_2 Request                        */
    DTIMER3_REQ1 = 8,                /*!< DTIMER3_2 Request                        */
    DTIMER3_REQ2 = 9,                /*!< DTIMER3_2 Request                        */
    I2C_TX       = 10,               /*!< PMBUS Module transmit to master Request  */
    I2C_RX       = 11,               /*!< PMBUS Module receive from master Request */
    SSP_TX       = 12,               /*!< SPI TX Request                           */
    SSP_RX       = 13,               /*!< SPI RX Request                           */
    UART_TX      = 14,               /*!< UART RX Request                          */
    UART_RX      = 15               /*!< UART TX Request                          */

} DMA_Channel_e;
/* ============================================================================
 * Function declarations
 * ========================================================================= */


/** DMA_enable_hw() enables the required clock signals for the DMA peripheral
 * and takes the DMA HW out of reset.
 *
 * The sequence of register accesses is important:
 * - first, enable the clock to the DMA, the DMA wrapper and the AHB2APB bridge
 * - then, take the DMA out of reset
 * .
 * See also @ref sctShasta_Ref_CPUS_ITS,  fig 6-1
 */
void        DMA_enable_hw(void);

/** dma_disable_hw() disables the clock signals for the DMA peripheral
 * and puts the DMA HW into reset.
 *
 * The sequence of register accesses is important and reverted compared to
 * dma_enable_hw():
 * - put the DMA into reset
 * - then, disable the clock to the DMA, the DMA wrapper and the AHB2APB bridge
 * .
 * See also @ref sctShasta_Ref_CPUS_ITS,  fig 6-1
 */
void        DMA_disable_hw(void);

/** dma_start() enables the DMA peripheral and activates the given descriptor.
 * 'descriptor' is the short word for a 'channel control data structure'.
 * See DMA_ChCtrl_t for SHASTA' way of defining a descriptor.
 *
 * @param desc DMA descriptor, 0-15 for a primary descriptor and 16-31 for
 * an alternate descriptor.
 */
void        DMA_start(uint32_t desc);

/** dma_stop() deactivates the given descriptor.
 * 'descriptor' is the short word for a 'channel control data structure'.
 * Note that dma_stop() does not disable the DMA controller itself, since it does
 * not have the overview about @b all current descriptors and consequently does
 * not know whether there is still an active one.
 *
 * See DMA_ChCtrl_t for SHASTA' way of declaring a descriptor.
 *
 * @param desc DMA descriptor, 0-15 for a primary descriptor and 16-31 for
 * an alternate descriptor.
 */

void        DMA_stop(uint32_t desc);

/** dma_get_dest_idx() returns the write index into the destination
 * buffer for the given descriptor. In case the given descriptor
 * is inactive (see Ch_Cfg_Cycle_Ctrl_Enum_t::STOP) and
 * has been consumed completely, -1 is returned.
 *
 * The DMA itself uses the write index to calculate the next address in the
 * destination buffer it will write to. Therefore, module-level code can use
 * it to derive the idx for recently written data.
 *
 * @note dma_get_dest_idx() is working with volatile data: In case the DMA
 * is doing a transfer @b while this code is executed, you may obtain stale
 * data, depending on whether the CPU is using the descriptor before or after
 * the DMA has been updating it.
 *
 * @param desc DMA descriptor, 0-15 for a primary descriptor and 16-31 for
 * an alternate descriptor.
 * @return write index into the destination buffer, -1 if given descriptor is
 * is inactive and has been consumed completely.
 */
int32_t    DMA_get_dest_idx(uint32_t desc);

/** dma_get_cycle_ctrl() returns the current operating mode of the given
 * descriptor. It is used for figuring out whether a descriptor is active.
 *
 * @param desc DMA descriptor, 0-15 for a primary descriptor and 16-31 for
 * an alternate descriptor.
 * @return 0 in case descriptor is invalid, 1 in case it is still valid.
 */
uint32_t    DMA_get_cycle_ctrl(uint32_t desc);


/** dma_get_cur_desc_type() returns an indication whether DMA uses the
 * primary or the alternate descriptor for the given channel.
 *
 * @param ch DMA channel, 0-15
 * @return 0 in case the primary descriptor is currently being used for ch, non-zero in case the
 * alternate descriptor is currently being used.
 */
uint32_t    DMA_get_cur_desc_type(DMA_Channel_e ch);

/** DMA_is_active() check whether any channel of the DMA is in use.
 *
 * @return 0 DMA not in use an can be disabled.
 *         1 DMA in use
 */
uint32_t DMA_is_active (void);

/** DMA_is_other_ch_active() check whether any channel of the DMA is in use.
 *
 * @param ch DMA channel, 0-15
 * @return 0 DMA not in use an can be disabled.
 *         1 DMA in use
 */

uint32_t DMA_is_other_ch_active (DMA_Channel_e ch);

/** DMA_setup configures channel ch to transfer data from source to destination
 *
 * @param ch channel
 * @param ch_cfg Pointer to the channel configuration
 */
void DMA_setup(DMA_Channel_e ch, const DMA_CONFIGURATION ch_cfg);

/** DMA_teardown() deconfigures channel ch.
 * @param ch channel
 *
 */
void        DMA_teardown(DMA_Channel_e ch);


/** DMA_req_sw() is a helper function to generate a DMA request for
 * a given channel.
 *
 * @param ch DMA channel for which to generate a SW request.
 */
void        DMA_req_sw(DMA_Channel_e ch);

/**
 * DMA_GetVersion retrieves the version of the dma drv and api
 *
 *  @return DRIVER_VERSION
 */
DRIVER_VERSION DMA_GetVersion (void);

/**
 * DMA_GetCapabilities retrieves the Capabilities of the dma
 *  @param none
 *
 *  @return DMA_CAPABILITIES is just \p 1
 */
DMA_CAPABILITIES DMA_GetCapabilities (void);


/** DMA API definitions */
typedef struct _DRIVER_DMA {
  DRIVER_VERSION   (*GetVersion)            (void);
  DMA_CAPABILITIES     (*GetCapabilities)       (void);
  void                 (*enable_hw)             (void);
  void                 (*disable_hw)            (void);
  void                 (*start)                 (uint32_t desc);
  void                 (*stop)                  (uint32_t desc);
  int32_t              (*get_dest_idx)          (uint32_t desc);
  uint32_t             (*get_cycle_ctrl)        (uint32_t desc);
  uint32_t             (*get_cur_desc_type)     (DMA_Channel_e ch);
  void                 (*setup)                 (DMA_Channel_e ch, const DMA_CONFIGURATION ch_cfg);
  void                 (*teardown)              (DMA_Channel_e ch);
  void                 (*req_sw)                (DMA_Channel_e ch);
} const DRIVER_DMA;


extern DRIVER_DMA Driver_DMA;

#endif /* DMA_DRV_H */
