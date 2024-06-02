/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 12 GPU object.
 */


#ifdef LSN_DX12

#include "LSNDirectX12Device.h"

namespace lsn {

	CDirectX12Device::CDirectX12Device() {
	}
	CDirectX12Device::~CDirectX12Device() {
		m_scSwapChain.Reset();
		m_pd3dDevice.Reset();
		m_pfFactory.Reset();
		m_hLib.Reset();
		m_hDxgiLib.Reset();
	}

	// == Functions.
	/**
	 * Creates a Direct3D 12 device.
	 *
	 * \param _hWnd The window to which to attach.
	 * \param _sAdapter The adapter to use.
	 * \return Returns true if the device was created.
	 **/
	bool CDirectX12Device::Create( HWND _hWnd, const std::string &_sAdapter ) {
		m_scSwapChain.Reset();
		m_pd3dDevice.Reset();
		m_pfFactory.Reset();
		m_hLib.Reset();
		m_hDxgiLib.Reset();

		// Load DLL's first to ensure they are the last deconstuctors called.  Every other object needs to be unloaded on failure before the DLL files.
		lsw::LSW_HMODULE hLib( "dxgi.dll" );
		if ( !hLib.Valid() ) { return false; }

		lsw::LSW_HMODULE hLibD3d( "d3d12.dll" );
		CDirectX12::PFN_D3D12CreateDevice pD3D12CreateDevice = reinterpret_cast<CDirectX12::PFN_D3D12CreateDevice>(::GetProcAddress( hLibD3d.hHandle, "D3D12CreateDevice" ));
		if ( !pD3D12CreateDevice ) { return false; }

		CDirectX12::PFN_CreateDXGIFactory1 pCreateDXGIFactory1 = reinterpret_cast<CDirectX12::PFN_CreateDXGIFactory1>(::GetProcAddress( hLib.hHandle, "CreateDXGIFactory1" ));
		if ( !pCreateDXGIFactory1 ) { return false; }
		
		// Create DXGI Factory.
		Microsoft::WRL::ComPtr<IDXGIFactory6> fFactory;
		HRESULT hRes = pCreateDXGIFactory1( IID_PPV_ARGS( &fFactory ) );
		if ( FAILED( hRes ) ) { return false; }

		


		UINT I = 0;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> paAdapter;
		// Enumerate all adapters.
		while ( fFactory->EnumAdapters1( I, &paAdapter ) != DXGI_ERROR_NOT_FOUND ) {
			DXGI_ADAPTER_DESC1 adDesc;
			paAdapter->GetDesc1( &adDesc );
			if ( (adDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 ) {

				// Check if the adapter supports Direct3D 12.
				Microsoft::WRL::ComPtr<ID3D12Device> dDevice;
				HRESULT hRes = pD3D12CreateDevice( paAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &dDevice ) );
				if ( SUCCEEDED( hRes ) ) {
					
				}
			}
			++I;
		}

		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
