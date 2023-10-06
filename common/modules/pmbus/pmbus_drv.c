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
 * @file    pmbus_drv.c
 * @brief   Implementation of the hardware-level part of the PMBus FW Module.
 */

/* Compiler configurations first */
#include "fw_config.h"           /* Essential fw-wide configuration for e.g. buffer size #defines */
#include "patch_run.h"          /* checking for function patch points */
#include "environment.h"         /* STATIC, INLINE */
#include "shasta.h"
#include "system_shasta.h"
#include "log_app.h"            // Logging of errors
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "cmsis_os_ext.h"                   // ARM::CMSIS:RTOS:Keil RTX ext
#include "log_app.h"             /* Logging of errors */
#include <string.h>         // memcpy

/* Always compare to the associated interface */
#include "driver_common.h"
#include "clk_ctrl_drv.h"
#include "pmbus_drv.h"
#include "pmbus_gpio_control.h"
#include "pmbus_api.h"
#include "telemetry_drv.h"
#include "faults_drv.h"
#include "regulation_drv.h"
#include "pmbus_autogen.h"
#include "shasta_hal_pmbus.h"
#include "shasta_hal_scu.h"
#include "shasta_hal_cgu.h"
#include "shasta_hal_rgu.h"

/* Standard library */
#include <stdint.h>              /* uint*_t */
#include "fw_bool.h"             /* boolean type */
//#include <string.h>              /* memset */

//#define PMBUS_DRIVER_SUPPORTS_BYTE_MASKS

#ifdef UVM_DEBUG
/*lint -e(551) */
uint32_t PMBUS_HANDLER_STATE;
#endif

/* ----------------------------------------------------------------------------
 * Forward declaration and documentation of private functions
 * ------------------------------------------------------------------------- */
__DRIVER_COMMON_INLINE void PMBUS_copy_cmd_support_table (void);
__DRIVER_COMMON_INLINE void PMBUS_DECOMPRESS_COMMAND_CONFIG(void);
__DRIVER_COMMON_INLINE void PMBUS_drv_state_init (void);
__DRIVER_COMMON_INLINE void PMBUS_drv_irq_enable(void);
__DRIVER_COMMON_INLINE void PMBUS_drv_irq_disable(void);
void PMBUS_IRQHandler(void);
void PMBUS_RANGE_CALCULATE(uint_fast8_t byte_num);
STATIC void PMBUS_Thread(void const *argument);

osThreadId tid_PMBUS_Thread;
osPoolId pmbus_inter_thread_mpool;
//osMailQId  mail_box;
osMessageQId  pmbus_inter_thread_queue;

mfr_specific_init_ptr ptr_pmbus_driver_user_error_handle;  // user pointer to execute extra pmbus error handling in IRQ (stack danger!)

osThreadDef(PMBUS_Thread, osPriorityHigh, 1, OS_THREAD_STACK_SIZE);

//osMailQDef(mail_box, 16, mail_t);
osPoolDef(pmbus_inter_thread_mpool, PMBUS_QUEUE_LENGTH, pmbus_inter_thread_message_t);
osMessageQDef(pmbus_inter_thread_queue, PMBUS_QUEUE_LENGTH, pmbus_inter_thread_message_t);

PMBUS_ADDRESSING_t PMBUS_ADDRESSING;

/* ============================================================================
 * Private data
 * ========================================================================= */
typedef struct {
	uint32_t pmbus_address;
	uint32_t pmbus_command;
	uint8_t pmbus_data[PMBUS_MAX_NUM_BYTES_PER_COMMAND];
	uint32_t transaction_protocol;
	PMBUS_PAGE_W_BROADCAST_t current_page_active;
	uint32_t error_status;
	PMBUS_CMD_t * temp_pmbus_command_s;
	uint8_t WRITE_PROTOCOL;
	uint8_t READ_PROTOCOL;
	uint8_t NUM_BYTES; // data only not including slave address and command
	uint8_t NUM_PROTOCOL_BYTES;  // slave address command and any other non-data bytes
	uint8_t RANGE_TYPE;
	uint8_t SUPPORTED;
	uint8_t PROTECTED0;
	uint8_t PROTECTED1;
	uint32_t require_pec;
} pmbus_drv_state_t;
pmbus_drv_state_t pmbus_drv_state;


// access functions for PMBUS0 interface
STATIC DRIVER_PMBUS Driver_PMBUS0 = {
    PMBUS_GetVersion,
    PMBUS_Initialize,
    PMBUS_Uninitialize,
    PMBUS_PowerControl
};

/* Driver Version */
STATIC const DRIVER_VERSION DriverVersion = {
    PMBUS_API_VERSION,
    PMBUS_DRV_VERSION
};

/* ============================================================================
 * Functions (public and private in a monolithic hierarchy)
 * ========================================================================= */

/**
 *  This function is used to copy the pmbus command support table from rom to LUT
 *
*/
__DRIVER_COMMON_INLINE void PMBUS_copy_cmd_support_table (void)
{
	// Pre-populate supported PMBUS commands here:
	for(uint_fast8_t i=0; i<8; i++)
	{
		PMBUS_DATA_LUT__SET((pmbus_data_lut_idx_t) i, PMBUS_CMD_SUPPORT_ARRAY_LOOP[i]);
	}
}

/**
 *  Used to decompress autogen CMD_CONFIG structure to pmbus_drv_state variables for speed.
 *
*/
__DRIVER_COMMON_INLINE void PMBUS_DECOMPRESS_COMMAND_CONFIG(void)
{
	pmbus_drv_state.temp_pmbus_command_s = PMBUS_CMD_ARRAY_LOOP[pmbus_drv_state.pmbus_command];
	pmbus_drv_state.WRITE_PROTOCOL = (pmbus_drv_state.temp_pmbus_command_s->CMD_CONFIG & WR_PROTO_MASK)>>WR_PROTO_POSITION;
	pmbus_drv_state.READ_PROTOCOL = (pmbus_drv_state.temp_pmbus_command_s->CMD_CONFIG & RD_PROTO_MASK)>>RD_PROTO_POSITION;
	pmbus_drv_state.NUM_BYTES = (pmbus_drv_state.temp_pmbus_command_s->CMD_CONFIG & NUMBYTES_MASK)>>NUMBYTES_POSITION; // data only not including slave address and command
	pmbus_drv_state.NUM_PROTOCOL_BYTES = (pmbus_drv_state.temp_pmbus_command_s->CMD_CONFIG & NUM_PROTO_BYTES_MASK)>>NUM_PROTO_BYTES_POSITION;  // slave address command and any other non-data bytes
	pmbus_drv_state.RANGE_TYPE = (pmbus_drv_state.temp_pmbus_command_s->CMD_CONFIG & RANGE_TYPE_MASK)>>RANGE_TYPE_POSITION;
	pmbus_drv_state.SUPPORTED = (pmbus_drv_state.temp_pmbus_command_s->CMD_CONFIG & SUPPORTED_MASK)>>SUPPORTED_POSITION;
	pmbus_drv_state.PROTECTED0 = PMBUS_GET_COMMAND_PROTECTION(PMBUS_PAGE_0, (uint8_t)pmbus_drv_state.pmbus_command );
	pmbus_drv_state.PROTECTED1 = PMBUS_GET_COMMAND_PROTECTION(PMBUS_PAGE_1, (uint8_t)pmbus_drv_state.pmbus_command );
}

/**
 * function to call at stop IRQ to reinitialize the driver struct
*/
__DRIVER_COMMON_INLINE void PMBUS_drv_state_init (void)
{
	uint8_t * data;
	// require_pec is bit position 57 of PMBUS_CMDCODE_FW_CONFIG_PMBUS
	data = PMBUS_GET_DATA_ARRAY_POINTER(PMBUS_PAGE_0, PMBUS_CMDCODE_FW_CONFIG_PMBUS);
	pmbus_drv_state.require_pec = ((data[7] >> (1))&1);

	// First initialize parameters in pmbus_drv_state
	pmbus_drv_state.pmbus_address = 0xff;  // no command
	pmbus_drv_state.pmbus_command = 0xff;  // no command
	PMBUS_DECOMPRESS_COMMAND_CONFIG();
	pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_NONE;
	pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_NONE;
	pmbus_drv_state.current_page_active = PMBUS_GET_PAGE();
	pmbus_drv_state.temp_pmbus_command_s = &PMBUS_CMD_UNSUPPORTED;
#ifdef PMBUS_DRIVER_DEBUG
	HAL_SET32((uint32_t *) 0x40003000, 	pmbus_drv_state.pmbus_address);
	HAL_SET32((uint32_t *) 0x40003004, 	pmbus_drv_state.pmbus_command);
	HAL_SET32((uint32_t *) 0x40003008, 	pmbus_drv_state.error_status);
	HAL_SET32((uint32_t *) 0x4000300c, 	pmbus_drv_state.transaction_protocol);
	HAL_SET32((uint32_t *) 0x40003010, 	pmbus_drv_state.current_page_active);
	HAL_SET32((uint32_t *) 0x40003020, 	(pmbus_drv_state.pmbus_data[3]<<24)
			| (pmbus_drv_state.pmbus_data[2]<<16)
			| (pmbus_drv_state.pmbus_data[1] <<8)
			| pmbus_drv_state.pmbus_data[0]);
	HAL_SET32((uint32_t *) 0x40003024, 	( (PMBUS_CMD_MFR_ID_DATA_LOOP0[1] <<8) | PMBUS_CMD_MFR_ID_DATA_LOOP0[0]));
	//HAL_SET32((uint32_t *) 0x40003028, 	PMBUS_GET_DATA_ARRAY_POINTER(0, PMBUS_CMDCODE_MFR_ID));
	//HAL_SET32((uint32_t *) 0x40003020, 	PMBUS_CMD_MFR_ID_DATA_LOOP0[0]);
	//HAL_SET32((uint32_t *) 0x40003024, 	PMBUS_CMD_MFR_ID_DATA_LOOP0[1]);
#endif
}

