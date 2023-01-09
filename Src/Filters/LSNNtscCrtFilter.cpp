/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: EMMIR (LMP88959)’s NTSC filter.
 */

#include "LSNNtscCrtFilter.h"
#include <Helpers/LSWHelpers.h>


namespace lsn {

	CNtscCrtFilter::CNtscCrtFilter() :
		m_ui32FinalStride( 0 ) {
		int iPhases[4] = { 0, 1, 0, -1 };
		std::memcpy( m_iPhaseRef, iPhases, sizeof( iPhases ) );
	}
	CNtscCrtFilter::~CNtscCrtFilter() {
	}


	// == Functions.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CNtscCrtFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( m_stStride * _ui16Height );
			for ( auto J = m_vBasicRenderTarget[I].size() / sizeof( uint16_t ); J--; ) {
				(*reinterpret_cast<uint16_t *>(&m_vBasicRenderTarget[I][J*sizeof( uint16_t )])) = 0x0F;
			}
		}

		m_ui32FinalStride = RowStride( _ui16Width, OutputBits() );
		m_vFilteredOutput.resize( m_ui32FinalStride * _ui16Height );
		m_vFinalOutput.resize( m_ui32FinalStride * _ui16Height );

		::crt_init( &m_nnCrtNtsc, _ui16Width, _ui16Height, reinterpret_cast<int *>(m_vFilteredOutput.data()) );

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
	uint8_t * CNtscCrtFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle ) {

		m_nsSettings.data = reinterpret_cast<unsigned short *>(_pui8Input);
		m_nsSettings.w = 256;
		m_nsSettings.h = 240;
		m_nsSettings.dot_crawl_offset = _ui64RenderStartCycle % 3;
		m_nsSettings.as_color = 1;
		constexpr int phase_offset = 2;
		m_nsSettings.cc[0] = m_iPhaseRef[(phase_offset + 0) & 3];
		m_nsSettings.cc[1] = m_iPhaseRef[(phase_offset + 1) & 3];
		m_nsSettings.cc[2] = m_iPhaseRef[(phase_offset + 2) & 3];
		m_nsSettings.cc[3] = m_iPhaseRef[(phase_offset + 3) & 3];
		::crt_nes2ntsc( &m_nnCrtNtsc, &m_nsSettings );
		::crt_draw( &m_nnCrtNtsc, 5 );
		for ( uint32_t Y = _ui32Height; Y--; ) {
			uint32_t ui32SwapWidthMe = (_ui32Height - 1) - Y;
			uint32_t * pui32Src = &reinterpret_cast<uint32_t *>(m_vFilteredOutput.data())[Y*_ui32Width];

			uint32_t * pui32Dst = &reinterpret_cast<uint32_t *>(m_vFinalOutput.data())[ui32SwapWidthMe*_ui32Width];
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				uint32_t ui32Src = pui32Src[X];
				//ui32Src = (ui32Src & 0xFF000000) | ((ui32Src & 0x00FF0000) >> 8) | ((ui32Src & 0x000000FF) << 16) | ((ui32Src & 0x0000FF00) >> 8);
				//ui32Src = (ui32Src & 0xFF000000) | ((255 - ((ui32Src & 0x00FF0000) >> 16)) << 16) | ((255 - ((ui32Src & 0x0000FF00) >> 8)) << 8) | ((255 - ((ui32Src & 0x000000FF) >> 0)) << 0);
				//ui32Src = (ui32Src & 0xFFFFFF00) | ((255 - ((ui32Src & 0x000000FF))));
				pui32Dst[X] = ui32Src;
				//std::swap( pui32Src0[X], pui32Src1[X] );
			}
		}
		return m_vFinalOutput.data();
	}

}	// namespace lsn
