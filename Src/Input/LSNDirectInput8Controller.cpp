#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard USB controller using DirectInput8.
 */

#include "LSNDirectInput8Controller.h"
#include <Base/LSWBase.h>

namespace lsn {

	CDirectInput8Controller::CDirectInput8Controller() {
	}
	CDirectInput8Controller::~CDirectInput8Controller() {
		StopThread();
	}


	// == Functions.
	/**
	 * Creates the device based off a GUID.
	 *
	 * \param _guId The GUID to use to create the controller.
	 * \param _pvData Platform-specific data.
	 * \return Returns true if the controller was created.
	 */
	bool CDirectInput8Controller::CreateController( const GUID &_guId, void * _pvData ) {
		if ( !m_did8Device.CreateDevice( _guId ) ) { return false; }
		if ( m_did8Device.Obj() ) {
			HRESULT hRet = m_did8Device.Obj()->SetDataFormat( &c_dfDIJoystick );
			if ( hRet != DI_OK ) {
				lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::CreateController::SetDataFormat" );
				return false;
			}
			// _pvData is an HWND.
			HWND hWnd = reinterpret_cast<HWND>(_pvData);
			hRet = m_did8Device.Obj()->SetCooperativeLevel( hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
			if ( hRet != DI_OK ) {
				lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::CreateController::SetCooperativeLevel" );
				return false;
			}

			m_dcCaps.dwSize = sizeof( m_dcCaps );
			hRet = m_did8Device.Obj()->GetCapabilities( &m_dcCaps );
			if ( hRet != DI_OK ) {
				lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::CreateController::GetCapabilities" );
				return false;
			}
			hRet = m_did8Device.Obj()->EnumObjects( EnumDeviceObjectsCallback, this, DIDFT_AXIS );
			if ( hRet != DI_OK ) {
				lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::CreateController::EnumObjects" );
				return false;
			}
			//BeginThread();
			return true;
		}
		return false;
	}

	/**
	 * Tells the controller to poll its device.  The polled data should not be inspected yet to determine which keys are held, it simply
	 *	tells the object to poll the controller for its data to then be gathered immediately after.
	 *
	 * \return Returns true if polling was successful.
	 */
	bool CDirectInput8Controller::Poll() {
		if ( !m_did8Device.Obj() ) { return false; }
		HRESULT hRes;
		hRes = m_did8Device.Obj()->Acquire();
		if ( hRes != DI_OK ) {
			return false;
		}
		hRes = m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &m_jsState );
		if ( hRes != DI_OK ) {
			//lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::Poll" );
			m_did8Device.Obj()->Unacquire();
			return false;
		}
		m_did8Device.Obj()->Unacquire();
		return true;
	}

