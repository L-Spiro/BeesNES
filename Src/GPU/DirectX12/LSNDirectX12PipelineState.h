/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 pipeline state.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12PipelineState
	 * \brief A Direct3D 12 pipeline state.
	 *
	 * Description: A Direct3D 12 pipeline state.
	 */
	class CDirectX12PipelineState {
	public :
		// == Various constructors.
		CDirectX12PipelineState();
		~CDirectX12PipelineState();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a graphics pipeline state object.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _pgpsdDesc A pointer to a D3D12_GRAPHICS_PIPELINE_STATE_DESC structure that describes graphics pipeline state.
		 * \return Returns true if the pipeline state was created.
		 **/
		bool													CreateGraphicsPipelineState(
			ID3D12Device * _pd12dDevice,
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC * _pgpsdDesc );

		/**
		 * Creates a compute pipeline state object.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _pcpsdDesc A pointer to a D3D12_COMPUTE_PIPELINE_STATE_DESC structure that describes compute pipeline state.
		 * \return Returns true if the compute pipeline state was created.
		 **/
		bool													CreateComputePipelineState(
			ID3D12Device * _pd12dDevice,
			const D3D12_COMPUTE_PIPELINE_STATE_DESC * _pcpsdDesc );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12PipelineState *							Get() { return m_ppsPipelineState; }


	protected :
		// == Members.
		/** The pipeline state we wrap. */
		ID3D12PipelineState *									m_ppsPipelineState = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12