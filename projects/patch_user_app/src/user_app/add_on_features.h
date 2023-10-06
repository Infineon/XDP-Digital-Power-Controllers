
#ifndef ADD_ON_FEATURES_H_
#define ADD_ON_FEATURES_H_

#include "regulation_drv.h"
#include "uart_drv.h"
//ADD-ON Features:

/**
 * Enable or disable various add on features here.  Implementation in add_on_features.c
 */
//#define retry_if_removed_bug_fix
#define uart_debug
#define configurator_crc_check
#define JIRA_752_Disable_I2C_EN
#define JIRA_743_OFFSET_UNDERFLOW
#define JIRA_754_SECURITY_DISABLED_ON_OTP_RESTORE
#define JIRA_769_SETUP_PASSWORD
#define JIRA_771_PMBUS_DATA_COHERENCY
#define JIRA_739_VCONTROL_DELTA_SET
#define JIRA_972_I2C_FIFO
#define JIRA_774_FREQUENCY_SWITCH_ZERO
#define JIRA_745_PID_VERR_ENTRY_THRS
#define JIRA_785_REMAP_CLEAR_NEEDED_ON_RESET
#define JIRA_889_WRITE_CLEAR_FAULT
#define JIRA_783_PATCH_IN_SAME_PARTITION_OF_INVALIDATED_PATCH
#define PMBUS_MEMORY_ERROR_CML
#define en_ishare
#define DEBOUNCE_GLITCH
#define VIN_ON_OFF_ISSUE

#ifdef uart_debug
#define N_ROWS   (128)
//Test variable
uint8_t  src_buf[N_ROWS];
uint8_t  dest_buf[N_ROWS];
ARM_DRIVER_USART * UARTdrv;
void cb_test (uint32_t event);
#endif

void add_on_features_init (void);
void USER_Log_Error(uint32_t program_counter, PMBUS_CML_STATUS_e cml_status);

#ifdef PMBUS_MEMORY_ERROR_CML
void patch_PMBUS_Copy_From_Memory(PMBUS_PAGE_t loop, const uint8_t *ram_location);
#endif

#ifdef en_ishare
void added_droop_disable(PMBUS_PAGE_t loop);
void added_droop_enable(PMBUS_PAGE_t loop);
void remove_added_droop_irq_callback(PMBUS_PAGE_t loop);

void enable_ishare(PMBUS_PAGE_t loop);
void disable_ishare(PMBUS_PAGE_t loop);
//void mfr_remove_added_droop(PMBUS_PAGE_t loop);
void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
#endif

#ifdef JIRA_774_FREQUENCY_SWITCH_ZERO
void patch_Regulation_Switching_Frequency_Set(PMBUS_PAGE_t loop, uint16_t freq_khz);
#endif

#ifdef JIRA_745_PID_VERR_ENTRY_THRS
void patch_Regulation_disable_ata(PMBUS_PAGE_t loop);
#endif

#ifdef JIRA_771_PMBUS_DATA_COHERENCY
void patch_Telemetry_Sample(void);
#endif

#ifdef JIRA_769_SETUP_PASSWORD
void patch_PMBUS_HANDLE_MFR_SETUP_PASSWORD(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

#ifdef JIRA_743_OFFSET_UNDERFLOW
int32_t patch_Telemetry_get(PMBUS_PAGE_t loop, TELEMETRY_TYPE_t telem_type);
#endif

#ifdef JIRA_783_PATCH_IN_SAME_PARTITION_OF_INVALIDATED_PATCH
void patch_patch_otp_cmd_patch_version_process(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint32_t otp_end);
#endif

#ifdef JIRA_785_REMAP_CLEAR_NEEDED_ON_RESET
/**
 * This is an example of how to add new firmware commands to MFR_FIRMWARE_COMMAND PMBus command
 * @param page Selects the PMBus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

void pmbus_driver_init(void);
#endif
