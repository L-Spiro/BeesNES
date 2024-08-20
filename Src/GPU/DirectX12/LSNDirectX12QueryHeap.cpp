/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 query heap.
 */


#ifdef LSN_DX12

#include "LSNDirectX12QueryHeap.h"

namespace lsn {

	// == Various constructors.
	CDirectX12QueryHeap::CDirectX12QueryHeap() {
	}
	CDirectX12QueryHeap::~CDirectX12QueryHeap() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12QueryHeap::Reset() {
		if ( m_pqhHeap ) {
			m_pqhHeap->Release();
			m_pqhHeap = nullptr;
		}
	}

	/**
	 * Creates a query heap. A query heap contains an array of queries.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _pqhdDesc Specifies the query heap in a D3D12_QUERY_HEAP_DESC structure.
	 * \return Returns true if the query heap was created.
	 **/
	bool CDirectX12QueryHeap::CreateQueryHeap(
		ID3D12Device * _pd12dDevice,
		const D3D12_QUERY_HEAP_DESC * _pqhdDesc ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateQueryHeap( _pqhdDesc, IID_PPV_ARGS( &m_pqhHeap ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
