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
 * @file    configurator.h
 * @brief   Public declarations and interface of the application of the
 *          configurator FW Module. This module is used to copy/save
 *          configuration parameters from/to OTP memory.
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_

/* Data types */
#include <stdint.h>            /* uint*_t */
#include "fw_bool.h"           /* boolean type */
#include "fw_params.h"         /* OTP definitions */

/* ============================================================================
 * Public data types
 * ========================================================================= */

/** To indicate the status of the configurator
 */

typedef enum CONFIGURATOR_STATUS_e
{
    /** configurator is not in use, it is uninitialized */
    CONFIGURATOR_STATUS_FREE = 0,
    /** configurator is busy means data transfer from/to OTP is ongoing.*/
    CONFIGURATOR_STATUS_BUSY
} CONFIGURATOR_STATUS_t;

/** To indicate the errors of the configurator
 */

typedef enum CONFIGURATOR_ERROR_e
{
    /** No error */
    CONFIGURATOR_NO_FAULT = 0,
    /** Read from OTP was not successful */
    CONFIGURATOR_READ_FAULT = 1,
    /** write to OTP was not successful */
    CONFIGURATOR_WRITE_FAULT = 2
} CONFIGURATOR_ERROR_t;

/* ============================================================================
 * PMBus Command Parameter
 * ========================================================================= */

/* ============================================================================
 * Function declarations
 * ========================================================================= */

/**
 * Function to set the internal xvalent value
 *
 *  @param   xv 1st byte: xvalent index for full configurations 0 - 15.
 *              2nd byte: xvalent index for partial register configurations 0 - 15.
 *              3rd byte: xvalent index for partial pmbus configurations 0 - 15.
 */
void configurator_xv_set(uint32_t xv);

/**
 * Function to get the internal xvalent value
 *
 *  @return   xvalent value.
 *            1st byte: xvalent index for full configurations 0 - 15.
 *            2nd byte: xvalent index for partial register configurations 0 - 15.
 *            3rd byte: xvalent index for partial pmbus configurations 0 - 15.
 */
uint32_t configurator_xv_get(void);

/** The configurator_boot_default function is used to initialize the FW at startup,
 * using xvalent = 0
 */

void configurator_boot_default(void);

/** The configurator_boot_xvalent function is used to initialize the FW at startup,
 * based on the current xvalent setting
 *
 *  @param   xv  xvalent value 0 - 15.
 */

void configurator_boot_xvalent(uint8_t xv);


/**
 * reads trim data from OTP and stores it into the respective registers.
 * Copy current configurations from OTP to the respective registers.
 * *
 * @throws see otp_fs_scan(), otp_fs_hdr_load().
 */
void configurator_trim_registers_load(void);

/**
 * reads trim data from the registers and stores it into OTP.
 *
 * @throws see otp_fs_scan(), otp_fs_hdr_load().
 */
void configurator_trim_registers_store(void);

/**
 * reads configuration data from OTP and stores it into registers.
 * *
 * @throws see otp_fs_scan(), otp_fs_hdr_load().
 */
void configurator_conf_registers_load(void);

/**
 * reads configuration data from the registers and stores it into OTP.
 *
 *  @param   xv  xvalent value 0 - 15.
 * @throws see otp_fs_scan(), otp_fs_hdr_load().
 */
void configurator_conf_registers_store(uint8_t xv);


/** The configurator_pmbus_default_set function is used to retrieve pointers to PMBUS data in OTP
 *
 * @throws CORRUPT_MEMORY      if the data can not be retrieved from the OTP.
 * @throws OUT_OF_RESOURCE from otp_fs_scan.
 * @throws A @p handler specific exception.
 * @throws ILLEGAL_PARAMETER from otp_fs_scan.
 * @throws TIMEOUT from otp_fs_scan.
 *
 * @param  loop 0 or 1.
 *
 */
void configurator_pmbus_default_set(uint8_t loop);


/** The configurator_pmbus_user_set function is used to retrieve pointers to PMBUS data in OTP
 *
 * @throws CORRUPT_MEMORY      if the data can not be retrieved from the OTP.
 * @throws OUT_OF_RESOURCE from otp_fs_scan.
 * @throws A @p handler specific exception.
 * @throws ILLEGAL_PARAMETER from otp_fs_scan.
 * @throws TIMEOUT from otp_fs_scan.
 *
 * @param  loop 0 or 1.
 *
 */
void configurator_pmbus_user_set(uint8_t loop);

/** The configurator_pmbus_partial_set function is used to retrieve a pointers to the
 *  partial pmbus configurations in OTP
 *
 */

void configurator_pmbus_partial_set(void);

/** The configurator_pmbus_default_ptr_get function is used to retrieve a pointer to
 * Default PMBUS settings in OTP for a respective loop
 *  @param  loop 0 or 1.
 *
 *  @return pointer to pmbus default data in OTP.
 */
uint8_t* configurator_pmbus_default_ptr_get(uint8_t loop);

