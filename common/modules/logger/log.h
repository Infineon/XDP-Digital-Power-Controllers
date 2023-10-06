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
 * @file    log.h
 * @brief   API for creating logs.
 */

#ifndef LOG_H
#define LOG_H

/* Data types */
#include <stdint.h>     /*  uint*_t */

/* ============================================================================
 * Public data types
 * ========================================================================= */

/** The maximum payload of a log in number of 32-bit words */
#define LOG_MAX_PAYLOAD   (uint8_t)((uint8_t)2 << (5-1))

/** Type of logs */
typedef struct
{
    /** Contains the meta information of the log.
     * - the source ID indicating where the log was created (8bit, see Log_Source_t)
     * - the severity level (3bit, see Log_Severity_t)
     * - the number of 32-bit words of the content (5bit => 128Byte content)
     * - a timestamp in nano seconds (16bit, 0..65536ns)
     * Instead of single variables one 32-bit header is used to set a constant
     * frame for this information. */
    uint32_t header;

    /** Payload in multiples of 32-bit words */
    uint32_t* payload;

} Log_t;

/** Firmware-wide list of sources which indicate the thread in which the log was created. */
typedef enum
{
    /** Indicates an empty log or a yet unspecified origin */
    UNKNOWN_SOURCE = 0,
    /** Indicates the boot sequence as the log's origin */
    BOOT_SEQUENCE,
    /** Indicates RTOS internals and inter-thread communication as the log's origin */
    OS_KERNEL,
    /** Indicates one of the interrupt request handlers as the log's origin */
    IRQ_HANDLER,
    /** Indicates one of the job_agency's threads as the log's distributor*/
    JOB_AGENCY_THREAD,
    /** Indicates the os_timer thread as the log's distributor*/
    TIMER_THREAD,
	/** Indicates pmbus as the log's distributor*/
	PMBUS,
	/** Indicates regulation as the log's distributor*/
	REGULATION,
	/** Indicates faults block as the log's distributor*/
	FAULTS,
	/** Indicates telemetry as the log's distributor*/
	TELEMETRY,
    /** Indicates a failure when deleting a mail from the ITC mail-pool*/
    ITC_MAIL,
    /** Indicates a failure in the Memory Manager */
    MMU,
    /** Indicates a failure in the OTP Filesystem  */
    OTP_FS,
    /** Indicates a failure in the patching function  */
    PATCHER,
    /** Indicates a failure in the configurator  */
    CONFIGURATOR,
	/** Indicates a failure in the user patch  */
	USER,
	/** Indicates a failure in the scratchpad semaphore  */
	SCRATCHPAD,
	/** Indicates a failure in the uart  */
	UART,
    /** The total number of different source IDs */
    LOG_NUM_SOURCES,

} Log_Source_t;


/*lint -esym(769, INFO) -esym(769, ERROR)
 * Info 769: global enumeration constant not referenced.
 * Part of the exported library. */

/** The severity levels define the handling of an exception at top-level. They can also be used
 * for filtering logs.
 * @note The naming of the severity levels is taken from the
 * <a href="https://tools.ietf.org/html/rfc5424">Syslog Protocol</a>. */
typedef enum
{
    /** Logging in the normal program flow; <b>not used for an exception</b> */
    INFO = 0,
    /** Error which can be corrected/ignored */
    WARNING,
    /** Error within a single module; cannot be corrected/ignored */
    ERROR,
    /** Error within the firmware; cannot be corrected/ignored */
    CRITICAL,
    /** Unrecoverable error within the firmware */
    EMERGENCY,
    /** The total number of different severity levels */
    LOG_NUM_SEVERITY

} Log_Severity_t;


/* ============================================================================
 * Function declarations
 * ========================================================================= */

/**
 * Returns a packed header for a log with the given values.
 * @param source    The source from which the log was created
 * @param severity  The severity level for the log
 * @param length    The number of 32-bit words of the log's payload
 * @param timestamp A timestamp for the log
 * @return The packed 32-bit header.
 */
uint32_t log_header_create(Log_Source_t source, Log_Severity_t severity, uint8_t length, uint16_t timestamp);

/**
 * Returns the source ID from the packed header of the log.
 * @param log   Log whose header will be read
 * @return the source ID of the log
 */
uint8_t log_source_get(const Log_t* log);

/**
 * Returns the severity level from the packed header of the log.
 * @param log   Log whose header will be read
 * @return the severity level of the log
 */
uint8_t log_severity_get(const Log_t* log);

/**
 * Returns the length from the packed header of the log.
 * @param log   Log whose header will be read
 * @return the number of 32-bit words of the log's payload
 */
uint8_t log_length_get(const Log_t* log);

/**
 * Returns the timestamp from the packed header of the log.
 * @param log   Log whose header will be read
 * @return the timestamp of the log
 */
uint16_t log_timestamp_get(const Log_t* log);

#endif /* LOG_H */
