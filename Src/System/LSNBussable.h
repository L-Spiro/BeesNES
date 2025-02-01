/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for hardware that can have its bus properties polled.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CBussable
	 * \brief The base class for hardware that can have its bus properties polled.
	 *
	 * Description: The base class for hardware that can have its bus properties polled.
	 */
	class CBussable {
	public :
		// == Functions.
		/**
		 * Gets the last bus address.
		 */
		virtual uint16_t									LastBusAddress() const { return m_ui16LastBusAddr; }

	protected :
		// == Members.
		uint16_t											m_ui16LastBusAddr = 0;
	};

}	// namespace lsn
