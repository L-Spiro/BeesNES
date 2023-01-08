#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Includes and links to DirectInput8.  Also provides the global IDirectInput8 * object and functionality.
 */

#include "LSNDirectInput8.h"
#include <Base/LSWBase.h>
#include <comdef.h>


namespace lsn {

	// == Members.
	/** The DirectInput8 object. */
	CDirectInput8::LSN_DIRECTINPUT8 CDirectInput8::m_di8DirectInput;

	// == Functions.
	/**
	 * Creates the object.
	 * 
	 * \return Returns true if the object was created successfully.
	 */
	bool CDirectInput8::LSN_DIRECTINPUT8::DirectInput8Create() {
		Release();

		HRESULT hRet = ::DirectInput8Create( ::GetModuleHandleW( NULL ),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			reinterpret_cast<void **>(&lpdi8Object),
			NULL );
		if ( hRet != DI_OK ) {
			lsw::CBase::MessageBoxError( NULL, ResultToString( hRet ).c_str(), L"DirectInput8 Error" );
			return false;
		}
		return true;
	}

	// == Functions.
	/**
	 * Converts the given HRESULT into a string.
	 *
	 * \param _hRes The HRESULT to convert.
	 * \return Returns the string form of the given HRESULT.
	 */
	std::wstring CDirectInput8::ResultToString( HRESULT _hRes ) {
		std::wstring wsString;

		LPWSTR wStr;
		DWORD dwLen = ::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, _hRes, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
			reinterpret_cast<LPWSTR>(&wStr), 0,
			NULL );
		if ( dwLen ) {
			for ( DWORD I = 0; I < dwLen; ++I ) {
				wsString.push_back( wStr[I] );
			}
		}
		::LocalFree( wStr );
		return wsString;
	}

	/**
	 * Creates the DirectInput8 global object.
	 *
	 * \return Returns true if the object was created.
	 */
	bool CDirectInput8::CreateDirectInput8() {
		if ( m_di8DirectInput.Obj() ) { return true; }
		return m_di8DirectInput.DirectInput8Create();
	}

	/**
	 * Enumerates all devices of a given type, returning an array of all devices of the given type at the end.
	 *
	 * \param _dwType Device type filter.
	 *	To restrict the enumeration to a particular type of device, set this parameter to a DI8DEVTYPE_* value. See DIDEVICEINSTANCEW.
	 *	To enumerate a class of devices, use one of the following values.
	 *	DI8DEVCLASS_ALL
	 *	All devices.
	 *	DI8DEVCLASS_DEVICE
	 *	All devices that do not fall into another class.
	 *	DI8DEVCLASS_GAMECTRL
	 *	All game controllers.
	 *	DI8DEVCLASS_KEYBOARD
	 *	All keyboards. Equivalent to DI8DEVTYPE_KEYBOARD.
	 *	DI8DEVCLASS_POINTER
	 *	All devices of type DI8DEVTYPE_MOUSE and DI8DEVTYPE_SCREENPOINTER.
	 * \return Returns an array fo gatherede devices.
	 */
	std::vector<DIDEVICEINSTANCEW> CDirectInput8::GatherDevices( DWORD _dwType ) {
		std::vector<DIDEVICEINSTANCEW> vRet;
		if ( !m_di8DirectInput.Obj() ) { return vRet; }
		HRESULT hRet = m_di8DirectInput.Obj()->EnumDevices( _dwType, DIEnumDevicesCallback_GatherDevices, &vRet, DIEDFL_ATTACHEDONLY );
		if ( hRet != DI_OK ) {
			lsw::CBase::MessageBoxError( NULL, ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInput8::GatherDevices" );
			return vRet;
		}
		return vRet;
	}

	/**
	 * The IDirectInput8::EnumDevices() callback function for gathering all devices of a kind.
	 *
	 * \param _lpdDi Address of a DIDEVICEINSTANCEW structure that describes the device instance.
	 * \param _pvRef The application-defined value passed to IDirectInput8::EnumDevices or IDirectInput8::EnumDevicesBySemantics as the pvRef parameter.
	 * \return Returns DIENUM_CONTINUE.
	 */
	BOOL CDirectInput8::DIEnumDevicesCallback_GatherDevices( LPCDIDEVICEINSTANCE _lpdDi, LPVOID _pvRef ) {
		std::vector<DIDEVICEINSTANCEW> * pvVector = reinterpret_cast<std::vector<DIDEVICEINSTANCEW> *>(_pvRef);
		pvVector->push_back( (*_lpdDi) );
		return DIENUM_CONTINUE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
