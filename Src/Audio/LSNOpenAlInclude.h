#pragma once

#if defined( LSN_IOS ) || defined( LSN_MAC )	// TODO: Fix these macros.
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include "al.h"
#include "alc.h"
#endif	// #ifdef LSE_IPHONE

#ifndef AL_EXT_float32
#define AL_EXT_float32 1
#define AL_FORMAT_MONO_FLOAT32                   0x10010
#define AL_FORMAT_STEREO_FLOAT32                 0x10011
#endif