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
 * @file    cmsis_os_exceptions.h
 * @brief   Library for using a simple Structured Exception Handling technique in
 *          an CMSIS-RTOS environment with multiple threads.
 */

#ifndef CMSIS_OS_EXCEPTIONS_H
#define CMSIS_OS_EXCEPTIONS_H

/** @page pgLib_StructExc   Structured Exception Handling in C
 *
 * Structured Exception Handling is a technique that separates the code for exception handling
 * from the normal application code. Instead of checking the returned success or failure value
 * after each function call and acting upon it, the program flow directly jumps to the exception handler
 * after the exception has been "thrown".<br>
 * The exception itself contains all the essential information for identifying and handling it.
 * This might be just an exception ID or additional information like a stack trace.<br>
 * In case you are totally unfamiliar with the concept of throwing exceptions and want to
 * convince yourself why using this technique is a good idea or you just need to refresh your
 * memory, read, for instance, the documentation of some development frameworks like
 * <a href="http://msdn.microsoft.com/en-us/library/ms229014%28v=vs.110%29.aspx">MSDN - Design Guidelines for Exceptions</a>
 * or have a look at <a href="http://en.wikipedia.org/wiki/Exception_handling">Wikipedia: Exception Handling</a>.
 *
 * @section sctLib_StructExc_Usage   Example Usage
 * To integrate the simple Structured Exception Handling technique into a CMSIS-RTOS environment
 * simply make use of the macros in cmsis_os_exceptions.h which is included in cmsis_os_ext.h as
 * shown in this example:
 *
@code
#include "cmsis_os_ext.h"   // The exception library is an extension of a CMSIS-RTOS.
#include "fw_bool.h"        // boolean type

// RTOS definition of threads
osThreadDef(main, osPriorityNormal, 1, 0);
osThreadDef(module_thread, osPriorityNormal, 1, 0);

// Maximum for parameter "index" of risky_function.
#define MAX_SIZE   10

// Demonstrates that throwing exceptions bypasses the function call chain.
// @throws ILLEGAL_PARAMETER from risky_function.
void intermediate_function(uint8_t num);

// Function on the lowest level which can throw exceptions.
// @param array     An array with maximum size MAX_SIZE
// @param num       Number which should not exceed MAX_SIZE-1
// @return a sum over @p num elements of @p array.
// @throws ILLEGAL_PARAMETER if
//         - parameter == NULL
//         - num >= MAX_SIZE
uint32_t risky_function(uint8_t* array, uint8_t num);

// Initializes the RTOS and threads
void __main(void)
{
    osExtKernelInitialize();
    osThreadId main_thread_id = osExtThreadCreate(osThread(main), NULL);
    osThreadId module_thread_id = osExtThreadCreate(osThread(module_thread), NULL);
    osKernelStart();
}

// ------------------------------------------------------------------------------------------------

// main() is an example that
// - sets up the top-level exception guard using the TRY macro
// - illustrates how to catch specific and remaining exceptions from functions called at main() level,
// using CATCH and CATCH_OTHERS
// - illustrates how to close the exception guard using the TRY_END macro
int main(void)
{
    // Set up exception guard (TRY macro) to catch any exception potentially thrown by
    // intermediate_function() and its childs.
    TRY
    {
        intermediate_function(MAX_SIZE);
        // Stuff that should be done if there was no exception.
        // Do NOT use "return" here because the TRY_END macro which removes the last guard would not be executed.
    }
    CATCH(ILLEGAL_PARAMETER)
    {
        // Handle specific exception.
        // Do NOT use "return" here because the TRY_END macro which removes the last guard would not be executed.
    }
    CATCH_OTHERS
    {
        // Handle all other exceptions.
        // Do NOT use "return" here because the TRY_END macro which removes the last guard would not be executed.
    }
    TRY_END
}

// Does some stuff, but does not throw an exception.
void intermediate_function(uint8_t num)
{
    uint8_t array[MAX_SIZE];
    // Initialize array and do other stuff.
    // ...

    uint32_t value = risky_function(array, num);

    // Do more stuff.
    // ...
}

// Does some stuff, too and is called by intermediate_function().
// Throws an exception which is then handled on main() level by
// 'unwinding' the call stack and passing the exception data back
// to where it is actually handled.
uint32_t risky_function(uint8_t* array, uint8_t num)
{
    // First check for illegal parameters.
    THROW_IF(parameter == NULL, ILLEGAL_PARAMETER);
    THROW_IF(index >= MAX_SIZE, ILLEGAL_PARAMETER);

    uint32_t sum = 0;
    for (uint8_t i = 0; i < index; i++)
    {
        sum += parameter[i];
    }
    return sum;
}

// ------------------------------------------------------------------------------------------------

// module_thread() is an example that
// - sets up the thread-level exception guard using the TRY_AGAIN macro for a thread that
// runs forever under RTX control
// - illustrates how to catch specific and remaining exceptions from functions called by the
// RTX-controlled module_thread() using CATCH and CATCH_OTHERS
// - illustrates how to close the exception guard using the TRY_END macro
void module_thread(const void* thread_input)
{
    for (;;) // ever (set up a new guard in case of a previous exception)
    {
        TRY_AGAIN // until an exception occurs
        {
            module_thread_function();
        }
        CATCH_OTHERS
        {
            // handle all exceptions. Access an exception with EXCEPTION
            // EXCEPTION.type, ...
        }
        TRY_END
    }
}

// module_thread_function(), called by module_thread()
// module_thread_function() casts one exception into another.
// @throws FAILED_COMMUNICATION for demonstration purpose.
void module_thread_function(void)
{
    TRY
    {
        intermediate_function(MAX_SIZE);
    }
    CATCH(ILLEGAL_PARAMETER)
    {
        // Do stuff like logging the exception.

        // Throw an exception of another type.
        // Note: This type does not apply here. It is just used for demonstration purpose.
        THROW_OTHER(FAILED_COMMUNICATION);
    }
    FINALLY
    {
        // FINALLY is always executed:
        // - in case the TRY block did not throw anything
        // - CATCH was entered and the exception was handled,
        // - CATCH was entered and THROW_OTHER was used
        // - a thrown exception was not caught due to a missing CATCH statement
        // Remember to NOT use "return" here so that TRY_END is executed.
    }
    TRY_END
}
@endcode
 *
 *
 * @section sctLib_StructExc_Implementation   Implementation
 *
 * The basic idea is presented in
 * <a href="http://www.on-time.com/ddj0011.htm">Exception Handling in C without C++</a>.
 * The goal was to provide a simple exception-handling library in C with the following requirements:
 * - No dynamic memory allocation.
 * - Robust (the exception handling library itself must not fail).
 * - Must support both exception-handlers and finally-handlers.
 * - Reentrant for multitasking applications.
 *
 * That idea was paired with
 * <a href="http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html">Exceptions in C with Longjmp and Setjmp</a>
 * which provided proposals for macros that look like the try-throw-catch statements from C++.<br>
 * @note <b>Edit</b>: The same approach was used and even further improved in
 * <a href="http://www.vishia.org/Jc/html/Exception_Jc.html">CRuntimeJavalike</a> (german).
 *
 * These two combined ideas plus some additional work resulted in an exception handling mechanism
 * whose functionality is nearly the same as the one in C++. The only difference is that
 * it is not allowed to use a @c return statement within the TRY, CATCH or FINALLY block.
 * The reason for this is that there are no destructors in C which could execute the TRY_END macro
 * for cleaning up the whole TRY block before the @c return statement takes effect.
 *
 * @par How it works
 *
 * The elementary functions of this Structured Exception Handling implementation
 * are setjmp() and longjmp() from the ANSI C library (header file <code>setjmp.h</code>).<br>
 * The basic procedure is to
 * -# set up a guard of type @c jmp_buf before entering a <em>risky function</em>. @em Risky means
 *    that the function contains a path along its sub-functions that might throw an exception.
 *    The guard is taken by the function @c setjmp to save the values of all processor registers
 *    (including stack pointer and program counter).
 * -# Then the risky function is executed. In case the function throws an exception on any level
 *    along its call chain the function @c longjmp uses the guard to restore the environment to the
 *    point in time when the guard was set up with @c setjmp. Additionally @c longjmp passes the
 *    exception ID to @c setjmp.
 * -# This exception ID is then returned by @c setjmp and taken as the selector for an appropriate
 *    exception handling routine.
 * .
 * This procedure has been enhanced with the possibilities
 * - to add additional information to an exception besides its ID (see Exception_t).
 * - to nest TRY blocks within one another and within any number of sub-functions.
 * - to use TRY blocks within multiple threads of an RTOS.
 * .
 * For the last two points a linked list of references to locally created guards was added to the context
 * of each thread. Each TRY macro adds a guard and each TRY_END removes it if it was not already
 * removed by a THROW.
 * The program flow for the TRY, CATCH, THROW macros is shown in the following image:
 * @image html cmsis_os_exceptions.svg "State flow chart of the implemented Structured Exception Handling macros"
 *
 * @par Adjustment of setjmp.h
 *
 * Since the environment that has to be saved is dependent on the processor, the size of the guard
 * or rather of the type @c jmp_buf had to be adjusted. For that purpose the sections 4.2.3.2 and
 * 5.11 of the document
 * <a href="http://infocenter.arm.com/help/topic/com.arm.doc.ihi0039c/IHI0039C_clibabi.pdf">ARM C Library ABI</a>
 * were considered.<br>
 * It reads:<br>
 * <em>"
 * In deference to VFP, XScale Wireless MMX, and other co - processors manipulating 8 - byte
 * aligned types, a jmp_buf must be 8 - byte aligned. The minimum jmp_buf size is calculated as
 * follows:
 * - SP, LR: 2x4; reserved to setjmp implementation: 4x4; Total 3x8 bytes
 * - General purpose register save: 8x4; Total 4x8 bytes
 * - Floating point register save: 8x8; Total 8x8 bytes
 * - WMMX (if present): 5x8; Total 5x8 bytes
 * .
 * Total: 20x8 = 160 bytes = 20 8 - byte double-words.<br>
 * If WMMX can be guaranteed not to be present this can be reduced to 15x8 = 120 bytes.<br>
 * If floating point hardware can be guaranteed not to be present this can be further reduced
 * to 7x8 = 56 bytes.
 * "</em>
 *
 * For this implementation @c jmp_buf was set to the minimum of 7x8 = 56 bytes:
@code
typedef __int64 jmp_buf[7];
@endcode
 *
 *
 * @par Timing
 *
 * On an ARM Cortex-M0 with a clock frequency of 100MHz the following runtime values were measured:
 *
 * TRY macro: Setup of a TRY block.
 * - before the first instruction within the block is executed: <b>3.14us</b> at thread-level and <b>1.76us</b> at ISR-level
 * - internal osKernelRunning(): <b>0.46us</b> at thread-level and <b>0.36us</b> at ISR-level
 * - internal osThreadGetId: <b>1.57us</b> at thread-level and <b>0.28us</b> at ISR-level
 * - internal setjmp: <b>0.40us</b>
 *
 * THROW macro: Throwing an exception
 * - before the first instruction within the corresponding CATCH block is executed: <b>4.18us</b> at thread-level and <b>2.79us</b> at ISR-level
 * - internal osKernelRunning(): <b>0.46us</b> at thread-level and <b>0.36us</b> at ISR-level
 * - internal osThreadGetId: <b>1.57us</b> at thread-level and <b>0.28us</b> at ISR-level
 * - internal longjmp: <b>0.38us</b>
 *
 * @par Memory footprint
 *
 * - Stack usage: <b>80 bytes</b> per TRY block (THROW is a function call)
 * - Code size: <b>178+200 bytes</b> for the code in cmsis_os_exceptions.c and cmsis_os_ext.c
 * - Data objects: <b>24 + 20*OS_TASKCNT bytes</b> for @c osException and @c osExtension
 *
 *
 * @section sctLib_StructExc_Comparison   Comparison to usual exception handling
 * @par Costs
 *
 * - <b>Runtime overhead for exceptions</b> <br>
 *   In case of an exception by simply returning the failure value and checking it afterwards costs
 *   0.10us per level of the function call chain (on an ARM Cortex-M0 running on 100MHz)
 *   whereas a THROW needs 4.18us until it reaches its corresponding CATCH (optimization possible,
 *   see section @ref sctLib_StructExc_Implementation - Timing).
 * - <b>Possible misuse</b> <br>
 *   A developer could use the direct jumps to control the normal flow of program
 *   execution similar to @c gotos or he could build up too many exception guards at every level
 *   along the function call chain (-> increase of runtime and stack usage).<br>
 *   Using the jumps is intended for exception handling only and most of the time one or two
 *   exception guards per thread are sufficient. As soon as the developer is aware of these facts,
 *   the misuse disadvantage is negligible.
 *
 * @par Benefits
 *
 * - <b>Runtime reduction for normal execution</b> <br>
 *   Most of the time no exception will occur during the program execution. So the longer the function
 *   call chain gets the more intermediate checks for a failure value plus the corresponding return
 *   statements are saved.
 * - <b>Maintainability of code</b> <br>
 *   The code for exception handling is separated from the application code and thus can be
 *   centralized which greatly improves the readability of the overall code.
 * - <b>No ignoration of exceptions</b> <br>
 *   In case of exception handling by returning failure values the programmer might forget to
 *   check this value along the call chain. Even a static code analysis tool could overlook the
 *   mistake in case that a function returns both success and failure values and only the success
 *   value is used.
 *
 * For more technical information on this topic you may
 * - have a look at
 *   <a href="http://www.embedded.com/electronics-blogs/programming-pointers/4026007/Bail-return-jump-or----throw-">Bail, return, jump, or . . . throw?</a>
 * - or also google words like "embedded exception handling setjmp".
 */

