/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for hardware that needs to be notified of NMI's and IRQ's.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CInterruptable
	 * \brief The base class for hardware that needs to be notified of NMI's and IRQ's.
	 *
	 * Description: The base class for hardware that needs to be notified of NMI's and IRQ's.
	 */
	class CInterruptable {
	public :
		// == Functions.
		/**
		 * Notifies the class that an NMI has occurred.
		 */
		virtual void										Nmi() {}

		/**
		 * Clears the NMI flag.
		 */
		virtual void										ClearNmi() {}

		/**
		 * Notifies the class that an IRQ has occurred.
		 *
		 * \param _ui8Source The source of the IRQ.
		 */
		virtual void										Irq( uint8_t /*_ui8Source*/ ) {}

		/**
		 * Clears the IRQ flag.
		 *
		 * \param _ui8Source The source of the IRQ.
		 */
		virtual void										ClearIrq( uint8_t /*_ui8Source*/ ) {}

		/**
		 * Gets the status of the IRQ line.
		 *
		 * \param _ui8Source The source of the IRQ.
		 * \return Returns true if the IRQ status line is low.
		 **/
		virtual bool										GetIrqStatus( uint8_t /*_ui8Source*/ ) const { return false; }

		/**
		 * Begins a DMC DMA.
		 * 
		 * \param _ui8Val The value written to 0x4015.
		 */
		virtual void										BeginDmc( uint8_t /*_ui8Val*/ ) {}
	};

}	// namespace lsn
