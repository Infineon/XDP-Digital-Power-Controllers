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
 * @file    dtimer_drv.h
 * @brief   Header file for dtimer
 *
 */

#ifndef DTIMER_DRV_H
#define DTIMER_DRV_H
#include <stdint.h>
#include "driver_common.h" // cmsis definitions
#include "system_shasta.h" // interrupt handler typedef
#include "shasta_hal_dtimer.h"


/** Version definitions for dtimer
 *  - Version 0.05:
 *      Timer did not work correctly with a load of \p 0
 *      In this case an interrupt is issued before the initialization was finished.
 *  - Version 0.06:
 *      Fast Timer start added, see JIRA Shasta-619
 */
#define DTIMER_API_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,06) /* API version */
#define DTIMER_DRV_VERSION   DRIVER_VERSION_MAJOR_MINOR(0,06) /* API version */

/** Capabilities \p 1 indicates that the dtimer driver is available */
typedef struct DTIMER_CAPABILITIES_s
{
    uint32_t DTIMER_CAPABLE;
} DTIMER_CAPABILITIES;

/** To indicate the status of the dtimer
 */

typedef enum DTIMER_STATUS_e
{
    /** dtimer is not in use, it is uninitialized */
    DTIMER_STATUS_FREE=0,
    /** dtimer is use means, it is initialized */
    DTIMER_STATUS_BUSY
} DTIMER_STATUS;

/**
 * DTimer can be programmed to interrupt one shot or interrupt periodically.
 */

/*lint -parent(uint32_t, dtimer_mode_e) -strong(AJX)*/
typedef enum {
    /** one time interrupt time-base is \p load*/
    ONE_SHOT_INT = 0,
    /** periodic interrupt, first time-base is \p load and afterwards \p bgload*/
    PERIODIC_INT }
dtimer_mode_e;

/**
 * DTimer can be started with the intitialization of started with a separated command after the initialization.
 */

/*lint -parent(uint32_t, dtimer_mode_e) -strong(AJX)*/
typedef enum {
    /** one time interrupt time-base is \p load*/
    STARTWITHINI = 0,
    /** periodic interrupt, first time-base is \p load and afterwards \p bgload*/
    STARTEXTRA }
dtimer_start_mode_e;

/**
 * Three dtimer modules are available with two timers each.
 * The first module contains DTIMER0 and DTIMER1.
 * The second module contains DTIMER2 and DTIMER3.
 * The third module contains DTIMER4 and DTIMER5.
 */
/*lint -parent(uint32_t, dtimer_instance_e) -strong(AJX)*/
typedef enum {
    DTIMER0 = 0, DTIMER1, DTIMER2, DTIMER3, DTIMER4, DTIMER5
} dtimer_instance_e;


/** Data set of DTimer driver */
typedef struct
{

    /** Mode Selector single shot or periodic
     */
    dtimer_mode_e mode;

    /** Mode Selector start with initializatio or separated
     */
    dtimer_start_mode_e start_mode;

    /** Load  */
	uint32_t load;

	/** Background Load */
    uint32_t bgload;

    /** Interrupt Service Routine Callback */
    Irq_Handler dtimer_ISR;

} DTIMER_CONFIG_t;

/** Array declaration to decode the dtimers more conveniently
 *  from 4 logical timers to a physical set of 2 timers with 2 timer channel each*/
extern const uint32_t dtimer_sel[6][2];

/* Array declaration to decode the dtimer control address more conveniently
 *  from 6 logical timers to a physical set of 3 timers with 2 timer channel each*/
extern const uint32_t dtimer_control_address[6];

/**
 * DTIMER_GetVersion retrieves the version of the dtimer drv and api
 *
 *  @return DRIVER_VERSION
 */
DRIVER_VERSION DTIMER_GetVersion (void);
/**
 * DTIMER_GetCapabilities retrieves the Capabilities of the dtimer
 *  @param
 *
 *  @return DTIMER_CAPABILITIES is just \p 1
 */
DTIMER_CAPABILITIES DTIMER_GetCapabilities (void);

