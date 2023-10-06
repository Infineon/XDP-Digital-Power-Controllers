/*
 * patch_pmbus_fifo.c
 *
 *  Created on: Jul 18, 2023
 *      Author: dlewis4
 */

#include <stdint.h>                  /* uint * */
#include <string.h>                  /* memset() */
#include "pmbus_autogen.h"
#include "pmbus_api.h"
#include "pmbus_drv.h"
#include "cmsis_os_exceptions.h"
#include "patch_rom_table.h"
#include "patch_pmbus_fifo.h"
#include "shasta_hal_pmbus.h"


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
extern pmbus_drv_state_t pmbus_drv_state;

//https://jirard.intra.infineon.com/browse/LANGLEY-327
// if we can get the pmbus fifo full the hw pointers can get out of sync if you get a write at the same time as read
// should be a rare condition only seen if you are toggling sda line rapidly creating start and stop symbols
#if 0
void patch_PMBUS_Inter_Thread_Set_Command(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, PMBUS_DIRECTION_e direction )
{
    uint32_t reg;

    if (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_DEVICE_BUSY)
    {
    	// take the scratchpad semaphore first
    	if (DRIVER_COMMON_STRUCT.scratchpad_status == 0)
    	{
			reg = PMBUS_CNFG__GET();

    		DRIVER_COMMON_STRUCT.scratchpad_status = 1;
			uint32_t* pmbus_per_pointer = __PMBUS_ISR__ADDRESS();
			memcpy(&DRIVER_COMMON_STRUCT.scratchpad[0], pmbus_per_pointer, sizeof(PMBUS_t)-offsetof(PMBUS_t, ISR));  // copy context of pmbus per starting from ISR register
			// reset peripheral
			RGU_RSTMODS_SET__BIFPERPMBUSRST__SET(1);
			// clear reset
			RGU_RSTMODS_CLR__BIFPERPMBUSRST__SET(1);
			// restore context
			memcpy(pmbus_per_pointer, &DRIVER_COMMON_STRUCT.scratchpad[0], sizeof(PMBUS_t)-offsetof(PMBUS_t, ISR));
			// release scratchpad semaphore
			DRIVER_COMMON_STRUCT.scratchpad_status = 0;

			//if (user_data.i2c_debounce_cnfg >= 7)
			//	user_data.i2c_debounce_cnfg = 7;
			//if(user_data.i2c_debounce_cnfg <= 1)
			//	user_data.i2c_debounce_cnfg = 1;

            PMBUS_CNFG__SET(reg);
	/*
			// unclockgate the pmbus block and enable the phy
			PMBUS_CNFG__SET(  PMBUS_CNFG__WDT_SCL_LOW_MAX__Shift(2930u)	// bus timeout on SCL LOW PH ACCUMULATION max, 40ns*256=10240ns per lsb, set to 30ms, spec 25-35ms
							| PMBUS_CNFG__WDT_SCL_STRETCH__Shift(0x1ffu)  // clock stretching timeout of 5us
							| PMBUS_CNFG__DEBOUNCE_LENGTH__Shift(user_data.i2c_debounce_cnfg)
		//					| PMBUS_CNFG__EN_APB_IRQ_CLK_G__Msk			// unclockgate the clocks in pmbus
							| PMBUS_CNFG__DS_FSM_FSM_CLK_G__Msk
		//					| PMBUS_CNFG__EN_FSM_IRQ_CLK_G__Msk
							| PMBUS_CNFG__EN_PHY__Msk					// enable the phy
							| PMBUS_CNFG__EN_AUTO_TRANSMIT_TX_PEC__Msk  // auto transmit pec when tx count equals current byte count
							| PMBUS_CNFG__PREEMPTIVE__Msk				// to use LUT table for ack/nack
							);
							*/
    	}
    }
    patch_rom_table.patch_PMBUS_Inter_Thread_Set_Command = NULL;
    PMBUS_Inter_Thread_Set_Command(loop, cmd, direction);
    patch_rom_table.patch_PMBUS_Inter_Thread_Set_Command = patch_PMBUS_Inter_Thread_Set_Command;
}
#endif

