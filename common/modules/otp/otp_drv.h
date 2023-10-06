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
 * @file    otp_drv.h
 * @brief   OTP device access layer
 */
#ifndef OTP_DRV_H
#define OTP_DRV_H

/** Version definitions
 *
 * Version History:<br>
 * Version 0.02: Conflicting APB/AHB access handling added
 *  Hardfault handler extended to handle conflict. It is looping
 *  until the conflict is resolved.
 */

/* Compiler configurations first */
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "driver_common.h"  // cmsis definitions

#include "cmsis_os_exceptions.h"       /* CMSIS-RTOS functions, incl. exception handling */
#include "fw_params.h"           //SECTION_LENGTH_OTP_DATA

#define OTP_API_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,02) /* API version */
#define OTP_DRV_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,02) /* API version */

/** To indicate OTP is busy performing an operation
 */

typedef enum OTP_STATUS_BUSY_e
{
    /** OTP is not in use, it is uninitialized */
    OTP_STATUS_FREE=0,
    /** OTP is in use means, a transaction is ongoing */
    OTP_STATUS_OCCUPIED,
} OTP_STATUS_BUSY;

/** To indicate OTP is powered up and ready to perform an operation
 */

typedef enum OTP_STATUS_PWR_e
{
    /** OTP is not powered, and there for not ready to perfrom an operation */
    OTP_STATUS_UNPOWERED=0,
    /** OTP is powered and ready to perform an operation*/
    OTP_STATUS_POWERED,
} OTP_STATUS_PWR;

#define OTP_STATUS_BUF_SIZE 8

/** To indicate the status of the mmu.
 */

typedef struct _OTP_ACCESS_ERROR_s
{
    /** Counts the number of access violations */
    uint8_t cnt;

    /** Last error which caused a violation  */
    uint32_t err_rpt;

} OTP_ACCESS_ERROR_s;

/** OTP Row Size (in Number of Bytes) */
#define OTP_ROW_SIZE (128u / 8u)

/**
 * OTP_GetVersion retrieves the version of the otp drv and api
 *
 *  @return DRIVER_VERSION
 */
DRIVER_VERSION OTP_GetVersion (void);

/**
 * This routine initializes the OTP and installs the respective IRQ handler.
 *
 *
 *  @throws OUT_OF_RESOURCE if the timer is busy.
 *
 *  @return none
 */
void OTP_Initialize (void);

/**
 * OTP_Uninitialize deactivates the OTP if not busy.
 *
 *  @param  none
 *
 *  @return none
 */
void OTP_Uninitialize (void);


/**
 * This function reads the dedicated OTP instance.
 * It allows accessing the OTP cross boundary for any Byte count granularity.
 *
 * The hardware restriction where only whole OTP rows are read or written is
 * encapsulated and hidden by this function.
 *
 * @param section Pointer to corresponding OTP section
 * @param offset Byte offset inside the OTP where to read or write the data
 * @param ptr RAM data pointer where to read the written data, or to write the read data
 * @param num Number of Byte to read or write
 *
 * @throws TIMEOUT.
 */
void OTP_Read(Otp_Partition_Selector_t partition, uint32_t offset, uint8_t** ptr, uint32_t num);

/**
 * This function writes the dedicated OTP instance.
 * It allows accessing the OTP cross boundary for any Byte count granularity.
 *
 * The hardware restriction where only whole OTP rows are read or written is
 * encapsulated and hidden by this function.
 *
 * @param section Pointer to corresponding OTP section
 * @param offset Byte offset inside the OTP where to read or write the data
 * @param ptr RAM data pointer where to read the written data, or to write the read data
 * @param num Number of Byte to read or write
 *
 * @throws TIMEOUT.
 */

void OTP_Write(Otp_Partition_Selector_t partition, uint32_t offset, const uint8_t* ptr, uint32_t num);
/**
 * OTP_ClearIRQ clears the latched interrupt.
 *
 *  @param  none.
 *
 *  @return none
 */
void OTP_ClearIRQ (void);


/**
 * OTP_GetStatusBusy retrieves the operational status of the OTP.
 *
 *  @param  none.
 *
 *  @return OTP_STATUS_BUSY indicates whether the OTP is busy or not.
 */

OTP_STATUS_BUSY OTP_GetStatusBusy (void);

/**
 * OTP_GetStatusPWR retrieves the Power status of the OTP.
 *
 *  @param  none.
 *
 *  @return OTP_STATUS_PWR indicates whether the OTP is powered or not.
 */

OTP_STATUS_PWR OTP_GetStatusPWR (void);

/**
 * OTP_ClearAccessError clears the latched access fault and resets the internal variables.
 *
 *  @param  none.
 *
 *  @return none
 */
void OTP_ClearAccessError (void);


/**
 * OTP_GetAccessError retrieves information which about conflicting AHB/ABP access to OTP.
 *
 *  @param  none
 *
 *  @return OTP_STATUS_ACCESS struct
 */

OTP_ACCESS_ERROR_s OTP_GetAccessError (void);

/**
 * This OTP_Hardfault IRQ handler handles concurrent ABP/AHB access conflicts.
 */
void OTP_Hardfault_IRQHandler(void);

/** OTP API definitions */

typedef struct _DRIVER_OTP{
  DRIVER_VERSION       (*GetVersion)       (void);
  void                 (*Initialize)       (void);
  void                 (*Uninitialize)     (void);
  void                 (*Read)             (Otp_Partition_Selector_t partition, uint32_t offset, uint8_t** ptr, uint32_t num);
  void                 (*Write)            (Otp_Partition_Selector_t partition, uint32_t offset, const uint8_t* ptr, uint32_t num);
  OTP_STATUS_BUSY      (*GetStatusBusy)    (void);
  OTP_STATUS_PWR       (*GetStatusPWR)     (void);
  void                 (*ClearAccessError) (void);
  OTP_ACCESS_ERROR_s   (*GetAccessError)   (void);
} const DRIVER_OTP;

extern DRIVER_OTP Driver_OTP;


#endif /* OTP_DRV_H */
