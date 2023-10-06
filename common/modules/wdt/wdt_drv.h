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
 * @file    wdt_drv.h
 * @brief   Watch dog timer driver header file
 */

#ifndef WDT_DRV_H
#define WDT_DRV_H

#include "shasta.h" // Cortex_M0_Stack_Frame_t

/*lint -esym(714, wdt_*) -esym(759, wdt_*) -esym(765, wdt_*) -esym(757, wdt_*) -esym(752, wdt_*)
 * These functions and variables might currently not been used inside the target image, 
 * but we need it inside the unit test framework or treated as library functions.
 * The linker will remove used symbols anyway. */
 
/*lint -esym(769, WDT_TIME_*)
 * These enums might currently not been used inside the target image, 
 * but we need it as library functions. */

/* ============================================================================
 * Public data types
 * ========================================================================= */

typedef enum { WDT_TIME_SEC,
                WDT_TIME_MS,
                WDT_TIME_US /* NS is not supported since the 100MHZ clock produces 10ns period*/
} wdt_time_unit_t;

/** Watchdog timer specific callback type */
/*lint -strong(l, wdt_Fkt_Cb_t) Strong type mismatch (type 'wdt_Fkt_Cb_t') in equality or conditional */
typedef void (*wdt_Fkt_Cb_t)(void);
/** Stack frame that will be collected in the Watchdog IRQ Handler just before a reset is issued. */
extern Cortex_M0_Stack_Frame_t wdt_last_stackframe;


/* ============================================================================
 * Function declarations
 * ========================================================================= */

/**
 * Provide a complete API to activate a watchdog timer(WDT). This function
 * calls following in sequence to start up a WDT:
 *  wdt_init();
 *  wdt_register_interrupt_callback();
 *  wdt_load();
 *  wdt_reset_enable();
 *  wdt_interrupt_enable();
 *  Please read the description of WatchDog_IRQHandler()too.
 *
 * @param time_ms The time to count down in ms. Sets the initial value for the WDT down counter.
 * When down counter decrements to 0, an interrupt will be generated.
 *
 * @param callback A user provided callback function. This callback will be called
 * when time reaches to 0 and the wdt interrupt is generated. In order to prevent a reset,
 * callback function could reload the down counter.
 * NULL if no callback provided.
 */
void wdt_activate(uint32_t time_ms, wdt_Fkt_Cb_t callback);


/**
 * Interrupt service routine(ISR) for the down counter, aka time expire.
 *
 * If the down counter is not reload with a call to wdt_interrupt_clear(),
 * a reset signal will be generated to the CPUS given the reset
 * is enabled earlier with a call to wdt_reset_enable(). The reset signal is not
 * generated right away after time expiration. The down counter is reloaded
 * with the initial value and begin to down counting again. If the ISR's callback
 * reload the down counter by calling wdt_interrupt_clear() before the counter
 * reaches to 0, no reset will be generated. In other word, the down counter counts
 * twice starting with the initial value from the time of interrupt to reset.
 */
void WatchDog_IRQHandler(const Cortex_M0_Stack_Frame_t* stack_frame);

/** Call this before the WDT count down to 0.
 * Reload the wdt down counter so interrupt or reset will not occur
 * will reload the WDT counter.
 */
void wdt_keep_alive(void);

/**
 * Get current down counter value.
 * @return current timer's value
 */
uint32_t wdt_get_current_timer_value(void);


#endif /* WDT_DRV_H */
