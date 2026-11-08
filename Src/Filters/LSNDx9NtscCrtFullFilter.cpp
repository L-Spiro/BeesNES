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
		// The convolution filters this filter looks best with (the options normally replace these).
		SetConvolutionFilter( LSN_CB_LOW, CResamplerBase::LSN_FF_ROBIDOUXSHARP );
		SetConvolutionFilter( LSN_CB_MEDIUM, CResamplerBase::LSN_FF_ROBIDOUXSOFT );
		SetConvolutionFilter( LSN_CB_HIGH, CResamplerBase::LSN_FF_LINEAR );
		SetConvolutionFilter( LSN_CB_MAX, CResamplerBase::LSN_FF_TOTAL );
		int iPhases[4] = { 0, 16, 0, -16 };
		std::memcpy( m_iPhaseRef, iPhases, sizeof( iPhases ) );

		m_vSettings.resize( sizeof( NTSC_SETTINGS ) );
		m_vCrtNtsc.resize( sizeof( CRT ) );

		m_rsResampler.SetFilter( CResamplerBase::LSN_FF_LINEAR );
	}
	CDx9NtscCrtFullFilter::~CDx9NtscCrtFullFilter() {
	}

	// == Functions.
	/**
	 * Sets a hue offset that is added to the filter's own hue (a color space's hue rotation; see CColorSpace::HueOffset()).
	 *
	 * \param _fHueOffset The hue offset, in radians.  Positive values rotate the demodulation axes forward.
	 */
	void CDx9NtscCrtFullFilter::SetHueOffset( float _fHueOffset ) {
		m_fHueOffset = _fHueOffset;
		// The CRT structure only exists after Init(), which applies m_fHueOffset itself.
		if ( m_vCrtNtsc.size() >= sizeof( CRT ) ) {
			// The CRT hue is in degrees.
			m_nnCrtNtsc.hue_offset = double( _fHueOffset ) * 180.0 / std::numbers::pi;
		}
	}

	/**
	 * Sets the user-adjustable settings of the filter.  They are applied immediately and again whenever the filter is initialized.
	 *
	 * \param _lsSettings The settings to apply.
	 */
	void CDx9NtscCrtFullFilter::SetSettings( const LSN_LMP88959_SETTINGS &_lsSettings ) {
		m_lsSettings = _lsSettings;
		ApplySettings();
	}

	/**
	 * Copies m_lsSettings into the CRT structures.
	 */
	void CDx9NtscCrtFullFilter::ApplySettings() {
		m_nnCrtNtsc.hue = m_lsSettings.i32Hue;
		m_nnCrtNtsc.do_vsync = m_lsSettings.bVSync;
		m_nnCrtNtsc.do_hsync = m_lsSettings.bHSync;
		m_nnCrtNtsc.brightness = m_lsSettings.i32Brightness;
		m_nnCrtNtsc.contrast = m_lsSettings.i32Contrast;
		m_nnCrtNtsc.saturation = m_lsSettings.i32Saturation;
		m_nnCrtNtsc.black_point = m_lsSettings.i32BlackPoint;
		m_nnCrtNtsc.white_point = m_lsSettings.i32WhitePoint;
		m_nnCrtNtsc.blend = m_lsSettings.bBlend ? 1 : 0;
		m_nsSettings.border_color = m_lsSettings.ui32BorderColor;
	}

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
		
		m_nnCrtNtsc.hue_offset = double( m_fHueOffset ) * 180.0 / std::numbers::pi;
		ApplySettings();

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
		::crt_demodulate_full( &m_nnCrtNtsc, m_lsSettings.i32Noise );

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
		m_tuUploader.Reset();
		CParent::DeActivate();
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
