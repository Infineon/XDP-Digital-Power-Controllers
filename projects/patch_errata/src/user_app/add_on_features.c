/*
 * ADD_ON_FEATURES.c
 *
 *  Created on: Mar 7, 2018
 *      Author: ashendar
 */

//#include "pmbus_autogen.h"
//#include "pmbus_mfr_autogen.h"
//#include "dtimer_drv.h"
//#include "telemetry_drv.h"
#include "telemetry_api.h"
#include "version.h"
//#include "telemetry_irq_handlers.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "shasta_hal_tsen.h"
//#include "pmbus_api.h"
//#include "pmbus_gpio_control.h"
//#include "shasta_hal_vsen.h"
//#include "shasta_hal_vcontrol.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_analog.h"
//#include "shasta_hal_pid.h"
//#include "shasta_hal_scu.h"
//#include "shasta_hal_cgu.h"
//#include "shasta_hal_rgu.h"
//#include "shasta_hal_telem.h"
#include "shasta_hal_common.h"
//#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_isen.h"
#include "pmbus_api.h"

#include "log_app.h"             /* Logging of errors */
/* Standard library */

#include <string.h>             /* memset */
//#include "ramp_data.h"
#include "patch_rom_table.h"

//#include "shasta_hal_fault.h"

#include "string.h"
#include "add_on_features.h"
//#include "ramp_data.h"

#include "shasta_hal_tstctrl.h"  	// ate test state
#include "mmu_drv.h"                /* MMU_Initialization */

#include "driver_common.h"

#include "shasta_hal_fault_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"
#include "linker_sections.h"

#include "sys_boot_descr_format.h"  /* boot descriptor bitfield definitions */

#ifdef en_ishare
//#include "regulation_state_machine_callbacks.h"

#include "shasta.h"
#include <stdint.h>             /* uint*_t */
#endif

#ifdef State_machine_debug
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "cmsis_os_ext.h"               // ARM::CMSIS:RTOS:Keil RTX ext
#include "cmsis_os_patch.h"             // osMessageGet_patch
#include "rt_TypeDef.h"
#include "rt_Mailbox.h"
#endif

#ifdef State_machine_debug
/*lint -esym(526, REGULATION_APP_QUEUE) */
extern regulation_inter_thread_message_t REGULATION_APP_QUEUE[REGULATION_QUEUE_LENGTH];
/*lint -esym(526, regulation_inter_thread_queue) */
extern osMessageQId regulation_inter_thread_queue;//
/*lint -esym(526, regulation_inter_thread_mpool) */
extern osPoolId regulation_inter_thread_mpool;//
uint32_t debug_data[7];
uint16_t debug_data_index;
#endif

#ifdef configurator_crc_check
#include "pmbus_configuration_data.h"

typedef struct _Configurator_PMBUS_t
{
    /** Container for the size of the data. */
    uint16_t size;
    /** Container for the pointer to the actual data. */
    uint8_t* data;

} Configurator_PMBUS_t;

typedef struct _pmbus_partial_cmd_t
{
    /** loop container */
    uint8_t lp;
    /** cmd container */
    uint8_t cmd;
    /** Pointer to the partial command */
    uint8_t* ptr;

} pmbus_partial_cmd_t;

#define PARTIAL_CMD_MAX 32

typedef struct _pmbus_partial_t
{
    /** number of elements */
    uint8_t num_cmds;

    /** Pointer to the bit-table */
    pmbus_partial_cmd_t cmd_arr[PARTIAL_CMD_MAX];

} pmbus_partial_t;

typedef struct _configurator_partial_data_t
{
    /** Register address */
    uint32_t * addr;

    /** Register data */
    uint32_t data;

} configurator_partial_data_t;

CONFIGURATOR_ERROR_t* configurator_error_ptr;
CONFIGURATOR_STATUS_t* configurator_status_ptr;

Configurator_PMBUS_t*    Configurator_PMBUS_default_s_ptr[NUM_LOOPS];
Configurator_PMBUS_t*    Configurator_PMBUS_user_s_ptr[NUM_LOOPS];
pmbus_partial_t*         Configurator_PMBUS_partials_ptr;

//lint -esym( 526, configurator_xv_full) Symbol 'configurator_xv_full' not defined
extern uint8_t configurator_xv_full;
//lint -esym( 526, configurator_xv_partial) Symbol 'configurator_xv_partial' not defined
extern uint8_t configurator_xv_partial;
//lint -esym( 526, configurator_xv_partial_pmbus) Symbol 'configurator_xv_partial_pmbus' not defined
extern uint8_t configurator_xv_partial_pmbus;

#endif


void add_on_features_init (void)
{

#ifdef State_machine_debug
	//memset(RAMP_DATA_BUFFER_BASE, 0, RAMP_DATA_SIZE_IN_BYTES);
	Ramp_data_buffer_reset();
	debug_data_index = 0;
	debug_data[0] = 0xFFFFFFFF;
	debug_data[1] = 0xFFFFFFFF;
	debug_data[2] = 0xFFFFFFFF;
	(void)Ramp_data_buffer_write(PMBUS_PAGE_0, debug_data_index, 1, (uint32_t *)debug_data);
	(void)Ramp_data_buffer_write(PMBUS_PAGE_0, debug_data_index, 3, (uint32_t *)debug_data);
	//patch_rom_table.patch_Regulation_Wait_For_SM_Message = &patch_Regulation_Wait_For_SM_Message;
	patch_rom_table.patch_Regulation_Get_All_Q_Messages_By_Priority = &patch_Regulation_Get_All_Q_Messages_By_Priority;
#endif

#ifdef en_ishare
	patch_rom_table.patch_Regulation_Shutdown_Sequence = &patch_Regulation_Shutdown_Sequence;
#endif
#ifdef configurator_crc_check

	configurator_error_ptr = (CONFIGURATOR_ERROR_t*)0x2005c681; //point to configurator_error
	configurator_status_ptr = (CONFIGURATOR_STATUS_t*)0x2005c680; //point to configurator_error

	Configurator_PMBUS_default_s_ptr[0] = (Configurator_PMBUS_t*)0x2005c7b4;
	Configurator_PMBUS_default_s_ptr[1] = (Configurator_PMBUS_t*)0x2005c7bc;

	Configurator_PMBUS_user_s_ptr[0] = (Configurator_PMBUS_t*)0x2005c7a4;
	Configurator_PMBUS_user_s_ptr[1] = (Configurator_PMBUS_t*)0x2005c7ac;
	Configurator_PMBUS_partials_ptr = (pmbus_partial_t*)0x2005c6a0;

	patch_rom_table.patch_configurator_pmbus_set = patch_configurator_pmbus_set;
	patch_rom_table.patch_configurator_pmbus_partial_set = patch_configurator_pmbus_partial_set;
	patch_rom_table.patch_configurator_conf_partial_load = patch_configurator_conf_partial_load;

	RGU_RSTMODS_SET__BIFREGRST__SET((uint32_t) 1);
	RGU_RSTMODS_CLR__BIFREGRST__SET((uint32_t) 1);
    //Here we load trim and config register configurations with xvalent id set to 0
    if ((SYS_BD_CONFIG_DISABLE & sys_boot_descriptor) != SYS_BD_CONFIG_DISABLE)
    {
        configurator_boot_default();
    }

#endif

#ifdef JIRA_916_STORE_PARTIAL_CONFIG_MFR_IOUT_OC_FAST_FAULT_LIMIT
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_IOUT_OC_FAST_FAULT_LIMIT] = patch_PMBUS_HANDLE_MFR_IOUT_OC_FAST_FAULT_LIMIT;
#endif

#ifdef JIRA_774_FREQUENCY_SWITCH_ZERO
	patch_rom_table.patch_Regulation_Switching_Frequency_Set = &patch_Regulation_Switching_Frequency_Set;
#endif

#ifdef JIRA_745_PID_VERR_ENTRY_THRS
	patch_rom_table.patch_Regulation_disable_ata = &patch_Regulation_disable_ata;
#endif

#ifdef JIRA_771_PMBUS_DATA_COHERENCY
	patch_rom_table.patch_Telemetry_Sample = &patch_Telemetry_Sample;
