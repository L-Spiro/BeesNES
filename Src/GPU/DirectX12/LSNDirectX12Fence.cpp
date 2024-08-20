/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 fence.
 */


#ifdef LSN_DX12

#include "LSNDirectX12Fence.h"

namespace lsn {

	// == Various constructors.
	CDirectX12Fence::CDirectX12Fence() {
	}
	CDirectX12Fence::~CDirectX12Fence() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12Fence::Reset() {
		if ( m_pfFence ) {
			m_pfFence->Release();
			m_pfFence = nullptr;
		}
	}

	/**
	 * Creates a fence.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _ui64InitialValue The initial value for the fence.
	 * \param _ffFlags A combination of D3D12_FENCE_FLAGS-typed values that are combined by using a bitwise OR operation. The resulting value specifies options for the fence.
	 * \return Returns true if the fence was created.
	 **/
	bool CDirectX12Fence::CreateFence(
		ID3D12Device * _pd12dDevice,
		UINT64 _ui64InitialValue,
		D3D12_FENCE_FLAGS _ffFlags ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		return SUCCEEDED( _pd12dDevice->CreateFence( _ui64InitialValue, _ffFlags, IID_PPV_ARGS( &m_pfFence ) ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
