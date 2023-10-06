/*
 * pmbus_mfr_specific_handlers.c
 *
 *  Created on: Jan 24, 2017
 *      Author: dlewis4
 */

#include "fw_config.h"           /* Essential fw-wide configuration for e.g. buffer size #defines */
#include "environment.h"         /* STATIC, */
#include <string.h>				 /* memcpy */
#include "fw_bool.h"             /* boolean type */
#include "otp_fs.h"             /* Save/get user parameter to/from OTP */
#include "otp_drv.h"             /* OTP initialize */
#include "configurator.h"
#include "configuration_structure.h"         /* trim and config structure data */
#include "pmbus_configuration_data.h"        /* pmbus default config data */
#include "crc32.h"              /* Misused for "encrypting" an user parameter */

#include "pmbus_mfr_specific_handlers.h"
#include "pmbus_api.h"
#include "regulation_drv.h"
#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "driver_common.h"
#include "log_app.h"


void pmbus_mfr_init (void)
{

}


uint32_t output_data = 0;

/**Handler to read the OTP Section CRC
 * PMBus command write "0", return Read out the sum of CRC(Header and Data)in Partition 0 except Partial Data type CRC
 * If write data not 0, return Read result of header CRC + Data CRC, of select type
 * [0:7]: file system image type see Otp_File_System_Cmd_Types_t
 * [8:15]: xvalent number 0-15
 * [16:23]: reserved
 * [24:31]: otp partition see Otp_Partition_Selector_t for details should be number between 0-17
 * @param page   		  Loop selection
 * @param direction 	  selected file system command
 *
 */
void PMBUS_HANDLE_MFR_OTP_CRC(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
#ifdef READ_OTP_CRC
	uint32_t input_data = PMBUS_Get_Command_Dword(page, PMBUS_CMDCODE_MFR_OTP_CRC);
	uint32_t temp_crc;

	// Now add new code here:
	if (direction == PMBUS_WRITE)
	{
		Otp_Partition_Selector_t partition = (Otp_Partition_Selector_t)(input_data >> 24);
		uint32_t xv = (input_data >> 8) & 0xFF;
		Otp_File_System_Cmd_Types_t cmd_type = (Otp_File_System_Cmd_Types_t)(input_data & 0xFF);

		if ((cmd_type == OTP_FS_CMD_CONFIG_DATA) ||
			((cmd_type >= OTP_FS_CMD_PMBUS_DEFAULT_LP0) && (cmd_type <= OTP_FS_CMD_PMBUS_PARTIAL))||
			(cmd_type == OTP_FS_PATCH_IMAGE_VERSION))
		{
			output_data = configurator_get_section_crc32_xv(partition, cmd_type, (uint8_t)xv);
		}
		else if(cmd_type == 0)
		{
			output_data = 0;
			for (uint8_t j = OTP_FS_CMD_CONFIG_DATA; j < OTP_FS_CMD_CONF_PARTIAL; j++)
			{
				if (j != OTP_FS_CMD_PMBUS_STRUCTURE )
				{
					for (uint8_t i = 0; i < 16; i++)
					{
						temp_crc = configurator_get_section_crc32_xv(partition, j, (uint8_t)i);
						if (temp_crc != -1)
						{
							output_data += temp_crc;
						}
					}
				}
			}
		}
		PMBUS_Inter_Thread_Set_Command(page, PMBUS_CMDCODE_MFR_OTP_CRC, PMBUS_READ);
	}
	else if (direction == PMBUS_READ)
	{
		PMBUS_Set_Command_Dword(page, PMBUS_CMDCODE_MFR_OTP_CRC, output_data);
	}
#endif
}
#ifdef READ_OTP_CRC
/** Get the crc32 value based on cmd_type and xv number for a specific OTP image. For non-partial files, return their data CRC.
 * For partial files, return the CRC of all matching partials header CRC and data CRC because their order matters.
 * @param partition   partition selected
 * @param cmd_type 	  selected file system command
 * @param xv		  selected xvalent number
 * @return 			  the crc32 value of this section
 */
uint32_t configurator_get_section_crc32_xv(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Types_t cmd_type, uint8_t xv)
{

    uint32_t crc32 = 0;
    Otp_File_System_Cmd_Hdr_t* config_ptr = NULL;
    Otp_File_System_Cmd_Callback_t crc_callback = NULL;
    uint32_t is_partial_cmd = 0;
    /*lint -e{838} Previously assigned value to variable 'dummy' has not been used -- Unclear*/
    Otp_File_System_Cmd_Types_t config_type = {(Otp_File_System_Cmd_Types_t) ((uint8_t)cmd_type +  (xv << 8))};
    Otp_File_System_Cmd_Handler_t config_data_handler =
            /* cmd_type | option | cmd_hdr_size | cmd_size_min | handler  */
            /*-------------------------------------------------------*/
        {config_type,      0,        0,             16,         crc_callback};

    uint8_t error = 0;
    do {
        //get pointer information
    	otp_fs_scan(partition, &config_data_handler, &config_ptr);
        if (config_ptr != NULL)
        {
            uint16_t sub_hdr_size = (cmd_type == OTP_FS_PATCH_IMAGE_VERSION)?sizeof(Patch_Cmd_Image_Version_t):0;
            THROW_IF(!otp_fs_ram_crc_hdr_is_valid((Otp_File_System_Cmd_Hdr_t*) config_ptr, sub_hdr_size), CORRUPT_MEMORY);
            THROW_IF(!otp_fs_ram_crc_data_is_valid((Otp_File_System_Cmd_Hdr_t*) config_ptr, sub_hdr_size), CORRUPT_MEMORY);

            /*lint -e(826) Suspicious pointer-to-pointer conversion (area too small) */
            crc32 = config_ptr->crc_hdr;
            uint32_t * data_crc = (uint32_t*) ((uint8_t*)config_ptr + ((config_ptr->size - sizeof(crc_data_t))));//get data_crc pointer
            crc32 = crc32 + *data_crc;
        }
    }
    /*lint -e(717) do ... while(0) */
    while(0);
    if(error != 0)
    {
    	LOG_ERROR_TYPE(OTP_FS, WARNING, CORRUPT_MEMORY);
        crc32 = (uint32_t)-1;
    }
	return crc32;
}
#endif