/**
 * lint -esym(714, PMBUS_drv_irq_enable) -esym(759, PMBUS_drv_irq_enable) -esym(765, PMBUS_drv_irq_enable)
 * This function enables pmbus irq's
*/
__DRIVER_COMMON_INLINE void PMBUS_drv_irq_enable(void)
{
	sys_irq_handler_set((IRQn_Type) PMBUS_IRQn,
			(Irq_Handler) PMBUS_IRQHandler); // Put interrupt handler into vector table
	// Cortex specific signals for enabling IRQs
	NVIC_ClearPendingIRQ(PMBUS_IRQn);
	NVIC_EnableIRQ(PMBUS_IRQn);
	uint32_t irq_status;
	// clear all pending interrupts before enabling
	do
	{
		irq_status = (uint32_t)PMBUS_ISR__I_CODE__GET();

		PMBUS_ISR__SET
		(
			(1 << PMBUS_ISR__IE_FSM__Pos)
			| (1 << PMBUS_ISR__I_FSM__Pos)
		);
	}  while (irq_status); // do

	// set initial interrupt mask
	// enable pmbus interface, by enabling the pmbus interrupts
	PMBUS_ISR__SET
	(
			(1 << PMBUS_ISR__IE_FSM__Pos)
			| (1 << PMBUS_ISR__SEL_FLAG_SRC__Pos)
	);
}

/**
 * lint -esym(714, pmbus_drv_irq_disable) -esym(759, pmbus_drv_irq_disable) -esym(765, pmbus_drv_irq_disable)
 * This function disables pmbus irq's
*/
__DRIVER_COMMON_INLINE void PMBUS_drv_irq_disable(void)
{
	// Cortex specific signals for disabling IRQs
	NVIC_DisableIRQ(PMBUS_IRQn);

	// Disable the [Rx], [Tx], [P] and [S]  PMBus interrupts by masking them. (see IRQ handler).
	PMBUS_ISR__SET(0);
}

/**
 * lint -esym(714, PMBus_IRQHandler) -esym(759, PMBus_IRQHandler) -esym(765, PMBus_IRQHandler)
 * This is the interrupt service routine for all pmbus interrupts
 * we handle both i2c 32 protocol, smbus and pmbus protocols with this routine
*/
void PMBUS_IRQHandler(void)
{
	uint_fast8_t ack_status;
	uint32_t irq_status;
	uint32_t pmbus_status;
	uint_fast8_t pmbus_data;
	uint_fast8_t byte_count_rx, byte_count_tx;
	pmbus_inter_thread_message_t *message;
	uint_fast8_t g;
	//uint_fast8_t pec;
	uint32_t temp;
	osStatus status;

	do
	{
		pmbus_status = PMBUS_STATUS__GET();
		irq_status = (uint32_t)PMBUS_ISR__I_CODE__GET();
		pmbus_data = PMBUS_RXDATA__GET();

#ifdef PMBUS_DRIVER_DEBUG
		CGU_SPARE_FF__SET((uint16_t) (irq_status));
#endif
//		if (irq_status != 0)  // clear only if status is non-zero
//		{
		PMBUS_ISR__SET
		(
	        (1 << PMBUS_ISR__IE_FSM__Pos)
		    | (1 << PMBUS_ISR__I_FSM__Pos)
			| (1 << PMBUS_ISR__SEL_FLAG_SRC__Pos)
		);
//		}

		switch(irq_status)
		{
			//case START_IRQ:
			case pmbus_isr__i_code_start_irq:
				//pec = 0;
				byte_count_rx = 0;
				byte_count_tx = 0;
				//PMBUS_CTRL_RX__ACK_SRC_SEL__SET(1);		// select the addr LUT
				// select the addr LUT & trigger RX
				//PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift(pmbus_ctrl_rx__ack_src_sel_adr_hit) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
				//pmbus_drv_state.temp_pmbus_command_s =
				//		PMBUS_CMD_ARRAY_LOOP[pmbus_drv_state.pmbus_command];
				PMBUS_DECOMPRESS_COMMAND_CONFIG();
				if (pmbus_drv_state.pmbus_command == PMBUS_CMDCODE_SMBALERT_MASK)
				{
					pmbus_drv_state.NUM_PROTOCOL_BYTES = 2;  // on reads we have 1 proto byte
				}
				// here we want to fill the tx buffer with data from the command array
				/** @todo: ASSUMES < 32 bytes limit by LUT add chaining code in tx_irq case ?   **/
				// here we assume if pmbus_address is set we are in a repeated start condition
				if ((pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS) || (pmbus_drv_state.pmbus_address == ZONE_SLAVE_ADDRESS))
				{
				// here we want to figure out what to transmit
				// hopefully if this is the first start the UNSUPPORTED struct is used
				// which should set the num bytes to tx to 0 and data to 0xff
				// this is really a blind pointer setup since its predictive... we have to nullify later
				// after we get more info

				// reading to a write only command:
				// check for broadcast page which means we cannot read!
				if ((PMBUS_PAGE_WB_BROADCAST == (PMBUS_GET_PAGE()))
						|| (pmbus_drv_state.READ_PROTOCOL == (uint32_t)TRANSACTION_PROTOCOL_NONE))
				{
					PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__DROP_TRANSMISSION__Msk | PMBUS_CTRL_TX__TX_TRIGGER__Msk); // stop transmitting data send 0xff
					//PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_set);  // set lut to 1's
					// trigger the state machine to send 1's
					//if(!(PMBUS_DMA_CTRL__GET()&PMBUS_DMA_CTRL__EN_DMATX__Msk) )
					//{
					//	PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__GET()|PMBUS_CTRL_TX__TX_TRIGGER__Msk); // or in the trigger on first lut population
					//}
				}
				else
				{
					if (((uint32_t)(NUMBYTES+NUM_PROTO_BYTES)) <= 32)   // check that we don't exceed lut size
					{
						// set the number of bytes to transmit
						PMBUS_CTRL_TX__BYTE_TO_TX__SET(NUMBYTES + NUM_PROTO_BYTES);

						PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_clr);  // clear lut to 0's
						if (pmbus_drv_state.pmbus_command == PMBUS_CMDCODE_SMBALERT_MASK)
						{
							// note this assumes lut clears to 0's!
							g = (PMBUS_GET_DATA_ARRAY_POINTER((PMBUS_PAGE_t)pmbus_drv_state.current_page_active,
									PMBUS_CMDCODE_SMBALERT_MASK))[(2 + (pmbus_drv_state.pmbus_data[1])) - PMBUS_CMDCODE_STATUS_VOUT];
							PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) 0, (g<<8) | (0x01));  // block count = 1 on PMBUS_CMDCODE_SMBALERT_MASK
							// trigger pmbus state machine to transmit the data
							if(!(PMBUS_DMA_CTRL__GET()&PMBUS_DMA_CTRL__EN_DMATX__Msk) )
							{
								PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__GET()|PMBUS_CTRL_TX__TX_TRIGGER__Msk); // or in the trigger on first lut population
							}
						}
						else
						{
							// set the data from the command
							if (NUM_PROTO_BYTES > 0)
							{

								// note this assumes lut clears to 0's!
								PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) 0,
										PMBUS_DATA_LUT__DATA__GET((pmbus_data_lut_idx_t) 0)
										| (NUMBYTES));  // assume block protocol and send the num_bytes as first byte of data for block count
								g = 1;  // skip the first byte in the for loop below
							}
							else
							{
								g = 0;
							}

							/*
							RGU_SPARE_FF__SET(0xabe);
							// DRL: changing over from hal to memcpy for performance, this can't be done until pmbus allows byte writes, needs to be ahb
							uint8_t * data_p = PMBUS_GET_DATA_ARRAY_POINTER((PMBUS_PAGE_t)pmbus_drv_state.current_page_active,
									(uint8_t)pmbus_drv_state.pmbus_command);
							uint8_t * data_lut_p = (uint8_t *) __PMBUS_DATA_LUT__ADDRESS((pmbus_data_lut_idx_t)0);  // cast uint32_t into uint8_t * for memcpy

							// if block read protocol g will be 1 otherwise 0, avoid overwriting the number of bytes from prior PMBUS_DATA_LUT__DATA__SET call
							if (data_p != NULL) // make sure pmbus command has data otherwise copying null pointer data to lut
							{
								// memcpy(dest , source , size)
								memcpy(&data_lut_p[g], &data_p[0], NUMBYTES);  // copy the data bytes to the data lut, assumes less than 32bytes here
							}
							RGU_SPARE_FF__SET(0);
							*/

							// loop through data of command to send and put it in fifo/lut
							for(; g<((uint32_t)(NUMBYTES+NUM_PROTO_BYTES)); g++)
							{

								temp = PMBUS_DATA_LUT__GET((pmbus_data_lut_idx_t) (g>>2));
								PMBUS_DATA_LUT__SET((pmbus_data_lut_idx_t) (g>>2),
										temp
										| ((uint32_t)(PMBUS_GET_DATA_ARRAY_POINTER((PMBUS_PAGE_t)pmbus_drv_state.current_page_active,
													(uint8_t)pmbus_drv_state.pmbus_command)[g-NUM_PROTO_BYTES]) << (8*(g%4)) )
										);
								// if we already programmed first word of lut and we have more words to send trigger the fsm sm early
								if(!(PMBUS_DMA_CTRL__GET()&PMBUS_DMA_CTRL__EN_DMATX__Msk) && (g==4) )
								{
									PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__GET()|PMBUS_CTRL_TX__TX_TRIGGER__Msk); // or in the trigger on first lut population
								}
							}

							// if we had less than a word trigger the fsm sm here
							if(!(PMBUS_DMA_CTRL__GET()&PMBUS_DMA_CTRL__EN_DMATX__Msk) && (g<4) )
							{
								PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__GET()|PMBUS_CTRL_TX__TX_TRIGGER__Msk); // or in the trigger on first lut population
							}

						}

					}
					else  // if (((uint32_t)(NUMBYTES+NUM_PROTO_BYTES)) < 32)
					{
						// since we don't know if read or write yet we drop both until stop condition seen:
						PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__DROP_TRANSMISSION__Msk | PMBUS_CTRL_TX__TX_TRIGGER__Msk); // stop transmitting data send 0xff
						PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// NACK & trigger RX
						PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_OTHER_COMM_FAULT);
					}
				}
				}
				else if (pmbus_drv_state.pmbus_address == I2C_SLAVE_ADDRESS)
				{
					PMBUS_CTRL_TX__BYTE_TO_TX__SET(4);  // always send 4 bytes in our protocol here and chain later
					// we need to fill the tx buffer here with 1 word of data
					PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) 0,
						HAL_GET32(	(volatile uint32_t*)
									(((uint32_t)pmbus_drv_state.pmbus_data[3]<<24) |
									((uint32_t)pmbus_drv_state.pmbus_data[2]<<16) |
									((uint32_t)pmbus_drv_state.pmbus_data[1]<<8) |
									((uint32_t)pmbus_drv_state.pmbus_data[0])))
								);
					// trigger pmbus state machine to transmit the data
					if(!(PMBUS_DMA_CTRL__GET()&PMBUS_DMA_CTRL__EN_DMATX__Msk) )
					{
						PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__GET()|PMBUS_CTRL_TX__TX_TRIGGER__Msk); // or in the trigger on first lut population
					}
				}
				else if (pmbus_drv_state.pmbus_address == ARA_SLAVE_ADDRESS)
				{
				// ARA doesn't need any handle since hardware controlled
				}
				/** @todo: add ARP here   **/
				else
				{
				// first start in packet
				}


				//PMBUS_CTRL_TX__TX_TRIGGER__SET(1);  // enable the tx buffer

				break;		// case pmbus_isr__i_code_start_irq
			case pmbus_isr__i_code_rx_adr_w_irq:
				//PMBUS_CTRL_TX__TX_TRIGGER__SET(0);  // disable tx buffer since we now expect write
				//ack_status = PMBUS_STATUS__ACK_STATUS__GET();
				//if (ack_status == 1)
				//{
					// if we got the pmbus address in the prior start without a stop in between this is group protocol
					if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
					{
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_GROUP;
					}
					pmbus_drv_state.pmbus_address = pmbus_data >> 1;
