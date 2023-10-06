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
 * @file    i2c_drv.h
 * @brief   Implementation of the hardware-level part of the I2C FW Module.
 *
 *  Created on:
 *  Author: A. Liu
 */

#ifndef I2C_DRV_H
#define I2C_DRV_H 

/* ============================================================================
 * Public data types
 * ========================================================================= */

// STATUS DEFINITIONS
#define I2C_TX_BUF_SIZE           100
#define I2C_RX_BUF_SIZE           100
#define I2C_STATUS_MASK           0xF8

typedef enum I2C_STATUS_TYPE_e {
    I2C_BUS_ERROR               = 0x00,	
	I2C_START                   = 0x08,
	I2C_REP_START               = 0x10,
	I2C_MT_SLA_ACK              = 0x18,
	I2C_MT_SLA_NACK             = 0x20,
	I2C_MT_DATA_ACK             = 0x28,
	I2C_MT_DATA_NACK            = 0x30,
	I2C_MT_ARB_LOST             = 0x38,
	I2C_MR_SLA_ACK              = 0x40,
	I2C_MR_SLA_NACK             = 0x48,
	I2C_MR_DATA_ACK             = 0x50,
	I2C_MR_DATA_NACK            = 0x58,
	I2C_SR_SLA_ACK              = 0x60,
	I2C_SR_ARB_LOST_SLA_ACK     = 0x68,
	I2C_SR_GCALL_ACK            = 0x70,
	I2C_SR_ARB_LOST_GCALL_ACK   = 0x78,
	I2C_SR_DATA_ACK             = 0x80,
	I2C_SR_DATA_NACK            = 0x88,
	I2C_SR_GCALL_DATA_ACK       = 0x90,
	I2C_SR_GCALL_DATA_NACK      = 0x98,
	I2C_SR_STOP                 = 0xA0,
	I2C_ST_SLA_ACK              = 0xA8,
	I2C_ST_ARB_LOST_SLA_ACK     = 0xB0,
	I2C_ST_DATA_ACK             = 0xB8,
	I2C_ST_DATA_NACK            = 0xC0,
	I2C_ST_LAST_DATA            = 0xC8,
	I2C_ST_LAST_NACK            = 0xD0,
	I2C_ST_2ND_ACK              = 0xE0,
    I2C_ST_2ND_NACK             = 0xE8,
    I2C_NO_INFO                 = 0xF8  
} I2C_STATUS_TYPE;

/* ============================================================================
 * API functions
 * ========================================================================= */ 
 
void i2c_init(uint8_t clk_mode);
void i2c_uninit (void);
void i2c_SetDevAddr(uint8_t dev_addr);
void i2c_SetSLA(uint8_t sla_addr);
void i2c_SetSLAX(uint8_t sla_addrx);
void i2c_SetGCE(uint8_t gen_call_en);

/*
 * polling mode APIs in i2c_drv.c
 */
void i2c_TxStart(void);
void i2c_TxStop(void);
void i2c_Wait4Sent(void);
void i2c_Wait4Idle(void);
void i2c_TxByte(uint8_t data);
void i2c_TxLByte(uint8_t data);
uint8_t i2c_RxByte(void);
uint8_t i2c_RxLByte(void);
void i2c_MasterTx(uint8_t devAddr, uint8_t length, const uint8_t *data);
void i2c_MasterRx(uint8_t devAddr, uint8_t length, uint8_t *data);

/*
 * interrupt mode APIs in i2c_drv.c
 */
void i2c_irq_enable(void);
void i2c_irq_disable(void);
void I2C_IRQHandler(void);
void i2c_TxStartI(void);
void i2c_TxStopI(void);
void i2c_TxStopStartI(void);
void i2c_Wait4IdleI(void);
void i2c_TxByteI(uint8_t data);
void i2c_TxLByteI(uint8_t data);
void i2c_AckByteI(uint8_t ackFlag);
void i2c_MasterTxI(uint8_t devAddr, uint8_t length, const uint8_t *data);
void i2c_MasterRxI(uint8_t devAddr, uint8_t length, uint8_t *data);


#endif /* I2C_DRV_H */
 

