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
 * driver_common.h
 *
 *  Created on: Oct 31, 2016
 *      Author: dlewis4
 */

#ifndef DRIVER_COMMON_H_
#define DRIVER_COMMON_H_

/* Compiler configurations first */
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"
#include "log.h"  // Modules_t
/* Data types */
#include <stddef.h>             // NULL pointer for public structs
#include "environment.h"         /* STATIC, __DRIVER_COMMON_INLINE */
#include "cmsis_os.h"

#define DRIVER_VERSION_MAJOR_MINOR(major, minor)   (((major) << 8) | (minor))

#define 	DRIVER_OK   0  //Operation succeeded.
#define 	DRIVER_ERROR   -1  //Unspecified error.
#define 	DRIVER_ERROR_BUSY   -2  //Driver is busy.
#define 	DRIVER_ERROR_TIMEOUT   -3  //Timeout occurred.
#define 	DRIVER_ERROR_UNSUPPORTED   -4  //Operation not supported.
#define 	DRIVER_ERROR_PARAMETER   -5  //Parameter error.
#define 	DRIVER_ERROR_SPECIFIC   -6  //Start of driver specific errors.

typedef struct DRIVER_COMMON_STRUCT_s{
	uint32_t scratchpad_status;						/** used for determining if scratchpad is in use */
	uint8_t* scratchpad;	/** scratchpad for otp patching, store functions, etc */
}DRIVER_COMMON_STRUCT_t;

typedef struct _DRIVER_VERSION {
  uint16_t api;                         ///< API version
  uint16_t drv;                         ///< Driver version
} DRIVER_VERSION;

typedef enum  _POWER_STATE_e {
  POWER_OFF,
  POWER_LOW,
  POWER_FULL
} POWER_STATE;

typedef enum  _OS_SIGNAL_WAIT_e {
	OS_SIGNAL_WAIT_ANY = 0x0,
	OS_SIGNAL_WAIT_PMBUS_INIT = (1<<0),
	OS_SIGNAL_WAIT_TELEM_INIT = (1<<1),
	OS_SIGNAL_WAIT_REGULATION_INIT = (1<<2)
} OS_SIGNAL_WAIT;

/*
typedef enum PMBUS_PAGE_NO_BROADCAST_e
{
	PMBUS_PAGE_NB_0 = 0u,
	PMBUS_PAGE_NB_1 = 1u
} PMBUS_PAGE_NO_BROADCAST_t;
*/

#if defined ( __CC_ARM )
#define __DRIVER_COMMON_INLINE  static __inline __attribute__((always_inline))
//#define __DRIVER_COMMON_INLINE  static __inline
#define __DRIVER_INLINE  static __inline
#elif defined ( __GNUC__ )
#define __DRIVER_COMMON_INLINE static inline __attribute__((always_inline))
#define __DRIVER_INLINE  static __inline
#elif defined ( _MSC_VER )
#define __DRIVER_COMMON_INLINE  static __forceinline
#define __DRIVER_INLINE  static __inline
#else
#warning Unsupported Compiler
#define __DRIVER_COMMON_INLINE static inline
#define __DRIVER_INLINE  static __inline
#endif


#define MAX_UNSIGNED_LINEAR11_MANTISSA (0x7FF)

/** Intialization routine for the scratchpad */
void DRIVER_COMMON_init(void);

/**
 * uint8_t TWOS_COMPLEMENT (uint8_t numbits, int32_t exp)
 * Converts int32_t exp into 8bit twos complement number
 * based on numbits input
 * Typical linear11 format is a 5bit exponent
 */
__DRIVER_INLINE uint8_t TWOS_COMPLEMENT (uint8_t numbits, int32_t exp)
{
	return ((uint8_t)exp & ((1 << numbits)-1));
}

/**
 * int32_t TWOS_UNCOMPLEMENT (uint8_t numbits, uint8_t exp)
 * Converts uint8_t exp into signed integer based on numbits input
 * Typical linear11 format is a 5bit exponent
 */
__DRIVER_INLINE int32_t TWOS_UNCOMPLEMENT (uint8_t numbits, uint32_t exp)
{
	// if negative:
	if ((exp & ((1 << numbits) - 1)) >> (numbits-1))
	{
		return -((int32_t) ((~exp &((1 << numbits) - 1)) + 1));  // not the value, add 1, negate
	}
	else  // positive
	{
		return ((int32_t) (exp & ((1 << (numbits-1)) - 1)));
	}
}

//__DRIVER_COMMON_INLINE void LINEAR11_TO_EXPONENT_MANTISSA(int32_t packed_linear_format_number, int32_t &exponent, int32_t &mantissa)
/**
 * int32_t LINEAR11_TO_MANTISSA(int32_t packed_linear_format_number)
 * Takes a linear11 format (16bit, 5bit exponent 11bit mantissa) number
 * and extracts the mantissa
 */