#endif

#ifdef JIRA_769_SETUP_PASSWORD
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_SETUP_PASSWORD] = patch_PMBUS_HANDLE_MFR_SETUP_PASSWORD;
#endif

#ifdef JIRA_743_OFFSET_UNDERFLOW
	patch_rom_table.patch_Telemetry_get = &patch_Telemetry_get;
#endif

#ifdef JIRA_783_PATCH_IN_SAME_PARTITION_OF_INVALIDATED_PATCH
	patch_rom_table.patch_patch_otp_cmd_patch_version_process = &patch_patch_otp_cmd_patch_version_process;
#endif

#ifdef JIRA_785_REMAP_CLEAR_NEEDED_ON_RESET
	ptr_pmbus_callback[PMBUS_CMDCODE_MFR_FIRMWARE_COMMAND] = patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND;
#endif

}

#ifdef configurator_crc_check

STATIC uint8_t is_non_partial_xv_en(void)
{
   uint32_t xv_en = fw_params_xv_en_get();
   if ((xv_en == 1u) || (xv_en == 3u)) return 1u;

   return 0u;

}
STATIC uint8_t is_partial_xv_en(void)
{
    uint32_t xv_en = fw_params_xv_en_get();
    if ((xv_en == 1u) || (xv_en == 2u)) return 1u;

    return 0u;
}
void patch_configurator_pmbus_set(uint8_t lp, uint8_t conf_type)
{
    /* Update the Configuration Registers*/

    /*lint -e(838) Previously assigned value to variable 'dummy' has not been used -- Unclear*/
    Otp_File_System_Cmd_Hdr_t* config_ptr = NULL;
    uint8_t xv = 0;
    Configurator_PMBUS_t* ptr;

    if (conf_type == 1)
    {
        // Only apply the xvalent setting to user parameters
        if (is_non_partial_xv_en() == 1) xv = configurator_xv_full;
        //This is a user configuration
        ptr = Configurator_PMBUS_user_s_ptr[0];
    }
    else
    {
        xv = 0;
        //This is a default configuration
        ptr = Configurator_PMBUS_default_s_ptr[0];
    }

    /*lint -e{838} Previously assigned value to variable 'dummy' has not been used -- Unclear*/
    Otp_File_System_Cmd_Types_t type = {(Otp_File_System_Cmd_Types_t) (((uint8_t) OTP_FS_CMD_PMBUS_DEFAULT_LP0 +
            ((2 * lp) + conf_type)) +  (xv << 8))};

    Otp_File_System_Cmd_Handler_t config_data_handler =
            /* cmd_type | option | cmd_hdr_size | cmd_size_min | handler  */
            /*-------------------------------------------------------*/
           {type,      0,        0,             16,         NULL};


    TRY
        /* Check if file is existing */
        otp_fs_scan(OTP_PARTITION_DATA, (Otp_File_System_Cmd_Handler_t*)&config_data_handler, &config_ptr);
		/* Validate the checksum to detect errors.*/
		if (config_ptr != NULL)
		{
			THROW_IF(!otp_fs_ram_crc_hdr_is_valid((Otp_File_System_Cmd_Hdr_t*) config_ptr, 0), CORRUPT_MEMORY);
			THROW_IF(!otp_fs_ram_crc_data_is_valid((Otp_File_System_Cmd_Hdr_t*) config_ptr, 0), CORRUPT_MEMORY);
		}
    CATCH_OTHERS
        *configurator_error_ptr = CONFIGURATOR_READ_FAULT;
        log_error(CONFIGURATOR, WARNING, &EXCEPTION);
    TRY_END

    /*lint -e{774} Boolean within 'if' always evaluates to False . */ \
    /*lint -e{845} The right argument to operator '&&' is certain to be 0 -- Not true */ \
    /*lint -e{826} The right argument to operator '&&' is certain to be 0 -- Not true */ \
    /*lint -e{838} The right argument to operator '&&' is certain to be 0 -- Not true */ \
	if ((config_ptr == NULL) && (conf_type == 0))
	{
		// point all the loops to loop0 since the default data is the same for all loops
		config_ptr = (Otp_File_System_Cmd_Hdr_t*)pmbus_default_data_array_loop0;
	}

    /*lint -e{774} Boolean within 'if' always evaluates to False . */ \
    /*lint -e{845} The right argument to operator '&&' is certain to be 0 -- Not true */ \
    if ((( conf_type == 0) || (*configurator_error_ptr == CONFIGURATOR_NO_FAULT)) && (config_ptr != NULL)) {

        (ptr + lp)->size = (config_ptr->size | config_ptr->size_red) - (sizeof(Otp_File_System_Cmd_Hdr_t) + sizeof(crc_data_t));
        (ptr + lp)->data = (uint8_t *) config_ptr + sizeof(Otp_File_System_Cmd_Hdr_t);
    }
    else
    {
        LOG_ERROR_TYPE(PMBUS, WARNING, CORRUPT_MEMORY);
        (ptr + lp)->size = 0;
        (ptr + lp)->data = NULL;
    }

}

/**
 * configurator_pmbus_partial_load_handler creates a list of partial pmbus commands
 */
/*lint -e{715}   Suppress: Symbol 'otp_end' not referenced */
STATIC void configurator_pmbus_partial_load_handler(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint32_t otp_end)
{
    if (Configurator_PMBUS_partials_ptr->num_cmds >= PARTIAL_CMD_MAX)
    {
        LOG_ERROR_TYPE(PMBUS, WARNING, CORRUPT_MEMORY);
        return;
    }

    /*lint -e{826} Suspicious pointer-to-pointer conversion (area too small) */
    Otp_File_System_Cmd_Hdr_t * hdr_ptr = (Otp_File_System_Cmd_Hdr_t *) (fw_params_otp_partition_ptr_get(partition) + otp_offset);

    /*lint -e778 */
	if ((otp_fs_ram_crc_hdr_is_valid((Otp_File_System_Cmd_Hdr_t*) hdr_ptr, 0)) &&
			(otp_fs_ram_crc_data_is_valid((Otp_File_System_Cmd_Hdr_t*) hdr_ptr, 0)))
	{
		// We get pairs of 32bit address and 32bit data
		Configurator_PMBUS_partials_ptr->cmd_arr[Configurator_PMBUS_partials_ptr->num_cmds].lp = (hdr_ptr->option) >> 8;
		Configurator_PMBUS_partials_ptr->cmd_arr[Configurator_PMBUS_partials_ptr->num_cmds].cmd = (hdr_ptr->option) & 0xFFu ;
		Configurator_PMBUS_partials_ptr->cmd_arr[Configurator_PMBUS_partials_ptr->num_cmds].ptr = (uint8_t *) hdr_ptr ;

		Configurator_PMBUS_partials_ptr->num_cmds++;
	}
	else
	{
		*configurator_error_ptr = (CONFIGURATOR_ERROR_t)3;//CONFIGURATOR_PARTIALS_CRC_FAULT;
        LOG_ERROR_TYPE(CONFIGURATOR, WARNING, CORRUPT_MEMORY);
	}

}

/* The configurator_pmbus_partial_set function is used to retrieve a pointers to the
 *  partial pmbus configurations in OTP
 *
 */

void patch_configurator_pmbus_partial_set(void)
{

    /* Update the Configuration Registers*/

    memset (Configurator_PMBUS_partials_ptr, 0u, sizeof(Configurator_PMBUS_partials_ptr));

    /*lint -e(838) Previously assigned value to variable 'dummy' has not been used -- Unclear*/
    Otp_File_System_Cmd_Hdr_t* config_ptr = NULL;
    uint16_t xv = 0;
    if(is_partial_xv_en() == 1) xv = configurator_xv_partial_pmbus;

    /*lint -e{838} Previously assigned value to variable 'dummy' has not been used -- Unclear*/
    Otp_File_System_Cmd_Types_t type = {(Otp_File_System_Cmd_Types_t) (((uint16_t) OTP_FS_CMD_PMBUS_PARTIAL + (xv << 8)))};

    Otp_File_System_Cmd_Handler_t config_data_handler =
            /* cmd_type | option | cmd_hdr_size | cmd_size_min | handler  */
            /*-------------------------------------------------------*/
                {type,      0,        0,             16,         configurator_pmbus_partial_load_handler};


    TRY
        /* Check if file is existing */
        otp_fs_scan(OTP_PARTITION_DATA, (Otp_File_System_Cmd_Handler_t*)&config_data_handler, &config_ptr);
    CATCH_OTHERS
        *configurator_error_ptr = CONFIGURATOR_READ_FAULT;
        log_error(CONFIGURATOR, WARNING, &EXCEPTION);
    TRY_END

}

