/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 graphics command list.
 */


#ifdef LSN_DX12

#include "LSNDirectX12GraphicsCommandList.h"

namespace lsn {

	// == Various constructors.
	CDirectX12GraphicsCommandList::CDirectX12GraphicsCommandList() {
	}
	CDirectX12GraphicsCommandList::~CDirectX12GraphicsCommandList() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12GraphicsCommandList::Reset() {
		if ( m_pgclList ) {
			m_pgclList->Release();
			m_pgclList = nullptr;
		}
	}

	/**
	 * Creates a command list.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _uiNodeMask For single-GPU operation, set this to zero. If there are multiple GPU nodes, then set a bit to identify the node (the device's physical adapter) for which to create the command list. Each bit in the mask corresponds to a single node. Only one bit must be set.
	 * \param _cltType Specifies the type of command list to create.
	 * \param _pcaCommandAllocator A pointer to the command allocator object from which the device creates command lists.
	 * \param _psInitialState An optional pointer to the pipeline state object that contains the initial pipeline state for the command list. If it is nullptr, then the runtime sets a dummy initial pipeline state, so that drivers don't have to deal with undefined state. The overhead for this is low, particularly for a command list, for which the overall cost of recording the command list likely dwarfs the cost of a single initial state setting. So there's little cost in not setting the initial pipeline state parameter, if doing so is inconvenient. For bundles, on the other hand, it might make more sense to try to set the initial state parameter (since bundles are likely smaller overall, and can be reused frequently).
	 * \return Returns true if the command list was created.
	 **/
	bool CDirectX12GraphicsCommandList::CreateCommandList(
		ID3D12Device * _pd12dDevice,
		UINT _uiNodeMask,
		D3D12_COMMAND_LIST_TYPE _cltType,
		ID3D12CommandAllocator * _pcaCommandAllocator,
		ID3D12PipelineState * _psInitialState ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateCommandList( _uiNodeMask, _cltType, _pcaCommandAllocator, _psInitialState, IID_PPV_ARGS( &m_pgclList ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
