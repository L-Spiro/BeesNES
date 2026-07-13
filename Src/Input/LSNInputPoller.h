/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base class for an object that can provide input feedback to the CPU.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

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

	/**
	 * Class CInputPoller
	 * \brief A base class for an object that can provide input feedback to the CPU.
	 *
	 * Description: A base class for an object that can provide input feedback to the CPU.
	 */
	class CInputPoller {
	public :
		// == Functions.
		/**
		 * Polls the given port and returns a byte containing the result of polling by combining the LSN_INPUT_BITS values.
		 *
		 * \param _ui8Value The value being written to $4016.
		 * \return Returns the result of polling the given port.
		 */
		virtual void					PollPort( uint8_t /*_ui8Value*/ ) {}

		/**
		 * Handles a read of $4016/$4017.
		 * 
		 * \param _ui16Address The address being read.
		 * \return Returns the desired read value.
		 **/
		virtual uint8_t					Read( uint16_t /*_ui16Address*/ ) { return 0; }
	};

}	// namespace lsn
