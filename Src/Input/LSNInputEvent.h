/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An input event for triggering a button press in a game.  An input event can come from one of different device types (keyboard or USB controller),
 *	come from a specific device instance of that type (a specific USB controller model), be one of multiple types (axis, POV, or button), and come from one of
 *	multiple instances (axis index, POV index, or button index).
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNUsbControllerBase.h"

#if defined( LSN_WINDOWS )
#include <Helpers/LSWHelpers.h>
#elif defined( LSN_APPLE )
#include <Carbon/Carbon.h>
#endif	// #if defined( LSN_WINDOWS )

namespace lsn {

	/**
	 * Class LSN_INPUT_EVENT
	 * \brief An input event for triggering a button press in a game.
	 *
	 * Description: An input event for triggering a button press in a game.  An input event can come from one of different device types (keyboard or USB controller),
	 *	come from a specific device instance of that type (a specific USB controller model), be one of multiple types (axis, POV, or button), and come from one of
	*	multiple instances (axis index, POV index, or button index).
	 */
	struct LSN_INPUT_EVENT {
		// == Enumerations.
		/** Device types. */
		enum LSN_DEVICE_TYPE : uint8_t {
			LSN_DT_KEYBOARD,																/**< The event comes from the system keyboard. */
			LSN_DT_USB_CONTROLLER,															/**< The event comes from a USB controller. */
		};


		// == Members.
		LSN_DEVICE_TYPE										dtType;							/**< The device type (keyboard or USB controller. */

		union {
			struct {
#if defined( LSN_WINDOWS )
				lsw::LSW_KEY								kKey;							/**< If LSN_DEVICE_TYPE is LSN_DT_KEYBOARD. */
#elif defined( LSN_APPLE )
				CGKeyCode									kcKey;							/**< The keyboard keycode. */
#endif	// #if defined( LSN_WINDOWS )
			}												kb;
			struct {
#if defined( LSN_WINDOWS )
				GUID										guId;							/**< The USB controller's product ID. */
#endif	// #if defined( LSN_WINDOWS )
				CUsbControllerBase::LSN_INPUT_EVENT			ieEvent;						/**< The controller event */
				float										fDeadzone;						/**< The dead zone if the input is an axis. */
			}												cont;
		}													u;
	};

}	// namespace lsn
