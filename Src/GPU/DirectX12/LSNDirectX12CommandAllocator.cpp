/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 command allocator.
 */


#ifdef LSN_DX12

#include "LSNDirectX12CommandAllocator.h"

namespace lsn {

	// == Various constructors.
	CDirectX12CommandAllocator::CDirectX12CommandAllocator() {
	}
	CDirectX12CommandAllocator::~CDirectX12CommandAllocator() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12CommandAllocator::Reset() {
		if ( m_pcaAllocator ) {
			m_pcaAllocator->Release();
			m_pcaAllocator = nullptr;
		}
	}

	/**
	 * Creates a command allocator object.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _cltType A D3D12_COMMAND_LIST_TYPE-typed value that specifies the type of command allocator to create. The type of command allocator can be the type that records either direct command lists or bundles.
	 * \return Returns true if the command allocator was created.
	 **/
	bool CDirectX12CommandAllocator::CreateCommandAllocator(
		ID3D12Device * _pd12dDevice,
		D3D12_COMMAND_LIST_TYPE _cltType ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateCommandAllocator( _cltType, IID_PPV_ARGS( &m_pcaAllocator ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
