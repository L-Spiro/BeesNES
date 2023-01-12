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
		// Temporary default.
		m_bmButtonMap.ui16Map[LSN_B_A] = VK_OEM_1;
		m_bmButtonMap.ui16Map[LSN_B_B] = 'L';
		m_bmButtonMap.ui16Map[LSN_B_START] = 'P';
		m_bmButtonMap.ui16Map[LSN_B_SELECT] = 'O';
		m_bmButtonMap.ui16Map[LSN_B_UP] = 'W';
		m_bmButtonMap.ui16Map[LSN_B_DOWN] = 'S';
		m_bmButtonMap.ui16Map[LSN_B_LEFT] = 'A';
		m_bmButtonMap.ui16Map[LSN_B_RIGHT] = 'D';

		m_bmRapidMap.ui16Map[LSN_B_A] = VK_OEM_2;
		m_bmRapidMap.ui16Map[LSN_B_B] = VK_OEM_PERIOD;
		m_bmRapidMap.ui16Map[LSN_B_START] = '0';
		m_bmRapidMap.ui16Map[LSN_B_SELECT] = '9';
		m_bmRapidMap.ui16Map[LSN_B_UP] = '2';
		m_bmRapidMap.ui16Map[LSN_B_DOWN] = 'X';
		m_bmRapidMap.ui16Map[LSN_B_LEFT] = 'Q';
		m_bmRapidMap.ui16Map[LSN_B_RIGHT] = 'E';
		
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
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_A] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CWindowsKeyboard::IsBPressed() {
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_B] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CWindowsKeyboard::IsSelectPressed() {
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_SELECT] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CWindowsKeyboard::IsStartPressed() {
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_START] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CWindowsKeyboard::IsUpPressed() {
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_UP] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CWindowsKeyboard::IsDownPressed() {
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_DOWN] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CWindowsKeyboard::IsLeftPressed() {
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_LEFT] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CWindowsKeyboard::IsRightPressed() {
		return (::GetAsyncKeyState( m_bmButtonMap.ui16Map[LSN_B_RIGHT] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo A button is pressed.
	 *
	 * \return Returns true if the A button is pressed.
	 */
	bool CWindowsKeyboard::IsATurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_A] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CWindowsKeyboard::IsBTurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_B] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CWindowsKeyboard::IsSelectTurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_SELECT] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CWindowsKeyboard::IsStartTurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_START] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CWindowsKeyboard::IsUpTurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_UP] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CWindowsKeyboard::IsDownTurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_DOWN] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CWindowsKeyboard::IsLeftTurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_LEFT] ) & 0x8000) != 0;
	}

	/**
	 * Determines if the turbo Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CWindowsKeyboard::IsRightTurboPressed() {
		return (::GetAsyncKeyState( m_bmRapidMap.ui16Map[LSN_B_RIGHT] ) & 0x8000) != 0;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
