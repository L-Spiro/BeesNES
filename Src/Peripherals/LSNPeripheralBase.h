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

#include <set>
#include <vector>


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


		// == Enumerations.
		/** The input bits. */
		enum LSN_INPUT_BITS : uint8_t {
			LSN_IB_A									= (1 << 7),							/**< The A button. */
			LSN_IB_B									= (1 << 6),							/**< The B button. */
			LSN_IB_SELECT								= (1 << 5),							/**< The Select button. */
			LSN_IB_START								= (1 << 4),							/**< The Start button. */
			LSN_IB_UP									= (1 << 3),							/**< The Up button. */
			LSN_IB_DOWN									= (1 << 2),							/**< The Down button. */
			LSN_IB_LEFT									= (1 << 1),							/**< The Left button. */
			LSN_IB_RIGHT								= (1 << 0),							/**< The Right button. */
		};

		/** The controller buttons in the order in which they appear in the options settings. */
		enum LSN_BUTTONS {
			LSN_B_UP,																		/**< The Up button. */
			LSN_B_LEFT,																		/**< The Left button. */
			LSN_B_RIGHT,																	/**< The Right button. */
			LSN_B_DOWN,																		/**< The Down button. */
			LSN_B_SELECT,																	/**< The Select button. */
			LSN_B_START,																	/**< The Start button. */
			LSN_B_B,																		/**< The B button. */
			LSN_B_A,																		/**< The A button. */

			LSN_B_TOTAL,																	/**< The total number of buttons on a controller (8). */
		};

		/** The total number of inputs sets. */
		enum LSN_INPUT_SETS {
			LSN_IS_INPUT_SETS							= 8,								/**< One set for every button on a standard controller. */
		};


		// == Types.
		/** When assigning keys, controllers need to know which devices have been "consumed" by other controllers.
		 *	For example, if controller 1 has consumed inputs from the USB controller in USB #1, and USB #2 has the same type of controller,
		 *	player 2's control will be assigned using USB #2's controller.
		 */
		struct LSN_INPUT_CONFIGURATION_APPLICATION_RECORD {
			// == Types.
			/** A peripheral/USB Controller pairing. */
			struct LSN_PERIPHERAL_USB_PAIR {
				IPeripheralBase *						ppbPeripheral = nullptr;			/**< The peripheral using the controller. */
				CUsbControllerBase *					pucbController = nullptr;			/**< The controller being used by the peripheral. */


				// == Operators.
				/**
				 * Less-than operator for sorting in a std::map.
				 * 
				 * \param _pupOther The other pair to compare against.
				 * \return Returns true if this pair is strictly less than the given pair based on uintptr_t casting.
				 */
				bool									operator < ( const LSN_PERIPHERAL_USB_PAIR &_pupOther ) const {
					if ( reinterpret_cast<uintptr_t>(ppbPeripheral) != reinterpret_cast<uintptr_t>(_pupOther.ppbPeripheral) ) {
						return reinterpret_cast<uintptr_t>(ppbPeripheral) < reinterpret_cast<uintptr_t>(_pupOther.ppbPeripheral);
					}
					return reinterpret_cast<uintptr_t>(pucbController) < reinterpret_cast<uintptr_t>(_pupOther.pucbController);
				}

				/**
				 * Equality operator.
				 * 
				 * \param _pupOther The other pair to compare against.
				 * \return Returns true if both pointers in the pairs are equal.
				 */
				bool									operator == ( const LSN_PERIPHERAL_USB_PAIR &_pupOther ) const {
					return (reinterpret_cast<uintptr_t>(ppbPeripheral) == reinterpret_cast<uintptr_t>(_pupOther.ppbPeripheral)) &&
						(reinterpret_cast<uintptr_t>(pucbController) == reinterpret_cast<uintptr_t>(_pupOther.pucbController));
				}
			};

			/** Specific button mappings that have already been made. */
			struct LSN_USED_MAPPING {
				IPeripheralBase *						ppbPeripheral = nullptr;			/**< The peripheral using the controller. */
				CUsbControllerBase *					pucbController = nullptr;			/**< The controller being used by the peripheral. */
				lsn::LSN_INPUT_EVENT					ieEvent;							/**< The input event. */
				size_t									sButtonIdx = size_t( -1 );			/**< The index of the button on the peripheral. */
			};

			// == Members.
			std::set<LSN_PERIPHERAL_USB_PAIR>			sPairings;							/**< Pairings between USB controllers and peripherals. */
			std::vector<LSN_USED_MAPPING>				vAssignments;						/**< A logged button assignment to a USB controller. */
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
		 * \param _ieConfig The configuration to apply.  [LSN_IS_INPUT_SETS] configurations, [LSN_B_TOTAL] buttons.
		 * \param _ieTurboConfig The turbo configuration to apply.  [LSN_IS_INPUT_SETS] configurations, [LSN_B_TOTAL] buttons.
		 * \param _vUsbControllers The attached USB controllers/devices.
		 * \param _icarRecord An in/out record of what controllers have been accessed and how.
		 * \param _bStrictApply If true, USB controllers should only connect to the specific devices that were used to configur inputs.
		 * \return Returns true if all buttons were able to be applied.
		 **/
		virtual bool									ApplyConfiguration( lsn::LSN_INPUT_EVENT _ieConfig[LSN_IS_INPUT_SETS][LSN_B_TOTAL], lsn::LSN_INPUT_EVENT _ieTurboConfig[LSN_IS_INPUT_SETS][LSN_B_TOTAL],
			const std::vector<CUsbControllerBase *> &_vUsbControllers,
			LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord,
			bool _bStrictApply ) = 0;
	};

}	// namespace lsn
