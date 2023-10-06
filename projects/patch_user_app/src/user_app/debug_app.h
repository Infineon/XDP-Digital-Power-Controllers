/*
 * debug_data.h
 *
 *  Created on: March 28, 2018
 *      Author: ashendar
 */

/* Standard library */
#include <stdint.h>     // uint*_t
#include <string.h>     // memset
#include "fw_bool.h"    // boolean type
#include "pmbus_autogen.h"      // PMBUS_PAGE_t
#include "pmbus_mfr_autogen.h"
#include "circular_buffer.h"    // The log buffer is based on a circular buffer
#include "debug_app_def.h"    // The log buffer is based on a circular buffer

/**
 *
 *
 *
 */
#ifdef debug_app

typedef struct
{
    /** The log entries are contained in a circular buffer */
    Circ_Buffer_t logs;

    /** Determines if a new debug_data overwrites the oldest one(s) if there is not
     * enough space left */
    boolean overwrite_is_enabled;
    /** Determines if debug_data is enabled
     */
} Capture_Buffer_t;

/** Type of logs */
typedef struct
{
    /** Contains the meta information of the log.
     * - counter (8bit)
     * - the number of 16-bit words of the content (8bit)
     * - a timestamp in nano seconds (16bit, 0..65536ns)
     * Instead of single variables one 32-bit header is used to set a constant
     * frame for this information. */
    uint32_t header;

    /** Payload in multiples of 16-bit words */
    uint32_t* payload;

} Capture_Log_t;

typedef struct
{
    /** The single instance of the debug_data buffer. */
	Capture_Buffer_t* Debug_data_buffer;

    uint8_t enable_debug_data[NUM_LOOPS];

    uint16_t debug_step_cnt[NUM_LOOPS];     //step counter
    uint16_t debug_step_cnt_max[NUM_LOOPS]; //number of steps in a debug - used to deactivate the timer

} debug_data_App_s;

extern debug_data_App_s debug_data_App;

/* Field positions and masks for packing the header and extracting its fields */
#define CAPTURE_HEADER_COUNT_POS      0
#define CAPTURE_HEADER_LOOP_POS       (CAPTURE_HEADER_COUNT_POS+8)
#define CAPTURE_HEADER_LENGTH_POS     (CAPTURE_HEADER_LOOP_POS +4)
#define CAPTURE_HEADER_TIMESTAMP_POS  (CAPTURE_HEADER_LENGTH_POS+4)

#define CAPTURE_HEADER_COUNT_MASK     (uint32_t) 0xFF // 8 bits
#define CAPTURE_HEADER_LOOP_MASK      (uint32_t) 0xF // 4 bits
#define CAPTURE_HEADER_LENGTH_MASK    (uint32_t) 0xF // 4 bits
#define CAPTURE_HEADER_TIMESTAMP_MASK (uint32_t) 0xFFFF // 16 bits

uint32_t capture_log_header_create(PMBUS_PAGE_t loop, uint16_t count, uint8_t length, uint16_t timestamp);
uint8_t capture_log_length_get(const Capture_Log_t* log);
void capture_log_overwrite_config(boolean is_enabled);

void Debug_data_buffer_reset(void);
boolean Debug_data_buffer_write(PMBUS_PAGE_t loop, uint16_t count, uint8_t length, void* payload);
boolean Debug_data_buffer_read(Capture_Log_t* log);

#endif
