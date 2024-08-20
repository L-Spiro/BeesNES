/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 query heap.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12QueryHeap
	 * \brief A Direct3D 12 query heap.
	 *
	 * Description: A Direct3D 12 query heap.
	 */
	class CDirectX12QueryHeap {
	public :
		// == Various constructors.
		CDirectX12QueryHeap();
		~CDirectX12QueryHeap();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a query heap. A query heap contains an array of queries.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _pqhdDesc Specifies the query heap in a D3D12_QUERY_HEAP_DESC structure.
		 * \return Returns true if the query heap was created.
		 **/
		bool													CreateQueryHeap(
			ID3D12Device * _pd12dDevice,
			const D3D12_QUERY_HEAP_DESC * _pqhdDesc );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12QueryHeap *								Get() { return m_pqhHeap; }


	protected :
		// == Members.
		/** The query heap we wrap. */
		ID3D12QueryHeap *										m_pqhHeap = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12