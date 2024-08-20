/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 command allocator.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12CommandAllocator
	 * \brief A Direct3D 12 command allocator.
	 *
	 * Description: A Direct3D 12 command allocator.
	 */
	class CDirectX12CommandAllocator {
	public :
		// == Various constructors.
		CDirectX12CommandAllocator();
		~CDirectX12CommandAllocator();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a command allocator object.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _cltType A D3D12_COMMAND_LIST_TYPE-typed value that specifies the type of command allocator to create. The type of command allocator can be the type that records either direct command lists or bundles.
		 * \return Returns true if the command allocator was created.
		 **/
		bool													CreateCommandAllocator(
			ID3D12Device * _pd12dDevice,
			D3D12_COMMAND_LIST_TYPE _cltType );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12CommandAllocator *							Get() { return m_pcaAllocator; }


	protected :
		// == Members.
		/** The command allocator we wrap. */
		ID3D12CommandAllocator *								m_pcaAllocator = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12