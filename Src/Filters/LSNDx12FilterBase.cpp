#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Direct3D 12 filters.
 */

#include "LSNDx12FilterBase.h"
#include "../Utilities/LSNUtilities.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>

namespace lsn {

	// == Members.
	/** Global: window class name for the DX12 child target. */
	const wchar_t * CDx12FilterBase::LSN_DX12_TARGET_CLASS = L"LSN_DX12_TARGET";

	/** The global Direct3D 12 state. */
	CDx12FilterBase::LSN_DX12_GLOBAL_STATE CDx12FilterBase::s_dgsState;

	CDx12FilterBase::CDx12FilterBase() {
	}
	CDx12FilterBase::~CDx12FilterBase() {
	}


	// == CDx12FilterBase.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx12FilterBase::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

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
	void CDx12FilterBase::Activate() {
		CGpuFilterBase::Activate();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx12FilterBase::DeActivate() {
		m_tgGamma.Reset();
		m_pPhosphor.Reset();
		m_tpsScaler.Reset();
		m_rsResampler.Reset();
		m_trRenderer.Reset();

		m_rtGamma.reset();
		m_rtPhosphorTarget.reset();
		m_rtResampled.reset();

		m_dhGammaRtv.reset();
		m_dhPhosphorRtv.reset();
		m_dhRtvHeap.reset();

		if ( m_pdx12dDevice ) {
			s_dgsState.DestroyDx12();
			m_pdx12dDevice = nullptr;
		}

		CGpuFilterBase::DeActivate();
	}

	/**
	 * \brief Registers the DX12 child target window class (no background erase).
	 * 
	 * \return Returns true if the class is registered or already existed.
	 */
	bool LSN_FASTCALL CDx12FilterBase::RegisterDx12TargetClass() {
		lsw::CWndClassEx wceEx(
			[]( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam )->LRESULT {
				switch ( _uMsg ) {
					case WM_ERASEBKGND : {
						return 1;
					}
					case WM_PAINT : {
						PAINTSTRUCT ps;
						::BeginPaint( _hWnd, &ps );
						::EndPaint( _hWnd, &ps );
						return 0;
					}
					case WM_PRINTCLIENT : {
						return 0;
					}
				}
				return ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam );
			}, LSN_DX12_TARGET_CLASS, CS_DBLCLKS, lsw::CBase::GetThisHandle(), NULL, ::LoadCursorW( NULL, IDC_ARROW ), NULL );
		return lsw::CBase::RegisterClassExW( wceEx.Obj() ) != 0;
	}

	/**
	 * \brief Handles resizing the DX12 buffers and targets.
	 * \return Returns true if resizing was handled correctly.
	 */
	bool CDx12FilterBase::OnSizeDx12() {
		s_dgsState.LayoutTargetChild();
		if ( s_dgsState.bValidState ) {
			if ( !s_dgsState.dx12Device.ResizeSwapChain() ) { return false; }
		}
		return true;
	}

