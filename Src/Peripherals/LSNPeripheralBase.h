/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for all peripherals.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Input/LSNUsbControllerBase.h"
#include "../Options/LSNInputOptions.h"

namespace lsn {

	/**
	 * Class IPeripheralBase
	 * \brief The base class for all peripherals.
	 *
	 * Description: The base class for all peripherals.
	 */
	class IPeripheralBase {
	public :
		virtual ~IPeripheralBase() = default;


		// == Types.
		/** When assigning keys, controllers need to know which devices have been "consumed" by other controllers.
		 *	For example, if controller 1 has consumed inputs from the USB controller in USB #1, and USB #2 has the same type of controller,
		 *	player 2's control will be assigned using USB #2's controller.
		 */
		struct LSN_INPUT_CONFIGURATION_APPLICATION_RECORD {
		};


		// == Functions.
		/**
		 * Handles a read of $4016/$4017.
		 * 
		 * \return Returns the desired read value.
		 **/
		virtual uint8_t									Read() = 0;

		/**
		 * Handles the write to $4016.
		 * 
		 * \param _ui8Val The value being written.
		 **/
		virtual void									Write( uint8_t /*_ui8Val*/ ) = 0;

		/**
		 * Called when the console is reset and at start-up.
		 **/
		virtual void									Reset() = 0;

		/**
		 * Called when controllers are detached from the system.
		 **/
		virtual void									ControllerDetached() {}

		/**
		 * Applies controller configurations to the perpheral.
		 * 
		 * \param _ieConfig The configuration to apply.  [4] configurations, [8] buttons.
		 * \param _ieTurboConfig The turbo configuration to apply.  [4] configurations, [8] buttons.
		 * \param _vUsbControllers The attached USB controllers/devices.
		 * \param _icarRecord An in/out record of what controllers have been accessed and how.
		 * \return Returns true if all buttons were able to be applied.
		 **/
		virtual bool									ApplyConfiguration( lsn::LSN_INPUT_EVENT _ieConfig[4][8], lsn::LSN_INPUT_EVENT _ieTurboConfig[4][8],
			const std::vector<CUsbControllerBase *> &_vUsbControllers,
			LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord ) = 0;
	};

}	// namespace lsn
