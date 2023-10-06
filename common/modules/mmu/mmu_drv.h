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
 * @file    mmu_drv.h
 * @brief   Header file for the memory management unit mmu
 *
 */

#ifndef MMU_DRV_H
#define MMU_DRV_H
#include <stdint.h>
#include "driver_common.h" // cmsis definitions
#include "system_shasta.h" // interrupt handler typedef
#include "shasta_hal_mmu.h"


/** Version definitions
 *
 * Version History:<br>
 * Version 0.03: Restructuring of the BIF per register protection
 *               The BIF_PER register protection can now be enabled per sub-block <br>
 * Version 0.04: RAM reduced to 2x16kbyte
 *               new shasta_hal_mmu.h
 * Version 0.05: ROM increased to 80kbyte
 *               Test, Teststat, cse registers added
 *               new shasta_hal_mmu.h
 * */
#define MMU_API_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,05) /* API version */
#define MMU_DRV_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,05) /* API version */

/**
 * The blk number 128-16 is pointing to the first offset of RAM1 1kbyte block
 * There are 128 1kByte partitions available but we use only the upper 16 one
 */
#define MMU_RAM_MAPPING_OFFSET (128-16)
/**
 * Enumerator to select which memory block shall be used
 */
typedef enum _MMU_BLK_e {
    /** ROM Selector  */
    ROM = 0,
    /** OTP Selector */
    OTP,
    /** RAM1 Selector */
    RAM1,
    /** RAM2 Selector */
    RAM2
} MMU_BLK_t;

/**
 * Enumerator to select which peripheral block shall be protected
 */
/*lint -parent(uint8_t, REG_BLK_t) -strong(AJX)*/
typedef enum _REG_BLK_e {
    /** No protection */
    NONE = mmu_mmu_per_space_set__per_none,
    /** Protection for Regulation Registers */
    MMU_CPUP = mmu_mmu_per_space_clr__per_cpup,
    /** Protection for CPUS Registers */
    MMU_BIFP = mmu_mmu_per_space_clr__per_bifp,
    /** Protection for TRIM Registers */
    MMU_TRIM = mmu_mmu_per_space__per_bifr_trim,
    /** Protection for Analog Registers */
    MMU_ANA = mmu_mmu_per_space__per_bifr_ana,
    /** Protection for the VSP Registers */
    MMU_VDSP0 = mmu_mmu_per_space__per_bifr_vsp0,
    MMU_VDSP1 = mmu_mmu_per_space__per_bifr_vsp1,
    MMU_VDSP2 = mmu_mmu_per_space__per_bifr_vsp2,
    /** Protection for VCTRL Registers */
    MMU_VCTRL0 = mmu_mmu_per_space__per_bifr_vctrl0,
    MMU_VCTRL1 = mmu_mmu_per_space__per_bifr_vctrl1,
    /** Protection for PID Registers */
    MMU_PID0 = mmu_mmu_per_space__per_bifr_pid0,
    MMU_PID1 = mmu_mmu_per_space__per_bifr_pid1,
    /** Protection for ISP Registers */
    MMU_ISP0 = mmu_mmu_per_space__per_bifr_isp0,
    MMU_ISP1 = mmu_mmu_per_space__per_bifr_isp1,
    /** Protection for PWM Registers */
    MMU_PWM = mmu_mmu_per_space__per_bifr_pwm,
    /** Protection for COM Registers */
    MMU_COMM = mmu_mmu_per_space__per_bifr_comm,
    /** Protection for Telemetry Registers */
    MMU_TELEM0 = mmu_mmu_per_space__per_bifr_telem0,
    MMU_TELEM1 = mmu_mmu_per_space__per_bifr_telem1,
    /** Protection for Fault Registers */
    MMU_FAULT0 = mmu_mmu_per_space__per_bifr_fault0,
    MMU_FAULT1 = mmu_mmu_per_space__per_bifr_fault1,
    /** Protection for Fan Registers */
    MMU_FAN1 = mmu_mmu_per_space__per_bifr_fan1,
    MMU_FAN2 = mmu_mmu_per_space__per_bifr_fan2,
    /** Protection for TS Registers */
    MMU_TS = mmu_mmu_per_space__per_bifr_ts,
    /** Protection for common Telemetry Registers */
    MMU_TLMCOM = mmu_mmu_per_space__per_bifr_tlmcom,
    /** Protection for common Fault Registers */
    MMU_FAULTCOM = mmu_mmu_per_space__per_bifr_faultcom,
    /** Protection for common Test Registers */
    MMU_TEST = mmu_mmu_per_space__per_bifr_test,
    /** Protection for common Teststat Registers */
    MMU_TESTSTAT = mmu_mmu_per_space__per_bifr_teststat,
    /** Protection for common CSE Registers */
    MMU_CSE = mmu_mmu_per_space__per_bifr_cse
} REG_BLK_t;