/* Data types */
#include "cmsis_os.h"
#include "linked_list.h"    // Linked_Node_t

/* APIs */
#include <setjmp.h>         // used within the macros


/* ============================================================================
 * Public data types
 * ========================================================================= */

/** The error classes are indicating the original cause of an exception.
 * @note A detected error during runtime is defined as an exception. The exception is usually
 * then used to interrupt the normal program flow.
 * @note The classes are sorted by their estimated likelihood to occur. */
typedef enum
{
    /** The return value of setjmp in case no exception was thrown due to an error
     * (= setjmp was not reached through longjmp). */
    NO_ERROR = 0,

    /** <ul>
     *  <li>A function parameter is outside its permitted range. Using it could lead to a null pointer
     *  access or the access of a wrong memory area like an array index that is out of bounds or
     *  a non-existent function.</li>
     *  </ul> */
    ILLEGAL_PARAMETER,

    /** <ul>
     *  <li>An event (interrupt, change of an interface value) did not occur during a given time frame
     *  or number of polling attempts.</li>
     *  </ul> */
    TIMEOUT,

    /** <ul>
     *  <li>During the attempt to allocate a HW resource like a block of memory or a special function
     *      peripheral the dedicated FW module detects that there is no resource remaining.</li>
     *  <li>A stack overflow was detected for the currently running task (CMSIS: OS_ERR_STK_OVF).</li>
     *  <li>The pending ISR calls (new IRQ, semaphore, queues) are exceeded (CMSIS: OS_ERR_FIFO_OVF).</li>
     *  <li>A mailbox (not message box) overflow was detected (CMSIS: OS_ERR_MBX_OVF).</li>
     *  </ul> */
    OUT_OF_RESOURCE,

    /** <ul>
     *  <li>The protocol of a communication was violated (e.g. wrong direction, wrong number of
     *      data bytes).</li>
     *  <li>The redundancy check of the transferred data has failed.</li>
     *  </ul> */
    FAILED_COMMUNICATION,

    /** <ul>
     *  <li>A redundancy check has failed for the data stored in any kind of memory.</li>
     *  </ul> */
    CORRUPT_MEMORY,

    /** <ul>
     *  <li>All errors that cannot be detected by software fall into this class.</li>
     *  </ul> */
    SYSTEM_FAULT

} osError_t;

