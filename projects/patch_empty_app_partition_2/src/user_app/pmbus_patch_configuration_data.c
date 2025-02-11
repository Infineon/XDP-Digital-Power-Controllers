/*
 * pmbus_patch_configuration_data.c
 *
 *  Created on: Aug 30, 2017 with python
 *      Author: ashendar
 */

#include "driver_common.h"       // scratchpad memory
/* Standard library */
#include <stdint.h>             /* uint*_t */
#include <string.h>             /* memset */
/* APIs */
#include "pmbus_cmd_handlers.h"
#include "pmbus_api.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "regulation_drv.h"
#include "pmbus_autogen.h"
#include "system_shasta.h"  // sys_reset function
#include "otp_drv.h"             /* OTP initialize */
#include "otp_fs.h"             /* Save/get user parameter to/from OTP */
#include "pmbus_patch_configuration_data.h"

#ifdef Patch_Pmbus_Table
/** trim_struct_data contains the structural information for trim data */
const uint8_t patch_pmbus_default_data_array_loop0[] = { 
        		
		6,
		0,
		3,
		0,
		192,
		1,
		192,
		1,
		169,
		199,
		171,
		238,
		249,
		1,
		0,
		0,
		0,
		31,
		0,
		208,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		24,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		16,
		232,
		0,
		0,
		255,
		255,
		0,
		0,
		128,
		241,
		125,
		8,
		3,
		140,
		240,
		0,
		240,
		0,
		0,
		0,
		0,
		255,
		255,
		0,
		0,
		5,
		0,
		0,
		0,
		0,
		0,
		255,
		0,
		0,
		0,
		0,
		254,
		0,
		128,
		7,
		0,
		125,
		0,
		0,
		90,
		0,
		236,
		15,
		231,
		15,
		0,
		170,
		248,
		0,
		160,
		248,
		144,
		240,
		136,
		240,
		0,
		255,
		240,
		0,
		1,
		240,
		0,
		8,
		0,
		5,
		0,
		0,
		80,
		240,
		252,
		243,
		0,
		0,
		0,
		40,
		240,
		255,
		240,
		255,
		19,
		255,
		19,
		51,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		16,
		0,
		64,
		0,
		0,
		136,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		255,
		179,
		255,
		179,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		255,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		16,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		71,
		27,
		182,
		136,

        };
        
/** conf_struct_data contains the structural information for conf data */
const uint8_t patch_pmbus_default_data_array_loop1[] = { 
        		
		8,
		0,
		3,
		0,
		192,
		1,
		192,
		1,
		155,
		213,
		33,
		251,
		249,
		1,
		0,
		0,
		0,
		31,
		0,
		208,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		24,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		16,
		232,
		0,
		0,
		255,
		255,
		0,
		0,
		128,
		241,
		125,
		8,
		3,
		140,
		240,
		0,
		240,
		0,
		0,
		0,
		0,
		255,
		255,
		0,
		0,
		5,
		0,
		0,
		0,
		0,
		0,
		255,
		0,
		0,
		0,
		0,
		254,
		0,
		128,
		7,
		0,
		125,
		0,
		0,
		90,
		0,
		236,
		15,
		231,
		15,
		0,
		170,
		248,
		0,
		160,
		248,
		144,
		240,
		136,
		240,
		0,
		255,
		240,
		0,
		1,
		240,
		0,
		8,
		0,
		5,
		0,
		0,
		80,
		240,
		252,
		243,
		0,
		0,
		0,
		40,
		240,
		255,
		240,
		255,
		19,
		255,
		19,
		51,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		16,
		0,
		64,
		0,
		0,
		136,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		255,
		179,
		255,
		179,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		255,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		16,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		71,
		27,
		182,
		136,

        };
        
/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */

void patch_PMBUS_HANDLE_RESTORE_DEFAULT_ALL(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if (direction == PMBUS_WRITE)
	{
		uint8_t * p;
		uint32_t size;
		uint32_t size_check;
		Otp_File_System_Cmd_Hdr_t* config_ptr = NULL;
		// first grab the trim and config restore results:
		CONFIGURATOR_ERROR_t error = configurator_error_get();
		if (error != CONFIGURATOR_NO_FAULT)
		{
			PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_OTHER_MEM_FAULT);  // we use bit0 other mem fault for trim and config issues
			configurator_error_clear();  // clear the config error
			configurator_pmbus_default_set((uint8_t)page);  // try to see if we have a default pmbus image, load this regardless of trim/config status
		}
		p = configurator_pmbus_default_ptr_get((uint8_t)page);

		size_check = ((uint32_t)p[3]<<24U) |  ((uint32_t)p[2]<<16U) | ((uint32_t)p[1]<<8U) | (uint32_t)p[0];
		// grab expected size plus command count from command descriptors
		size = PMBUS_Calculate_Size_Check();
		if (size != size_check)
		{
			// point all the loops to loop0 since the default data is the same for all loops
			 config_ptr = (Otp_File_System_Cmd_Hdr_t*)patch_pmbus_default_data_array_loop0;
			 p = (uint8_t *) config_ptr + sizeof(Otp_File_System_Cmd_Hdr_t);
		}
		PMBUS_Restore_All(page, p);
	}
}
#endif

        

