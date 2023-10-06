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
#include "regulation_state_machine_callbacks.h"

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
void patch_PMBUS_Inter_Thread_Set_Command(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, PMBUS_DIRECTION_e direction )
{
    if (pmbus_drv_state.error_status == (uint32_t)PMBUS_PROTOCOL_ERROR_DEVICE_BUSY)
    {
    	// take the scratchpad semaphore first
    	if (DRIVER_COMMON_STRUCT.scratchpad_status == 0)
    	{
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

			if (user_data.i2c_debounce_cnfg >= 7)
				user_data.i2c_debounce_cnfg = 7;
			if(user_data.i2c_debounce_cnfg <= 1)
				user_data.i2c_debounce_cnfg = 1;

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
    	}
    }
    patch_rom_table.patch_PMBUS_Inter_Thread_Set_Command = NULL;
    PMBUS_Inter_Thread_Set_Command(loop, cmd, direction);
    patch_rom_table.patch_PMBUS_Inter_Thread_Set_Command = patch_PMBUS_Inter_Thread_Set_Command;
}

void patch_pmbus_fifo(void)
{
    patch_rom_table.patch_PMBUS_Inter_Thread_Set_Command = patch_PMBUS_Inter_Thread_Set_Command;
}
