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
#define _WIN32_WINNT_NT4						0x0400 // Windows NT 4.0  
#define _WIN32_WINNT_WIN2K						0x0500 // Windows 2000  
#define _WIN32_WINNT_WINXP						0x0501 // Windows XP  
#define _WIN32_WINNT_WS03						0x0502 // Windows Server 2003  
#define _WIN32_WINNT_WIN6						0x0600 // Windows Vista  
#define _WIN32_WINNT_VISTA						0x0600 // Windows Vista  
#define _WIN32_WINNT_WS08						0x0600 // Windows Server 2008  
#define _WIN32_WINNT_LONGHORN					0x0600 // Windows Vista  
#define _WIN32_WINNT_WIN7						0x0601 // Windows 7  
#define _WIN32_WINNT_WIN8						0x0602 // Windows 8  
#define _WIN32_WINNT_WINBLUE					0x0603 // Windows 8.1  
#define _WIN32_WINNT_WINTHRESHOLD				0x0A00 // Windows 10  
#define _WIN32_WINNT_WIN10						0x0A00 // Windows 10  
*/

#define _WIN32_IE								0x0601
#define WINVER									0x0601
#define _WIN32_WINNT							0x0601
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


#define LSN_FASTCALL							__fastcall


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
