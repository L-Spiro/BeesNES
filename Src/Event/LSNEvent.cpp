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
	void CEvent::WaitForSignal() {
#ifdef LSN_USE_WINDOWS
		::WaitForSingleObject( m_hHandle, INFINITE );
#else
#endif	// #ifdef LSN_USE_WINDOWS
	}

	/**
	 * Signals the event.
	 */
	void CEvent::Signal() {
#ifdef LSN_USE_WINDOWS
		::SetEvent( m_hHandle );
#else
#endif	// #ifdef LSN_USE_WINDOWS
	}

}	// namespace lsn
