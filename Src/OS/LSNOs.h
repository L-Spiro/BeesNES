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

#include <pthread.h>
#else
#endif  // #if defined( _WIN32 ) || defined( _WIN64 )

#ifndef LSN_FASTCALL
#define LSN_FASTCALL
#endif	// LSN_FASTCALL


#if defined( _MSC_VER )
    // Microsoft Visual Studio Compiler
    #define LSN_ALIGN( n ) 						__declspec( align( n ) )
	#define	LSN_FALLTHROUGH						[[fallthrough]];

	#define LSN_FORCEINLINE 					__forceinline
	#define LSN_PREFETCH_LINE( ADDR )			_mm_prefetch( reinterpret_cast<const char *>(ADDR), _MM_HINT_T0 );
    #define LSN_LIKELY( x )						( x ) [[likely]]
    #define LSN_UNLIKELY( x )					( x ) [[unlikely]]
#elif defined( __GNUC__ ) || defined( __clang__ )
    // GNU Compiler Collection (GCC) or Clang
    #define LSN_ALIGN( n ) 						__attribute__( (aligned( n )) )
	#define	LSN_FALLTHROUGH

	#define LSN_FORCEINLINE 					__inline__ __attribute__( (__always_inline__) )
	#define LSN_PREFETCH_LINE( ADDR )			__builtin_prefetch( reinterpret_cast<const void *>(ADDR), 1, 1 );
    #define LSN_LIKELY( x )						( __builtin_expect( !!(x), 1 ) )
    #define LSN_UNLIKELY( x )					( __builtin_expect( !!(x), 0 ) )
    #define __assume( x )
#else
	#define LSN_FORCEINLINE						inline
    #error "Unsupported compiler"
#endif


#ifdef LSN_WINDOWS
inline void SetThreadHighPriority() {
    ::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_HIGHEST );
}
inline void SetThreadNormalPriority() {
    ::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_NORMAL );
}
#else
inline void SetThreadHighPriority() {
    sched_param spSchParms;
    spSchParms.sched_priority = ::sched_get_priority_max( SCHED_FIFO );
    ::pthread_setschedparam( ::pthread_self(), SCHED_FIFO, &spSchParms );
}
void SetThreadNormalPriority() {
    sched_param spSchParms;
    spSchParms.sched_priority = 0;  // Normal priority
    ::pthread_setschedparam( ::pthread_self(), SCHED_OTHER, &spSchParms );
}
#endif  // LSN_WINDOWS

inline void SetThreadAffinity( size_t sCoreId ) {
#ifdef LSN_WINDOWS
	// Set thread affinity to the specified core on Windows
	DWORD_PTR dwptrMask = DWORD_PTR( 1 ) << sCoreId;
	::SetThreadAffinityMask( ::GetCurrentThread(), dwptrMask );
#elif defined( LSN_APPLE ) || defined( __linux__ )
	// Set thread affinity on Linux
	cpu_set_t cpuset;
	CPU_ZERO( &cpuset );
	CPU_SET( sCoreId, &cpuset );
	::pthread_setaffinity_np( ::pthread_self(), sizeof( cpu_set_t ), &cpuset );
#endif
}