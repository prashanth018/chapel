/* include/qthread/common.h.  Generated from common.h.in by configure.  */
/*  -*- Mode: C -*-
 * ------------------------------------------------------------------
 * DO NOT EDIT THIS FILE!  It has been automatically generated.
 * ------------------------------------------------------------------
 */

#ifndef QTHREAD_COMMON_H
#define QTHREAD_COMMON_H

/* Whether C compiler supports GCC style inline assembly */
#define HAVE_GCC_INLINE_ASSEMBLY 1

/* if the compiler supports inline assembly, we can prevent reordering */
#define COMPILER_FENCE __asm__ __volatile__ ("":::"memory")

/* Architecture type of assembly to use */
#define QTHREAD_ASSEMBLY_ARCH QTHREAD_AMD64

/* use mutexes when incrementing, rather than architecture-specific assembly
   */
/* #undef QTHREAD_MUTEX_INCREMENT */

/* use inlined functions */
/* #undef QTHREAD_INLINE */

/* if the compiler supports __attribute__((deprecated)) */
#define Q_DEPRECATED __attribute__((deprecated))

/* Allow functions to be inlined */
#define QINLINE inline

/* Define if bitfields are in forward order */
/* #undef BITFIELD_ORDER_FORWARD */

/* Define if bitfields are in reverse order */
#define BITFIELD_ORDER_REVERSE 1

/* The size of `void*', as computed by sizeof. */
#define SIZEOF_VOIDP 8

/* alignment of aligned_t */
#define QTHREAD_ALIGNMENT_ALIGNED_T 8

/* size of aligned_t */
#define QTHREAD_SIZEOF_ALIGNED_T 8

/* builtin cas supported */
#define QTHREAD_ATOMIC_CAS 1

/* if the compiler supports __sync_val_compare_and_swap on 32-bit ints */
#define QTHREAD_ATOMIC_CAS32 1

/* if the compiler supports __sync_val_compare_and_swap on 64-bit ints */
#define QTHREAD_ATOMIC_CAS64 1

/* if the compiler supports __sync_val_compare_and_swap on pointers */
#define QTHREAD_ATOMIC_CAS_PTR 1

/* builtin incr supported */
#define QTHREAD_ATOMIC_INCR 1

/* ia64intrin.h available */
/* #undef HAVE_IA64INTRIN_H */

/* if ia64intrin is needed */
/* #undef QTHREAD_NEEDS_IA64INTRIN */

/* specifying data alignment is allowed */
#define QTHREAD_ALIGNEDDATA_ALLOWED 1

/* if the compiler supports __attribute__((malloc)) */
#define Q_MALLOC __attribute__((malloc))

/* if the compiler supports __attribute__((unused)) */
#define Q_UNUSED __attribute__((unused))

/* Last resort, if no function name macros can be found */
/* #undef __FUNCTION__ */

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#ifndef restrict
#define restrict __restrict
#endif
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif

/* If __builtin_trap can be used */
#define QTHREAD_TRAP_OKAY 1

#ifdef QTHREAD_TRAP_OKAY
# define QTHREAD_TRAP() __builtin_trap()
#else
# define QTHREAD_TRAP() *(volatile int *)0 = 0
#endif

/* Work around another bug in Sun C++: it does not support __asm__, even though
 * their C compiler DOES. */
#if defined(__SUNPRO_CC)
# define __asm__ asm
# define __volatile__ volatile
#endif

/* Architecture defines */
#define QTHREAD_UNSUPPORTED 0
#define QTHREAD_IA32        1
#define QTHREAD_AMD64       2
#define QTHREAD_IA64        3
#define QTHREAD_ALPHA       4
#define QTHREAD_MIPS        5
#define QTHREAD_POWERPC32   6
#define QTHREAD_POWERPC64   7
#define QTHREAD_SPARCV9_32  8
#define QTHREAD_SPARCV9_64  9
#define QTHREAD_TILEPRO	    10
#define QTHREAD_TILEGX	    11

#endif
