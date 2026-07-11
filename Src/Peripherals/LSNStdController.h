/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard 8-button controller.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNPeripheralBase.h"

namespace lsn {

	/**
	 * Class CStdController
	 * \brief A standard 8-button controller.
	 *
	 * Description: A standard 8-button controller.
	 */
	class CStdController : public IPeripheralBase {
	public :
		// == Enumerations.
		/** The input bits. */
		enum LSN_INPUT_BITS : uint8_t {
			LSN_IB_A						= (1 << 7),			/**< The A button. */
			LSN_IB_B						= (1 << 6),			/**< The B button. */
			LSN_IB_SELECT					= (1 << 5),			/**< The Select button. */
			LSN_IB_START					= (1 << 4),			/**< The Start button. */
			LSN_IB_UP						= (1 << 3),			/**< The Up button. */
			LSN_IB_DOWN						= (1 << 2),			/**< The Down button. */
			LSN_IB_LEFT						= (1 << 1),			/**< The Left button. */
			LSN_IB_RIGHT					= (1 << 0),			/**< The Right button. */
		};

	
		// == Functions.
		/**
		 * Handles a read of $4016/$4017.
		 * 
		 * \return Returns the desired read value.
		 **/
		virtual uint8_t									Read();

		/**
		 * Handles the write to $4016.
		 * 
		 * \param _ui8Val The value being written.
		 **/
		virtual void									Write( uint8_t _ui8Val );

		/**
		 * Called when the console is reset and at start-up.
		 **/
		virtual void									Reset();

		/**
		 * Called when controllers are detached from the system.
		 **/
		virtual void									ControllerDetached();

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
			LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord );


	protected :
		// == Members.
		/** The 8-bit result of polling. **/
		uint8_t											m_ui8Status = 0;
	};

}	// namespace lsn
