/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for all filters.
 */

#include "LSNFilterBase.h"


namespace lsn {

	CFilterBase::CFilterBase() :
		m_stBufferIdx( 0 ),
		m_stStride( 0 ) {
	}
	CFilterBase::~CFilterBase() {
	}

}	// namespace lsn
