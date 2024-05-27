/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for textures.
 */


#include "LSNTextureBase.h"

namespace lsn {

	CTextureBase::CTextureBase() {
	}
	CTextureBase::~CTextureBase() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the object instance back to scratch, ready to be reused.
	 **/
	void CTextureBase::Reset() {
		m_sArraySize = 0;
		m_sFaces = 0;
		for ( auto I = m_vMipMaps.size(); I--; ) {
			m_vMipMaps[I]->clear();
			m_vMipMaps[I].reset();
		}
		m_vMipMaps = std::vector<std::unique_ptr<CSurface>>();
	}

}	// namespace lsn
