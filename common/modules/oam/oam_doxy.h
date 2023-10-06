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
 * @file    oam_doxy.h
 * @brief   Doxygen overview of the Operation and Maintenance FW Module.
 */

#ifndef OAM_DOXY_H
#define OAM_DOXY_H

/** @page pgShasta_Oam   Operation, Administration & Maintenance Module
 *
 * @section sctShasta_Oam_Intro   Operation, Administration & Maintenance
 * This module is responsible for the handling of Hard Faults
 * (see @ref sctShasta_Exc_Known_Errors_Boot). This makes sure that reporting and resolving errors
 * which may impact multiple modules is done in an "overall" and centralized approach.
 *
 * @section sctShasta_Oam_Hard_Fault   Handling of Hard Faults
 * The OAM is the owner of the Hard Fault Handler since a Hard Fault of the Cortex M0 may occur
 * everywhere in the firmware (device-level). For handling a Hard Fault see
 * @ref sctShasta_Exc_Known_Errors_Boot.
 */

#endif /* OAM_DOXY_H */
