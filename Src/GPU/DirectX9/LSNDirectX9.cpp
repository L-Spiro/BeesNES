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
	BOOL CDirectX9::m_bSupported = 3;						/**< Is Direct3D 9 supported? */

	// == Functions.
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

		// Check for hardware support.
		D3DCAPS9 cCaps;
		return !FAILED( pD3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &cCaps ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
