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
 * @file    patch_app.h
 * @brief   API of the thread-level part of the Patch FW Module.
 */

#ifndef PATCH_APP_H
#define PATCH_APP_H

/* all include files, starting with the mandatory fw_config.h. */
#include "fw_config.h"
#include "fw_params.h" // OTP sections
#include "otp_fs.h" // OTP sections

/* Data types */
#include <stdint.h>     // uint*_t


/* ============================================================================
 * API functions
 * ========================================================================= */

/**
 * Initializes and configures the patch FW module.
 *
 * Following operations are performed
 *
 * - configure any required HW peripheras
 * - search and executes patches
 *
 */
void patch_init(void);


/**
 * The function patch_otp_process scans the OTP partition for an existing patch and executes it.
 * All patches which are set to invalid are ignored.
 *
 * This function could either process a full image patch (after loading the CPU is reset) or a
 * partial patch which is loaded and executed without reset. <br>
 * Depending on the the @ref Otp_File_System_Cmd_Hdr_t::option settings either multiple
 * patches or only a single patch can be loaded from a partition.
 *
 * @param partition Corresponding OTP partition
 *
 * @throws OUT_OF_RESOURCE from otp_fs_scan
 * @throws ILLEGAL_PARAMETER from otp_fs_scan.
 * @throws TIMEOUT from otp_fs_scan.
 * @throws CORRUPT_MEMORY from otp_fs_scan
 */
void patch_otp_process(Otp_Partition_Selector_t partition);


/**
 * The patch_ram_excecute allows to execute a function from ram. The function is
 * compiled and re-mapped to the the otp partition. Thus it can be later stored directly to OTP
 *
 * @param partition Pointer to corresponding OTP partition.
 * @param cmd_hdr_ptr Pointer to the ram location where the patch is located
 */
void patch_ram_execute(Otp_Partition_Selector_t partition,  Otp_File_System_Cmd_Hdr_t* cmd_hdr_ptr);

/**
 * The function patch_otp_remap is used to map a patch to the beginning of a patch partition
 *
 * @param partition Pointer to corresponding OTP partition
 * @param hdr is a pointer to the actual patch position
 *
 */
void patch_otp_remap(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Hdr_t * cmd_hdr);

#endif /* PATCH_APP_H */