/**
 * configurator_conf_partial_register_load_handler copies the data of a partial configuration into
 * the HW registers
 */
/*lint -e{715}   Suppress: Symbol 'otp_end' not referenced */
STATIC void patch_configurator_conf_partial_register_load_handler(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint32_t otp_end)
{
    /*lint -e{826} Suspicious pointer-to-pointer conversion (area too small) */
    Otp_File_System_Cmd_Hdr_t * hdr_ptr = (Otp_File_System_Cmd_Hdr_t *) (fw_params_otp_partition_ptr_get(partition) + otp_offset);

	if ((otp_fs_ram_crc_hdr_is_valid((Otp_File_System_Cmd_Hdr_t*) hdr_ptr, 0)) &&
			(otp_fs_ram_crc_data_is_valid((Otp_File_System_Cmd_Hdr_t*) hdr_ptr, 0)))
	{
	    // We get pairs of 32bit address and 32bit data
	    uint16_t NumberofElements = ((hdr_ptr->size - sizeof(Otp_File_System_Cmd_Hdr_t))) >> 3 ;

	    configurator_partial_data_t *partial_data_ptr;

	    /*lint -e{826} Suspicious pointer-to-pointer conversion (area too small) */
	    partial_data_ptr = (configurator_partial_data_t *)((uint8_t*) hdr_ptr + sizeof(Otp_File_System_Cmd_Hdr_t));

	    for (uint8_t i = 0; i < NumberofElements; i++)
	    {
	        HAL_SET32((uint32_t *)partial_data_ptr->addr, partial_data_ptr->data);
	        partial_data_ptr++;
	    }
	}
	else
	{
		*configurator_error_ptr = (CONFIGURATOR_ERROR_t)3;//CONFIGURATOR_PARTIALS_CRC_FAULT;
        LOG_ERROR_TYPE(CONFIGURATOR, WARNING, CORRUPT_MEMORY);
	}

}
/* The configurator_conf_partial_load function is used to retrieve data from OTP
 * and store it in registers starting at address 0x7000_0000.
 *
 * @throws CORRUPT_MEMORY      if the data can not be retrieved from the OTP.
 * @throws OUT_OF_RESOURCE from otp_fs_scan.
 * @throws A @p handler specific exception.
 * @throws ILLEGAL_PARAMETER from otp_fs_scan.
 * @throws TIMEOUT from otp_fs_scan.
 *
 */

void patch_configurator_conf_partial_load(void)
{

    /* Update the Trim registers*/

    Otp_File_System_Cmd_Hdr_t* data_ptr;

    uint16_t xv = 0;
    if(is_partial_xv_en() == 1) xv = configurator_xv_partial;

    uint16_t d = (uint16_t) OTP_FS_CMD_CONF_PARTIAL + (uint16_t)(xv << 8);

    /*lint -e{838} Previously assigned value to variable 'dummy' has not been used -- Unclear*/
    Otp_File_System_Cmd_Types_t data_type = {(Otp_File_System_Cmd_Types_t) d};

    Otp_File_System_Cmd_Handler_t data_handler =
            /* cmd_type | option | cmd_hdr_size | cmd_size_min | handler                                          */
            /*----------------------------------------------------------------------------------------------------*/
           {data_type,      0,          0,             16,         patch_configurator_conf_partial_register_load_handler};


    *configurator_status_ptr = CONFIGURATOR_STATUS_BUSY;
    TRY
        otp_fs_scan(OTP_PARTITION_DATA, (Otp_File_System_Cmd_Handler_t*)&data_handler, &data_ptr);
    CATCH_OTHERS
        *configurator_error_ptr = CONFIGURATOR_WRITE_FAULT;
        log_error(CONFIGURATOR, WARNING, &EXCEPTION);
    TRY_END
    *configurator_status_ptr = CONFIGURATOR_STATUS_FREE;


}

#endif

#ifdef en_ishare

void enable_ishare(PMBUS_PAGE_t loop)
{

	uint16_t data = PMBUS_Get_Command_Word(PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD);
	data = (uint16_t)LINEAR11_TO_MANTISSA(data);  // grab mantissa
	uint8_t pwm11_bit_index = 6; //bit6

	if (data !=0)
	{
		//when enabled Low
		COMMON_MP_FUNC5__PWM11_FUNC__SET(1);  //GPIO0[6]
		COMMON_MP_FUNC5__PWM11_PD__SET(0); 	 //pulldown disabled
		COMMON_MP_FUNC5__PWM11_PU_N__SET(1);  //pullup disabled
		GpioBit_SetCnfg (GPIO_0, pwm11_bit_index);
		GpioBit_SetDir (GPIO_0, pwm11_bit_index, DIRECTION_OUTPUT);

		TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(1);// IMON output current DAC enable
		COMMON_EN_REG3__EN_ISHARE__SET(1);//disable for IMON based current sharing function

		// PMW11

		GpioBit_DatWr(GPIO_0, pwm11_bit_index, 0x0); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location
		COMMON_MP_FUNC5__PWM11_PPEN__SET(1);  //CMOS output 0 -	open drain output

	}

	else

	{
		TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(0);// IMON output current DAC enable
		COMMON_EN_REG3__EN_ISHARE__SET(0);//disable for IMON based current sharing function
	}

	// Read mfr_added_droop
	uint32_t mfr_added_vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP);

	// Align VOUT_DROOP to PMBUS_Q_EXP precision
	mfr_added_vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)mfr_added_vout_droop);

	if (mfr_added_vout_droop !=0)
	{
		user_data.enable_added_droop = 1; //enable periodic irq at target to remove droop
		// Read vout_scale and vout_droop
		uint32_t vout_scale = PMBUS_Get_Vout_Scale(loop, PMBUS_Q_EXP);

		uint32_t vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_DROOP);
		int32_t vout_droop_exp = LINEAR11_TO_EXPONENT((uint16_t)vout_droop);

		mfr_added_vout_droop = PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_MFR_ADDED_DROOP_DURING_RAMP);
		int32_t mfr_added_vout_droop_exp = LINEAR11_TO_EXPONENT((uint16_t)mfr_added_vout_droop);

		// Grab mantissa of vout droop
		vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)vout_droop);
		// multiply droop by vout scale and shift to LOADLINE_EXP
		uint32_t vout_droop_mohms = (uint32_t)SHIFT_EXPONENT((int32_t)(vout_droop * vout_scale) , (-LOADLINE_EXP+vout_droop_exp+PMBUS_Q_EXP));


		// Grab mantissa of vout droop
		mfr_added_vout_droop = (uint32_t)LINEAR11_TO_MANTISSA((uint16_t)mfr_added_vout_droop);
		// multiply droop by vout scale and shift to LOADLINE_EXP
		uint32_t mfr_added_vout_droop_mohms = (uint32_t)SHIFT_EXPONENT((int32_t)(mfr_added_vout_droop * vout_scale) , (-LOADLINE_EXP+mfr_added_vout_droop_exp+PMBUS_Q_EXP));


		uint32_t mohms = vout_droop_mohms + mfr_added_vout_droop_mohms;
		user_data.calculated_droop = vout_droop_mohms;
		user_data.current_droop = mohms;

		if (mohms > 0x7ff) mohms = 0x7ff;  // cap at max
		VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) loop, mohms);
	}
}

