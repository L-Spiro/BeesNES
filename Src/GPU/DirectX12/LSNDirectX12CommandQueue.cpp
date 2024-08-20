/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 command queue.
 */


#ifdef LSN_DX12

#include "LSNDirectX12CommandQueue.h"

namespace lsn {

	// == Various constructors.
	CDirectX12CommandQueue::CDirectX12CommandQueue() {
	}
	CDirectX12CommandQueue::~CDirectX12CommandQueue() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12CommandQueue::Reset() {
		if ( m_pcqQueue ) {
			m_pcqQueue->Release();
			m_pcqQueue = nullptr;
		}
	}

	/**
	 * Creates a command queue.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _pcqdDesc Specifies a D3D12_COMMAND_QUEUE_DESC that describes the command queue.
	 * \return Returns true if the command queue was created.
	 **/
	bool CDirectX12CommandQueue::CreateCommandQueue(
		ID3D12Device * _pd12dDevice,
		const D3D12_COMMAND_QUEUE_DESC * _pcqdDesc ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateCommandQueue( _pcqdDesc, IID_PPV_ARGS( &m_pcqQueue ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
