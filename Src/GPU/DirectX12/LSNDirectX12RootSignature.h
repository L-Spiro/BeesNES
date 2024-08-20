/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 root signature.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12RootSignature
	 * \brief A Direct3D 12 root signature.
	 *
	 * Description: A Direct3D 12 root signature.
	 */
	class CDirectX12RootSignature {
	public :
		// == Various constructors.
		CDirectX12RootSignature();
		~CDirectX12RootSignature();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a root signature layout.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _uiNodeMask For single GPU operation, set this to zero. If there are multiple GPU nodes, set bits to identify the nodes (the device's physical adapters) to which the root signature is to apply. Each bit in the mask corresponds to a single node.
		 * \param _pvBlobWithRootSignature A pointer to the source data for the serialized signature.
		 * \param _sBlobLengthInBytes The size, in bytes, of the block of memory that pBlobWithRootSignature points to.
		 * \return Returns true if the root signature was created.
		 **/
		bool													CreateRootSignature(
			ID3D12Device * _pd12dDevice,
			UINT _uiNodeMask,
			const void * _pvBlobWithRootSignature,
			SIZE_T _sBlobLengthInBytes );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12RootSignature *							Get() { return m_prsRoot; }


	protected :
		// == Members.
		/** The root signature we wrap. */
		ID3D12RootSignature *									m_prsRoot = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12#pragma once