/** Data object of an exception. The data is collected just before the exception is thrown
 * and is used for an appropriate handling within the catch routine. */
typedef struct
{
    /** ID indicating the type of the error which is used to determine its handling */
    osError_t type;

    /** Value of the program counter when a THROW was executed */
    uint32_t program_counter;

} osException_t;

/** Data object for catching exceptions */
typedef struct
{
    /** Stack of try-block guards. A guard contains the environment information
     * (processor registers) to restore in case of an exception. */
    Linked_List_t exc_guard_stack;

    /** The last exception that has occurred */
    osException_t last_exception;

} osExceptionCatcher_t;


/* ============================================================================
 * API functions
 * ========================================================================= */
/*lint -esym(755, THROW*) Info 755: global macro not referenced */
/* These macros are exported as part of the exception library. Only part of these macros might be used.*/

/** Throws an exception. The macro may be used everywhere since no action is taken if there is no
 * TRY-CATCH-block to guard the THROW or even if the thread was not registered.<br>
 * Exceptions can only be handled in code that is guarded by a TRY-CATCH within a registered thread.
 * Note: PROGRAM_COUNTER is compiler specific and has to be defined elsewhere. */
#define THROW(exc_type) \
    osExcThrow((osError_t)exc_type, PROGRAM_COUNTER)

