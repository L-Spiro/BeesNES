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
		const uint32_t dwStride = uint32_t( m_stStride = RowStride( _ui16Width, wBitDepth ) );
		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( dwStride * _ui16Height );	
		}

		return InputFormat();
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

}	// namespace lsn

#endif	// #ifdef LSN_DX9