#ifdef PMBUS_DRIVER_DEBUG
					HAL_SET32((uint32_t *) 0x40003000, 	pmbus_drv_state.pmbus_address);
#endif
					if (pmbus_drv_state.pmbus_address == I2C_SLAVE_ADDRESS)
					{
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_I2C_WRITE32;
						//if(!(PMBUS_DMA_CTRL__GET()&0x1))
						//{
							PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_ack) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);      // ACK & trigger RX
						//}
					}
					else if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
					{
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_WRITE; // write transaction type
						PMBUS_copy_cmd_support_table();
						//PMBUS_CTRL_RX__ACK_SRC_SEL__SET(2); 			// select the data LUT
						PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_data_hit) | PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// select the data LUT & trigger RX
					}
					else if (pmbus_drv_state.pmbus_address == ARP_SLAVE_ADDRESS)
					{
						PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// NACK & trigger RX
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_ADDRESS_RESOLUTION;
					}
					else if (pmbus_drv_state.pmbus_address == ZONE_SLAVE_ADDRESS)
					{
						PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// NACK & trigger RX
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_ZONE_WRITE;
					}
					else
					{
						//PMBUS_CTRL_RX__ACK_SRC_SEL__SET(0);  // NACK everything
						PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// NACK & trigger RX
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_NONE; 	// not for us
					}
#ifdef PMBUS_DRIVER_DEBUG
					HAL_SET32((uint32_t *) 0x4000300c, 	pmbus_drv_state.transaction_protocol);
#endif
					//read_write = PMBUS_STATUS__DIRECTION__GET();
					////RGU_SPARE_FF__SET(pmbus_drv_state.transaction_protocol);

				//}  // if (ack_status == 1)
				//else  // if (ack_status == 1)
				//{
					//PMBUS_CTRL_RX__ACK_SRC_SEL__SET(0);  // NACK everything
				//	PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift(pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// NACK & trigger RX
					// this is not our address so don't do anything
				//	pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_NONE; // not for us
//#ifdef PMBUS_DRIVER_DEBUG
//					HAL_SET32((uint32_t *) 0x4000300c, 	pmbus_drv_state.transaction_protocol);
//#endif
				//}
			break;  // case pmbus_isr__i_code_rx_adr_w_irq

			case pmbus_isr__i_code_rx_adr_r_irq:
				// check the ack to see if this is for us
				//ack_status = PMBUS_STATUS__ACK_STATUS__GET();
#ifdef CHECK_PMBUS_ADDRESS_ON_RESTART
				if (pmbus_drv_state.pmbus_address != 0xff)
				{
					// here we got a restart so must be read lets check that we have correct address from write portion
					if ((pmbus_data >> 1) != pmbus_drv_state.pmbus_address)
					{
						// we got a different address than when we got the write
						pmbus_drv_state.error_status = PMBUS_PROTOCOL_ERROR_OTHER;
						// NACK & trigger RX
						PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
					}
				}
#endif
				pmbus_drv_state.pmbus_address = pmbus_data >> 1;
#ifdef PMBUS_DRIVER_DEBUG
				HAL_SET32((uint32_t *) 0x40003000, 	pmbus_drv_state.pmbus_address);
#endif
				//if (ack_status == 1)
				//{

					if (pmbus_drv_state.pmbus_address == I2C_SLAVE_ADDRESS)
					{
						if (pmbus_drv_state.transaction_protocol == (uint32_t)TRANSACTION_PROTOCOL_I2C_WRITE32)
						{
							// here we got the restart and expect to read
							pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_I2C_READ32;

						}
					}
					else if (pmbus_drv_state.pmbus_address == ARA_SLAVE_ADDRESS)
					{
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_ALERT_RESPONSE_ADDRESS;
					}
					else if (pmbus_drv_state.pmbus_address == ZONE_SLAVE_ADDRESS)
					{
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_ZONE_READ;
					}
					else if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
					{
						// if we have a read to a write only command we should just drop transmission early:
						if ((pmbus_drv_state.READ_PROTOCOL == (uint32_t)TRANSACTION_PROTOCOL_NONE))
						{
							pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_UNSUPPORTED_DATA;
							PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__DROP_TRANSMISSION__Msk | PMBUS_CTRL_TX__TX_TRIGGER__Msk); // stop transmitting data send 0xff
						}
						else
						{
							pmbus_drv_state.transaction_protocol = pmbus_drv_state.READ_PROTOCOL;
						}

					}
					else
					{
						// NACK everything and trigger state machine to move on
						PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// NACK & trigger RX
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_NONE; // not for us
					}
#ifdef PMBUS_DRIVER_DEBUG
					HAL_SET32((uint32_t *) 0x4000300c, 	pmbus_drv_state.transaction_protocol);
#endif
				//}  // if (ack_status == 1)
				//else
				//{
					// this is not our address so don't do anything
				//	pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_NONE; // not for us
				//	PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__DROP_TRANSMISSION__Msk | PMBUS_CTRL_TX__TX_TRIGGER__Msk);
				//}
			break;  // case pmbus_isr__i_code_rx_adr_r_irq:

			case pmbus_isr__i_code_rx_irq:
				//pmbus_status = PMBUS_STATUS__GET();
				ack_status = (pmbus_status & PMBUS_STATUS__ACK_STATUS__Msk)>> PMBUS_STATUS__ACK_STATUS__Pos;
				byte_count_rx = (pmbus_status & PMBUS_STATUS__BYTE_CNT_RX__Msk)>> PMBUS_STATUS__BYTE_CNT_RX__Pos;
				//ack_status = PMBUS_STATUS__ACK_STATUS__GET();
				//byte_count_rx = PMBUS_STATUS__BYTE_CNT_RX__GET();
#ifdef PMBUS_DRIVER_DEBUG
				RGU_SPARE_FF__SET(byte_count_rx);
#endif
				if (ack_status == 1)
				{

						if (pmbus_drv_state.pmbus_address == I2C_SLAVE_ADDRESS)
						{
							if (byte_count_rx == 10)
							{
								// select the pec source & trigger RX
								PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_pec_chk) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
							}
							else if ( (byte_count_rx > 1) && (byte_count_rx <=9)) // skip 0
							{
								pmbus_drv_state.pmbus_data[(byte_count_rx-2)] = (uint8_t)pmbus_data;
								//PMBUS_CTRL_RX__ACK_SRC_SEL__SET(2);  	// select the data LUT
								PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_ack)
										| PMBUS_CTRL_RX__RX_TRIGGER__Msk); 		// ACK & trigger RX
#ifdef PMBUS_DRIVER_DEBUG
								HAL_SET32((uint32_t *) 0x40003014, 	pmbus_drv_state.pmbus_data[0]);
#endif
							}
							else if (byte_count_rx > 10)
							{
								// here we have too many bytes of data, set the source to always nack
								PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
							}
						}
						else if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
						{
							//SCU_SPARE_FF__SET(PMBUS_STATUS__BYTE_CNT_RX__GET());
							switch(byte_count_rx)
							{
								case 0:
								case 1:
									PMBUS_CTRL_RX__RX_TRIGGER__SET(1);  // trigger state machine but leave ack source
								break;
								case 2:
									// this is always the command byte
									pmbus_drv_state.pmbus_command = pmbus_data;  // get rx data
#ifdef PMBUS_DRIVER_DEBUG
									HAL_SET32((uint32_t *) 0x40003004, 	pmbus_drv_state.pmbus_command);
									RGU_SPARE_FF__SET(0xcafe);
#endif
									// use command to update struct
									pmbus_drv_state.current_page_active = PMBUS_GET_PAGE();
									PMBUS_DECOMPRESS_COMMAND_CONFIG();
									if (pmbus_drv_state.pmbus_command == PMBUS_CMDCODE_SMBALERT_MASK)
										pmbus_drv_state.NUM_PROTOCOL_BYTES = 2;  // on writes we have 1 less proto byte

									//pmbus_drv_state.temp_pmbus_command_s = PMBUS_CMD_ARRAY_LOOP[pmbus_drv_state.pmbus_command];
									pmbus_drv_state.transaction_protocol = pmbus_drv_state.WRITE_PROTOCOL;

									// if we are block protocol the range lut is = numbytes for next byte
									if ( pmbus_drv_state.WRITE_PROTOCOL == (uint32_t)TRANSACTION_PROTOCOL_BLOCK_WRITE )
									{
										PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_clr);
										PMBUS_DATA_LUT_BIT_SET__SET(NUMBYTES);
									}
									else
									{
										// otherwise just call range calc as usual
										PMBUS_RANGE_CALCULATE(byte_count_rx);
									}
									// if command has zero data we need to expect PEC next so select pec check next
									if(byte_count_rx==(NUMBYTES + pmbus_drv_state.NUM_PROTOCOL_BYTES))
									{
										// select the pec source & trigger RX
										PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_pec_chk) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
									}
									else  // otherwise expect data next so set ack source to data lut, this lut is populated by PMBUS_RANGE_CALCULATE function
									{
										PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_data_hit) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
									}
								break;
								default:

