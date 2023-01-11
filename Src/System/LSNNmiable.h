/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for hardware that needs to be notified of NMI's.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CTickable
	 * \brief The base class for hardware that needs to be notified of NMI's.
	 *
	 * Description: The base class for hardware that needs to be notified of NMI's.
	 */
	class CNmiable {
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
	};

}	// namespace lsn
