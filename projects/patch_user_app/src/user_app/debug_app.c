/*
 * debug_data.c
 *
 *  Created on: March 28, 2018
 *      Author: ashendar
 */


/**
 * Debug data app provides a circular buffer api functions for logging data on scratchpad.
 * This data can be read through the GUI, debugger or pmbus command.
 * debug data in scratchpad ram and limited by the size of scratchpad in ram
 * See log_debug_data_examples.c for usage examples
 *
 */
/*lint -e766 */
#include "debug_app_def.h"    // The log buffer is based on a circular buffer

#ifdef debug_app


/* Compiler configurations first */
#include "debug_app.h"

#include "fw_config.h"          // Essential fw-wide configuration for e.g. buffer size #defines
#include "environment.h"        // STATIC, RESET_RESISTANT_DECLARE
//#include "linker_sections.h"    // Linker specific memory sections

/* Always compare to the associated interface */
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
#include "pmbus_api.h"


/** Type of a Debug_data buffer. It is a special circular buffer which can be configured arbitrarily. */

debug_data_App_s debug_data_App;

/**
 * Resets the log buffer of the debug data buffer.
 */
void Debug_data_buffer_reset(void)
{
    /*lint -e{826} Suppress once: Suspicious pointer-to-pointer conversion (area too small).
     * The area really is big enough. Log_Buffer_t + a lot of space for logs. */
	debug_data_App.Debug_data_buffer = (Capture_Buffer_t*) DEBUG_DATA_BUFFER_BASE;

    /* Set all configurations to default values. */
    memset(debug_data_App.Debug_data_buffer, 0, sizeof(Capture_Buffer_t));
    /* Prepare to put all log entries after the log buffer control structure.
     * Note: Padding bytes for structures are automatically inserted. */
    Circ_Buffer_Element_t* p_start_logs = (Circ_Buffer_Element_t*)(
            (uint32_t)debug_data_App.Debug_data_buffer + sizeof(Capture_Buffer_t) );

    uint32_t length_logs = ( ((uint32_t)debug_data_App.Debug_data_buffer + DEBUG_DATA_SIZE_IN_BYTES)
            - (uint32_t)p_start_logs )
            / sizeof(Circ_Buffer_Element_t);

    /* Clear the log buffer by resetting the control data of the circular buffer. */
    circ_buffer_init(&debug_data_App.Debug_data_buffer->logs, length_logs, p_start_logs);

}


/*lint -esym(714, log_write) -esym(759, log_write) -esym(765, log_write)
 *
 * */
/**
 * Stores a new log at the end of the buffer if enough space is left.<br>
 * A timestamp will be added internally.
 * @param loop      The loop from which the log was created (see PMBUS_PAGE_t)  Valid range of 0 or 1.
 * @param count     debug step count
 * @param length    The number of 4-byte words of the log's @p content
 * @param payload   The payload for the log made of 32-bit chunks (will be copied)
 * @return @c true in case the log was written or otherwise @c false.
 */

boolean Debug_data_buffer_write(PMBUS_PAGE_t loop, uint16_t count, uint8_t length, void* payload)
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

    uint32_t memory_left = (debug_data_App.Debug_data_buffer->logs.size - debug_data_App.Debug_data_buffer->logs.count)
                           * sizeof(Circ_Buffer_Element_t);

    if (memory_needed > memory_left)
    {
        /* Remove oldest entry(s) if desired. */
        /*lint -e{638, 839} Strong type mismatch for type '(uint32_t*uint32_t)' in relational -- Unclear*/
        if (debug_data_App.Debug_data_buffer->overwrite_is_enabled
                && ( memory_needed <= (debug_data_App.Debug_data_buffer->logs.size * (uint32_t)sizeof(Circ_Buffer_Element_t))) )
        {
            /* Remove logs from the buffer until enough space is left. */
            while (memory_left < memory_needed)
            {
            	Capture_Log_t oldest_log;
                oldest_log.header = (uint32_t) circ_buffer_pop(&debug_data_App.Debug_data_buffer->logs);
                uint8_t old_log_length = capture_log_length_get(&oldest_log);

                circ_buffer_elements_remove(&debug_data_App.Debug_data_buffer->logs, (uint32_t)old_log_length);
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
    circ_buffer_push(&debug_data_App.Debug_data_buffer->logs, (Circ_Buffer_Element_t) log.header);
    for (uint8_t i = 0; i < length; i++)
    {
        circ_buffer_push(&debug_data_App.Debug_data_buffer->logs, (Circ_Buffer_Element_t) log.payload[i]);
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
boolean Debug_data_buffer_read(Capture_Log_t* log)

{
    // Do not throw an exception. The developer should take care.
    if (log == NULL) return boolean_false;

    if (circ_buffer_is_empty(&debug_data_App.Debug_data_buffer->logs) || (log->payload == NULL))
    {
        log->header = 0;
        return boolean_false;
    }
    else
    {
        log->header = (uint32_t) circ_buffer_pop(&debug_data_App.Debug_data_buffer->logs);

        uint8_t log_length = capture_log_length_get(log);
        for (uint8_t i = 0; i < log_length; i++)
        {
            log->payload[i] = (uint32_t) circ_buffer_pop(&debug_data_App.Debug_data_buffer->logs);
        }

        return boolean_true;
    }
}

/*lint -esym(714, log_overwrite_config) -esym(759, log_overwrite_config) -esym(765, log_overwrite_config)
* This function is currently not used, but we keep it for future use. The linker will remove
* used symbols anyway. */
void capture_log_overwrite_config(boolean is_enabled)
{
	debug_data_App.Debug_data_buffer->overwrite_is_enabled = is_enabled;
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


#endif
