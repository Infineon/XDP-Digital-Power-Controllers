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
 * @file    fw_params.h
 * @brief   Public declarations and interface of the application of the
 *          fw_params Module. This module contains trim and configuration data for FW
 *          which can be stored/loaded to/from OTP.
 */

#ifndef FW_PARAMS_H
#define FW_PARAMS_H

/* Data types */
#include <stdint.h>            /* uint*_t */
#include "fw_bool.h"           /* boolean type */
#include "linker_sections.h"


/* ============================================================================
 * Public data types
 * ========================================================================= */

#if defined ( __CC_ARM )
#define __FW_PARAMS_INLINE  static __inline __attribute__((always_inline))
#elif defined ( __GNUC__ )
#define __FW_PARAMS_INLINE  static __inline
#elif defined ( _MSC_VER )
#define __FW_PARAMS_INLINE  static __forceinline
#else
#warning Unsupported Compiler
#define __FW_PARAMS_INLINE  static inline
#endif

/** OTP partition selector */
typedef enum
{
    /** OTP partition: Data Section of the OTP. */
    OTP_PARTITION_DATA = 0,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH1,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH2,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH3,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH4,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH5,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH6,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH7,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH8,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH9,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH10,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH11,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH12,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH13,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH14,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH15,
    /** OTP partition: Patch section*/
    OTP_PARTITION_PATCH16,
    /** Maximum number of supported table entries */
    OTP_PARTITION_MAX
} Otp_Partition_Selector_t;


/** Container for OTP sections
 */

typedef struct _OTP_SECTION
{
    /** Size in bytes of the OTP partition */
    uint16_t size;

} OTP_SECTION_t;


/** Container for FW trim data
 */

typedef struct FW_TRIM_
{
    /** Size in bytes of the FW trim parameters */
    uint16_t size;
    /** Version of the FW trim parameters */
    uint16_t version;
    /** Boot descriptor definition */
    uint32_t sys_boot_descriptor;
    /** OTP section parameter in OTP */
    OTP_SECTION_t otp_sections[OTP_PARTITION_MAX];
} FW_TRIM_t;

/** Container for FW configuration data
 */

typedef struct FW_CONFIG_
{
    /** Size in bytes of the FW configuration parameters */
    uint16_t size;
    /** Version of the FW configuration parameters */
    uint16_t version;
    /** Enables/Disables the use of xvalent setting
     * - 0 ... xvalent use disabled
     * - 1 ... xvalent use enabled
     * */
    uint8_t xv_en;
    uint8_t addr_pin_xv;
    uint8_t addr_pin_pmbus_address_offset;
    uint8_t addr_pin_i2c_address_offset;
    /** Dummy entries are required for 4 byte data alignment */
} FW_CONFIG_t;


extern FW_TRIM_t fw_trim_data;
extern FW_CONFIG_t fw_config_data;
/**
 * fw_params_init sets default configuration very early before OTP is read
 */
void fw_params_init(void);

/**
 * fw_params_boot_descriptor_get returns the current boot descriptor loaded from the fw trim parameters.
 *
 * @return sys_boot_descriptor from OTP
 */

__FW_PARAMS_INLINE uint16_t fw_params_boot_descriptor_get(void)
{
    return fw_trim_data.sys_boot_descriptor;
}

/**
 * fw_params_otp_partition_ptr_get reads the partition ptr
 *
 * @param partition .. partition index
 *
 * @return pointer to partition
 */
__FW_PARAMS_INLINE uint8_t *fw_params_otp_partition_ptr_get(Otp_Partition_Selector_t partition)
{
    uint16_t absolute_offset = 0;
    for (uint8_t i = 0; i < partition; i++)   absolute_offset += fw_trim_data.otp_sections[i].size;
    return (uint8_t *) SECTION_BASE_OTP_DATA + absolute_offset;
}

/**
 * fw_params_otp_partition_size_get reads the size of the partition
 *
 * @param partition index
 *
 * @return partition size
 */
__FW_PARAMS_INLINE uint32_t fw_params_otp_partition_size_get(Otp_Partition_Selector_t partition)
{
    return fw_trim_data.otp_sections[partition].size;
}

/**
 * fw_params_xv_en_get reads the xvalent enable parameter
 *
 */
__FW_PARAMS_INLINE uint32_t fw_params_xv_en_get(void)
{
    return fw_config_data.xv_en;
}

/**
 * fw_params_xv_en_set sets the xvalent enable parameter
 *
 * @param xv xvalent enable 0
 *
 */
__FW_PARAMS_INLINE void fw_params_xv_en_set(uint8_t xv_en)
{
    if (xv_en <= 3u)
    {
        fw_config_data.xv_en = xv_en;
    }
}
/**
 * load fw trim parameter from otp trim file
 *
 *  @param  ptr to the fw_trim parameter
 *
 */
void fw_params_trim_load_cb(void * ptr);

/**
 * store fw trim parameter to otp trim file
 *
 *  @param  ptr to the fw_trim parameter
 *
 */
void fw_params_trim_store_cb(uint8_t * ptr);


/**
 * load fw config parameter from otp config file
 *
 *  @param  ptr to the fw_config parameter
 *
 */
void fw_params_config_load_cb(void * ptr);

/**
 * store fw config parameter to otp config file
 *
 *  @param  ptr to the fw_config parameter
 *
 */
void fw_params_config_store_cb(uint8_t * ptr);

/** fw_params callback.
 * This callback is used to hand-over one of the fw_params load or store call backs to the specific function.
 *
 * @param ptr - Pointer to the respective configuration data
 *
 */
typedef  void (*fw_params_callback_t)(uint8_t * ptr);

#endif /* FW_PARAMS_H */