#ifdef PMBUS_DRIVER_DEBUG
									HAL_SET32((uint32_t *) 0x40003018, 	pmbus_drv_state.pmbus_data[0]);
									HAL_SET32((uint32_t *) 0x40003004, 	pmbus_drv_state.pmbus_command);
									//HAL_SET32((uint32_t *) 0x40003020, 	byte_count_rx);
									//HAL_SET32((uint32_t *) 0x40003024, 	NUMBYTES);
#endif

									if(byte_count_rx==(NUMBYTES + pmbus_drv_state.NUM_PROTOCOL_BYTES))
									{
#ifdef PMBUS_DRIVER_DEBUG
										RGU_SPARE_FF__SET(pmbus_data);
#endif
										pmbus_drv_state.pmbus_data[(byte_count_rx - pmbus_drv_state.NUM_PROTOCOL_BYTES)-1] = (uint8_t)pmbus_data;
										// select the pec source & trigger RX
										PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_pec_chk) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
									}
									else if (byte_count_rx > (NUMBYTES + pmbus_drv_state.NUM_PROTOCOL_BYTES) )
									{
										// CML & trigger RX
										PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
									}
									else
									{
										// get the data if we are past the protocol byte count:
										if (byte_count_rx > pmbus_drv_state.NUM_PROTOCOL_BYTES)
										{
#ifdef PMBUS_DRIVER_DEBUG
											RGU_SPARE_FF__SET(pmbus_data);
#endif
											pmbus_drv_state.pmbus_data[(byte_count_rx - pmbus_drv_state.NUM_PROTOCOL_BYTES)-1] = (uint8_t)pmbus_data;
										}
										// program the data lut for next data byte range:
										PMBUS_RANGE_CALCULATE(byte_count_rx);

										// select the data LUT & trigger RX
										PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_data_hit) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
									}

								break;
							}  // switch(byte_count)
						} //else if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
						else
						{  //else if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
							// Packet is not for us do nothing
#ifdef PMBUS_DRIVER_DEBUG
										HAL_SET32((uint32_t *) 0x4000302c, 	0xdead);
#endif
						}
				}  // if (ack_status == 1)
				else // if (ack_status == 1)
				{
					// here we have an unexpected nack, CML? & trigger RX
					PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift((uint32_t)pmbus_ctrl_rx__ack_src_sel_nack_0) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);
					// send to CML handler
					if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
					{

						// BAD command:
						if (byte_count_rx == 2)
						{
							pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_UNSUPPORTED_COMMAND;
						}
						// bad PEC, only if direction is write!:
						else if ((byte_count_rx==(1 + NUMBYTES + pmbus_drv_state.NUM_PROTOCOL_BYTES)) && ((pmbus_status & PMBUS_STATUS__DIRECTION__Msk)==0))
						{
							pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_CORRUPTED_DATA;
						}
						// bad data (too many bytes or too few bytes collected at stop condition:
						else
						{
							pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_UNSUPPORTED_DATA;
						}

					}
#ifdef PMBUS_DRIVER_DEBUG
					HAL_SET32((uint32_t *) 0x40003008, 	pmbus_drv_state.error_status);
#endif
				}
			break;  // case pmbus_isr__i_code_rx_irq:

			case pmbus_isr__i_code_tx_irq:
				/** @todo: handle chaining of the data here if transaction is >32 bytes   **/
				//pmbus_status = PMBUS_STATUS__GET();
				ack_status = (pmbus_status & PMBUS_STATUS__ACK_STATUS__Msk) >> PMBUS_STATUS__ACK_STATUS__Pos;
				byte_count_tx = (pmbus_status & PMBUS_STATUS__BYTE_CNT_TX__Msk)>> PMBUS_STATUS__BYTE_CNT_TX__Pos;
				if (pmbus_drv_state.pmbus_command == PMBUS_CMDCODE_SMBALERT_MASK)
					pmbus_drv_state.NUM_PROTOCOL_BYTES = 2;  // on reads we have 1 proto byte
				//ack_status = PMBUS_STATUS__ACK_STATUS__GET();
				//byte_count_tx = PMBUS_STATUS__BYTE_CNT_TX__GET();

				// check status to see if we need to drop transmission
				// if nacking drop
				// if packet is larger than 32 bytes drop
				// if reading from broadcast page drop
				// if reading from write only drop
				if ((ack_status == 1)&&(byte_count_tx<=32) && !((PMBUS_PAGE_WB_BROADCAST == (PMBUS_GET_PAGE()))
						|| (pmbus_drv_state.transaction_protocol == (uint32_t)TRANSACTION_PROTOCOL_NONE)))
				{
					// if dma is handling we don't need to trigger state machine
				    if(!(PMBUS_DMA_CTRL__GET()&PMBUS_DMA_CTRL__EN_DMATX__Msk))
				    {
						if ((byte_count_tx) > (uint32_t)(NUMBYTES + NUM_PROTO_BYTES))
						{
							pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_TX_TOO_MANY_BYTES;
							PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__DROP_TRANSMISSION__Msk | PMBUS_CTRL_TX__TX_TRIGGER__Msk); // stop transmitting data send 0xff
						}
						else if ((byte_count_tx) == (NUMBYTES + NUM_PROTO_BYTES))  // expect pec next  /** @todo: does hw send interrupt on this?  doesn't seem to   **/
						{
							PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__TX_TRIGGER__Msk);  // trigger state machine for pec transfer
						}
				    }
				}
				else
				{
					PMBUS_CTRL_TX__SET(PMBUS_CTRL_TX__DROP_TRANSMISSION__Msk | PMBUS_CTRL_TX__TX_TRIGGER__Msk); // stop transmitting data send 0xff
				}
			break;  // case pmbus_isr__i_code_tx_irq:

			case pmbus_isr__i_code_stop_irq:
				//pmbus_status = PMBUS_STATUS__GET();
				ack_status = (pmbus_status & PMBUS_STATUS__ACK_STATUS__Msk) >> PMBUS_STATUS__ACK_STATUS__Pos;
				byte_count_rx = (pmbus_status & PMBUS_STATUS__BYTE_CNT_RX__Msk)>> PMBUS_STATUS__BYTE_CNT_RX__Pos;
				byte_count_tx = (pmbus_status & PMBUS_STATUS__BYTE_CNT_TX__Msk)>> PMBUS_STATUS__BYTE_CNT_TX__Pos;

				// preemtively select the address lut as the ack source in case the start is slow
				// PMBUS_CTRL_RX__SET(PMBUS_CTRL_RX__ACK_SRC_SEL__Shift(pmbus_ctrl_rx__ack_src_sel_adr_hit) | PMBUS_CTRL_RX__RX_TRIGGER__Msk);

				PMBUS_CTRL_TX__DROP_TRANSMISSION__SET(0);  // clear drop transmission in stop

#ifdef PMBUS_DRIVER_DEBUG
				HAL_SET32((uint32_t *) 0x40003024, 	ack_status);
				HAL_SET32((uint32_t *) 0x40003028, 	byte_count_rx);
				HAL_SET32((uint32_t *) 0x4000302c, 	byte_count_tx);
