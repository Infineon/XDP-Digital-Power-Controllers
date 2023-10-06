/* ============================================================================
** Copyright (C) 2015 Infineon Technologies AG
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
 * @file    __main.c
 * @brief   Contains the initialisation of all software parts. It is assumed that the hardware
 *          part is already initialised when calling functions within this file.
 *          The essential part of this file is the definition of an own __main symbol to suppress
 *          the linking of tool specific libraries which create a bare metal C environment.
 *          This allows full control over the executable image but one has to consider that the
 *          RAM initialisation including decompression (if not deactivated) and other desired
 *          functionality like floating point support must be re-implemented.
 */

/* Compiler configurations first */
#include "fw_config.h"          // Essential fw-wide configuration for e.g. buffer size #defines
#include "patch_run.h"          /* checking for function patch points */
#include "environment.h"        // STATIC, INLINE, FOREVER, PROGRAM_COUNTER
#include "linker_sections.h"    // Linker specific memory sections

/* Standard library */
#include <string.h>     // memcpy

/* APIs */
#include "system_shasta.h"          /* Accessing global data, reset and power down */
#include "cmsis_os_ext.h"           /* CMSIS-RTOS functions, incl. exception handling */
#include "sys_boot_descr_format.h"  /* boot descriptor bitfield definitions */
#include "shasta_hal_cgu.h"    /* Access to HAL: Clock Generation Unit */
#include "shasta_hal_rgu.h"    /* Access to HAL: Reset Generation Unit registers */
#include "shasta_hal_scu.h"    /* Keeping track of the boot state */
#include "log_app.h"                /* Module initialization */
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

/** drivers*/
#include "configurator.h"           /* Configuration module */
#include "fw_params.h"				/* Firmware Parameters */
#include "pmbus_drv.h"              /* Module initialization */
#include "faults_drv.h"             /* Module initialization */
#include "telemetry_drv.h"          /* Module initialization */
#include "regulation_drv.h"         /* Module initialization */
#include "gpio_drv.h"              	/* Module initialization */
#include "patch_app.h"              /* Module initialization */
#include "mmu_drv.h"                /* MMU_Initialization */
#include "driver_common.h"          /* fw_patch_ptr type, fw_patch_p instance */
#include "shasta_hal_tstctrl.h"  	// ate test state
//#include "user_app.h"
#include "telemetry_api.h"
#include "version.h"

#if IMAGE_TYPE == ROM_IMAGE
#include "wdt_drv.h"
#endif


/*lint -esym(552, sys_last_critical_exception) -esym(765, sys_last_critical_exception) : Could be made static
 * Exported for debugging purposes. */
/** Last exception with a #CRITICAL severity level. Only used for #EMERGENCY handling. */
RESET_RESISTANT_DECLARE(osException_t, sys_last_critical_exception);

/* ============================================================================
 * Functions (public and private in a monolithic hierarchy)
 * ========================================================================= */

/* ----------------------------------------------------------------------------
 * Forward declaration and documentation of private functions
 * ------------------------------------------------------------------------- */

/**
Initializes the memory for the C runtime (CRT) environment (see @ref sctShasta_Boot for more details).<br>
Depending on the image type (see image_types.h), different parts of the memory are affected.
For all image types, the function
- Clears zero-initialized data in RAM (.bss section; remember that RAM has random values after a POR).
- Copies non-zero data from its linker load section to the corresponding .data section.
- Switches the memory map of Shasta to use a vector table in RAM (not ROM). This allows patching of interrupt service routines.

For a #ROM_IMAGE, the vector table in ROM is additionally copied to RAM.

As outlined in @ref sctShasta_Boot, the CRT code needs to
- Zero-initialize all C variables that are declared as uninitialized (e.g. "uint16_t my_var;"). The C standard mandates them to be zeroed before main() starts to execute. Technically, this is done by means of a call to memset() in which the linker generates the start address and the length of the region that needs to be zeroed.
- Initialize all C variables that are initialized to some non-zero value (e.g. "uint16_t my_var = 123;"). For a RAM image, this initialization is simple: the elf file that is loaded into RAM also writes the relevant initial data to the corresponding physical address where the variables resides.
  For a ROM image, this needs to be done by copying the respective values from a table in ROM to the corresponding locations in RAM. Having the data in ROM is not sufficient because such data is not declared `const`.
 */
STATIC void sys_mem_init(void);

