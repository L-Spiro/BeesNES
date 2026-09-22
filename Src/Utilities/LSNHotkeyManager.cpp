/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The hotkey manager. Always register hotkeys through ths class.
 */


 #include "LSNHotkeyManager.h"


 namespace lsn {

	CHotkeyManager::CHotkeyManager( uint16_t _ui16IdBase ) 
#ifdef _WIN32
		:
		m_ui16HotkeyBase( _ui16IdBase )
#endif	// #ifdef _WIN32
		{
	}
	CHotkeyManager::~CHotkeyManager() {
		DeleteAllHotkeys();
	}

// == Functions.
#ifdef _WIN32
	/**
	 * Adds a hotkey.  if the hotkey aready exists, 0 is returned.
	 * 
	 * \param _iCode The key code of the hotkey.
	 * \param _iMod The virtual-key modifier.
	 * \param _uiScanCode The hotkey scancode.
	 * \param _pfFunc The callback function.
	 * \param _pvParm0 The first parameter to pass to the function handler.
	 * \param _ui64Parm1 The second parameter to pass to the function handler.
	 * \param _ui32parm2 The third parameter to pass to the function handler.
	 * \return Returns the ID of the hotkey if added, 0 otherwise.
	 **/
	int32_t CHotkeyManager::AddHotkey( INT _iCode, INT _iMod, UINT _uiScanCode,
		PfHotkeyFunction _pfFunc, void * _pvParm0, uint64_t _ui64Parm1, uint32_t _ui32parm2 ) {
		std::lock_guard<std::recursive_mutex> lgLock( m_rmtxAccess );
		if ( NULL == m_hWnd ) { return 0; }
		if ( HasKey( _iCode, _iMod ) ) { return 0; }
		LSN_REGISTERED_HOTKEY rhTmp = {
			.ui64Parm1 = _ui64Parm1,
			.pfFunc = _pfFunc,
			.pvParm0 = _pvParm0,
			.ui32Parm2 = _ui32parm2,
			.i32Id = NewId(),
			.i32Code = _iCode,
			.i32Mod = _iMod,
			.uiScanCode = _uiScanCode,
			.hWnd = m_hWnd,
		};
		try {
			if ( TRUE == ::RegisterHotKey( m_hWnd, rhTmp.i32Id, VirtualKeyToModifier( _iMod ), _iCode ) ) {
				m_sHotkeys.insert( rhTmp );
			}
			else { return 0; }
			return rhTmp.i32Id;
		}
		catch ( ... ) { return 0; }
	}

	/**
	 * Deletes a hotkey given its ID.
	 * 
	 * \param _i32Id The ID of the hotkey to delete.
	 * \return Returns true if the hotkey was found and deleted.
	 **/
	bool CHotkeyManager::DeleteHotkey( int32_t _i32Id ) {
		std::lock_guard<std::recursive_mutex> lgLock( m_rmtxAccess );
		for ( const auto & rhThis : m_sHotkeys ) {
			if ( rhThis.i32Id == _i32Id ) {
				if ( TRUE == ::UnregisterHotKey( rhThis.hWnd, rhThis.i32Id ) ) {
					m_sHotkeys.erase( rhThis );
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * Deletes all hotkeys.
	 **/
	void CHotkeyManager::DeleteAllHotkeys() {
		std::lock_guard<std::recursive_mutex> lgLock( m_rmtxAccess );
		auto sHotkeys = m_sHotkeys;
		for ( const auto & rhThis : sHotkeys ) {
			DeleteHotkey( rhThis.i32Id );
		}
	}

	/**
	 * Handles a hotkey by ID. When the window catches the WM_HOTKEY message, it should send that ID here so this function can
	 *	call the associated function.  If the hotkey ID does not exist, false is returned and the WM_HOTKEY message should be
	 *	handled by another system.
	 * 
	 * \param _i32Id The ID of the hotkey to handle.
	 * \return Returns true if the hotkey is registered.  A return of false indicates that the ID is not among the registered hotkeys
	 *	and should be passed off to some other system to handle.
	 **/
	bool CHotkeyManager::HandleHotkey( int32_t _i32Id ) {
		std::lock_guard<std::recursive_mutex> lgLock( m_rmtxAccess );
		for ( const auto & rhThis : m_sHotkeys ) {
			if ( rhThis.i32Id == _i32Id ) {
				if ( !rhThis.pfFunc( rhThis.pvParm0, rhThis.ui64Parm1, rhThis.ui32Parm2 ) ) {
					DeleteHotkey( _i32Id );
				}
				return true;
			}
		}
		return false;
	}

#endif	// #ifdef _WIN32
}	// namespace lsn
