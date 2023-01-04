/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Blargg’s NTSC filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNFilterBase.h"
#include "nes_ntsc/nes_ntsc.h"


namespace lsn {

	/**
	 * Class CNtscBlarggFilter
	 * \brief Blargg’s NTSC filter.
	 *
	 * Description: Blargg’s NTSC filter.
	 */
	class CNtscBlarggFilter : public CFilterBase {
	public :
		CNtscBlarggFilter();
		virtual ~CNtscBlarggFilter();


	protected :
		// == Members.
		/** The Blargg NTSC emulation. */
		nes_ntsc_t								m_nnBlarggNtsc;
	};

}	// namespace lsn
