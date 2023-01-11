/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Thread events, allowing signalling and waiting between threads.
 */

#pragma once

#include "../OS/LSNOs.h"


namespace lsn {

	/**
	 * Class CEvent
	 * \brief Thread events, allowing signalling and waiting between threads.
	 *
	 * Description: Thread events, allowing signalling and waiting between threads.
	 */
	class CEvent {
	public :
		CEvent();
		~CEvent();


		// == Functions.
		/**
		 * Waits for the event/signal.
		 */
		void									WaitForSignal();

		/**
		 * Signals the event.
		 */
		void									Signal();


	protected :
		// == Members.
#ifdef LSN_USE_WINDOWS
		/** The event handle. */
		HANDLE									m_hHandle;
#else
#endif	// #ifdef LSN_USE_WINDOWS
	};

}	// namespace lsn
