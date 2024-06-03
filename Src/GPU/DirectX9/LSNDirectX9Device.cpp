/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 9 GPU object.
 */


#ifdef LSN_DX9

#include "LSNDirectX9Device.h"

namespace lsn {

	CDirectX9Device::CDirectX9Device() {
	}
	CDirectX9Device::~CDirectX9Device() {
		m_pd3dDevice.Reset();
		m_pdD3d.Reset();
		m_hLib.Reset();
	}

	// == Functions.
	/**
	 * Creates a Direct3D 9 device.
	 *
	 * \param _hWnd The window to which to attach.
	 * \param _sAdapter The adapter to use.
	 * \return Returns true if the device was created.
	 **/
	bool CDirectX9Device::Create( HWND _hWnd, const std::string &_sAdapter ) {
		m_pd3dDevice.Reset();
		m_pdD3d.Reset();
		m_hLib.Reset();

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
					RECT rClient;
					::GetClientRect( _hWnd, &rClient );
					D3DPRESENT_PARAMETERS ppPresent = {
						.BackBufferWidth					= static_cast<UINT>(rClient.right - rClient.left),
						.BackBufferHeight					= static_cast<UINT>(rClient.bottom - rClient.top),
						.BackBufferFormat					= D3DFMT_X8R8G8B8,
						.BackBufferCount					= 1,

						.MultiSampleType					= D3DMULTISAMPLE_NONE,
						.MultiSampleQuality					= 0,

						.SwapEffect							= D3DSWAPEFFECT_DISCARD,
						.hDeviceWindow						= _hWnd,
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
						return true;
					}
					return false;
				}
			}
			catch ( ... ) {}
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9