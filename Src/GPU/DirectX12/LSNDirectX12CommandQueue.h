/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 command queue.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12CommandQueue
	 * \brief A Direct3D 12 command queue.
	 *
	 * Description: A Direct3D 12 command queue.
	 */
	class CDirectX12CommandQueue {
	public :
		// == Various constructors.
		CDirectX12CommandQueue();
		~CDirectX12CommandQueue();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a command queue.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _pcqdDesc Specifies a D3D12_COMMAND_QUEUE_DESC that describes the command queue.
		 * \return Returns true if the command queue was created.
		 **/
		bool													CreateCommandQueue(
			ID3D12Device * _pd12dDevice,
			const D3D12_COMMAND_QUEUE_DESC * _pcqdDesc );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12CommandQueue *								Get() { return m_pcqQueue; }


	protected :
		// == Members.
		/** The command queue we wrap. */
		ID3D12CommandQueue *									m_pcqQueue = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12