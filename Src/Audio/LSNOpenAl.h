/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles global OpenAL functionality.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#if defined( LSN_IOS ) || defined( LSN_MAC )		// TODO: Fix these macros.
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include "al.h"
#include "alc.h"
#endif	// #ifdef LSE_IPHONE

namespace lsn {

	/**
	 * Class CCpu6502
	 * \brief Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 *
	 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 */
	class COpenAl {
	public :
	};

}	// namespace lsn