/**
This routine performs the memset operation on the .bss segment. The standard lib contains a memset function, which performs a single 32-bit write per loop iteration. The performance of such memset strategy is very bad and leads to a long Shasta CPUs boot-up phase.

@note: memset requires 56 us on Shasta to clear 2900 bytes.

 */
STATIC void sys_memset_bss(uint8_t *start, uint32_t size);

/**
 Central point to initialize all firmware modules and their corresponding RTOS threads.
 First, the patch table is checked to see if any module functions need to be patched.
 Afterwards there are no constraints on which order the modules should be initialized.
 *
 * @throws OUT_OF_RESOURCE from patch_otp_process, pmbus_init, patch_init,
 *         dcdc_config_init, svid_init
 * @throws ILLEGAL_PARAMETER from dcdc_config_init, patch_otp_process.
 * @throws TIMEOUT from dcdc_config_init, patch_otp_process.
 */
STATIC void sys_modules_init(void);

/* ------------------------------------------------------------------------- */

/**
This is the top-level function for initializing the software part of the CPU subsystem (CPUS). It implements the setup of the C runtime (CRT) environment, which is required for each module initialization.
Moreover, it configures and starts the RTOS and all modules.<br>
Steps in the __main() function:
- Initialize the memory.
- If required, implement a full patch and reset the Shasta controller. Otherwise, proceed with the boot sequence.
- Initialize the RTOS.
- Initialize all modules.
- Start the RTOS (which invokes all module threads).
 
@note __main() overrides code from RTX_CM_lib.h, namely the routines _main_init() and __rt_entry(). In other words, __main() deliberately replaces any CRT code from RTX or the ARM tool chain. This is done in order to have full control over the boot sequence of the FW because one requirement is to be able to control and understand the duration of the boot sequence.
 */
/*lint -esym(714, __main)  Suppress: Symbol '__main(void)' not referenced */
/*lint -e(957)  Suppress: Function '__main' defined without a prototype in scope.
 * We do not use a header file __main.h since __main is called from assembler. */
/*lint -esym(765, __main)  Suppress: external '__main(void)' could be made static.
 * No, its called from startup_Shasta. */