/** Readability macro for a single-line conditional THROW which puts the intention to throw an
 * exception at the beginning of the line. */
#define THROW_IF(cond, exc_type) \
    if (cond) THROW(exc_type)

/** Used within CATCH for changing the exception and throwing it further up.<br>
 * Note that the unintended use of THROW instead of THROW_OTHER within CATCH would bypass
 * the FINALLY block. */
#define THROW_OTHER(exc_type) \
    uncaught_exception = boolean_true; /* Although exception was caught throw another one after FINALLY */ \
    exc_catcher->last_exception.type = exc_type;

/**
 * Sets up a try-guard in which one can execute a function that might throw an exception.
 * A minimum top-level TRY block looks like this:
@code
TRY
    something_risky();
CATCH_OTHERS
    handle_exceptions();
TRY_END
@endcode */
/*lint -emacro(586,TRY) Suppress: 'setjmp' is deprecated. There is no alternative. */
/*lint -emacro(578,TRY) Suppress Declaration of symbol 'guard' hides symbol 'guard'. This is ok
 * since TRY_END releases the hiding. The same applies to 'exc_catcher'. */
#define TRY \
{ \
    jmp_buf guard; \
    Linked_Node_t guard_node = {(void*) guard, NULL, NULL}; \
    osExceptionCatcher_t* exc_catcher = osExcGuardSet(&guard_node); \
    exc_catcher->last_exception.type = (osError_t) setjmp(guard); \
    boolean uncaught_exception = (exc_catcher->last_exception.type != NO_ERROR); \
    switch(exc_catcher->last_exception.type) \
    { \
        case NO_ERROR: /* normal operation */ \
        for(;;) /* following breaks exit this loop -> default is used after each case */ \
        {

/**
 * This macro is needed for the use case of an efficient TRY block within a thread's forever loop.
 * The utilization looks like this:
@code
for (;;)
{
    TRY_AGAIN
        something_risky();
    CATCH_OTHERS
        handle_exceptions();
    TRY_END
}
@endcode
 * <br>
 * Usually one would think to use the following code:
@code
for (;;)
{
    TRY
        something_risky();
    CATCH_OTHERS
        handle_exceptions();
    TRY_END
}
@endcode
 * This has the disadvantage that the time costly setup of a TRY block would be done in each
 * iteration. So an inner loop is needed which will be exited in case of an exception.
 * The thread will then continue with the outer loop. With this construction the TRY block does
 * not have to be set up for each iteration.<br>
 * So then why not use these lines?:
@code
for (;;)
{
    TRY
        for (;;)
        {
            something_risky();
        }
    CATCH_OTHERS
        handle_exceptions();
    TRY_END
}
@endcode
 * In this case the compiler will throw a warning for unreachable code of the CATCH(_OTHERS),
 * FINALLY and TRY_END macros. This warning is unnecessary since longjmp within THROW will exit
 * the inner forever loop in the TRY block.<br>
 * To inhibit the warning a variable in the forever loop of TRY_AGAIN is used. This will also
 * prevent the compiler from removing the "unreachable code sections" in a possible optimization
 * step. */
#define TRY_AGAIN \
    /* Loop until an exception occurs. */ \
    TRY for (uint8_t always_true = 1; always_true; )

/** Block in which a specific exception is caught and handled. */
/*lint -emacro(725,CATCH) Suppress: Expected positive indentation from TRY. The "loop break" confuses lint. */
/*lint -emacro(646,CATCH) Suppress: case/default within for loop; may have been misplaced -> No, it is correct. */
#define CATCH(exc_type) \
            break; \
        case exc_type: \
            uncaught_exception = boolean_false;

/** Instead of using FINALLY all other exceptions can be caught with this macro.
 * CATCH_OTHERS is intended to definitely handle the exception and not to THROW it further. */
/*lint -emacro(725,CATCH_OTHERS) Suppress: Expected positive indentation from TRY. The "loop-break" confuses lint. */
/*lint -emacro(838,CATCH_OTHERS) Suppress: Previously assigned value to variable 'uncaught_exception' has not been used. */
#define CATCH_OTHERS \
            break; \
        } /* end of for(;;) */ \
        break; /* break statement to prevent fall-through into default from each case */ \
        default: \
            uncaught_exception = boolean_false;

/** Instructions in this block are always executed before an uncaught exception is thrown further up. */
/*lint -emacro(725,FINALLY) Suppress: Expected positive indentation from TRY. The "loop-break" confuses lint. */
#define FINALLY \
            break; \
        } /* end of for(;;) */ \
        /* no break statement for fall-through into default from each case */ \
        /*lint -fallthrough */ \
        default:

/** Closes the guarded try-block by removing the guard or throwing
 * an uncaught exception further up. */
/*lint -emacro(725,TRY_END) Suppress: Expected positive indentation from TRY. The "loop-break" confuses lint. */
#define TRY_END \
            break; \
    } /* end of switch */ \
    /*lint -e(774) Boolean within 'if' always evaluates to False . */ \
    if (exc_catcher->last_exception.type == NO_ERROR) \
    { \
        /*lint -e(534) Suppress "Ignoring return value". The returned guard is only used within THROW. */ \
        osExcLastGuardPop(exc_catcher); \
    } \
    /*lint -e(774) Boolean within 'if' always evaluates to False . */ \
    else if (uncaught_exception) \
    { \
        THROW(exc_catcher->last_exception.type); \
    } \
    else /* (There was an exception && it was caught) */ \
    { \
        /* At this point last_exception.type is always unequal to NO_ERROR. For the TRY block
         * above it must look like as if there was no exception and that its TRY_END must remove
         * its guard. */ \
        exc_catcher->last_exception.type = NO_ERROR; \
    } \
}

