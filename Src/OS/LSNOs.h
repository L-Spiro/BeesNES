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
    #define LSN_ALIGN( n ) 						alignas( n )//__declspec( align( n ) )
	#define	LSN_FALLTHROUGH						[[fallthrough]];

	#define LSN_FORCEINLINE 					__forceinline
	#define LSN_PREFETCH_LINE( ADDR )			_mm_prefetch( reinterpret_cast<const char *>(ADDR), _MM_HINT_T0 );
    #define LSN_LIKELY( x )						( x ) [[likely]]
    #define LSN_UNLIKELY( x )					( x ) [[unlikely]]
	#define LSN_STDCALL							__stdcall
#elif defined( __GNUC__ ) || defined( __clang__ )
    // GNU Compiler Collection (GCC) or Clang
    #define LSN_ALIGN( n ) 						alignas( n )//__attribute__( (aligned( n )) )
	#define	LSN_FALLTHROUGH

	#define LSN_FORCEINLINE 					__inline__ __attribute__( (__always_inline__) )
	#define LSN_PREFETCH_LINE( ADDR )			__builtin_prefetch( reinterpret_cast<const void *>(ADDR), 1, 1 );
    #define LSN_LIKELY( x )						( __builtin_expect( !!(x), 1 ) )
    #define LSN_UNLIKELY( x )					( __builtin_expect( !!(x), 0 ) )
    #define __assume( x )
	#define LSN_STDCALL
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

#if defined( _MSC_VER )
#define bswap_16( X )							_byteswap_ushort( X )
#define bswap_32( X )							_byteswap_ulong( X )
#define bswap_64( X )							_byteswap_uint64( X )
#elif defined( __APPLE__ )
// Mac OS X/Darwin features.
#include <libkern/OSByteOrder.h>
#define bswap_16( X )							OSSwapInt16( X )
#define bswap_32( X )							OSSwapInt32( X )
#define bswap_64( X )							OSSwapInt64( X )
#elif defined( __sun ) || defined( sun )

#include <sys/byteorder.h>
#define bswap_16( X )							BSWAP_16( X )
#define bswap_32( X )							BSWAP_32( X )
#define bswap_64( X )							BSWAP_64( X )

#elif defined( __FreeBSD__ )

#include <sys/endian.h>
#define bswap_16( X )							bswap16( X )
#define bswap_32( X )							bswap32( X )
#define bswap_64( X )							bswap64( X )

#elif defined( __OpenBSD__ )

#include <sys/types.h>
#define bswap_16( X )							swap16( X )
#define bswap_32( X )							swap32( X )
#define bswap_64( X )							swap64( X )

#elif defined( __NetBSD__ )

#include <sys/types.h>
#include <machine/bswap.h>
#if defined( __BSWAP_RENAME ) && !defined( __bswap_32 )
#define bswap_16( X )							bswap16( X )
#define bswap_32( X )							bswap32( X )
#define bswap_64( X )							bswap64( X )
#endif

#else
inline uint16_t                                 bswap_16( uint16_t _ui16Val ) { return (_ui16Val >> 8) | (_ui16Val << 8); }
inline unsigned long                            bswap_32( unsigned long _ui32Val ) { return uint32_t( (uint32_t( _ui32Val ) >> 24) |
	((_ui32Val >> 8) & 0x0000FF00) |
	((_ui32Val << 8) & 0x00FF0000) |
	(_ui32Val << 24) ); }
inline uint64_t                                 bswap_64( uint64_t _ui64Val ) { return (_ui64Val >> 56) |
	((_ui64Val >> 40) & 0x000000000000FF00ULL) |
	((_ui64Val >> 24) & 0x0000000000FF0000ULL) |
	((_ui64Val >> 8) & 0x00000000FF000000ULL) |
	((_ui64Val << 8) & 0x000000FF00000000ULL) |
	((_ui64Val << 24) & 0x0000FF0000000000ULL) |
	((_ui64Val << 40) & 0x00FF000000000000ULL) |
	(_ui64Val << 56); }
#endif	// #if defined( _MSC_VER )