#endif


				if (pmbus_drv_state.pmbus_address == ARA_SLAVE_ADDRESS) {
					// if we have slave address and read bit set and no bus contention we should clear faults:
					if ( (pmbus_status & PMBUS_STATUS__DIRECTION__Msk) &&
							//((pmbus_status & (PMBUS_STATUS__TOO_LONG_SCL_STRETCH__Msk | PMBUS_STATUS__BUS_CONTENTION__Msk))==0)
							((pmbus_status & PMBUS_STATUS__BUS_CONTENTION__Msk)==0)
							)
					{
						// clear the smbalert and clear its latch:
						PMBUS_ARA_CW__SMBALERT__SET(0);
					}
					// SHASTA-501 Require PEC
					// For ARA we need to just check that we get pec + slave address bytes
					if ((pmbus_drv_state.require_pec == 1) && (byte_count_tx != 2))
					{
						pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_CORRUPTED_DATA;
						PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
							PMBUS_CMDCODE_STATUS_CML, 1<<(uint32_t)PMBUS_STATUS_CML_PACKET_ERROR );
						PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active, PMBUS_CMDCODE_STATUS_CML, PMBUS_READ);
					}

				}
				if (pmbus_drv_state.pmbus_address == I2C_SLAVE_ADDRESS) {
					if (pmbus_status & PMBUS_STATUS__DIRECTION__Msk)  // check if read or write
					{
						pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_I2C_READ32;
					}
					else
					{

						if ( (byte_count_rx == 9) || (byte_count_rx == 10))// just make sure we have enough data
						{
							pmbus_drv_state.transaction_protocol = (uint32_t)TRANSACTION_PROTOCOL_I2C_WRITE32;
#ifdef PMBUS_DRIVER_DEBUG
							HAL_SET32((uint32_t *) 0x4000300c, 	pmbus_drv_state.transaction_protocol);
#endif
							for (uint_fast8_t ii = 4; ii < (byte_count_rx-1); ii+=4)
							{
								HAL_SET32(	(volatile uint32_t*)
											(((uint32_t)pmbus_drv_state.pmbus_data[3]<<24) |
											((uint32_t)pmbus_drv_state.pmbus_data[2]<<16) |
											((uint32_t)pmbus_drv_state.pmbus_data[1]<<8) |
											((uint32_t)pmbus_drv_state.pmbus_data[0])),

											(uint32_t)
											(((uint32_t)pmbus_drv_state.pmbus_data[ii+3]<<24) |
											((uint32_t)pmbus_drv_state.pmbus_data[ii+2]<<16) |
											((uint32_t)pmbus_drv_state.pmbus_data[ii+1]<<8) |
											((uint32_t)pmbus_drv_state.pmbus_data[ii]))
											);
							}
						}
					}

				}
				if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
				{
					// first run through the protocols and see if there are errors:
#ifdef DISABLE_ERROR_CHECKS
					pmbus_drv_state.error_status = PMBUS_PROTOCOL_ERROR_NONE;
#else
					switch (pmbus_drv_state.transaction_protocol)
					{
						case TRANSACTION_PROTOCOL_READ:
						case TRANSACTION_PROTOCOL_BLOCK_READ:
						case TRANSACTION_PROTOCOL_PROCESS_CALL:
						case TRANSACTION_PROTOCOL_PAGE_PLUS_READ:
						case TRANSACTION_PROTOCOL_EXTENDED_COMMAND_READ:
							// too few bits
							if (((pmbus_status&PMBUS_STATUS__TOO_FEW_BITS__Msk)>>PMBUS_STATUS__TOO_FEW_BITS__Pos) && (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
							{
								pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_TX_TOO_FEW_BITS;
							}
							// too few bytes
							if ((byte_count_tx < (uint32_t)(NUMBYTES + NUM_PROTO_BYTES))&& (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
							{
								pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_TX_TOO_FEW_BYTES;
							}
							// broadcast page read not allowed, or read of a write only
							if ((((PMBUS_PAGE_WB_BROADCAST == (PMBUS_GET_PAGE()))
									|| (pmbus_drv_state.READ_PROTOCOL == (uint32_t)TRANSACTION_PROTOCOL_NONE))
									&& (pmbus_status & PMBUS_STATUS__DIRECTION__Msk))
									&& (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
							{
								pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_OTHER;
							}
							// SHASTA-501 Require PEC
							// In Read protocols the rx byte count will either be 0 for no pec or 1 for pec
							if ((pmbus_drv_state.require_pec == 1) && (byte_count_tx != TOTAL_NUM_BYTES_READ_W_PEC))
							{
								pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_CORRUPTED_DATA;
							}
						break;
						case TRANSACTION_PROTOCOL_WRITE:
						case TRANSACTION_PROTOCOL_BLOCK_WRITE:
						case TRANSACTION_PROTOCOL_PAGE_PLUS_WRITE:
						case TRANSACTION_PROTOCOL_EXTENDED_COMMAND_WRITE:
							// too few bits
							if (((pmbus_status&PMBUS_STATUS__TOO_FEW_BITS__Msk)>>PMBUS_STATUS__TOO_FEW_BITS__Pos)&& (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
							{
								pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_RX_TOO_FEW_BITS;
							}
							// too few bytes
							if ((byte_count_rx < (NUMBYTES + pmbus_drv_state.NUM_PROTOCOL_BYTES))&& (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
							{
								pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_RX_TOO_FEW_BYTES;
							}
							// SHASTA-501 Require PEC
							if ((pmbus_drv_state.require_pec == 1) && (byte_count_rx != TOTAL_NUM_BYTES_WRITE_W_PEC))
							{
								pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_CORRUPTED_DATA;
							}
						break;
						default:
							//pmbus_drv_state.error_status = PMBUS_PROTOCOL_ERROR_UNSUPPORTED_COMMAND;
						break;
					}  // switch (pmbus_drv_state.transaction_protocol)
					// bus contention with another slave
					if ((pmbus_status & PMBUS_STATUS__BUS_CONTENTION__Msk)&& (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
					{
						pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_CORRUPTED_DATA;
					}



					// timeout on clock stretching
					//if ((pmbus_status & PMBUS_STATUS__TOO_LONG_SCL_STRETCH__Msk)&& (pmbus_drv_state.error_status == PMBUS_PROTOCOL_ERROR_NONE))
					//{
					//	pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_CORRUPTED_DATA;
					//}

					// writing to a protected command
					if ((PMBUS_GET_COMMAND_PROTECTION((PMBUS_PAGE_t)pmbus_drv_state.current_page_active, (uint8_t)pmbus_drv_state.pmbus_command))
							&& (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
					{
						pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_COMMAND_PROTECTED;
					}
					// writing to a read only command:
					if ((((pmbus_status & PMBUS_STATUS__DIRECTION__Msk)==0) && (pmbus_drv_state.WRITE_PROTOCOL == (uint32_t)TRANSACTION_PROTOCOL_NONE))
						&& (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE))
					{
						pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_UNSUPPORTED_DATA;
					}

#endif
					// SHASTA-490:  Create a user error callback function in pmbus driver
					// Just before we send data to the queue or error out call a user overridable function here:
					// Here we call all the command handles with OTP restore direction
					if (ptr_pmbus_driver_user_error_handle != (mfr_specific_init_ptr)NULL)
					{
						(*ptr_pmbus_driver_user_error_handle)();
					}

					// Now determine whether the error prevents sending data
					if (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE)
					{
						// if we had a write transaction write the data out (if there is data in command)
						if (((pmbus_status & PMBUS_STATUS__DIRECTION__Msk)==0) && (pmbus_drv_state.NUM_BYTES !=0 ))
						{
							PMBUS_SET_COMMAND(pmbus_drv_state.current_page_active,
								(uint8_t)pmbus_drv_state.pmbus_command,
								&pmbus_drv_state.pmbus_data[0]);
							//RGU_SPARE_FF__SET(0xbabe);
							//RGU_SPARE_FF__SET(pmbus_drv_state.current_page_active);
							//RGU_SPARE_FF__SET(pmbus_drv_state.pmbus_command);
							//RGU_SPARE_FF__SET(pmbus_drv_state.pmbus_data[0]);
							//RGU_SPARE_FF__SET(pmbus_drv_state.pmbus_data[1]);
						}
						// here we form the queue item to send to the pmbus task
						message = (pmbus_inter_thread_message_t*)osPoolAlloc(pmbus_inter_thread_mpool);
						if (message != NULL)
						{
							message->command = (uint8_t)pmbus_drv_state.pmbus_command;
							message->page = pmbus_drv_state.current_page_active;
							message->direction = (PMBUS_DIRECTION_e)((pmbus_status & PMBUS_STATUS__DIRECTION__Msk)>>PMBUS_STATUS__DIRECTION__Pos);
						}
						status = osMessagePut(pmbus_inter_thread_queue, (uint32_t)message, 0);

						// catch either that we are out of resources or the queue id is null
						if(status != osOK)
						{
							// destroy message
							(void)osPoolFree(pmbus_inter_thread_mpool, message);
							LOG_ERROR_TYPE(PMBUS, WARNING, OUT_OF_RESOURCE);
							// try to create smbalert but no guarantee this will happen since queue is full
							PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_OTHER_MEM_FAULT);
						}
					}
					else  // if (pmbus_drv_state.error_status == PMBUS_PROTOCOL_ERROR_NONE)
					{
						//RGU_SPARE_FF__SET(0xdead);
						//RGU_SPARE_FF__SET(pmbus_drv_state.error_status);
#ifdef PMBUS_DRIVER_DEBUG
					HAL_SET32((uint32_t *) 0x40003008, 	pmbus_drv_state.error_status);
#endif
						// if we got an error we are not going to push to queue a typical pmbus mail item but we
						// might need to create a cml event depending on if its a protocol error
						switch (pmbus_drv_state.error_status)
						{
							case PMBUS_PROTOCOL_ERROR_CORRUPTED_DATA:
								//PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
								//		PMBUS_CMDCODE_STATUS_BYTE, 1<<PMBUS_STATUS_CML );
								PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
										PMBUS_CMDCODE_STATUS_CML, 1<<(uint32_t)PMBUS_STATUS_CML_PACKET_ERROR );
							break;
							case PMBUS_PROTOCOL_ERROR_RX_TOO_MANY_BYTES:
							case PMBUS_PROTOCOL_ERROR_DATA_OUT_OF_RANGE:
							case PMBUS_PROTOCOL_ERROR_UNSUPPORTED_DATA:
							case PMBUS_PROTOCOL_ERROR_COMMAND_PROTECTED:
								//PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
								//		PMBUS_CMDCODE_STATUS_BYTE, 1<<PMBUS_STATUS_CML );
								PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
										PMBUS_CMDCODE_STATUS_CML, (1<<(uint32_t)PMBUS_STATUS_CML_INVALID_DATA) );
							break;
							case PMBUS_PROTOCOL_ERROR_RX_TOO_FEW_BITS:
							case PMBUS_PROTOCOL_ERROR_TX_TOO_FEW_BITS:
							case PMBUS_PROTOCOL_ERROR_RX_TOO_FEW_BYTES:
							case PMBUS_PROTOCOL_ERROR_TX_TOO_FEW_BYTES:
							case PMBUS_PROTOCOL_ERROR_TX_TOO_MANY_BYTES:
							case PMBUS_PROTOCOL_ERROR_SET_READ_BIT_IN_ADDRESS_BYTE:
							case PMBUS_PROTOCOL_ERROR_OTHER:
								//PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
								//		PMBUS_CMDCODE_STATUS_BYTE, 1<<PMBUS_STATUS_CML );
								PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
										PMBUS_CMDCODE_STATUS_CML, 1<<(uint32_t)PMBUS_STATUS_CML_OTHER_COMM_FAULT );
							break;
							//case PMBUS_PROTOCOL_ERROR_DEVICE_BUSY:
							//	PMBUS_COMMAND_OR_WORD((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
							//			PMBUS_CMDCODE_STATUS_WORD, 1<<(uint32_t)PMBUS_STATUS_BUSY );
							//break;
							case PMBUS_PROTOCOL_ERROR_UNSUPPORTED_COMMAND:
								//PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
								//		PMBUS_CMDCODE_STATUS_BYTE, 1<<PMBUS_STATUS_CML );
								PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
										PMBUS_CMDCODE_STATUS_CML, 1<<(uint32_t)PMBUS_STATUS_CML_INVALID_COMMAND );
							break;
							default:
								//PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
								//		PMBUS_CMDCODE_STATUS_BYTE, 1<<PMBUS_STATUS_CML );
								PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
										PMBUS_CMDCODE_STATUS_CML, 1<<(uint32_t)PMBUS_STATUS_CML_OTHER_COMM_FAULT );
							break;
						}  // switch (pmbus_drv_state.error_status)

						// forces status update to create smb alert if not masked
						PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active, PMBUS_CMDCODE_STATUS_CML, PMBUS_READ);
						//PMBUS_SET_SMB_ALERT (0x1);  // send alert to host on error
					} //else if (pmbus_drv_state.error_status == PMBUS_PROTOCOL_ERROR_NONE)
				} // if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS) {
				else  // if (pmbus_drv_state.pmbus_address == PMBUS_SLAVE_ADDRESS)
				{
					/** @todo: Handle packets that are not for us?   **/
				}
				PMBUS_drv_state_init();  // re-init all the parameters back to defaults for next packet
				//RGU_SPARE_FF__SET(0xdeca);
			break;

			case pmbus_isr__i_code_wdt_irq:
				pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_OTHER;
				PMBUS_COMMAND_OR_BYTE((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active,
						PMBUS_CMDCODE_STATUS_CML, 1<<(uint32_t)PMBUS_STATUS_CML_PACKET_ERROR );
				PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active, PMBUS_CMDCODE_STATUS_CML, PMBUS_READ);
				message = (pmbus_inter_thread_message_t*)osPoolAlloc(pmbus_inter_thread_mpool);
				message->command = (uint8_t)pmbus_drv_state.pmbus_command;
				message->page = pmbus_drv_state.current_page_active;
				status = osMessagePut(pmbus_inter_thread_queue, (uint32_t)message, 0);
				if (status != osOK)
				{
					PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
				}
			break;  // case pmbus_isr__i_code_stop_irq:

			case pmbus_isr__i_code_busy_irq:

				PMBUS_CTRL_TX__DROP_TRANSMISSION__SET(0);  // clear drop transmission in busy
				// Flag protocol busy if there was no previous error
				//if (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_NONE)
				//{
					pmbus_drv_state.error_status = (uint32_t)PMBUS_PROTOCOL_ERROR_DEVICE_BUSY;
					if ((pmbus_drv_state.current_page_active == PMBUS_PAGE_WB_BROADCAST) || (pmbus_drv_state.current_page_active == PMBUS_PAGE_WB_0))
					{
						PMBUS_STATUS_BUSY__SET(pmbus_status_word_0, 1);
					}
					if ((pmbus_drv_state.current_page_active == PMBUS_PAGE_WB_BROADCAST) || (pmbus_drv_state.current_page_active == PMBUS_PAGE_WB_1))
					{
						PMBUS_STATUS_BUSY__SET(pmbus_status_word_1, 1);
					}
					PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)pmbus_drv_state.current_page_active, PMBUS_CMDCODE_STATUS_CML, PMBUS_READ);
				//}
				PMBUS_drv_state_init();  // re-init all the parameters back to defaults for next packet

			break;

			case pmbus_isr__i_code_none_irq:

			break;

			default:

			break;
		} // switch(irq_status)
		//irq_status = PMBUS_ISR__GET() & 0x1;
	}  while (PMBUS_ISR__GET() & 0x1); // do
	// Clear Pending Interrupt
	NVIC_ClearPendingIRQ(PMBUS_IRQn);
}  // close handler  void PMBus_IRQHandler(void)

/**
 * function to calculate the command range and program the lut for the specified byte number
 * @param page  Selects the pmbus page to operate on
 * @param command Selects the pmbus command to operate on
 * @param byte_num Is the byte number the range_calculate function is being called from, note this
 * means that the calculate is being used for the next incoming byte so a value of 2 would mean that we need to calculate for
 * byte number 3.  If we get byte_num= 2 we are in the command byte so use byte_num==2 for fist data byte range check.
 * For instance temp_pmbus_command_s->RANGE[byte_num-2] would index the range 0 data entry during byte num==2
 *
 * @return None
 */
void PMBUS_RANGE_CALCULATE(uint_fast8_t byte_num)
{
    /*python automated patch entry*/
    VOID_PATCH_TABLE_RUN(PMBUS_RANGE_CALCULATE, byte_num)
#ifdef PMBUS_DRIVER_DEBUG
	RGU_SPARE_FF__SET(0xa);
#endif
	//PMBUS_CMD_t * PMBUS_CMD_TEMP = PMBUS_CMD_ARRAY_LOOP[command];
	uint_fast8_t range_type = pmbus_drv_state.RANGE_TYPE;
#ifdef USE_FW_RANGE
	uint32_t PMBUS_RANGE_TEMP_ARRAY[8] = {0,0,0,0,0,0,0,0};
#else
	int32_t data_byte_index = (int32_t)byte_num - (int32_t)pmbus_drv_state.NUM_PROTOCOL_BYTES;
#endif
#ifdef DISABLE_RANGE_CHECKS
	range_type = PMBUS_RANGE_ALL;
#endif
	PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_clr);
	if (byte_num < (pmbus_drv_state.NUM_BYTES + pmbus_drv_state.NUM_PROTOCOL_BYTES))  // note don't need to check greater than num proto bytes done in isr
	{
		// check if we got a write to read only in which case nack all incoming data past protocol bytes
		// note that this lut will not be selected as ack source on pec so should nack everything other than pec
		if (pmbus_drv_state.WRITE_PROTOCOL == (uint8_t)TRANSACTION_PROTOCOL_NONE)
		{
			range_type = (uint_fast8_t)PMBUS_RANGE_NONE;
		}
		switch (range_type)
		{
			// nack any incoming value with PMBUS_RANGE_NONE
			case PMBUS_RANGE_NONE:
			break;
			// special case for OPERATION need speed in LUT copy
			case PMBUS_RANGE_OPERATION:
				for (uint_fast8_t idx = 0; idx < 8; idx++)
				{
					PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) idx, PMBUS_RANGE_OPERATION_ARRAY[idx]);
				}
				break;
				// special case for SMBALERT_MASK need speed in LUT copy
			case PMBUS_RANGE_SMBALERT_MASK:
				if (byte_num==2)
				{
					for (uint_fast8_t idx = 0; idx < 8; idx++)
					{
						PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) idx, PMBUS_RANGE_SMBALERT_MASK_ARRAY[idx]);
					}
				}
				if (byte_num==3)
				{
					PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_set);  // accept any mask bits
				}
				break;
			// special case for _MFR_IOUT_OC_FAST_FAULT_RESPONSE need speed in LUT copy
			case PMBUS_RANGE_MFR_IOUT_OC_FAST_FAULT_RESPONSE:
				for (uint_fast8_t idx = 0; idx < 8; idx++)
				{
					PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) idx, PMBUS_RANGE_MFR_IOUT_OC_FAST_FAULT_RESPONSE_ARRAY[idx]);
				}
				break;
			// these cases all allow everything to pass through
			case PMBUS_RANGE_ALL:
			case PMBUS_RANGE_VOUT_MODE:
			case PMBUS_RANGE_LINEAR11_SIGNED:
			case PMBUS_RANGE_LINEAR11_SIGNED_ARRAY:
				PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_set);  // allow any exponents or any mantissa
				break;
			// these cases only nack the signed bit 11 being set
			case PMBUS_RANGE_LINEAR11_UNSIGNED:
			case PMBUS_RANGE_LINEAR11_UNSIGNED_ARRAY:
				if (byte_num==2)
				{
					PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_set);  // anything allowed in mantissa bits 0-7
				}
				if (byte_num==3)
				{
					for (uint_fast8_t idx = 0; idx < 8; idx++)
					{
						PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) idx, PMBUS_RANGE_UNSIGNED_LINEAR11_ARRAY[idx]);  // bit 2 of upper byte not allowed (signed bit)
					}
				}
				break;

			// sparse8 looks like this in spreadsheet 0x00, 0x03, 0x04.  The range array contains each of these values
			// the code below sets these values in the LUT index to be true to only accept these values otherwise nack
			// note this will have a performance issue for large numbers of bitsets.  try to avoid more than 16 values
			case PMBUS_RANGE_SPARSE8:
				// the first byte in the range array will tell the size of the array (including itself)
				for(uint_fast8_t i=1; i<=(pmbus_drv_state.temp_pmbus_command_s->RANGE[0]); i++)
				{
					// spare8 is comma separated array indexes to support so just bitwise write each
					PMBUS_DATA_LUT_BIT_SET__SET(pmbus_drv_state.temp_pmbus_command_s->RANGE[i]);
				}
			break;

			// spares pairs looks like this in spreadsheet: 0x00-0x7F,0x84-0x89
			// this is a very slow range set because the firmware might need to bitset upto 255 bits!
			// avoid if possible or use small ranges
			// the first byte in range is the size of the data after it
			case PMBUS_RANGE_SPARSE8_PAIRS:
				//PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_set);
				// here we assume that the range is provided in an array of min/max byte pairs
				// the first byte is always a min and second byte is always a max (ie odds=max evens = min)
				// we set the lut values between these ranges to 1