/*lint -e715 */
void disable_ishare(PMBUS_PAGE_t loop)
{

	uint16_t data = PMBUS_Get_Command_Word(PMBUS_PAGE_0, PMBUS_CMDCODE_MFR_ISHARE_THRESHOLD);
	data = (uint16_t)LINEAR11_TO_MANTISSA(data);  // grab mantissa

	TSEN_TSEN_EN__TS_TSIDAC_IMON_SEL__SET(0);// IMON output current DAC disable
	COMMON_EN_REG3__EN_ISHARE__SET(0);//disable for IMON based current sharing function

	if (data !=0)
	{
		uint8_t pwm11_bit_index = 6; //bit6
		COMMON_MP_FUNC5__PWM11_PD__SET(0); 	 //pulldown disabled
		COMMON_MP_FUNC5__PWM11_PU_N__SET(1);  //pullup disabled


		// PMW11
		GpioBit_SetCnfg (GPIO_0, pwm11_bit_index);
		GpioBit_SetDir (GPIO_0, pwm11_bit_index, DIRECTION_OUTPUT);
		COMMON_MP_FUNC5__PWM11_PPEN__SET(0);  //CMOS output 0 -	open drain output
		GpioBit_DatWr(GPIO_0, pwm11_bit_index, 0xFF); // gpio driver requires a value of 1 on bus to be 0xff in index of that data location

		COMMON_MP_FUNC5__PWM11_FUNC__SET(1);  //GPIO0[5]

	}
}


/**
 * Function to disable the added droop feature
 * @param loop regulation loop to operate on.
 */
void added_droop_disable(PMBUS_PAGE_t loop)
{
	Regulation_setup_fsw_irq(loop, fsw_irq_idx_2, fsw_irq_rate_sel_disabled);
}

/**
 * Function to enable the added droop feature
 * @param loop regulation loop to operate on.
 */
void added_droop_enable(PMBUS_PAGE_t loop)
{
	// if the pmbus command is 0 disable feature
	if (user_data.enable_added_droop != 0)
	{
		if(loop==PMBUS_PAGE_0)
		{
			user_data.remove_added_droop_counter = 0;
			// enalbe the irq every 8 cycles
			Regulation_setup_fsw_irq(loop, fsw_irq_idx_2, fsw_irq_rate_sel_8);
		}
	}
	//else
	//{
	//	added_droop_disable(loop);
	//}
}

/**
 * Interrupt service callback for FSW_IRQn
 * This is used removed the added droop at startup
 * by slowly decreasing it syncronized with fsw
 * @param loop regulation loop to operate on.
 */
void remove_added_droop_irq_callback(PMBUS_PAGE_t loop)
{

	uint32_t mohms = ((user_data.current_droop - user_data.calculated_droop)>>1U) -1;
	user_data.current_droop -= mohms;

	if (user_data.current_droop > 0x7ff) user_data.current_droop = 0x7ff;  // cap at max
	//if (user_data.current_droop < 0x0) user_data.current_droop = 0x0;  // cap at min

	if ((user_data.current_droop <= user_data.calculated_droop) || (mohms == 0))
	{
		user_data.current_droop = user_data.calculated_droop;
		//VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) loop, user_data.current_droop);
		// disable the interrupt
		user_data.enable_added_droop = 0; //enable periodic irq at target to remove droop
		Regulation_setup_fsw_irq(PMBUS_PAGE_0, fsw_irq_idx_2, fsw_irq_rate_sel_disabled);
	}

	user_data.remove_added_droop_counter++;  // keep track of interrupt count
	VCONTROL_VC_VAVP3__VC_VAVP_RLL__SET((vcontrol_idx_t) PMBUS_PAGE_0, (uint32_t)user_data.current_droop);

}

void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control)
{
	//////////////SHUTDOWN SEQUENCE//////////////
	//Mask faults
	Regulation_Fault_Mask (loop, state, control);

	Regulation_PWM_Control(loop, REGULATION_PWMS_OFF);  // ALL FET's OFF
	Regulation_Set_VID(loop, 0, 0, 0, 0, 1);
	VCONTROL_VC_VRAMP1__VC_VRAMP_ON__SET((vcontrol_idx_t)loop, 0);  // turn off vramp_on signal to allow pwms to stop regulation
	// clear the interrupt here in case it was left over from a fault
	VCONTROL_VC_IRQ__VC_VCONTROL_IRQ_EN__SET((vcontrol_idx_t) loop, 0);

	//disable the hardware blocks: (telemetry and faults are always active but masked at different times)
	Regulation_Power_Mode (loop, state);

	// disable power good and ton_max interrupts
	TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__SET(TLMCOM_TLM_IRQ_EN__TLM_IRQ_EN__GET() &
																	~((1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_0) |
																			(1u<<(uint32_t)VOUT_UV_TON_MAX_DISABLE_1)));

	// update pmbus status off bit:
	PMBUS_STATUS_OFF__SET((pmbus_status_loop_idx_t)loop, 1);
	// update status word from result:
	PMBUS_Inter_Thread_Set_Command((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_STATUS_WORD, PMBUS_READ);

	// Clear HW Fault Shutdown mask
	FAULT_FAULT_SHUT_CLR_LOOP__FAULT_SHUT_CLR_LOOP__SET((fault_idx_t) REGULATION_APP.loop, 1) ;

	if (loop == PMBUS_PAGE_0)
	{
	FAULTCOM_FAULT_SHUT_CLR_COM__FAULT_SHUT_CLR_COM__SET(1) ;
	}
	///////////////////////////////

	disable_ishare(PMBUS_PAGE_0);
}
#endif

#ifdef State_machine_debug

void patch_Regulation_Get_All_Q_Messages_By_Priority (void)
{
	osEvent evt;
	regulation_inter_thread_message_t *message;

	// first we read the rtos queue count to see how many messages exist
	REGULATION_APP.mbx_cnt = rt_mbx_count(regulation_inter_thread_queue);

	// then we pop all the queue messages to our local queue to rearrange priority
	for (uint8_t i=0; i<REGULATION_APP.mbx_cnt; i++)
	{
		// if the count is less than our queue size (they should be equal unless someone changed defines)
		if (REGULATION_APP.mbx_size<REGULATION_QUEUE_LENGTH)
		{
			// pop from queue
			evt = osMessageGet(regulation_inter_thread_queue, 0);
			// check that the status is ok
			if (evt.status == osEventMessage)
			{
				// get the message pointer
				message = (regulation_inter_thread_message_t*)evt.value.p;
				// increment the local queue write pointer and current size
				// store the message in our local queue
				if (message != NULL )
				{
					REGULATION_APP_QUEUE[REGULATION_APP.rear].loop =	message->loop;
					REGULATION_APP_QUEUE[REGULATION_APP.rear].control = message->control;
					REGULATION_APP_QUEUE[REGULATION_APP.rear].control_data = message->control_data;
				}
				REGULATION_APP.rear++;
				REGULATION_APP.mbx_size++;
				// wrap the write pointer when at the queue size back to 0
				if (REGULATION_APP.rear >= REGULATION_QUEUE_LENGTH)
				{
					REGULATION_APP.rear = 0;
				}
				evt.status = osPoolFree(regulation_inter_thread_mpool, message);  // free the pool
			}
		}
	}

	regulation_inter_thread_message_t high_priority_control;
	uint32_t high_priority_index = REGULATION_APP.front;
	high_priority_control.control = (REGULATION_CONTROLS)0xff;
	// now we have our local queue of all the rtos queued messages.
	// now search it for the highest priority message and store its index and data locally
	for(uint32_t y=0; y<REGULATION_APP.mbx_size; y++)
	{
		uint32_t queue_index = (REGULATION_APP.front +y) & (REGULATION_QUEUE_LENGTH-1);  //%32
		if(REGULATION_APP_QUEUE[queue_index].control<high_priority_control.control )
		{
			high_priority_control = REGULATION_APP_QUEUE[queue_index];
			high_priority_index = queue_index;
		}
	}
	if (REGULATION_APP.mbx_size!=0)
	{
		// so here we need to decrement from high priority index back to front
		// if queue index is equal to 0 currently set to 31 in next loop iteration
		// otherwise decrement until qidx == front
		// since we push to front the highest priority we need to
		// shift everything back until we get to the message we replaced:
		// note that high_priority_index has already been stored above so we write to that location first
		for(uint32_t qidx = high_priority_index; qidx!=REGULATION_APP.front; qidx = (qidx==0) ? (REGULATION_QUEUE_LENGTH-1): (qidx-1))
		{
			uint32_t next_index = (qidx==0) ? (REGULATION_QUEUE_LENGTH-1): (qidx-1);  // 1 index below the current q index
			REGULATION_APP_QUEUE[qidx] = REGULATION_APP_QUEUE[next_index];
		}
		REGULATION_APP_QUEUE[REGULATION_APP.front] = high_priority_control;  // finally put the highest priority at the front
	}

	//Pop the last highest priority message
	REGULATION_APP.loop =	REGULATION_APP_QUEUE[REGULATION_APP.front].loop;
	REGULATION_APP.control[REGULATION_APP.loop] = REGULATION_APP_QUEUE[REGULATION_APP.front].control;
	REGULATION_APP.control_data[REGULATION_APP.loop] = REGULATION_APP_QUEUE[REGULATION_APP.front].control_data;

	///////////////debug using ramp data circular buffer example//////////////
	if (REGULATION_APP.control[REGULATION_APP.loop] != REGULATION_CONTROLS_TELEMETRY_UPDATED)
	{
		debug_data[0] = (uint32_t)REGULATION_APP.current_state[REGULATION_APP.loop];
		debug_data[1] = (uint32_t)REGULATION_APP.control[REGULATION_APP.loop];
		debug_data[2] = (uint32_t)REGULATION_APP.control_data[REGULATION_APP.loop];

		//The number of 4-byte words of the log's content
		debug_data_index++;
		uint8_t length = 3; //sizeof(debug_data)/sizeof(uint32_t);
		(void)Ramp_data_buffer_write(REGULATION_APP.loop, debug_data_index, length, (uint32_t *)debug_data);
	}
	//////////////////////////debug using ramp data circular buffer example

	// increment read pointer and update queue size, wrap pointer to 0 when greater than queue size:
	REGULATION_APP.front++;  // looking for a number between 0 - 31 when REGULATION_QUEUE_LENGTH == 32
	REGULATION_APP.mbx_size--; // looking for a number between 0 - 32 when REGULATION_QUEUE_LENGTH == 32
	if (REGULATION_APP.front >= REGULATION_QUEUE_LENGTH)
	{
		REGULATION_APP.front = 0;
	}
}
#endif

#ifdef JIRA_916_STORE_PARTIAL_CONFIG_MFR_IOUT_OC_FAST_FAULT_LIMIT

void patch_PMBUS_HANDLE_MFR_IOUT_OC_FAST_FAULT_LIMIT(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		int32_t exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_IOUT_OC_FAST_FAULT_LIMIT));
		int16_t mantissa = (int16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word(page, PMBUS_CMDCODE_MFR_IOUT_OC_FAST_FAULT_LIMIT));
		uint8_t fault_response_cmd_byte = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_IOUT_OC_FAST_FAULT_RESPONSE);
		FAULTS_SETUP_FAULT(page, FAULT_TYPE_MFR_IOUT_OC_FAST, fault_response_cmd_byte,
								FAULTS_CONFIG_IOUT_DELAY_UNIT, CURRENT_RESPONSE_TYPE,
								(uint8_t)PMBUS_CMDCODE_STATUS_MFR_SPECIFIC, (uint8_t)PMBUS_STATUS_MFR_SPECIFIC_5, (FAULT_TYPE_t) FAULT_TYPE_MFR_IOUT_OC_FAST);
		//Fault_Mfr_Oc_Threshold_Set(page, mantissa, exponent);
		Fault_Threshold_Set(page, FAULT_TYPE_MFR_IOUT_OC_FAST, (int32_t)mantissa, (int32_t)exponent);
	}
	if (direction == OTP_RESTORE)
	{
		PMBUS_HANDLE_IOUT_OC_LV_FAULT_LIMIT(page, direction);
	}
}
#endif

