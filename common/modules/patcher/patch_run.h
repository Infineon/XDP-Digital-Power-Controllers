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
 * @file    patch_run.h
 * @brief   Describing the OTP patch check macro to be placed inside the
 *          routine to be patchable.
 */
#ifndef PATCH_RUN_H
#define PATCH_RUN_H

#include "patch_rom_table.h"

/** Checks the OTP Patch function lookup table for a dedicated patch function.
    This function is executed instead in case it is available in the table. */
#define PATCH_TABLE_RUN(x, y...) \
    if (patch_rom_table.patch_##x != NULL) \
    { \
        return patch_rom_table.patch_##x(y); \
    }

#define VOID_PATCH_TABLE_RUN(x, y...) \
    if (patch_rom_table.patch_##x != NULL) \
    { \
        patch_rom_table.patch_##x(y); \
        return; \
    }

#endif /* PATCH_RUN_H */
