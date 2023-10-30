/*
 * user_app.h
 *
 *  Created on: Jun 13, 2017
 *      Author: dlewis4
 */

/** @page pgUser_App   User Example Code
 * The user application provides example code for various functions using the ROM linked API function calls.
 *
 * @section addon Add On Features
 * Features that user can enable via a define
 *
 * @ref retry_if_removed_bug_fix
 *
 * @ref en_ishare
 *
 * @image html add_on_features.png "Add On Features"
 *
 * @section periodic Periodic Functions
 * Functions to execute periodically based on switching frequency.
 * Example includes frequency jitter/dither and slowly ramping FSW during PCMC startup.
 *
 * @ref periodic_functions.c
 *
 * @image html periodic_functions.png "Periodic Functions"
 *
 * @section mfrauto Autogenerated PMBus Command Descriptors
 * Autogenerated output from make pmbus.
 * Takes spreadsheet and creates pmbus command handles.
 * This should not be modified by user.  Use make PMBus to create.  See @pgPMBUS_Guide for more details.
 *
 * @ref pmbus_mfr_autogen.c
 * @image html pmbus_mfr_autogen.png "Autogenerated PMBus Command Descriptors"
 *
 * @section mfrhandles User PMBus Command Callbacks
 * Example of implementing callback function (handlers) for user PMBus commands.
 * These are executed after PMBus transactions are received.
 *
 * @ref pmbus_mfr_specific_handlers.c
 * @image html pmbus_mfr_specific_handlers.png "User PMBus Command Callbacks"
 *
 * @section ramp_data Ramp Data Snapshot Examples
 * Example of how to implement blackbox snapshot features.
 *
 * @image html ramp_data.png "Ramp Data Snapshot Examples"
 *
 * @section regsmcbs Regulation State Machine Callback Functions
 * Examples of callback (handles) for regulation state machine events.
 * Several functions are defined for particular states examples:
 *
 * @ref TON_RISE_VID_REACHED
 *
 * @ref TON_RISE_ENABLE
 *
 * @ref AT_TARGET_ENABLE
 *
 * @ref AT_SHUTDOWN
 *
 * @ref regulation_state_machine_callbacks.c
 * @image html regulation_state_machine_callbacks.png "Regulation State Machine Callback Functions"
 *
 * @section usrdrv Main User Application
 * Local patch entry function equivalent to typical main entry point.
 *
 * @ref user_drv_init
 * @image html user_app.png "Main User Application"
 *
 * @section userntc User NTC Temperature Look Up Table
 * Example of a user implemented NTC lookup table.
 * Index of the array is the raw ADC code for TSEN A/B inputs.
 * Temperature is the return value of that ADC code.
 *
 * @ref user_ntc_temperature_lut.h
 * @image html user_ntc_temperature_lut.png "User NTC Temperature Look Up Table"
 *
 * @section DbgData Debug Data App
 *
 * Log debug data app provides a circular buffer and api for saving debug data scratchpad.
 * This data can be read through the GUI, debugger or pmbus command.
 * Use cases would be logging telemetry data, warnings or state machine states.
 *
 * Debug app is using a circular buffer
 * Each log entry contains a header and a number of 4 byte payload chunks. Header contains
 * count, loop and length of payload information.
 *
 * To use debug data app first uncomment #define debug_app in debuf_app_def.h and set the parameters to configure debug app:
 * RAMP_DATA_BUFFER_BASE, RAMP_DATA_SIZE_IN_BYTES and CAPTURE_LOG_MAX_PAYLOAD. See comments in debug_app_def on how to set these.
 *
 * log_debug_data_examples contain examples on how to use debug data app.
 * debug_example (#define debug_example) shows how to write data on scratchpad.
 * read_log_data_by_pmbus (#define read_log_data_by_pmbus) is an example of pmbus handler that is used to read log data via pmbus
 * State_machine_debug (#define State_machine_debug) is can be used to log state machine transitions.
 *
 *
 *
 */

#ifndef COMMON_MODULES_USER_APP_USER_APP_H_
#define COMMON_MODULES_USER_APP_USER_APP_H_
#include "pmbus_api.h"

void USER_Log_Error(uint32_t program_counter, PMBUS_CML_STATUS_e cml_status);
void user_drv_init(void);



#endif /* COMMON_MODULES_USER_APP_USER_APP_H_ */
