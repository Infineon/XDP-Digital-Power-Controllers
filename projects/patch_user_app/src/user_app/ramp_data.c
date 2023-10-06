/*
 * ramp_data.c
 *
 *  Created on: March 28, 2018
 *      Author: ashendar
 */


/**
 *
 * snapshot control in scratchpad ram and limited otp storage of upto 5 snapshots lifetime
 *
 */

/* Compiler configurations first */
#include "fw_config.h"          // Essential fw-wide configuration for e.g. buffer size #defines
#include "environment.h"        // STATIC, RESET_RESISTANT_DECLARE
//#include "linker_sections.h"    // Linker specific memory sections

/* Always compare to the associated interface */
#include "ramp_data.h"

/* Standard library */
#include <stdint.h>     // uint*_t
#include <string.h>     // memset
#include "fw_bool.h"    // boolean type

/* APIs */
#include "cmsis_os_ext.h"       // CMSIS-RTOS functions, incl. exception handling
#include "circular_buffer.h"    // The log buffer is based on a circular buffer

#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"
#include "dtimer_drv.h"
#include "regulation_state_machine_callbacks.h"
#include "telemetry_api.h"
#include "telemetry_irq_handlers.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "add_on_features.h"

STATIC DTIMER_CONFIG_t dtimer4_cfg;
STATIC DTIMER_CONFIG_t dtimer5_cfg;

STATIC DRIVER_DTIMER *drv4_info;
STATIC DRIVER_DTIMER *drv5_info;

uint32_t telemetry_data[6];
/** Type of a ramp_data buffer. It is a special circular buffer which can be configured arbitrarily. */

ramp_data_App_s ramp_data_App;

void DTIMER4_Ramp_Data_Timer_IRQHandler(void);
void DTIMER5_Ramp_Data_Timer_IRQHandler(void);
void Ramp_Data_Timer_Activation (PMBUS_PAGE_t loop, uint32_t load, uint32_t bgload);

void Ramp_Data_Timer_Activation (PMBUS_PAGE_t loop, uint32_t load, uint32_t bgload)
{

	if(loop == PMBUS_PAGE_0) {
		drv4_info = &Driver_DTIMER;
    	memset(&dtimer4_cfg, 0, sizeof(dtimer4_cfg));
    	dtimer4_cfg.mode = PERIODIC_INT;
    	dtimer4_cfg.load = load; //first interrupt
    	dtimer4_cfg.bgload = bgload; //consecutive periodic interrupts
    	dtimer4_cfg.dtimer_ISR = DTIMER4_Ramp_Data_Timer_IRQHandler;
    	dtimer_config (DTIMER4, dtimer4_cfg);
//    	drv4_info->Initialize(DTIMER4, dtimer4_cfg);
	}
	else { //DTIMER1
		drv5_info = &Driver_DTIMER;
		memset(&dtimer5_cfg, 0, sizeof(dtimer5_cfg));
		dtimer5_cfg.mode = PERIODIC_INT;
		dtimer5_cfg.load = load; //first interrupt
		dtimer5_cfg.bgload = bgload; //consecutive periodic interrupts
		dtimer5_cfg.dtimer_ISR = DTIMER5_Ramp_Data_Timer_IRQHandler;
		dtimer_config (DTIMER5, dtimer5_cfg);
//		drv5_info->Initialize(DTIMER5, dtimer5_cfg);
	}
}

/**
 * Interrupt service routines for TON Delay and TOFF delay
*/
void DTIMER4_Ramp_Data_Timer_IRQHandler(void)
{
    drv4_info->ClearIRQ(DTIMER4);
	ramp_data_App.ramp_step_cnt[PMBUS_PAGE_0]++;
	telemetry_Ramp_data_sample(PMBUS_PAGE_0, ramp_data_App.ramp_step_cnt[PMBUS_PAGE_0]);

	if (ramp_data_App.ramp_step_cnt[PMBUS_PAGE_0] >= ramp_data_App.ramp_step_cnt_max[PMBUS_PAGE_0])
	{
		// The timer is deactivated after processing the interrupt:
//		dtimer_shutdown(DTIMER4);
		drv4_info->Uninitialize(DTIMER4);
	}
}

