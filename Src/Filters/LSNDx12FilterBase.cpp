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
		return CGpuFilterBase::Init( _stBuffers, _ui16Width, _ui16Height );
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx12FilterBase::Activate() {
		CGpuFilterBase::Activate();

		// Create device/swap chain on first activation.
		if ( s_dgsState.CreateDx12() ) {
			s_dgsState.bValidState = true;
		}
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx12FilterBase::DeActivate() {
		CGpuFilterBase::DeActivate();
		s_dgsState.DestroyDx12();
	}

	/**
	 * Informs the filter of a window resize.
	 */
	void CDx12FilterBase::FrameResize() {
		OnSizeDx12();
	}

	/**
	 * Gets a reference to the shared global device.
	 * 
	 * \return Returns a reference to the shared global device.
	 **/
	CDirectX12Device & CDx12FilterBase::Device() {
		return s_dgsState.dx12Device;
	}

	/**
	 * \brief Handles a window-size change for the swap chain.
	 *
	 * \return Returns true on success; false if the device is not available or resize failed.
	 */
	bool CDx12FilterBase::OnSizeDx12() {
		if ( !s_dgsState.pwParent || !s_dgsState.dx12Device.GetDevice() ) { return false; }

		if ( !s_dgsState.LayoutTargetChild() ) { return false; }

		return s_dgsState.dx12Device.ResizeSwapChain();
	}

	/**
	 * \brief Registers the DX12 child target window class (no background erase).
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
	 * \brief Fills the screen-space quad vertex buffer with an XYZRHW|TEX1 quad.
	 */
	bool CDx12FilterBase::FillQuad( CDirectX12Resource &_dx12rBuffer, float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 ) {
		LSN_XYZRHWTEX1 * pvP = nullptr;
		D3D12_RANGE rReadRange = { 0, 0 }; // Zero range since we are only writing, not reading.
		
		if ( SUCCEEDED( _dx12rBuffer.Get()->Map( 0, &rReadRange, reinterpret_cast<void **>(&pvP) ) ) ) {
			pvP[0] = { _fL, _fT, 0.0f, 1.0f, _fU0, _fV0 };
			pvP[1] = { _fR, _fT, 0.0f, 1.0f, _fU1, _fV0 };
			pvP[2] = { _fL, _fB, 0.0f, 1.0f, _fU0, _fV1 };
			pvP[3] = { _fR, _fB, 0.0f, 1.0f, _fU1, _fV1 };
			
			_dx12rBuffer.Get()->Unmap( 0, nullptr );
			return true;
		}
		return false;
	}


	// == LSN_DX12_GLOBAL_STATE.
	/**
	 * Initializes the DirectX 12 device for the current parent window.
	 * 
	 * \return Returns true if the device/swap chain are ready.
	 **/
	bool CDx12FilterBase::LSN_DX12_GLOBAL_STATE::CreateDx12() {
		if ( dx12Device.GetDevice() ) {
			++i32RefCnt;
			return true;
		}
		bValidState = false;

		if ( !pwParent || !CDirectX12::Supported() ) { return false; }
		if ( !CDx12FilterBase::RegisterDx12TargetClass() ) { return false; }
		if ( !CreateDx12TargetChild() ) { return false; }
		
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
	 */
	bool CDx12FilterBase::LSN_DX12_GLOBAL_STATE::CreateDx12TargetChild() {
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

	CDx12FilterBase::LSN_DX12_GLOBAL_STATE::~LSN_DX12_GLOBAL_STATE() {
		if ( i32RefCnt ) {
			::OutputDebugStringA( "\r\n* * * * * LSN_DX12_GLOBAL_STATE::~LSN_DX12_GLOBAL_STATE():i32RefCnt NOT 0 * * * * *\r\n\r\n" );
		}
		if ( hWndTarget ) {
			::OutputDebugStringA( "\r\n* * * * * LSN_DX12_GLOBAL_STATE::~LSN_DX12_GLOBAL_STATE():hWndTarget NOT DESTROYED * * * * *\r\n\r\n" );
			::DestroyWindow( hWndTarget );
			hWndTarget = NULL;
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
