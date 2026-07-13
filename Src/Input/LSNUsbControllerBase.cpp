/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base class for a USB controller.
 */

#include "LSNUsbControllerBase.h"
#include "LSNControllerListener.h"


namespace lsn {

	CUsbControllerBase::CUsbControllerBase() {
	}
	CUsbControllerBase::~CUsbControllerBase() {
		StopThread();
	}


	// == Functions.
	/**
	 * Starts the thread.
	 * 
	 * \param _pclListener A pointer to an object that provides a listener interface for receiving notifications about controller events
	 **/
	void CUsbControllerBase::BeginThread( CControllerListener * _pclListener ) {
		StopThread();

		m_tThreadData.m_pucbThis = this;
		m_tThreadData.m_pclListener = _pclListener;
		m_bStopThread = false;

		m_ptThread = std::make_unique<std::thread>( Thread, &m_tThreadData );
	}

	/**
	 * Stops the thread.
	 **/
	void CUsbControllerBase::StopThread() {
		SignalStop();

		if ( !m_ptThread || !m_ptThread->joinable() ) { return; }

		if ( std::this_thread::get_id() != m_ptThread->get_id() ) {
			m_ptThread->join();
		}
		else {
			m_ptThread->detach();
		}
		
		m_ptThread.reset();
	}

	/**
	 * The polling thread.
	 *
	 * \param _ptThread Pointer to this object.
	 */
	void CUsbControllerBase::Thread( LSN_THREAD * _ptThread ) {
		while ( !_ptThread->m_pucbThis->m_bStopThread && _ptThread->m_pucbThis->ThreadFunc( _ptThread ) ) {

			std::unique_lock<std::mutex> ulLock( _ptThread->m_pucbThis->m_mThreadMutex );
			
			_ptThread->m_pucbThis->m_cvThreadClose.wait_for( ulLock, std::chrono::milliseconds( 1 ), [ _ptThread ]() {
				return _ptThread->m_pucbThis->m_bStopThread.load();
			} );
		}
	}

}	// namespace lsn
