/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base class for a USB controller.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CUsbControllerBase
	 * \brief A wrapper around the global IDirectInput8W * object.
	 *
	 * Description: A wrapper around the global IDirectInput8W * object.
	 */
	class CUsbControllerBase {
	public :
		CUsbControllerBase();
		virtual ~CUsbControllerBase();


		// == Functions.
		/**
		 * Tells the controller to poll its device.  The polled data should not be inspected yet to determine which keys are held, it simply
		 *	tells the object to poll the controller for its data to then be gathered immediately after.
		 *
		 * \return Returns true if polling was successful.
		 */
		virtual bool											Poll() { return false; }

		/**
		 * Determines if the A button is pressed.
		 *
		 * \return Returns true if the A button is pressed.
		 */
		virtual bool											IsAPressed();

		/**
		 * Determines if the B button is pressed.
		 *
		 * \return Returns true if the B button is pressed.
		 */
		virtual bool											IsBPressed();

		/**
		 * Determines if the Select button is pressed.
		 *
		 * \return Returns true if the Select button is pressed.
		 */
		virtual bool											IsSelectPressed();

		/**
		 * Determines if the Start button is pressed.
		 *
		 * \return Returns true if the Start button is pressed.
		 */
		virtual bool											IsStartPressed();

		/**
		 * Determines if the Up button is pressed.
		 *
		 * \return Returns true if the Up button is pressed.
		 */
		virtual bool											IsUpPressed();

		/**
		 * Determines if the Down button is pressed.
		 *
		 * \return Returns true if the Down button is pressed.
		 */
		virtual bool											IsDownPressed();

		/**
		 * Determines if the Left button is pressed.
		 *
		 * \return Returns true if the Left button is pressed.
		 */
		virtual bool											IsLeftPressed();

		/**
		 * Determines if the Right button is pressed.
		 *
		 * \return Returns true if the Right button is pressed.
		 */
		virtual bool											IsRightPressed();

		/**
		 * Determines if the turbo A button is pressed.
		 *
		 * \return Returns true if the A button is pressed.
		 */
		virtual bool											IsATurboPressed();

		/**
		 * Determines if the turbo B button is pressed.
		 *
		 * \return Returns true if the B button is pressed.
		 */
		virtual bool											IsBTurboPressed();

		/**
		 * Determines if the turbo Select button is pressed.
		 *
		 * \return Returns true if the Select button is pressed.
		 */
		virtual bool											IsSelectTurboPressed();

		/**
		 * Determines if the turbo Start button is pressed.
		 *
		 * \return Returns true if the Start button is pressed.
		 */
		virtual bool											IsStartTurboPressed();

		/**
		 * Determines if the turbo Up button is pressed.
		 *
		 * \return Returns true if the Up button is pressed.
		 */
		virtual bool											IsUpTurboPressed();

		/**
		 * Determines if the turbo Down button is pressed.
		 *
		 * \return Returns true if the Down button is pressed.
		 */
		virtual bool											IsDownTurboPressed();

		/**
		 * Determines if the turbo Left button is pressed.
		 *
		 * \return Returns true if the Left button is pressed.
		 */
		virtual bool											IsLeftTurboPressed();

		/**
		 * Determines if the turbo Right button is pressed.
		 *
		 * \return Returns true if the Right button is pressed.
		 */
		virtual bool											IsRightTurboPressed();

	protected :
		// == Members.

	};

}	// namespace lsn
