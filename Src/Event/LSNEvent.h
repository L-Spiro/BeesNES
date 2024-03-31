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

#ifdef __GNUC__
#include <pthread.h>

#ifndef WAIT_TIMEOUT
#define WAIT_TIMEOUT 258
#endif	// #ifndef WAIT_TIMEOUT

#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 static_cast<uint32_t>(0)
#endif	// #ifndef WAIT_OBJECT_0

#endif	// #ifdef __GNUC__


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
#ifdef LSN_WINDOWS
		HANDLE									Handle() const { return m_hHandle; }
#else
#endif// #ifdef LSN_WINDOWS


	protected :
		// == Members.
#ifdef LSN_WINDOWS
		/** The event handle. */
		HANDLE									m_hHandle;
#elif defined( __GNUC__ )
		::pthread_cond_t						m_cHandle;		/**< The event handle. */
		::pthread_mutex_t						m_mLock;		/**< The mutex. */
		bool									m_bTriggered;	/**< If the mutex has been signaled. */
#else
#endif	// #ifdef LSN_WINDOWS
	};

}	// namespace lsn
