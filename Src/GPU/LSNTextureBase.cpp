/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for textures.
 */


#include "LSNTextureBase.h"

#include <algorithm>

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
		m_ui32MemoryPool = 0;
		m_ui32Usage = 0;
		m_ui32Format = 0;
		for ( auto I = m_vMipMaps.size(); I--; ) {
			if ( m_vMipMaps[I].get() ) {
				m_vMipMaps[I]->clear();
				m_vMipMaps[I].reset();
			}
		}
		m_vMipMaps = std::vector<std::unique_ptr<CSurface>>();
	}

	/**
	 * Allocates surfaces.
	 * 
	 * \param _ui32W Surface width.
	 * \param _ui32H Surface height.
	 * \param _ui32D Surface depth.
	 * \param _sMips Total mipmaps.
	 * \param _sFaces Total faces.
	 * \param _sArray Total array surfaces.
	 * \param _ui32Pool Texture pool.
	 * \param _ui32Usage Texture usage.
	 * \param _ui32Format Texture format.
	 * \return Returns true if the allocation was successful.
	 **/
	bool CTextureBase::Alloc( uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, size_t _sMips, size_t _sFaces, size_t _sArray,
		uint32_t _ui32Pool, uint32_t _ui32Usage, uint32_t _ui32Format ) {
		try {
			m_vMipMaps.resize( _sMips );
			for ( size_t I = 0; I < _sMips; ++I ) {
				uint32_t ui32W = std::max( _ui32W >> I, 1U );
				uint32_t ui32H = std::max( _ui32H >> I, 1U );
				uint32_t ui32D = std::max( _ui32D >> I, 1U );
				m_vMipMaps[I] = std::make_unique<CSurface>();
				if ( !m_vMipMaps[I].get() ) { return false; }
				m_vMipMaps[I]->SetSurface( ui32W, ui32H, ui32D );
			}
			m_sArraySize = _sArray;
			m_sFaces = _sFaces;
			m_ui32MemoryPool = _ui32Pool;
			m_ui32Usage = _ui32Usage;
			m_ui32Format = _ui32Format;
			return true;
		}
		catch ( ... ) { return false; }
	}

}	// namespace lsn
