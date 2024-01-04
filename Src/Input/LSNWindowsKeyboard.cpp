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
		/*m_bmButtonMap.ui16Map[LSN_B_A] = VK_OEM_1;
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
		m_bmRapidMap.ui16Map[LSN_B_RIGHT] = 'E';*/
		
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

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
