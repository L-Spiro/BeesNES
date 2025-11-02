/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 9 GPU object.
 */


#ifdef LSN_DX9

#include "LSNDirectX9Device.h"
#include "LSNDirectX9LosableResourceManager.h"

namespace lsn {

	CDirectX9Device::CDirectX9Device() {
	}
	CDirectX9Device::~CDirectX9Device() {
		Reset();
	}

	// == Functions.
	/**
	 * Creates a Direct3D 9 device.
	 *
	 * \param _pwWnd The window to which to attach.
	 * \param _sAdapter The adapter to use.
	 * \return Returns true if the device was created.
	 **/
	bool CDirectX9Device::Create( lsw::CWidget * _pwWnd, const std::string &_sAdapter ) {
		Reset();

		lsw::LSW_HMODULE hLib( "d3d9.dll" );
		if ( !hLib.Valid() ) { return false; }

		CDirectX9::PFN_Direct3DCreate9 pfDirect3DCreate9 = reinterpret_cast<CDirectX9::PFN_Direct3DCreate9>(::GetProcAddress( hLib.hHandle, "Direct3DCreate9" ));
		if ( pfDirect3DCreate9 == nullptr ) { return false; }

		Microsoft::WRL::ComPtr<IDirect3D9> pdD3d;
		pdD3d.Attach( pfDirect3DCreate9( D3D_SDK_VERSION ) );
		if ( pdD3d == nullptr ) { return false; }

		// Get the number of adapters.
		UINT uiAdaptorCnt = pdD3d->GetAdapterCount();

		// Enumerate through all display adapters.
		for ( UINT I = 0; I < uiAdaptorCnt; ++I ) {
			D3DADAPTER_IDENTIFIER9 aiAdapterInfo;
        
			// Get the adapter identifier.
			if ( FAILED( pdD3d->GetAdapterIdentifier( I, 0, &aiAdapterInfo ) ) ) { continue; }

			try {
				if ( std::string( aiAdapterInfo.Description ) == _sAdapter || !_sAdapter.size() ) {
					lsw::LSW_RECT rClient = _pwWnd->VirtualClientRect( nullptr );
					D3DPRESENT_PARAMETERS ppPresent = {
						.BackBufferWidth					= static_cast<UINT>(rClient.right - rClient.left),
						.BackBufferHeight					= static_cast<UINT>(rClient.bottom - rClient.top),
						.BackBufferFormat					= D3DFMT_X8R8G8B8,
						.BackBufferCount					= 1,

						.MultiSampleType					= D3DMULTISAMPLE_NONE,
						.MultiSampleQuality					= 0,

						.SwapEffect							= D3DSWAPEFFECT_DISCARD,
						.hDeviceWindow						= _pwWnd->Wnd(),
						.Windowed							= TRUE,
						.EnableAutoDepthStencil				= TRUE,
						.AutoDepthStencilFormat				= D3DFMT_D24S8,
						.Flags								= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL,

						.FullScreen_RefreshRateInHz			= D3DPRESENT_RATE_DEFAULT,
						.PresentationInterval				= D3DPRESENT_INTERVAL_IMMEDIATE,
					};


					// Get the device capabilities.
					DWORD dwFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					D3DCAPS9 cCaps;
					if ( SUCCEEDED( pdD3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &cCaps ) ) ) {
						if ( (cCaps.DevCaps & (D3DDEVCAPS_PUREDEVICE | D3DDEVCAPS_HWTRANSFORMANDLIGHT)) == (D3DDEVCAPS_PUREDEVICE | D3DDEVCAPS_HWTRANSFORMANDLIGHT) ) { dwFlags = D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING; }
						else if ( cCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) { dwFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING; }
					}

					if ( SUCCEEDED( pdD3d->CreateDevice( I, D3DDEVTYPE_HAL, ppPresent.hDeviceWindow,
						dwFlags, &ppPresent, &m_pd3dDevice ) ) ) {
						m_hLib = std::move( hLib );
						m_pdD3d = std::move( pdD3d );
						m_dwFlags = dwFlags;
						m_ppPresentParms = ppPresent;
						m_pwWnd = _pwWnd;
						return true;
					}
					return false;
				}
			}
			catch ( ... ) {}
		}
		return false;
	}

	/**
	 * Resets the device back to scratch.  Everything that depends on it must already be reset.
	 **/
	void CDirectX9Device::Reset() {
		if ( m_pd3dDevice ) { m_pd3dDevice.Reset(); }
		if ( m_pdD3d ) { m_pdD3d.Reset(); }
		m_hLib.Reset();
		m_dwFlags = 0;
	}

	/**
	 * \brief Reset the device to match the current client-area size of a window.
	 *
	 * Updates \c m_ppPresentParms (backbuffer size, \c hDeviceWindow) from the given window,
	 * calls \c CDirectX9LosableResourceManager::OnLostDevice(), performs \c Reset(&m_ppPresentParms),
	 * then calls \c CDirectX9LosableResourceManager::OnResetDevice().
	 *
	 * \param _pwWnd The target window whose client-area size determines the new backbuffer size.
	 *              If \c nullptr, the cached \c m_pwWnd is used.
	 * \return Returns \c true on success; \c false if the device is still lost or \c Reset() failed.
	 **/
	bool CDirectX9Device::ResetForWindowSize( lsw::CWidget * _pwWnd ) {
		if ( !m_pd3dDevice ) { return false; }
		if ( _pwWnd ) { m_pwWnd = _pwWnd; }

		lsw::LSW_RECT rRect = _pwWnd->VirtualClientRect( nullptr );
		const UINT uiW = std::max<LONG>( 1, rRect.Width() );
		const UINT uiH = std::max<LONG>( 1, rRect.Height() );

		m_ppPresentParms.hDeviceWindow		= m_pwWnd->Wnd();
		m_ppPresentParms.BackBufferWidth	= uiW;
		m_ppPresentParms.BackBufferHeight	= uiH;

		CDirectX9LosableResourceManager::OnLostDevice();
		const HRESULT hRes = m_pd3dDevice->Reset( &m_ppPresentParms );
		if ( FAILED( hRes ) ) { return false; }
		CDirectX9LosableResourceManager::OnResetDevice();
		return true;
	}

	/**
	 * \brief Check cooperative level and recover from device loss when possible.
	 *
	 * When \c TestCooperativeLevel() returns \c D3DERR_DEVICENOTRESET, this attempts a \c Reset()
	 * using the current client-area size (via \c ResetForWindowSize()).
	 *
	 * \param _pwWnd The window used to refresh \c m_ppPresentParms on reset. If \c nullptr,
	 *              the cached \c m_pwWnd is used.
	 * \return Returns \c true if the device is usable for this frame (OK or reset succeeded);
	 *         \c false if still lost and rendering should be skipped this frame.
	 **/
	bool CDirectX9Device::HandleDeviceLoss( lsw::CWidget * _pwWnd ) {
		if ( !m_pd3dDevice ) { return false; }
		const HRESULT hRes = m_pd3dDevice->TestCooperativeLevel();
		if ( hRes == D3DERR_DEVICELOST ) { return false; }
		if ( hRes == D3DERR_DEVICENOTRESET ) {
			return ResetForWindowSize( _pwWnd ? _pwWnd : m_pwWnd );
		}
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9