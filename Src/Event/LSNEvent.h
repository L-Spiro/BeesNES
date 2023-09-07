/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Thread events, allowing signalling and waiting between threads.
 */

#pragma once

#include "../OS/LSNOs.h"
#include <cstdint>


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
		uint32_t								WaitForSignal();

		/**
		 * Waits for the event/signal for a given number of milliseconds.
		 * 
		 * \param _ui32Milliseconds The time-out time in milliseconds.
		 **/
		uint32_t								WaitForSignal( uint32_t _ui32Milliseconds );

		/**
		 * Signals the event.
		 */
		void									Signal();

		/**
		 * Gets the platform-specific handle to the event.
		 * 
		 * \return Returns the platform-specific handle to the event.
		 **/
#ifdef LSN_USE_WINDOWS
		HANDLE									Handle() const { return m_hHandle; }
#else
#endif// #ifdef LSN_USE_WINDOWS


	protected :
		// == Members.
#ifdef LSN_USE_WINDOWS
		/** The event handle. */
		HANDLE									m_hHandle;
#else
#endif	// #ifdef LSN_USE_WINDOWS
	};

}	// namespace lsn
