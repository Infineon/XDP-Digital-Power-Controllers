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
 * @file    linker_sections.h
 * @brief   Contains linker specific memory sections.
 */

#ifndef LINKER_SECTIONS_H
#define LINKER_SECTIONS_H

/* This files to centralize the linker specific variables and abstract the from the
 * actual linker script
 */
/* Compiler configurations first */
#include "fw_config.h"  // here we need to figure out IMAGE_TYPE, to allow resolving all those linker-provided macros

/* Data types */
#include <stdint.h>     // uint*_t

/* Define symbols and conversion macros */


#if defined (__CC_ARM)
    /* Import symbols from the ARM linker. Note that armlink uses a strange and misleading syntax
     * to access the value of a symbol. This is the &-operator which actually is the
     * "address of"-operator but armlink uses it for its symbols as if it was the unary *-operator
     * or "access"-operator.
     * As elaborated in tests there is no difference between importing the symbols as pointer
     * (void*) or value (uint32_t) but the types are used to make it more readable. */
    #define SYMBOL_TO_POINTER(symbol) ((void*) &symbol)
    /*lint -emacro(923, SYMBOL_TO_VALUE)  Suppress: cast from pointer to unsigned int. armlink specific. */
    #define SYMBOL_TO_VALUE(symbol) ((uint32_t) &symbol)

/* This is a dirty hack until someone has cleaned up and unified all
 * our linker script files, see OPUS-646.
 * Essentially, the ram image does not have a RAM_FRONT exe region, but
 * it has ROM_EXEC instead. Obviously, ROM_EXEC is a bad name for the
 * ram image, totally misleading. But OPUS-646 is supposed to clean
 * this up.
 */
#if IMAGE_TYPE != RAM_IMAGE
    #define SYMBOL_BASE_RAM_FRONT           Image$$RAM_FRONT$$Base
#else
    #define SYMBOL_BASE_RAM_FRONT           Image$$ROM_EXEC$$Base
#endif

    #define SYMBOL_BASE_RAM_TOP             Image$$RAM_TOP$$Base
    #define SYMBOL_BASE_RW_SOURCE           Load$$RAM_INIT$$RW$$Base
    #define SYMBOL_BASE_RW_DESTINATION      Image$$RAM_INIT$$RW$$Base
    #define SYMBOL_LENGTH_RW                Image$$RAM_INIT$$RW$$Length

    #define SYMBOL_BASE_ZI                  Image$$RAM_INIT$$ZI$$Base
    #define SYMBOL_LENGTH_ZI                Image$$RAM_INIT$$ZI$$Length

    #define SYMBOL_BASE_VECTORS_ROM         VECTORS_ROM$$Base
    #define SYMBOL_BASE_VECTORS_RAM         VECTORS_RAM$$Base
    #define SYMBOL_BASE_VECTORS_OTP         VECTORS_OTP$$Base
    #define SYMBOL_LENGTH_VECTORS_ROM       VECTORS_ROM$$Length
    #define SYMBOL_LENGTH_VECTORS_OTP       VECTORS_OTP$$Length

    #define SYMBOL_BASE_LOGS                Image$$LOGS$$ZI$$Base
    #define SYMBOL_LENGTH_LOGS              Image$$LOGS$$ZI$$Length

    #define SYMBOL_BASE_SCRATCHPAD          Image$$SCRATCHPAD$$ZI$$Base
    #define SYMBOL_LENGTH_SCRATCHPAD        Image$$SCRATCHPAD$$ZI$$Length

    #define SYMBOL_BASE_OTP_DATA            Image$$OTP_DATA$$ZI$$Base
    #define SYMBOL_LENGTH_OTP_DATA          Image$$OTP_DATA$$ZI$$Length

    #define SYMBOL_BASE_OTP_EXEC_PATCH1     Image$$OTP_EXEC_PATCH1$$ZI$$Base
    #define SYMBOL_LENGTH_OTP_EXEC_PATCH1   Image$$OTP_EXEC_PATCH1$$ZI$$Length


    #define SYMBOL_BASE_ROM_EXEC            Image$$ROM_EXEC$$Base
    #define SYMBOL_LENGTH_ROM_EXEC          Image$$ROM_EXEC$$Length

    #define SYMBOL_BASE_ROM_CRC32           Image$$ROM_CRC32$$Base

	#define SYMBOL_BASE_RAM_EXEC_SOURCE         Load$$RAM_INIT$$RW$$Base
    #define SYMBOL_BASE_RAM_EXEC_DESTINATION    Image$$RAM_EXEC$$Base
    #define SYMBOL_LENGTH_RAM_EXEC              Image$$RAM_EXEC$$Length

