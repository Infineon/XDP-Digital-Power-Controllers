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
 * @file    system_shasta.c
 * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer Header File for Device Shasta.
 *
 *          From CMSIS-CORE->Reference->System and Clock Configuration:<br>
 *          ARM provides a template file system_device.c that must be adapted by the silicon
 *          vendor to match their actual device. As a minimum requirement, this file must provide:
 *          - A device-specific system configuration function, SystemInit().
 *          - A global variable that contains the system frequency, SystemCoreClock.
 *          .
 *          The file configures the device and, typically, initializes the oscillator (PLL) that is
 *          part of the microcontroller device. This file might export other functions or variables
 *          that provide a more flexible configuration of the microcontroller system.
 */

/* Compiler configurations first */
#include "fw_config.h"          // Essential fw-wide configuration for e.g. buffer size #defines
#include "environment.h"        // STATIC, INLINE, FOREVER
#include "linker_sections.h"    // Linker specific memory sections

/* Always compare to the associated interface */
#include "system_shasta.h"

/* Standard library */
#include <stdint.h>     // uint*_t
#include "fw_bool.h"    // boolean type

/* APIs */
#include "shasta_hal_cgu.h"         // Access to HAL: Clock Generation Unit
#include "shasta_hal_rgu.h"         // Access to HAL: Reset Generation Unit registers
#include "shasta_hal_scu.h"         // Access to HAL: System Control Unit registers
#include "shasta_hal_tstctrl.h"  	// ate test state
#include "shasta.h"                 // Access to CMSIS functions that related to the NVIC controller
#include "sys_boot_descr_format.h"  // sys_boot_descriptor bitfield definitions
#include "fw_params.h"               // boot_descriptor
#include "log_app.h"                // Log buffer reset
#include "clk_ctrl_drv.h"           // clock control driver
#if IMAGE_TYPE == ROM_IMAGE
#include "wdt_drv.h"                // Access to HAL: Watchdog timer
#endif

/* ============================================================================
 * Private data
 * ========================================================================= */

/** @defgroup grpShasta_Clock_Values   Clock frequencies and dividers
 * The CPU subsystem has multiple dividers for the clocks of different modules and some of the
 * clocks shall run at a fixed frequency. The according values are summarized here.
 * @{
 */

/** Defined frequency in MHz for the CPU subsystem clock (alpha_clk) */
#define FREQ_ALPHA_CLOCK    ((uint32_t) 100)
/** Defined frequency in MHz for the OTP memory modules clock (cnfg_otpX_w_clk2) */
#define FREQ_OTP_CLOCK      ((uint32_t) 25)

/** Clock divider for the CPU subsystem clock (alpha_clk_div). Note: div = 0 means clock/1. */
#define SYS_ALPHA_CLK_DIVIDER \
    (uint8_t) (FREQ_PRIMARY_CLOCK / FREQ_ALPHA_CLOCK)

/** Calculates the clock divider for the CPU (cpuclk_div). */
#define SYS_CPU_CLK_DIVIDER(freq_cpu_clock) \
    (uint8_t) (FREQ_ALPHA_CLOCK / freq_cpu_clock)

/** Calculates the clock divider for the OTP memory (otpXclk2_div). */
#define SYS_OTP_CLK_DIVIDER(freq_cpu_clock) \
    (uint8_t) (freq_cpu_clock / FREQ_OTP_CLOCK)

/** @} */ //grpShasta_Clock_Values

/** For a delay loop this is the number of loop iterations per 1 microsecond (us),
 * assuming that one loop iteration takes 4 cycles and that we use a 100MHz core clock:
 * @code
    iter = 1 * 10e-9 s / (4 cycles * 1/100e9 MHz) = 100 / 4
 * @endcode
 */
#define SYS_LOOP_ITER_PER_US (uint32_t)(FREQ_ALPHA_CLOCK / 4)


STATIC const IRQn_Type priority_0_IRQs[] =
{
	FH_FAULT1_IRQn,   	            // = 21,       /*!< Event 1 in Fault Module module */
	RAMP0_T1_IRQn,              	// = 0,        /*!<ext0>_IRQn < Interrupt on T1 crossing of pwm ramp0 > */
	RAMP0_T2_IRQn,               	// = 6,        /*!<ext1>_IRQn < Interrupt on T2 crossing of pwm ramp0 */
	RAMP1_T1_IRQn,               	// = 7,        /*!<ext2>_IRQn< Interrupt on T1 crossing of pwm ramp1 */
	RAMP1_T2_IRQn,               	// = 8,        /*!<ext3>_IRQn< Interrupt on T2 crossing of pwm ramp1 */
	FSW_IRQn,                		// = 26,       /*!< External source 12: Switching Frequency Loop 0 Interrupt */
};