#ifdef USE_FW_RANGE
				// loop through all pairs in range array:
				// the first byte in the range array will tell the size of the array (including itself)
				for(uint_fast8_t i=1; i<=(pmbus_drv_state.temp_pmbus_command_s->RANGE[0]); i+=2)
				{
					// read modify write the lut values allows out of order range values in array
					for (uint_fast8_t j = pmbus_drv_state.temp_pmbus_command_s->RANGE[i]; j<=pmbus_drv_state.temp_pmbus_command_s->RANGE[i + 1]; j++)
					{
						// this is equivalent of PMBUS_RANGE_TEMP_ARRAY[(j/5)] |= 1<<(j%32);
						PMBUS_RANGE_TEMP_ARRAY[(j>>5U)] |= 1U<<(j&31U);  // set the bits in temp array first
					}
				}
				// copy the PMBUS_RANGE_TEMP_ARRAY to the PMBUS_DATA_LUT:
				for (uint_fast8_t idx = 0; idx < 8; idx++)
				{
					PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) idx, PMBUS_RANGE_TEMP_ARRAY[idx]);
				}
#else
				// HW supported range check:
				for(uint_fast8_t i=1; i<=(pmbus_drv_state.temp_pmbus_command_s->RANGE[0]); i+=2)
				{
					PMBUS_CTRL_RX__MIN_RANGE__SET(pmbus_drv_state.temp_pmbus_command_s->RANGE[i]);
					PMBUS_CTRL_RX__MAX_RANGE__SET(pmbus_drv_state.temp_pmbus_command_s->RANGE[i + 1]);
					PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_msk);
				}