#ifdef JIRA_774_FREQUENCY_SWITCH_ZERO
/*
void patch_Regulation_Switching_Frequency_Set(PMBUS_PAGE_t loop, uint16_t freq_khz)
{
	// avoid divide by 0
	if (freq_khz > 0)
	{
		uint32_t period = 1000000/(freq_khz*SWITCHING_PERIOD_LSB_NS);
		if (period > 0x1ff) period = 0x1ff;
		if (loop == PMBUS_PAGE_0)
		{
			PWM_RAMP0_TSWITCH0__TSWITCH0__SET(period);
		}
		else
		{
			PWM_RAMP1_TSWITCH1__TSWITCH1__SET(period);
		}
	}
}
*/
void patch_Regulation_Switching_Frequency_Set(PMBUS_PAGE_t loop, uint16_t freq_khz)
{

       uint32_t period;
       if (freq_khz == 0) period = 0x1ff;
       else
       {
              period = 1000000/(freq_khz*SWITCHING_PERIOD_LSB_NS);
              if (period > 0x1ff) period = 0x1ff;
       }

       if (loop == PMBUS_PAGE_0)
       {
              PWM_RAMP0_TSWITCH0__TSWITCH0__SET(period);
       }
       else
       {
              PWM_RAMP1_TSWITCH1__TSWITCH1__SET(period);
       }
}

#endif

#ifdef JIRA_745_PID_VERR_ENTRY_THRS
void patch_Regulation_disable_ata(PMBUS_PAGE_t loop)
{
	PID_PID_TRANS__PID_VERR_ENTRY_THRS__SET((pid_idx_t) loop, 0);
	PID_PID_OVS__PID_OVS_ENTRY_THRS__SET((pid_idx_t) loop, 0);
}
#endif