void DTIMER5_Ramp_Data_Timer_IRQHandler(void)
{
    drv5_info->ClearIRQ(DTIMER5);
	ramp_data_App.ramp_step_cnt[PMBUS_PAGE_1]++;
	telemetry_Ramp_data_sample(PMBUS_PAGE_1, ramp_data_App.ramp_step_cnt[PMBUS_PAGE_1]);

	if (ramp_data_App.ramp_step_cnt[PMBUS_PAGE_1] >= ramp_data_App.ramp_step_cnt_max[PMBUS_PAGE_1])
	{
		// The timer is deactivated after processing the interrupt:
//		dtimer_shutdown(DTIMER5);
		drv5_info->Uninitialize(DTIMER5);
	}
}
/*
 * This function executes in Regulation_Controls_Enable Control in Regulation State Ton Rise
 *
 */
void user_regulation_state_ton_rise_cb_regulation_controls_enable (PMBUS_PAGE_t loop)
{
	//Move this enable to PMBUS command
	//This example code stores ramp data for loop0
	if (loop == PMBUS_PAGE_0)
	{
		ramp_data_App.enable_ramp_data[loop] = 0; //enable here for example purpose
	}

	if ((loop == PMBUS_PAGE_0) && (ramp_data_App.enable_ramp_data[loop] == 1))
	{
		//int32_t status;
	    uint16_t delay;
	    uint32_t load;
	    //uint32_t bgload;
		uint32_t command;
		int32_t exponent;
		ramp_data_App.ramp_step_cnt[loop] = 0; // Reset ramp counter to 0
		//Number of steps depends on RAMP_DATA_SIZE_IN_BYTES
		//Capture_Buffer_t struct is stored at RAMP_DATA_BUFFER_BASE with sizeof(Capture_Buffer_t)
		//data is stored in 4-byte words with a header sizeof(uint32_t)
		//In this example we store 3 4-byte words (telemetry_data: (uint16_t)vout,iout,vin,iin,duty,freq)
		//ramp_step_cnt_max = (1024 - 24)/(3+1) = 250 steps
		//ramp_data_App.ramp_step_cnt_max[loop] = 10; //for debug
		ramp_data_App.ramp_step_cnt_max[loop] = (RAMP_DATA_SIZE_IN_BYTES - sizeof(Capture_Buffer_t))/
				((sizeof(telemetry_data)/sizeof(uint32_t)) + 1); // +1 (uint32_t)header
		Ramp_data_buffer_reset();
		command = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_TON_RISE);
		delay = (uint16_t)LINEAR11_TO_MANTISSA((uint16_t)command);
		exponent = LINEAR11_TO_EXPONENT((uint16_t)command);
		load = (uint32_t)(SHIFT_EXPONENT((100000*delay) , (exponent))/ramp_data_App.ramp_step_cnt_max[loop]);  // LSB for load is 10ns
		Ramp_Data_Timer_Activation(loop, load,load);  // use loop number as the timer number

	}
}
/**
 * Convenience function for creating ramp data log from telemetry.
 * To be modified with telemetry values of interest
 * @param loop      The loop from which the log was created (see PMBUS_PAGE_t)  Valid range of 0 or 1.
 * @param count     Ramp step count
 */