#endif
				break;

			// byte masks look like this in spreadsheet: 0x1F;
			// they indicate that the byte has specific bits it needs nack'ed
			// the first byte in range is the size of the data after it
			// note that with bit masks the bits set must be consecutive with bit 0 supported always
			case PMBUS_RANGE_BYTE_MASKS:
				// first check that we are in the range array
				//RGU_SPARE_FF__SET(byte_num);
#ifdef USE_FW_RANGE
				if (byte_num > 1)
				{
					// loop through entire lut
					for(uint32_t i=0; i<256; i++)
					{
						// note this is upto spreadsheet owner to have same array size for byte masks as num bytes else
						// bad indexing !!!!
						RGU_SPARE_FF__SET(i);
						//RGU_SPARE_FF__SET(pmbus_drv_state.temp_pmbus_command_s->RANGE[byte_num-2]);
						if ((pmbus_drv_state.temp_pmbus_command_s->RANGE[byte_num-2] & i)==i)
						{
							// this is equivalent of PMBUS_RANGE_TEMP_ARRAY[(i/5)] |= 1<<(i%32);
							PMBUS_RANGE_TEMP_ARRAY[(i>>5U)] |= 1U<<(i&31U);
							//RGU_SPARE_FF__SET(0xbeef);
						}
					}
					//RGU_SPARE_FF__SET(0xdead);
					// copy the PMBUS_RANGE_TEMP_ARRAY to the PMBUS_DATA_LUT:
					for (uint_fast8_t idx = 0; idx < 8; idx++)
					{
						PMBUS_DATA_LUT__DATA__SET((pmbus_data_lut_idx_t) idx, PMBUS_RANGE_TEMP_ARRAY[idx]);
					}
				}
				else
				{
					PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_set);
				}

#else  // #ifdef PMBUS_DRIVER_SUPPORTS_BYTE_MASKS
				if (data_byte_index >= 0)
				{
					PMBUS_CTRL_RX__MIN_RANGE__SET(0);
					PMBUS_CTRL_RX__MAX_RANGE__SET(pmbus_drv_state.temp_pmbus_command_s->RANGE[data_byte_index]);
					PMBUS_CTRL_RX__DATA_LUT_OP__SET(pmbus_ctrl_rx__data_lut_op_msk);
				}
#endif
				break;
			default:
				break;
		} // switch (range_type)
	} //if (byte_num <= (PMBUS_CMD_TEMP->NUM_BYTES + PMBUS_CMD_TEMP->NUM_PROTOCOL_BYTES - 2))
#ifdef PMBUS_DRIVER_DEBUG
	RGU_SPARE_FF__SET(0xb);
#endif
}

/**
* This function initializes the PMBUS driver and creates the pmbus thread.
*
* @return	None
*
*/
void PMBUS_drv_init(void)
{
    /*python automated patch entry*/
    VOID_PATCH_TABLE_RUN(PMBUS_drv_init)

	ClkControl_SetPMBKrnClk(4);                     // ALPHA_CLK/4
	ClkControl_EnableClockGate(BIF_PER_PMBUS_CLK);  // PMB BIF clock
	ClkControl_EnableClockGate(PMBUS_KERNEL_CLK);   // PMB kernel clock
	ClkControl_ClrRSTMODS(BIFPERPMBUSRST);

//#ifdef UVM_DEBUG
//	/* lint -e(64) */
//	UVM_PRINT_INFO("pmbus thread start",sizeof("pmbus thread start"));
//#endif
#ifdef PMBUS_DRIVER_DEBUG
	SCU_SPARE_FF__SET(0xd0d0);	// debug message
#endif
	/* Initialize RTOS objects */
	/* Note: os*Creates are not checked for osErrorParameter since the os*Defs must be already
	 * defined correctly during development. */
	pmbus_inter_thread_mpool = osPoolCreate(osPool(pmbus_inter_thread_mpool));
	pmbus_inter_thread_queue = osMessageCreate(osMessageQ(pmbus_inter_thread_queue), NULL);
	//mail_box = osMailCreate(osMailQ(mail_box), NULL);
	tid_PMBUS_Thread = osExtThreadCreate(osThread(PMBUS_Thread), NULL);
	THROW_IF((tid_PMBUS_Thread == NULL), OUT_OF_RESOURCE); /* osErrorNoMemory */

}

// CMSIS standard access functions:--------------------------------------------------------------------------------------
/**
* This function returns the driver version
*
* @return	DRIVER_VERSION  Returns driver version
*
*/
DRIVER_VERSION PMBUS_GetVersion (void)
{
	return (DriverVersion);  // version 1.00
}

/**
* This function initializes the PMBUS instance/driver and sets intial values in HW.
*
* @return	osOK if successful
*
*/
int32_t PMBUS_Initialize (void)
{
    /*python automated patch entry*/
    PATCH_TABLE_RUN(PMBUS_Initialize)

#ifdef PMBUS_DRIVER_DEBUG
	SCU_SPARE_FF__SET(0xdead);	// debug message
#endif

	pmbus_autogen_init();  // initialize all the autogenerated pmbus structs

	// First initialize parameters in pmbus_drv_state struct
	PMBUS_drv_state_init();
	for(uint_fast32_t i=0; i<PMBUS_MAX_NUM_BYTES_PER_COMMAND; i++)
	{
		pmbus_drv_state.pmbus_data[i] = 0x0; // init to 0
	}

	// unclockgate the pmbus block and enable the phy
	PMBUS_CNFG__SET(  PMBUS_CNFG__WDT_SCL_LOW_MAX__Shift(2930u)	// bus timeout on SCL LOW PH ACCUMULATION max, 40ns*256=10240ns per lsb, set to 30ms, spec 25-35ms
					| PMBUS_CNFG__WDT_SCL_STRETCH__Shift(0x1ffu)  // clock stretching timeout of 5us
					| PMBUS_CNFG__DEBOUNCE_LENGTH__Shift(1)
//					| PMBUS_CNFG__EN_APB_IRQ_CLK_G__Msk			// unclockgate the clocks in pmbus
//					| PMBUS_CNFG__EN_FSM_FSM_CLK_G__Msk
//					| PMBUS_CNFG__EN_FSM_IRQ_CLK_G__Msk
					| PMBUS_CNFG__EN_PHY__Msk					// enable the phy
					| PMBUS_CNFG__EN_AUTO_TRANSMIT_TX_PEC__Msk  // auto transmit pec when tx count equals current byte count
					| PMBUS_CNFG__PREEMPTIVE__Msk				// to use LUT table for ack/nack
					);

	PMBUS_CTRL_TX__SET(0x0);

	// Note we only modify this once at init.  This prevents i2c/pmbus address changing when pmbus command is written.  The
	// command must be stored to OTP and used on next restore to change the address.  XADDR1,2 is strapped on boot as well.
	uint32_t temp = PMBUS_Get_Command_Dword(PMBUS_PAGE_0,PMBUS_CMDCODE_FW_CONFIG_PMBUS);  // only look at loop0 since common command
	if((temp&1)==1)  // if resistor offset feature enabled we grab the base from pmbus command and add pmbus_addr_offset field from xaddr1
	{
		I2C_SLAVE_ADDRESS = (uint8_t)(((temp & 0xff00)>>8) +
				((fw_config_data.addr_pin_i2c_address_offset&1) ?
						TLMCOM_TLM_XADDR_PINSET__TLM_XADDR2_PINSET__GET()
						: TLMCOM_TLM_XADDR_PINSET__TLM_XADDR1_PINSET__GET()));
	}
	else  // if resistor offset feature disabled we grab the base from pmbus command and add pmbus_addr_offset field from command
	{
		I2C_SLAVE_ADDRESS = (((temp & 0xff00)>>8)+((temp & 0xfe)>>1));
	}

	if((temp&0x10000)==0x10000)  // if resistor offset feature enabled we grab the base from pmbus command and add pmbus_addr_offset field from xaddr1
	{
		PMBUS_SLAVE_ADDRESS = (uint8_t)(((temp & 0xff000000)>>24) +
				((fw_config_data.addr_pin_pmbus_address_offset&1) ?
				TLMCOM_TLM_XADDR_PINSET__TLM_XADDR2_PINSET__GET()
				: TLMCOM_TLM_XADDR_PINSET__TLM_XADDR1_PINSET__GET()));
	}
	else  // if resistor offset feature disabled we grab the base from pmbus command and add pmbus_addr_offset field from command
	{
		PMBUS_SLAVE_ADDRESS = (((temp & 0xff000000)>>24)+((temp & 0xfe0000)>>17));
	}

	// now check and see if we got 0's in which case we provide a hardcoded value to recover:
	if (PMBUS_SLAVE_ADDRESS == 0) PMBUS_SLAVE_ADDRESS = 0x40;
	if (I2C_SLAVE_ADDRESS == 0) I2C_SLAVE_ADDRESS = 0x10;

	//SCU_SPARE_FF__SET(0xabcd1234);
	//SCU_SPARE_FF__SET(I2C_SLAVE_ADDRESS);
	//SCU_SPARE_FF__SET(0xabcd1234);
	//SCU_SPARE_FF__SET(PMBUS_SLAVE_ADDRESS);

	//PMBUS_TXDATA__SET(0xAA);            		// set DATA to be transmitted
	PMBUS_ARA_CW__SLAVE_ADDR_GEN_SMBALERT__SET(PMBUS_SLAVE_ADDRESS);  // set an ARA address and enable AUTO_NACK
	PMBUS_ARA_CW__ARA_ADDR__SET(ARA_SLAVE_ADDRESS);
	PMBUS_ARA_CW__DIRECTION__SET(1);
	// I think this is handled by hardware completely now
	//PMBUS_ARA_CW__SMBALERT__SET((PMBUS_Smbalert_Latch_Loop[0]|PMBUS_Smbalert_Latch_Loop[1]));  // set SMBALERT to latch default
	PMBUS_ARA_CW__EN_ARA_CW__SET(1);

	// populate the address lut with supported slave addresses:
	//PMBUS_ADDR_LUT__ADDR__SET(pmbus_addr_lut_0, ARA_SLAVE_ADDRESS);
	//PMBUS_ADDR_LUT__EN_ADDR__SET(pmbus_addr_lut_0, pmbus_addr_lut__en_addr_rd);
	//PMBUS_ADDR_LUT__TYPE__SET(pmbus_addr_lut_0, pmbus_addr_lut__type_pmb);
	uint32_t data;

	data = PMBUS_ADDR_LUT__ADDR__Shift(I2C_SLAVE_ADDRESS);
	data |= PMBUS_ADDR_LUT__EN_ADDR__Shift((uint32_t)pmbus_addr_lut__en_addr_rw);
	data |= PMBUS_ADDR_LUT__TYPE__Shift((uint32_t)pmbus_addr_lut__type_i2c);
	PMBUS_ADDR_LUT__SET(pmbus_addr_lut_1, data);

	data = PMBUS_ADDR_LUT__ADDR__Shift(PMBUS_SLAVE_ADDRESS);
	data |= PMBUS_ADDR_LUT__EN_ADDR__Shift((uint32_t)pmbus_addr_lut__en_addr_rw);
	data |= PMBUS_ADDR_LUT__TYPE__Shift((uint32_t)pmbus_addr_lut__type_pmb);
	PMBUS_ADDR_LUT__SET(pmbus_addr_lut_2, data);

	//data = PMBUS_ADDR_LUT__ADDR__Shift(ARP_SLAVE_ADDRESS);
	//data |= PMBUS_ADDR_LUT__EN_ADDR__Shift(pmbus_addr_lut__en_addr_rw);
	//data |= PMBUS_ADDR_LUT__TYPE__Shift(pmbus_addr_lut__type_pmb);
	//PMBUS_ADDR_LUT__SET(pmbus_addr_lut_3, data);

	//data = PMBUS_ADDR_LUT__ADDR__Shift(ZONE_SLAVE_ADDRESS);
	//data |= PMBUS_ADDR_LUT__EN_ADDR__Shift(pmbus_addr_lut__en_addr_rw);
	//data |= PMBUS_ADDR_LUT__TYPE__Shift(pmbus_addr_lut__type_pmb);
	//PMBUS_ADDR_LUT__SET(pmbus_addr_lut_4, data);

	//PMBUS_ADDR_LUT__ADDR__SET(pmbus_addr_lut_1, I2C_SLAVE_ADDRESS);
	//PMBUS_ADDR_LUT__EN_ADDR__SET(pmbus_addr_lut_1, pmbus_addr_lut__en_addr_rw);
	//PMBUS_ADDR_LUT__TYPE__SET(pmbus_addr_lut_1, pmbus_addr_lut__type_i2c);

	//PMBUS_ADDR_LUT__ADDR__SET(pmbus_addr_lut_2, PMBUS_SLAVE_ADDRESS);
	//PMBUS_ADDR_LUT__EN_ADDR__SET(pmbus_addr_lut_2, pmbus_addr_lut__en_addr_rw);
	//PMBUS_ADDR_LUT__TYPE__SET(pmbus_addr_lut_2, pmbus_addr_lut__type_pmb);

	//PMBUS_ADDR_LUT__ADDR__SET(pmbus_addr_lut_3, ARP_SLAVE_ADDRESS);
	//PMBUS_ADDR_LUT__EN_ADDR__SET(pmbus_addr_lut_3, pmbus_addr_lut__en_addr_rw);
	//PMBUS_ADDR_LUT__TYPE__SET(pmbus_addr_lut_3, pmbus_addr_lut__type_pmb);

	//PMBUS_ADDR_LUT__ADDR__SET(pmbus_addr_lut_4, ZONE_SLAVE_ADDRESS);
	//PMBUS_ADDR_LUT__EN_ADDR__SET(pmbus_addr_lut_4, pmbus_addr_lut__en_addr_rw);
	//PMBUS_ADDR_LUT__TYPE__SET(pmbus_addr_lut_4, pmbus_addr_lut__type_pmb);

	PMBUS_copy_cmd_support_table();				// function to load the lut with commands supported (not required)

	/* enable IRQs from the PMBus HW Module. */
	PMBUS_drv_irq_enable();
	/*lint -e(534) */
	osSignalSet(tid_Telemetry_Thread, (int32_t)OS_SIGNAL_WAIT_PMBUS_INIT);  // signal to the other threads to initialize
	/*lint -e(534) */
	osSignalSet(tid_Regulation_Thread, (int32_t)OS_SIGNAL_WAIT_PMBUS_INIT);  // signal to the other threads to initialize
#ifdef PMBUS_DRIVER_DEBUG
	SCU_SPARE_FF__SET(0xcafe);	// debug message
#endif
	return ((int32_t)osOK);
}

