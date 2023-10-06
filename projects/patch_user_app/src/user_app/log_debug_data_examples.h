/*
 * log_debug_data.h
 *
 *  Created on: Jul 12, 2019
 *      Author: ashendar
 */

/** Example code : Log Debug Data
 * Log debug data provides an example code for how to use debug app to log data on scratchpad.
 * This data can be read through the GUI or debugger.
 *
 */

#include "pmbus_autogen.h"

void log_debug_data_init(void);

//#define debug_example //define debug_app in debug_app_def.h
//#define log_telemetry_data //define debug_app in debug_app_def.h
//#define read_log_data_by_pmbus //define debug_app in debug_app_def.h
//#define State_machine_debug //define debug_app in debug_app_def.h

#ifdef read_log_data_by_pmbus
#include "pmbus_mfr_autogen.h"
#endif

#ifdef debug_example
void log_debug_data(void);
#endif

#ifdef log_telemetry_data
void telemetry_data_sample(PMBUS_PAGE_t loop, uint16_t count);
#endif

#ifdef read_log_data_by_pmbus
void PMBUS_HANDLE_MFR_RAMP_DATA(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

#ifdef State_machine_debug
void State_machine_debug_init (void);

void patch_Regulation_Get_All_Q_Messages_By_Priority (void);
#endif