void __main(void)
{
    /* The first thing to do is to provide a stable C runtime environment -> initialize the RAM
     * content except for those sections that are reset resistant (neither .bss nor .data). */
    sys_mem_init();
    SYS_BOOT_STATE_TRACK(MEM_INIT);

    osException_t last_exception = {NO_ERROR, NULL};
    TRY
    {
        /* In case of booting from power on (also known as "cold" reset) nothing was initialized
         * before. So we also have to initialize all reset resistant memory and perform the
         * full patching again. */
        if (RGU_RSTSR__GET() == 0x0)
        {
            /* Our little world is in order now. */
            sys_num_critical_reboots = 0;
            sys_last_critical_exception.type = NO_ERROR;
            sys_last_critical_exception.program_counter = 0;
/* Don't turn on wdt if it is not ROM. It is pain in the butt to debug with it on */
            wdt_last_stackframe = (Cortex_M0_Stack_Frame_t){ 0 };
        }
        else if (RGU_RSTSR__GET() & RGU_RSTSR__WDTRST__Msk)
        {
            /* If the watchdog timer issued the reset we log the exception with the information
             * from the stack frame that was saved in the WatchDog_IRQHandler and let an emergency
             * shutdown draw nearer. */
            osException_t exception = {SYSTEM_FAULT, wdt_last_stackframe.pc};
            log_error(UNKNOWN_SOURCE, CRITICAL, &exception);

            /* Let an emergency shutdown draw nearer and retry a stable system execution. */
            sys_num_critical_reboots++;
        }
        else
        {
            /* All other resets don't have a special handling. */
        }

        /* Check if the maximum number of critical reboots is exceeded.
         * Note: This has to be done after the counter has been initialized for the first time. */
        if (SYS_BD_BOOT_FAULT & sys_boot_descriptor)
        {
            THROW_IF(sys_num_critical_reboots > SYS_MAX_CRITICAL_REBOOTS, SYSTEM_FAULT);
        }

        /* Initialize the CMSIS-RTOS. */
        osStatus status = osExtKernelInitialize();
        THROW_IF(status == osErrorNoMemory, OUT_OF_RESOURCE);
        SYS_BOOT_STATE_TRACK(RTOS_INIT);

        /* Initialize all firmware modules which rely on the RTOS. Note: Their threads will
         * not be started before osKernelStart() is called. */
        sys_modules_init();
        SYS_BOOT_STATE_TRACK(MODULES_INIT);
    }
    CATCH(SYSTEM_FAULT)
        /* Too many critical issues have happened. */

        /* Log every exception, always. */
        log_error(BOOT_SEQUENCE, EMERGENCY, &EXCEPTION);
        SYS_BOOT_STATE_TRACK(BOOT_FAILED);

        /* Multiple resets did not bring the system back into a stable state. Power it down now. */
        sys_power_down();
     /*lint -e(838) Suppress: Previously assigned value to variable 'uncaught_exception' has not been used. */
     CATCH_OTHERS
        /* Applying a full patch failed
         * or
         * the memory needed for the module's thread or mailbox exceeds the reserved OS memory,
         * or the PMBus command table is too small for adding the module's commands.
         * In case there are more exceptions without an specific handler treat them the same way. */

        /* Do the handling after osKernelStart(). See explanation below. */
        last_exception = EXCEPTION;
    TRY_END

    /* Hand over the control to the RTOS. osKernelStart() does not return in case of success.
     * Note: The kernel needs to be started after the TRY_END since otherwise the exception guard
     * which is reused at ISR-level will not be cleaned up. */
    if (last_exception.type == NO_ERROR)
    {
        (void)osKernelStart();

        /* In case it does return the Cortex M0 CONTROL register has indicated an invalid state
         * for starting the OS. */
        LOG_ERROR_TYPE(BOOT_SEQUENCE, CRITICAL, ILLEGAL_PARAMETER);
    }
    else
    {
        /* Handling for CATCH_OTHERS which is different from the one for osKernelStart(). */
        /* Log every exception, always. */
        log_error(BOOT_SEQUENCE, CRITICAL, &last_exception);
    }

    /* Proceed with the exception handling which is common for CATCH_OTHERS and osKernelStart():
     * Let an emergency shutdown draw nearer and retry a stable system execution. */
    SYS_BOOT_STATE_TRACK(BOOT_FAILED);
    sys_num_critical_reboots++;
    // see SHASTA-457
    SCU_CPUS_CFG__SET_REMAP__SET(0UL);
    sys_reset();
}


STATIC void sys_mem_init(void)
{
    /* Note: The handling of .bss and .data follows the code from __low_level_init() as found in
     * RTX example code. */

    /* Initialise the section .bss
     * .bss is the section that holds non-initialised global data,
     * therefore they are initialised to 0. */
    sys_memset_bss(SECTION_BASE_ZI, SECTION_LENGTH_ZI);

    /* Initialise the section .data
     * .data is the section that holds initialised global data.
     * Note that for RAM_IMAGE this seems somewhat stupid: Why copy non-zero data
     * from some RAM location 'a' to 'b' instead of having the elf loader directly
     * copy it to 'b'? The reason is simple: In case you want to restart the RAM image
     * (without re-downloading it!), someone needs to make sure that .data is
     * reinitialised ... and this is why this memcpy() is necessary.
     * Zj: or maybe not, let's try*/

    memcpy(SECTION_BASE_RW_DESTINATION, SECTION_BASE_RW_SOURCE, SECTION_LENGTH_RW);

#if IMAGE_TYPE == ROM_IMAGE
    /* Mirror the vector table from ROM to RAM.
     * This initializes the RAM part of the "VECTOR_RAM"- section. */
    memcpy(SECTION_BASE_VECTORS_RAM, SECTION_BASE_VECTORS_ROM, SECTION_LENGTH_VECTORS_ROM);
#endif

    /* Set the remapping bit in order to use the vector table in RAM instead of ROM. */
    SCU_CPUS_CFG__SET_REMAP__SET(1UL);
}