#define STATUS_BUF_SIZE 8

/** To indicate the status of the mmu.
 */

typedef struct _MMU_STATUS_s
{
    /** Counts the number of protection violations */
	uint8_t cnt;
    /** Indicates which memory block was affected by the protection.  */
    uint16_t blk[STATUS_BUF_SIZE];
    /** Indicates which page was affected. */
    uint8_t page[STATUS_BUF_SIZE];
    /** Indicates if a memory range was exceeded. */
    uint8_t range_error;

} MMU_STATUS_s;


/**
 * MMU_GetVersion retrieves the version of the mmu drv and api
 *
 *  @return DRIVER_VERSION
 */
DRIVER_VERSION MMU_GetVersion (void);

/**
 * This routine initializes a MMU by creating a default 1:1 mapping and
 * installs the MMU_Hardfault_IRQ handler.
 *
 *  @param  none
 *
 *  @return none.
 */
void MMU_Initialize (void);

/**
 * MMU_Uninitialize deactivates the mmu.
 *
 *  @param  none
 *
 *  @return none
 */
void MMU_Uninitialize (void);

/**
 * MMU_configure configures a mapping entry.
 *
 *  @param  blk_i  sets the block to be re-mapped
 *  @param  page_i sets the page to be re-mapped
 *  @param  blk_o  sets the target block
 *  @param  page_o sets the target page
 * *
 *  @return none
 */
void MMU_Configure (MMU_BLK_t blk_i, uint8_t page_i, MMU_BLK_t blk_o, uint8_t page_o);


/**
 * MMU_SetMmuProtection sets the protection for the specific memory
 *
 *  @param  blk  sets the block to be protected
 *  @param  page sets the page to be protected
 *
 *  @return none
 */
void MMU_SetMemProtection (MMU_BLK_t blk, uint8_t page);

/**
 * MMU_ClearMmuProtection clears the protection for the specific memory
 *
 *  @param  blk  sets the block to be unprotected
 *  @param  page sets the page to be unprotected
 *
 *  @return none
 */
void MMU_ClearMemProtection (MMU_BLK_t blk, uint8_t page);

/**
 * MMU_SetRegProtection sets the protection for the specific registers
 *
 *  @param  blk  sets the register block to be protected
 *
 *  @return none
 */
void MMU_SetRegProtection (REG_BLK_t blk);

/**
 * MMU_SetRegProtection sets the protection for the specific registers
 *
 *  @param  none
 *
 *  @return Current protection setting
 */
uint32_t MMU_GetRegProtection (void);
/**
 * MMU_ClearRegProtection clears the protection for the specific registers
 *
 *  @param  blk  sets the register block to be unprotected
 *
 *  @return none
 */
void MMU_ClearRegProtection (REG_BLK_t blk);

/**
 * MMU_ClearStatus clears the latched protection fault and resets the internal variables.
 *
 *  @param  none.
 *
 *  @return none
 */
void MMU_ClearStatus (void);


/**
 * MMU_GetStatus retrieves information which protected MMU block was hit by a write operation.
 *
 *  @param  none
 *
 *  @return MMU_STATUS struct
 */

MMU_STATUS_s MMU_GetStatus (void);


/**
 * MMU_Hardfault_IRQHandler handles the Hardfault Exception in case of a write protection
 * violation.
 *
 *  @param  none
 *
 *  @return none
 */

void MMU_Hardfault_IRQHandler(void);


/** MMU API definitions */
typedef struct _DRIVER_MMU {
  DRIVER_VERSION   (*GetVersion)         (void);
  void                 (*Initialize)         (void);
  void                 (*Uninitialize)       (void);
  void                 (*Configure)          (MMU_BLK_t blk_i, uint8_t page_i, MMU_BLK_t blk_o, uint8_t page_o);
  void                 (*SetMemProtection)   (MMU_BLK_t blk, uint8_t page);
  void                 (*ClearMemProtection) (MMU_BLK_t blk, uint8_t page);
  void                 (*SetRegProtection)   (REG_BLK_t blk);
  uint32_t             (*GetRegProtection)   (void);
  void                 (*ClearRegProtection) (REG_BLK_t blk);
  MMU_STATUS_s         (*GetStatus)          (void);
  void                 (*ClearStatus)        (void);
} const DRIVER_MMU;

extern DRIVER_MMU Driver_MMU;


#endif /* MMU_DRV_H */
