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

		/**
		 * Gets a pointer to the Direct3D 9 device.
		 *
		 * \return Returns a pointer to the Direct3D 9 device.
		 **/
		inline IDirect3DDevice9 *								GetDirectX9Device();

		/**
		 * Gets the creation flags.
		 *
		 * \return Returns the creation flags.
		 **/
		inline DWORD											Flags() const { return m_dwFlags; }

		/**
		 * Gets the present parameters.
		 *
		 * \return Returns the present parameters.
		 **/
		inline const D3DPRESENT_PARAMETERS &					PresentParameters() const { return m_ppPresentParms; }


	protected :
		// == Members.
		Microsoft::WRL::ComPtr<IDirect3D9>						m_pdD3d;								/**< The Direct3D 9 COM interface. */
		Microsoft::WRL::ComPtr<IDirect3DDevice9>				m_pd3dDevice;							/**< The Direct3D 9 device. */
		DWORD													m_dwFlags = 0;							/**< Flags used to create the device. */
		D3DPRESENT_PARAMETERS									m_ppPresentParms;						/**< The present parameters used to create the device. */

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Gets a pointer to the Direct3D 9 device.
	 *
	 * \return Returns a pointer to the Direct3D 9 device.
	 **/
	inline IDirect3DDevice9 * CDirectX9Device::GetDirectX9Device() { return m_pd3dDevice.Get(); }

}	// namespace lsn

#endif	// #ifdef LSN_DX9
