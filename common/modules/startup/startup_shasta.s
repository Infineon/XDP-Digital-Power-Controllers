; ============================================================================
; Copyright (C) 2014 Infineon Technologies AG
; All rights reserved.
; ============================================================================
;
; ============================================================================
; This document contains proprietary information of Infineon Technologies AG.
; Passing on and copying of this document, and communication of its contents
; is not permitted without Infineon's prior written authorisation.
; ============================================================================
;
; @brief    CMSIS Cortex-M0 Core Device Startup File for
;           Infineon Shasta.<p>
;           The Startup File contains:
;           - The reset handler which is executed after CPU reset and typically calls
;             the SystemInit function.
;           - The setup values for the Main Stack Pointer (MSP).
;           - System Exception vectors of the Cortex-M Processor with weak functions
;             that implement default routines.
;           - IRQ vectors that are device specific with weak functions that
;             implement default routines.
;
;           The file exists for each supported toolchain and is the only tool-chain
;           specific CMSIS file.<p>
;           See also
;           <a href="http://infocenter.arm.com/help/topic/com.arm.doc.dui0497a/DUI0497A_cortex_m0_r0p0_generic_ug.pdf">
;            Cortex-M0 Devices Generic User Guide
;           </a>
;           for the programmers model and detailed information about the core
;           peripherals.
; @note    This file uses syntax and a specific initialization routine for
;          the ARM compiler 'armcc'.
;
; ============================================================================

#include "fw_config.h"


;/** @todo Find a way to parse assembler files with doxygen. http://stackoverflow.com/questions/267143/doxygen-and-assembly-language */

; Indentation: Unified Assembler syntax as specified by Advanced RISC Machines
; Ltd is indented 4 spaces. Assembler specific syntax is indented 8 spaces.

; Note on address alignment:
; When input sections have been ordered and before the base addresses are
; fixed, the linker inserts padding, if required, to force each input section
; to start at an address that is a multiple of the input section alignment.

; Beginning of the startup module

    ; Sets a module attribute that informs the linker that all
    ; exported functions in the module preserves an 8-byte
    ; aligned stack.
    PRESERVE8 ; from template, probably important to align the stack correctly

    ; Subsequent instructions are assembled either as 16-bit Thumb instructions,
    ; or as 32-bit Thumb-2 instructions if the specified core supports the
    ; Thumb-2 instruction set. The assembler syntax follows the Unified
    ; Assembler syntax as specified by Advanced RISC Machines Ltd.
    THUMB

; "AREA .stack, NOINIT, READWRITE, ALIGN=3"
; Defines a section with space reservation only, readwrite type and
; address alignment of 2^(3)=8
; Sections are independent, named, indivisible chunks of code or data that are
; manipulated (most probably only relocated) by the linker.

; =============================================================================
; Define empty stack region for the system initalization (heap is not allowed)
;    AREA STACKHEAP, NOINIT, READWRITE, ALIGN=3
; Allocate uninitialized memory
; (labels used for __user_initial_stackheap)
;StackHeap_Size  EQU 1024/2
;StackHeap_Mem   SPACE StackHeap_Size
;__initial_sp
; =============================================================================
; =============================================================================
; Define empty data region for logging
;    AREA LOGS, NOINIT, READWRITE
;#if IMAGE_TYPE == RAM_IMAGE
    ; Allocate uninitialized memory
;    SPACE 1*1024
;#endif
;#if IMAGE_TYPE == ROM_IMAGE
    ; Allocate uninitialized memory
;    SPACE 13*1024
;#endif
; =============================================================================
; =============================================================================
; Data section containing the Cortex-M0 startup and exception vectors
; It either needs to be stored in ROM or RAM, depending on the current image type we
; are about to build.
#if (IMAGE_TYPE == ROM_IMAGE)
    AREA VECTORS_ROM, DATA, READONLY
#else
; @todo AP: what about the DATA and READONLY attributes? READONLY does not
; make any sense when we build the ram image. But leaving it out creates an
; elf file that misses to set the entry point to Reset_Handler() ..
    AREA VECTORS_RAM, DATA, READONLY
#endif

    IMPORT Default_IRQHandler
    ; PMBus Handler is disabled for now
    ; IMPORT PMBus_IRQHandler


    IMPORT |Image$$STACKHEAP$$ZI$$Limit|

    ; Export symbols
    EXPORT __Vectors
    EXPORT __Vectors_End
    EXPORT __Vectors_Size

