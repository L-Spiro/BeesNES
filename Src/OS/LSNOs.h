/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Includes all OS headers.
 */

#pragma once

#if defined( _WIN32 ) || defined( _WIN64 )
#include "LSNWindows.h"
#elif defined( __APPLE__ )
#include "LSNApple.h"
#else
#endif  // #if defined( _WIN32 ) || defined( _WIN64 )

#ifndef LSN_FASTCALL
#define LSN_FASTCALL
#endif	// LSN_FASTCALL


#if defined( _MSC_VER )
    // Microsoft Visual Studio Compiler
    #define LSN_ALIGN( n ) 						__declspec( align( n ) )
#elif defined( __GNUC__ ) || defined( __clang__ )
    // GNU Compiler Collection (GCC) or Clang
    #define LSN_ALIGN( n ) 						__attribute__( (aligned( n )) )
#else
    #error "Unsupported compiler"
#endif


#if defined( _MSC_VER )
	#define LSN_FORCEINLINE 					__forceinline
	#define LSN_PREFETCH_LINE( ADDR )			_mm_prefetch( reinterpret_cast<const char *>(ADDR), _MM_HINT_T0 );
    #define LSN_LIKELY(x) (x)
    #define LSN_UNLIKELY(x) (x)
#elif defined( __GNUC__ ) || defined( __clang__ )
	#define LSN_FORCEINLINE 					__inline__ __attribute__( (__always_inline__) )
	#define LSN_PREFETCH_LINE( ADDR )			__builtin_prefetch( reinterpret_cast<const void *>(ADDR), 1, 1 );
    #define LSN_LIKELY(x)                       __builtin_expect(!!(x), 1)
    #define LSN_UNLIKELY(x)                     __builtin_expect(!!(x), 0)
    #define __assume(x)
#else
	#define LSN_FORCEINLINE inline
#endif

