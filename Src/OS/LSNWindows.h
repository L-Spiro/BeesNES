#pragma once

#ifdef LSN_USE_WINDOWS
#include <LSWWin.h>

#pragma comment( lib, "shlwapi.lib" )
#else
/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Windows macros and header.
 */


/*
// Just for reference.
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0  
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000  
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP  
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003  
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista  
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista  
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008  
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista  
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7  
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8  
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1  
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10  
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10  
*/

#define _WIN32_IE							0x0601
#define WINVER								0x0601
#define _WIN32_WINNT						0x0601
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#endif  // #ifdef LSN_USE_WINDOWS

#if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )
#define LSN_WINDOWS
#if defined( WIN32 ) || defined( _WIN32 )
#define LSN_WIN32
#endif	// #ifdef defined( WIN32 ) || defined( _WIN32 )
#ifdef _WIN64
#define LSN_WIN64
#endif	// #ifdef _WIN64
#endif	// #if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )


#ifdef LSN_WINDOWS

#include <cstdint>
#include <immintrin.h>
#include <stdexcept>


#define LSN_FASTCALL						__fastcall

#pragma intrinsic( _BitScanReverse )
#ifdef _WIN64
#pragma intrinsic( _BitScanReverse64 )
#endif  // #ifdef _WIN64


inline unsigned int                             CountLeadingZeros( uint32_t _ui32X ) {
#if defined( _MSC_VER )
    unsigned long ulIndex;
	auto ucIsNonZero = _BitScanReverse( &ulIndex, _ui32X );
	return ucIsNonZero ? (31 - static_cast<int>(ulIndex)) : 32;
#else
    return _ui32X != 0 ? __builtin_clz( _ui32X ) : 32;
#endif  // #if defined( _MSC_VER )
}

inline unsigned int                             CountLeadingZeros( uint64_t _ui64X ) {
#if defined( _MSC_VER )
    #if defined( _WIN64 )
        // Benchmark against (1000000*50) values.
        // _BitScanReverse64(): 0.01772186666666666 seconds
        // Manual version: 0.0004546 seconds. (!!!)
        #if 0
	        unsigned long ulIndex;
	        auto ucIsNonZero = _BitScanReverse64( &ulIndex, _ui64X );
	        return ucIsNonZero ? (63 - static_cast<int>(ulIndex)) : 64;
        #else
            unsigned long uiN = 0;
            if ( _ui64X == 0 ) { return 64; }
            if ( (_ui64X & 0xFFFFFFFF00000000ULL) == 0 ) { uiN += 32; _ui64X <<= 32; }
            if ( (_ui64X & 0xFFFF000000000000ULL) == 0 ) { uiN += 16; _ui64X <<= 16; }
            if ( (_ui64X & 0xFF00000000000000ULL) == 0 ) { uiN += 8;  _ui64X <<= 8; }
            if ( (_ui64X & 0xF000000000000000ULL) == 0 ) { uiN += 4;  _ui64X <<= 4; }
            if ( (_ui64X & 0xC000000000000000ULL) == 0 ) { uiN += 2;  _ui64X <<= 2; }
            if ( (_ui64X & 0x8000000000000000ULL) == 0 ) { uiN += 1; }

            return uiN;
        #endif  // #if 0
    #else
        if ( _ui64X == 0 ) { return 64; }
        unsigned int uiN = 0;

        #if 0
            // Benchmark against (1000000*50) values (x86 on x64 hardware). 
            // 0.0002651
            // 0.0002426333333333333
            if ( (_ui64X & 0xFFFFFFFF00000000ULL) == 0 ) { uiN += 32; _ui64X <<= 32; }
            if ( (_ui64X & 0xFFFF000000000000ULL) == 0 ) { uiN += 16; _ui64X <<= 16; }
            if ( (_ui64X & 0xFF00000000000000ULL) == 0 ) { uiN += 8;  _ui64X <<= 8; }
            if ( (_ui64X & 0xF000000000000000ULL) == 0 ) { uiN += 4;  _ui64X <<= 4; }
            if ( (_ui64X & 0xC000000000000000ULL) == 0 ) { uiN += 2;  _ui64X <<= 2; }
            if ( (_ui64X & 0x8000000000000000ULL) == 0 ) { uiN += 1; }
            return uiN;
        #else
            // Benchmark against (1000000*50) values (x86 on x64 hardware). 
            // 0.0002392 seconds.
            if ( _ui64X & 0xFFFFFFFF00000000ULL )        { uiN += 32; _ui64X >>= 32; }
	        if ( _ui64X & 0xFFFF0000 )                   { uiN += 16; _ui64X >>= 16; }
	        if ( _ui64X & 0xFFFFFF00 )                   { uiN += 8;  _ui64X >>= 8; }
	        if ( _ui64X & 0xFFFFFFF0 )                   { uiN += 4;  _ui64X >>= 4; }
	        if ( _ui64X & 0xFFFFFFFC )                   { uiN += 2;  _ui64X >>= 2; }
	        if ( _ui64X & 0xFFFFFFFE )                   { uiN += 1; }
	        return 63 - uiN;
        #endif  // #if 0
    #endif  // #if defined( _WIN64 )
#elif defined( __GNUC__ )
    return _ui64X != 0 ? __builtin_clzll( _ui64X ) : 64;
#endif  // #if defined( __GNUC__ )
}

