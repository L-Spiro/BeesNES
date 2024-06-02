/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 9 GPU object.
 */


#pragma once

#ifdef LSN_DX9

#include "../../LSNLSpiroNes.h"
#include "../LSNGpuBase.h"
#include "LSNDirectX9.h"

#include <wrl/client.h>

namespace lsn {

	/**
	 * Class CDirectX9Device
	 * \brief The DirectX 9 GPU object.
	 *
	 * Description: The DirectX 9 GPU object.
	 */
	class CDirectX9Device : public CGpuBase {
	public :
		CDirectX9Device();
		virtual ~CDirectX9Device();


		// == Functions.
		/**
		 * Creates a Direct3D 9 device.
		 *
		 * \param _hWnd The window to which to attach.
		 * \param _sAdapter The adapter to use.
		 * \return Returns true if the device was created.
		 **/
		virtual bool											Create( HWND _hWnd, const std::string &_sAdapter );

	protected :
		// == Members.
		Microsoft::WRL::ComPtr<IDirect3D9>						m_pdD3d;								/**< The Direct3D 9 COM interface. */
		Microsoft::WRL::ComPtr<IDirect3DDevice9>				m_pd3dDevice;							/**< The direct3D 9 device. */

	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
