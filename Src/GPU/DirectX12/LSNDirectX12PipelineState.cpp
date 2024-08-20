/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 pipeline state.
 */


#ifdef LSN_DX12

#include "LSNDirectX12PipelineState.h"

namespace lsn {

	// == Various constructors.
	CDirectX12PipelineState::CDirectX12PipelineState() {
	}
	CDirectX12PipelineState::~CDirectX12PipelineState() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12PipelineState::Reset() {
		if ( m_ppsPipelineState ) {
			m_ppsPipelineState->Release();
			m_ppsPipelineState = nullptr;
		}
	}

	/**
	 * Creates a graphics pipeline state object.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _pgpsdDesc A pointer to a D3D12_GRAPHICS_PIPELINE_STATE_DESC structure that describes graphics pipeline state.
	 * \return Returns true if the pipeline state was created.
	 **/
	bool CDirectX12PipelineState::CreateGraphicsPipelineState(
		ID3D12Device * _pd12dDevice,
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC * _pgpsdDesc ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateGraphicsPipelineState( _pgpsdDesc, IID_PPV_ARGS( &m_ppsPipelineState ) ) );
	}

	/**
	 * Creates a compute pipeline state object.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _pcpsdDesc A pointer to a D3D12_COMPUTE_PIPELINE_STATE_DESC structure that describes compute pipeline state.
	 * \return Returns true if the compute pipeline state was created.
	 **/
	bool CDirectX12PipelineState::CreateComputePipelineState(
		ID3D12Device * _pd12dDevice,
		const D3D12_COMPUTE_PIPELINE_STATE_DESC * _pcpsdDesc ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateComputePipelineState( _pcpsdDesc, IID_PPV_ARGS( &m_ppsPipelineState ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
