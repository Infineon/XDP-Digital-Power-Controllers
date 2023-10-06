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
 * @file    image_types.h
 * @brief   The firmware build process of Shasta is capable of linking
 *          an image as ROM image, as RAM image, as self contained RAM full patch
 *          or as RAM full patch that is linked against an existing ROM image.
 *
 *          Each firmware project contains a 'fw_config.h' file. This file
 *          must contain a @ref IMAGE_TYPE pre-compiler macro which is
 *          set to one of the supported image types listed below
 *
 *          This header defines pre-compiler macros which are used inside
 *          the source code to enable image type specific code parts.
 */
#ifndef IMAGE_TYPES_H
#define IMAGE_TYPES_H

/* ============================================================================
 * Supported Image location types
 * ========================================================================= */
/**
 * The project specific @ref IMAGE_TYPE macro is set to @ref ROM_IMAGE
 * in case the image shall be stored and executed from the ROM.
 * This means that the code and constant variables
 * are stored in ROM, and the variables, stack and heap are kept in RAM.
 *
 * This definition is used inside the source code to include ROM code related
 * initialization parts that need to be included during the build.
 */
#define ROM_IMAGE (1)

/**
 * The project specific @ref IMAGE_TYPE macro is set to @ref RAM_IMAGE
 * in case the image shall be stored and executed from RAM.
 * A RAM image stores the code, the variables, stack and heap in RAM.
 * Such an image is loaded into RAM through the debugger.
 *
 * The procedure to run a ram image is to
 * - download the image (ie. the elf file) using the debugger to RAM,
 * - reset the processor using the debugger. After reset is released, the
 * processor starts to execute the code from the ram.
 *
 */
#define RAM_IMAGE (2)

#endif /* IMAGE_TYPES_H */
