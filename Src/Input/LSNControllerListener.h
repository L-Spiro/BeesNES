/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base class for an object that wants to listen to a controller for any button being pressed (for controller-configuration purposes).
 *	This class provides an object with a way to receive notifications about button presses on a USB controller.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNUsbControllerBase.h"

namespace lsn {

	class CUsbControllerBase;

	/**
	 * Class CControllerListener
	 * \brief A base class for an object that wants to listen to a controller for any button being pressed.
	 *
	 * Description: A base class for an object that wants to listen to a controller for any button being pressed (for controller-configuration purposes).
	 *	This class provides an object with a way to receive notifications about button presses on a USB controller.
	 */
	class CControllerListener {
	public :
		// == Functions.
		/**
		 * Called when a controller input event is generated.
		 * 
		 * \param _pucbController A pointer to the controller that triggered the event.
		 * \param _ieEvent A constant reference to the input event data.
		 **/
		virtual void											OnInput( CUsbControllerBase * /*_pucbController*/, const CUsbControllerBase::LSN_INPUT_EVENT &/*_ieEvent*/ ) {}
	};

}	// namespace lsn