; Cortex-M0 Vector Table (must be mapped correctly to address 0x0 by the linker)
__Vectors
        ; 1) Initial SP value and addresses to Reset Handler
        ;    and System Exception Handlers
        ; DCD: Allocate memory for a 32-Bit long word
        ;                 Exception#|IRQ#| Address| Description
        ;                 ----------+----+-----+---------------
        DCD |Image$$STACKHEAP$$ZI$$Limit|      ; --| ---| 0x00| Top of Stack (= MSP)
        DCD Reset_Handler       ;  1| ---| 0x04| Reset Handler
        DCD NMI_Handler         ;  2| -14| 0x08| Non-Maskable Interrupt Handler
        DCD HardFault_PreHandler ;  3| -13| 0x0C| Hard Fault Handler
        DCD 0                   ;  4| ---| 0x10| Reserved
        DCD 0                   ;  5| ---| 0x14| Reserved
        DCD 0                   ;  6| ---| 0x18| Reserved
        DCD 0                   ;  7| ---| 0x1C| Reserved
        DCD 0                   ;  8| ---| 0x20| Reserved
        DCD 0                   ;  9| ---| 0x24| Reserved
        DCD 0                   ; 10| ---| 0x28| Reserved
        DCD SVC_Handler         ; 11|  -5| 0x2C| Supervisor Call Handler
        DCD 0                   ; 12| ---| 0x30| Reserved
        DCD 0                   ; 13| ---| 0x34| Reserved
        DCD PendSV_Handler      ; 14|  -2| 0x38| Pending SV Handler
        DCD SysTick_Handler     ; 15|  -1| 0x3C| System Tick Handler

        ; 2) Adresses to the IRQ Handlers (= ISRs) for IRQ0..IRQn
        ; From the CPU Subsystem ITS "CPUS.pdf".
        ; Compare to shasta.h "enum IRQn".
        ;                             Exception#|IRQ#| Address| Note
        ;                             ----------+----+-----+---------------
        DCD Default_IRQHandler              ; 16|   0| 0x40| Reserved
        DCD Default_IRQHandler              ; 17|   1| 0x44| Reserved
        DCD Default_IRQHandler              ; 18|   2| 0x48| Reserved
        DCD Default_IRQHandler              ; 19|   3| 0x4C| Reserved
        DCD PMBus_IRQHandler                ; 20|   4| 0x50| see shasta.h
        DCD Default_IRQHandler              ; 21|   5| 0x54| Reserved
        DCD Default_IRQHandler              ; 22|   6| 0x58| Reserved
        DCD Default_IRQHandler              ; 23|   7| 0x5C| Reserved
        DCD Default_IRQHandler              ; 24|   8| 0x60| Reserved
        DCD Default_IRQHandler              ; 25|   9| 0x64| Reserved
        DCD Default_IRQHandler              ; 26|  10| 0x68| Reserved
        DCD Default_IRQHandler              ; 27|  11| 0x6C| Reserved
        DCD WatchDog_PreHandler             ; 28|  12| 0x70| Reserved     WDT handler
        DCD Default_IRQHandler              ; 29|  13| 0x74| Reserved
        DCD Default_IRQHandler              ; 30|  14| 0x78| Reserved
        DCD Default_IRQHandler              ; 31|  15| 0x7C| Reserved
        DCD Default_IRQHandler              ; 32|  16| 0x80| Reserved
        DCD Default_IRQHandler              ; 33|  17| 0x84| see shasta.h
        DCD Default_IRQHandler              ; 34|  18| 0x88| Reserved
        DCD Default_IRQHandler              ; 35|  19| 0x8C| Reserved
        DCD Default_IRQHandler              ; 36|  20| 0x90| Reserved
        DCD Default_IRQHandler              ; 37|  21| 0x94| Reserved
        DCD Default_IRQHandler              ; 38|  22| 0x98| Reserved
        DCD Default_IRQHandler              ; 39|  23| 0x9C| Reserved
        DCD Default_IRQHandler              ; 40|  24| 0xA0| Reserved
        DCD Default_IRQHandler              ; 41|  25| 0xA4| Reserved
        DCD Default_IRQHandler              ; 42|  26| 0xA8| Reserved
        DCD Default_IRQHandler              ; 43|  27| 0xAC| Reserved
        DCD Default_IRQHandler              ; 44|  28| 0xB0| Reserved
        DCD Default_IRQHandler              ; 45|  29| 0xB4| Reserved
        DCD Default_IRQHandler              ; 46|  30| 0xB8| Reserved
        DCD Default_IRQHandler              ; 47|  31| 0xBC| Reserved

; Symbols for the size of the vector table (useful if no specific size
; function exists for the linker)
__Vectors_End
__Vectors_Size EQU __Vectors_End - __Vectors
; =============================================================================

