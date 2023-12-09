/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 1-pole filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioFilterBase.h"

namespace lsn {

	/**
	 * Class CPoleFilter
	 * \brief A 1-pole filter.
	 *
	 * Description: A 1-pole filter.
	 */
	class CPoleFilter : public CAudioFilterBase {
	public :
		inline CPoleFilter();
		virtual ~CPoleFilter() {}

	};

}	// namespace lsn
