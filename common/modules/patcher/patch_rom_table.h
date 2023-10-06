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
 * @file    patch_rom_table.h
 * @brief   This file describes the format of the OTP patch function
 *          table, which  is located in ROM.  This table needs to be
 *          initialized during system startup by the
 *          'patch_otp_process()' function.
 */
#ifndef PATCH_ROM_TABLE_H
#define PATCH_ROM_TABLE_H

#include "stdint.h"
#include "fw_bool.h"

/*python append header list here*/
#include "uart_drv.h"
#include "fw_params.h"
#include "configurator.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "gpio_drv.h"
#include "otp_drv.h"
#include "otp_fs.h"
#include "patch_app.h"
#include "pmbus_api.h"
#include "pmbus_cmd_handlers.h"
#include "pmbus_drv.h"
#include "pmbus_gpio_control.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "telemetry_api.h"
#include "telemetry_drv.h"
#include "telemetry_irq_handlers.h"
/*python end of header list*/

/** OTP Patch Table definition Function Pointer. */
typedef struct
{
    /** this entry is a dummy entry which patches the function 'dummy_entry'.
        This dummy entry contains three 'int' parameter
        Add other patch function need to added to this structure in the same
        way, where the patch variable is generated out of the original
        function name enhanced by the "patch_..." prefix. */
    int (*patch_dummy_entry)(int , int , int );

    /*python append function list here */
     uint32_t (*patch_PMBUS_Calculate_Size_Check)(void);
     uint32_t (*patch_PMBUS_Copy_Command_To_RAM)(PMBUS_PAGE_t loop, uint8_t *ram_location, uint8_t command);
     void (*patch_PMBUS_Restore_Code)(PMBUS_PAGE_t page, uint8_t command);
     int32_t (*patch_PMBUS_Initialize)(void);
     void (*patch_PMBUS_drv_init)(void);
     void (*patch_PMBUS_GPIO_CB)(gpio_idx_t Bank, uint32_t Status);
     void (*patch_REGULATION_INTER_THREAD_SET)(PMBUS_PAGE_t loop, REGULATION_CONTROLS control, uint16_t control_data);
     void (*patch_Regulation_Compute_Feed_Forward)(PMBUS_PAGE_t loop);
     void (*patch_Regulation_Drv_Init)(void);
     void (*patch_Regulation_Initialize)(void);
     void (*patch_Regulation_OSP_Fault_Detection_sequence)(PMBUS_PAGE_t loop);
     void (*patch_Regulation_disable_ata)(PMBUS_PAGE_t loop);
     void (*patch_Regulation_smooth_transition_feedforward)(boolean enable_disable);
     void (*patch_iout_temperature_compensation)(PMBUS_PAGE_t loop);
     int32_t (*patch_Uart_Control)(uint32_t control, uint32_t arg);
     int32_t (*patch_Uart_Initialize)(ARM_USART_SignalEvent_t cb_event);
     int32_t (*patch_Uart_PowerControl)(POWER_STATE state);
     int8_t (*patch_Uart_ProcessRx)(void);
     int8_t (*patch_Uart_ProcessTx)(void);
     int32_t (*patch_Uart_Receive)(void *data, uint32_t num);
     int8_t (*patch_Uart_RxSetup)(uint8_t *data, uint32_t num);
     int32_t (*patch_Uart_Send)(const void *data, uint32_t num);
     void (*patch_Uart_SetBaudRate)(uint32_t baudRate, uint32_t uartCLKkHz);
     int32_t (*patch_Uart_SetModemControl)(ARM_USART_MODEM_CONTROL control);
     int32_t (*patch_Uart_Transfer)(const void *data_out, void *data_in, uint32_t num);
     int8_t (*patch_Uart_TxSetup)(uint8_t *data, uint32_t num);
     int32_t (*patch_Uart_Uninitialize)(void);
     void (*patch_sys_modules_init)(void);
     void (*patch_configurator_boot_default)(void);
     void (*patch_configurator_boot_xvalent)(uint8_t xv);
     void (*patch_configurator_conf_registers_load)(void);
     void (*patch_configurator_conf_registers_store)(uint8_t xv);
     void (*patch_configurator_error_clear)(void);
     CONFIGURATOR_ERROR_t (*patch_configurator_error_get)(void);
     uint8_t* (*patch_configurator_pmbus_default_ptr_get)(uint8_t loop);
     void (*patch_configurator_pmbus_default_set)(uint8_t loop);
     void (*patch_configurator_pmbus_default_store)(uint8_t loop, const uint8_t* ptr, uint16_t size);
     void (*patch_configurator_pmbus_set)(uint8_t lp, uint8_t conf_type);
     void (*patch_configurator_pmbus_partial_set)(void);
     void (*patch_configurator_pmbus_store)(uint8_t lp, uint8_t conf_type, uint8_t xv_type, const uint8_t* ptr, uint16_t size);
     void (*patch_configurator_pmbus_partial_store) (uint8_t lp, uint8_t cmd,  uint8_t xv_type, const uint8_t * ptr, uint16_t size);
     void (*patch_configurator_conf_partial_store) (uint8_t xv_type, const uint8_t * ptr, uint16_t size);
     void (*patch_configurator_conf_partial_load) (void);
     void (*patch_fw_params_trim_load_cb) (void *ptr);
     void (*patch_fw_params_config_load_cb) (void *ptr);
     uint8_t* (*patch_configurator_pmbus_user_ptr_get)(uint8_t loop);
     void (*patch_configurator_pmbus_user_set)(uint8_t loop);
     void (*patch_configurator_pmbus_user_store)(uint8_t loop, uint8_t xv, const uint8_t* ptr, uint16_t size);
     void (*patch_configurator_registers_load)(Otp_File_System_Cmd_Types_t type);
     void (*patch_configurator_registers_store)(Otp_File_System_Cmd_Types_t type, uint8_t xv);
     uint32_t (*patch_configurator_remaining_otp_size_get)(Otp_Partition_Selector_t partition);
     void (*patch_configurator_trim_registers_load)(void);
     void (*patch_configurator_trim_registers_store)(void);
     void (*patch_Clear_Firmware_Fault)(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type);
     FAULT_TYPE_t (*patch_FAULTS_Resolve_Shutdown_Priority)(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type_current, FAULT_TYPE_t fault_type_previous);
     uint32_t (*patch_FAULTS_Get_PMBUS_Configuration)(PMBUS_PAGE_t loop, FAULTS_CONFIG config);
     void (*patch_FAULTS_SETUP_FAULT)(
             PMBUS_PAGE_t loop,
             FAULT_TYPE_t fault_type,
             uint8_t fault_response_cmd_byte, //Response[7:6], Retry_Setting{5:3] and Delay_Time[2:0]
             FAULTS_CONFIG Config_Delay_Unit,
             RESPONSE_CONFIG_t response_config,
             uint8_t pmbus_status_command,
             uint8_t pmbus_status_bit_position,
             FAULT_TYPE_t hysteresis);
     void (*patch_Faults_Clear_Faults)(PMBUS_PAGE_t page);
     void (*patch_Set_Firmware_Fault)(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type);
     void (*patch_COMMON_FAULT_HANDLE)(FAULT_TYPE_t fault_code);
     int32_t (*patch_FAULTS_Initialize)(void);
     int32_t (*patch_FAULTS_PowerControl)(POWER_STATE state);
     void (*patch_Fault_Threshold_Set)(PMBUS_PAGE_t loop, FAULT_TYPE_t fault_type, int32_t mantissa, int32_t exponent);
     void (*patch_Faults_IRQHandler)(void);
     void (*patch_Faults_Mask_Fault)(PMBUS_PAGE_t loop, boolean hw_fault, uint32_t set_mask, uint32_t clear_mask);
     void (*patch_Faults_Set_Shutdown_Mask_Hardware_Faults)(PMBUS_PAGE_t loop, uint32_t shutdown_mask);
     void (*patch_LOOP_FAULT_HANDLE)(PMBUS_PAGE_t loop, FAULT_TYPE_t  fault_type);
     void (*patch_faults_drv_init)(void);
     void (*patch_GPIO_Drv_init)(gpio_idx_t Bank);
     void (*patch_OTP_Initialize)(void);
     void (*patch_OTP_Read)(Otp_Partition_Selector_t partition, uint32_t offset, uint8_t** ptr, uint32_t num);
     void (*patch_OTP_Write)(Otp_Partition_Selector_t partition, uint32_t offset, const uint8_t* ptr, uint32_t num);
     void (*patch_otp_fs_command_header_update_program)(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Hdr_t* p_cmd_hdr);
     void (*patch_otp_fs_command_invalidate_handler)(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint32_t otp_end);
     uint8_t (*patch_otp_fs_max_xvalent)(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Types_t cmd);
     void (*patch_otp_fs_scan)(Otp_Partition_Selector_t partition, const Otp_File_System_Cmd_Handler_t* handler, Otp_File_System_Cmd_Hdr_t** cmd_hdr);
     void (*patch_patch_init)(void);
     void (*patch_patch_otp_cmd_patch_version_process)(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint32_t otp_end);
     void (*patch_PMBUS_Copy_From_Memory)(PMBUS_PAGE_t loop, const uint8_t *ram_location);
     uint32_t (*patch_PMBUS_Copy_To_RAM)(PMBUS_PAGE_t loop, uint8_t *ram_location);
     uint8_t (*patch_PMBUS_Get_On_Off_State)(PMBUS_PAGE_t loop);
     void (*patch_PMBUS_Inter_Thread_Set_Command)(PMBUS_PAGE_W_BROADCAST_t loop, uint8_t cmd, PMBUS_DIRECTION_e direction );
     void (*patch_PMBUS_Log_Error)(Log_Source_t source, uint32_t program_counter, PMBUS_CML_STATUS_e cml_status);
     void (*patch_PMBUS_Restore_All)(PMBUS_PAGE_t page, uint8_t * p);
     void (*patch_PMBUS_Status_Cmd_Handle)(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction, uint8_t cmd);
     void (*patch_pmbus_clear_protection)(PMBUS_PAGE_t page);
     void (*patch_protect_pmbus)(PMBUS_PAGE_t page, uint8_t cmd);
     int32_t (*patch_PMBUS_PowerControl)(POWER_STATE state);
     void (*patch_PMBUS_RANGE_CALCULATE)(uint_fast8_t byte_num);
     int32_t (*patch_PMBUS_Uninitialize)(void);
     void (*patch_PMBUS_Gpio_Configure)(void);
     boolean (*patch_PMBUS_Gpio_Get_Feature)(PMBUS_PAGE_t loop, PMBUS_GPIO_CONFIG config);
     uint8_t (*patch_PMBUS_Gpio_Get_PMBUS_Configuration)(PMBUS_PAGE_t loop, PMBUS_GPIO_CONFIG config);
     void (*patch_PMBUS_Gpio_Set_Feature)(PMBUS_PAGE_t loop, PMBUS_GPIO_CONFIG config, boolean value);
     void (*patch_Regulation_PWM_Deadtime_Set)(uint8_t phase, uint8_t rising_edge_delay, uint8_t falling_edge_delay);
     void (*patch_Regulation_Set_VID)(PMBUS_PAGE_t loop, uint32_t voltage_mantissa, uint32_t vout_scale, int32_t vout_exp, uint32_t window, boolean immediate);
     void (*patch_Regulation_Switching_Frequency_Set)(PMBUS_PAGE_t loop, uint16_t freq_khz);
     void (*patch_Regulation_Voltage_Ramp_Rate_Set)(PMBUS_PAGE_t loop, uint32_t mv_per_ms);
     void (*patch_Regulation_FAULT_TON_TOFF_MAX_Timer_Activation)(PMBUS_PAGE_t loop, uint32_t load);
     void (*patch_Regulation_FAULT_Timer_Activation)(PMBUS_PAGE_t loop, uint32_t load);
     void (*patch_Regulation_Fault_Mask)(PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
     uint16_t (*patch_Regulation_Get_PMBUS_Configuration)(PMBUS_PAGE_t loop, REGULATION_CONFIG config);
     uint32_t (*patch_Regulation_Get_Ramp_Rate)(PMBUS_PAGE_t loop, uint32_t vout_target, uint32_t prebias_voltage, uint8_t cmd);
     uint16_t (*patch_Regulation_Get_Vid)(PMBUS_PAGE_t loop);
     uint32_t (*patch_Regulation_Get_Vout_Transition_Rate)(PMBUS_PAGE_t loop);
     void (*patch_Regulation_Power_Mode)(PMBUS_PAGE_t loop, REGULATION_STATE state);
     void (*patch_Regulation_Set_Vout)(PMBUS_PAGE_t loop, uint32_t vout, uint32_t ramp_rate_mv_per_ms, boolean immediate, boolean trim);
     void (*patch_Regulation_Shutdown_Sequence)(PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
     void (*patch_Regulation_TON_TOFF_DELAY_Timer_Activation)(PMBUS_PAGE_t loop, uint32_t load);
     void (*patch_Regulation_Update_Vid)(PMBUS_PAGE_t loop, REGULATION_STATE state);
     void (*patch_Telemetry_Enable_IRQ_Type)(Telemetry_IRQ_TYPE_t Telemetry_type);
     void (*patch_Telemetry_Generic_Irq_Threshold_Set)(Telemetry_IRQ_TYPE_t index, Telemetry_irq_src_t source, int32_t mantissa, int32_t exponent);
     uint8_t (*patch_Telemetry_Get_PMBUS_Configuration)(PMBUS_PAGE_t loop, TELEMETRY_CONFIG config);
     uint8_t (*patch_Telemetry_Get_Xvalent_Pinset)(boolean xaddr);
     void (*patch_Telemetry_Set_Irq_Hysteresis_Threshold)(Telemetry_IRQ_TYPE_t Telemetry_type, int32_t mantissa, int8_t exponent);
     void (*patch_Telemetry_Set_Irq_Polarity)(Telemetry_IRQ_TYPE_t Telemetry_type, uint8_t polarity);
     void (*patch_Telemetry_Set_Irq_Threshold)(Telemetry_IRQ_TYPE_t Telemetry_type, int32_t mantissa, int8_t exponent);
     void (*patch_Telemetry_Setup_Irq)(PMBUS_PAGE_t page, Telemetry_irq_src_t source, Telemetry_IRQ_TYPE_t Telemetry_type, Telemetry_handle handle);
     void (*patch_Telemetry_change_scales)(PMBUS_PAGE_t loop);
     int32_t (*patch_Telemetry_get)(PMBUS_PAGE_t loop, TELEMETRY_TYPE_t telem_type);
     uint16_t (*patch_Telemetry_get_adc_code)(int16_t temp);
     uint16_t (*patch_Telemetry_get_fan_speed)(fan_idx_t fan);
     uint32_t (*patch_Telemetry_get_power)(uint32_t vout, int8_t vout_exp, int32_t iout, int8_t iout_exp, int8_t pout_exp);
     int16_t (*patch_Telemetry_get_tempi)(void);
     TELEMETRY_VIN_SOURCE (*patch_Telemetry_get_vin_source)(PMBUS_PAGE_t loop);
     void (*patch_Telemetry_IRQHandler)(void);
     int32_t (*patch_Telemetry_Initialize)(void);
     int32_t (*patch_Telemetry_PowerControl)(POWER_STATE state);
     void (*patch_Telemetry_Sample)(void);
     int32_t (*patch_Telemetry_Uninitialize)(void);
     void (*patch_Telemetry_drv_init)(void);
     void (*patch_Telemetry_VOUT_UV_TON_MAX_DISABLE_Irq_Threshold_Set)(PMBUS_PAGE_t loop, uint32_t mantissa, int8_t exponent);
     void (*patch_Regulation_Wait_For_SM_Message) (void);
     void (*patch_Regulation_Get_All_Q_Messages_By_Priority) (void);

    /*python end of function list */

} Patch_ROM_Table_t;
/*lint -restore */
/** OTP Patch Table definition */
extern Patch_ROM_Table_t patch_rom_table;

#endif /* PATCH_ROM_TABLE_H */