#if defined( _MSC_VER )
    #ifdef _WIN64
        #include <intrin.h>
        #pragma intrinsic( _udiv128 )
    #else
        inline uint64_t                         _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
            if ( _ui64Divisor == 0 ) {
		        throw std::overflow_error( "Division by zero is not allowed." );
	        }

	        if ( _ui64High >= _ui64Divisor ) {
		        throw std::overflow_error( "The division would overflow the 64-bit quotient." );
	        }

	        if ( _ui64High == 0 ) {
		        if ( _pui64Remainder ) { (*_pui64Remainder) = _ui64Low % _ui64Divisor; }
		        return _ui64Low / _ui64Divisor;
	        }

	        uint64_t ui64Q = 0;
	        uint64_t ui64R = _ui64High;

	        for ( int I = 63; I >= 0; --I ) {
		        ui64R = (ui64R << 1) | ((_ui64Low >> I) & 1);

		        if ( ui64R >= _ui64Divisor ) {
			        ui64R -= _ui64Divisor;
			        ui64Q |= 1ULL << I;
		        }
	        }

	        if ( _pui64Remainder ) { (*_pui64Remainder) = ui64R; }
	        return ui64Q;
        }
    #endif  // #ifdef _WIN64
#elif defined( __x86_64__ ) || defined( _M_X64 )
    #include <immintrin.h>

    // Implementation using x86_64 assembly for GCC and Clang
    inline uint64_t                             _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
        uint64_t ui64Quot, ui64Rem;

        asm(
            "divq %4"
            : "=a"(ui64Quot), "=d"(ui64Rem)
            : "a"(_ui64Low), "d"(_ui64High), "r"(_ui64Divisor)
        );

        if ( _pui64Remainder ) {
            (*_pui64Remainder) = ui64Rem;
        }
        return ui64Quot;
    }
#elif defined( __SIZEOF_INT128__ )
    // Implementation for compilers that support __uint128_t (e.g., GCC, Clang)
    inline uint64_t                             _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
        __uint128_t ui128Div = ((__uint128_t)_ui64High << 64) | _ui64Low;
        uint64_t ui64Quot = ui128Div / _ui64Divisor;
        if ( _pui64Remainder ) {
            (*_pui64Remainder) = ui128Div % _ui64Divisor;
        }
        return ui64Quot;
    }
#endif  // #if defined( _MSC_VER )

#ifndef LSN_WIN64
// Mercilessly ripped from: https://stackoverflow.com/a/46924301
// Still need to find somewhere from which to mercilessly rip a _udiv128() implementation.  MERCILESSLY.
#include <intrin.h>
#include <stdexcept>
//#include <winnt.h>
inline uint64_t LSN_FASTCALL                    _umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, 
    uint64_t *_pui64ProductHi ) {
#if 0
    return UnsignedMultiply128( _ui64Multiplier, _ui64Multiplicand, _pui64ProductHi );
#else
    // _ui64Multiplier   = ab = a * 2^32 + b
    // _ui64Multiplicand = cd = c * 2^32 + d
    // ab * cd = a * c * 2^64 + (a * d + b * c) * 2^32 + b * d
    uint64_t ui64A = _ui64Multiplier >> 32;
    uint64_t ui64B = static_cast<uint32_t>(_ui64Multiplier);
    uint64_t ui64C = _ui64Multiplicand >> 32;
    uint64_t ui64D = static_cast<uint32_t>(_ui64Multiplicand);

    uint64_t ui64Ad = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64D) );
    uint64_t ui64Bd = __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64D) );

    uint64_t ui64Abdc = ui64Ad + __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64C) );
    uint64_t ui64AbdcCarry = (ui64Abdc < ui64Ad);

    // _ui64Multiplier * _ui64Multiplicand = _pui64ProductHi * 2^64 + ui64ProductLo
    uint64_t ui64ProductLo = ui64Bd + (ui64Abdc << 32);
    uint64_t ui64ProductLoCarry = (ui64ProductLo < ui64Bd);
    (*_pui64ProductHi) = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64C) ) + (ui64Abdc >> 32) + (ui64AbdcCarry << 32) + ui64ProductLoCarry;

    return ui64ProductLo;
#endif  // 1
}

#endif  // #ifndef LSN_WIN64

extern "C" {

#if defined( _M_AMD64 )
extern void sincos( double _dAngle, double * _pdSin, double * _pdCos );
extern void sincosf( float _fAngle, float * _pfSin, float * _pfCos );
#else

// 32 bit implementation in inline assembly
inline void                                     sincos( double _dAngle, double * _pdSin, double * _pdCos ) {
	double dSin, dCos;
	__asm {
		fld QWORD PTR[_dAngle]
		fsincos
		fstp QWORD PTR[dCos]
		fstp QWORD PTR[dSin]
		fwait
	}
	(*_pdSin) = dSin;
	(*_pdCos) = dCos;
}

inline void                                     sincosf( float _fAngle, float * _pfSin, float * _pfCos ) {
    float fSinT, fCosT;
    __asm {
        fld DWORD PTR[_fAngle]		// Load the 32-bit float into the FPU stack.
        fsincos						// Compute cosine and sine.
        fstp DWORD PTR[fCosT]		// Store the cosine value.
        fstp DWORD PTR[fSinT]		// Store the sine value.
        fwait						// Wait for the FPU to finish.
    }
    (*_pfSin) = fSinT;
    (*_pfCos) = fCosT;
}

#endif

}	// extern "C"

#endif	// #ifdef LSN_WINDOWS
