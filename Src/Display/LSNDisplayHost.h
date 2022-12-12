/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the window responsible for drawing the emulated result.  This allows
 *	the window to receive notifications from a CDisplayClient (the console) regarding rendering.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	class CDisplayClient;

	/**
	 * Class CDisplayHost
	 * \brief The base class for the window responsible for drawing the emulated result.  This allows
	 *	the window to receive notifications from a CDisplayClient (the console) regarding rendering.
	 *
	 * Description: The base class for the window responsible for drawing the emulated result.  This allows
	 *	the window to receive notifications from a CDisplayClient (the console) regarding rendering.
	 */
	class CDisplayHost {
	public :
		// == Various constructors.
		CDisplayHost() :
			m_pdcClient( nullptr ) {
		}
		virtual ~CDisplayHost();


		// == Functions.
		/**
		 * Detatches from the display client.
		 */
		virtual void							DetatchFromDisplayClient() { m_pdcClient = nullptr; }

		/**
		 * Informs the host that a frame has been rendered.  This typically causes a display update and a framebuffer swap.
		 */
		virtual void							Swap() {}


	protected :
		// == Members.
		/** The display client. */
		CDisplayClient *						m_pdcClient;
	};

}	// namespace lsn
