/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Thread events, allowing signalling and waiting between threads.
 */

#include "LSNEvent.h"

#if defined( __GNUC__ )
#include <errno.h>
#endif	// #if defined( __GNUC__ )


namespace lsn {

	CEvent::CEvent() :
#ifdef LSN_WINDOWS
		m_hHandle( ::CreateEventW( NULL, FALSE, FALSE, NULL ) )
#elif defined( __GNUC__ )
		m_bTriggered( false )
#endif	// #ifdef LSN_WINDOWS
		{
#if defined( __GNUC__ )
		::pthread_condattr_t cAttr;
		::pthread_condattr_init( &cAttr );
#ifndef __APPLE__
		::pthread_condattr_setclock( &cAttr, CLOCK_MONOTONIC );
#endif	// #ifndef __APPLE__
		::pthread_cond_init( &m_cHandle, &cAttr );
		::pthread_mutex_init( &m_mLock, nullptr );
#endif	// #if defined( __GNUC__ )
	}
	CEvent::~CEvent() {
#ifdef LSN_WINDOWS
		::CloseHandle( m_hHandle );
		m_hHandle = NULL;
#elif defined( __GNUC__ )
		::pthread_cond_destroy( &m_cHandle );
		::pthread_mutex_destroy( &m_mLock );
#endif	// #ifdef LSN_WINDOWS
	}

	// == Functions.
	/**
	 * Waits for the event/signal.
	 */
	uint32_t CEvent::WaitForSignal() {
		return WaitForSignal( UINT32_MAX );
	}

	/**
	 * Waits for the event/signal for a given number of milliseconds.
	 * 
	 * \param _ui32Milliseconds The time-out time in milliseconds.
	 **/
	uint32_t CEvent::WaitForSignal( uint32_t _ui32Milliseconds ) {
#ifdef LSN_WINDOWS
		return ::WaitForSingleObject( m_hHandle, _ui32Milliseconds );
#elif defined( __GNUC__ )
		::pthread_mutex_lock( &m_mLock );

		int iResult = 0;
		if ( !m_bTriggered ) {
			if ( _ui32Milliseconds == UINT32_MAX ) {
				iResult = ::pthread_cond_wait( &m_cHandle, &m_mLock );
			}
			else {
#if !defined( __APPLE__ )
				::timespec tsTime;
				::clock_gettime( CLOCK_MONOTONIC, &tsTime );

				tsTime.tv_sec += _ui32Milliseconds / 1000;
				tsTime.tv_nsec += (_ui32Milliseconds % 1000) * 1000000;
				tsTime.tv_sec += tsTime.tv_nsec / 1000000000;
				tsTime.tv_nsec %= 1000000000;

				result = ::pthread_cond_timedwait( &m_cHandle, &m_mLock, &tsTime );
#else
				::timespec tsTime;
				tsTime.tv_sec = _ui32Milliseconds / 1000;
				tsTime.tv_nsec = (_ui32Milliseconds % 1000) * 1000000;

				iResult = ::pthread_cond_timedwait_relative_np( &m_cHandle, &m_mLock, &tsTime );
#endif	// #if !defined( __APPLE )
			}
		}

		if ( iResult == 0 ) {
			m_bTriggered = false;  // Automatically reset the event state.
		}

		::pthread_mutex_unlock( &m_mLock );
		return iResult == ETIMEDOUT ? WAIT_TIMEOUT : WAIT_OBJECT_0;
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
