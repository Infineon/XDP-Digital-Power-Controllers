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
 * @file    sys_boot_descr_format.h
 * @brief   Boot descriptor format description.
 */

#ifndef SYS_BOOT_DESCR_FORMAT_H
#define SYS_BOOT_DESCR_FORMAT_H

/* Data types */
#include <stdint.h>     // uint*_t

/* ------------------------------------------------------------------------- */
/* Definition of the Boot descriptor bit values */
/* ------------------------------------------------------------------------- */
/*lint -esym(755, SYS_BD_*)
 * Info: Not referenced
 * Not all boot descriptor modes are used for all image types */

/** Set CPU Subsystem Clock to 25 MHz.
 * The default clock is 100 MHz in case this configuration is not set. */
#define SYS_BD_CLK_SEL_25                          (uint16_t)((uint16_t)1u << 0)

/** Set CPU Subsystem Clock to 50 MHz.
 * The default clock is 100 MHz in case this configuration is not set. */
#define SYS_BD_CLK_SEL_50                          (uint16_t)((uint16_t)2u << 0)

/** Set CPU Subsystem Clock to 50 MHz.
 * The default clock is 100 MHz in case this configuration is not set. */
#define SYS_BD_CLK_SEL_100                         (uint16_t)((uint16_t)3u << 0)

/** Don not update boot descriptor from OTP. */
#define SYS_BD_BOOT_RESIST_ENABLE                  (uint16_t)((uint16_t)1u << 2)

/** Disable retrieval of partial patches from OTP. */
#define SYS_BD_PATCH_DISABLE                       (uint16_t)((uint16_t)1u << 3)

/** Disable read configuration from OTP. */
#define SYS_BD_CONFIG_DISABLE                      (uint16_t)((uint16_t)1u << 4)

/** Enables hosc clock gating for power aware systems. */
#define SYS_BD_SLEEP_ENABLE                        (uint16_t)((uint16_t)1u << 5)

/** Enables BOOT_FAULT handling. */
#define SYS_BD_BOOT_FAULT                          (uint16_t)((uint16_t)1u << 6)

/* ------------------------------------------------------------------------- */

#endif /* SYS_BOOT_DESCR_FORMAT_H */