	/**
	 * Determines if the A button is pressed.
	 *
	 * \return Returns true if the A button is pressed.
	 */
	bool CDirectInput8Controller::IsAPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CDirectInput8Controller::IsBPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CDirectInput8Controller::IsSelectPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CDirectInput8Controller::IsStartPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CDirectInput8Controller::IsUpPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CDirectInput8Controller::IsDownPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CDirectInput8Controller::IsLeftPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CDirectInput8Controller::IsRightPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the turbo A button is pressed.
	 *
	 * \return Returns true if the A button is pressed.
	 */
	bool CDirectInput8Controller::IsATurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the turbo B button is pressed.
	 *
	 * \return Returns true if the B button is pressed.
	 */
	bool CDirectInput8Controller::IsBTurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the turbo Select button is pressed.
	 *
	 * \return Returns true if the Select button is pressed.
	 */
	bool CDirectInput8Controller::IsSelectTurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the turbo Start button is pressed.
	 *
	 * \return Returns true if the Start button is pressed.
	 */
	bool CDirectInput8Controller::IsStartTurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the turbo Up button is pressed.
	 *
	 * \return Returns true if the Up button is pressed.
	 */
	bool CDirectInput8Controller::IsUpTurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the turbo Down button is pressed.
	 *
	 * \return Returns true if the Down button is pressed.
	 */
	bool CDirectInput8Controller::IsDownTurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}
		
	/**
	 * Determines if the turbo Left button is pressed.
	 *
	 * \return Returns true if the Left button is pressed.
	 */
	bool CDirectInput8Controller::IsLeftTurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Determines if the turbo Right button is pressed.
	 *
	 * \return Returns true if the Right button is pressed.
	 */
	bool CDirectInput8Controller::IsRightTurboPressed() {
		if ( !m_did8Device.Obj() ) { return false; }
	}

	/**
	 * Starts the thread.
	 **/
	void CDirectInput8Controller::BeginThread() {
		m_tThreadData.m_pci8cThis = this;
		m_bStopThread = false;
		m_ptThread = std::make_unique<std::thread>( Thread, &m_tThreadData );
	}

	/**
	 * Stops the thread.
	 **/
	void CDirectInput8Controller::StopThread() {
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
	 * The resizing thread.
	 *
	 * \param _pblppFilter Pointer to this object.
	 */
	void CDirectInput8Controller::Thread( LSN_THREAD * _ptThread ) {
		HRESULT hRes;
		hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->Acquire();
		//hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->Poll();
		hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &_ptThread->m_pci8cThis->m_jsState );
		hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->Unacquire();
		hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->SetEventNotification( _ptThread->m_pci8cThis->m_eThreadClose.Handle() );
		while ( !_ptThread->m_pci8cThis->m_bStopThread ) {
			uint32_t ui32Wait = _ptThread->m_pci8cThis->m_eThreadClose.WaitForSignal( 1000 / 10 );
			//uint32_t ui32Wait = _ptThread->m_pci8cThis->m_eThreadClose.WaitForSignal();
			/*hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->Acquire();
			hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->Poll();
			hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &_ptThread->m_pci8cThis->m_jsState );
			hRes = _ptThread->m_pci8cThis->m_did8Device.Obj()->Unacquire();
			for ( auto I = LSN_ELEMENTS( _ptThread->m_pci8cThis->m_jsState.rgbButtons ); I--; ) {
				if ( _ptThread->m_pci8cThis->m_jsState.rgbButtons[I] ) {
					//lsw::CBase::MessageBoxError( NULL, L"Pressed", L"BUTTON" );
				}
			}
			for ( auto I = LSN_ELEMENTS( _ptThread->m_pci8cThis->m_jsState.rgdwPOV ); I--; ) {
				if ( _ptThread->m_pci8cThis->m_jsState.rgdwPOV[I] != -1 ) {
					lsw::CBase::MessageBoxError( NULL, L"Pressed", L"D-PAD" );
				}
			}*/
			switch ( ui32Wait ) {
				case WAIT_TIMEOUT : { break; }
				case WAIT_OBJECT_0 : {
					if ( _ptThread->m_pci8cThis->m_bStopThread ) { break; }
					//lsw::CBase::MessageBoxError( NULL, L"Pressed", L"BUTTON" );
					break;
				}
			}
		}
		_ptThread->m_pci8cThis->m_did8Device.Obj()->SetEventNotification( NULL );
		_ptThread->m_pci8cThis->m_eThreadClosed.Signal();
	}

	/**
	 * Application-defined callback function that receives DirectInputDevice objects as a result of a call to the IDirectInputDevice8::EnumObjects method.
	 * 
	 * \param _lpddoiInstance DIDEVICEOBJECTINSTANCE structure that describes the object being enumerated.
	 * \param _pvRef The application-defined value passed to IDirectInputDevice8::EnumObjects as the _pvRef parameter.
	 * \return Returns DIENUM_CONTINUE to continue the enumeration or DIENUM_STOP to stop the enumeration.
	 **/
	BOOL CALLBACK CDirectInput8Controller::EnumDeviceObjectsCallback( LPCDIDEVICEOBJECTINSTANCE _lpddoiInstance,  LPVOID _pvRef ) {
		CDirectInput8Controller * pdi8cController = reinterpret_cast<CDirectInput8Controller *>(_pvRef);
		DIPROPRANGE propRange; 
		propRange.diph.dwSize       = sizeof( DIPROPRANGE );
		propRange.diph.dwHeaderSize = sizeof( DIPROPHEADER );
		propRange.diph.dwHow        = DIPH_BYID;
		propRange.diph.dwObj        = _lpddoiInstance->dwType;
		propRange.lMin              = -1000;
		propRange.lMax              = +1000;
    
		HRESULT hRes;
		hRes = pdi8cController->m_did8Device.Obj()->SetProperty( DIPROP_RANGE, &propRange.diph );
		if ( hRes != S_OK ) {
			lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRes ).c_str(), L"DirectInput8 Error: EnumDeviceObjectsCallback" );
			return DIENUM_STOP;
		}

		return DIENUM_CONTINUE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
