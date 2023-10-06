/* setjmp.h: ANSI 'C' (X3J11 Oct 88) library header, section 4.6 */

/* Copyright (C) ARM Ltd., 1999
 * All rights reserved
 * RCS $Revision: 178085 $
 * Checkin $Date: 2012-12-11 14:54:17 +0000 (Tue, 11 Dec 2012) $
 * Revising $Author: drodgman $
 */

/* Copyright (C) Codemist Ltd., 1988                            */
/* Copyright 1991 ARM Limited. All rights reserved.             */


/*
 * setjmp.h declares two functions and one type, for bypassing the normal
 * function call and return discipline (useful for dealing with unusual
 * conditions encountered in a low-level function of a program).
 */

#ifndef __setjmp_h
#define __setjmp_h
#define __ARMCLIB_VERSION 5030076

#define _ARMABI __declspec(__nothrow)
#define _ARMABI_NORETURN __declspec(__nothrow) __declspec(__noreturn)

  #ifndef __SETJMP_DECLS
  #define __SETJMP_DECLS

    #undef __CLIBNS

    #ifdef __cplusplus
      namespace std {
          #define __CLIBNS std::
          extern "C" {
    #else
      #define __CLIBNS
    #endif  /* __cplusplus */

#if _AEABI_PORTABILITY_LEVEL != 0 || (!defined _AEABI_PORTABILITY_LEVEL && __DEFAULT_AEABI_PORTABILITY_LEVEL != 0)
typedef __int64 jmp_buf[];      /* do not specify size at all */
#else
typedef __int64 jmp_buf[7];    /* size as defined in the ATEPCS */
                            /* an array type suitable for holding the data */
                            /* needed to restore a calling environment.    */

/* Regarding the implementation of the jmp_buf the C Library ABI for the ARM Architecture states:
 *
 * In deference to VFP, XScale Wireless MMX, and other co - processors manipulating 8 - byte
 * aligned types, a jmp_buf must be 8 - byte aligned. The minimum jmp_buf size is calculated as
 * follows:
 * - SP, LR: 2x4; reserved to setjmp implementation: 4x4; Total 3x8 bytes
 * - General purpose register save: 8x4; Total 4x8 bytes
 * - Floating point register save: 8x8; Total 8x8 bytes
 * - WMMX (if present): 5x8; Total 5x8 bytes
 * Total: 20x8 = 160 bytes = 20 8 - byte double-words.
 * If WMMX can be guaranteed not to be present this can be reduced to 15x8 = 120 bytes.
 * If floating point hardware can be guaranteed not to be present this can be further reduced
 * to 7x8 = 56 bytes.
 *
 * An implementation may define the size of a jmp_buf to be larger than the ABI-defined minimum
 * size.
 *
 * If code allocates a jmp_buf statically using a compile-time constant size smaller than the
 * "maximum minimum" value of 160 bytes, the size of the jmp_buf becomes part of its interface
 * contract. Portable code is urged not to do this.
 *
 * The link-time constant __aeabi_JMP_BUF_SIZE gives the actual size of a target system jmp_buf
 * measured in 8-byte double-words.
 *
 * When _AEABI_PORTABILITY_LEVEL != 0, the required definition of jmp_buf cannot be used to create
 * jmp_buf objects. Instead, a jmp_buf must be passed as a parameter or allocated dynamically.
 */

#endif
extern const int __aeabi_JMP_BUF_SIZE;

/* setjmp is a macro so that it cannot be used other than directly called. */
/* NB that ANSI declare that anyone who undefines the setjmp macro or uses */
/* (or defines) the name setjmp without including this header will get     */
/* what they deserve.  In addition, C++ requires setjmp to be a macro.     */

extern _ARMABI int setjmp(jmp_buf /*env*/) __attribute__((__nonnull__(1)));
   /* Saves its calling environment in its jmp_buf argument, for later use
    * by the longjmp function.
    * Returns: If the return is from a direct invocation, the setjmp function
    *          returns the value zero. If the return from a call to the longjmp
    *          function, the setjmp function returns a non zero value.
    */
#define setjmp(jmp_buf) (__CLIBNS setjmp(jmp_buf))

extern _ARMABI_NORETURN void longjmp(jmp_buf /*env*/, int /*val*/) __attribute__((__nonnull__(1)));
   /* Restores the environment saved by the most recent call to setjmp in the
    * same invocation of the program, with the corresponding jmp_buf argument.
    * If there has been no such call, or if the function containing the call
    * to setjmp has terminated execution (eg. with a return statement) in the
    * interim, the behaviour is undefined.
    * All accessible objects have values as of the time longjmp was called,
    * except that the values of objects of automatic storage duration that do
    * not have volatile type and have been changed between the setjmp and
    * longjmp calls are indeterminate.
    * As it bypasses the usual function call and return mechanism, the longjmp
    * function shall execute correctly in contexts of interrupts, signals and
    * any of their associated functions. However, if the longjmp function is
    * invoked from a nested signal handler (that is, from a function invoked as
    * a result of a signal raised during the handling of another signal), the
    * behaviour is undefined.
    * Returns: After longjmp is completed, program execution continues as if
    *          the corresponding call to setjmp had just returned the value
    *          specified by val. The longjmp function cannot cause setjmp to
    *          return the value 0; if val is 0, setjmp returns the value 1.
    */

    #ifdef __cplusplus
         }  /* extern "C" */
      }  /* namespace std */
    #endif /* __cplusplus */
  #endif /* __SETJMP_DECLS */

  #if _AEABI_PORTABILITY_LEVEL != 0 && !defined _AEABI_PORTABLE
    #define _AEABI_PORTABLE
  #endif

  #ifdef __cplusplus
    #ifndef __SETJMP_NO_EXPORTS
      using ::std::jmp_buf;
      using ::std::setjmp;
      using ::std::longjmp;
      using ::std::__aeabi_JMP_BUF_SIZE;
    #endif
  #endif /* __cplusplus */

#endif

/* end of setjmp.h */