STATIC const IRQn_Type priority_1_IRQs[] =
{

    INT_SVID0_IRQn,                 // = 1,        /*!< frameIRQ: A new SVID frame has been received */
    INT_SVID1_IRQn,                 // = 2,        /*!< sliceIRQ: A detection slice has detected a specific frame */
    PMBUS_IRQn,                    	// = 4,        /*!< Event in PMBus module */
    SSP_IRQn,                       // = 29,       /*!< Event in SPI Interface Module */
    UART_IRQn,                      // = 31,       /*!< Event in UART Interface Module */
    I2C_IRQn,                       // = 30,       /*!< Event in I2C Interface Module */
    WDT_IRQn,                        // = 12,       /*!< Event in ARM Watchdog Timer module */
    TEST_IRQn              			// = 22,      	/*!<ext8>_IRQn < Event 2 in Fault Module module */
};
STATIC const IRQn_Type priority_3_IRQs[] =
{
	OTP1_W_IRQn,                 	//= 5,        /*!< OPT1 finished operation */
    GPIO0_IRQn,                    	// = 19,       /*!< Event in GPIO-0 module */
    GPIO1_IRQn,                    	// = 20,       /*!< Event in GPIO-1 module */
    CSC_IRQn,                      	// = 3,        /*!< Event in Clock & System Control container */
	DTIMER1_0_IRQn,               	// = 13,       /*!< Event 1 in ARM Dual Timer module */
	DTIMER1_1_IRQn,               	// = 14,       /*!< Event 2 in ARM Dual Timer module */
	DTIMER2_0_IRQn,               	// = 15,       /*!< Event 1 in ARM Dual Timer module */
	DTIMER2_1_IRQn,               	// = 16,       /*!< Event 2 in ARM Dual Timer module */
	DTIMER3_0_IRQn,               	// = 17,       /*!< Event 1 in ARM Dual Timer module */
	DTIMER3_1_IRQn,                	// = 18,       /*!< Event 2 in ARM Dual Timer module */
	VC0_VCONTROL_IRQn,           	//= 23,       /*!< External source 9 for Voltage Control 0 Window Interrupt */
	VC1_VCONTROL_IRQn,           	//= 24,       /*!< External source 10 for Voltage Control 1 Window Interrupt */
	TLM_IRQn,                    	//= 25,       /*!< External source 11 for Telemetry Interrupts */
    DMA_IRQn,                      	// = 28,       /*!< Event in Direct Memory Access module */
    CAL_ALL_DONE_IRQn              // = 9,        /*!<ext4>_IRQn< External source is not used (yet) */

};

/**
 * Calculate number of entries for each level of interrupt priority
 */
#define NUM_IRQS_PRIORITY_0        (sizeof(priority_0_IRQs)/sizeof(IRQn_Type))
#define NUM_IRQS_PRIORITY_1        (sizeof(priority_1_IRQs)/sizeof(IRQn_Type))
#define NUM_IRQS_PRIORITY_3        (sizeof(priority_3_IRQs)/sizeof(IRQn_Type))


/* ============================================================================
 * Exported global data (see header file for the documentation)
 * ========================================================================= */

/*lint -esym(714, SystemCoreClock) -esym(552, SystemCoreClock) -esym(765, SystemCoreClock) : Not accessed, could be made static
 * Test variable for verification. */
/**
 * Holds the system core clock, which is the system clock frequency supplied to the SysTick timer
 * and the processor core clock. This variable can be used by debuggers to query the frequency of
 * the debug timer or to configure the trace clock speed.
 *
 * @attention Compilers must be configured to avoid removing this variable in case the application
 * program is not using it. Debugging systems require the variable to be physically present in
 * memory so that it can be examined to configure the debugger.
 *
 * @note CamelCase due to CMSIS naming convention.
 */


/*lint -esym(714, sys_boot_state) -esym(552, sys_boot_state) -esym(765, sys_boot_state) : Not accessed, could be made static
 * Test variable for verification. */
RESET_RESISTANT_DECLARE(Osys_Boot_State_t, sys_boot_state);
/*lint -esym(714, sys_filler_byte) -esym(552, sys_filler_byte) -esym(765, sys_filler_byte) : Not accessed, could be made static
 * Test variable for verification. */
RESET_RESISTANT_DECLARE(uint8_t, sys_filler_byte);
RESET_RESISTANT_DECLARE(uint32_t, sys_boot_descriptor);
RESET_RESISTANT_DECLARE(uint8_t, sys_num_critical_reboots);
RESET_RESISTANT_DECLARE(uint32_t, SystemCoreClock);


