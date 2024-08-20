/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 fence.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12Fence
	 * \brief A Direct3D 12 fence.
	 *
	 * Description: A Direct3D 12 fence.
	 */
	class CDirectX12Fence {
	public :
		// == Various constructors.
		CDirectX12Fence();
		~CDirectX12Fence();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a fence.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _ui64InitialValue The initial value for the fence.
		 * \param _ffFlags A combination of D3D12_FENCE_FLAGS-typed values that are combined by using a bitwise OR operation. The resulting value specifies options for the fence.
		 * \return Returns true if the fence was created.
		 **/
		bool													CreateFence(
			ID3D12Device * _pd12dDevice,
			UINT64 _ui64InitialValue,
			D3D12_FENCE_FLAGS _ffFlags );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12Fence *									Get() { return m_pfFence; }


	protected :
		// == Members.
		/** The fence we wrap. */
		ID3D12Fence *											m_pfFence = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12