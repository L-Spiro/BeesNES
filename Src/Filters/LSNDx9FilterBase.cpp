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


}	// namespace lsn

#endif	// #ifdef LSN_DX9
