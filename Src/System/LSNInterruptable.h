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
		virtual void						Nmi() {}

		/**
		 * Clears the NMI flag.
		 */
		virtual void						ClearNmi() {}

		/**
		 * Notifies the class that an IRQ has occurred.
		 */
		virtual void						Irq() {}

		/**
		 * Clears the IRQ flag.
		 */
		virtual void						ClearIrq() {}
	};

}	// namespace lsn