#ifdef JIRA_771_PMBUS_DATA_COHERENCY
void patch_Telemetry_Sample(void)
{
	uint8_t loop;
	uint32_t vout, vin, iin, pin, pout, duty, fsw;

	int8_t vout_read_exp[NUM_LOOPS];
	int8_t vin_read_exp[NUM_LOOPS];
	int8_t iout_read_exp[NUM_LOOPS];
	int8_t iin_read_exp[NUM_LOOPS];
	int8_t duty_read_exp[NUM_LOOPS];
	int8_t power_read_exp[NUM_LOOPS];
	int8_t fan_read_exp[NUM_LOOPS];
	int8_t fsw_read_exp[NUM_LOOPS];

	int32_t iout;
	int32_t tempa, tempb, tempi;
	int32_t temp_1, temp_2;
	// update the telemetry data at the bandwidth period:

	//for (uint8_t loop=0;loop<(uint8_t)NUM_LOOPS;loop++)
	for(loop=0; loop<(uint8_t)NUM_LOOPS; loop++)  // update for the number of loops in the system
	{
	//GET_EXPONENTS
		//int32_t vout_exp = PMBUS_Q_EXP;
		vout_read_exp[loop] = (int8_t)PMBUS_Get_Vout_Mode_Exponent((PMBUS_PAGE_t)loop);
		fsw_read_exp[loop]  = (int8_t)LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_FREQUENCY_SWITCH));
		vin_read_exp[loop]  = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_VIN_EXP));
		iout_read_exp[loop] = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IOUT_EXP));
		iin_read_exp[loop]  = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IIN_EXP));
		duty_read_exp[loop] = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_DUTY_EXP));
		power_read_exp[loop]= (int8_t)TWOS_UNCOMPLEMENT(3, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_POWER_EXP));
		fan_read_exp[loop]  = (int8_t)TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)PMBUS_PAGE_0, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_FAN_EXP));

	//VOUT_TELEM
		vout = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, VOUT_TELEM);
		vout = (uint32_t)SHIFT_EXPONENT((int32_t)vout,  PMBUS_Q_EXP - vout_read_exp[loop]);  // convert to vout_mode format
		if (vout>0xFFFF) vout = 0xFFFF;
		TELEM_DATA.vout[loop] = vout;
	//IOUT_TELEM
		iout = Telemetry_get((PMBUS_PAGE_t)loop, IOUT_TELEM);
		if (iout>1023) iout = 1023;
		if (iout<-1023) iout = -1023;
		TELEM_DATA.iout[loop] = iout;
	//VIN_TELEM
		vin = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, VIN_TELEM);
		if (vin>0x3FF) vin = 0x3FF;
		TELEM_DATA.vin[loop] = vin;
	//IIN_TELEM
		iin = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, IIN_TELEM);
		if (iin>0x3FF) iin = 0x3FF;
		TELEM_DATA.iin[loop] = iin;
	//DUTY_TELEM
		duty = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, DUTY_TELEM);
		if (duty>0x3FF) duty = 0x3FF;
		TELEM_DATA.duty[loop] = duty;
	//FSW_TELEM
		fsw = (uint32_t)Telemetry_get((PMBUS_PAGE_t)loop, FSW_TELEM);
		if (fsw>0x3FF)  fsw = 0x3FF;
		TELEM_DATA.fsw[loop] = fsw;
	//POUT_TELEM
		pout = (uint32_t)Telemetry_get_power(vout, vout_read_exp[loop], iout, iout_read_exp[loop], power_read_exp[loop]); //in read_power_exp
		if (pout>0x3ff) pout=0x3ff;
		TELEM_DATA.pout[loop] = pout;
	//PIN_TELEM
		pin = (uint32_t)Telemetry_get_power(vin, vin_read_exp[loop], (int32_t)iin, iin_read_exp[loop], power_read_exp[loop]);
		if (pin > 0x3ff) pin = 0x3ff;  // cap value
		TELEM_DATA.pin[loop] = pin;

		if (Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_FW_FILTER_EN) == 1) {
			vout = (vout + (uint32_t)(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_VOUT)))>>1;
			/*lint -e702 */
			iout = (iout + (LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_IOUT))))>>1u;
			vin  = (vin  + (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_VIN))))>>1u;
			iin  = (iin  + (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_IIN))))>>1u;
			duty = (duty + (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_DUTY_CYCLE))))>>1u;
			fsw  = (fsw +  (uint32_t)(LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_FREQUENCY))))>>1u;
		}

		TELEMETRY_VIN_SOURCE source = Telemetry_get_vin_source((PMBUS_PAGE_t)loop);
		NVIC_DisableIRQ(PMBUS_IRQn);
		if ((ON_OFF_STATE.On_Off_State[loop]==0)&&
				((source == (TELEMETRY_VIN_SOURCE)TELEMETRY_VIN_SOURCE_VRECT1) || (source == (TELEMETRY_VIN_SOURCE)TELEMETRY_VIN_SOURCE_VRECT2)))
		{
			// if we have vrect sensing and not actively switching we need to set read_vin to 0v
			PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_VIN, (uint16_t)(((TWOS_COMPLEMENT (5, vin_read_exp[loop]))<<11)));
		}
		else
		{
			// otherwise we have real telemetry for vin from primary or forced values
			PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_VIN, (uint16_t)(((TWOS_COMPLEMENT (5, vin_read_exp[loop]))<<11) | (vin & 0x7ff))); // read_vin is u7.2
		}
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_VOUT,      (uint16_t)vout);
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_IOUT,      (uint16_t)( (TWOS_COMPLEMENT (5, iout_read_exp[loop] )<<11) | ((uint16_t)iout & 0x7ff))); // read_iout is s7.2
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_IIN,       (uint16_t)( (TWOS_COMPLEMENT (5, iin_read_exp[loop]  )<<11) | (iin  & 0x7ff))); // read_iin is u6.4
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_POUT,      (uint16_t)( (TWOS_COMPLEMENT (5, power_read_exp[loop])<<11) | (pout & 0x7ff))); // fixed at u10.0 format
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_PIN,       (uint16_t)( (TWOS_COMPLEMENT (5, power_read_exp[loop])<<11) | (pin  & 0x7ff)));
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_DUTY_CYCLE,(uint16_t)( (TWOS_COMPLEMENT (5, duty_read_exp[loop] )<<11) | (duty & 0x7ff)));  // u7.3 %
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, (uint8_t)PMBUS_CMDCODE_READ_FREQUENCY, (uint16_t)( (TWOS_COMPLEMENT (5, fsw_read_exp[loop]  )<<11) | (fsw  & 0x7ff)));  // Return READ_FREQUENCY in FREQUENCY_SWITCH format
		NVIC_EnableIRQ(PMBUS_IRQn);
		//GET_POWER_Thresholds
			int32_t exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_POUT_OP_WARN_LIMIT));
			int16_t mantissa = (int16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_POUT_OP_WARN_LIMIT));
			POWER_WARN_THRESH[loop].pout_op_warn_thresh = (uint16_t) SHIFT_EXPONENT(mantissa, (- power_read_exp[loop] + exponent));
			exponent = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_PIN_OP_WARN_LIMIT));
			mantissa = (int16_t)LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_PIN_OP_WARN_LIMIT));
			POWER_WARN_THRESH[loop].pin_op_warn_thresh = (uint16_t) SHIFT_EXPONENT(mantissa, (- power_read_exp[loop] + exponent));

		//POUT_OP Firmware Fault
		if (pout>POWER_WARN_THRESH[loop].pout_op_warn_thresh)
		{
			Set_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_POUT_OP_WARN);
		}
		else
		{
			Clear_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_POUT_OP_WARN);
		}

		//PIN_OP Firmware Fault
		if (pin>POWER_WARN_THRESH[loop].pin_op_warn_thresh)
		{
			Set_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_PIN_OP_WARN);
		}
		else
		{
			Clear_Firmware_Fault((PMBUS_PAGE_t)loop, FAULT_TYPE_PIN_OP_WARN);
		}

	}

  //ATSEN_TELEM
	tempa = Telemetry_get(PMBUS_PAGE_0, ATSEN_TELEM);
	TELEM_DATA.temperature[0] = tempa;
  //BTSEN_TELEM
	tempb = Telemetry_get(PMBUS_PAGE_0, BTSEN_TELEM);
	TELEM_DATA.temperature[1] = tempb;
  //ITSEN_TELEM
	tempi = Telemetry_get_tempi();
	TELEM_DATA.temperature[2] = tempi;
	//tempa = Telemetry_get((PMBUS_PAGE_t)loop, ITSEN_TELEM);

	// Here we determine which READ_TEMPERATURE gets tempA, tempB, and tempI
	for(loop=0; loop<NUM_LOOPS; loop++)
	{
		switch ((PMBUS_Get_Command_Byte ((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_MFR_SELECT_TEMPERATURE_SENSOR)) & 0x7 )
		{
			case 0:
				temp_1 = tempa;
				temp_2 = tempb;
				break;
			case 1:
				temp_1 = tempa;
				temp_2 = tempi;
				break;
			case 2:
				temp_1 = tempb;
				temp_2 = tempa;
				break;
			case 3:
				temp_1 = tempb;
				temp_2 = tempi;
				break;
			case 4:
				temp_1 = tempi;
				temp_2 = tempa;
				break;
			case 5:
				temp_1 = tempi;
				temp_2 = tempb;
				break;
			default:
				temp_1 = tempa;
				temp_2 = tempb;
				break;
		}

		if (Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_FW_FILTER_EN) == 1) {
			/*lint -e702 */
			temp_1 = (temp_1 + (LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_1))))>>1u;
			/*lint -e702 */
			temp_2 = (temp_2 + (LINEAR11_TO_MANTISSA(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_2))))>>1u;
		}

		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_1, ((uint16_t)temp_1 & 0x7ff));
		NVIC_EnableIRQ(PMBUS_IRQn);
		NVIC_DisableIRQ(PMBUS_IRQn);
		PMBUS_Set_Command_Word((PMBUS_PAGE_W_BROADCAST_t)loop, PMBUS_CMDCODE_READ_TEMPERATURE_2, ((uint16_t)temp_2 & 0x7ff));
		NVIC_EnableIRQ(PMBUS_IRQn);

	}

	uint16_t fan_speed1 = Telemetry_get_fan_speed(FAN1);
	uint16_t fan_speed2 = Telemetry_get_fan_speed(FAN2);

	// Get fan speed, replicate on both loops
	// since the bottom 8 lsb's of fan_speed register are always zero we shift those off and use +6 exponent to get 64 to 65k rpm
	int32_t temp = SHIFT_EXPONENT(fan_speed1, -(fan_read_exp[PMBUS_PAGE_0]));
	if (temp > 0x3ff) temp = 0x3ff;  // clamp fan speed at mantissa size
	NVIC_DisableIRQ(PMBUS_IRQn);
	PMBUS_Set_Command_Word(PMBUS_PAGE_WB_BROADCAST, (uint8_t)PMBUS_CMDCODE_READ_FAN_SPEED_1, (uint16_t)(((TWOS_COMPLEMENT (5, fan_read_exp[PMBUS_PAGE_0]))<<11)
			| temp));
	NVIC_EnableIRQ(PMBUS_IRQn);
	NVIC_DisableIRQ(PMBUS_IRQn);
	temp = SHIFT_EXPONENT(fan_speed2, -(fan_read_exp[PMBUS_PAGE_0]));  // shift the fan speed by the exponent
	if (temp > 0x3ff) temp = 0x3ff;  // clamp fan speed at mantissa size
	PMBUS_Set_Command_Word(PMBUS_PAGE_WB_BROADCAST, (uint8_t)PMBUS_CMDCODE_READ_FAN_SPEED_2, (uint16_t)(((TWOS_COMPLEMENT (5, fan_read_exp[PMBUS_PAGE_0]))<<11)
			| temp));
	NVIC_EnableIRQ(PMBUS_IRQn);

	call_system_cb(TELEMETRY_SAMPLE);

}