/**
 * This routine initializes a timer and install the respective IRQ handler. The DTimer can
 * be in periodic or one-shot.
 *  @param  dtie Which timer instance to use DTIMER0 .. DTIMER5.
 *  @param  dts  Status and configuration of the timer module.
 *
 *  @throws OUT_OF_RESOURCE if the timer is busy.
 *
 *  @return none
 */
void DTIMER_Initialize (dtimer_instance_e dtie, DTIMER_CONFIG_t dts);

/**
 * DTIMER_Uninitialize deactivates the respective timer. if all timers of a module are deactivated
 * the clock gets gated and the reset is activated for the respective module.
 *
 *  @param  dtie Which timer instance to use DTIMER0 .. DTIMER5.
 *
 *  @return none
 */
void DTIMER_Uninitialize (dtimer_instance_e dtie);
/**
 * DTIMER_ClearIRQ clears the latched interrupt of the respective module.
 *
 *  @param  dtie Which timer instance to use DTIMER0 .. DTIMER5.
 *
 *  @return none
 */
void DTIMER_ClearIRQ (dtimer_instance_e dtie);


/**
 * DTIMER_GetStatus retrieves the status of the respective timer.
 *
 *  @param  dtie Which timer instance to use DTIMER0 .. DTIMER5.
 *
 *  @return DTIMER_STATUS indicates whether the timer is busy or not
 */

DTIMER_STATUS DTIMER_GetStatus (dtimer_instance_e dtie);


/** DTIMER0_Start fast start of a preconfigured timer
 *
 *  @param  dtie Which timer instance to use DTIMER0 .. DTIMER5.
 */
__DTIMER_INLINE void DTIMER_Start (dtimer_instance_e dtie)
{

    HAL_SET32((volatile uint32_t*) dtimer_control_address[dtie],
            HAL_GET32((volatile uint32_t*) dtimer_control_address[dtie]) |
            DTIMER_TIM_SEQ_TIMERCONTROL__TEN__Shift((uint32_t )dtimer_tim_seq_timercontrol__ten_s1) |   // timer enabled
            DTIMER_TIM_SEQ_TIMERCONTROL__INTEN__Shift((uint32_t)dtimer_tim_seq_timercontrol__inten_s1) // interrupt enabled
    );

}


/** DTIMER_Stop fast stop of a pre-configured timer
 *
 *  @param  dtie Which timer instance to use DTIMER0 .. DTIMER5.
 */
void DTIMER_Stop (dtimer_instance_e dtie);

/**
 * The DTIMER_Singleshot_Reload function is reloading the timer with a new start value.
 * This function is used for single shot timers to reactivate the timer
 *
 *  @param  dtie    Which timer instance to use DTIMER0 .. DTIMER5
 *  @param  ticks   Number of clock cycles
 *
 *  @return none
 */

void DTIMER_Singleshot_Reload(dtimer_instance_e dtie, uint32_t ticks);

/** DTIMER API definitions */

typedef struct _DRIVER_DTIMER {
  DRIVER_VERSION   (*GetVersion)      (void);
  DTIMER_CAPABILITIES (*GetCapabilities) (void);
  void                 (*Initialize)      (dtimer_instance_e dtie, DTIMER_CONFIG_t dts);
  void                 (*Uninitialize)    (dtimer_instance_e dtie);
  void                 (*ClearIRQ)        (dtimer_instance_e dtie);
  DTIMER_STATUS        (*GetStatus)       (dtimer_instance_e dti);
} const DRIVER_DTIMER;

extern DRIVER_DTIMER Driver_DTIMER;

/**
 * The dtimer_reload_count function is reload the timer with a new start value.
 * This function is used for single shot timers to reload the counter
 *
 *  @param  dtie    Which timer instance to use DTIMER0 .. DTIMER5
 *  @param  ticks   Number of clock cycles
 *
 *  @return none
 */

void dtimer_reload_count(dtimer_instance_e dtie, uint32_t ticks);

void dtimer_stop (dtimer_instance_e dtie);
void dtimer_config(dtimer_instance_e dtie, DTIMER_CONFIG_t dts);
void dtimer_shutdown(dtimer_instance_e dtie);


#endif /* DTIMER_DRV_H */
