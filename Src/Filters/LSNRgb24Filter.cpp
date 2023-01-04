/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard 24-bit RGB filter.
 */

#include "LSNRgb24Filter.h"
#include <Helpers/LSWHelpers.h>

using namespace lsw;


namespace lsn {

	CRgb24Filter::CRgb24Filter() {
	}
	CRgb24Filter::~CRgb24Filter() {
	}

	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CRgb24Filter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

		const WORD wBitDepth = 24;
		const DWORD dwStride = DWORD( m_stStride = RowStride( _ui16Width, wBitDepth ) );
		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			size_t szPrevSize = m_vBasicRenderTarget[I].size();
			uint8_t * pui8PrevData = m_vBasicRenderTarget[I].size() ? m_vBasicRenderTarget[I].data() : nullptr;
			m_vBasicRenderTarget[I].resize( dwStride * _ui16Height );
			
			if ( szPrevSize != m_vBasicRenderTarget[I].size() || pui8PrevData != m_vBasicRenderTarget[I].data() ) {
				// For fun.
				uint8_t * pui8Pixels = m_vBasicRenderTarget[I].data();
				for ( auto Y = _ui16Height; Y--; ) {
					for ( auto X = _ui16Width; X--; ) {
						uint8_t * pui8This = &pui8Pixels[Y*dwStride+X*3];
						/*pui8This[2] = uint8_t( CHelpers::LinearTosRGB( X / 255.0 ) * 255.0 );
						pui8This[1] = uint8_t( CHelpers::LinearTosRGB( Y / 255.0 ) * 255.0 );*/
						pui8This[2] = uint8_t( CHelpers::sRGBtoLinear( X / 255.0 ) * 255.0 );
						pui8This[0] = uint8_t( CHelpers::LinearTosRGB( Y / 255.0 ) * 255.0 );
					}
				}
			}
		}

		return CDisplayClient::LSN_POF_RGB;
	}

	/**
	 * Swaps to the next render target.
	 */
	void CRgb24Filter::Swap() {
		m_stBufferIdx = (m_stBufferIdx + 1) % m_vBasicRenderTarget.size();
	}

}	// namespace lsn
