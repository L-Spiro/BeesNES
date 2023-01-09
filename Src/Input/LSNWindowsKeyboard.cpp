#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard keyboard accessed through the Win32 API.
 */

#include "LSNWindowsKeyboard.h"

namespace lsn {

	CWindowsKeyboard::CWindowsKeyboard() {
	}
	CWindowsKeyboard::~CWindowsKeyboard() {
	}

	// == Functions.
	/**
	 * Tells the controller to poll its device.  The polled data should not be inspected yet to determine which keys are held, it simply
	 *	tells the object to poll the controller for its data to then be gathered immediately after.
	 *
	 * \return Returns true if polling was successful.
	 */
	bool CWindowsKeyboard::Poll() {
		return true;
	}

	/**
	 * Determines if the A button is pressed.
	 *
	 * \return Returns true if the A button is pressed.
	 */
	bool CWindowsKeyboard::IsAPressed() {
		return (::GetAsyncKeyState( VK_OEM_1 ) & 0x8000) != 0;
	}

	/**
	 * Determines if the B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CWindowsKeyboard::IsBPressed() {
		return (::GetAsyncKeyState( 'L' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CWindowsKeyboard::IsSelectPressed() {
		return (::GetAsyncKeyState( 'O' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CWindowsKeyboard::IsStartPressed() {
		return (::GetAsyncKeyState( 'P' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CWindowsKeyboard::IsUpPressed() {
		return (::GetAsyncKeyState( 'W' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CWindowsKeyboard::IsDownPressed() {
		return (::GetAsyncKeyState( 'S' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CWindowsKeyboard::IsLeftPressed() {
		return (::GetAsyncKeyState( 'A' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CWindowsKeyboard::IsRightPressed() {
		return (::GetAsyncKeyState( 'D' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo A button is pressed.
	 *
	 * \return Returns true if the A button is pressed.
	 */
	bool CWindowsKeyboard::IsATurboPressed() {
		return (::GetAsyncKeyState( VK_OEM_2 ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CWindowsKeyboard::IsBTurboPressed() {
		return (::GetAsyncKeyState( VK_OEM_PERIOD ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CWindowsKeyboard::IsSelectTurboPressed() {
		return (::GetAsyncKeyState( '9' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CWindowsKeyboard::IsStartTurboPressed() {
		return (::GetAsyncKeyState( '0' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CWindowsKeyboard::IsUpTurboPressed() {
		return (::GetAsyncKeyState( '2' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CWindowsKeyboard::IsDownTurboPressed() {
		return (::GetAsyncKeyState( 'X' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CWindowsKeyboard::IsLeftTurboPressed() {
		return (::GetAsyncKeyState( 'Q' ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CWindowsKeyboard::IsRightTurboPressed() {
		return (::GetAsyncKeyState( 'E' ) & 0x8000) != 0;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
