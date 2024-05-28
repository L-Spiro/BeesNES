/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of DirectX 9 functionality.
 */


#ifdef LSN_DX9

#include "LSNDirectX9.h"
#include "Helpers/LSWHelpers.h"

#include <wrl/client.h>

namespace lsn {

	// == Members.
	BOOL CDirectX9::m_bSupported = 3;																/**< Is Direct3D 9 supported? */
	std::vector<CDirectX9::LSN_ADAPTOR_INFO> CDirectX9::m_vDisplayDevices;							/**< The array of display devices. */

	// == Functions.
	/**
	 * Cleans up any resources used by Direct3D 9.
	 **/
	void CDirectX9::ShutDown() {
		m_vDisplayDevices.clear();
		m_vDisplayDevices = std::vector<LSN_ADAPTOR_INFO>();
		m_bSupported = 3;
	}

	/**
	 * Enumerates display devices via Direct3D 9.
	 * 
	 * \param _pd3d9Dx9 The Direct3D 9 device pointer.
	 * \param _vDevices Display devices are added to _vDevices.
	 * \return Returns true if no allocations issues were encountered while adding display devices to _vDevices.
	 **/
	bool CDirectX9::EnumerateDisplayDevices( IDirect3D9 * _pd3d9Dx9, std::vector<CDirectX9::LSN_ADAPTOR_INFO> &_vDevices ) {
		if ( nullptr == _pd3d9Dx9 ) { return false; }
		// Get the number of adapters.
		UINT adapterCount = _pd3d9Dx9->GetAdapterCount();

		// Enumerate through all display adapters.
		for ( UINT I = 0; I < adapterCount; ++I ) {
			LSN_ADAPTOR_INFO aiAdapterInfo;
			aiAdapterInfo.uiAdapter = I;
        
			// Get the adapter identifier.
			if ( FAILED( _pd3d9Dx9->GetAdapterIdentifier( I, 0, &aiAdapterInfo.aiIdentifier ) ) ) { continue; }

			// Get the device capabilities.
			if ( FAILED( _pd3d9Dx9->GetDeviceCaps( I, D3DDEVTYPE_HAL, &aiAdapterInfo.cCaps ) ) ) { continue; }

			try {
				_vDevices.push_back( aiAdapterInfo );
			}
			catch ( ... ) { return false; }
		}
		return true;
	}

	/**
	 * Checks for Direct3D 9 support.
	 * 
	 * \return Returns true if Direct3D 9 is supported.
	 **/
	bool CDirectX9::IsSupported() {
		lsw::LSW_HMODULE hLib( "d3d9.dll" );
		if ( !hLib.Valid() ) { return false; }

		
		PfDirect3DCreate9 pfDirect3DCreate9 = reinterpret_cast<PfDirect3DCreate9>(::GetProcAddress( hLib.hHandle, "Direct3DCreate9" ));
		if ( pfDirect3DCreate9 == nullptr ) { return false; }


		// Create the Direct3D9 object.
		Microsoft::WRL::ComPtr<IDirect3D9> pD3d = pfDirect3DCreate9( D3D_SDK_VERSION );
		if ( pD3d == nullptr ) { return false; }

		GatherDevices( pD3d.Get() );
		for ( size_t I = m_vDisplayDevices.size(); I--; ) {
			if ( !FAILED( pD3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_vDisplayDevices[I].cCaps ) ) ) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Populates m_vDisplayDevices.
	 * 
	 * \param _pd3d9Dx9 The Direct3D 9 device pointer.
	 **/
	void CDirectX9::GatherDevices( IDirect3D9 * _pd3d9Dx9 ) {
		if ( !m_vDisplayDevices.size() ) {
			EnumerateDisplayDevices( _pd3d9Dx9, m_vDisplayDevices );
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
