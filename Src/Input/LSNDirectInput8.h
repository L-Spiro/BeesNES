#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Includes and links to DirectInput8.  Also provides the global IDirectInput8 * object and functionality.
 */


#pragma once

#include <LSWWin.h>									// Must come before dinput.h.
#define DIRECTINPUT_VERSION							0x0800
#include <dinput.h>
#include <string>
#include <vector>

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )


namespace lsn {

	/**
	 * Class CDirectInput8
	 * \brief A wrapper around the global IDirectInput8W * object.
	 *
	 * Description: A wrapper around the global IDirectInput8W * object.
	 */
	class CDirectInput8 {
	public :


		// == Functions.
		/**
		 * Converts the given HRESULT into a string.
		 *
		 * \param _hRes The HRESULT to convert.
		 * \return Returns the string form of the given HRESULT.
		 */
		static std::wstring							ResultToString( HRESULT _hRes );

		/**
		 * Creates the DirectInput8 global object.
		 *
		 * \return Returns true if the object was created.
		 */
		static bool									CreateDirectInput8();

		/**
		 * Called at shut-down to release the DirectInput8 object.
		 */
		static void									Release() { m_di8DirectInput.Release(); }

		/**
		 * Gets a pointer to the DirectInput8 object.  A previous call to CreateDirectInput8() must have been successful.
		 *
		 * \return Returns the global IDirectInput8W * pointer.
		 */
		static inline IDirectInput8W *				Obj() { return m_di8DirectInput.Obj(); }

		/**
		 * Enumerates all devices of a given type, returning an array of all devices of the given type at the end.
		 *
		 * \param _dwType Device type filter.
		 *	To restrict the enumeration to a particular type of device, set this parameter to a DI8DEVTYPE_* value. See DIDEVICEINSTANCEW.
		 *	To enumerate a class of devices, use one of the following values.
		 *	DI8DEVCLASS_ALL: All devices.
		 *	DI8DEVCLASS_DEVICE: All devices that do not fall into another class.
		 *	DI8DEVCLASS_GAMECTRL: All game controllers.
		 *	DI8DEVCLASS_KEYBOARD: All keyboards. Equivalent to DI8DEVTYPE_KEYBOARD.
		 *	DI8DEVCLASS_POINTER: All devices of type DI8DEVTYPE_MOUSE and DI8DEVTYPE_SCREENPOINTER.
		 * \return Returns an array fo gatherede devices.
		 */
		static std::vector<DIDEVICEINSTANCEW>		GatherDevices( DWORD _dwType );

	protected :
		// == Types.
		/** Direct wrapper around an IDirectInput8W * pointer to provide fail-safe RAII. */
		struct LSN_DIRECTINPUT8 {
			LSN_DIRECTINPUT8() :
				lpdi8Object( nullptr) {
			}
			~LSN_DIRECTINPUT8() {
				Release();
			}

			/**
			 * Creates the object.
			 * 
			 * \return Returns true if the object was created successfully.
			 */
			bool									DirectInput8Create();

			/**
			 * Releases the IDirectInput8W * object.
			 */
			void									Release() {
				if ( lpdi8Object ) {
					lpdi8Object->Release();
					lpdi8Object = nullptr;
				}
			}

			/**
			 * Gets the object pointer.
			 *
			 * \return Returns the IDirectInput8W * object.
			 */
			inline IDirectInput8W *					Obj() { return lpdi8Object; }

		protected :
			/** The IDirectInput8W * object. */
			IDirectInput8W *						lpdi8Object;
		};


		// == Members.
		/** The DirectInput8 object. */
		static LSN_DIRECTINPUT8						m_di8DirectInput;


		// == Functions.
		/**
		 * The IDirectInput8::EnumDevices() callback function for gathering all devices of a kind.
		 *
		 * \param _lpdDi Address of a DIDEVICEINSTANCEW structure that describes the device instance.
		 * \param _pvRef The application-defined value passed to IDirectInput8::EnumDevices or IDirectInput8::EnumDevicesBySemantics as the pvRef parameter.
		 * \return Returns DIENUM_CONTINUE.
		 */
		static BOOL PASCAL							DIEnumDevicesCallback_GatherDevices( LPCDIDEVICEINSTANCEW _lpdDi, LPVOID _pvRef );
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