/**
* This function uninitializes the PMBUS instance/driver.
*
* @return	osOK if successful
*
*/
int32_t PMBUS_Uninitialize (void)
{
    /*python automated patch entry*/
    PATCH_TABLE_RUN(PMBUS_Uninitialize)

	PMBUS_drv_irq_disable();
	PMBUS_CNFG__EN_PHY__SET(0);
	return ((int32_t)osOK);
}

/**
* This function controls the power state of the PMBUS driver.
* @param state Select the power state for the driver to operate in.
* @return	osOK if successful
*
*/
int32_t PMBUS_PowerControl (POWER_STATE state)
{
    /*python automated patch entry*/
    PATCH_TABLE_RUN(PMBUS_PowerControl, state)

   switch (state)
	{
	case POWER_OFF:
//		PMBUS_CNFG__EN_APB_IRQ_CLK_G__SET(1);
//		PMBUS_CNFG__EN_FSM_FSM_CLK_G__SET(1);
//		PMBUS_CNFG__EN_FSM_IRQ_CLK_G__SET(1);
//		PMBUS_CNFG__EN_PHY__SET(0);
		break;

	case POWER_LOW:
		break;

	case POWER_FULL:
		break;

	default:
		return (DRIVER_ERROR_UNSUPPORTED);
	}
   return ((int32_t)osOK);
}

/**
* The RTOS thread/task for the pmbus firmware module.
* @return	None
*
*/
/*lint -e(715) suppress "Symbol 'arg' (line 1290) not referenced" */
STATIC void PMBUS_Thread(void const* arg)
{
    DRIVER_PMBUS* PMBUSdrv = &Driver_PMBUS0;
    osEvent evt;
    pmbus_inter_thread_message_t *message;
    PMBUS_PAGE_W_BROADCAST_t page;
    uint_fast8_t command;
    PMBUS_DIRECTION_e direction;
    int32_t status;
    //uint_fast8_t * data;
    if (arg != NULL)
    {
    	PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
    }

    command = 0xff;
    page = PMBUS_PAGE_WB_0;
    direction = PMBUS_WRITE;
    /* Initialize the PMBUS driver */
#ifdef PMBUS_DRIVER_DEBUG
    SCU_SPARE_FF__SET(0xb0a0);
#endif
    status = PMBUSdrv->Initialize();
    if (status!=(int32_t)osOK)
    	PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
    /* Power up the PMBUS peripheral */
    status = PMBUSdrv->PowerControl(POWER_FULL);
    if (status!=(int32_t)osOK)
        PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);

    PMBUS_Gpio_Configure();  // setup the gpio pins like VR_ENABLE

    // execute a callback function for end of module init
    call_module_init_cb(MODULE_PMBUS);

    /* thread loop */
    for(;;)
    {
        /* Wait for completion */
#ifdef PMBUS_DRIVER_DEBUG
    	SCU_SPARE_FF__SET(0xb0b0);	// debug message
#endif
    	evt = osMessageGet(pmbus_inter_thread_queue, osWaitForever);
    	if (evt.status == osEventTimeout)
		{
			LOG_ERROR_TYPE(PMBUS, ERROR, TIMEOUT);  // osWaitForever should never hit this timeout
		}
    	if (evt.status == osEventMessage)
    	{
    		message = (pmbus_inter_thread_message_t*)evt.value.p;
    		page = message->page;
			command = message->command;
			direction = message->direction;
			evt.status = osPoolFree(pmbus_inter_thread_mpool, message);  // free the pool
			if (evt.status == osEventTimeout)
			{
				LOG_ERROR_TYPE(PMBUS, ERROR, TIMEOUT);  // osWaitForever should never hit this timeout
			}
    	}

        // here lets just store the command and data into spare registers for debug:
		//data = PMBUS_GET_DATA_ARRAY_POINTER(page, command);
#ifdef PMBUS_DRIVER_DEBUG
		SCU_SPARE_FF__SET(command);
		if (error)
		{
			SCU_SPARE_FF__SET(error);

		}
		else
		{
			SCU_SPARE_FF__SET(PMBUS_GET_COMMAND_SIZE(page, command));
			for( uint_fast8_t datasize = 0;  datasize < PMBUS_GET_COMMAND_SIZE(page, command); datasize++ )
			{
				RGU_SPARE_FF__SET(datasize);
				//SCU_SPARE_FF__SET(data[datasize]);

			}
		}
#endif
#ifdef UVM_DEBUG
	PMBUS_HANDLER_STATE = (((uint32_t)page<<16) | command);
#endif
		if(page==PMBUS_PAGE_WB_BROADCAST)
		{
			PMBUS_CMD_HANDLER(PMBUS_PAGE_0, (uint8_t)command, (PMBUS_DIRECTION_e)direction);
			PMBUS_CMD_HANDLER(PMBUS_PAGE_1, (uint8_t)command, (PMBUS_DIRECTION_e)direction);
		}
		else
		{
			PMBUS_CMD_HANDLER((PMBUS_PAGE_t)page, (uint8_t)command, (PMBUS_DIRECTION_e)direction);
		}
#ifdef UVM_DEBUG
		PMBUS_HANDLER_STATE = (0xbbbb);
#endif
    }
}
