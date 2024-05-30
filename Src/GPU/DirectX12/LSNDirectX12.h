/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of DirectX 12 functionality.
 */


#pragma once

#ifdef LSN_DX12

#include "../../LSNLSpiroNes.h"
#include "../../OS/LSNWindows.h"
#include "../LSNGpuBase.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

namespace lsn {

	/**
	 * Class CDirectX12
	 * \brief The root of DirectX 12 functionality.
	 *
	 * Description: The root of DirectX 12 functionality.
	 */
	class CDirectX12 {
	public :
		// == Types.
		typedef HRESULT (WINAPI *								PFN_CreateDXGIFactory1)( REFIID, _COM_Outptr_ void ** );
		typedef HRESULT (WINAPI*								PFN_D3D12CreateDevice)( _In_opt_ IUnknown *, D3D_FEATURE_LEVEL, _In_ REFIID, _COM_Outptr_opt_ void ** );


		// == Functions.
		/**
		 * Cleans up any resources used by Direct3D 12.
		 **/
		static void												ShutDown();

		/**
		 * Determines whether DirectX 12 is available.
		 *
		 * \return Returns TRUE if DirectX 12 functionality is available.
		 **/
		static inline BOOL										Supported();

		/**
		 * Enumerates display devices via Direct3D 12.
		 * 
		 * \param _pfFactory The DXGI factory.
		 * \param _vDevices Display devices are added to _vDevices.
		 * \return Returns true if no allocations issues were encountered while adding display devices to _vDevices.
		 **/
		static bool												EnumerateDisplayDevices( IDXGIFactory6 * _pfFactory, std::vector<DXGI_ADAPTER_DESC1> &_vDevices );


	protected :
		// == Members.
		static BOOL												m_bSupported;								/**< Is Direct3D 12 supported? */
		static std::vector<DXGI_ADAPTER_DESC1>					m_vDisplayDevices;							/**< The array of display devices. */


		// == Functions.
		/**
		 * Checks for Direct3D 12 support.
		 * 
		 * \return Returns true if Direct3D 12 is supported.
		 **/
		static bool												IsSupported();

		/**
		 * Populates m_vDisplayDevices.
		 * 
		 * \param _pfFactory The DXGI factory.
		 **/
		static void												GatherDevices( IDXGIFactory6 * _pfFactory );
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Determines whether DirectX 12 is available.
	 *
	 * \return Returns TRUE if DirectX 12 functionality is available.
	 **/
	inline BOOL CDirectX12::Supported() {
		if ( m_bSupported == 3 ) { m_bSupported = IsSupported(); }
		return m_bSupported;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