#else
	
	#define SYMBOL_TO_POINTER(symbol) ((void*) symbol)
	#define SYMBOL_TO_VALUE(symbol) ((uint32_t) symbol)
    /* Symbols are defined elsewhere */

#endif

#if defined (__CC_ARM) || defined (UNITTEST)
	/* Beginning of RAM Section */
	extern void* SYMBOL_BASE_RAM_FRONT;
	extern void* SYMBOL_BASE_RAM_TOP;
	#define SECTION_BASE_RAM_FRONT      SYMBOL_TO_POINTER( SYMBOL_BASE_RAM_FRONT )
	#define SECTION_BASE_RAM_TOP        SYMBOL_TO_POINTER( SYMBOL_BASE_RAM_TOP )

	/* .data section (and other RW sections) */
	extern void* SYMBOL_BASE_RW_SOURCE;
	extern void* SYMBOL_BASE_RW_DESTINATION;
	extern uint32_t SYMBOL_LENGTH_RW;

	#define SECTION_BASE_RW_SOURCE      SYMBOL_TO_POINTER( SYMBOL_BASE_RW_SOURCE )
	#define SECTION_BASE_RW_DESTINATION SYMBOL_TO_POINTER( SYMBOL_BASE_RW_DESTINATION )
	#define SECTION_LENGTH_RW           SYMBOL_TO_VALUE( SYMBOL_LENGTH_RW )


	/* .bss section (and other ZI sections) */
	extern void* SYMBOL_BASE_ZI;
	extern uint32_t SYMBOL_LENGTH_ZI;

	#define SECTION_BASE_ZI     SYMBOL_TO_POINTER( SYMBOL_BASE_ZI )
	#define SECTION_LENGTH_ZI   SYMBOL_TO_VALUE( SYMBOL_LENGTH_ZI )

	/* Section of mirrored Cortex-M0 vector table */
	extern void* SYMBOL_BASE_VECTORS_ROM;      // in load & exec region
	extern void* SYMBOL_BASE_VECTORS_RAM;      // in exec region only
	extern void* SYMBOL_BASE_VECTORS_OTP;      // in exec region only
	extern uint32_t SYMBOL_LENGTH_VECTORS_ROM;
	extern uint32_t SYMBOL_LENGTH_VECTORS_OTP;

	#define SECTION_BASE_VECTORS_ROM    SYMBOL_TO_POINTER( SYMBOL_BASE_VECTORS_ROM )
	#define SECTION_BASE_VECTORS_RAM    SYMBOL_TO_POINTER( SYMBOL_BASE_VECTORS_RAM )
	#define SECTION_BASE_VECTORS_OTP    SYMBOL_TO_POINTER( SYMBOL_BASE_VECTORS_OTP )
	#define SECTION_LENGTH_VECTORS_ROM  SYMBOL_TO_VALUE( SYMBOL_LENGTH_VECTORS_ROM )
	#define SECTION_LENGTH_VECTORS_OTP  SYMBOL_TO_VALUE( SYMBOL_LENGTH_VECTORS_OTP )

	/* Section of logs */
	extern void* SYMBOL_BASE_LOGS;
	extern uint32_t SYMBOL_LENGTH_LOGS;

	#define SECTION_BASE_LOGS       SYMBOL_TO_POINTER( SYMBOL_BASE_LOGS )
	#define SECTION_LENGTH_LOGS     SYMBOL_TO_VALUE( SYMBOL_LENGTH_LOGS )
	
	/* Section of scratchpad */
	extern void* SYMBOL_BASE_SCRATCHPAD;
	extern uint32_t SYMBOL_LENGTH_SCRATCHPAD;

	#define SECTION_BASE_SCRATCHPAD       SYMBOL_TO_POINTER( SYMBOL_BASE_SCRATCHPAD )
	#define SECTION_LENGTH_SCRATCHPAD     SYMBOL_TO_VALUE( SYMBOL_LENGTH_SCRATCHPAD )

	/* Section of OTP */

	extern void* SYMBOL_BASE_OTP_DATA;
	extern uint32_t SYMBOL_LENGTH_OTP_DATA;

	extern void* SYMBOL_BASE_OTP_EXEC_PATCH1;
	extern uint32_t SYMBOL_LENGTH_OTP_EXEC_PATCH1;


	#define SECTION_BASE_OTP_DATA       SYMBOL_TO_POINTER( SYMBOL_BASE_OTP_DATA )
	#define SECTION_LENGTH_OTP_DATA     SYMBOL_TO_VALUE( SYMBOL_LENGTH_OTP_DATA )

	#define SECTION_BASE_OTP_EXEC_PATCH1       SYMBOL_TO_POINTER( SYMBOL_BASE_OTP_EXEC_PATCH1 )
	#define SECTION_LENGTH_OTP_EXEC_PATCH1     SYMBOL_TO_VALUE( SYMBOL_LENGTH_OTP_EXEC_PATCH1 )

	/* ROM image section */
	extern void* SYMBOL_BASE_ROM_EXEC;
	extern uint32_t SYMBOL_LENGTH_ROM_EXEC;

	#define SECTION_BASE_ROM_EXEC       SYMBOL_TO_POINTER( SYMBOL_BASE_ROM_EXEC )
	#define SECTION_LENGTH_ROM_EXEC     SYMBOL_TO_VALUE( SYMBOL_LENGTH_ROM_EXEC )

	/* Checksum section */
	extern void* SYMBOL_BASE_ROM_CRC32;
	#define SECTION_BASE_ROM_CRC32  SYMBOL_TO_POINTER( SYMBOL_BASE_ROM_CRC32 )
	
	/* RAM execution section for OTP functions */
	extern void* SYMBOL_BASE_RAM_EXEC_SOURCE;
	extern void* SYMBOL_BASE_RAM_EXEC_DESTINATION;
	extern uint32_t SYMBOL_LENGTH_RAM_EXEC;

	#define SECTION_BASE_RAM_EXEC_SOURCE        SYMBOL_TO_POINTER( SYMBOL_BASE_RAM_EXEC_SOURCE )
	#define SECTION_BASE_RAM_EXEC_DESTINATION   SYMBOL_TO_POINTER( SYMBOL_BASE_RAM_EXEC_DESTINATION )
	#define SECTION_LENGTH_RAM_EXEC             SYMBOL_TO_VALUE( SYMBOL_LENGTH_RAM_EXEC )
	