/*lint -e715 */
void telemetry_Ramp_data_sample(PMBUS_PAGE_t loop, uint16_t count)
{

	uint16_t vout = 0x1111; //(uint16_t)TELEM_TLM_VOUT_FSW__GET((telem_idx_t)loop);// lsb = 1.25mv u12.0
	uint16_t iout = 0x2222; //(uint16_t)TELEM_TLM_IOUT_FSW__GET((telem_idx_t)loop);// S9.4, LSB 62.5mA
	uint16_t vin  = 0x3333; //(uint16_t)TELEM_TLM_VIN_LPF__GET((telem_idx_t)loop); //u7.4
	uint16_t iin  = 0x4444; //(uint16_t)TELEM_TLM_IIN_LPF__GET((telem_idx_t)loop);
	uint16_t duty = 0x5555; //(uint16_t)TELEM_TLM_DUTY_FSW__GET((telem_idx_t)loop); // u0.16
	uint16_t freq = 0x6666; //(uint16_t)TELEM_TLM_FREQ__GET((telem_idx_t)loop);     // u11.0 format 1khz lsb

	telemetry_data[0] = vout;
	telemetry_data[1] = iout;
	telemetry_data[2] = vin;
	telemetry_data[3] = iin;
	telemetry_data[4] = duty;
	telemetry_data[5] = freq;

	//The number of 4-byte words of the log's content
	uint8_t length = sizeof(telemetry_data)/sizeof(uint32_t);

	boolean status = Ramp_data_buffer_write(PMBUS_PAGE_0, count, length, (uint32_t *)telemetry_data);

	if (status == boolean_false)
	{
		USER_Log_Error(PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
	}
}

/**
 * Resets the log buffer of the Ramp data buffer.
 */
void Ramp_data_buffer_reset(void)
{
    /*lint -e{826} Suppress once: Suspicious pointer-to-pointer conversion (area too small).
     * The area really is big enough. Log_Buffer_t + a lot of space for logs. */
	ramp_data_App.Ramp_data_buffer = (Capture_Buffer_t*) RAMP_DATA_BUFFER_BASE;

    /* Set all configurations to default values. */
    memset(ramp_data_App.Ramp_data_buffer, 0, sizeof(Capture_Buffer_t));
    /* Prepare to put all log entries after the log buffer control structure.
     * Note: Padding bytes for structures are automatically inserted. */
    Circ_Buffer_Element_t* p_start_logs = (Circ_Buffer_Element_t*)(
            (uint32_t)ramp_data_App.Ramp_data_buffer + sizeof(Capture_Buffer_t) );

    uint32_t length_logs = ( ((uint32_t)ramp_data_App.Ramp_data_buffer + RAMP_DATA_SIZE_IN_BYTES)
            - (uint32_t)p_start_logs )
            / sizeof(Circ_Buffer_Element_t);

    /* Clear the log buffer by resetting the control data of the circular buffer. */
    circ_buffer_init(&ramp_data_App.Ramp_data_buffer->logs, length_logs, p_start_logs);

}


/*lint -esym(714, log_write) -esym(759, log_write) -esym(765, log_write)
 *
 * */
/**
 * Stores a new log at the end of the buffer if enough space is left.<br>
 * A timestamp will be added internally.
 * @param loop      The loop from which the log was created (see PMBUS_PAGE_t)  Valid range of 0 or 1.
 * @param count     Ramp step count
 * @param length    The number of 4-byte words of the log's @p content
 * @param payload   The payload for the log made of 32-bit chunks (will be copied)
 * @return @c true in case the log was written or otherwise @c false.
 */

boolean Ramp_data_buffer_write(PMBUS_PAGE_t loop, uint16_t count, uint8_t length, void* payload)
{

    /* Retrieve the sysTick timestamp in case the RTOS has been started successfully. Otherwise
     * use a default timestamp. */
    uint16_t timestamp;
    boolean os_kernel_running = (boolean) osKernelRunning();
    if (os_kernel_running)
    {
        timestamp = (uint16_t)osKernelSysTick();
    }
    else
    {
        timestamp = LOG_DEFAULT_TIMESTAMP;
    }

    Capture_Log_t log = {capture_log_header_create(loop, count, length, timestamp), (uint32_t*) payload};

    /* Check if there is enough space left for a new log */
    uint32_t memory_needed;
    memory_needed = sizeof(log.header) + (length * sizeof(log.payload[0]) );

    uint32_t memory_left = (ramp_data_App.Ramp_data_buffer->logs.size - ramp_data_App.Ramp_data_buffer->logs.count)
                           * sizeof(Circ_Buffer_Element_t);

    if (memory_needed > memory_left)
    {
        /* Remove oldest entry(s) if desired. */
        /*lint -e{638, 839} Strong type mismatch for type '(uint32_t*uint32_t)' in relational -- Unclear*/
        if (ramp_data_App.Ramp_data_buffer->overwrite_is_enabled
                && ( memory_needed <= (ramp_data_App.Ramp_data_buffer->logs.size * (uint32_t)sizeof(Circ_Buffer_Element_t))) )
        {
            /* Remove logs from the buffer until enough space is left. */
            while (memory_left < memory_needed)
            {
            	Capture_Log_t oldest_log;
                oldest_log.header = (uint32_t) circ_buffer_pop(&ramp_data_App.Ramp_data_buffer->logs);
                uint8_t old_log_length = capture_log_length_get(&oldest_log);

                circ_buffer_elements_remove(&ramp_data_App.Ramp_data_buffer->logs, (uint32_t)old_log_length);
                memory_left += sizeof(oldest_log.header)
                               + (old_log_length * sizeof(oldest_log.payload[0]) );
            }
        }
        /* No overwrite mode or the payload exceeds the overall size: Do not push the log. */
        else
        {
            return boolean_false;
        }
    }

    /* Push the complete new log (header + payload). */
    circ_buffer_push(&ramp_data_App.Ramp_data_buffer->logs, (Circ_Buffer_Element_t) log.header);
    for (uint8_t i = 0; i < length; i++)
    {
        circ_buffer_push(&ramp_data_App.Ramp_data_buffer->logs, (Circ_Buffer_Element_t) log.payload[i]);
    }

    return boolean_true;
}

/*lint -esym(714, log_read) -esym(759, log_read) -esym(765, log_read)
*
* */
/**
 * Retrieves and removes the oldest log from the buffer.
 * @param log   Return value: The oldest log or a log with header 0 if the buffer was empty
 * @return @c true in case a log was read otherwise @c false.
 */
boolean Ramp_data_buffer_read(Capture_Log_t* log)

{
    // Do not throw an exception. The developer should take care.
    if (log == NULL) return boolean_false;

    if (circ_buffer_is_empty(&ramp_data_App.Ramp_data_buffer->logs) || (log->payload == NULL))
    {
        log->header = 0;
        return boolean_false;
    }
    else
    {
        log->header = (uint32_t) circ_buffer_pop(&ramp_data_App.Ramp_data_buffer->logs);

        uint8_t log_length = capture_log_length_get(log);
        for (uint8_t i = 0; i < log_length; i++)
        {
            log->payload[i] = (uint32_t) circ_buffer_pop(&ramp_data_App.Ramp_data_buffer->logs);
        }

        return boolean_true;
    }
}

/*lint -esym(714, log_overwrite_config) -esym(759, log_overwrite_config) -esym(765, log_overwrite_config)
* This function is currently not used, but we keep it for future use. The linker will remove
* used symbols anyway. */
void capture_log_overwrite_config(boolean is_enabled)
{
	ramp_data_App.Ramp_data_buffer->overwrite_is_enabled = is_enabled;
}

uint32_t capture_log_header_create(PMBUS_PAGE_t loop, uint16_t count, uint8_t length, uint16_t timestamp)
{
    return (( (uint32_t)timestamp & CAPTURE_HEADER_TIMESTAMP_MASK) << CAPTURE_HEADER_TIMESTAMP_POS)
            | (( (uint32_t)length & CAPTURE_HEADER_LENGTH_MASK) << CAPTURE_HEADER_LENGTH_POS)
            | (( (uint32_t)loop & CAPTURE_HEADER_LOOP_MASK) << CAPTURE_HEADER_LOOP_POS)
            | (( (uint32_t)count & CAPTURE_HEADER_COUNT_MASK));
}

uint8_t capture_log_length_get(const Capture_Log_t* log)
{
    return (uint8_t)((log->header >> CAPTURE_HEADER_LENGTH_POS) & CAPTURE_HEADER_LENGTH_MASK);
}

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
		log_is_not_empty = Ramp_data_buffer_read(&log);
		if (log_is_not_empty != 0)
		{
			//uncomment this line after pmbus command is generated by spreadsheet
			//memcpy(PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_RAMP_DATA), &log, sizeof(Capture_Log_t));
		}
	}
}