/** The data structure of the last exception that was caught within the current TRY block.
 * Contains the type and additional information (see Exception_t). */
#define EXCEPTION   exc_catcher->last_exception

/**
 * Initializes the Structured Exception Handling to use it within an RTOS environment.
 * Call this function with a NULL parameter in case this library needs to be utilized
 * before osKernelStart() and before the memory has been initialized.
 * @param exc_catcher_get  Callback function which defines how the exception catchers of the
 * threads can be accessed from the thread context
 */
void osExcInitialize(osExceptionCatcher_t* (*exc_catcher_get)(const osThreadId thread_id));

/* ----------------------------------------------------------------------------
 * Functions that are called within the macros
 * ------------------------------------------------------------------------- */

/**
 * Implicitly used within TRY macro (so not intended for explicit use!):
 * Links a new guard (the back-jump address in case of an exception)
 * to the exception catcher for the current thread.
 * No dynamic memory allocation will be used here:
 * A node containing the guard is expected by this function since the data
 * for exception handling has to be created on the stack of the thread.
 * @param guard_node    Linked list node containing the guard (will be modified)
 * @return Data object for managing the exception handling.
 */
osExceptionCatcher_t* osExcGuardSet(Linked_Node_t* guard_node);

/**
 * Implicitly used within THROW and TRY_END macro (so not intended for explicit use!):
 * Unlinks the last guard of the current thread from the Exception FW Module and returns it.
 * @param exc_catcher   Exception catcher of the current thread
 * @return The last guard of the indicated thread or NULL if the thread
 *         was not registered for exception handling.
 */
jmp_buf* osExcLastGuardPop(osExceptionCatcher_t* exc_catcher);

/**
 * Implicitly used within THROW macro (so not intended for explicit use!):
 * Attaches additional information to the specified exception and throws it to the last guard
 * of the current thread.
 * @param type              ID indicating the type of the exception
 * @param program_counter   Value of the PC register when the exception was thrown
 */
/*lint -function(exit,osExcThrow)
 * Copy the attributes of exit() to osExcThrow(). This effectively suppresses all warnings that
 * might arise if the execution of a function would proceed after an osExcThrow().
 * For instance
 *      if (pointer == NULL) osExcThrow(EXCEPTION);
 *      pointer->access_me = value;
 * does not produce "Warning 613: Possible use of null pointer".
 */
void osExcThrow(osError_t type, uint32_t program_counter);

#endif /* CMSIS_OS_EXCEPTIONS_H */
