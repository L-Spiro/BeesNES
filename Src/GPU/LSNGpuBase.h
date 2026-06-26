/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the primary interface to the underlying graphics API.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "Helpers/LSWHelpers.h"

namespace lsn {

	/**
	 * Class CGpuBase
	 * \brief The base class for the primary interface to the underlying graphics API.
	 *
	 * Description: The base class for the primary interface to the underlying graphics API.
	 */
	class CGpuBase {
	public :
		CGpuBase();
		virtual ~CGpuBase();


		// == Functions.
		/**
		 * Creates an API device.
		 *
		 * \param _hWnd The window to which to attach.
		 * \param _sAdapter The adapter to use.
		 * \return Returns true if the device was created.
		 **/
		virtual bool											Create( HWND /*_hWnd*/, const std::string &/*_sAdapter*/ );

		/**
		 * Gets a handle to the loaded API DLL.
		 * 
		 * \return Returns a handle to the loaded API-specific DLL.
		 **/
		const lsw::LSW_HMODULE &								Dll() const { return m_hLib; }


	protected :
		// == Members.
		lsw::LSW_HMODULE										m_hLib;									/**< The API DLL. */
	};

}	// namespace lsn
