
#include "log_debug_data_examples.h"
#include "debug_app.h"

/*
 * These parameters need to be defined by the user to configure debug app:
 *
 * DEBUG_DATA_BUFFER_BASE is where debug_app can start storing the data. Typically should be pointing to scratchpad.
 * Note that the first 24 bytes will be reserved by the log app itself - the size of cicular buffer structure [typedef ramp_data_App_s] ramp_data_App.
 *
 * DEBUG_DATA_SIZE_IN_BYTES is the maximum size of all logs in bytes. It should include 24 bytes required by the ramp data app and maximum
 * shouldn't exceed SCRATCHPAD_SIZE_IN_BYTES.
 * Also each log has a 4 byte header so , for ex., if the user is trying to log 4x [int32_t] values 10 times the size should be more than:
 * size = 24[log app struct] + (4[size of log]*4bytes[int32_t] + 4byte[header]) * 10 = 224 bytes
 *
 * CAPTURE_LOG_MAX_PAYLOAD is the maximum payload of a log in number of 32-bit words
 *
 */

//#define DEBUG_DATA_BUFFER_BASE (DRIVER_COMMON_STRUCT.scratchpad) //scratchpad
//#define DEBUG_DATA_SIZE_IN_BYTES (24 + 1024)  //24bytes required for debug app structure
//#define CAPTURE_LOG_MAX_PAYLOAD   (uint8_t)((uint8_t)2 << (5-1)) // The maximum payload of a log in number of 32-bit words

#ifdef log_telemetry_data
#include "shasta_hal_telem.h"
#include "user_app.h"  // user log error
#endif

#ifdef read_log_data_by_pmbus
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#endif

#ifdef State_machine_debug
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "cmsis_os_ext.h"               // ARM::CMSIS:RTOS:Keil RTX ext
#include "cmsis_os_patch.h"             // osMessageGet_patch
#include "rt_TypeDef.h"
#include "rt_Mailbox.h"
#include <stdint.h>             /* uint*_t */
#include "shasta.h"
#include "sys_boot_descr_format.h"  /* boot descriptor bitfield definitions */
#include "regulation_api.h"
#include "regulation_drv.h"
#include "patch_rom_table.h"
#endif

#ifdef log_telemetry_data
uint32_t telemetry_data[6];
#endif

/*lint -e522 */
void log_debug_data_init(void)
{
#ifdef debug_example
	log_debug_data();
#endif

#ifdef log_telemetry_data
	memset(DEBUG_DATA_BUFFER_BASE, 0, DEBUG_DATA_SIZE_IN_BYTES);
	Debug_data_buffer_reset();
	telemetry_data_sample(PMBUS_PAGE_0, 1); //count = 1
	telemetry_data_sample(PMBUS_PAGE_1, 2); //count = 2
#endif

#ifdef State_machine_debug
	State_machine_debug_init();
#endif

}

#ifdef debug_example
/**
 * Example of a function that writes to scratchpad some debug data.
 * This can be viewed from the GUI in the scratchpad area
 */
void log_debug_data(void)
{
	///////////////debug using debug data app (circular buffer) example//////////////
    uint16_t count;
    PMBUS_PAGE_t loop;
    uint32_t debug_data_array[8];

	memset(DEBUG_DATA_BUFFER_BASE, 0, DEBUG_DATA_SIZE_IN_BYTES);
	Debug_data_buffer_reset();
	debug_data_array[0] = 0x111111;
	debug_data_array[1] = 0x222222;
	debug_data_array[2] = 0x333333;
	debug_data_array[3] = 0x444444;
	debug_data_array[4] = 0x555555;
	debug_data_array[5] = 0x666666;
	debug_data_array[6] = 0x777777;
	debug_data_array[7] = 0x888888;
	//The number of 4-byte words of the log's content
	loop = PMBUS_PAGE_0;
	count = 0;
	uint8_t length = sizeof(debug_data_array)/sizeof(uint32_t);
	(void)Debug_data_buffer_write(loop, count, length, (uint32_t *)debug_data_array);

	debug_data_array[0] = 0x999999;
	debug_data_array[1] = 0xAAAAAA;
	loop = PMBUS_PAGE_0;
	count = 1;
	length = 2;
	(void)Debug_data_buffer_write(loop, count, length, (uint32_t *)debug_data_array);

	///////////////debug example//////////////
}
#endif

#ifdef log_telemetry_data
/**
 * Convenience function for creating ramp data log from telemetry.
 * To be modified with telemetry values of interest
 * @param loop      The loop from which the log was created (see PMBUS_PAGE_t)  Valid range of 0 or 1.
 * @param count     Ramp step count
 */
