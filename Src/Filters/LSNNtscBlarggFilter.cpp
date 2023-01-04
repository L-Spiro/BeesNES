/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Blargg’s NTSC filter.
 */

#include "LSNNtscBlarggFilter.h"


namespace lsn {

	CNtscBlarggFilter::CNtscBlarggFilter() {
		::nes_ntsc_init( &m_nnBlarggNtsc, &nes_ntsc_composite );
	}
	CNtscBlarggFilter::~CNtscBlarggFilter() {
	}

}	// namespace lsn
