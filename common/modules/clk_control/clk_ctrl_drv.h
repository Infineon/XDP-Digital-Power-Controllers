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
 * clk_ctrl_drv.h
 *
 *  Created on:
 *  Author: A. Liu
 */

#ifndef CLK_CTRL_DRV_H_
#define CLK_CTRL_DRV_H_

/***************************** Include Files *********************************/

#include <stddef.h>         // NULL pointer for public structs
#include "driver_common.h"
#include "fw_config.h"      // Essential fw-wide configuration for e.g. buffer size #defines
#include "shasta.h"

/**************************** Type Definitions *******************************/

typedef enum CLOCK_TYPE_e {
    HOSC_CLK           = 0u,
    ROM_CLK            = 1u,
    RAM1_CLK           = 2u,
    RAM2_CLK           = 3u,
    AMBA_CLK           = 4u,
    DMA_CLK            = 5u,
    CNFG_OTP1_W_CLK    = 6u,
    CNFG_DMA_CLK       = 10u,
    BIF_REG_CLK        = 11u,
    BIF_PER_SVID_CLK   = 12u,
    BIF_PER_PMBUS_CLK  = 13u,
    BIF_PER_SSP_CLK    = 14u,
    BIF_PER_I2C_CLK    = 15u,
    BIF_PER_UART_CLK   = 16u,
    DTIMER1_CLK        = 17u,
    DTIMER2_CLK        = 18u,
    DTIMER3_CLK        = 19u,
    WDT_CLK            = 20u,
    GPIO0_CLK          = 21u,
    GPIO1_CLK          = 22u,
    DTIMER1_KERNEL_CLK = 23u,
    DTIMER2_KERNEL_CLK = 24u,
    DTIMER3_KERNEL_CLK = 25u,
    PMBUS_KERNEL_CLK   = 26u,
    OTP_KERNEL_CLK     = 27u
} CLOCK_TYPE;

typedef enum CLOCK_RSTSR_TYPE_e {
    WDT_RST            = 4u,
	SW_RST             = 3u,
	SYSRESETQ          = 2u,
	HW_PWDN            = 1u,
	SW_PWDN            = 0u
} CLOCK_RSTSR_TYPE;

typedef enum CLOCK_RSTMODS_TYPE_e {
    GPIO1RST           = 15u,
    GPIO0RST           = 14u,
    WDTIFRST           = 13u,
    DTIMER3RST         = 12u,
    DTIMER2RST         = 11u,
    DTIMER1RST         = 10u,
    BIFPERUARTRST      = 9u,
    BIFPERI2CRST       = 8u,
    BIFPERSSPRST       = 7u,
    BIFPERPMBUSRST     = 6u,
    BIFPERSVIDRST      = 5u,
    BIFREGRST          = 4u,
    CNFGOTP1WRST       = 3u,
    DMARST             = 2u,
    SWRST              = 1u
} CLOCK_RSTMODS_TYPE;

typedef enum CLOCK_WKUP_TYPE_e {
    GPIO1_WKUP         = 23u,	
    GPIO0_WKUP         = 22u,	
    PMBUS_WKUP         = 21u,	
    EXTWKUP            = 20u,	
    SVID1_WKUP         = 19u, 	
    SVID0_WKUP         = 18u, 	
    DTIMER11_KRN_CLK   = 17u, 	
    DTIMER12_KRN_CLK   = 16u, 	
    DTIMER21_KRN_CLK   = 15u, 	
    DTIMER22_KRN_CLK   = 14u,	
    DTIMER31_KRN_CLK   = 13u,	
    DTIMER32_KRN_CLK   = 12u, 	
    USR_CNFG7          = 11u,	
    USR_CNFG6          = 10u, 	
    USR_CNFG5          =  9u, 	
    USR_CNFG4          =  8u, 	
    USR_CNFG3          =  7u, 	
    USR_CNFG2          =  6u, 	
    USR_CNFG1          =  5u, 	
    USR_CNFG0          =  4u,	
    EN_EXTWKUP         =  3u,	
    DS_DBGPORT         =  2u, 	
    SET_REMAP          =  1u 	
} CLOCK_WKUP_TYPE;

/*****************************************************************************/

/*
 * Clock gating APIs in clk_ctrl_drv.c
 */ 
void ClkControl_EnableClockGate(CLOCK_TYPE Type);
void ClkControl_DisableClockGate(CLOCK_TYPE Type);
void ClkControl_EnableClockAll (void);
void ClkControl_DisableClockAll (void);
void ClkControl_SetSEMsk(CLOCK_TYPE Type);
void ClkControl_RstSEMsk(CLOCK_TYPE Type);
void ClkControl_SetSEMskAll(void);
void ClkControl_RstSEMskAll(void);
void ClkControl_SetDSEMsk(CLOCK_TYPE Type);
void ClkControl_RstDSEMsk(CLOCK_TYPE Type);
void ClkControl_SetHoscGate(uint32_t Kill);

/*
 * Clock frequency APIs in clk_ctrl_drv.c
 */
void ClkControl_SetAlphaClk(uint8_t Div);
void ClkControl_SetCPUClk(uint8_t Div);
void ClkControl_SetOTPKrnClk(uint8_t Div);
void ClkControl_SetPMBKrnClk(uint8_t Div);
void ClkControl_SetWDTKrnClk(uint8_t Div);
void ClkControl_SetDtimer3KrnClk(uint8_t Div);
void ClkControl_SetDtimer2KrnClk(uint8_t Div);
void ClkControl_SetDtimer1KrnClk(uint8_t Div);
void ClkControl_CoreClockMin(void);

/*
 * Reset APIs in clk_ctrl_drv.c
 */
uint32_t ClkControl_RdRSTSR(void);
uint32_t ClkControl_RdBitRSTSR(CLOCK_RSTSR_TYPE Bit);
void ClkControl_SetSWRST_CTRL(uint8_t Enable);
void ClkControl_SetRSTMODS(CLOCK_RSTMODS_TYPE Type);
void ClkControl_ClrRSTMODS(CLOCK_RSTMODS_TYPE Type);
void ClkControl_SetSWReset(void);
void ClkControl_SetSW_PWDN_CTRL(uint8_t Enable);
void ClkControl_SetSW_PWDN_REQ(uint8_t Request); 
void ClkControl_SetSWPowerDown(void);
void ClkControl_SetModuleWKUP(CLOCK_WKUP_TYPE Type);
void ClkControl_RstModuleWKUP(CLOCK_WKUP_TYPE Type);
void ClkControl_SetEssentialWKUP(void);
void ClkControl_RstEssentialWKUP(void);

/*
 * System clock enable APIs in clk_ctrl_drv.c
 */
void ClkControl_SystemClkInit(void);


#endif /* CLK_CTRL_DRV_H_ */
