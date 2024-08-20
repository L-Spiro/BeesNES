/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 root signature.
 */


#ifdef LSN_DX12

#include "LSNDirectX12RootSignature.h"

namespace lsn {

	// == Various constructors.
	CDirectX12RootSignature::CDirectX12RootSignature() {
	}
	CDirectX12RootSignature::~CDirectX12RootSignature() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12RootSignature::Reset() {
		if ( m_prsRoot ) {
			m_prsRoot->Release();
			m_prsRoot = nullptr;
		}
	}

	/**
	 * Creates a root signature layout.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _uiNodeMask For single GPU operation, set this to zero. If there are multiple GPU nodes, set bits to identify the nodes (the device's physical adapters) to which the root signature is to apply. Each bit in the mask corresponds to a single node.
	 * \param _pvBlobWithRootSignature A pointer to the source data for the serialized signature.
	 * \param _sBlobLengthInBytes The size, in bytes, of the block of memory that pBlobWithRootSignature points to.
	 * \return Returns true if the root signature was created.
	 **/
	bool CDirectX12RootSignature::CreateRootSignature(
		ID3D12Device * _pd12dDevice,
		UINT _uiNodeMask,
		const void * _pvBlobWithRootSignature,
		SIZE_T _sBlobLengthInBytes ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateRootSignature( _uiNodeMask, _pvBlobWithRootSignature, _sBlobLengthInBytes, IID_PPV_ARGS( &m_prsRoot ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
