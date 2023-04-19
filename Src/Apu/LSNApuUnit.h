/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The primary base class for each APU unit (pulse, noise, etc.)
 */


#pragma once

#include "../LSNLSpiroNes.h"


namespace lsn {

	/**
	 * Class CApuUnit
	 * \brief The primary base class for each APU unit (pulse, noise, etc.)
	 *
	 * Description: The primary base class for each APU unit (pulse, noise, etc.)
	 */
	class CApuUnit {
	public :
		CApuUnit() {
		}
		virtual ~CApuUnit() {
		}

	};

}	// namespace lsn