	/**
	 * \brief Fills a buffer with an XYZRHW|TEX1 quad.
	 *
	 * Note: Direct3D 12 does not require the -0.5f offset used in Direct3D 9.
	 *
	 * \param _dx12rBuffer The vertex buffer resource to arrange.
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
	bool CDx12FilterBase::FillQuad( CDirectX12Resource &_dx12rBuffer, float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 ) {
// TMP.
//#pragma pack( push, 1 )
//		struct LSN_XYZRHWTEX1 {
//			float fX, fY, fZ, fW;
//			float fU, fV;
//		};
//#pragma pack( pop )
		D3D12_RANGE rReadRange = { 0, 0 };
		LSN_XYZRHWTEX1 * pvP = nullptr;
		if LSN_UNLIKELY( FAILED( _dx12rBuffer.Get()->Map( 0, &rReadRange, reinterpret_cast<void **>(&pvP) ) ) ) { return false; }
		pvP[0] = { _fL, _fT, 0.0f, 1.0f, _fU0, _fV0 };
		pvP[1] = { _fR, _fT, 0.0f, 1.0f, _fU1, _fV0 };
		pvP[2] = { _fL, _fB, 0.0f, 1.0f, _fU0, _fV1 };
		pvP[3] = { _fR, _fB, 0.0f, 1.0f, _fU1, _fV1 };
		_dx12rBuffer.Get()->Unmap( 0, nullptr );
		return true;
	}

	/**
	 * \brief Ensures base internal size is updated and size-dependent resources are (re)created.
	 *
	 * \param _pdx12dDevice The active Direct3D 12 device wrapper.
	 * \param _pgclCommandList The command list used to initialize resource state barriers on creation.
	 * \param _ui32NativeW The unscaled native output width.
	 * \param _ui32NativeH The unscaled native output height.
	 * \return Returns true on success.
	 */
	bool CDx12FilterBase::EnsureBaseSizeAndResources( CDirectX12Device * _pdx12dDevice, CDirectX12GraphicsCommandList * /*_pgclCommandList*/, uint32_t _ui32NativeW, uint32_t _ui32NativeH ) {
		if LSN_UNLIKELY( !_pdx12dDevice || !_pdx12dDevice->GetDevice() ) { return false; }
		ID3D12Device * pd12Device = _pdx12dDevice->GetDevice();

		if ( m_ui32RsrcW == _ui32NativeW && m_ui32RsrcH == _ui32NativeH && m_rtGamma.get() && m_rtGamma->Get() && m_rtPhosphorTarget.get() && m_rtPhosphorTarget->Get() ) {
			return true;
		}

		ReleaseBaseSizeDependents();

		m_uiRtvDescriptorSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

		if LSN_UNLIKELY( !m_dhGammaRtv.get() ) {
			m_dhGammaRtv = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
			if ( !m_dhGammaRtv->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_dhPhosphorRtv.get() ) {
			m_dhPhosphorRtv = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
			if ( !m_dhPhosphorRtv->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_dhRtvHeap.get() ) {
			m_dhRtvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			// Capacity: Slot 0 = BackBuffer, Slot 1 = Resampled Intermediate Render Target
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
			if ( !m_dhRtvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
		DXGI_FORMAT fmtRt = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
		
		m_rtGamma = std::make_unique<CDirectX12Resource>();
		D3D12_RESOURCE_DESC rdGamma = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, _ui32NativeW, _ui32NativeH, 1, 1, fmtRt, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
		D3D12_CLEAR_VALUE cvClear = { fmtRt, { 0.0f, 0.0f, 0.0f, 1.0f } };
		if ( !m_rtGamma->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdGamma, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear ) ) { return false; }
		pd12Device->CreateRenderTargetView( m_rtGamma->Get(), nullptr, m_dhGammaRtv->Get()->GetCPUDescriptorHandleForHeapStart() );

		m_rtPhosphorTarget = std::make_unique<CDirectX12Resource>();
		D3D12_RESOURCE_DESC rdPhos = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, _ui32NativeW, _ui32NativeH, 1, 1, fmtRt, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
		if ( !m_rtPhosphorTarget->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdPhos, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear ) ) { return false; }
		pd12Device->CreateRenderTargetView( m_rtPhosphorTarget->Get(), nullptr, m_dhPhosphorRtv->Get()->GetCPUDescriptorHandleForHeapStart() );

		m_ui32RsrcW = _ui32NativeW;
		m_ui32RsrcH = _ui32NativeH;
		return true;
	}

	/**
	 * \brief Releases size-dependent base resources.
	 */
	void CDx12FilterBase::ReleaseBaseSizeDependents() {
		if LSN_LIKELY( m_rtGamma.get() && m_rtGamma->Get() ) { m_rtGamma->Reset(); }
		if LSN_LIKELY( m_rtPhosphorTarget.get() && m_rtPhosphorTarget->Get() ) { m_rtPhosphorTarget->Reset(); }
		if LSN_LIKELY( m_rtResampled.get() && m_rtResampled->Get() ) { m_rtResampled->Reset(); }
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * Renders the common D3D12 pipeline components.
	 *
	 * \param _pdx12dDevice The active Direct3D 12 device wrapper.
	 * \param _pgclCommandList The command list in which to record draw commands. (Caller must close and execute).
	 * \param _prSrc The starting RGB source texture resource generated by the subclass.
	 * \param _ui32NativeW The base resolution width (prior to integer scaling).
	 * \param _ui32NativeH The base resolution height (prior to integer scaling).
	 * \param _rOutput The destination rectangle in client pixels.
	 * \param _bFlipY Determines whether the integer scaler pass should flip the Y axis.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx12FilterBase::RenderBase( CDirectX12Device * _pdx12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, bool _bFlipY ) {
		if LSN_UNLIKELY( !_prSrc || !_pdx12dDevice || !_pgclCommandList ) { return false; }
		if LSN_UNLIKELY( !EnsureBaseSizeAndResources( _pdx12dDevice, _pgclCommandList, _ui32NativeW, _ui32NativeH ) ) { return false; }

		ID3D12Device * pd12Device = _pdx12dDevice->GetDevice();
		if LSN_UNLIKELY( !pd12Device ) { return false; }

		CDirectX12Resource * prScaleSource = _prSrc;
		DXGI_FORMAT fmtRt = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;

		// --- PASS 1: GAMMA ---
		CNesPalette::LSN_GAMMA effGamma = GetEffectiveGamma();
		if ( effGamma != CNesPalette::LSN_G_NONE ) {
			D3D12_RESOURCE_BARRIER rbGamma[1];
			rbGamma[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtGamma->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
			_pgclCommandList->Get()->ResourceBarrier( 1, rbGamma );

			D3D12_CPU_DESCRIPTOR_HANDLE hGammaRtv = m_dhGammaRtv->Get()->GetCPUDescriptorHandleForHeapStart();

			if ( m_tgGamma.Render( _pdx12dDevice, _pgclCommandList, prScaleSource, _ui32NativeW, _ui32NativeH, hGammaRtv, effGamma, fmtRt ) ) {
				prScaleSource = m_rtGamma.get();
			}

			rbGamma[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			rbGamma[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			_pgclCommandList->Get()->ResourceBarrier( 1, rbGamma );
		}

		// --- PASS 2: PHOSPHOR DECAY ---
		if ( m_bEnablePhosphorDecay ) {
			D3D12_RESOURCE_BARRIER rbPhosphor[1];
			rbPhosphor[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtPhosphorTarget->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
			_pgclCommandList->Get()->ResourceBarrier( 1, rbPhosphor );

			D3D12_CPU_DESCRIPTOR_HANDLE hPhosphorRtv = m_dhPhosphorRtv->Get()->GetCPUDescriptorHandleForHeapStart();

			if ( m_pPhosphor.Render( _pdx12dDevice, _pgclCommandList, prScaleSource, _ui32NativeW, _ui32NativeH, m_rtPhosphorTarget->Get(), hPhosphorRtv, fmtRt, m_fInitPhosphorDecay, m_fPhosphorDecayRateRed, m_fPhosphorDecayRateGreen, m_fPhosphorDecayRateBlue ) ) {
				prScaleSource = m_rtPhosphorTarget.get();
			}

			rbPhosphor[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			rbPhosphor[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			_pgclCommandList->Get()->ResourceBarrier( 1, rbPhosphor );
		}

		// --- PASS 3: PIXEL SCALER ---
		uint32_t uiActualW = GetActualHorSharpness( _rOutput.Width() );
		uint32_t uiActualH = GetActualVertSharpness( _rOutput.Height() );

		if ( !m_tpsScaler.Render( _pdx12dDevice, _pgclCommandList, prScaleSource, _ui32NativeW, _ui32NativeH, uiActualW, uiActualH, m_bUse16BitInitialTarget, _bFlipY ) ) {
			return false;
		}

		// --- PASS 4: COMPOSITE (RESAMPLER/RENDERER) ---
		Microsoft::WRL::ComPtr<ID3D12Resource> rBackBuffer;
		if LSN_LIKELY( SUCCEEDED( _pdx12dDevice->GetSwapChain()->GetBuffer( _pdx12dDevice->GetSwapChain()->GetCurrentBackBufferIndex(), IID_PPV_ARGS( &rBackBuffer ) ) ) ) {
			
			D3D12_RESOURCE_BARRIER rbBack[1];
			rbBack[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { rBackBuffer.Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET } };
			_pgclCommandList->Get()->ResourceBarrier( 1, rbBack );

			D3D12_CPU_DESCRIPTOR_HANDLE hBackRtv = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			pd12Device->CreateRenderTargetView( rBackBuffer.Get(), &rtvDesc, hBackRtv );

			// RESTORE FULL VIEWPORT FOR THE BACKBUFFER DRAW TO FIX CROPPING BUG
			D3D12_VIEWPORT vpViewport = { 0.0f, 0.0f, static_cast<float>(_rOutput.Width()), static_cast<float>(_rOutput.Height()), 0.0f, 1.0f };
			D3D12_RECT rScissor = { 0, 0, static_cast<LONG>(_rOutput.Width()), static_cast<LONG>(_rOutput.Height()) };
			_pgclCommandList->Get()->RSSetViewports( 1, &vpViewport );
			_pgclCommandList->Get()->RSSetScissorRects( 1, &rScissor );

			if ( m_bUseHighQualityResampler ) {
				uint32_t ui32DstW = static_cast<uint32_t>(_rOutput.Width());
				uint32_t ui32DstH = static_cast<uint32_t>(_rOutput.Height());

				m_rsResampler.SetFilter( GetPreferredConvolutionFilter( ui32DstW, ui32DstH ) );
				
				if LSN_UNLIKELY( !m_rtResampled.get() || !m_rtResampled->Get() || m_ui32ResampledTargetW != ui32DstW || m_ui32ResampledTargetH != ui32DstH ) {
					if LSN_LIKELY( m_rtResampled.get() && m_rtResampled->Get() ) { m_rtResampled->Reset(); }
					else if LSN_UNLIKELY( !m_rtResampled.get() ) { m_rtResampled = std::make_unique<CDirectX12Resource>(); }

					D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
					DXGI_FORMAT fmtRt2 = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
					D3D12_RESOURCE_DESC rdResampled = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, ui32DstW, ui32DstH, 1, 1, fmtRt2, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
					D3D12_CLEAR_VALUE cvClear = { fmtRt2, { 0.0f, 0.0f, 0.0f, 1.0f } };
					
					m_rtResampled->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdResampled, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear );
					
					m_ui32ResampledTargetW = ui32DstW;
					m_ui32ResampledTargetH = ui32DstH;

					D3D12_CPU_DESCRIPTOR_HANDLE hResampledRtv = hBackRtv; hResampledRtv.ptr += m_uiRtvDescriptorSize;
					pd12Device->CreateRenderTargetView( m_rtResampled->Get(), nullptr, hResampledRtv );
				}

				D3D12_CPU_DESCRIPTOR_HANDLE hResampledRtv = hBackRtv; hResampledRtv.ptr += m_uiRtvDescriptorSize;

				D3D12_RESOURCE_BARRIER rbResampled[1];
				rbResampled[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtResampled->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
				_pgclCommandList->Get()->ResourceBarrier( 1, rbResampled );

				if ( m_rsResampler.Render( _pdx12dDevice, _pgclCommandList, m_tpsScaler.GetTexture(), m_tpsScaler.GetWidth(), m_tpsScaler.GetHeight(), m_rtResampled->Get(), hResampledRtv, ui32DstW, ui32DstH ) ) {
					rbResampled[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtResampled->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
					_pgclCommandList->Get()->ResourceBarrier( 1, rbResampled );

					m_trRenderer.Render( _pdx12dDevice, _pgclCommandList, m_rtResampled.get(), rBackBuffer.Get(), hBackRtv, _rOutput, 1.0f, false, true );
				}
				else {
					// Fallback to scaler if the 2-pass resampler aborts.
					rbResampled[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtResampled->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
					_pgclCommandList->Get()->ResourceBarrier( 1, rbResampled );

					m_trRenderer.Render( _pdx12dDevice, _pgclCommandList, m_tpsScaler.GetTexture(), rBackBuffer.Get(), hBackRtv, _rOutput, 1.0f, false, true );
				}
			}
			else {
				m_trRenderer.Render( _pdx12dDevice, _pgclCommandList, m_tpsScaler.GetTexture(), rBackBuffer.Get(), hBackRtv, _rOutput, 1.0f, false, true );
			}

			D3D12_RESOURCE_BARRIER rbPresent[1];
			rbPresent[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { rBackBuffer.Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT } };
			_pgclCommandList->Get()->ResourceBarrier( 1, rbPresent );
		}

		return true;
	}

	// == Functions.
	/**
	 * \brief Initializes the DirectX 12 device for this window.
	 * 
	 * Dynamically loads d3d12.dll via the DX12 wrapper and creates a device bound to this HWND.
	 * Call once (e.g., after the window is created) and, if successful, the Paint() path will
	 * render using DirectX 12 instead of the software blitter.
	 *
	 * \return Returns true if the DX12 device was created and is ready.
	 **/
	bool CDx12FilterBase::LSN_DX12_GLOBAL_STATE::CreateDx12() {
		if ( dx12Device.GetDevice() ) {
			++i32RefCnt;
			return true;
		}
		bValidState = false;
		if ( !pwParent || !CDirectX12::Supported() ) { return false; }
		if ( !CDx12FilterBase::RegisterDx12TargetClass() ) { return false; }
		if ( !CreateTargetChild() ) { return false; }
		LayoutTargetChild();

		std::string sAdapter;
		if ( dx12Device.Create( hWndTarget, sAdapter ) ) {
			++i32RefCnt;
			bValidState = true;
			return true;
		}
		
		return false;
	}

	/**
	 * \brief Creates the child target window used for DX12 presentation.
	 * 
	 * \return Returns true if created or already exists.
	 */
	bool CDx12FilterBase::LSN_DX12_GLOBAL_STATE::CreateTargetChild() {
		if ( hWndTarget && ::IsWindow( hWndTarget ) ) { return true; }
		if ( !pwParent ) { return false; }
		constexpr DWORD dwStyle   = WS_CHILD | WS_VISIBLE;
		constexpr DWORD dwExStyle = WS_EX_NOPARENTNOTIFY;
		rScreenRect = pwParent->VirtualClientRect( nullptr );
		hWndTarget = ::CreateWindowExW(
			dwExStyle, CDx12FilterBase::LSN_DX12_TARGET_CLASS, L"", dwStyle,
			rScreenRect.left, rScreenRect.top, rScreenRect.Width(), rScreenRect.Height(),
			pwParent->Wnd(), NULL, ::GetModuleHandleW( NULL ), NULL );

		::SetClassLongPtrW( hWndTarget, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(nullptr) );
		return hWndTarget != nullptr;
	}

	/**
	 * Destroys the DirectX 12 device and swap chain when the ref-count reaches 0.
	 **/
	void CDx12FilterBase::LSN_DX12_GLOBAL_STATE::DestroyDx12() {
		if ( i32RefCnt ) {
			if ( --i32RefCnt == 0 ) {
				bValidState = false;
				dx12Device.Reset();
				
				if ( hWndTarget ) {
					::DestroyWindow( hWndTarget );
					hWndTarget = NULL;
				}
			}
		}
	}

	/**
	 * \brief Lays out and caches the drawable client region for the swap chain.
	 *
	 * \return Returns true on success.
	 */
	bool CDx12FilterBase::LSN_DX12_GLOBAL_STATE::LayoutTargetChild() {
		if ( !hWndTarget || !::IsWindow( hWndTarget ) ) { return false; }
		
		rScreenRect = pwParent->VirtualClientRect( nullptr );
		const int iW = int( std::max<LONG>( 1, rScreenRect.Width() ) );
		const int iH = int( std::max<LONG>( 1, rScreenRect.Height() ) );
		
		::SetWindowPos( hWndTarget, nullptr, rScreenRect.left, rScreenRect.top, iW, iH,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS );
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