/*lint -e715 */
void telemetry_data_sample(PMBUS_PAGE_t loop, uint16_t count)
{

	uint16_t vout = (uint16_t)TELEM_TLM_VOUT_FSW__GET((telem_idx_t)loop);// lsb = 1.25mv u12.0
	uint16_t iout = (uint16_t)TELEM_TLM_IOUT_FSW__GET((telem_idx_t)loop);// S9.4, LSB 62.5mA
	uint16_t vin  = (uint16_t)TELEM_TLM_VIN_LPF__GET((telem_idx_t)loop); //u7.4
	uint16_t iin  = (uint16_t)TELEM_TLM_IIN_LPF__GET((telem_idx_t)loop);
	uint16_t duty = (uint16_t)TELEM_TLM_DUTY_FSW__GET((telem_idx_t)loop); // u0.16
	uint16_t freq = (uint16_t)TELEM_TLM_FREQ__GET((telem_idx_t)loop);     // u11.0 format 1khz lsb

	telemetry_data[0] = vout;
	telemetry_data[1] = iout;
	telemetry_data[2] = vin;
	telemetry_data[3] = iin;
	telemetry_data[4] = duty;
	telemetry_data[5] = freq;

	//The number of 4-byte words of the log's content
	uint8_t length = sizeof(telemetry_data)/sizeof(uint32_t);

	boolean status = Debug_data_buffer_write(PMBUS_PAGE_0, count, length, (uint32_t *)telemetry_data);

	if (status == boolean_false)
	{
		USER_Log_Error(PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
	}
}
#endif

#ifdef read_log_data_by_pmbus
/*
 * Example pmbus command handler implementation
 * NOTE: put this pmbus command into spreadsheet and copy this handler to pmbus_mfr_autogen.c
 *
 */
/*lint -e715 */
void PMBUS_HANDLE_MFR_RAMP_DATA(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	// update only if a read:
	if(direction==PMBUS_READ)
	{
		boolean log_is_not_empty;
		uint32_t cached_payload[CAPTURE_LOG_MAX_PAYLOAD];
		Capture_Log_t log;
		log.header = 0;
		log.payload = &cached_payload[0];
		log_is_not_empty = Debug_data_buffer_read(&log);
		if (log_is_not_empty != 0)
		{
			//uncomment this line after pmbus command is generated by spreadsheet
			//memcpy(PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_RAMP_DATA), &log, sizeof(Capture_Log_t));
		}
	}
}
#endif

#ifdef State_machine_debug

/*lint -esym(526, REGULATION_APP_QUEUE) */
extern regulation_inter_thread_message_t REGULATION_APP_QUEUE[REGULATION_QUEUE_LENGTH];
/*lint -esym(526, regulation_inter_thread_queue) */
extern osMessageQId regulation_inter_thread_queue;//
/*lint -esym(526, regulation_inter_thread_mpool) */
extern osPoolId regulation_inter_thread_mpool;//
uint32_t debug_data[8];
uint16_t debug_data_index;

/*lint -e522 */
void State_machine_debug_init (void)
{

	memset(DEBUG_DATA_BUFFER_BASE, 0, DEBUG_DATA_SIZE_IN_BYTES);
	Debug_data_buffer_reset();
	debug_data_index = 0;
	//debug_data[0] = 0xFFFFFFFF;
	//debug_data[1] = 0xFFFFFFFF;
	//debug_data[2] = 0xFFFFFFFF;
	//(void)Debug_data_buffer_write(PMBUS_PAGE_0, debug_data_index, 1, (uint32_t *)debug_data);
	//(void)Debug_data_buffer_write(PMBUS_PAGE_0, debug_data_index, 3, (uint32_t *)debug_data);
	//patch_rom_table.patch_Regulation_Wait_For_SM_Message = &patch_Regulation_Wait_For_SM_Message;
	patch_rom_table.patch_Regulation_Get_All_Q_Messages_By_Priority = &patch_Regulation_Get_All_Q_Messages_By_Priority;
}

void patch_Regulation_Get_All_Q_Messages_By_Priority (void)
{
	osEvent evt;
	regulation_inter_thread_message_t *message;

	// first we read the rtos queue count to see how many messages exist
	REGULATION_APP.mbx_cnt = rt_mbx_count(regulation_inter_thread_queue);

	// then we pop all the queue messages to our local queue to rearrange priority
	for (uint8_t i=0; i<REGULATION_APP.mbx_cnt; i++)
	{
		// if the count is less than our queue size (they should be equal unless someone changed defines)
		if (REGULATION_APP.mbx_size<REGULATION_QUEUE_LENGTH)
		{
			// pop from queue
			evt = osMessageGet(regulation_inter_thread_queue, 0);
			// check that the status is ok
			if (evt.status == osEventMessage)
			{
				// get the message pointer
				message = (regulation_inter_thread_message_t*)evt.value.p;
				// increment the local queue write pointer and current size
				// store the message in our local queue
				if (message != NULL )
				{
					REGULATION_APP_QUEUE[REGULATION_APP.rear].loop =	message->loop;
					REGULATION_APP_QUEUE[REGULATION_APP.rear].control = message->control;
					REGULATION_APP_QUEUE[REGULATION_APP.rear].control_data = message->control_data;
				}
				REGULATION_APP.rear++;
				REGULATION_APP.mbx_size++;
				// wrap the write pointer when at the queue size back to 0
				if (REGULATION_APP.rear >= REGULATION_QUEUE_LENGTH)
				{
					REGULATION_APP.rear = 0;
				}
				evt.status = osPoolFree(regulation_inter_thread_mpool, message);  // free the pool
			}
		}
	}

	regulation_inter_thread_message_t high_priority_control;
	uint32_t high_priority_index = REGULATION_APP.front;
	high_priority_control.control = (REGULATION_CONTROLS)0xff;
	// now we have our local queue of all the rtos queued messages.
	// now search it for the highest priority message and store its index and data locally
	for(uint32_t y=0; y<REGULATION_APP.mbx_size; y++)
	{
		uint32_t queue_index = (REGULATION_APP.front +y) & (REGULATION_QUEUE_LENGTH-1);  //%32
		if(REGULATION_APP_QUEUE[queue_index].control<high_priority_control.control )
		{
			high_priority_control = REGULATION_APP_QUEUE[queue_index];
			high_priority_index = queue_index;
		}
	}
	if (REGULATION_APP.mbx_size!=0)
	{
		// so here we need to decrement from high priority index back to front
		// if queue index is equal to 0 currently set to 31 in next loop iteration
		// otherwise decrement until qidx == front
		// since we push to front the highest priority we need to
		// shift everything back until we get to the message we replaced:
		// note that high_priority_index has already been stored above so we write to that location first
		for(uint32_t qidx = high_priority_index; qidx!=REGULATION_APP.front; qidx = (qidx==0) ? (REGULATION_QUEUE_LENGTH-1): (qidx-1))
		{
			uint32_t next_index = (qidx==0) ? (REGULATION_QUEUE_LENGTH-1): (qidx-1);  // 1 index below the current q index
			REGULATION_APP_QUEUE[qidx] = REGULATION_APP_QUEUE[next_index];
		}
		REGULATION_APP_QUEUE[REGULATION_APP.front] = high_priority_control;  // finally put the highest priority at the front
	}

	//Pop the last highest priority message
	REGULATION_APP.loop =	REGULATION_APP_QUEUE[REGULATION_APP.front].loop;
	REGULATION_APP.control[REGULATION_APP.loop] = REGULATION_APP_QUEUE[REGULATION_APP.front].control;
	REGULATION_APP.control_data[REGULATION_APP.loop] = REGULATION_APP_QUEUE[REGULATION_APP.front].control_data;

	///////////////debug using ramp data circular buffer example//////////////
	if (REGULATION_APP.control[REGULATION_APP.loop] != REGULATION_CONTROLS_TELEMETRY_UPDATED)
	{
		debug_data[0] = (uint32_t)REGULATION_APP.current_state[REGULATION_APP.loop];
		debug_data[1] = (uint32_t)REGULATION_APP.next_state[REGULATION_APP.loop];
		debug_data[2] = (uint32_t)REGULATION_APP.control[REGULATION_APP.loop];
		debug_data[3] = (uint32_t)REGULATION_APP.control_data[REGULATION_APP.loop];

		//The number of 4-byte words of the log's content
		debug_data_index++;
		uint8_t length = 4; //sizeof(debug_data)/sizeof(uint32_t);
		(void)Debug_data_buffer_write(REGULATION_APP.loop, debug_data_index, length, (uint32_t *)debug_data);
	}
	//////////////////////////debug using ramp data circular buffer example

	// increment read pointer and update queue size, wrap pointer to 0 when greater than queue size:
	REGULATION_APP.front++;  // looking for a number between 0 - 31 when REGULATION_QUEUE_LENGTH == 32
	REGULATION_APP.mbx_size--; // looking for a number between 0 - 32 when REGULATION_QUEUE_LENGTH == 32
	if (REGULATION_APP.front >= REGULATION_QUEUE_LENGTH)
	{
		REGULATION_APP.front = 0;
	}
}
#endif

