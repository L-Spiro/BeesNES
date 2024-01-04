/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base class for a USB controller.
 */

#include "LSNUsbControllerBase.h"


namespace lsn {

	CUsbControllerBase::CUsbControllerBase() {
		/*std::memset( &m_bmButtonMap, 0, sizeof( m_bmButtonMap ) );
		std::memset( &m_bmRapidMap, 0, sizeof( m_bmRapidMap ) );*/
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
		m_tThreadData.m_pucbThis = this;
		m_tThreadData.m_pclListener = _pclListener;
		m_bStopThread = false;
		m_ptThread = std::make_unique<std::thread>( Thread, &m_tThreadData );
	}

	/**
	 * Stops the thread.
	 **/
	void CUsbControllerBase::StopThread() {
		m_bStopThread = true;
		{
			if ( m_ptThread.get() ) {
				m_eThreadClose.Signal();
				m_eThreadClosed.WaitForSignal();
				m_ptThread->join();
				m_ptThread.reset();
			}
		}
		m_bStopThread = false;
	}

	/**
	 * The polling thread.
	 *
	 * \param _ptThread Pointer to this object.
	 */
	void CUsbControllerBase::Thread( LSN_THREAD * _ptThread ) {
		while ( _ptThread->m_pucbThis->ThreadFunc( _ptThread ) && !_ptThread->m_pucbThis->m_bStopThread ) {
		}
		_ptThread->m_pucbThis->m_eThreadClosed.Signal();
		//HRESULT hRes;
		//hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->Acquire();
		////hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->Poll();
		//hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &_ptThread->m_pucbThis->m_jsState );
		//hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->Unacquire();
		//hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->SetEventNotification( _ptThread->m_pucbThis->m_eThreadClose.Handle() );
		//while ( !_ptThread->m_pucbThis->m_bStopThread ) {
		//	uint32_t ui32Wait = _ptThread->m_pucbThis->m_eThreadClose.WaitForSignal( 1000 / 10 );
		//	//uint32_t ui32Wait = _ptThread->m_pucbThis->m_eThreadClose.WaitForSignal();
		//	/*hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->Acquire();
		//	hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->Poll();
		//	hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &_ptThread->m_pucbThis->m_jsState );
		//	hRes = _ptThread->m_pucbThis->m_did8Device.Obj()->Unacquire();
		//	for ( auto I = LSN_ELEMENTS( _ptThread->m_pucbThis->m_jsState.rgbButtons ); I--; ) {
		//		if ( _ptThread->m_pucbThis->m_jsState.rgbButtons[I] ) {
		//			//lsw::CBase::MessageBoxError( NULL, L"Pressed", L"BUTTON" );
		//		}
		//	}
		//	for ( auto I = LSN_ELEMENTS( _ptThread->m_pucbThis->m_jsState.rgdwPOV ); I--; ) {
		//		if ( _ptThread->m_pucbThis->m_jsState.rgdwPOV[I] != -1 ) {
		//			lsw::CBase::MessageBoxError( NULL, L"Pressed", L"D-PAD" );
		//		}
		//	}*/
		//	switch ( ui32Wait ) {
		//		case WAIT_TIMEOUT : { break; }
		//		case WAIT_OBJECT_0 : {
		//			if ( _ptThread->m_pucbThis->m_bStopThread ) { break; }
		//			//lsw::CBase::MessageBoxError( NULL, L"Pressed", L"BUTTON" );
		//			break;
		//		}
		//	}
		//}
		//_ptThread->m_pucbThis->m_did8Device.Obj()->SetEventNotification( NULL );
		//_ptThread->m_pucbThis->m_eThreadClosed.Signal();
	}

}	// namespace lsn