#endif


#ifdef JIRA_769_SETUP_PASSWORD
void patch_PMBUS_HANDLE_MFR_SETUP_PASSWORD(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	uint8_t * data_p;
	PMBUS_HANDLE_MFR_SETUP_PASSWORD(page, direction);
	// zero out extra data on writes
	if ((direction == PMBUS_WRITE))
	{
		data_p = PMBUS_GET_DATA_ARRAY_POINTER(page, PMBUS_CMDCODE_MFR_SETUP_PASSWORD);
		if (data_p!=NULL)
		{
			data_p[5] = 0;
			data_p[4] = 0;
		}
	}
#ifdef JIRA_754_SECURITY_DISABLED_ON_OTP_RESTORE
	if ((direction == OTP_RESTORE) && (protection_data.hash!=0))
	{
		protection_data.password_locked = 1; // indicate security on by starting at 1
	}
#endif
}
#endif


#ifdef JIRA_743_OFFSET_UNDERFLOW
/**
 * Generic function to get telemetry values
 * @param loop Selects the loop to operate on.  Valid range of 0 or 1.
 * @return vout in vout_exp format unsigned
 */
int32_t patch_Telemetry_get(PMBUS_PAGE_t loop, TELEMETRY_TYPE_t telem_type)
{

	int32_t telem;
    uint32_t telem_unsigned = 0;
	int32_t gain;
	int32_t offset;
	uint32_t vout_scale;
	int32_t Telemetry_exponent;
	int32_t read_exp;

	switch (telem_type)
	{
		case VOUT_TELEM:
			telem = (int32_t)TELEM_TLM_VOUT_FSW__GET((telem_idx_t)loop);  // lsb = 1.25mv u12.0
			//debug[0] = 0xAA;
			Telemetry_exponent = Telemetry_VOUT_EXP;
			read_exp = PMBUS_Q_EXP;
			break;
		case IOUT_TELEM :
			telem = (int32_t)TWOS_UNCOMPLEMENT(13, (TELEM_TLM_IOUT_FSW__GET((telem_idx_t)loop)));  // S9.4, LSB 62.5mA
			Telemetry_exponent = Telemetry_IOUT_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IOUT_EXP));
			break;
		case VIN_TELEM :
			telem = (int32_t)TELEM_TLM_VIN_LPF__GET((telem_idx_t)loop); //u7.4
			Telemetry_exponent = Telemetry_VIN_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_VIN_EXP));
			break;
		case IIN_TELEM :
			telem = (int32_t)TELEM_TLM_IIN_LPF__GET((telem_idx_t)loop);
			Telemetry_exponent = Telemetry_IIN_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_IIN_EXP));
			break;
		case ATSEN_TELEM :
			telem = (int32_t)TLMCOM_TLM_ATSEN_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case BTSEN_TELEM :
			telem = (int32_t)TLMCOM_TLM_BTSEN_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case ITSEN_TELEM :
			telem = (int32_t)TLMCOM_TLM_ITSEN_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case XADDR1_TELEM :
			telem = (int32_t)TLMCOM_TLM_XADDR1_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case XADDR2_TELEM :
			telem = (int32_t)TLMCOM_TLM_XADDR2_LPF__GET();
			Telemetry_exponent = Telemetry_TEMP_EXP;
			read_exp = Telemetry_TEMP_EXP;
			break;
		case DUTY_TELEM:
			telem = (int32_t)TELEM_TLM_DUTY_FSW__GET((telem_idx_t)loop);  // u0.16
			telem = telem*100;  // convert to percent in u0.16 format
			Telemetry_exponent = Telemetry_DUTY_EXP;
			read_exp = TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)TELEMETRY_CONFIG_READ_DUTY_EXP));
			break;
		case FSW_TELEM :
			telem = (int32_t)TELEM_TLM_FREQ__GET((telem_idx_t)loop);  // u11.0 format 1khz lsb
			Telemetry_exponent = Telemetry_FSW_EXP;
			read_exp = LINEAR11_TO_EXPONENT(PMBUS_Get_Command_Word((PMBUS_PAGE_t)loop, PMBUS_CMDCODE_FREQUENCY_SWITCH));
			break;
		default:
			telem = 0;  // u11.0 format 1khz lsb
			read_exp = 0;
			Telemetry_exponent = 0;
			PMBUS_Log_Error(TELEMETRY, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
			break;
	}


	if (telem_type == VOUT_TELEM)
	{
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED((uint32_t)telem, Telemetry_exponent - read_exp); // convert vout fsw to Q=-16 (32 bit number)
		// see vout_target_setpoint_computations.xlsx for equations
		vout_scale = (PMBUS_Get_Command_Word(loop, PMBUS_CMDCODE_VOUT_SCALE_LOOP));  // get scale in Q=-16 (16bit number)
		if (vout_scale == 0) vout_scale = 1;  // avoid divide by zero
		telem_unsigned = DIVIDE_WITH_ROUNDING((uint32_t)telem_unsigned , ((uint32_t)DAC_STEP_SIZE_VOLTS_TO_0_156MV));  // convert vout_fsw to volts Q=-16
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED(telem_unsigned, -read_exp-3);  // shift back up to Q=-13 to divide
		telem_unsigned = DIVIDE_WITH_ROUNDING(telem_unsigned , vout_scale);  // divide by vout_scale to get scaled volts
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED(telem_unsigned, 3);  // back to Q=-16
	}

	else
	{
		telem = SHIFT_EXPONENT(telem, Telemetry_exponent - read_exp); // convert vout fsw to Q=-16 (32 bit number)
	}

	if ((uint32_t)telem_type < (uint32_t)ITSEN_TELEM) //Itsen, Duty and Freq do not have offset and gain
	{
		// grab the s-5.9 or format gain from telem config convert to int32
		// 1 + (gain_base[s4.4] << 5)[s4.9] + gain[s-5.9]
		/*lint -e701 */
		gain = 512 + (TWOS_UNCOMPLEMENT(8, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)((uint32_t)TELEMETRY_CONFIG_VOUT_GAIN_BASE + (8*(uint32_t)telem_type)))) << 5);
		gain += TWOS_UNCOMPLEMENT(4, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)((uint32_t)TELEMETRY_CONFIG_VOUT_GAIN + (4*(uint32_t)telem_type))));

		// offset in telem_type format already covert to int32
		offset = TWOS_UNCOMPLEMENT(8, Telemetry_Get_PMBUS_Configuration((PMBUS_PAGE_t)loop, (TELEMETRY_CONFIG)((uint32_t)TELEMETRY_CONFIG_VOUT_OFFSET + (8*(uint32_t)telem_type))));

		if (telem_type == VOUT_TELEM)
		{
			offset = SHIFT_EXPONENT(offset, PMBUS_Get_Vout_Mode_Exponent((PMBUS_PAGE_t)loop) - read_exp); // convert to READ_Exponent
		}
	}
	else {
		gain = SHIFT_EXPONENT(1, -Telemetry_GAIN_EXP);
		offset = 0;
	}
	// multiply the gain by the telem value and shift down by Telemetry_GAIN_EXP to
	// get read_exp Q format number post multiply
	if (telem_type == VOUT_TELEM)
	{
		telem_unsigned = telem_unsigned*(uint32_t)gain;  // multiply by gain ~512 in Q0 format
		telem_unsigned = SHIFT_EXPONENT_UNSIGNED(telem_unsigned, Telemetry_GAIN_EXP);  // shift down by 9
		telem = offset + (int32_t)telem_unsigned;  // add in the offset
		if (telem < 0) telem = 0;
	}
	else
	{
		telem = SHIFT_EXPONENT(telem*gain, Telemetry_GAIN_EXP);
		telem = offset + telem;  // add in the offset
		// if the value becomes negative and we don't support it we need to cap at 0
		if ((telem_type == IIN_TELEM) && (telem < 0))
		{
			telem = 0;
		}
	}

	/*lint -e845 */
	if ((telem_type ==  ATSEN_TELEM) || (telem_type == BTSEN_TELEM))
	{
		if (ntc_temp != NULL)
		{
			if (telem<0) telem = 0;
			if (telem>511) telem = 511;
			telem = ntc_temp[telem];
		}
		else
		{
			PMBUS_Log_Error(TELEMETRY, PROGRAM_COUNTER, PMBUS_STATUS_CML_PROCESSOR_FAULT);
		}
	}

	return telem;
}