#else /* GCC but not Unittest */
	
	/* Symbols are defined in the linker configuration script.
	 * The array construct allows direct access to the linker variables as
	 * they are stored in DCD the sections attached to the corresponding executable
	 * module code in ROM.
	 */

	/* .data section (and other RW sections)
	 * The .data section is appended at the end of the ROM and must be copied into the RAM
	 * data section at startup. This allows to initialize RAM variables with a non-zero value  */
	extern char _end_text[];
	extern char _start_data[];
	extern char _end_data[];

	#define SECTION_BASE_RW_SOURCE      SYMBOL_TO_POINTER( _end_text )
	#define SECTION_BASE_RW_DESTINATION SYMBOL_TO_POINTER( _start_data )
	#define SECTION_LENGTH_RW           SYMBOL_TO_VALUE((_end_data - _start_data) )


	/* .bss section (and other ZI sections) */
	extern char _start_bss[];
	extern char _end_bss[];

	#define SECTION_BASE_ZI     SYMBOL_TO_POINTER( _start_bss )
	#define SECTION_LENGTH_ZI   SYMBOL_TO_VALUE( (_end_bss - _start_bss) )


	/* Section of mirrored Cortex-M0 vector table
	 * Vector table is copied from ROM to RAM */
    /* Section of mirrored Cortex-M0 vector table
     * Vector table is copied from ROM to RAM */
    extern char _start_vectors[];
    extern char _end_vectors[];
    extern char _start_vectors_ram[];

    extern char _start_vectors_otp[];
    extern char _end_vectors_otp[];

	#define SECTION_BASE_VECTORS_ROM    SYMBOL_TO_POINTER( _start_vectors )
    #define SECTION_LENGTH_VECTORS_ROM  SYMBOL_TO_VALUE( (_end_vectors -_start_vectors) )

    #define SECTION_BASE_VECTORS_RAM    SYMBOL_TO_POINTER( _start_vectors_ram )

    #define SECTION_BASE_VECTORS_OTP    SYMBOL_TO_POINTER( _start_vectors_otp )
    #define SECTION_LENGTH_VECTORS_OTP  SYMBOL_TO_VALUE( (_end_vectors_otp -_start_vectors_otp) )

    extern char _ram_base[];
    extern char _end_ram[];
    #define SECTION_BASE_RAM_FRONT       SYMBOL_TO_POINTER( _ram_base )
    #define SECTION_BASE_RAM_TOP       SYMBOL_TO_POINTER( _end_ram )

	/* Section of logs */
	extern char _log_base[];
	extern char _log_size[];
	#define SECTION_BASE_LOGS       SYMBOL_TO_POINTER( _log_base )
	#define SECTION_LENGTH_LOGS     SYMBOL_TO_VALUE( _log_size )


	/* ROM image section */
	//extern void* SYMBOL_BASE_ROM_EXEC;
	//extern uint32_t SYMBOL_LENGTH_ROM_EXEC;
	//
	//#define SECTION_BASE_ROM_EXEC       SYMBOL_TO_POINTER( SYMBOL_BASE_ROM_EXEC )
	//#define SECTION_LENGTH_ROM_EXEC     SYMBOL_TO_VALUE( SYMBOL_LENGTH_ROM_EXEC )

    /* Section of scratchpad */
    extern char _scratch_pad[];
    extern char _scratch_pad_size[];

    #define SECTION_BASE_SCRATCHPAD       SYMBOL_TO_POINTER( _scratch_pad )
    #define SECTION_LENGTH_SCRATCHPAD     SYMBOL_TO_VALUE( _scratch_pad_size )

    /* Section of OTP */

    /* Data section */
    extern char _otp_data_base[];
    extern char _otp_data_size[];

    #define SECTION_BASE_OTP_DATA       SYMBOL_TO_POINTER( _otp_data_base )
    #define SECTION_LENGTH_OTP_DATA     SYMBOL_TO_VALUE( _otp_data_size )

    /* Patch1 section */
    extern char _otp_patch1_base[];
    extern char _otp_patch1_size[];

    #define SECTION_BASE_OTP_EXEC_PATCH1       SYMBOL_TO_POINTER( _otp_patch1_base )
    #define SECTION_LENGTH_OTP_EXEC_PATCH1     SYMBOL_TO_VALUE( _otp_patch1_size )

    /* Patch2 section */
    extern char _otp_patch2_base[];
    extern char _otp_patch2_size[];

    #define SECTION_BASE_OTP_EXEC_PATCH2       SYMBOL_TO_POINTER( _otp_patch2_base )
    #define SECTION_LENGTH_OTP_EXEC_PATCH2     SYMBOL_TO_VALUE( _otp_patch2_size )

    /* Patch3 section */
    extern char _otp_patch3_base[];
    extern char _otp_patch3_size[];

    #define SECTION_BASE_OTP_EXEC_PATCH3       SYMBOL_TO_POINTER( _otp_patch3_base )
    #define SECTION_LENGTH_OTP_EXEC_PATCH3     SYMBOL_TO_VALUE( _otp_patch3_size )

    /* Patch4 section */
    extern char _otp_patch4_base[];
    extern char _otp_patch4_size[];

    #define SECTION_BASE_OTP_EXEC_PATCH4       SYMBOL_TO_POINTER( _otp_patch4_base )
    #define SECTION_LENGTH_OTP_EXEC_PATCH4     SYMBOL_TO_VALUE( _otp_patch4_size )

	/* RAM execution image section */
	extern char _ram_exec_source[];
	extern char _start_ram_exec[];
    extern char _end_ram_exec[];

	#define SECTION_BASE_RAM_EXEC_SOURCE        SYMBOL_TO_POINTER( _ram_exec_source )
	#define SECTION_BASE_RAM_EXEC_DESTINATION   SYMBOL_TO_POINTER( _start_ram_exec )
	#define SECTION_LENGTH_RAM_EXEC             SYMBOL_TO_VALUE( (_end_ram_exec - _start_ram_exec) )
	
	/* Checksum section */
	extern void* SYMBOL_BASE_ROM_CRC32;
	#define SECTION_BASE_ROM_CRC32  SYMBOL_TO_POINTER( SYMBOL_BASE_ROM_CRC32 )

#endif /*__CC_ARM*/

#endif /* LINKER_SECTIONS_H */
