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
		std::memset( &m_jsState, 0, sizeof( m_jsState ) );
		std::memset( &m_dcCaps, 0, sizeof( m_dcCaps ) );
	}
	CDirectInput8Controller::~CDirectInput8Controller() {
	}


	// == Functions.
	/**
	 * Creates the device based off a GUID.
	 *
	 * \param _diInstance The device instance containing the GUID to use to create the controller.
	 * \param _pvData Platform-specific data.
	 * \return Returns true if the controller was created.
	 */
	bool CDirectInput8Controller::CreateController( const DIDEVICEINSTANCEW &_diInstance, void * _pvData ) {
		if ( !m_did8Device.CreateDevice( _diInstance.guidInstance ) ) { return false; }
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
			m_diDeviceInstance = _diInstance;
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
	 * Polls a button by its index.  Return true if the given button is pressed.
	 * 
	 * \param _ui8Idx The controller's button index to poll.
	 * \return Returns true if the button indexed by _ui8Idx is pressed, false otherwise.
	 **/
	bool CDirectInput8Controller::PollButton( uint8_t _ui8Idx ) const {
		if ( !m_did8Device.Obj() || _ui8Idx >= LSN_ELEMENTS( JoyState().rgbButtons ) ) { return false; }
		return JoyState().rgbButtons[_ui8Idx] != 0;
	}

	/**
	 * Polls an axis by its index.  Axis 0 = X, 1 = Y, 2 = Z, and further indices are extra axes[IDX-2].
	 * 
	 * \param _ui8Idx The controller's axis index to poll.
	 * \return Returns the axis value at the given axis index.
	 **/
	LONG CDirectInput8Controller::PollAxis( uint8_t _ui8Idx ) const {
		if ( !m_did8Device.Obj() ) { return 0; }
		if ( _ui8Idx == 0 ) { return JoyState().lX; }
		if ( _ui8Idx == 1 ) { return JoyState().lY; }
		if ( _ui8Idx == 2 ) { return JoyState().lZ; }
		_ui8Idx -= 2;
		if ( _ui8Idx >= LSN_ELEMENTS( JoyState().rglSlider ) ) { return 0; }
		return JoyState().rglSlider[_ui8Idx];
	}

	/**
	 * Gets a POV value given its POV index.
	 * 
	 * \param _ui8Idx The controller's POV index to poll.
	 * \return Returns the POV value given the POV array index.
	 **/
	DWORD CDirectInput8Controller::PollPov( uint8_t _ui8Idx ) const {
		if ( !m_did8Device.Obj() || _ui8Idx >= LSN_ELEMENTS( JoyState().rgdwPOV ) ) { return false; }
		return JoyState().rgdwPOV[_ui8Idx];
	}

	/**
	 * The thread function.
	 * 
	 * \param _ptThread A pointer to the thread data.
	 * \return Return true to keep the thread going, false to stop the thread.
	 **/
	bool CDirectInput8Controller::ThreadFunc( LSN_THREAD * /*_ptThread*/ ) {
		if ( !Poll() ) {
			return false;
		}


		return true;
#if 0
		HRESULT hRes;
		hRes = m_did8Device.Obj()->Acquire();
		//hRes = m_did8Device.Obj()->Poll();
		hRes = m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &m_jsState );
		hRes = m_did8Device.Obj()->Unacquire();
		hRes = m_did8Device.Obj()->SetEventNotification( m_eThreadClose.Handle() );
		while ( !m_bStopThread ) {
			uint32_t ui32Wait = m_eThreadClose.WaitForSignal( 1000 / 10 );
			//uint32_t ui32Wait = m_eThreadClose.WaitForSignal();
			/*hRes = m_did8Device.Obj()->Acquire();
			hRes = m_did8Device.Obj()->Poll();
			hRes = m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &m_jsState );
			hRes = m_did8Device.Obj()->Unacquire();
			for ( auto I = LSN_ELEMENTS( m_jsState.rgbButtons ); I--; ) {
				if ( m_jsState.rgbButtons[I] ) {
					//lsw::CBase::MessageBoxError( NULL, L"Pressed", L"BUTTON" );
				}
			}
			for ( auto I = LSN_ELEMENTS( m_jsState.rgdwPOV ); I--; ) {
				if ( m_jsState.rgdwPOV[I] != -1 ) {
					lsw::CBase::MessageBoxError( NULL, L"Pressed", L"D-PAD" );
				}
			}*/
			switch ( ui32Wait ) {
				case WAIT_TIMEOUT : { break; }
				case WAIT_OBJECT_0 : {
					if ( m_bStopThread ) { break; }
					//lsw::CBase::MessageBoxError( NULL, L"Pressed", L"BUTTON" );
					break;
				}
			}
		}
		m_did8Device.Obj()->SetEventNotification( NULL );
		//m_eThreadClosed.Signal();
#endif	// 0
		return false;
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