/* ============================================================================
 * Functions (public and private in a monolithic hierarchy)
 * ========================================================================= */

/* ----------------------------------------------------------------------------
 * Forward declaration and documentation of private functions
 * ------------------------------------------------------------------------- */

/**
 * sys_delay_by_us() burns time by running through an empty
 * while() loop. Each iteration consumes 4 clock cycles at the
 * given optimization level `O3`.
 *
 * @param iters number of loop iterations, each one consuming 4 cycles.
 *
 * @section sctShasta_NopDelayLoop   Usage of a No-Operation delay loop for busy-waiting delays
 * The idea is pretty simple: Use an empty loop and use its execution to obtain a
 * specific delay. Using a carefully calibrated while loop allows to implement a
 * minimum delay even in the presence of an RTOS that is scheduled at a higher
 * scheduling period.
 */
STATIC void sys_delay_by_nops(uint32_t iters);

/**
 * sys_interrupt_priorities_configure() configures
 * IRQs into four levels of decreasing interrupt priority ranging 0..3
 * with 0 as the highest.
 */
STATIC void sys_interrupt_priorities_configure(void);

/* ------------------------------------------------------------------------- */

/*lint -esym(714, SystemInit) -esym(759, SystemInit) -esym(765, SystemInit)
 * Not declared as local, because called from init assembly routine. */
void SystemInit(void)
{
    SYS_BOOT_STATE_TRACK(BOOT_STARTED);
    sys_filler_byte = 0;

#if ( IMAGE_TYPE == ROM_IMAGE )
    /* As the very first action initialize the reset resistant log buffer in case of booting
     * from power on (also known as "cold" reset) or SWPWDN or HWPWDN so that it can already
     * be used in the Hard Fault Handler. The initialization resets and hence empties the log buffer.
     * All resets except Power on, SW PWD and HW PWD keep the log entries. */
    if ((RGU_RSTSR__GET() == 0) ||
            ((RGU_RSTSR__GET() & RGU_RSTSR__HWPWDNRST__Msk) ==  RGU_RSTSR__HWPWDNRST__Msk) ||
            ((RGU_RSTSR__GET() & RGU_RSTSR__SWPWDNRST__Msk) ==  RGU_RSTSR__SWPWDNRST__Msk))

#endif
    {
        /* for cold boot get the boot descriptor from early fw settings (no OTP involved). */
        fw_params_init();
        log_buffer_reset();
        sys_boot_descriptor = fw_trim_data.sys_boot_descriptor;
    }

    /* Enable dynamic clock gating all clocks the CPU subsystem.
     * This means that during a __wfi() or a __wfe() instruction these modules stop
     * processing. */
    ClkControl_SetSEMskAll();

    /*Enable the essential sources which wake the CPU up after calling
     * the instructions __wfi() and __wfe(). */
    ClkControl_SetEssentialWKUP();

    // Only set the boot descriptor to default if we do not want to use the last setting in RAM
    if ((SYS_BD_BOOT_RESIST_ENABLE & sys_boot_descriptor) != SYS_BD_BOOT_RESIST_ENABLE)
    {
        sys_boot_descriptor = fw_trim_data.sys_boot_descriptor;
    }
    SYS_BOOT_STATE_TRACK(BD_READ);

    /* Assign interrupt priorities */
    sys_interrupt_priorities_configure();

    /* The CPUS shall be activated. So at first set the clock according to the boot descriptor. */
    SystemCoreClockUpdate();

    /* Activate the Watchdog timer (WDT) unless it should be disabled. */
    /* Normal operation */
    wdt_activate(SYS_WATCHDOG_EXPIRATION_MILLISECONDS, NULL);

    SYS_BOOT_STATE_TRACK(SYSTEM_INIT);
}

