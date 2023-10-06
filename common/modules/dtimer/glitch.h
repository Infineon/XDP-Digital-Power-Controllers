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
/*
 * glitch.h
 *
 *  Created on: Oct 6, 2014
 *      Author: pangyuet
 */

#ifndef GLITCH_H
#define GLITCH_H
#include <stdint.h>
#include "dtimer_drv.h"
#include "system_shasta.h" //register handler in interrupt handler table

/**
 *  Initialize the DTimer for dglitch operation
 *  @param  dtie Which timer instance to use DTIMER0 .. DTIMER3.
 *  @param  dts  Status and configuration of the timer module.
 */
void deglitch_init(dtimer_instance_e dtie, DTIMER_CONFIG_t dts);

/**
 * Reload the DTimer with new value supplied by
 * the parameter

 * @param  dtie Which timer instance to use DTIMER0 .. DTIMER3.
 * @param  ticks Reload the DTimer with this number of ticks
 */

void deglitch( dtimer_instance_e dtie, uint32_t ticks);

/**
 * Convert microsecond to ticks, depending on how the DTimer1 kernel clock
 * is initialized
 * @param  us A 32-bit value in microsecond
 * @return ticks A 32-bit value in clock ticks used by DTimer
 */
uint32_t DTimer1_us2ticks(uint32_t us);

/**
 * Convert microsecond to ticks, depending on how the DTimer2 kernel clock
 * is initialized
 * @param  us A 32-bit value in microsecond
 * @return ticks A 32-bit value in clock ticks used by DTimer
 */
uint32_t DTimer2_us2ticks(uint32_t us);


#endif /* GLITCH_H */
