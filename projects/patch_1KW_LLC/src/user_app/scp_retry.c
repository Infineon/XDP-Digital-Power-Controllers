/*
 * scp_retry.c
 *
 *  Created on: 23 Sep 2020
 *      Author: halimand
 */



#include "pmbus_autogen.h"
#include "pmbus_mfr_autogen.h"
#include "faults_drv.h"
#include "regulation_state_machine_callbacks.h"
#include "shasta_hal_isen.h"
#include "shasta_hal_fault_common.h"

#include "scp_retry.h"

// SCP_RETRY patch
void patch_COMMON_FAULT_HANDLE(FAULT_TYPE_t fault_code)
{

	boolean fault_pin_mask;

   /*
	 * PROCESS COMMON LOOP
	 * Read Fault Status
	 * Read Shutdown Masks
	 * Update regulation thread due to shutdown event
	 * Report fault on Pmbus
	 * Clear Fault
	 */

	/*Read which fault is set to shutdown*/

	//FAULT_DRV_App.fault_shut_mask_common = COMMON_FAULT_SHUT_MASK_COM__GET();

	/* Check which event happened:
	* sht_evt_stat_common  - shutdown  fault_status[x] = 1 and shutdown_mask[x] = 1
	*/

    FAULTS_REPORT_PMBUS(PMBUS_PAGE_0, FAULT_TYPE_COMMON_SHUTDOWN);
    FAULTS_REPORT_PMBUS(PMBUS_PAGE_1, FAULT_TYPE_COMMON_SHUTDOWN);
    //Set fault pin if not masked by pmbus
	fault_pin_mask = (boolean)(((~FAULTS_Get_PMBUS_Configuration(PMBUS_PAGE_0,FAULTS_CONFIG_FAULT_PIN_MASK_FW)) >> ((uint32_t)FAULT_TYPE_COMMON_SHUTDOWN-32))&1);
	PMBUS_Gpio_Set_Feature((PMBUS_PAGE_t)PMBUS_PAGE_0, PMBUS_GPIO_CONFIG_FAULT_SELECT, fault_pin_mask);  // set fault pin for given loop
	fault_pin_mask = (boolean)(((~FAULTS_Get_PMBUS_Configuration(PMBUS_PAGE_1,FAULTS_CONFIG_FAULT_PIN_MASK_FW)) >> ((uint32_t)FAULT_TYPE_COMMON_SHUTDOWN-32))&1);
	PMBUS_Gpio_Set_Feature((PMBUS_PAGE_t)PMBUS_PAGE_1, PMBUS_GPIO_CONFIG_FAULT_SELECT, fault_pin_mask);  // set fault pin for given loop

	//PMBUS_COMMAND_OR_BYTE(PMBUS_PAGE_WB_BROADCAST, PMBUS_CMDCODE_STATUS_MFR_SPECIFIC, (1u<<PMBUS_STATUS_MFR_SPECIFIC_4));
	//PMBUS_Inter_Thread_Set_Command(PMBUS_PAGE_WB_BROADCAST, PMBUS_CMDCODE_STATUS_MFR_SPECIFIC, PMBUS_READ);
	//PMBUS_Inter_Thread_Set_Command(PMBUS_PAGE_WB_BROADCAST, PMBUS_CMDCODE_STATUS_WORD, PMBUS_READ);
	//save status for reporting
	FAULT_COMMON_App.fault_status |= (1u<<(uint32_t)fault_code);
	uint32_t sht_evt_stat_common = (1u<<(uint32_t)fault_code) & FAULTCOM_FAULT_SHUT_MASK_COM__GET();

	//All Common faults are FAULT_TYPE_COMMON_IGNORE or FAULT_TYPE_COMMON_SHUTDOWN
	if (sht_evt_stat_common) {
		FAULT_COMMON_App.fault_status_event_latch |= sht_evt_stat_common;
		/* update regulation thread due to common event */
		REGULATION_INTER_THREAD_SET(PMBUS_PAGE_1, REGULATION_CONTROLS_FAULT, ((uint16_t)FAULT_TYPE_COMMON_SHUTDOWN & 0xff));
		REGULATION_INTER_THREAD_SET(PMBUS_PAGE_1, REGULATION_CONTROLS_FAULT, ((uint16_t)FAULT_TYPE_COMMON_SHUTDOWN & 0xff));
		REGULATION_INTER_THREAD_SET(PMBUS_PAGE_0, REGULATION_CONTROLS_FAULT, ((uint16_t)FAULT_TYPE_COMMON_SHUTDOWN & 0xff));
		REGULATION_INTER_THREAD_SET(PMBUS_PAGE_0, REGULATION_CONTROLS_FAULT, ((uint16_t)FAULT_TYPE_COMMON_SHUTDOWN & 0xff));

        if (fault_code == SCP_COMMON_FAULT_BIT)
        {
        	Set_Firmware_Fault(PMBUS_PAGE_0, FAULT_TYPE_SCP);
        }

	}

	//FAULTCOM_FAULT_POLARITY_COM__SET(FAULTCOM_FAULT_POLARITY_COM__GET() ^ FAULT_DRV_App.fault_status_common) ;
	/* clear faults reported by the status */
	FAULTCOM_FAULT_ENABLE_COM__SET(FAULTCOM_FAULT_ENABLE_COM__GET() & ~(1u<<(uint32_t)fault_code));
	FAULTCOM_FAULT_CLEAR_COM__SET((1u<<(uint32_t)fault_code));
	/*Save Current Shutdown status*/

  /*
   * END OF PROCESS COMMON LOOP.
   */

}

void PMBUS_HANDLE_MFR_SCP_FAULT_RESPONSE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
       if ((direction == PMBUS_WRITE)  || (direction == OTP_RESTORE))
       {
		   uint8_t fault_response_cmd_byte = PMBUS_Get_Command_Byte(page, PMBUS_CMDCODE_MFR_SCP_FAULT_RESPONSE);
		   FAULTS_SETUP_FAULT(page, FAULT_TYPE_SCP, fault_response_cmd_byte,
						 FAULTS_CONFIG_VOUT_DELAY_UNIT, VOLTAGE_RESPONSE_TYPE,PMBUS_CMDCODE_STATUS_MFR_SPECIFIC, (uint8_t)PMBUS_STATUS_MFR_SPECIFIC_4, (FAULT_TYPE_t)0);

		   /////////Response Type////////
		   FAULT_RESPONSE_TYPE_t fault_response_type = FAULTS_Get_Fault_Response_Type(VOLTAGE_RESPONSE_TYPE,fault_response_cmd_byte);

		   if  (fault_response_type == FAULT_RESPONSE_RETRY_SHUTDOWN)
		   {
				  FAULTCOM_FAULT_SHUT_MASK_COM__SET((FAULTCOM_FAULT_SHUT_MASK_COM__GET()  | (1<<SCP_COMMON_FAULT_BIT)));
		   }
		   else
		   {
				  FAULTCOM_FAULT_SHUT_MASK_COM__SET((FAULTCOM_FAULT_SHUT_MASK_COM__GET() & ~(1<<SCP_COMMON_FAULT_BIT)));
		   }
       }
}




