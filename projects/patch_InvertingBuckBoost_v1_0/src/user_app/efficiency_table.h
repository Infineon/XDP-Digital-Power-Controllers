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
** Created on:  2020-08-19
*
 *      Author: kotvytsr
 */

#ifndef SRC_USER_APP_EFFICIENCY_TABLE_H_
#define SRC_USER_APP_EFFICIENCY_TABLE_H_

#include <stdint.h>

#define VIN_MAX_IDX 3
#define IOUT_MAX_IDX 5 //220504 changes

//================================

/*
 * 	Efficiency Look-Up Table

 *  	Iout
Vin	1.25A	2.5A	6.25A	12.5A	15.0A
	=========================================
36V	85.4%	91.5%	95.6%	96.4%	96.4%
48V	82.4%	89.7%	94.8%	96.2%	96.2%
60V	79.3%	88.0%	94.0%	95.9%	95.9%
 *
	219		234		245		247		247
	211		230		243		246		246
	203		225		241		246		246

 * */

// Efficiency LUT U0.8 format
// Set your efficiency values
//220504 changes
const uint8_t efficiency_table [VIN_MAX_IDX][IOUT_MAX_IDX] = {
		{219, 234, 245, 247, 247},
		{211, 230, 243, 246, 246},
		{203, 225, 241, 246, 246}
};

// Efficiency table ranges
// Set your values for input voltage
// and output current accordingly

// {36, 48, 60}; -> adjusted for ranges 0->(36)->40 // 40->(48)->52 // 52->(60)->inf
const uint32_t 	vin_table[VIN_MAX_IDX] 		= {40, 52, 60};

// LSB = 0.25 (uX.2) -> ranges: 0->1.25A->2.5A->6.25A->12.5A->15A
const uint32_t 	iout_table[IOUT_MAX_IDX] 	= {5, 10, 25, 50, 60}; //220504 changes
//================================

#endif /* SRC_USER_APP_EFFICIENCY_TABLE_H_ */
