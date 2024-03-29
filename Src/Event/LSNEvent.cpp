/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Thread events, allowing signalling and waiting between threads.
 */

#include "LSNEvent.h"


namespace lsn {

	CEvent::CEvent() :
		m_hHandle( ::CreateEventW( NULL, FALSE, FALSE, NULL ) ) {
	}
	CEvent::~CEvent() {
		::CloseHandle( m_hHandle );
		m_hHandle = NULL;
	}

	// == Functions.
	/**
	 * Waits for the event/signal.
	 */
	uint32_t CEvent::WaitForSignal() {
		return WaitForSignal( INFINITE );
	}

	/**
	 * Waits for the event/signal for a given number of milliseconds.
	 * 
	 * \param _ui32Milliseconds The time-out time in milliseconds.
	 **/
	uint32_t CEvent::WaitForSignal( uint32_t _ui32Milliseconds ) {
#ifdef LSN_WINDOWS
		return ::WaitForSingleObject( m_hHandle, _ui32Milliseconds );
#else
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Signals the event.
	 */
	void CEvent::Signal() {
#ifdef LSN_WINDOWS
		::SetEvent( m_hHandle );
#else
#endif	// #ifdef LSN_WINDOWS
	}

}	// namespace lsn
