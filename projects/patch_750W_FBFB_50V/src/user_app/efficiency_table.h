/* ============================================================================
** Copyright (C) 2020 Infineon. All rights reserved.
**               Infineon Technologies, PSS SYS / DES
** ============================================================================
**
** ============================================================================
** This document contains proprietary information. Passing on and
** copying of this document, and communication of its contents is not
** permitted without prior written authorisation.
** ============================================================================
**
** Created on:  2021-03-05
*
 *      Author: kotvytsr
 */

#ifndef SRC_USER_APP_EFFICIENCY_TABLE_H_
#define SRC_USER_APP_EFFICIENCY_TABLE_H_

#include <stdint.h>


/*	estimate_Efficiency
 * 	Efficiency Table
 *	Old
 *	Vin\Po	40W		80W		160W	320W	640W	800W
 *	========================================================
 * 	36V		94.0%	94.5%	95.0%	96.0%	96.1%	96.0%
 * 	48V		94.0%	94.5%	95.2%	96.3%	96.4%	95.9%
 * 	60V		93.9%	94.4%	95.6%	96.4%	96.6%	95.3%
 * 	75W		93.5%	94.1%	95.2%	96.1%	96.6%	94.8%
 *
 * 	New
 *  Vin\Po  50W     100W    150W    350W    550W    750W
 *  ========================================================
 *  36V     87.1%   92.1%   93.8%   96.3%   96.6%   96.3%
 *  48V     81.7%   89.2%   91.7%   94.5%   95.3%   95.2%
 *  60V     76.6%   86.3%   89.8%   93.1%   94.3%   94.5%
 *  72V     72.4%   83.6%   87.9%   92.0%   93.4%   93.8%
 *
 *
 * Each element of efficiency table (as "eff") was recalculated accordingly following formula:
 * correction_element = 1 / eff - 1
 *
 *	New Table "correction_table" in u1.9 format <0.2% accuracy.
 *	[33, 30, 27, 21, 21],
 *	[33, 30, 26, 20, 19, 22],
 *	[33, 30, 24, 19, 18, 25],
 *	[36, 32, 26, 21, 18, 28]
 *
 * */

#define VIN_MAX_IDX 4
#define POUT_MAX_IDX 6


//
// Correction_table in u1.9 format. Is using for Input Current (Input Power) correction
//const uint16_t correction_table [VIN_MAX_IDX][POUT_MAX_IDX] = {
//		{0, 33, 30, 27, 21, 21},
//		{0, 33, 30, 26, 20, 19, 22},
//		{0, 33, 30, 24, 19, 18, 25},
//		{0, 36, 32, 26, 21, 18, 28},
//};
// Efficiency table U0.8 format. Is using for efficiency display via PMBus MFR_ESTIMATE_EFFICIENCY
//const uint8_t efficiency_table [VIN_MAX_IDX][POUT_MAX_IDX] = {
//		{241, 242, 243, 246, 246, 246},
//		{241, 242, 244, 247, 247, 246},
//		{240, 242, 245, 247, 247, 244},
//		{239, 241, 244, 246, 247, 243},
//};
const uint8_t efficiency_table [VIN_MAX_IDX][POUT_MAX_IDX] = {
		{223, 236, 240, 247, 247, 247},
		{209, 228, 235, 242, 244, 244},
		{196, 221, 230, 238, 241, 242},
		{185, 214, 225, 236, 239, 240},
};

const uint32_t 	Vin_table[VIN_MAX_IDX] 		= {42, 54, 64, 76};		// {36, 48, 60, 72}; + 4 offset
const uint32_t 	Pout_table[POUT_MAX_IDX] 	= {50, 100, 150, 350, 550, 830}; //{40, 80, 160, 320, 640, 800};


#endif /* SRC_USER_APP_EFFICIENCY_TABLE_H_ */
