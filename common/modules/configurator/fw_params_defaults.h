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
 * @file    fw_params_defaults.h
 * @brief   default fw parameter definitions
 */

#ifndef FW_PARAMS_DEFAULT_H
#define FW_PARAMS_DEFAULT_H

/** This parameter reflects the version of the FW Trim parameters
 *  It must match the version parameter in the excel sheet
 */
#define FW_PARAMS_TRIM_VERSION 2

/** This parameter reflects the size of the FW Trim parameters in bytes
 *  Should be updated whenever a parameter is added or removed
 *  It must match the size parameter in the excel sheet
 */
#define FW_PARAMS_TRIM_SIZE 42

/** This parameter reflects the version of the FW configuration parameters
 *  Should be updated whenever a parameter is added or removed
 *  It must match the version parameter in the excel sheet
 */
#define FW_PARAMS_CONFIG_VERSION 1

/** This parameter reflects the size of the FW Trim parameters in bytes
 *  It must match the size parameter in the excel sheet
 */
#define FW_PARAMS_CONFIG_SIZE 8

/** This parameter reflects the default data partition size
 */
#define FW_PARAMS_INITIAL_BOOT_OTP_DATA_LENGTH (0x4000)

/** This parameter reflects the default patch partition size
 */
#define FW_PARAMS_INITIAL_BOOT_OTP_PATCH1_LENGTH (0x4000)

/** This parameter reflects the default patch partition size
 */
#define FW_PARAMS_INITIAL_BOOT_OTP_PATCH2_LENGTH (0x4000)

/** This parameter reflects the default patch partition size
 */
#define FW_PARAMS_INITIAL_BOOT_OTP_PATCH3_LENGTH (0x4000)

/** This parameter reflects the default patch1 partition size
 */
#define FW_PARAMS_INITIAL_BOOT_OTP_PATCH4_LENGTH (0x0)

/** This parameter reflects the default boot descriptor
 */
#define FW_PARAMS_INITIAL_BOOT_DESCRIPTOR 3

/** This parameter reflects the default xvalent setting
 */
#define FW_PARAMS_INITIAL_XV 0


#endif /* FW_PARAMS_DEFAULT_H */