STATIC void sys_memset_bss(uint8_t *start, uint32_t size)
{
    uint32_t *start32;
    uint32_t len_align = ((uint32_t)start) & 0x3u;

    if (len_align > 0)
    {
        /* the start address is not 32-Bit aligned */
        /* perform 8_bit access till they are 32-Bit aligned */
        if (len_align < size)
        {
            len_align = size;
        }
        for (uint32_t i = 0; i < len_align; i++)
        {
            *start++ = 0;
        }
        size -= len_align;
    }

    /* now memset in a big loop (16 x 32-Bit in one loop iteration) */

    /*lint -e{826}  Suppress once: Suspicious pointer-to-pointer conversion (area too small).
     * The pointer "start" will be always aligned to 32-bit. */
    /*lint --e{661} */
    /*lint --e{662} */
    start32 = (uint32_t*)start;
    while (size > (16 * sizeof(uint32_t)))
    {
        *start32++ = 0;  // #1
        *start32++ = 0;  // #2
        *start32++ = 0;  // #3
        *start32++ = 0;  // #4
        *start32++ = 0;  // #5
        *start32++ = 0;  // #6
        *start32++ = 0;  // #7
        *start32++ = 0;  // #8
        *start32++ = 0;  // #9
        *start32++ = 0;  // #10
        *start32++ = 0;  // #11
        *start32++ = 0;  // #12
        *start32++ = 0;  // #13
        *start32++ = 0;  // #14
        *start32++ = 0;  // #15
        *start32++ = 0;  // #16
        size -= 16 * sizeof(uint32_t);
    }

    while (size > sizeof(uint32_t))
    {
        /* set the remaining 32-Bit values, which
         * are not catched by the big 16x32-Bit loop. */
        *start32++ = 0;
        size -= sizeof(uint32_t);
    }

    start = (uint8_t*)start32;

    while (size > 0)
    {
        /* set the remaining Bytes */
        *start++ = 0;
        size--;
    }
}

STATIC void sys_modules_init(void)
{
    /*python automated patch entry*/
    VOID_PATCH_TABLE_RUN(sys_modules_init)

	// write the patch id (base version) to SCU spare allows easy debug of patch loaded or not
	SCU_SPARE_FF__SET(version_timestamp);

    /* Initialize the scratchpad here */
    DRIVER_COMMON_init();

    //Here we load trim and config register configurations with xvalent id set to 0
    if ((SYS_BD_CONFIG_DISABLE & sys_boot_descriptor) != SYS_BD_CONFIG_DISABLE)
    {
        configurator_boot_default();
    }

    // Now we can update the clock based on the boot_descriptor
    if ((SYS_BD_BOOT_RESIST_ENABLE & sys_boot_descriptor) != SYS_BD_BOOT_RESIST_ENABLE)
    {
        sys_boot_descriptor = fw_trim_data.sys_boot_descriptor;
    }
    SystemCoreClockUpdate();

    // Patch should get the correct otp partitioning therefore default is loaded first
    if ((SYS_BD_PATCH_DISABLE & sys_boot_descriptor) != SYS_BD_PATCH_DISABLE)
    {
        patch_init();
    }

    uint8_t xaddr_pinset = 0;
    if (fw_params_xv_en_get() != 0)
    {
        xaddr_pinset = Telemetry_Get_Xvalent_Pinset(fw_config_data.addr_pin_xv&1);  // returns number between 0-15.
        //uint8_t xaddr2_pinset = Telem_Get_Xvalent_Pinset(1);  // returns number between 0-15.

        // Here we just redo the pinset read for both XADDR pins regardless of config read
		// Note that fw_config_data.addr_pin_xv&1 has already been sampled so skip that to save time
		// just grab the pinset of the opposite of fw_config_data.addr_pin_xv to make sure both pinsets have been read
		// SHASTA-567 fix.
		/*lint -e(534)  Ignoring return value of function.  We want the side affects of pinset register being set but don't need value of it now */
		Telemetry_Get_Xvalent_Pinset((~fw_config_data.addr_pin_xv)&1);
    }

    if ((SYS_BD_CONFIG_DISABLE & sys_boot_descriptor) != SYS_BD_CONFIG_DISABLE)
    {
        configurator_boot_xvalent(xaddr_pinset);
    }

    MMU_Initialize();
    GPIO_Drv_init((gpio_idx_t) 0);
    GPIO_Drv_init((gpio_idx_t) 1);
    faults_drv_init();
    PMBUS_drv_init();
    Telemetry_drv_init();
    Regulation_Drv_Init();

}

/**
 Dummy function for RTX which always allocates a thread definition for main() (see RTX_CM_lib.h).
 Our threads are contained within each module. There is no actual 'main' thread.
 @return a bogus value ... If main() does not return anything, the Keil compiler complains.
 */
/*lint -e(970)  Suppress once: Use of modifier or type 'int' outside of a typedef. armcc needs this. */
int main(void)
{
    return 0;
}
