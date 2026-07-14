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
			bool _bStrictApply );

		/**
		 * Determines if the given POV value is Up.
		 * 
		 * \param _ui32Pov The POV value to test for being Up.
		 * \return Returns true if the given POV value is between 315 degrees (Upper-Left) and 45 degrees (Upper-Right).
		 **/
		static inline bool								PovIsUp( uint32_t _ui32Pov ) {
			return _ui32Pov <= 35999 && ((_ui32Pov <= 4500) || (_ui32Pov >= 31500));
		}

		/**
		 * Determines if the given POV value is Left.
		 * 
		 * \param _ui32Pov The POV value to test for being Left.
		 * \return Returns true if the given POV value is between 225 degrees (Lower-Left) and 315 degrees (Upper-Left).
		 **/
		static inline bool								PovIsLeft( uint32_t _ui32Pov ) {
			return _ui32Pov <= 35999 && ((_ui32Pov >= 22500) && (_ui32Pov <= 31500));
		}

		/**
		 * Determines if the given POV value is Right.
		 * 
		 * \param _ui32Pov The POV value to test for being Right.
		 * \return Returns true if the given POV value is between 45 degrees (Upper-Right) and 135 degrees (Lower-Right).
		 **/
		static inline bool								PovIsRight( uint32_t _ui32Pov ) {
			return _ui32Pov <= 35999 && ((_ui32Pov >= 4500) && (_ui32Pov <= 13500));
		}

		/**
		 * Determines if the given POV value is Down.
		 * 
		 * \param _ui32Pov The POV value to test for being Down.
		 * \return Returns true if the given POV value is between 135 degrees (Lower-Right) and 225 degrees (Lower-Left).
		 **/
		static inline bool								PovIsDown( uint32_t _ui32Pov ) {
			return _ui32Pov <= 35999 && ((_ui32Pov >= 13500) && (_ui32Pov <= 22500));
		}


	protected :
		// == Types.
		/** Contains information for run-time polling. */
		struct LSN_RUNTIME_POLL {
			CUsbControllerBase *						pucbController = nullptr;					/**< The controller from which to read, if any. */
			lsn::LSN_INPUT_EVENT						ieEvent;									/**< Holds information for polling. */
			/** The bit pattern for turbo controls. */
			uint64_t									ui64TurboBits = 0b1111000011110000111100001111000011110000111100001111000011110000;
		};

		/** A poll function. */
		typedef bool (*									PfPollFunc)( const LSN_RUNTIME_POLL &_rpPollData );


		// == Members.
		/** The normal assignments for all 8 buttons. */
		std::vector<LSN_RUNTIME_POLL>					m_vMappings[LSN_B_TOTAL];
		/** The turbo assignments for all 8 buttons. */
		std::vector<LSN_RUNTIME_POLL>					m_vTurboMappings[LSN_B_TOTAL];
		/** The polling functions (1-to-1 with m_vMappings) for all 8 normal buttons. */
		std::vector<PfPollFunc>							m_vNormalPollFuncs[LSN_B_TOTAL];
		/** The polling functions (1-to-1 with m_vTurboMappings) for all 8 turbo buttons. */
		std::vector<PfPollFunc>							m_vTurboPollFuncs[LSN_B_TOTAL];
		/** The previous status of each button. */
		uint8_t											m_ui8PrevStatus[LSN_B_TOTAL] = {};
		/** The 8-bit result of polling. **/
		uint8_t											m_ui8Status = 0;
		/** Disallow left/right and up/down being pressed at the same time. */
		bool											m_bDisallowInvalidDpad = true;


		// == Functions.
		/**
		 * Finds a preferred USB controller to use for polling for a given button.
		 * 
		 * \param _ieConfig The input event describing the controller to seek.
		 * \param _vUsbControllers The array of USB controllers attached to the PC.
		 * \param _icarRecord A record of past attachments, used to avoid double-pairing on the same device when _bStrict is false. Ignored when _bStrict is true.
		 * \param _bStrict AIf true, only the specific device specified by _ieConfig.u.cont.guId is selected. Otherwise any device of the same product can be select if it has not been
		 *	already.
		 * \return Returns the preferred controller for the given button or nullptr.
		 **/
		CUsbControllerBase *							GetPreferredController( lsn::LSN_INPUT_EVENT _ieConfig,
			const std::vector<CUsbControllerBase *> &_vUsbControllers,
			LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord, bool _bStrict );

		/**
		 * Determines if the supplied button mapping has already been made.
		 * 
		 * \param _umAssignment The button mapping to check for existing in _icarRecord.
		 * \param _icarRecord The record of mappings to check for the existence of _umAssignment.
		 * \return Returns true if the given button map exists in the record.
		 **/
		bool											CheckForExistingMapping( const LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_USED_MAPPING &_umAssignment,
			const LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord );

		/**
		 * Polls a button, returning 1 if it is pressed, 0 otherwise.
		 * 
		 * \param _bButton The button to poll.
		 * \return Returns 1 if the given button is pressed, 0 otherwise.
		 **/
		uint8_t											Poll( LSN_BUTTONS _bButton );

		/**
		 * Polls the keyboard for a given button.
		 * 
		 * \param _rpPollData Contains which key to poll.
		 * \return Returns true if the given key is pressed.
		 **/
		static bool										KeyBoardPoll( const LSN_RUNTIME_POLL &_rpPollData );

		/**
		 * Polls a USB controller for a given button being pressed.
		 * 
		 * \param _rpPollData Contains the data for which button to poll.
		 * \return Returns true if the assigned button is pressed on the given controller.
		 **/
		static bool										UsbController_ButtonPoll( const LSN_RUNTIME_POLL &_rpPollData );

		/**
		 * Polls a USB controller for the POV being Up.
		 * 
		 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
		 * \return Returns true if the controller's POV is in the Up direction.
		 **/
		static bool										UsbController_PovUpPoll( const LSN_RUNTIME_POLL &_rpPollData );

		/**
		 * Polls a USB controller for the POV being Left.
		 * 
		 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
		 * \return Returns true if the controller's POV is in the Left direction.
		 **/
		static bool										UsbController_PovLeftPoll( const LSN_RUNTIME_POLL &_rpPollData );

		/**
		 * Polls a USB controller for the POV being Right.
		 * 
		 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
		 * \return Returns true if the controller's POV is in the Right direction.
		 **/
		static bool										UsbController_PovRightPoll( const LSN_RUNTIME_POLL &_rpPollData );

		/**
		 * Polls a USB controller for the POV being Down.
		 * 
		 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
		 * \return Returns true if the controller's POV is in the Down direction.
		 **/
		static bool										UsbController_PovDownPoll( const LSN_RUNTIME_POLL &_rpPollData );

		/**
		 * Polls a USB controller for having a positive axis.
		 * 
		 * \param _rpPollData Contains the data for the controller whose axes are to be tested and the deadzone for said axis.
		 * \return Returns true if the desired axis is above the threshold in the positive direction.
		 **/
		static bool										UsbController_AxisPositivePoll( const LSN_RUNTIME_POLL &_rpPollData );

		/**
		 * Polls a USB controller for having a negative axis.
		 * 
		 * \param _rpPollData Contains the data for the controller whose axes are to be tested and the deadzone for said axis.
		 * \return Returns true if the desired axis is above the threshold in the negative direction.
		 **/
		static bool										UsbController_AxisNegativePoll( const LSN_RUNTIME_POLL &_rpPollData );
	};

}	// namespace lsn
