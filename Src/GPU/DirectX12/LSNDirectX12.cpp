/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of DirectX 12 functionality.
 */


#ifdef LSN_DX12

#include "LSNDirectX12.h"
#include "Helpers/LSWHelpers.h"

#include <wrl/client.h>

namespace lsn {

	// == Members.
	BOOL CDirectX12::m_bSupported = 3;																/**< Is Direct3D 12 supported? */
	std::vector<DXGI_ADAPTER_DESC1> CDirectX12::m_vDisplayDevices;									/**< The array of display devices. */

	// == Functions.
	/**
	 * Cleans up any resources used by Direct3D 12.
	 **/
	void CDirectX12::ShutDown() {
		m_vDisplayDevices.clear();
		m_vDisplayDevices = std::vector<DXGI_ADAPTER_DESC1>();
		m_bSupported = 3;
	}

	/**
	 * Enumerates display devices via Direct3D 12.
	 * 
	 * \param _pfFactory The DXGI factory.
	 * \param _vDevices Display devices are added to _vDevices.
	 * \return Returns true if no allocations issues were encountered while adding display devices to _vDevices.
	 **/
	bool CDirectX12::EnumerateDisplayDevices( IDXGIFactory6 * _pfFactory, std::vector<DXGI_ADAPTER_DESC1> &_vDevices ) {
		if ( nullptr == _pfFactory ) { return false; }

		lsw::LSW_HMODULE hLib( "d3d12.dll" );
		PfD3D12CreateDevice pD3D12CreateDevice = reinterpret_cast<PfD3D12CreateDevice>(::GetProcAddress( hLib.hHandle, "D3D12CreateDevice" ));
		if ( !pD3D12CreateDevice ) { return false; }


		UINT I = 0;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> paAdapter;
		// Enumerate all adapters.
		while ( _pfFactory->EnumAdapters1( I, &paAdapter ) != DXGI_ERROR_NOT_FOUND ) {
			DXGI_ADAPTER_DESC1 adDesc;
			paAdapter->GetDesc1( &adDesc );
			if ( (adDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 ) {

				// Check if the adapter supports Direct3D 12.
				Microsoft::WRL::ComPtr<ID3D12Device> dDevice;
				HRESULT hRes = pD3D12CreateDevice( paAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &dDevice ) );
				if ( SUCCEEDED( hRes ) ) {
					try {
						_vDevices.push_back( adDesc );
					}
					catch ( ... ) { return false; }
				}
			}
			++I;
		}
		return true;
	}

	/**
	 * Checks for Direct3D 12 support.
	 * 
	 * \return Returns true if Direct3D 12 is supported.
	 **/
	bool CDirectX12::IsSupported() {
		lsw::LSW_HMODULE hLib( "dxgi.dll" );
		if ( !hLib.Valid() ) { return false; }

		PfCreateDXGIFactory1 pCreateDXGIFactory1 = reinterpret_cast<PfCreateDXGIFactory1>(::GetProcAddress( hLib.hHandle, "CreateDXGIFactory1" ));
		if ( !pCreateDXGIFactory1 ) { return false; }
		
		// Create DXGI Factory.
		Microsoft::WRL::ComPtr<IDXGIFactory6> fFactory;
		HRESULT hRes = pCreateDXGIFactory1( IID_PPV_ARGS( &fFactory ) );
		if ( FAILED( hRes ) ) { return false; }

		GatherDevices( fFactory.Get() );

		return m_vDisplayDevices.size() != 0;
	}

	/**
	 * Populates m_vDisplayDevices.
	 * 
	 * \param _pfFactory The DXGI factory.
	 **/
	void CDirectX12::GatherDevices( IDXGIFactory6 * _pfFactory ) {
		if ( !m_vDisplayDevices.size() ) {
			EnumerateDisplayDevices( _pfFactory, m_vDisplayDevices );
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
