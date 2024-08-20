/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 command signature.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12CommandSignature
	 * \brief A Direct3D 12 command signature.
	 *
	 * Description: A Direct3D 12 command signature.
	 */
	class CDirectX12CommandSignature {
	public :
		// == Various constructors.
		CDirectX12CommandSignature();
		~CDirectX12CommandSignature();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * This method creates a command signature.
		 * 
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _pcsdDesc Describes the command signature to be created with the D3D12_COMMAND_SIGNATURE_DESC structure.
		 * \param _rspRootSignature Specifies the ID3D12RootSignature that the command signature applies to. The root signature is required if any of the commands in the signature will update bindings on the pipeline. If the only command present is a draw or dispatch, the root signature parameter can be set to NULL.
		 * \return Returns true if the command signature was created.
		 **/
		bool													CreateCommandSignature(
			ID3D12Device * _pd12dDevice,
			const D3D12_COMMAND_SIGNATURE_DESC * _pcsdDesc,
			ID3D12RootSignature * _rspRootSignature );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline ID3D12CommandSignature *							Get() { return m_pcsSignature; }


	protected :
		// == Members.
		/** The command signature we wrap. */
		ID3D12CommandSignature *								m_pcsSignature = nullptr;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12