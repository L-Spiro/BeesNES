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
				lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::CreateController" );
				return false;
			}
			// _pvData is an HWND.
			HWND hWnd = reinterpret_cast<HWND>(_pvData);
			hRet = m_did8Device.Obj()->SetCooperativeLevel( hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
			if ( hRet != DI_OK ) {
				lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::CreateController" );
				return false;
			}
		}
	}

	/**
	 * Tells the controller to poll its device.  The polled data should not be inspected yet to determine which keys are held, it simply
	 *	tells the object to poll the controller for its data to then be gathered immediately after.
	 *
	 * \return Returns true if polling was successful.
	 */
	bool CDirectInput8Controller::Poll() {
		if ( !m_did8Device.Obj() ) { return false; }
		HRESULT hRet = m_did8Device.Obj()->GetDeviceState( sizeof( m_jsState ), &m_jsState );
		if ( hRet != DI_OK ) {
			//lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8Controller::Poll" );
			return false;
		}
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

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
