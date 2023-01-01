#pragma once

#if defined( LSN_IOS ) || defined( LSN_MAC )		// TODO: Fix these macros.
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include "al.h"
#include "alc.h"
#endif	// #ifdef LSE_IPHONE
