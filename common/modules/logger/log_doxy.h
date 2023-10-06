/* ============================================================================
** Copyright (C) 2015 Infineon Technologies AG
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
 * @file    log_doxy.h
 * @brief   Doxygen overview of the Logger.
 */

#ifndef LOG_DOXY_H
#define LOG_DOXY_H

/** @page pgShasta_Logger   Logger FW Module

@section sctShasta_Logger_Intro Introduction
The Logging FW Module records the activity within the Shasta device for later analysis.<br>
The produced @b logfile or rather list of log entries is seen as a diary of the program flow.
It is @b not used to record feature specific values (e.g. SVID profile data) which need
to be accessed directly. For that purpose there are dedicated memory areas and dedicated
PMBus commands.

A use cases for analyzing the logfile is, for instance, finding errors in the program flow
in case the Shasta behaves unexpectedly (e.g. "A PMBus command was not executed.").
Another use case could be to search for optimization possibilities by analyzing the sequence of
states in the program flow.
@note As of April 2015 only errors are logged by the FW. For the use case of optimization
additionally the internal states (e.g. Execute SVID use case X, Patch was read, ...) need
to be logged.


@section sctShasta_Log_File   Log file
The logfile is a list of <b>log entries</b> in a configurable circular buffer referred to as
<b>log buffer</b>. The logfile and the buffer are reset resistant, meaning upon a reset
(e.g. due to an error or a patch) all of the log entries and control data will remain
unchanged.<br>
The log entries are implemented in a generic way to be prepared for upcoming use cases. This
means each log entry consists of a header and an arbitrary length or rather number of 4 byte
payload chunks (see @ref Log_t). Besides the length information the header contains the information
@ref Log_Source_t, @ref Log_Severity_t and a timestamp which indicates the system clock.
@image html log_buffer.svg "Content of the log buffer"


@section sctShasta_Logger_Interface   Module Interface
- The logfile needs to be read log-wise, meaning each log entry is read by first extracting the
  length from the header and then reading the payload chunks with <br>
  log_pmbus_cmd_log_read().
- The log buffer is configurable. It can be set to
  - let new logs @b overwrite older ones in case the buffer is full with <br>
    log_pmbus_cmd_log_overwrite_config().
  - <b>filter out</b> new log entries by their source and severity with <br>
    log_pmbus_cmd_log_filter_source_config() and log_pmbus_cmd_log_filter_severity_config().
 */


#endif /* LOG_DOXY_H */