#endif

#ifdef JIRA_783_PATCH_IN_SAME_PARTITION_OF_INVALIDATED_PATCH
/*lint -e{715}   Suppress: Symbol 'otp_end' not referenced */
void patch_patch_otp_cmd_patch_version_process(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint32_t otp_end)
{
    struct Patch_Image_s {
        /*lint -e754 cmd_hdr not referenced */
        Otp_File_System_Cmd_Hdr_t cmd_hdr;
        Patch_Cmd_Image_Version_t image_version;
    } *hdr;

    boolean command_is_invalid = boolean_false;
    TRY
        /* This command ignored due to checksum errors, but all following commands can still be processed. */
        /*lint -e(740) Unusual pointer cast (incompatible indirect types) */
        otp_fs_hdr_load(partition, otp_offset, (uint8_t** )&hdr, sizeof(Patch_Cmd_Image_Version_t));
    CATCH_OTHERS
        log_error(PATCHER, WARNING, &EXCEPTION);
        command_is_invalid = boolean_true;
    TRY_END

    /* If there was anything wrong with the command, skip it. */
    /*lint -e{774} Boolean within 'if' always evaluates to False -- not true*/ \
    if (command_is_invalid) return;

    /*
     * Version_base = 0: don't check version.
     *      This is useful for patches which do not call function from ROM,
     *      they are self-contained.
     * Otherwise check against the current version, it could be another versioned patch.
     */
    /*lint -e(644, 530) Suppress Variable 'hdr' may not have been initialized. That is not true!
     * Lint is just confused by the TRY block. */
    if ((hdr->image_version.version_base != version_timestamp) &&
            (hdr->image_version.version_base != 0UL)   )
    {
        /* patch is made for other firmware version. Therefore ignore it
         * and go to the next patch */
        LOG_ERROR_TYPE(UNKNOWN_SOURCE, WARNING, ILLEGAL_PARAMETER);
        return;
    }

    uint32_t hdr_ptr =  (uint32_t) &(hdr->cmd_hdr);
    /* verify the data checksum in RAM */
    if (!otp_fs_ram_crc_data_is_valid((Otp_File_System_Cmd_Hdr_t *) hdr_ptr,
            sizeof(Patch_Cmd_Image_Version_t)))
    {
        /* invalid checksum, skip this command */
        LOG_ERROR_TYPE(UNKNOWN_SOURCE, WARNING, CORRUPT_MEMORY);
        return;
    }
    /* ------------------------------------------------------------------ */
    /*
     * Version_new = 0: patch has no explicit version.
     *      This is useful for patches which do not call function from another patch,
     *      just from the ROM.
     * Otherwise update the current firmware version UTC timestamp based on the applied
     * patch. */
    if (hdr->image_version.version_new != 0)
    {
        version_timestamp = hdr->image_version.version_new;
    }
    /* ------------------------------------------------------------------ */
    /* remap the patch always to the the beginning of the partition
     */

    if ((hdr->cmd_hdr.option & OTP_FS_PATCH_ABSOLUTE) == 0u)
        patch_otp_remap(partition, &(hdr->cmd_hdr));

    // JIRA 783: After patch_otp_remap is called the pointer "hdr" should be reloaded from an offset of 0 before use:
    TRY
         /* This command ignored due to checksum errors, but all following commands can still be processed. */
         /*lint -e(740) Unusual pointer cast (incompatible indirect types) */
         otp_fs_hdr_load(partition, 0, (uint8_t** )&hdr, sizeof(Patch_Cmd_Image_Version_t));
     CATCH_OTHERS
         log_error(PATCHER, WARNING, &EXCEPTION);
         command_is_invalid = boolean_true;
     TRY_END

    /* ------------------------------------------------------------------ */
    /* The patch command might come with a patch entry routine
     * read the function callback and execute */
    void (*fkt_ptr)(void);
    fkt_ptr = (void (*)(void)) hdr->image_version.execute;

    if ((fkt_ptr != NULL)&&(!command_is_invalid))
    {
        fkt_ptr();
    }
    /* ------------------------------------------------------------------ */
}
#endif


#ifdef JIRA_785_REMAP_CLEAR_NEEDED_ON_RESET
/**
 * This is an example of how to add new firmware commands to MFR_FIRMWARE_COMMAND PMBus command
 * @param page Selects the PMBus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */
void patch_PMBUS_HANDLE_MFR_FIRMWARE_COMMAND(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	uint8_t command;
	if (direction == PMBUS_WRITE)
	{
		// get the command code here:
		command = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_FIRMWARE_COMMAND);

		if (command == (uint8_t)PMBUS_MFR_FIRMWARE_COMMAND_SYSTEM_RESET)
		{
			// see SHASTA-457
			SCU_CPUS_CFG__SET_REMAP__SET(0UL);
		}

	}

	PMBUS_HANDLE_MFR_FIRMWARE_COMMAND(page, direction); // call the ROM handle
}
#endif

#ifdef JIRA_973_I2C_FIFO
void pmbus_driver_init(void)
{
	PMBUS_CNFG__SET(  PMBUS_CNFG__WDT_SCL_LOW_MAX__Shift(2930u)	// bus timeout on SCL LOW PH ACCUMULATION max, 40ns*256=10240ns per lsb, set to 30ms, spec 25-35ms
					| PMBUS_CNFG__WDT_SCL_STRETCH__Shift(0x1ffu)  // clock stretching timeout of 5us
					| PMBUS_CNFG__DEBOUNCE_LENGTH__Shift(1)
					| PMBUS_CNFG__DS_FSM_FSM_CLK_G__Msk
					| PMBUS_CNFG__EN_PHY__Msk					// enable the phy
					| PMBUS_CNFG__EN_AUTO_TRANSMIT_TX_PEC__Msk  // auto transmit pec when tx count equals current byte count
					| PMBUS_CNFG__PREEMPTIVE__Msk				// to use LUT table for ack/nack
					);
}
#endif

#ifdef JIRA_974_RESTORE_USER_CODE
void patch_PMBUS_HANDLE_RESTORE_USER_CODE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if (direction == PMBUS_WRITE)
	{
		uint8_t cmd = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_RESTORE_USER_CODE);
		PMBUS_Restore_Code(page, cmd);
		// Shasta-974: call handlers on restore code calls
		PMBUS_CMD_HANDLER(page, (uint8_t)cmd, OTP_RESTORE);
		PMBUS_CMD_HANDLER(page, (uint8_t)cmd, PMBUS_WRITE);
	}
}
#endif

