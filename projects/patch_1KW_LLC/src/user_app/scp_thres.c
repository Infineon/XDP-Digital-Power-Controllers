/*
 * scp_thres.c
 *
 *  Created on: 30 Sep 2020
 *      Author: halimand
 */


#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "regulation_state_machine_callbacks.h"

#include "scp_thres.h"


void PMBUS_HANDLE_MFR_STARTUP_SCP_THRESH(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction){
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		// Get and store Startup SCP threshold
		user_data.startup_scp_thres = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_STARTUP_SCP_THRESH);

		// Get and store Steady State SCP threshold
//		user_data.steady_scp_thres = ISEN_ISP_FAULT1__ISP_SCP_THRESH__GET(page);


//		PMBUS_Set_Command_Byte(page, PMBUS_CMDCODE_MFR_SCP_DBG, user_data.startup_scp_thres);
	}
}

void PMBUS_HANDLE_MFR_STEADY_SCP_THRESH(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction){
	if ((direction == PMBUS_WRITE)||(direction == OTP_RESTORE))
	{
		// Get and store Startup SCP threshold
		user_data.steady_scp_thres = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_STEADY_SCP_THRESH);

		// Get and store Steady State SCP threshold
//		user_data.steady_scp_thres = ISEN_ISP_FAULT1__ISP_SCP_THRESH__GET(page);


//		PMBUS_Set_Command_Byte(page, PMBUS_CMDCODE_MFR_SCP_DBG, user_data.steady_scp_thres);
	}
}
