#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for GPU filters.
 */

#include "LSNDx9FilterBase.h"
#include "../Utilities/LSNUtilities.h"


namespace lsn {

	CDx9FilterBase::CDx9FilterBase() {
	}
	CDx9FilterBase::~CDx9FilterBase() {
	}

	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx9FilterBase::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;

		const uint16_t wBitDepth = uint16_t( OutputBits() );
		const uint32_t dwStride = uint32_t( m_stStride = RowStride( _ui16Width, wBitDepth ) );
		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			size_t szPrevSize = m_vBasicRenderTarget[I].size();
			m_vBasicRenderTarget[I].resize( dwStride * _ui16Height );	
		}

		return InputFormat();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx9FilterBase::Activate() {
	}
		
	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9FilterBase::DeActivate() {
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
