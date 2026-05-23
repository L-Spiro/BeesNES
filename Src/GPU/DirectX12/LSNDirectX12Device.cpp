/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 12 GPU object.
 */


#ifdef LSN_DX12

#include "LSNDirectX12Device.h"

namespace lsn {

	CDirectX12Device::CDirectX12Device() {
	}
	CDirectX12Device::~CDirectX12Device() {
		Reset();
	}

	// == Functions.
	/**
	 * Creates a Direct3D 12 device.
	 *
	 * \param _hWnd The window to which to attach.
	 * \param _sAdapter The adapter to use.
	 * \return Returns true if the device was created.
	 **/
	bool CDirectX12Device::Create( HWND _hWnd, const std::string &_sAdapter ) {
		Reset();

		// Load DLL's first to ensure they are the last deconstuctors called.  Every other object needs to be unloaded on failure before the DLL files.
		lsw::LSW_HMODULE hLib( "dxgi.dll" );
		if ( !hLib.Valid() ) { return false; }
		CDirectX12::PFN_CreateDXGIFactory1 pCreateDXGIFactory1 = reinterpret_cast<CDirectX12::PFN_CreateDXGIFactory1>(::GetProcAddress( hLib.hHandle, "CreateDXGIFactory1" ));
		if ( !pCreateDXGIFactory1 ) { return false; }

		lsw::LSW_HMODULE hLibD3d( "d3d12.dll" );
		if ( !hLibD3d.Valid() ) { return false; }
		CDirectX12::PFN_D3D12CreateDevice pD3D12CreateDevice = reinterpret_cast<CDirectX12::PFN_D3D12CreateDevice>(::GetProcAddress( hLibD3d.hHandle, "D3D12CreateDevice" ));
		if ( !pD3D12CreateDevice ) { return false; }

		
		
		// Create DXGI Factory.
		Microsoft::WRL::ComPtr<IDXGIFactory6> fFactory;
		HRESULT hRes = pCreateDXGIFactory1( IID_PPV_ARGS( &fFactory ) );
		if ( FAILED( hRes ) ) { return false; }

		UINT I = 0;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> paAdapter;
		// Enumerate all adapters.
		std::wstring wsTmpName;
		try {
			wsTmpName = std::wstring( _sAdapter.begin(), _sAdapter.end() );

			while ( fFactory->EnumAdapters1( I, &paAdapter ) != DXGI_ERROR_NOT_FOUND ) {
				DXGI_ADAPTER_DESC1 adDesc;
				paAdapter->GetDesc1( &adDesc );
				if ( ((adDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 && _sAdapter.empty()) ||
					wsTmpName == std::wstring( adDesc.Description ) ) {

					// Check if the adapter supports Direct3D 12.
					Microsoft::WRL::ComPtr<ID3D12Device> dDevice;
					hRes = pD3D12CreateDevice( paAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &dDevice ) );
					if ( SUCCEEDED( hRes ) ) {
						// This should be the adapter.
						// Create command queue
						D3D12_COMMAND_QUEUE_DESC cqdQueueDesc = {
							.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
							.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
						};
						Microsoft::WRL::ComPtr<ID3D12CommandQueue> cqQueue;
						if ( FAILED( dDevice->CreateCommandQueue( &cqdQueueDesc, IID_PPV_ARGS( &cqQueue ) ) ) ) { return false; }

						// Get client rect size.
						RECT rClient;
						::GetClientRect( _hWnd, &rClient );
						// Create swap chain.
						DXGI_SWAP_CHAIN_DESC1 scdSwapChainDesc = {};
						scdSwapChainDesc.BufferCount = 2;
						scdSwapChainDesc.Width = static_cast<UINT>(rClient.right - rClient.left);
						scdSwapChainDesc.Height = static_cast<UINT>(rClient.bottom - rClient.top);
						scdSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						scdSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
						scdSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
						scdSwapChainDesc.SampleDesc.Count = 1;

						Microsoft::WRL::ComPtr<IDXGISwapChain1> scChain1;
						if ( FAILED( fFactory->CreateSwapChainForHwnd(
							cqQueue.Get(),
							_hWnd,
							&scdSwapChainDesc,
							nullptr,
							nullptr,
							&scChain1 ) ) ) { return false; }

						if ( FAILED( scChain1.As( &m_scSwapChain ) ) ) { return false; }

						// Create synchronization objects.
						Microsoft::WRL::ComPtr<ID3D12Fence> pfFence;
						if ( FAILED( dDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &pfFence ) ) ) ) { return false; }
						m_hFenceEvent = ::CreateEventW( nullptr, FALSE, FALSE, nullptr );
						if ( !m_hFenceEvent ) { return false; }

						m_pfFence = std::move( pfFence );
						m_ui64FenceValue = 0;
						m_cqCommandQueue = std::move( cqQueue );
						m_pd3dDevice = std::move( dDevice );
						m_pfFactory = std::move( fFactory );
						m_hLib = std::move( hLibD3d );
						m_hDxgiLib = std::move( hLib );
						return true;
					}
				}
				++I;
			}
		}
		catch ( ... ) { return false; }

		return false;
	}

	/**
	 * Flushes the command queue, blocking the CPU until the GPU has finished all pending operations.
	 *
	 * \return Returns true if the flush was successful.
	 **/
	bool CDirectX12Device::FlushCommandQueue() {
		if ( !m_cqCommandQueue.Get() || !m_pfFence.Get() || !m_hFenceEvent ) { return false; }

		m_ui64FenceValue++;
		if ( FAILED( m_cqCommandQueue->Signal( m_pfFence.Get(), m_ui64FenceValue ) ) ) { return false; }

		if ( m_pfFence->GetCompletedValue() < m_ui64FenceValue ) {
			if ( FAILED( m_pfFence->SetEventOnCompletion( m_ui64FenceValue, m_hFenceEvent ) ) ) { return false; }
			::WaitForSingleObject( m_hFenceEvent, INFINITE );
		}
		return true;
	}

	/**
	 * Resizes the swap chain backbuffers to match the current target window dimensions.
	 * Note: It is the caller's responsibility to release any active Render Target Views referencing the swap chain before calling this.
	 *
	 * \return Returns true if the swap chain was successfully resized.
	 **/
	bool CDirectX12Device::ResizeSwapChain() {
		if ( !m_scSwapChain.Get() ) { return false; }

		if ( !FlushCommandQueue() ) { return false; }

		DXGI_SWAP_CHAIN_DESC1 scdDesc;
		if ( FAILED( m_scSwapChain->GetDesc1( &scdDesc ) ) ) { return false; }

		// Passing 0 for width and height automatically uses the client area of the target HWND.
		HRESULT hRes = m_scSwapChain->ResizeBuffers(
			scdDesc.BufferCount,
			0,
			0,
			scdDesc.Format,
			scdDesc.Flags
		);

		return SUCCEEDED( hRes );
	}

	/**
	 * Frees all resources used by this object and leaves the object in a valid reusable state.
	 **/
	void CDirectX12Device::Reset() {
		if ( m_hFenceEvent ) {
			::CloseHandle( m_hFenceEvent );
			m_hFenceEvent = NULL;
		}
		m_ui64FenceValue = 0;
		m_pfFence.Reset();
		m_scSwapChain.Reset();
		m_cqCommandQueue.Reset();
		m_pd3dDevice.Reset();
		m_pfFactory.Reset();
		m_hLib.Reset();
		m_hDxgiLib.Reset();
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
