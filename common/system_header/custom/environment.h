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
 * @file    environment.h
 * @brief   Contains compiler environment specific adaptations and definitions.
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#ifndef UNIT_TEST
    #if defined (__CC_ARM)
        /* ARM compiler */
		#define INLINE __inline
		#define STATIC static
        #define CONST const
	#else
        /* Other compilers */
        #define INLINE inline
        #define STATIC static
        #define CONST const
	#endif
#else
	/* unit test and mocking platform compilation */
	#define INLINE
	#define STATIC
    #define CONST
#endif /* TEST_NO_INLINE_STATIC */


#ifndef UNIT_TEST

    /* for loop, used for not unit test environment (target image) */
    #define FOREVER for(;;)
    #define TRY_FOREVER     TRY_AGAIN

#else

    #include "stdint.h"
    extern uint32_t forever(void);
    extern uint32_t try_forever(void);

    /* for loop, used for unit test environment.
     * The forever() - function need to be provided by the unit test environment */
    #define FOREVER while(forever())
    #define TRY_FOREVER     TRY while(try_forever())

#endif /* UNIT_TEST */


#if defined (__CC_ARM)
    #define PROGRAM_COUNTER   (uint32_t)__current_pc()
#else
    #define PROGRAM_COUNTER   (uint32_t)0xFFFFFFFF
#endif /* (__CC_ARM) */

/** Declaration for a global variable which shall not be reset during general
 * memory initialization because it shall be set before the initialization or it shall be reset
 * only in specific cases. */
#define RESET_RESISTANT_DECLARE(type, name) \
    type name __attribute__ ((section ("RESET_RESIST")))

/** Declaration for a function which should reside in a RAM execution region for faster performance.
 *  used for patches which reside in OTP  */
#define RAM_EXEC_DECLARE(type, name) \
        __attribute__ ((section ("RAM_EXEC"))) type name

#endif /* ENVIRONMENT_H */
