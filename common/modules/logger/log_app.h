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
 * @file    log_app.h
 * @brief   API of the Logging FW Module.
 */

#ifndef LOG_APP_H
#define LOG_APP_H

/* Data types */
#include "fw_bool.h"        // boolean type
#include "log.h"            // Log_t
#include "cmsis_os_ext.h"   // CMSIS-RTOS functions, incl. exception handling


/* ============================================================================
 * Public data types
 * ========================================================================= */

/** Time stamp for a log in case the RTOS is not running */
#define LOG_DEFAULT_TIMESTAMP   (uint16_t)0xFFFF


/* ============================================================================
 * Function declarations
 * ========================================================================= */

/**
 * "Overloaded" convenience function for creating an error log from an error type. This function
 * can be used if an error was recognized without the necessity to throw an exception and thus to
 * abort the calling function. The program counter will be added internally.
 * @param source    The source from which the log was created (see Log_Source_t)
 * @param severity  The severity level for the log (see Log_Severity_t)
 * @param type      The error type from which an exception will be generated
 */
#define LOG_ERROR_TYPE(source, severity, type) \
    osException_t exception = {(osError_t)type, PROGRAM_COUNTER}; \
    log_error(source, severity, &exception)

/**
 * Resets the log buffer of the logging module.
 */
void log_buffer_reset(void);

/**
 * Stores a new log at the end of the buffer if enough space is left.<br>
 * A timestamp will be added internally.
 * @param source    The source from which the log was created (see Log_Source_t)
 * @param severity  The severity level for the log (see Log_Severity_t)
 * @param length    The number of 4-byte words of the log's @p content
 * @param payload   The payload for the log made of 32-bit chunks (will be copied)
 * @return @c true in case the log was written or otherwise @c false.
 */
boolean log_write(Log_Source_t source, Log_Severity_t severity, uint8_t length, const uint32_t* payload);

/**
 * Convenience function for creating an error log from an exception.
 * @param source    The source from which the log was created (see Log_Source_t)
 * @param severity  The severity level for the log (see Log_Severity_t)
 * @param exception The exception to be logged (contains the error/cause)
 */
void log_error(Log_Source_t source, Log_Severity_t severity, const osException_t* exception);

/**
 * Retrieves and removes the oldest log from the buffer.
 * @param log   Return value: The oldest log or a log with header 0 if the buffer was empty
 * @return @c true in case a log was read otherwise @c false.
 */
boolean log_read(Log_t* log);

/**
 * Configures the log buffer to let a new log overwrite older logs in case there is not enough
 * space left.<br>
 * If a new log entry exceeds the available memory by default it would be discarded (overwriting
 * is disabled). If overwriting is enabled older logs will be removed until there is enough memory
 * available for the new log.
 * @param is_enabled    Enables or disables the overwrite mode
 */
void log_overwrite_config(boolean is_enabled);

/**
 * Disables or re-enables the recording of logs whose source ID matches the parameter
 * (see Log_Source_t).<br>
 * By default the logs from all sources are logged.<br>
 * Old logs with the indicated source ID will remain within the buffer until they are read out.
 * @param filter        Logs with this source ID will be rejected
 * @param is_enabled    Enables or disables filtering of the indicated @p filter
 */
void log_filter_source_config(Log_Source_t filter, boolean is_enabled);

/**
 * Disables or re-enables the recording of logs whose severity level matches the parameter
 * (see Log_Severity_t).<br>
 * By default the logs for all severity levels are logged.<br>
 * Old logs with the indicated severity level will remain within the buffer until they are read out.
 * @param filter        Logs with this severity level will be rejected
 * @param is_enabled    Enables or disables filtering of the indicated @p filter
 */
void log_filter_severity_config(Log_Severity_t filter, boolean is_enabled);

#endif /* LOG_APP_H */
