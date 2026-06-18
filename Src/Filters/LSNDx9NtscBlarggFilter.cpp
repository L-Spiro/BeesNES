#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Blargg’s implementation of an NTSC filter.
 */

#include "LSNDx9NtscBlarggFilter.h"
#include "../GPU/DirectX9/LSNDirectX9DiskInclude.h"
#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

namespace lsn {

	// == Members.
	CDx9NtscBlarggFilter::CDx9NtscBlarggFilter() :
		m_ui32FinalStride( 0 ) {
		nes_ntsc_setup_t nsTmp = nes_ntsc_composite;
		nsTmp.artifacts = 0.62;
		nsTmp.bleed = 0.0;
		nsTmp.fringing = 0.05;
		nsTmp.sharpness = 0.78;
		nsTmp.merge_fields = 0;
		nsTmp.saturation = -0.250;
		nsTmp.brightness = -0.084;
		nsTmp.gamma = 0.0;
		nsTmp.hue = 7.89 / 180.0;
		
		::nes_ntsc_init( &m_nnBlarggNtsc, &nsTmp );

		m_rsResampler.SetFilter( CResamplerBase::LSN_FF_ROBIDOUX );
	}
	CDx9NtscBlarggFilter::~CDx9NtscBlarggFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx9NtscBlarggFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_tuUploader.Reset();
		m_tpsScaler.Reset();
		m_rsResampler.Reset();
		m_trRenderer.Reset();
		ReleaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		m_ui32FinalStride = RowStride( NES_NTSC_OUT_WIDTH( _ui16Width ), OutputBits() );
		m_vRgbBuffer.resize( m_ui32FinalStride * _ui16Height );

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
	uint8_t * CDx9NtscBlarggFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		if LSN_UNLIKELY( !m_pdx9dDevice ) { return m_vBasicRenderTarget[0].data(); }
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

		//FilterFrame( _pui8Input, _ui64RenderStartCycle + 2 );
		::nes_ntsc_blit( &m_nnBlarggNtsc,
			reinterpret_cast<NES_NTSC_IN_T *>(_pui8Input), _ui32Width, _ui64RenderStartCycle % 3, 3,
			_ui32Width, _ui32Height,
			m_vRgbBuffer.data(), m_ui32FinalStride );
		_ui32Width = NES_NTSC_OUT_WIDTH( _ui32Width );
		_ui32Stride = m_ui32FinalStride;

		m_tuUploader.UploadTexels( m_pdx9dDevice, m_vRgbBuffer.data(), _ui32Width, m_ui32SrcH, ui32Pitch, D3DFMT_X8R8G8B8 );

		Render( rRect );

		_ui32Width = uint32_t( s_dgsState.rScreenRect.Width() );
		_ui32Height = uint32_t( s_dgsState.rScreenRect.Height() );
		_ui32Stride = _ui32Width * sizeof( uint32_t );
		return m_vBasicRenderTarget[0].data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx9NtscBlarggFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9NtscBlarggFilter::DeActivate() {
		CParent::DeActivate();

		m_tuUploader.Reset();
		m_tpsScaler.Reset();
		m_rsResampler.Reset();
		m_trRenderer.Reset();
		m_rtResampled.reset();

		if ( m_pdx9dDevice ) {
			s_dgsState.DestroyDx9();
			m_pdx9dDevice = nullptr;
		}
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CDx9NtscBlarggFilter::FrameResize() {
		s_dgsState.OnSizeDx9();
		EnsureSizeAndResources();
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * 
	 * \return Returns true on success.
	 */
	bool CDx9NtscBlarggFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx9dDevice ) {
			if ( !s_dgsState.CreateDx9() ) { return false; }
			m_pdx9dDevice = &s_dgsState.dx9Device;
			m_tuUploader.Reset();
			m_tpsScaler.Reset();
			m_rsResampler.Reset();
			m_trRenderer.Reset();
			m_rtResampled.reset();
		}

		if ( m_ui32RsrcW == m_ui32SrcW && m_ui32RsrcH == m_ui32SrcH ) {
			m_bValidState = true; 
			return true;
		}

		ReleaseSizeDependents();
		m_bValidState = true;
		m_ui32RsrcW = m_ui32SrcW;
		m_ui32RsrcH = m_ui32SrcH;
		return true;
	}

	/**
	 * \brief Releases size-dependent resources (index texture, FP RTs, quad VB).
	 */
	void CDx9NtscBlarggFilter::ReleaseSizeDependents() {
		if LSN_LIKELY( m_rtResampled.get() ) { m_rtResampled->Reset(); }
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * Renders the final output to the backbuffer.
	 *
	 * \param _rOutput The destination rectangle.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx9NtscBlarggFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_tuUploader.GetTexture() || !m_tuUploader.GetTexture()->Valid() ) { return false; }
		IDirect3DDevice9 * pd3d9dDevice = m_pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }


		if ( !m_tpsScaler.Render( m_pdx9dDevice, m_tuUploader.GetTexture()->Get(), NES_NTSC_OUT_WIDTH( m_ui32SrcW ), m_ui32SrcH, GetActualHorSharpness(), GetActualVertSharpness(), CNesPalette::LSN_G_CRT2, m_bUse16BitInitialTarget ) ) {
			return false;
		}


		IDirect3DSurface9 * psBackBuffer = nullptr;
		if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &psBackBuffer ) ) ) {
			
			if ( m_bUseHighQualityResampler ) {
				uint32_t ui32DstW = static_cast<uint32_t>(_rOutput.Width());
				uint32_t ui32DstH = static_cast<uint32_t>(_rOutput.Height());

				m_rsResampler.SetFilter( GetPreferredConvolutionFilter( ui32DstW, ui32DstH ) );
				
				if LSN_UNLIKELY( !m_rtResampled.get() || !m_rtResampled->Valid() || m_ui32ResampledTargetW != ui32DstW || m_ui32ResampledTargetH != ui32DstH ) {
					if LSN_LIKELY( m_rtResampled.get() && m_rtResampled->Get() ) { m_rtResampled->Reset(); }
					else if LSN_UNLIKELY( !m_rtResampled.get() ) { m_rtResampled = std::make_unique<CDirectX9RenderTarget>( m_pdx9dDevice ); }
					
					m_rtResampled->CreateColorTarget( ui32DstW, ui32DstH, m_bUse16BitInitialTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F );
					m_ui32ResampledTargetW = ui32DstW;
					m_ui32ResampledTargetH = ui32DstH;
				}

				if ( m_rtResampled->Valid() && m_rsResampler.Render( m_pdx9dDevice, m_tpsScaler.GetTexture()->Get(), m_tpsScaler.GetWidth(), m_tpsScaler.GetHeight(), m_rtResampled.get(), ui32DstW, ui32DstH ) ) {
					m_trRenderer.Render( m_pdx9dDevice, m_rtResampled->Texture()->Get(), psBackBuffer, _rOutput, 1.0f, false, true );
				}
				else {
					m_trRenderer.Render( m_pdx9dDevice, m_tpsScaler.GetTexture()->Get(), psBackBuffer, _rOutput, 1.0f, false, true );
				}
			}
			else {
				m_trRenderer.Render( m_pdx9dDevice, m_tpsScaler.GetTexture()->Get(), psBackBuffer, _rOutput, 1.0f, false, true );
			}
			
			psBackBuffer->Release();
		}

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