/** The configurator_pmbus_default_size_get function is used to retrieve the size of the
 * Default PMBUS settings in OTP for a respective loop
 *
 * The size is multiple of 4 bytes so it could be up to 3 bytes larger than the actual size.
 *
 *  @param  loop 0 or 1.
 *
 *  @return size in bytes.
 *
 */
uint16_t configurator_pmbus_default_size_get(uint8_t loop);

/** The configurator_pmbus_user_ptr_get function is used to retrieve a pointer to
 * User PMBUS settings in OTP for a respective loop.
 *
 *  @param  loop 0 or 1.
 *
 *  @return pointer to pmbus user data in OTP.
 */
uint8_t* configurator_pmbus_user_ptr_get(uint8_t loop);

/** The configurator_pmbus_user_size_get function is used to retrieve the size of the
 * User PMBUS settings in OTP for a respective loop.
 *
 * The size is multiple of 4 bytes so it could be up to 3 bytes larger than the actual size.
 *
 *  @param  loop 0 or 1.
 *
 *  @return size in bytes.
 *
 */
uint16_t configurator_pmbus_user_size_get(uint8_t loop);

/** The configurator_pmbus_default_store function is used to store default data to OTP
 *
 *  @param  loop 0 or 1.
 *  @param  ptr pointer to the respective data.
 *  @param  size of data in bytes.
 *
 */
void configurator_pmbus_default_store(uint8_t loop, const uint8_t* ptr, uint16_t size);

/** The configurator_pmbus_user_store function is used to store user data to OTP
 *
 *  @param  loop 0 or 1.
 *  @param  xv  xvalent parameter 0 - 15.
 *  @param  ptr pointer to the respective data.
 *  @param  size size of data in bytes.
 *
 */
void configurator_pmbus_user_store(uint8_t loop, uint8_t xv, const uint8_t* ptr, uint16_t size);

/** The configurator_pmbus_partial_ptr_get function is used to retrieve a pointer to
 * partial PMBUS cmd settings in OTP for a respective loop.
 *  @param  loop 0 or 1.
 *  @param  cmd 0 - 255.
 *
 *  @return pointer to pmbus partial PMBUS data in OTP.
 */
uint8_t* configurator_pmbus_partial_ptr_get(uint8_t loop, uint8_t cmd);

/** The configurator_pmbus_partial_store function is used to store a single pmbus command configuration.
 *
 *  @param  lp loop 0 or 1
 *  @param  cmd PMBUS command code 0 - 255
 *  @param  xv_type xvalent type 0 ... 15
 *  @param ptr points to the pmbus data to store
 *  @param size gives the number of bytes to store 0 - 31
 *
 */

void configurator_pmbus_partial_store (uint8_t lp, uint8_t cmd,  uint8_t xv_type, const uint8_t * ptr, uint16_t size);

/** The configurator_conf_partial_store function is used to store a partial configuration data.
 *
 *  @param  xv_type xvalent type 0 ... 15
 *  @param ptr points to the configuration data to store
 *  @param size gives the number of bytes to store 0 - 31
 *
 */
void configurator_conf_partial_store (uint8_t xv_type, const uint8_t * ptr, uint16_t size);

/** The configurator_conf_partial_load function is used to retrieve data from OTP
 * and store it in registers starting at address 0x7000_0000.
 *
 * @throws CORRUPT_MEMORY      if the data can not be retrieved from the OTP.
 * @throws OUT_OF_RESOURCE from otp_fs_scan.
 * @throws A @p handler specific exception.
 * @throws ILLEGAL_PARAMETER from otp_fs_scan.
 * @throws TIMEOUT from otp_fs_scan.
 *
 */

void configurator_conf_partial_load(void);

/** The configurator_status_get retrieves the status of the configurator.
 *
 *  @return  status of the configurator.
 *              either free or busy.
 *
 */
CONFIGURATOR_STATUS_t configurator_status_get(void);

/** The configurator_error_get function retrieves the errors caused by accessing the OTP.
 *
 *  @return  status of the configurator.
 *
 */
CONFIGURATOR_ERROR_t configurator_error_get(void);

/** The configurator_error_clear function clears the errors in the cofigurator.
 *
 *  @return  status of the configurator.
 *
 */
void configurator_error_clear(void);

/** The configurator_remaining_otp_size_get function retrieves the remaining OTP size
 * for configurations in bytes.
 *
 * @param partition Parameter to indicate used OTP partition.
 *
 *  @return  remaining size in bytes.
 *
 */
uint32_t configurator_remaining_otp_size_get(Otp_Partition_Selector_t partition);

/** This function returns the pointer to the next available 32 bit aligned position in memory.
 * The remaining bytes are filled with zeroes in case of a
 * store operation.
 *
 * @param ptr                  Pointer to the data
 * @param size_ptr             Pointer to the current size in bits
 * @param operation            Type of operation
 *                             0   write to registers, load operation
 *                             1   write to data-buffer, store operation
 *
 * @return      pointer to next free 32 bit aligned position in the data array
 *
 */

uint8_t* configurator_ptr_align_and_zero_fill(uint8_t* ptr, uint32_t* size_ptr, uint32_t operation);

#endif /* CONFIGURATOR_H_ */
