/*
 * ADD_ON_FEATURES.c
 *
 *  Created on: Mar 7, 2018
 *      Author: ashendar
 */


#include "telemetry_api.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "shasta_hal_tsen.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_analog.h"
#include "shasta_hal_common.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_isen.h"
#include "log_app.h"             /* Logging of errors */
#include <string.h>             /* memset */
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"
#include "shasta_hal_tstctrl.h"  	// ate test state
#include "mmu_drv.h"                /* MMU_Initialization */
#include "driver_common.h"
#include "shasta_hal_fault_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include <math.h>


void add_on_features_init (void)
{
	float test;
	test = pow(2,3);
	PMBUS_Set_Command_Dword (PMBUS_PAGE_WB_0,PMBUS_CMDCODE_MFR_EXAMPLE,test);

}

#ifdef pmbus_memory_error
/**
 * Copy from NVM or buffer to pmbus data storage.
 * @param ram_location Pointer to buffer.
 * @return None
 */
void patch_PMBUS_Copy_From_Memory(PMBUS_PAGE_t loop, const uint8_t *ram_location)
{

	uint8_t * data;
	uint8_t data_array[PMBUS_MAX_NUM_BYTES_PER_COMMAND];  // max of 32 data bytes per command
	uint8_t otp_store;
	uint32_t size;
	uint32_t size_check;
	data = &data_array[0];
	// grab the size check value from first 4 bytes of OTP pointer data
	size_check = ((uint32_t)ram_location[3]<<24U) |  ((uint32_t)ram_location[2]<<16U) | ((uint32_t)ram_location[1]<<8U) | (uint32_t)ram_location[0];

	// grab expected size plus command count from command descriptors
	size = PMBUS_Calculate_Size_Check();
	//RGU_SPARE_FF__SET(0xdead);
	//RGU_SPARE_FF__SET(size_check);
	//RGU_SPARE_FF__SET(size);
	//RGU_SPARE_FF__SET(0xbeef);
	//RGU_SPARE_FF__SET(0);
	if (size != size_check)
	{
		PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_MEMORY_FAULT);  // use cml mem fault for pmbus image issues
		PMBUS_Set_Command_Byte((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_OPERATION, 0x0);
		PMBUS_Set_Command_Byte((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_ON_OFF_CONFIG, 0x1f);  // if we have bad defaults at least default on off config to off
		PMBUS_Set_Command_Byte((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_CAPABILITY, 0xD0);
		PMBUS_Set_Command_Byte((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_PMBUS_REVISION, 0x33);
		return;
	}
	size = 4;  // offset the image by 4bytes because first 4 bytes are size checker

	// loop through all commands:
	for (uint8_t command = 0; command < 255; command++)
	{
		// note that this must filter out commands without data and unsupported commands to avoid null pointers on PMBUS_SET_COMMAND
		otp_store = PMBUS_GET_OTP_STORE(command);
		//RGU_SPARE_FF__SET(command);
		//SCU_SPARE_FF__SET(otp_store);
		if (otp_store != 0)
		{
			// PMBUS_CMDCODE_MFR_SETUP_PASSWORD has a 4 byte storage always ignore command size
			if (command == PMBUS_CMDCODE_MFR_SETUP_PASSWORD)
			{
				for( uint8_t datasize = 0;  datasize < 4; datasize++)
				{
					//SCU_SPARE_FF__SET(command);
					//CGU_SPARE_FF__SET(size);
					//RGU_SPARE_FF__SET(ram_location[size]);
					data_array[datasize] = ram_location[size];
					size++;
				}
				PMBUS_SET_COMMAND((PMBUS_PAGE_W_BROADCAST_t)loop, command, data);
			}
			else if (command != PMBUS_CMDCODE_SMBALERT_MASK)
			{
				for( uint8_t datasize = 0;  datasize < PMBUS_GET_COMMAND_SIZE(command); datasize++)
				{
					//SCU_SPARE_FF__SET(command);
					//CGU_SPARE_FF__SET(size);
					//RGU_SPARE_FF__SET(ram_location[size]);
					data_array[datasize] = ram_location[size];
					size++;
				}
				PMBUS_SET_COMMAND((PMBUS_PAGE_W_BROADCAST_t)loop, command, data);
			}
			else
			{
				data_array[0] = 0;  // set the non-otp restored data to 0 (should be mask and data on writes, not required but cleaner than stale data
				data_array[1] = 0;
				for( uint8_t datasize = 0;  datasize < PMBUS_SMBALERT_MASK_SIZE; datasize++)
				{
					//SCU_SPARE_FF__SET(command);
					//CGU_SPARE_FF__SET(size);
					//RGU_SPARE_FF__SET(ram_location[size]);
					data_array[datasize+2] = ram_location[size];  // SMBALERT_MASK stores from index 2-10
					size++;
				}
				//PMBUS_SET_COMMAND((PMBUS_PAGE_W_BROADCAST_t)loop, command, data);  // cannot use set command because smbalert otp data is beyond num bytes of 2
				for (int32_t i = 0; i < (PMBUS_SMBALERT_MASK_SIZE + 2); i++)
				{
					PMBUS_GET_DATA_ARRAY_POINTER((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_SMBALERT_MASK)[i] = data[i];
				}
			}
		}
	}

}
#endif


