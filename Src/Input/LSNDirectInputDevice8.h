#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wrapper around IDirectInputDevice8W *.
 */

#pragma once

#include "LSNDirectInput8.h"
#include <Base/LSWBase.h>


namespace lsn {

	/**
	 * Copyright L. Spiro 2023
	 *
	 * Written by: Shawn (L. Spiro) Wilcoxen
	 *
	 * Description: Wrapper around IDirectInputDevice8W *.
	 */
	class CDirectInputDevice8 {
	public :
		CDirectInputDevice8() :
			m_pdidDevice( nullptr ),
			m_guId() {
		}
		~CDirectInputDevice8() {
			Release();
		}


		// == Functions.
		/**
		 * Creates and initializes an instance of a device based on a given globally unique identifier (GUID), and obtains an IDirectInputDevice8 Interface interface.
		 *
		 * \param _guId Reference to the GUID for the desired input device. The GUID is retrieved through the IDirectInput8::EnumDevices method, or it can be one of the
		 *	predefined GUIDs listed below. For the following GUID values to be valid, your application must define INITGUID before all other preprocessor directives at the beginning of the source file, or link to Dxguid.lib.
		 *		GUID_SysKeyboard: The default system keyboard.
		 *		GUID_SysMouse: The default system mouse.
		 * \param _lpuUnker Address of the controlling object's IUnknown interface for COM aggregation, or NULL if the interface is not aggregated. Most calling applications pass NULL.
		 * \return Returns true if the device was created without issue.
		 */
		bool						CreateDevice( REFGUID _guId, LPUNKNOWN _lpuUnker = NULL ) {
			if ( !CDirectInput8::Obj() ) { return false; }
			Release();
			HRESULT hRet = CDirectInput8::Obj()->CreateDevice( _guId, &m_pdidDevice, _lpuUnker );
			if ( hRet != DI_OK ) {
				lsw::CBase::MessageBoxError( NULL, CDirectInput8::ResultToString( hRet ).c_str(), L"DirectInput8 Error: CDirectInputDevice8::CreateDevice" );
				return false;
			}
			m_guId = _guId;
			return true;
		}

		/**
		 * Resets the object back to scratch, releasing all associated memory.
		 */
		void						Release() {
			if ( m_pdidDevice ) {
				m_pdidDevice->Release();
				m_pdidDevice = nullptr;
			}
		}

		/**
		 * Gets the GUID of the device.
		 *
		 * \return Returns a constant reference to the device’s GUID.
		 */
		const GUID &				Guid() const { return m_guId; }

		/**
		 * Gets the IDirectInputDevice8W pointer.
		 *
		 * \return Returns the IDirectInputDevice8W pointer.
		 */
		IDirectInputDevice8W *		Obj() { return m_pdidDevice; }

	protected :
		// == Members.
		/** The object we wrap. */
		IDirectInputDevice8W *		m_pdidDevice;
		/** The GUID associated with this device. */
		GUID						m_guId;
	};
	
}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
