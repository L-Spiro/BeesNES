/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base class for a USB controller.
 */

#include "LSNUsbControllerBase.h"


namespace lsn {

	CUsbControllerBase::CUsbControllerBase() {
	}
	CUsbControllerBase::~CUsbControllerBase() {
	}


	// == Functions.
	/**
	 * Determines if the A button is pressed.
	 *
	 * \return Returns true if the A button is pressed.
	 */
	bool CUsbControllerBase::IsAPressed() { return false; }

	/**
	 * Determines if the B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CUsbControllerBase::IsBPressed() { return false; }
		
	/**
	 * Determines if the Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CUsbControllerBase::IsSelectPressed() { return false; }

	/**
	 * Determines if the Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CUsbControllerBase::IsStartPressed() { return false; }

	/**
	 * Determines if the Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CUsbControllerBase::IsUpPressed() { return false; }

	/**
	 * Determines if the Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CUsbControllerBase::IsDownPressed() { return false; }

	/**
	 * Determines if the Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CUsbControllerBase::IsLeftPressed() { return false; }

	/**
	 * Determines if the Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CUsbControllerBase::IsRightPressed() { return false; }

	/**
	 * Determines if the turbo A button is pressed.
	 *
	 * \return Returns true if the A button is pressed.
	 */
	bool CUsbControllerBase::IsATurboPressed() { return false; }

	/**
	 * Determines if the turbo B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CUsbControllerBase::IsBTurboPressed() { return false; }
	
	/**
	 * Determines if the turbo Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CUsbControllerBase::IsSelectTurboPressed() { return false; }

	/**
	 * Determines if the turbo Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CUsbControllerBase::IsStartTurboPressed() { return false; }

	/**
	 * Determines if the turbo Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CUsbControllerBase::IsUpTurboPressed() { return false; }

	/**
	 * Determines if the turbo Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CUsbControllerBase::IsDownTurboPressed() { return false; }

	/**
	 * Determines if the turbo Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CUsbControllerBase::IsLeftTurboPressed() { return false; }
		
	/**
	 * Determines if the turbo Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CUsbControllerBase::IsRightTurboPressed() { return false; }

}	// namespace lsn
