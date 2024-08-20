/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 descriptor heap.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12DescriptorHeap
	 * \brief A Direct3D 12 descriptor heap.
	 *
	 * Description: A Direct3D 12 descriptor heap.
	 */
	class CDirectX12DescriptorHeap {
	public :
		// == Various constructors.
		CDirectX12DescriptorHeap();
		~CDirectX12DescriptorHeap();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a descriptor heap object.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _pdhdDescriptorHeapDesc AnA pointer to a D3D12_DESCRIPTOR_HEAP_DESC structure that describes the heap.
		 * \return Returns true if the descriptor heap was created.
		 **/
		bool													CreateDescriptorHeap(
			ID3D12Device * _pd12dDevice,
			const D3D12_DESCRIPTOR_HEAP_DESC * _pdhdDescriptorHeapDesc );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12DescriptorHeap *							Get() { return m_pdhDescHeap; }


	protected :
		// == Members.
		/** The descriptor heap we wrap. */
		ID3D12DescriptorHeap *									m_pdhDescHeap = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12