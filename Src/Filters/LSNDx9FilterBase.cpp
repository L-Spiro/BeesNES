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

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>


namespace lsn {

	// == Members.
	/** Global: window class name for the DX9 child target. */
	const wchar_t * CDx9FilterBase::LSN_DX9_TARGET_CLASS = L"LSN_DX9_TARGET";

	/** The global Direct3D 9 state. */
	CDx9FilterBase::LSN_DX9_GLOBAL_STATE CDx9FilterBase::s_dgsState;

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

		/*m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;*/

		const uint16_t wBitDepth = uint16_t( OutputBits() );
		const uint32_t dwStride = uint32_t( RowStride( _ui16Width, wBitDepth ) );
		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( dwStride * _ui16Height );	
		}
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );
		return InputFormat();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx9FilterBase::Activate() {
		CGpuFilterBase::Activate();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9FilterBase::DeActivate() {
		m_tgGamma.Reset();
		m_pPhosphor.Reset();
		m_tpsScaler.Reset();
		m_rsResampler.Reset();
		m_trRenderer.Reset();

		m_rtGamma.reset();
		m_rtPhosphorTarget.reset();
		m_rtResampled.reset();

		CGpuFilterBase::DeActivate();
	}

	/**
	 * \brief Registers the DX9 child target window class (no background erase).
	 * 
	 * \return Returns true if the class is registered or already existed.
	 */
	bool LSN_FASTCALL CDx9FilterBase::RegisterDx9TargetClass() {
		lsw::CWndClassEx wceEx(
			[]( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam )->LRESULT {
				switch ( _uMsg ) {
					case WM_ERASEBKGND : {
						// Never erase; avoids flicker.
						return 1;
					}
					case WM_PAINT : {
						// Validate only. Rendering is driven externally.
						PAINTSTRUCT ps;
						::BeginPaint( _hWnd, &ps );
						::EndPaint( _hWnd, &ps );
						return 0;
					}
					case WM_PRINTCLIENT : {
						// Nothing to draw via GDI.
						return 0;
					}
				}
				return ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam );
			}, LSN_DX9_TARGET_CLASS, CS_DBLCLKS, lsw::CBase::GetThisHandle(), NULL, ::LoadCursorW( NULL, IDC_ARROW ), NULL );
		return lsw::CBase::RegisterClassExW( wceEx.Obj() ) != 0;
	}

	/**
	 * \brief Resizes the DX9 backbuffer and reinitializes size-dependent presenter resources.
	 *
	 * Updates cached D3DPRESENT_PARAMETERS with the new client size and resets the device.
	 * Then re-initializes the presenter so its DEFAULT-pool resources (index/LUT targets, VBs)
	 * are recreated against the new device state.
	 *
	 * \return True on success; false if the DX9 path is disabled or reset failed.
	 */
	bool CDx9FilterBase::OnSizeDx9() {
		s_dgsState.LayoutDx9TargetChild();
		if ( !s_dgsState.dx9Device.ResetForWindowSize( s_dgsState.hWndDx9Target ) ) {
			s_dgsState.bValidState = false;
			return false;
		}
		return true;
	}

	/**
	 * \brief Fills the screen-space quad vertex buffer with an XYZRHW|TEX1 quad.
	 *
	 * Applies a -0.5f XY bias to align texel centers with pixel centers in D3D9 when using XYZRHW.
	 *
	 * \param _dx9vbBuffer The vertex buffer to arrange.
	 * \param _fL Left X in pixels.
	 * \param _fT Top Y in pixels.
	 * \param _fR Right X in pixels.
	 * \param _fB Bottom Y in pixels.
	 * \param _fU0 Left U coordinate.
	 * \param _fV0 Top V coordinate.
	 * \param _fU1 Right U coordinate.
	 * \param _fV1 Bottom V coordinate.
	 * \return Returns true on success.
	 */
	bool CDx9FilterBase::FillQuad( CDirectX9VertexBuffer &_dx9vbBuffer, float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 ) {
		constexpr float fOff = 0.5f;
		LSN_XYZRHWTEX1 * pvP = nullptr;
		if LSN_UNLIKELY( !_dx9vbBuffer.Lock( 0, 0, reinterpret_cast<void **>(&pvP), D3DLOCK_DISCARD ) ) { return false; }
		pvP[0] = { _fL - fOff, _fT - fOff, 0.0f, 1.0f, _fU0, _fV0 };
		pvP[1] = { _fR - fOff, _fT - fOff, 0.0f, 1.0f, _fU1, _fV0 };
		pvP[2] = { _fL - fOff, _fB - fOff, 0.0f, 1.0f, _fU0, _fV1 };
		pvP[3] = { _fR - fOff, _fB - fOff, 0.0f, 1.0f, _fU1, _fV1 };
		_dx9vbBuffer.Unlock();

		/*LSN_XYZRHWTEX1 vVerts[4] = {
			{ _fL - fOff, _fT - fOff, 0.0f, 1.0f, _fU0, _fV0 },
			{ _fR - fOff, _fT - fOff, 0.0f, 1.0f, _fU1, _fV0 },
			{ _fL - fOff, _fB - fOff, 0.0f, 1.0f, _fU0, _fV1 },
			{ _fR - fOff, _fB - fOff, 0.0f, 1.0f, _fU1, _fV1 },
		};
		void * pvP = nullptr;
		if LSN_UNLIKELY( !_dx9vbBuffer.Lock( 0, 0, &pvP, D3DLOCK_DISCARD ) ) { return false; }
		std::memcpy( pvP, vVerts, sizeof( vVerts ) );
		_dx9vbBuffer.Unlock();*/
		return true;
	}

	/**
	 * \brief Ensures base internal size is updated and size-dependent resources are (re)created.
	 *
	 * \param _pdx9dDevice The active Direct3D 9 device wrapper.
	 * \param _ui32NativeW The unscaled native output width.
	 * \param _ui32NativeH The unscaled native output height.
	 * \return Returns true on success.
	 */
	bool CDx9FilterBase::EnsureBaseSizeAndResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32NativeW, uint32_t _ui32NativeH ) {
		if LSN_UNLIKELY( !_pdx9dDevice || !_pdx9dDevice->GetDirectX9Device() ) { return false; }

		if ( m_ui32RsrcW == _ui32NativeW && m_ui32RsrcH == _ui32NativeH && m_rtGamma.get() && m_rtGamma->Valid() && m_rtPhosphorTarget.get() && m_rtPhosphorTarget->Valid() ) {
			return true;
		}

		ReleaseBaseSizeDependents();

		D3DFORMAT fmtRt = m_bUse16BitInitialTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F;
		
		m_rtGamma = std::make_unique<CDirectX9RenderTarget>( _pdx9dDevice );
		if ( !m_rtGamma->CreateColorTarget( _ui32NativeW, _ui32NativeH, fmtRt ) ) { return false; }

		m_rtPhosphorTarget = std::make_unique<CDirectX9RenderTarget>( _pdx9dDevice );
		if ( !m_rtPhosphorTarget->CreateColorTarget( _ui32NativeW, _ui32NativeH, fmtRt ) ) { return false; }

		m_ui32RsrcW = _ui32NativeW;
		m_ui32RsrcH = _ui32NativeH;
		return true;
	}

	/**
	 * \brief Releases size-dependent base resources.
	 */
	void CDx9FilterBase::ReleaseBaseSizeDependents() {
		if LSN_LIKELY( m_rtGamma.get() ) { m_rtGamma->Reset(); }
		if LSN_LIKELY( m_rtPhosphorTarget.get() ) { m_rtPhosphorTarget->Reset(); }
		if LSN_LIKELY( m_rtResampled.get() ) { m_rtResampled->Reset(); }
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * Renders the common D3D9 pipeline components.
	 *
	 * \param _pdx9dDevice The active Direct3D 9 device wrapper.
	 * \param _ptSrc The starting RGB source texture generated by the subclass.
	 * \param _ui32NativeW The base resolution width (prior to integer scaling).
	 * \param _ui32NativeH The base resolution height (prior to integer scaling).
	 * \param _rOutput The destination rectangle in client pixels.
	 * \param _bFlipY Determines whether the integer scaler pass should flip the Y axis.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx9FilterBase::RenderBase( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, bool _bFlipY ) {
		if LSN_UNLIKELY( !_ptSrc || !_pdx9dDevice ) { return false; }
		if LSN_UNLIKELY( !EnsureBaseSizeAndResources( _pdx9dDevice, _ui32NativeW, _ui32NativeH ) ) { return false; }

		IDirect3DDevice9 * pd3d9dDevice = _pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		IDirect3DTexture9 * ptScaleSource = _ptSrc;

		// --- PASS 1: GAMMA ---
		CNesPalette::LSN_GAMMA effGamma = GetEffectiveGamma();
		if ( effGamma != CNesPalette::LSN_G_NONE ) {
			if ( m_tgGamma.Render( _pdx9dDevice, ptScaleSource, _ui32NativeW, _ui32NativeH, m_rtGamma.get(), effGamma ) ) {
				ptScaleSource = m_rtGamma->Texture()->Get();
			}
		}

		// --- PASS 2: PHOSPHOR DECAY ---
		if ( m_bEnablePhosphorDecay ) {
			if ( m_pPhosphor.Render( _pdx9dDevice, ptScaleSource, _ui32NativeW, _ui32NativeH, m_rtPhosphorTarget.get(), m_fInitPhosphorDecay, m_fPhosphorDecayRateRed, m_fPhosphorDecayRateGreen, m_fPhosphorDecayRateBlue ) ) {
				ptScaleSource = m_rtPhosphorTarget->Texture()->Get();
			}
		}

		// --- PASS 3: PIXEL SCALER ---
		uint32_t uiActualW = GetActualHorSharpness( _rOutput.Width() );
		uint32_t uiActualH = GetActualVertSharpness( _rOutput.Height() );

		if ( !m_tpsScaler.Render( _pdx9dDevice, ptScaleSource, _ui32NativeW, _ui32NativeH, uiActualW, uiActualH, m_bUse16BitInitialTarget, _bFlipY ) ) {
			return false;
		}

		// --- PASS 4: COMPOSITE (RESAMPLER/RENDERER) ---
		IDirect3DSurface9 * psBackBuffer = nullptr;
		if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &psBackBuffer ) ) ) {
			
			if ( m_bUseHighQualityResampler ) {
				uint32_t ui32DstW = static_cast<uint32_t>(_rOutput.Width());
				uint32_t ui32DstH = static_cast<uint32_t>(_rOutput.Height());

				m_rsResampler.SetFilter( GetPreferredConvolutionFilter( ui32DstW, ui32DstH ) );
				
				if LSN_UNLIKELY( !m_rtResampled.get() || !m_rtResampled->Valid() || m_ui32ResampledTargetW != ui32DstW || m_ui32ResampledTargetH != ui32DstH ) {
					if LSN_LIKELY( m_rtResampled.get() && m_rtResampled->Get() ) { m_rtResampled->Reset(); }
					else if LSN_UNLIKELY( !m_rtResampled.get() ) { m_rtResampled = std::make_unique<CDirectX9RenderTarget>( _pdx9dDevice ); }
					
					m_rtResampled->CreateColorTarget( ui32DstW, ui32DstH, m_bUse16BitInitialTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F );
					m_ui32ResampledTargetW = ui32DstW;
					m_ui32ResampledTargetH = ui32DstH;
				}

				if ( m_rtResampled->Valid() && m_rsResampler.Render( _pdx9dDevice, m_tpsScaler.GetTexture()->Get(), m_tpsScaler.GetWidth(), m_tpsScaler.GetHeight(), m_rtResampled.get(), ui32DstW, ui32DstH ) ) {
					m_trRenderer.Render( _pdx9dDevice, m_rtResampled->Texture()->Get(), psBackBuffer, _rOutput, 1.0f, false, true );
				}
				else {
					m_trRenderer.Render( _pdx9dDevice, m_tpsScaler.GetTexture()->Get(), psBackBuffer, _rOutput, 1.0f, false, true );
				}
			}
			else {
				m_trRenderer.Render( _pdx9dDevice, m_tpsScaler.GetTexture()->Get(), psBackBuffer, _rOutput, 1.0f, false, true );
			}
			
			psBackBuffer->Release();
		}

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