#ifdef JIRA_972_I2C_FIFO
void patch_PMBUS_Inter_Thread_Set_Command(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, PMBUS_DIRECTION_e direction )
{
    uint32_t reg;
    if (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_DEVICE_BUSY)
    {
	 // take the scratchpad semaphore first
	    if (DRIVER_COMMON_STRUCT.scratchpad_status == 0)
	    {
	    	reg = PMBUS_CNFG__GET();

	        DRIVER_COMMON_STRUCT.scratchpad_status = 1;

	        // first copy the entire context of pmbus to scratchpad
	        uint32_t* pmbus_per_pointer = __PMBUS_STATUS__ADDRESS();
	        memcpy(&DRIVER_COMMON_STRUCT.scratchpad[0], pmbus_per_pointer, sizeof(PMBUS_t));  // copy context of pmbus per starting from ISR register

	        // reset peripheral
	        RGU_RSTMODS_SET__BIFPERPMBUSRST__SET(1);
	        // clear reset
	        RGU_RSTMODS_CLR__BIFPERPMBUSRST__SET(1);

	        // restore context from ISR to REGMEMGAP3
	        pmbus_per_pointer = __PMBUS_ISR__ADDRESS();
	        /*lint -e(740)  Unusual pointer cast (incompatible indirect types)*/
	        memcpy(pmbus_per_pointer, &DRIVER_COMMON_STRUCT.scratchpad[offsetof(PMBUS_t, ISR)], offsetof(PMBUS_t, REGMEMGAP3)-offsetof(PMBUS_t, ISR));

	        // restore context from STATUS_WORD to STATUS_CLEAR_ALL
	        pmbus_per_pointer = __PMBUS_STATUS_WORD__ADDRESS(pmbus_status_word_0);
	        /*lint -e(740)  Unusual pointer cast (incompatible indirect types)*/
	        memcpy(pmbus_per_pointer, &DRIVER_COMMON_STRUCT.scratchpad[offsetof(PMBUS_t, STATUS_WORD)], offsetof(PMBUS_t, STATUS_CLEAR_ALL)-offsetof(PMBUS_t, STATUS_WORD));

	        // restore context from STATUS_MASK_LP0 to end of buffer
	        pmbus_per_pointer = __PMBUS_STATUS_MASK_LP0__ADDRESS(pmbus_status_mask_lp0_0);
	        /*lint -e(740)  Unusual pointer cast (incompatible indirect types)*/
	        memcpy(pmbus_per_pointer, &DRIVER_COMMON_STRUCT.scratchpad[offsetof(PMBUS_t, STATUS_MASK_LP0)], sizeof(PMBUS_t) - offsetof(PMBUS_t, STATUS_MASK_LP0));

	        // release scratchpad semaphore
	        DRIVER_COMMON_STRUCT.scratchpad_status = 0;

            PMBUS_CNFG__SET(reg);
/*
	        // unclockgate the pmbus block and enable the phy
	        PMBUS_CNFG__SET(  PMBUS_CNFG__WDT_SCL_LOW_MAX__Shift(2930u)    // bus timeout on SCL LOW PH ACCUMULATION max, 40ns*256=10240ns per lsb, set to 30ms, spec 25-35ms
	                        | PMBUS_CNFG__WDT_SCL_STRETCH__Shift(0x1ffu)  // clock stretching timeout of 5us
	                        | PMBUS_CNFG__DEBOUNCE_LENGTH__Shift((fw_config_data.xv_en>>1) & 7)
	                        | PMBUS_CNFG__DS_APB_IRQ_CLK_G__Msk            // disable power saving clockgating in apb irq block
	                        | PMBUS_CNFG__DS_FSM_FSM_CLK_G__Msk            // disable power saving clockgating in fsm block
	                        | PMBUS_CNFG__DS_FSM_IRQ_CLK_G__Msk            // disable power saving clockgating in fsm irq block
	                        | PMBUS_CNFG__EN_PHY__Msk                    // enable the phy
	                        | PMBUS_CNFG__EN_AUTO_TRANSMIT_TX_PEC__Msk  // auto transmit pec when tx count equals current byte count
	                        | PMBUS_CNFG__PREEMPTIVE__Msk                // to use LUT table for ack/nack
	                        );
	                        */
	    }
    }

}


void patch_pmbus_fifo(void)
{
    patch_rom_table.patch_PMBUS_Inter_Thread_Set_Command = patch_PMBUS_Inter_Thread_Set_Command;
}
#endif