; for the rom image, we need another section in addition to VECTORS_ROM which
; holds the interrupt vectors in RAM. RAM_IMAGE does not need it, it has
; VECTORS_RAM.
#if IMAGE_TYPE == ROM_IMAGE
; =============================================================================
; Define empty data region to mirror the Cortex-M0 vectors in RAM
    AREA VECTORS_RAM, NOINIT, READWRITE

    ; Allocate uninitialized memory
    SPACE __Vectors_Size
; =============================================================================
#endif

; =============================================================================
; Section of system exception and IRQ handlers
    AREA |.text|, CODE, READONLY

; Reset is invoked on power up or a warm reset. The exception
; model treats reset as a special form of exception. When reset is
; asserted, the operation of the processor stops, potentially at any
; point in an instruction. When reset is deasserted, execution
; restarts from the address provided by the reset entry in the vector
; table. Execution restarts in Thread mode.
Reset_Handler PROC
    ENTRY ; Entry point for the application. This prevents the necessity of --entry linker param.
    EXPORT Reset_Handler
    IMPORT SystemInit
    IMPORT __main
        LDR R0, =SystemInit
        BLX R0
        LDR R0, =__main
        BX  R0
    ENDP

; -----------------------------------------------------------------------------
; System Exception Handlers
; -----------------------------------------------------------------------------
; Note: Only default routines are defined here. The exports are weak because
; the handlers are intended to be overwritten elsewhere.

; An NMI can be signalled by a peripheral or triggered by software.
; This is the highest priority exception other than reset. It is
; permanently enabled and has a fixed priority of -2. NMIs cannot be:
; * masked or prevented from activation by any other exception
; * preempted by any exception other than Reset.
NMI_Handler PROC
    EXPORT NMI_Handler [WEAK]
        LDR R0, =Default_IRQHandler
        BX R0
    ENDP

; A HardFault is an exception that occurs because of an error during
; normal or exception processing. HardFaults have a fixed priority
; of -1, meaning they have higher priority than any exception with
; configurable priority.
HardFault_PreHandler PROC
    ; Bit 2 of r14 shows us whether the main stack (boot sequence, OS kernel) or the process stack
    ; (thread level) was in use when the hard fault hit us.
    ; So, decide which one tells where we find the thread context that
    ; was saved when the hard fault happened.
    ; See also http://community.arm.com/thread/5414
    ; Note: Checking the LR in assembly instead of C is necessary since the armcc does not create
    ; the correct assembly code for reading out the LR.
    ; "register uint32_t link_register __asm("lr");" should produce "MOV rX, lr" but it does not.

    ;Hard coded register address from shasta_hal_mmu.h
MMU_ERR_RPT    EQU 0x40004704
OTP_SH_ERR_RPT EQU 0x50020070
    ;Hard coded register address from shasta_hal_otp.h
OTP_ACC_ERR    EQU 0x1
OTP_WRT_ERR    EQU 0x2
    ; Stack return of offset for the PC, see "The Definitive Guide to the ARM Cortex-M0" pg. 214
SP_OFFS     EQU 0x18
    ; We are advancing the PC by one instruction which is 2 bytes. otherwise the hardfault
    ; is generated again
