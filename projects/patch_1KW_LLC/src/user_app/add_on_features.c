/*
 * ADD_ON_FEATURES.c
 *
 *  Created on: Mar 7, 2018
 *      Author: ashendar
 */

#include "telemetry_api.h"
#include "regulation_api.h"
#include "regulation_drv.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "shasta_hal_tsen.h"
#include "shasta_hal_pwm.h"
#include "shasta_hal_pid.h"
#include "shasta_hal_analog.h"
#include "shasta_hal_common.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_isen.h"
#include "shasta_hal_fault_common.h"
#include "shasta_hal_fault.h"
#include "shasta_hal_vcontrol.h"
#include "shasta_hal_tstctrl.h"  	// ate test state
#include "log_app.h"             /* Logging of errors */
/* Standard library */

#include <string.h>             /* memset */
#include "patch_rom_table.h"
#include "string.h"
#include "add_on_features.h"
#include "mmu_drv.h"                /* MMU_Initialization */
#include "driver_common.h"
#include "pmbus_mfr_autogen.h"



/*lint -e522 */
void add_on_features_init (void)
{

}

