/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Includes all OS headers.
 */

#pragma once

#include "LSNWindows.h"

#ifndef LSN_FASTCALL
#define LSN_FASTCALL
#endif	// LSN_FASTCALL


#if defined( _MSC_VER )
    // Microsoft Visual Studio Compiler
    #define LSN_ALIGN( n ) __declspec( align( n ) )
#elif defined( __GNUC__ ) || defined( __clang__ )
    // GNU Compiler Collection (GCC) or Clang
    #define LSN_ALIGN( n ) __attribute__( (aligned( n )) )
#else
    #error "Unsupported compiler"
#endif