void SystemCoreClockUpdate(void)
{
    /* CMSIS-Core: Calculate the system frequency based upon the current register settings.
     * This function can be used to retrieve the system core clock frequency
     * after user changed register settings. */
    /* Note: Constants are chosen for the clock dividers to prevent division at runtime. */
    uint8_t cpu_clk_div;
    uint8_t otp_clk_div;

    /* If no clock frequency is specified, default to 100MHz.
     * In case more than one frequency is specified, take the highest
     * among the specified ones in the boot descriptor. */
    if ((sys_boot_descriptor & SYS_BD_CLK_SEL_100 ) == SYS_BD_CLK_SEL_100 )
    {
        SystemCoreClock = 100000000;
        cpu_clk_div = SYS_CPU_CLK_DIVIDER(100);
        otp_clk_div = SYS_OTP_CLK_DIVIDER(100);
    }
    else if ((sys_boot_descriptor & SYS_BD_CLK_SEL_50 ) == SYS_BD_CLK_SEL_50 )
    {
        SystemCoreClock = 50000000;
        cpu_clk_div = SYS_CPU_CLK_DIVIDER(50);
        otp_clk_div = SYS_OTP_CLK_DIVIDER(50);
    }
    else if ((sys_boot_descriptor & SYS_BD_CLK_SEL_25 ) == SYS_BD_CLK_SEL_25 )
    {
        SystemCoreClock = 25000000;
        cpu_clk_div = SYS_CPU_CLK_DIVIDER(25);
        otp_clk_div = SYS_OTP_CLK_DIVIDER(25);
    }
    else /* Set the core clock to 100MHz if not restricted by the boot descriptor */
    {
        SystemCoreClock = 100000000;
        cpu_clk_div = SYS_CPU_CLK_DIVIDER(100);
        otp_clk_div = SYS_OTP_CLK_DIVIDER(100);
    }

    /* Set the clock dividers from the inner to outer ones. */

    /* Set the CPU clock to the value determined by the boot descriptor and
     * set the clock of OTP1&2 to constant 25MHz. */
    ClkControl_SetCPUClk(cpu_clk_div);
    ClkControl_SetOTPKrnClk(otp_clk_div);

    /* Set the watchdog timer clock to the same value as the CPU clock (it is also derived from
     * the alpha clock). Leave the double timer clock untouched. */

    //SHASTA-307 Keep Watchdog Kernel Clock at default
	//ClkControl_SetWDTKrnClk(cpu_clk_div);

    /* Now set the alpha clock to constant 100MHz (hosc_clk/1) to speed up all CPUS clocks.
     * On reset the value is 25MHz (hosc_clk/4). */
	ClkControl_SetAlphaClk(SYS_ALPHA_CLK_DIVIDER);
	
	/* Enable all clocks needed during system init */
	ClkControl_SystemClkInit();
}

void sys_delay_by_us(uint32_t delay_us)
{
    uint32_t clk_div = (CGU_ALPHA_CLK_DIV_CTRL__GET() & CGU_ALPHA_CLK_DIV_CTRL__ALPHA_CLK_DIV__Msk)
                        >> CGU_ALPHA_CLK_DIV_CTRL__ALPHA_CLK_DIV__Pos;
    uint32_t cpu_div = (CGU_CPU_CLK_DIV_CTRL__GET() & CGU_CPU_CLK_DIV_CTRL__CPUCLK_DIV__Msk)
                        >> CGU_CPU_CLK_DIV_CTRL__CPUCLK_DIV__Pos;

    /* Restrict to divider settings up to 4. clk_tick_shifts does not support more as of today and
     * the current requirements do not mandate to go below 100/4MHz = 25MHz. */
    clk_div &= (uint32_t)0x3;
    cpu_div &= (uint32_t)0x3;

    /* clk_tick_shifts maps the divider settings of ALPHA_CLK_DIV_CTRL.alpha_clk_div
     * to shift values. This allows to use simple shift operations when adjusting
     * a specific delay for the currently configured clock frequency.
     *
     * @note Using shift values restricts the allowed divider configuration to
     * power-of-2 values. That is, we limit the core clock configuration to
     * 100MHz, 50MHz and 25MHz. As of today, this seems to be sufficient.
     */
    uint8_t clk_tick_shifts;
    switch(clk_div)
    {
    case 0:
        /* alpha_clk_div = 0 ==> divider is set to 1 => 100 MHz clock */
        switch(cpu_div)
	{
	case 0:
	   clk_tick_shifts = 0;  /* cpu_clk_div = 0 ==> divider is set to 1 => 100 MHz clock */
           break;
	case 1:
           clk_tick_shifts = 1;  /* cpu_clk_div = 1 ==> divider is set to 2 => 50 MHz clock */
           break;
	default:   
           clk_tick_shifts = 2;  /* cpu_clk_div = 2,3 ==> divider is set to 3,4 => 25 MHz clock */
           break;
	}
	break;
    case 1:
        /* alpha_clk_div = 1 ==> divider is set to 2 => 50 MHz clock */
        switch(cpu_div)
	{
	case 0:
	   clk_tick_shifts = 1;  /* cpu_clk_div = 0 ==> divider is set to 1 => 50 MHz clock */
           break;
	case 1:
           clk_tick_shifts = 2; /* cpu_clk_div = 1 ==> divider is set to 2 => 25 MHz clock */
           break;
	default:   
           clk_tick_shifts = 3; /* cpu_clk_div = 2,3 ==> divider is set to 3,4 => 12.5 MHz clock */
           break;
	}
	break;
    default:
        /* alpha_clk_div = 2 ==> divider is set to 3 => 33.3 MHz clock, not supported, mapped to 25MHz */
        /* alpha_clk_div = 3 ==> divider is set to 4 => 25 MHz MHz clock */
        switch(cpu_div)
	{
	case 0:
	   clk_tick_shifts = 2; /* cpu_clk_div = 0 ==> divider is set to 1 => 25 MHz clock */
           break;
	case 1:
	   clk_tick_shifts = 3; /* cpu_clk_div = 1 ==> divider is set to 2 => 12.5 MHz clock */
           break;
	default:   
           clk_tick_shifts = 4; /* cpu_clk_div = 2,3 ==> divider is set to 3,4 => 6.25 MHz clock */
           break;
	}
        break;
    }

    /* Convert input parameter to core clock ticks by multiplying with
     * the microsecond scale factor, adapted by the current clock divider setting. */
    /*lint -e{522} Suppress: Highest operation, function 'sys_delay_by_nops', lacks side-effects  */
    sys_delay_by_nops((delay_us * SYS_LOOP_ITER_PER_US) >> clk_tick_shifts);
}

