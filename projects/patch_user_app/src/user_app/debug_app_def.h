#ifndef DEBUG_APP_DEF_H_
#define DEBUG_APP_DEF_H_
/*
 * These parameters need to be defined by the user to configure debug app:
 *
 * DEBUG_DATA_BUFFER_BASE is where debug_app can start storing the data. Typically should be pointing to scratchpad.
 * Note that the first 24 bytes will be reserved by the log app itself - the size of cicular buffer structure [typedef ramp_data_App_s] ramp_data_App.
 */
#define DEBUG_DATA_BUFFER_BASE (DRIVER_COMMON_STRUCT.scratchpad) //scratchpad
 /*
 * DEBUG_DATA_SIZE_IN_BYTES is the maximum size of all logs in bytes. It should include 24 bytes required by the ramp data app and maximum
 * shouldn't exceed SCRATCHPAD_SIZE_IN_BYTES.
 * Also each log has a 4 byte header so , for ex., if the user is trying to log 4x [int32_t] values 10 times the size should be more than:
 * size = 24[log app struct] + (4[size of log]*4bytes[int32_t] + 4byte[header]) * 10 = 224 bytes
 */
#define DEBUG_DATA_SIZE_IN_BYTES (24 + 1024)  //24bytes required for debug app structure
/*
 *  CAPTURE_LOG_MAX_PAYLOAD is the maximum payload of a log in number of 32-bit words
 */
#define CAPTURE_LOG_MAX_PAYLOAD   (uint8_t)((uint8_t)2 << (5-1)) // The maximum payload of a log in number of 32-bit words


//#define debug_app



#endif /* DEBUG_APP_DEF_H_ */
