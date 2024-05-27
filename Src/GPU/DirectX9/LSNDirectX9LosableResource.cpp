/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of DirectX 9 functionality.
 */


#ifdef LSN_DX9

#include "LSNDirectX9LosableResource.h"
#include "LSNDirectX9LosableResourceManager.h"

namespace lsn {

	// == Various constructors.
	CDirectX9LosableResource::CDirectX9LosableResource() :
		m_bResourceCanBeLost( false ) {
		CDirectX9LosableResourceManager::RegisterRes( this );
	}
	CDirectX9LosableResource::~CDirectX9LosableResource() {
		CDirectX9LosableResourceManager::RemoveRes( m_ui32UniqueLosableResourceId );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
