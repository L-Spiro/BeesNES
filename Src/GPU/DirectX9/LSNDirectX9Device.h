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

#include <Widget/LSWWidget.h>

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
		 * Resets the device back to scratch.  Everything that depends on it must already be reset.
		 **/
		void													Reset();

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

		/**
		 * \brief Reset the device to match the current client-area size of a window.
		 *
		 * Updates \c m_ppPresentParms (backbuffer size, \c hDeviceWindow) from the given window,
		 * calls \c CDirectX9LosableResourceManager::OnLostDevice(), performs \c Reset(&m_ppPresentParms),
		 * then calls \c CDirectX9LosableResourceManager::OnResetDevice().
		 *
		 * \param _hWnd The target window whose client-area size determines the new backbuffer size.
		 *              If \c nullptr, the cached \c m_hWnd is used.
		 * \return Returns \c true on success; \c false if the device is still lost or \c Reset() failed.
		 **/
		bool													ResetForWindowSize( HWND _hWnd );

		/**
		 * \brief Check cooperative level and recover from device loss when possible.
		 *
		 * When \c TestCooperativeLevel() returns \c D3DERR_DEVICENOTRESET, this attempts a \c Reset()
		 * using the current client-area size (via \c ResetForWindowSize()).
		 *
		 * \param _hWnd The window used to refresh \c m_ppPresentParms on reset. If \c nullptr,
		 *              the cached \c m_hWnd is used.
		 * \return Returns \c true if the device is usable for this frame (OK or reset succeeded);
		 *         \c false if still lost and rendering should be skipped this frame.
		 **/
		bool													HandleDeviceLoss( HWND _hWnd );


	protected :
		// == Members.
		Microsoft::WRL::ComPtr<IDirect3D9>						m_pdD3d;								/**< The Direct3D 9 COM interface. */
		Microsoft::WRL::ComPtr<IDirect3DDevice9>				m_pd3dDevice;							/**< The Direct3D 9 device. */
		HWND													m_hWnd = NULL;							/**< The window to which we are attached. */
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
