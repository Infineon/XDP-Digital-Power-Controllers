
#ifndef ADD_ON_FEATURES_H_
#define ADD_ON_FEATURES_H_

#include "pmbus_api.h"
#include "regulation_drv.h"
#include "telemetry_api.h"
#include "fw_params.h"
#include "patch_pmbus_fifo.h"
//ADD-ON Features:

//#define State_machine_debug
//#define en_ishare
#define configurator_crc_check
#define JIRA_774_FREQUENCY_SWITCH_ZERO
#define JIRA_745_PID_VERR_ENTRY_THRS
//#define JIRA_771_PMBUS_DATA_COHERENCY
#define JIRA_769_SETUP_PASSWORD
//#define JIRA_743_OFFSET_UNDERFLOW
#define JIRA_783_PATCH_IN_SAME_PARTITION_OF_INVALIDATED_PATCH
#define JIRA_785_REMAP_CLEAR_NEEDED_ON_RESET
#define JIRA_916_STORE_PARTIAL_CONFIG_MFR_IOUT_OC_FAST_FAULT_LIMIT
#define JIRA_974_RESTORE_USER_CODE

void add_on_features_init (void);

#ifdef State_machine_debug
void patch_Regulation_Get_All_Q_Messages_By_Priority (void);
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

#ifdef JIRA_916_STORE_PARTIAL_CONFIG_MFR_IOUT_OC_FAST_FAULT_LIMIT
void patch_PMBUS_HANDLE_MFR_IOUT_OC_FAST_FAULT_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

#ifdef configurator_crc_check
void patch_configurator_pmbus_set(uint8_t lp, uint8_t conf_type);
void patch_configurator_pmbus_partial_set(void);
void patch_configurator_conf_partial_load(void);

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


#ifdef JIRA_973_I2C_FIFO
void pmbus_driver_init();
#endif

#ifdef JIRA_974_RESTORE_USER_CODE
void patch_PMBUS_HANDLE_RESTORE_USER_CODE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

__DRIVER_COMMON_INLINE uint32_t PMBUS_Set_Command_READ_PROTECTION(uint8_t cmd)
{
	uint32_t cmd_config_read_protocol = (PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & RD_PROTO_MASK) >> RD_PROTO_POSITION;
	// zeros the command config read protcol position makes the command nack on reads
	PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG = (PMBUS_CMD_ARRAY_LOOP[cmd]->CMD_CONFIG & ~RD_PROTO_MASK);
	return cmd_config_read_protocol;  // note this needs to be restored at some point so returned here to be kept someplace else
}

#endif