IS_OFFS     EQU 2

    EXPORT HardFault_PreHandler
    IMPORT HardFault_Handler
    IMPORT MMU_Hardfault_IRQHandler
    IMPORT OTP_Hardfault_IRQHandler
        ; Check MMU Error status
        ; If a protection violation occurred execute the MMU_Hardfault Handler
        ; Otherwise move to OTP access check
        LDR     R0, =MMU_ERR_RPT
        LDR     R0,[R0, #0]
        TST     R0,R0
        BEQ     HardFault_OTP_Error
        LDR     R0,[SP, #SP_OFFS]
        ADDS    R0, #IS_OFFS
        STR     R0,[SP, #SP_OFFS]
        ; Link register needs to be saved before BL instruction if we want to return to the
        ; origin of the modified address where the Hardfault occured to continue operation
        PUSH    {LR}
        BL      MMU_Hardfault_IRQHandler
        POP     {PC}
HardFault_OTP_Error
        LDR     R0, =OTP_SH_ERR_RPT
        LDR     R0,[R0, #0]
        MOVS    R1, #OTP_WRT_ERR
        TST     R0, R1
        BEQ     HardFault_OTP_Access_Error
        ; From OTP write error we want to skip the access because it is accidental.
        ; And we want to continue operation as for MMU protection
        ; Link register needs to be saved before BL instruction if we want to return to the
        ; origin of the modified address where the Hardfault occured to continue operation
        LDR     R0,[SP, #SP_OFFS]
        ADDS    R0, #IS_OFFS
        STR     R0,[SP, #SP_OFFS]
        PUSH    {LR}
        BL      OTP_Hardfault_IRQHandler
        POP     {PC}
HardFault_OTP_Access_Error
        ; From OTP access error we want to return to the same position.
        ; It's like while looping waiting until the write to OTP is finished
        ; Link register needs to be saved before BL instruction if we want to return to the
        ; origin of the modified address where the Hardfault occured to continue operation
        LDR     R0, =OTP_SH_ERR_RPT
        LDR     R0,[R0, #0]
        MOVS    R1, #OTP_ACC_ERR
        TST     R0, R1
        BEQ     HardFault_Other_Errors
        PUSH    {LR}
        BL      OTP_Hardfault_IRQHandler
        POP     {PC}
HardFault_Other_Errors
        MOVS    r0, #0x4
        MOV     r1, lr
        TST     r1, r0
        BEQ     HardFault_Choose_MSP
        MRS     R0, psp
        BL      HardFault_Handler
        B . ; todo replace infinite loop. In case reset not happens
HardFault_Choose_MSP
        MRS     R0, msp
        BL      HardFault_Handler
        B . ; todo replace infinite loop. In case reset not happens
    ENDP

; A supervisor call (SVC) is an exception that is triggered by the
; SVC instruction. In an OS environment, applications can use SVC
; instructions to access OS kernel functions and device drivers.
SVC_Handler PROC
    EXPORT SVC_Handler [WEAK]
        B . ; todo replace infinite loop
    ENDP

; PendSV is an interrupt-driven request for system-level service. In
; an OS environment, use PendSV for context switching when no
; other exception is active.
PendSV_Handler PROC
    EXPORT PendSV_Handler [WEAK]
        B . ; todo replace infinite loop
    ENDP

; The dummy PMBus interrupt Handler
PMBus_IRQHandler PROC
    EXPORT PMBus_IRQHandler [WEAK]
        B . ; todo replace infinite loop
    ENDP
; If the device implements the SysTick timer, a SysTick exception
; is an exception the system timer generates when it reaches zero.
; Software can also generate a SysTick exception. In an OS
; environment, the device can use this exception as system tick.
SysTick_Handler PROC
    EXPORT SysTick_Handler [WEAK]
        B . ; todo replace infinite loop
    ENDP

; Jump to the default handler in case no watchdog timer is available in the image.
WatchDog_IRQHandler PROC
    EXPORT WatchDog_IRQHandler [WEAK]
        LDR R0, =Default_IRQHandler
        BX R0
    ENDP

; The Debug Entry allows to stop the debugger in idle daemon and execute
; an arbitrary function
DebugEntry PROC
    EXPORT DebugEntry
        NOP
        BX LR
    ENDP
; -----------------------------------------------------------------------------
; IRQ Handlers
; -----------------------------------------------------------------------------
; An interrupt request, or IRQ, is an exception signalled by a peripheral, or
; generated by a software request. All interrupts are asynchronous
; to instruction execution. In the system, peripherals use interrupts
; to communicate with the processor.
;
; Note: Only default routines are defined here. The exports are weak because
; the handlers are intended to be overwritten elsewhere.
; You need to provide the weak definition, if not, the
; assembler wil complain EVEN IF a 'strong' definition is available
; somewhere in the code. That is, you can't remove the following
; lines, the assembler requires them. The linker then will remove
; these lines and replace them with the 'strong' code, e.g.,
; PMBus_IRQHandler() in i2c.c ...

WatchDog_PreHandler PROC
    ; Bit 2 of r14 shows us whether the main stack (boot sequence, OS kernel) or the process stack
    ; (thread level) was in use when the hard fault hit us.
    ; So, decide which one tells where we find the thread context that
    ; was saved when the hard fault happened.
    ; Its the same procedure as for the HardFault_PreHandler.
    EXPORT WatchDog_PreHandler
        MOVS    r0, #0x4
        MOV     r1, lr
        TST     r1, r0
        BEQ     WatchDog_Choose_MSP
        MRS     R0, psp
        PUSH    {lr}  ; save return address
        BL      WatchDog_IRQHandler
        POP     {pc}  ; return to where it is interrupted from
WatchDog_Choose_MSP
        MRS     R0, msp
        PUSH    {lr}  ; save return address
        BL      WatchDog_IRQHandler
        POP     {pc}  ; return to where it is interrupted from
    ENDP

    ALIGN ; (copied from template)

; =============================================================================

    ; End of the source file
    END
