var NAVTREE =
[
  [ "XDPP1100 Firmware", "index.html", [
    [ "XDPP1100 System Overview", "index.html", [
      [ "System Top Level FW/HW View", "index.html#sctMainpage_System_Toplevel_FWHW", null ],
      [ "System Top Level View", "index.html#sctMainpage_System_Toplevel", null ],
      [ "Feature Set", "index.html#sctMainpage_Feature_Set", null ],
      [ "HW Functional Block Diagram", "index.html#sctMainpage_HW_Functional_Block_Diagram", null ],
      [ "HW Architecture", "index.html#sctMainpage_HW_Architecture", null ],
      [ "CPUS Architecture", "index.html#sctMainpage_CPUS_Architecture", null ],
      [ "FW Feature Set", "index.html#sctMainpage_FW_Feature_set", null ],
      [ "FW Top Level View", "index.html#sctMainpage_FW_Toplevel", null ],
      [ "Real-Time Requirements", "index.html#sctMainpage_Real_Time", [
        [ "Interrupt Handling", "index.html#sctMainpage_Int", null ],
        [ "Thread Handling", "index.html#sctMainpage_Thread", null ]
      ] ]
    ] ],
    [ "PMBUS User Guide", "pg_p_m_b_u_s__guide.html", [
      [ "Some Key Features Of PMBus", "pg_p_m_b_u_s__guide.html#sctIntro", null ],
      [ "PMBus Characteristics", "pg_p_m_b_u_s__guide.html#sctIntro1", null ],
      [ "SMBALERT#", "pg_p_m_b_u_s__guide.html#sctAlert", null ],
      [ "How to Decode Waveforms", "pg_p_m_b_u_s__guide.html#sctDecode", null ],
      [ "PMBus Command Set Descriptions", "pg_p_m_b_u_s__guide.html#sctPMBUS_CMDS", null ],
      [ "Adding PMBus MFR Commands To Example Project", "pg_p_m_b_u_s__guide.html#sctAddcmds", null ],
      [ "XADDR Pin Offsets", "pg_p_m_b_u_s__guide.html#sctXADDR", null ]
    ] ],
    [ "Patch Users Guide", "pg_shasta_patch_user_guide.html", [
      [ "Introduction", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_Introduction", [
        [ "Partial Patch Project", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_introduction_partial_patch_Project", null ],
        [ "Full Patch Project", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_introduction_full_patch_project", null ],
        [ "Supported Tool-Chains", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_introduction_tool_chain", null ],
        [ "Patch Make Build Target Overview", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_introduction_build_targets", null ]
      ] ],
      [ "Full User Entry Points", "pg_shasta_patch_user_guide.html#pgUser_Entry", null ],
      [ "Am I creating data configuration or firmware patches?", "pg_shasta_patch_user_guide.html#pgData_or_FW", null ],
      [ "Setting Up A New Patch Project", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_Setup_a_new_Project", [
        [ "Partial Patch example project", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_Partial_Patch_Example_Project", null ],
        [ "Full patch example project", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_Full_Patch_Example_Project", null ]
      ] ],
      [ "Loading a Patch to RAM", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_loading_a_patch_to_RAM", null ],
      [ "Releasing a Patch", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_releasing_a_patch", [
        [ "Reverting a Patch Release", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_reverting", null ]
      ] ],
      [ "Upload a Patch to OTP", "pg_shasta_patch_user_guide.html#pgShasta_patch_user_guide_upload_a_patch_to_OTP", null ]
    ] ],
    [ "Firmware Modules", "pg_shasta_fw_modules.html", "pg_shasta_fw_modules" ],
    [ "OTP File System", "pg_shasta_otp_file_system.html", [
      [ "Introduction", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_Intro", null ],
      [ "OTP Partitions", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_partitions", [
        [ "Partition Tables", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_partitions_table", null ],
        [ "OTP Configuration Partitions", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_config_partition", [
          [ "OTP Configuration Files Format", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_configuration_file_format", null ]
        ] ],
        [ "OTP Patch Partitions", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_patch_partitions", [
          [ "OTP Patch Files Format", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_patch_file_format", null ]
        ] ]
      ] ],
      [ "OTP File CRC validation", "pg_shasta_otp_file_system.html#sctShasta_otp_file_crc_validation", null ],
      [ "OTP File invalidation", "pg_shasta_otp_file_system.html#sctShasta_otp_file_ivalidation", null ],
      [ "OTP Firmware Structure", "pg_shasta_otp_file_system.html#sctShasta_otp_fw_structure", null ],
      [ "OTP File Handling Examples", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_file_handling_examples", [
        [ "File Invalidation", "pg_shasta_otp_file_system.html#sctShasta_otp_file_system_file_invalidation", null ],
        [ "File Store", "pg_shasta_otp_file_system.html#pgsctShasta_otp_file_system_file_store", null ]
      ] ]
    ] ],
    [ "Exception Handling", "pg_shasta__exc.html", [
      [ "Introduction", "pg_shasta__exc.html#sctShasta_Exc_Intro", null ],
      [ "Exception Handling Strategy", "pg_shasta__exc.html#sctShasta_Exc_Strategy", [
        [ "Severity Levels and Reactions", "pg_shasta__exc.html#sctShasta_Exc_Severity_Levels", null ]
      ] ],
      [ "List of Errors", "pg_shasta__exc.html#sctShasta_Exc_Known_Errors", [
        [ "Everywhere", "pg_shasta__exc.html#sctShasta_Exc_Known_Errors_Everywhere", null ],
        [ "Boot sequence", "pg_shasta__exc.html#sctShasta_Exc_Known_Errors_Boot", null ],
        [ "OS kernel, inter-thread communication", "pg_shasta__exc.html#sctShasta_Exc_Known_Errors_Kernel", null ]
      ] ]
    ] ],
    [ "Custom RTOS Extensions", "pg_lib__rtos__ext.html", "pg_lib__rtos__ext" ],
    [ "User Example Code", "pg_user__app.html", [
      [ "Add On Features:", "pg_user__app.html#aof", null ],
      [ "Periodic Functions:", "pg_user__app.html#pf", null ],
      [ "Autogenerated PMBus Command Descriptors:", "pg_user__app.html#apcd", null ],
      [ "User PMBus Command Callbacks:", "pg_user__app.html#upcc", null ],
      [ "Ramp Data Snapshot Examples:", "pg_user__app.html#rdse", null ],
      [ "Regulation State Machine Callback Functions:", "pg_user__app.html#rsmcf", null ],
      [ "Main User Application:", "pg_user__app.html#mua", null ],
      [ "User NTC Temperature Look Up Table:", "pg_user__app.html#untlut", null ],
      [ "Debug Data App:", "pg_user__app.html#dda", null ],
      [ "Add On Features", "pg_user__app.html#addon", null ],
      [ "Periodic Functions", "pg_user__app.html#periodic", null ],
      [ "Autogenerated PMBus Command Descriptors", "pg_user__app.html#mfrauto", null ],
      [ "User PMBus Command Callbacks", "pg_user__app.html#mfrhandles", null ],
      [ "Ramp Data Snapshot Examples", "pg_user__app.html#ramp_data", null ],
      [ "Regulation State Machine Callback Functions", "pg_user__app.html#regsmcbs", null ],
      [ "Main User Application", "pg_user__app.html#usrdrv", null ],
      [ "User NTC Temperature Look Up Table", "pg_user__app.html#userntc", null ],
      [ "Debug Data App", "pg_user__app.html#DbgData", null ]
    ] ],
    [ "References", "pg_shasta__ref.html", [
      [ "Register Map and Pmbus Command Documentation", "pg_shasta__ref.html#sctShasta_Regmap", null ],
      [ "Interface to the FW", "pg_shasta__ref.html#sctShasta_Ref_Interface", null ],
      [ "RTX User Manual", "pg_shasta__ref.html#sctShasta_Ref_RTX", null ],
      [ "Internal Target Specification (ITS) of the CPU Subsystem (CPUS)", "pg_shasta__ref.html#sctShasta_Ref_CPUS_ITS", null ],
      [ "PMBUS Reference documentation", "pg_shasta__ref.html#sctShasta_Ref_Pmbus_Spec", null ],
      [ "Lauterbach Verilog T32 interface documentation", "pg_shasta__ref.html#sctShasta_Ref_Lauterbach_T32_RTL", null ],
      [ "The Definitive Guide to the ARM Cortex M0", "pg_shasta__ref.html#sctRefDefinitiveGuideToArmCortexM0", null ]
    ] ],
    [ "Revision History", "pg_shasta_revision_history.html", null ],
    [ "Lauterbach Debugger", "pg_shasta__lauterach.html", [
      [ "Introduction", "pg_shasta__lauterach.html#sctShasta_Lauterbuch_Introduction", null ],
      [ "Per File Generation", "pg_shasta__lauterach.html#sctShasta_Lauterbuch_per_files", null ]
    ] ],
    [ "Header File Template: cmsis_os.h", "cmsis_os_h.html", null ],
    [ "MISRA-C:2004 Compliance Exceptions", "_c_m_s_i_s__m_i_s_r_a__exceptions.html", null ],
    [ "Custom RTOS Patches", "pg_lib__rtos_patch.html", [
      [ "Wrapper for RTOS Functions", "pg_lib__rtos_patch.html#pgLib_Rtos_patch_wrapper", null ]
    ] ],
    [ "Exceptions", "_exceptions.html", null ],
    [ "Data Structures", "annotated.html", "annotated" ],
    [ "File List", "files.html", "files" ]
  ] ]
];

var NAVTREEINDEX =
[
"____main_8c.html",
"arm__math_8h.html#a59eafcdcdb52da60d37f20aec6ff4577",
"clk__ctrl__drv_8h.html#a1cefd11b8154d8968018d63286cf9278a1243c37064edae904ac2abf05c2ed258",
"cmsis__os__exceptions_8h.html#acd9f76a1fbd8bc9084ff34add637094fabf350750d0d4fabd8954c0f1e9bbae94",
"dma__drv_8h.html#a663c55ff87af46f2323c2ad04b62ef13",
"faults__api_8h.html#a51d2b28744f39280b1b452d10120fd7fa5e2e8bcdfbd28db5f0a404a8b1312e98",
"gpio_8h.html#a901a2bfc887c46149fff9a84d91ea9e2",
"group___c_m_s_i_s___core_debug.html#ga906476e53c1e1487c30f3a1181df9e30",
"group___c_m_s_i_s___d_w_t.html#gaf798ae34e2b9280ea64f4d9920cd2e7d",
"group___c_m_s_i_s___s_c_b.html#ga43e8cbe619c9980e0d1aacc85d9b9e47",
"group___c_m_s_i_s___s_c_b.html#gacb6992e7c7ddc27a370f62878a21ef72",
"group___c_m_s_i_s___t_p_i.html#ga07bafa971b8daf0d63b3f92b9ae7fa16",
"group___c_m_s_i_s__core__base.html#gaaaf6477c2bde2f00f99e3c2fd1060b01",
"group__cgu.html#ga0615c114abb6852510ab2a31b6ed24ef",
"group__cgu.html#ga30bcaa8c42d727511401cbd0ac60fe14",
"group__cgu.html#ga5fbbbddf4409c0a69b8a7f27bb3dc2b0",
"group__cgu.html#ga8e602211baec878e38b4eee382e3fd42",
"group__cgu.html#gabc487e79a2a6b201e588bac6023024be",
"group__cgu.html#gaed3b93438d5130dfe23b10ac8b18b6f2",
"group__common.html#ga0f9b221592b98091fba4e3ac5a2e927c",
"group__common.html#ga2d78128160d5c1ce70b49374850d255f",
"group__common.html#ga504d1ff3a0ac238521908fad736029e7",
"group__common.html#ga737d6fd20e37e66d2064986057aa1485",
"group__common.html#ga980db8fa2c750fb57b660beb89c8b9b0",
"group__common.html#gab8ef1908005485fbbc1c05fb0e2cc20a",
"group__common.html#gadd283dc0643305fc098c73562c0b3ab5",
"group__crc32.html#ga25160ee9693c7fa79956417c8eb62b77",
"group__dma__sh.html#gaa7931f2ca2360e4191691cec9d526d2f",
"group__dtimer.html#gaf1673850714254ff3a48b579ae5ca2ef",
"group__fault.html#ga4bc27532045323cbdba5b7e746e1fe10",
"group__fault.html#gad5267960e3b18d8ac5d85ee6276522d2",
"group__gpio.html#ga6eea99b34b2f50e1b9e947bbc848630d",
"group__gpio.html#gga4517755fff9e23134c57f621a8936fd3a8be6f1d8fbb3bfe3b479dcddb748a6e5",
"group__i2c.html#gaee641ec9756309b546cfabdbe833d208",
"group__mmu.html#ga0bd34ddf8002714b4f2d608280eab03a",
"group__mmu.html#gga70ac3749c83c2f78f6a5ecd786376f2baa54a95346e37c793e3db8c59fda78ad5",
"group__otp__sh.html#ga2c4ffdf6b8c117ff2eef90e3a7acf114",
"group__otp__sh.html#gaa2722e52e37497011fbb16afb55fc169",
"group__pid.html#ga17fce9eb2eeb941084dcb80c59d01f0d",
"group__pid.html#gacd702680e1f2d6a4b3be75436ec0badb",
"group__pmbus.html#ga5aeb084d33c2a11b10c7184654b36507",
"group__pmbus.html#gacf5b4b2fbe316c3b442aa16a094b77a9",
"group__pwm.html#ga1781648edb6ffe54111efc6046950823",
"group__pwm.html#ga5ee8c45dc2bae4a68233c7c5666b1d93",
"group__pwm.html#ga9b8b03a0af07312f3241c0a47e86d77f",
"group__pwm.html#gae479d292fa18c405f8eec1c730e0c5e7",
"group__rgu.html#ga71752f3e202e965daadbd220ab48e728",
"group__scu.html#ga107286815ad4f6fc57a15608d4d10924",
"group__scu.html#ga681ace8a1f3cddf89030100bdcf2fcff",
"group__scu.html#gac8d8ca8f2587a06590b6aab6477975e5",
"group__ssp.html#ga26d89f6a381a93236f729fd80d1eff5f",
"group__ssp.html#ga8a9a8d704f17ac67aee899d0bf64f20c",
"group__ssp.html#gafbaf24da2f645b303f481afdca2c0364",
"group__telem.html#gab1a193779d493e6a2708e31a6b53c3e3",
"group__test.html#ga365993159148cd0a490e7d6d5abf33d5",
"group__test.html#ga8474bad9bc992bca438591248973cbb0",
"group__test.html#gae08ada09cc926672da935de37d5146e9",
"group__teststat.html#ga35d162935168625b2511ced83d021b59",
"group__teststat.html#ga9109e0f0d4c0fcdbd9ea945ee602513c",
"group__teststat.html#gaeddf49a0fcef56d2a32c47bf911c36a2",
"group__tlmcom.html#ga7f820c4e9a0dc5395a18379c24ffe500",
"group__trim.html#ga13b274528159d800d13f7d65cb6bf5c7",
"group__trim.html#ga8504e3e930a675fbba4dc3c0fb2e43c1",
"group__trim.html#gaf6da4220b8811b52dab91a12e8ecbe2a",
"group__tsen.html#gae561cb99aa97010b02519ea4055485c5",
"group__uart.html#ga0ed3d7bcf24e60bfc089c60c48ea79f7",
"group__uart.html#ga77c1d0a1d555e670b0b75b3745ff6854",
"group__uart.html#gad4a63eceacd9f5a6a1507f7d99195e4a",
"group__vcontrol.html#ga4fd5cafbfde8a665e6294f877576458b",
"group__vsen.html#gaf1f86a6abc0d213858f4e798abbedd93",
"hal__dma_8h.html#ab03bdf447cfabd7df5b0adef0b265495",
"log_8h.html#ab59b6f06263786e0075630d69c3b04a4a6c2d00fcfaa570aeca1c3320d49adaca",
"otp__fs_8h.html#ae62fd3e221f8fc9102177b7a04ebd787",
"pg_user__app.html#regsmcbs",
"pmbus__autogen_8c.html#a2847b21280ef2a4f535e0297a35abdb3",
"pmbus__autogen_8c.html#aae05a09ac26981f9e734fd696022b118",
"pmbus__autogen_8h.html#a1ea95e16c229936d53a208385b34400f",
"pmbus__autogen_8h.html#a7b5be846941ef54e47e15479bf5d6a55",
"pmbus__autogen_8h.html#ac30c41a99054d1cad3c386cc7215cba8a740d707f926f8a308dc63cc3579bad64",
"pmbus__cmd__handlers_8c.html#ad71c38cbd3becac09d7519e60ec83829",
"ramp__data_8c.html#a3ba6d03227534e2bd600724aa0754846",
"regulation__state__machine__callbacks_8c.html#ac0a6bcca2bcacdf2918f754351a6c63e",
"struct___t_e_l_e_m___r_e_a_d___e_x_p__s.html#a0c57541b3dead122dd0e3c44f4662a65",
"struct_patch___r_o_m___table__t.html#aa4eaa663379453f0c2c929507daa4f54",
"structarm__fir__instance__f32.html",
"structpmbus__drv__state__t.html#a94d7403b900ef10d4c2fd91af6af62e9",
"telemetry__drv_8h.html#ac4e4f2766cdec267fc938cfc9d66784b",
"version_8c.html#a9bac22652d36175e43f3643051a168ce"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';