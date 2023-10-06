/* alloca.h: Stack based allocation */
/* Copyright 1999 ARM Limited. All rights reserved */

/*
 * RCS $Revision: 178085 $
 * Checkin $Date: 2012-12-11 14:54:17 +0000 (Tue, 11 Dec 2012) $
 * Revising $Author: drodgman $
 */

#ifndef __alloca_h
#define __alloca_h
#define __ARMCLIB_VERSION 5030076

  #ifndef __ALLOCA_DECLS
  #define __ALLOCA_DECLS

    #ifdef __cplusplus
      namespace std {
          extern "C" {
    #endif  /* __cplusplus */

#if defined(__cplusplus) || !defined(__STRICT_ANSI__)
 /* unconditional in C++ and non-strict C for consistency of debug info */
  typedef unsigned int size_t;
#elif !defined(__size_t)
  #define __size_t 1
  typedef unsigned int size_t;   /* see <stddef.h> */
#endif

#undef NULL
#define NULL 0                  /* see <stddef.h> */

#define alloca(n) (__builtin_alloca(n))

/* Allocate a block of at least size bytes. Returns a pointer to the first
 * word, or NULL if the allocation isn't successful.
 * Deallocation of all blocks allocated in a function is done on return.
 * Alloca can not be used as a function pointer.
 */

    #ifdef __cplusplus
          }  /* extern "C" */
      }  /* namespace std */
    #endif /* __cplusplus */
  #endif /* __ALLOCA_DECLS */

  #ifdef __cplusplus
    #ifndef __ALLOCA_NO_EXPORTS
      using ::std::size_t;
    #endif
  #endif /*__cplusplus*/
#endif

/* end of alloca.h */