/* All current pragma settings go to the stack to temporarily allow different ones. */
#pragma push
/* Make sure we have a well-defined optimization level, otherwise
 * SYS_LOOP_ITER_PER_US is not accurate, the actual code looks different. */
#pragma O3
#pragma no_inline

STATIC void sys_delay_by_nops(uint32_t iters)
{
    while (iters-- != 0) {}
    return;
}

#pragma pop

STATIC void sys_interrupt_priorities_configure(void)
{
    for (uint32_t i = 0; i < NUM_IRQS_PRIORITY_0; i++)
    {
        NVIC_SetPriority(priority_0_IRQs[i], 0);
    }
    for (uint32_t i = 0; i < NUM_IRQS_PRIORITY_1; i++)
    {
        NVIC_SetPriority(priority_1_IRQs[i], 1);
    }
    for (uint32_t i = 0; i < NUM_IRQS_PRIORITY_3; i++)
    {
        NVIC_SetPriority(priority_3_IRQs[i], 3);
    }
}

void sys_reset(void)
{
    /* Provide access for the CPU to reset the CPU Subsystem by issuing a Soft Reset. */
    /* Issue the Soft Reset. */
    ClkControl_SetSWReset();
}

void sys_power_down(void)
{
    /* Disable the wakeup sources. Otherwise any IRQ or other trigger will wake up the CPU
     * again after the following "software power down" which in turn leads to a reboot
     * when the CPUS should sleep forever. */
    ClkControl_RstEssentialWKUP();

    /* Enable and execute "software power down". */
    ClkControl_SetSWPowerDown();

    /* The CPU is off at this point. */
}

boolean sys_irq_is_pending(IRQn_Type irq_id)
{
    if (NVIC_GetPendingIRQ((IRQn_Type)irq_id))
    {
        /* CMSIS does not provide an NVIC_IsEnabledIRQ function.
         * So directly read the NVIC register. */
        /*lint -e{571} Suppress: Suspicious cast of irq_id.
         * This code was copied from the CMSIS file. */
        if (NVIC->ISER[((uint32_t)irq_id >> 5)] & ((uint32_t)1 << ((uint32_t)irq_id & (uint8_t)0x1F)))
        {
            return boolean_true;
        }
    }

    return boolean_false;
}

Irq_Handler sys_irq_handler_get(IRQn_Type irq_id)
{
    /* Use an additional variable to mute armlink */
    uint32_t addr_vector_table = (uint32_t)SECTION_BASE_VECTORS_RAM;

    Irq_Handler* irq_handler_list = (Irq_Handler*) ( addr_vector_table );

    return irq_handler_list[(uint8_t)(CORTEX_M_NUM_SYSTEM_EXCEPTIONS + (uint8_t) irq_id)];
}

void sys_irq_handler_set(IRQn_Type irq_id, Irq_Handler handler)
{
    /* Use an additional variable to mute armlink */
    uint32_t addr_vector_table = (uint32_t)SECTION_BASE_VECTORS_RAM;

    Irq_Handler* irq_handler_list = (Irq_Handler*) ( addr_vector_table);

    irq_handler_list[(uint8_t)(CORTEX_M_NUM_SYSTEM_EXCEPTIONS + (uint8_t) irq_id)] = handler;
}

