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

	protected :
		// == Members.
		lsw::LSW_HMODULE										m_hDxgiLib;								/**< The DXGI DLL. */
		Microsoft::WRL::ComPtr<IDXGIFactory6>					m_pfFactory;							/**< The Direct3D 12 COM interface. */
		Microsoft::WRL::ComPtr<ID3D12Device>					m_pd3dDevice;							/**< The direct3D 12 device. */
		//Microsoft::WRL::ComPtr<IDXGIAdapter1>					m_aAdapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain4>					m_scSwapChain;							/**< The swapchain. */


	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
