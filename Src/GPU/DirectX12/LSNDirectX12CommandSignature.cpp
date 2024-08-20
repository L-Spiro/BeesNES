/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 command signature.
 */


#ifdef LSN_DX12

#include "LSNDirectX12CommandSignature.h"

namespace lsn {

	// == Various constructors.
	CDirectX12CommandSignature::CDirectX12CommandSignature() {
	}
	CDirectX12CommandSignature::~CDirectX12CommandSignature() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12CommandSignature::Reset() {
		if ( m_pcsSignature ) {
			m_pcsSignature->Release();
			m_pcsSignature = nullptr;
		}
	}

	/**
	 * This method creates a command signature.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _pcsdDesc Describes the command signature to be created with the D3D12_COMMAND_SIGNATURE_DESC structure.
	 * \param _rspRootSignature Specifies the ID3D12RootSignature that the command signature applies to. The root signature is required if any of the commands in the signature will update bindings on the pipeline. If the only command present is a draw or dispatch, the root signature parameter can be set to NULL.
	 * \return Returns true if the command signature was created.
	 **/
	bool CDirectX12CommandSignature::CreateCommandSignature(
		ID3D12Device * _pd12dDevice,
		const D3D12_COMMAND_SIGNATURE_DESC * _pcsdDesc,
		ID3D12RootSignature * _rspRootSignature ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateCommandSignature( _pcsdDesc, _rspRootSignature, IID_PPV_ARGS( &m_pcsSignature ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
