/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of DirectX 9 functionality.
 */


#pragma once

#ifdef LSN_DX9

#include "../../LSNLSpiroNes.h"
#include "../../OS/LSNWindows.h"
#include "../LSNGpuBase.h"

#include <d3d9.h>
#include <d3dx9shader.h>
#include <vector>

namespace lsn {

	// == Types.
	/**
	 * Class CDirectX9
	 * \brief The root of DirectX 9 functionality.
	 *
	 * Description: The root of DirectX 9 functionality.
	 */
	class CDirectX9 {
	public :
		// == Types.
		typedef IDirect3D9 * (WINAPI *							PFN_Direct3DCreate9)( UINT );

		struct LSN_ADAPTOR_INFO {
			UINT												uiAdapter;
			D3DADAPTER_IDENTIFIER9								aiIdentifier;
			D3DCAPS9											cCaps;
		};



		// == Functions.
		/**
		 * Cleans up any resources used by Direct3D 9.
		 **/
		static void												ShutDown();

		/**
		 * Determines whether DirectX 9 is available.
		 *
		 * \return Returns TRUE if DirectX 9 functionality is available.
		 **/
		static inline BOOL										Supported();

		/**
		 * Enumerates display devices via Direct3D 9.
		 * 
		 * \param _pd3d9Dx9 The Direct3D 9 device pointer.
		 * \param _vDevices Display devices are added to _vDevices.
		 * \return Returns true if no allocations issues were encountered while adding display devices to _vDevices.
		 **/
		static bool												EnumerateDisplayDevices( IDirect3D9 * _pd3d9Dx9, std::vector<LSN_ADAPTOR_INFO> &_vDevices );

		/**
		 * Safely release an object.
		 *
		 * \param _tObj The object to release.
		 */
		template <typename _tnType>
		static inline void										SafeRelease( _tnType &_tObj );


	protected :
		// == Members.
		static BOOL												m_bSupported;								/**< Is Direct3D 9 supported? */
		static std::vector<LSN_ADAPTOR_INFO>					m_vDisplayDevices;							/**< The array of display devices. */


		// == Functions.
		/**
		 * Checks for Direct3D 9 support.
		 * 
		 * \return Returns true if Direct3D 9 is supported.
		 **/
		static bool												IsSupported();

		/**
		 * Populates m_vDisplayDevices.
		 * 
		 * \param _pd3d9Dx9 The Direct3D 9 device pointer.
		 **/
		static void												GatherDevices( IDirect3D9 * _pd3d9Dx9 );
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Safely release an object.
	 *
	 * \param _tObj The object to release.
	 */
	template <typename _tnType>
	inline void CDirectX9::SafeRelease( _tnType &_tObj ) {
		if ( _tObj ) {
			_tObj->Release();
			_tObj = NULL;
		}
	}

	/**
	 * Determines whether DirectX 9 is available.
	 *
	 * \return Returns TRUE if DirectX 9 functionality is available.
	 **/
	inline BOOL CDirectX9::Supported() {
		if ( m_bSupported == 3 ) { m_bSupported = IsSupported(); }
		return m_bSupported;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