__DRIVER_INLINE int32_t LINEAR11_TO_MANTISSA(uint16_t packed_linear_format_number)
{
	int32_t mant_bits=packed_linear_format_number & 0x07ff;
	int32_t mant_signbit=mant_bits & 0x0400;
	return ((int32_t) (mant_signbit ? (mant_bits | 0xfffff800) : mant_bits));
}

/**
 * int32_t LINEAR11_TO_EXPONENT(int32_t packed_linear_format_number)
 * Takes a linear11 format (16bit, 5bit exponent 11bit mantissa) number
 * and extracts the exponent
 */
__DRIVER_INLINE int32_t LINEAR11_TO_EXPONENT(uint16_t packed_linear_format_number)
{
	int32_t exp_bits=(packed_linear_format_number >> 11) & 0x1f;
	int32_t exp_signbit=exp_bits & 0x0010;
	return ((int32_t) (exp_signbit ? (exp_bits | 0xffffffe0) : exp_bits));
}

__DRIVER_INLINE int16_t SIGN_EXTEND_MANTISSA(uint16_t mantissa)
{
	uint16_t temp = 0x7ff & mantissa;  //mask off exponent
	if ( (temp & 0x0400) == 0x0400)
	{
		return (int16_t)(temp | 0xf800);  // if negative or in masked off exponent
	}
	else
	{
		return (int16_t)temp;
	}
}

__DRIVER_COMMON_INLINE int32_t SHIFT_EXPONENT(int32_t value, int32_t exponent)
{
	if ( exponent <= 0)
	{
		exponent = -exponent;
		value = value >> (exponent);
	}
	else
	{
		value = value << (exponent);
	}
	return value;
}


__DRIVER_COMMON_INLINE uint32_t SHIFT_EXPONENT_UNSIGNED(uint32_t value, int32_t exponent)
{
	if ( exponent <= 0)
	{
		exponent = -exponent;
		value = value >> (exponent);
	}
	else
	{
		value = value << (exponent);
	}
	return value;
}

/**
 * Takes 2 uint32_t format numbers and divides them then rounds the LSB
 * Note that 0.5 would round up to 1 and 0.4 would round down to 0
 */
__DRIVER_COMMON_INLINE uint32_t DIVIDE_WITH_ROUNDING(uint32_t num, uint32_t div)
{
	return (num + (div / 2)) / div;
}


#ifdef UVM_DEBUG

#define UVM_INFO (0<<4)
#define UVM_WARNING (1<<4)
#define UVM_ERROR (2<<4)
#define UVM_FATAL (3<<4)
#define UVM_TRIGGER (1<<6)

#define UVM_DBG (6)
#define UVM_FULL (5)
#define UVM_HIGH (4)
#define UVM_MEDIUM (3)
#define UVM_LOW (2)
#define UVM_NONE (1)


void UVM_PRINT_INFO(const void * msg, uint8_t msgSize);
void UVM_PRINT_ERROR(const void * msg, uint8_t msgSize);
void UVM_PRINT_WARNING(const void * msg, uint8_t msgSize);
void UVM_ERROR_COUNT(void);
void UVM_PASS_COUNT(void);
void UVM_TEST_END(void);
#endif


/** Firmware-wide list of sources which indicate the thread in which the log was created. */
typedef enum
{
    MODULE_UNKNOWN_SOURCE = 0,
	MODULE_PMBUS,
	MODULE_REGULATION,
	MODULE_FAULTS,
	MODULE_TELEMETRY,
	MODULE_MMU,
	MODULE_DMA,
	MODULE_DTIMER,
	MODULE_I2C,
	MODULE_JOB_AGENCY,
	MODULE_LOGGER,
	MODULE_OAM,
	MODULE_OTP,
	MODULE_GPIO,
	MODULE_INIT_CALLBACK_ARRAY_SIZE
} Modules_t;

typedef enum
{
	TELEMETRY_SAMPLE,
	SHUTDOWN_SEQUENCE,
	SYSTEM_CALLBACK_ARRAY_SIZE
} System_t;

// Callback functions for end of driver initialization (used for user driver configurations):
typedef void (* module_init_callback)(void);
typedef void (* system_callback)(void);
void set_module_init_cb(Modules_t module_name, module_init_callback module_init_cb);
void call_module_init_cb(Modules_t module_name);
void set_system_cb(System_t system_name, system_callback system_cb);
void call_system_cb(System_t system_name);
uint32_t scratchpad_semaphore_get (void);
void scratchpad_semaphore_release(void);

// FW patch execution pointer:
typedef void (* fw_patch_ptr)(void);
extern fw_patch_ptr fw_patch_p;

extern DRIVER_COMMON_STRUCT_t DRIVER_COMMON_STRUCT;
extern osSemaphoreId scratchpad_semaphore_id; // Semaphore ID

#endif /* DRIVER_COMMON_H_ */
