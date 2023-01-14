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

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;

		const WORD wBitDepth = WORD( OutputBits() );
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

		return InputFormat();
	}

	/**
	 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.  The final buffer, along with
	 *	its width, height, bit-depth, and stride, are returned.
	 *
	 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.  Its format will be that returned in InputFormat().
	 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
	 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
	 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
	 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
	 * \param _ui64PpuFrame The PPU frame associated with the input data.
	 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CRgb24Filter::ApplyFilter( uint8_t * _pui8Input, uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/ ) {
		// This is a pass-through filter.
		return _pui8Input;
	}

}	// namespace lsn
