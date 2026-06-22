#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: LMP88959’s implementation of an NTSC filter.
 */

#include "LSNDx9NtscCrtFullFilter.h"
#include "../GPU/DirectX9/LSNDirectX9DiskInclude.h"
#include "../Utilities/LSNScopedNoSubnormals.h"
#include "NTSC-CRT-Full/crt_core.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

#define m_nsSettings				(*reinterpret_cast<NTSC_SETTINGS *>(m_vSettings.data()))
#define m_nnCrtNtsc					(*reinterpret_cast<CRT *>(m_vCrtNtsc.data()))

namespace lsn {

	// == Members.
	CDx9NtscCrtFullFilter::CDx9NtscCrtFullFilter() :
		m_ui32FinalStride( 0 ) {
		int iPhases[4] = { 0, 16, 0, -16 };
		std::memcpy( m_iPhaseRef, iPhases, sizeof( iPhases ) );

		m_vSettings.resize( sizeof( NTSC_SETTINGS ) );
		m_vCrtNtsc.resize( sizeof( CRT ) );

		m_rsResampler.SetFilter( CResamplerBase::LSN_FF_LINEAR );
		SetPhosphorDecayLevel( 0.15f );
		SetPhosphorDecayPeriod( 1.79113161563873291015625f / 7.0f );
	}
	CDx9NtscCrtFullFilter::~CDx9NtscCrtFullFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx9NtscCrtFullFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_tuUploader.Reset();
		ReleaseBaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		constexpr uint32_t ui32Scale = 1;
		m_ui32FinalWidth = CRT_HRES * ui32Scale;
		m_ui32FinalHeight = _ui16Height * ui32Scale;
		m_ui32FinalStride = RowStride( m_ui32FinalWidth, OutputBits() );
		m_vRgbBuffer.resize( m_ui32FinalStride * m_ui32FinalHeight );

		::crt_init_full( &m_nnCrtNtsc, m_ui32FinalWidth, m_ui32FinalHeight, CRT_PIX_FORMAT_BGRA, m_vRgbBuffer.data() );
		
		m_nnCrtNtsc.hue = 20;
		m_nnCrtNtsc.brightness = 4;
		m_nnCrtNtsc.contrast = 180;
		m_nnCrtNtsc.saturation = 17;
		m_nnCrtNtsc.black_point = 4;
		m_nnCrtNtsc.white_point = 75;
		m_nnCrtNtsc.blend = 0;
		m_nnCrtNtsc.do_vsync = true;
		m_nnCrtNtsc.do_hsync = true;

		m_nsSettings.border_color = 0x22;

		return pofOut;
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
	 * \param _i32DispLeft The display area left.
	 * \param _i32DispTop The display area top.
	 * \param _ui32DispWidth The display area width.
	 * \param _ui32DispHeight The display area height
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CDx9NtscCrtFullFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		if LSN_UNLIKELY( !Device().GetDirectX9Device() ) { return m_vBasicRenderTarget[0].data(); }
		if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
			m_ui32SrcW = _ui32Width;
			m_ui32SrcH = _ui32Height;
			EnsureSizeAndResources();
		}
		
		lsw::LSW_RECT rRect;
		rRect.left = LONG( _i32DispLeft );
		rRect.top = LONG( _i32DispTop );
		rRect.right = rRect.left + LONG( _ui32DispWidth );
		rRect.bottom = rRect.top + LONG( _ui32DispHeight );

		const uint32_t ui32Pitch = m_ui32FinalStride;
		m_vRgbBuffer.resize( ui32Pitch * m_ui32SrcH );

		m_nsSettings.data = reinterpret_cast<unsigned short *>(_pui8Input);
		m_nsSettings.w = int( m_ui32OutputWidth );
		m_nsSettings.h = int( m_ui32OutputHeight );
		m_nsSettings.dot_crawl_offset = _ui64RenderStartCycle % 3;

		::crt_modulate_full( &m_nnCrtNtsc, &m_nsSettings );
		::crt_demodulate_full( &m_nnCrtNtsc, 3 );

		m_tuUploader.UploadTexels( &Device(), m_vRgbBuffer.data(), m_ui32FinalWidth, m_ui32FinalHeight, ui32Pitch, D3DFMT_X8R8G8B8 );

		Render( rRect );

		_ui32Width = uint32_t( s_dgsState.rScreenRect.Width() );
		_ui32Height = uint32_t( s_dgsState.rScreenRect.Height() );
		_ui32Stride = _ui32Width * sizeof( uint32_t );
		return m_vBasicRenderTarget[0].data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx9NtscCrtFullFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9NtscCrtFullFilter::DeActivate() {
		CParent::DeActivate();

		m_tuUploader.Reset();
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CDx9NtscCrtFullFilter::FrameResize() {
		s_dgsState.OnSizeDx9();
		EnsureSizeAndResources();
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * \return Returns true on success.
	 */
	bool CDx9NtscCrtFullFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx9dDevice ) {
			if ( !s_dgsState.CreateDx9() ) { return false; }
			m_pdx9dDevice = &s_dgsState.dx9Device;
			m_tuUploader.Reset();
		}
		if ( !m_pdx9dDevice ) { return false; }

		m_bValidState = true;
		return true;
	}

	/**
	 * Renders the final output to the backbuffer.
	 *
	 * \param _rOutput The destination rectangle.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx9NtscCrtFullFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_tuUploader.GetTexture() || !m_tuUploader.GetTexture()->Valid() ) { return false; }
		return RenderBase( &Device(), m_tuUploader.GetTexture()->Get(), m_ui32FinalWidth, m_ui32FinalHeight, _rOutput, false );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9

#undef m_nsSettings
#undef m_nnCrtNtsc
