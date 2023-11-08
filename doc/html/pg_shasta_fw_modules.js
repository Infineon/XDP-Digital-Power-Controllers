var pg_shasta_fw_modules =
[
    [ "System FW Module", "pg_shasta__sys__mod.html", "pg_shasta__sys__mod" ],
    [ "Patch FW Module", "pg_shasta__patch.html", [
      [ "Introduction", "pg_shasta__patch.html#sctShasta_Patch_Introduction", null ],
      [ "OTP Patch Processing", "pg_shasta__patch.html#pgShasta_Patch_Processing", null ],
      [ "RAM Patch Table", "pg_shasta__patch.html#pgShasta_Patch_RAM_Patch_Table", [
        [ "Using A ROM Code Patch", "pg_shasta__patch.html#pgShasta_Patch_Usage", null ],
        [ "Patched ROM Code Example", "pg_shasta__patch.html#pgShasta_Patch_RAM_Patch_Table_Example", null ],
        [ "Patch Table Example", "pg_shasta__patch.html#pgShasta_Patch_Table_Example", null ],
        [ "Filling the Patch Table", "pg_shasta__patch.html#pgShasta_Patch_Table_Filling", null ]
      ] ],
      [ "Interrupt and Exception Patching", "pg_shasta__patch.html#pgShasta_Patch_Interrupt", null ],
      [ "System Startup Precondition", "pg_shasta__patch.html#pgShasta_Patch_System_Startup_Precondition", null ],
      [ "Patch Example", "pg_shasta__patch.html#pgShasta_Patch_Example", null ]
    ] ],
    [ "RTX RTOS FW Module", "pg_shasta__r_t_x.html", [
      [ "Introduction", "pg_shasta__r_t_x.html#sctShasta_RTX_Intro", null ],
      [ "RTX usage and documentation", "pg_shasta__r_t_x.html#sctShasta_RTOS_usage", null ],
      [ "ARM M0 real-time behaviour", "pg_shasta__r_t_x.html#sctShasta_RTOS_MO", null ],
      [ "RTX real-time behaviour", "pg_shasta__r_t_x.html#sctShasta_RTOS_Eval", null ],
      [ "ARM M0 Supervisor Calls", "pg_shasta__r_t_x.html#sctShasta_RTOS_2", null ],
      [ "Configuration", "pg_shasta__r_t_x.html#sctShasta_RTOS_Config", null ],
      [ "Implementation Notes", "pg_shasta__r_t_x.html#sctShasta_RTOS_Notes", null ],
      [ "Custom RTOS extensions", "pg_shasta__r_t_x.html#sctShasta_RTOS_Extensions", null ],
      [ "Custom RTOS patches", "pg_shasta__r_t_x.html#sctShasta_RTOS_patche", null ]
    ] ],
    [ "PMBUS FW Module", "pg_shasta__p_m_b_u_s.html", [
      [ "Introduction", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_Intro", null ],
      [ "PMBUS HW/FW interface", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_HWFW", null ],
      [ "PMBUS HW/FW interface", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_HWFW1", null ],
      [ "PMBUS GPIO Controls", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_GPIO", null ],
      [ "Using XADDR Pins to Offset PMBUS Address", "pg_shasta__p_m_b_u_s.html#sctXaddrOffsets", null ],
      [ "PMBUS Packet Types", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_Packets", [
        [ "Write Word", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_Packets_Write_Word", null ],
        [ "Read Word", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_Packets_Read_Word", null ],
        [ "Block Write", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_Packets_Block_Write", null ],
        [ "Block Read", "pg_shasta__p_m_b_u_s.html#sctShasta_PMBUS_Packets_Block_Read", null ]
      ] ],
      [ "PMBUS I2C AHB Bridge Interface", "pg_shasta__p_m_b_u_s.html#sctShasta_I2C", [
        [ "Read32", "pg_shasta__p_m_b_u_s.html#sctShasta_I2C_Read32", null ],
        [ "Write32", "pg_shasta__p_m_b_u_s.html#sctShasta_I2C_Write32", null ]
      ] ]
    ] ],
    [ "Regulation FW Module", "pg_shasta__regulation.html", [
      [ "Introduction", "pg_shasta__regulation.html#sctShasta_REGULATION_Intro", null ],
      [ "Startup Behavior", "pg_shasta__regulation.html#sctStartup", null ],
      [ "Fault recovery", "pg_shasta__regulation.html#sctFAULT", null ],
      [ "Prebias Response", "pg_shasta__regulation.html#sctPrebias", null ],
      [ "Current Limited startup", "pg_shasta__regulation.html#sctCurrent_limited_startup", null ],
      [ "Diode emulation on startup", "pg_shasta__regulation.html#sctde_mode", null ]
    ] ],
    [ "Telemetry FW Module", "pg_shasta__t_e_l_e_m_e_t_r_y.html", [
      [ "Introduction", "pg_shasta__t_e_l_e_m_e_t_r_y.html#sctShasta_TELEMETRY_Intro", null ],
      [ "Telemetry Generic IRQ module", "pg_shasta__t_e_l_e_m_e_t_r_y.html#sctShasta_Telemetry_IRQ", null ],
      [ "Telemetry Thread", "pg_shasta__t_e_l_e_m_e_t_r_y.html#sctShasta_Telemetry_THREAD", null ]
    ] ],
    [ "Faults FW Module", "pg_shasta__f_a_u_l_t_s.html", [
      [ "Introduction", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_Intro", null ],
      [ "Fault detection and hysteresis", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_HYSTERESIS", null ],
      [ "Fault Response", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_RESPONSE", [
        [ "Fault Response Type", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_RESPONSE_TYPE", null ],
        [ "Fault Response Retry Count", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_RESPONSE_RETRY_COUNT", null ],
        [ "Fault Response Retry Delay", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_RESPONSE_RETRY_DELAY", null ],
        [ "Fault Response Setup", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_RESPONSE_SETUP", null ],
        [ "Fault Response IOUT_UC_FAULT", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_RESPONSE_IOUT_UC_FAULT", null ]
      ] ],
      [ "Fault Reporting and Clearing on PMBUS", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_REPORTING_PMBUS", null ],
      [ "Fault masking", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_MASKING", null ],
      [ "Fault pin masking", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_PIN_MASKING", null ],
      [ "Faults HW Module and Driver Operation", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_OPERATION", [
        [ "Fault HW Module Operation", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_HW", null ],
        [ "Fault Driver Operation Hardware faults", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_Driver_HW", null ],
        [ "Fault Driver Operation Firmware faults", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_DRIVER_FW", null ]
      ] ],
      [ "Supported Fault Types", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_TYPES", [
        [ "Fault Type Enum Table", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_TYPES_ENUM", null ],
        [ "Fault Type Vout OV Fault Example", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_TYPES_VOUT_OV_FAULT", null ],
        [ "Fault Type Vout UV Fault Example", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_TYPES_VOUT_UV_FAULT", null ],
        [ "Fault Type Vin UV Fault Example", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_TYPES_VIN_UV_FAULT", null ],
        [ "Fault Type IOUT OC Fault Example", "pg_shasta__f_a_u_l_t_s.html#sctShasta_FAULTS_FAULT_TYPES_IOUT_OC_FAULT", null ]
      ] ]
    ] ],
    [ "Logger FW Module", "pg_shasta__logger.html", [
      [ "Introduction", "pg_shasta__logger.html#sctShasta_Logger_Intro", null ],
      [ "Log file", "pg_shasta__logger.html#sctShasta_Log_File", null ],
      [ "Module Interface", "pg_shasta__logger.html#sctShasta_Logger_Interface", null ]
    ] ],
    [ "Configurator FW Module", "pg_shasta__configurator.html", [
      [ "Introduction", "pg_shasta__configurator.html#sctShasta_Configurator_Intro", null ],
      [ "Xvalent File Selector", "pg_shasta__configurator.html#sctShasta_Configurator_xvalent", null ],
      [ "Register Structure File", "pg_shasta__configurator.html#sctShasta_Configurator_Register_Structure", null ],
      [ "FW Configuration", "pg_shasta__configurator.html#sctShasta_Configurator_Register_FW_Config", [
        [ "FW Trim Data", "pg_shasta__configurator.html#sctShasta_Configurator_Register_FW_Config_Trim", null ],
        [ "FW Config Data", "pg_shasta__configurator.html#sctShasta_Configurator_Register_FW_Config_Config", null ]
      ] ],
      [ "PMBUS Structure file", "pg_shasta__configurator.html#sctShasta_Configurator_PMBUS_Structure", null ],
      [ "Partial Register Configurations", "pg_shasta__configurator.html#sctShasta_Configurator_partial_register_configurations", null ],
      [ "Partial PMBus Configurations", "pg_shasta__configurator.html#sctShasta_Configurator_partial_pmbus_configurations", null ],
      [ "OTP File Generation", "pg_shasta__configurator.html#sctShasta_Configurator_otp_file_generation", null ],
      [ "Error Handling", "pg_shasta__configurator.html#sctShasta_Configurator_Error_Handling", null ]
    ] ],
    [ "Operation, Administration & Maintenance Module", "pg_shasta__oam.html", [
      [ "Operation, Administration & Maintenance", "pg_shasta__oam.html#sctShasta_Oam_Intro", null ],
      [ "Handling of Hard Faults", "pg_shasta__oam.html#sctShasta_Oam_Hard_Fault", null ]
    ] ],
    [ "DMA Driver module", "pg_shasta_dma_drv.html", [
      [ "DMA Driver", "pg_shasta_dma_drv.html#sctShasta_DMA_drv_Intro", null ],
      [ "Example application", "pg_shasta_dma_drv.html#sctShasta_dma_drv_example", null ]
    ] ],
    [ "DTIMER Driver module", "pg_shasta_dtimer_drv.html", [
      [ "DTIMER Driver", "pg_shasta_dtimer_drv.html#sctShasta_dtimer_drv_Intro", null ],
      [ "Single shot mode", "pg_shasta_dtimer_drv.html#sctShasta_dtimer_drv_single", null ],
      [ "Periodic mode", "pg_shasta_dtimer_drv.html#sctShasta_dtimer_drv_periodic", null ],
      [ "Example application", "pg_shasta_dtimer_drv.html#sctShasta_dtimer_drv_example", null ]
    ] ],
    [ "MMU Driver module", "pg_shasta_mmu_drv.html", [
      [ "MMU Driver", "pg_shasta_mmu_drv.html#sctShasta_mmu_drv_Intro", null ],
      [ "MMU Initialization", "pg_shasta_mmu_drv.html#sctShasta_mmu_drv_init", null ],
      [ "Example application", "pg_shasta_mmu_drv.html#sctShasta_mmu_drv_example", null ]
    ] ],
    [ "OTP Driver Module", "pg_shasta__otp.html", [
      [ "Introduction", "pg_shasta__otp.html#sctShasta_Otp_Intro", null ],
      [ "OTP Driver Initialization", "pg_shasta__otp.html#sctShasta_otp_driver_init", null ],
      [ "OTP Data Write", "pg_shasta__otp.html#sctShasta_Otp_Write", null ],
      [ "OTP Data Read", "pg_shasta__otp.html#sctShasta_Otp_read", null ],
      [ "OTP File Structure", "pg_shasta__otp.html#sctShasta_Otp_file_structure", null ]
    ] ],
    [ "UART FW Module", "pg_shasta__u_a_r_t.html", [
      [ "Introduction", "pg_shasta__u_a_r_t.html#sctShasta_UART_Intro", null ]
    ] ]
];