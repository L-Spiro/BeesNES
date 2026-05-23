/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 12 GPU object.
 */


#pragma once

#ifdef LSN_DX12

#include "../../LSNLSpiroNes.h"
#include "../LSNGpuBase.h"
#include "LSNDirectX12.h"

#include <wrl/client.h>

namespace lsn {

	/**
	 * Class CDirectX12Device
	 * \brief The DirectX 12 GPU object.
	 *
	 * Description: The DirectX 12 GPU object.
	 */
	class CDirectX12Device : public CGpuBase {
	public :
		CDirectX12Device();
		virtual ~CDirectX12Device();


		// == Functions.
		/**
		 * Creates a Direct3D 12 device.
		 *
		 * \param _hWnd The window to which to attach.
		 * \param _sAdapter The adapter to use.
		 * \return Returns true if the device was created.
		 **/
		virtual bool											Create( HWND _hWnd, const std::string &_sAdapter );

		/**
		 * Gets a pointer to the device.
		 *
		 * \return Returns a pointer to the device.
		 **/
		ID3D12Device *											GetDevice() { return m_pd3dDevice.Get(); }

		/**
		 * Gets a pointer to the command queue.
		 *
		 * \return Returns a pointer to the command queue.
		 **/
		ID3D12CommandQueue *									GetCommandQueue() { return m_cqCommandQueue.Get(); }

		/**
		 * Gets a pointer to the swap chain.
		 *
		 * \return Returns a pointer to the swap chain.
		 **/
		IDXGISwapChain4 *										GetSwapChain() { return m_scSwapChain.Get(); }

		/**
		 * Resizes the swap chain backbuffers to match the current target window dimensions.
		 * Note: It is the caller's responsibility to release any active Render Target Views referencing the swap chain before calling this.
		 *
		 * \return Returns true if the swap chain was successfully resized.
		 **/
		bool													ResizeSwapChain();

		/**
		 * Flushes the command queue, blocking the CPU until the GPU has finished all pending operations.
		 *
		 * \return Returns true if the flush was successful.
		 **/
		bool													FlushCommandQueue();
		
		/**
		 * Frees all resources used by this object and leaves the object in a valid reusable state.
		 **/
		virtual void											Reset();

	protected :
		// == Members.
		lsw::LSW_HMODULE										m_hDxgiLib;								/**< The DXGI DLL. */
		Microsoft::WRL::ComPtr<IDXGIFactory6>					m_pfFactory;							/**< The Direct3D 12 COM interface. */
		Microsoft::WRL::ComPtr<ID3D12Device>					m_pd3dDevice;							/**< The direct3D 12 device. */
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>				m_cqCommandQueue;						/**< The primary command queue. */
		Microsoft::WRL::ComPtr<IDXGISwapChain4>					m_scSwapChain;							/**< The swapchain. */
		Microsoft::WRL::ComPtr<ID3D12Fence>						m_pfFence;								/**< The fence used to flush the command queue. */
		UINT64													m_ui64FenceValue = 0;					/**< The current fence value. */
		HANDLE													m_hFenceEvent = NULL;					/**< The event used for fence synchronization. */

	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
