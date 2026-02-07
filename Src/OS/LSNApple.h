/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: macOS/iOS/tvOS macros and header.
 */

#pragma once

#ifdef __APPLE__

#include <stdexcept>

#define LSN_APPLE

#ifdef __GNUC__
#include <math.h>
#endif	// #ifdef __GNUC__

#if defined( LSN_METAL )
#define LSN_GPU_SUPPORT
#endif	// #if defined( LSN_METAL )


#ifdef __GNUC__

#ifndef sincos
#define sincos								__sincos
#endif	// #ifndef sincos

#endif	// #ifdef __GNUC__

#endif  // #ifdef __APPLE__
