/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 descriptor heap.
 */


#ifdef LSN_DX12

#include "LSNDirectX12DescriptorHeap.h"

namespace lsn {

	// == Various constructors.
	CDirectX12DescriptorHeap::CDirectX12DescriptorHeap() {
	}
	CDirectX12DescriptorHeap::~CDirectX12DescriptorHeap() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12DescriptorHeap::Reset() {
		if ( m_pdhDescHeap ) {
			m_pdhDescHeap->Release();
			m_pdhDescHeap = nullptr;
		}
	}

	/**
	 * Creates a descriptor heap object.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _pdhdDescriptorHeapDesc AnA pointer to a D3D12_DESCRIPTOR_HEAP_DESC structure that describes the heap.
	 * \return Returns true if the descriptor heap was created.
	 **/
	bool CDirectX12DescriptorHeap::CreateDescriptorHeap(
		ID3D12Device * _pd12dDevice,
		const D3D12_DESCRIPTOR_HEAP_DESC * _pdhdDescriptorHeapDesc ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateDescriptorHeap( _pdhdDescriptorHeapDesc, IID_PPV_ARGS( &m_pdhDescHeap ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
