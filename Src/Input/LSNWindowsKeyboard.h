#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard keyboard accessed through the Win32 API.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNDirectInputDevice8.h"
#include "LSNUsbControllerBase.h"


namespace lsn {

	/**
	 * Class CWindowsKeyboard
	 * \brief A standard keyboard accessed through the Win32 API.
	 *
	 * Description: A standard keyboard accessed through the Win32 API.
	 */
	class CWindowsKeyboard : public CUsbControllerBase {
	public :
		CWindowsKeyboard();
		virtual ~CWindowsKeyboard();


		// == Functions.
		/**
		 * Tells the controller to poll its device.  The polled data should not be inspected yet to determine which keys are held, it simply
		 *	tells the object to poll the controller for its data to then be gathered immediately after.
		 *
		 * \return Returns true if polling was successful.
		 */
		virtual bool											Poll();

	protected :
		// == Members.
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
