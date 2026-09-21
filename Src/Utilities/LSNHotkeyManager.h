/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The hotkey manager. Always register hotkeys through ths class.
 */

#pragma once

#include "../Utilities/LSNUtilities.h"

#ifdef LSN_USE_WINDOWS
#include <Helpers/LSWHelpers.h>
#endif	// #ifdef LSN_USE_WINDOWS

#include <mutex>
#include <set>

namespace lsn {

	/**
	 * Class CHotkeyManager
	 * \brief The hotkey manager.
	 *
	 * Description: The hotkey manager. Always register hotkeys through ths class.
	 */
	class CHotkeyManager {
	public :
		CHotkeyManager( uint16_t _ui16IdBase );
		~CHotkeyManager();


		// == Types.
		typedef bool (*								PfHotkeyFunction)( void *, uint64_t, uint32_t );		/**< A Hotkey callback function. Return false to have the hotkey deleted. */


		// == Functions.
#ifdef _WIN32
		/**
		 * Sets the window associated with the hotkey.
		 * 
		 * \param _hWnd The window to receive the WM_HOTKEY messages.
		 **/
		void inline									SetWindow( HWND _hWnd ) {
			std::lock_guard<std::recursive_mutex> lgLock( m_rmtxAccess );
			m_hWnd = _hWnd;
		}

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
		int32_t										AddHotkey( INT _iCode, INT _iMod, UINT _uiScanCode,
			PfHotkeyFunction _pfFunc, void * _pvParm0, uint64_t _ui64Parm1, uint32_t _ui32parm2 );

		/**
		 * Deletes a hotkey given its ID.
		 * 
		 * \param _i32Id The ID of the hotkey to delete.
		 * \return Returns true if the hotkey was found and deleted.
		 **/
		bool										DeleteHotkey( int32_t _i32Id );

		/**
		 * Determines if a given key and modifier exist.
		 * 
		 * \param _i32Code The key code.
		 * \param _i32Mod The modifier.
		 * \return Returns a boolean indicating whether the given key and modifier exist.
		 **/		
		inline bool									HasKey( int32_t _i32Code, int32_t _i32Mod ) const {
			std::lock_guard<std::recursive_mutex> lgLock( m_rmtxAccess );
			_i32Mod = VirtualKeyToModifier( _i32Mod );
			for ( const auto & rhThis : m_sHotkeys ) {
				if ( rhThis.i32Code == _i32Code && int32_t( VirtualKeyToModifier( rhThis.i32Mod ) ) == _i32Mod ) { return true; }
			}
			return false;
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
		bool										HandleHotkey( int32_t _i32Id );
#endif	// #ifdef _WIN32


	protected :
		// == Types.
		/** A registered hotkey. */
		struct LSN_REGISTERED_HOTKEY {
			uint64_t								ui64Parm1 = 0;											/**< The second parameter to pass to the function handler. */
			PfHotkeyFunction						pfFunc = nullptr;										/**< The callback function associated with the given hotkey. */
			void *									pvParm0 = nullptr;										/**< The first parameter to pass to the function handler. */
			uint32_t								ui32Parm2 = 0;											/**< The third parameter to pass to the function handler. */
			int32_t									i32Id = 0;												/**< The system ID of the hotkey. */
			int32_t									i32Code = 0;											/**< Virtual key code. */
			int32_t									i32Mod = 0;												/**< Key-code modifiers. */
			uint32_t								uiScanCode = 0;											/**< The scan code for printing. */
#ifdef _WIN32
			HWND									hWnd = NULL;											/**< The window with which the hotkey is associated. */
#endif	// #ifdef _WIN32


			// == Operators.
			/**
			 * Less-than operator.
			 * 
			 * \param _rhOther The other hotkey against which to compare.
			 * \return Returns true if this->i32Id < _rhOther.i32Id.
			 **/
			inline bool								operator < ( const LSN_REGISTERED_HOTKEY &_rhOther ) const {
				return this->i32Id < _rhOther.i32Id;
			}

			/**
			 * Equality operator.
			 * 
			 * \param _rhOther The other hotkey against which to compare.
			 * \return Returns true if this->i32Id == _rhOther.i32Id.
			 **/
			inline bool								operator == ( const LSN_REGISTERED_HOTKEY &_rhOther ) const {
				return this->i32Id == _rhOther.i32Id;
			}
		};


		// == Members.
		std::set<LSN_REGISTERED_HOTKEY>				m_sHotkeys;												/**< The active hotkeys. */
		mutable std::recursive_mutex				m_rmtxAccess;											/**< The mutex for accessing the hotkeys. */
#ifdef _WIN32
		HWND										m_hWnd = NULL;											/**< The window that will receive the WM_HOTKEY messages. */
		mutable std::atomic<uint32_t>				m_ui32Id = 0;											/**< The new ID to assign to each new hotkey. */
		uint16_t									m_ui16HotkeyBase = 0x1000;								/**< The starting ID for each hotkey. */
#endif	// #ifdef _WIN32


		// == Functions.
#ifdef _WIN32
		/**
		 * Converts a virtual key to a ::RegisterHotKey() modifier.
		 * 
		 * \param _uiKey The virtual key to convert.
		 * \return Returns the expected modifer for ::RegisterHotKey().
		 **/
		static inline UINT							VirtualKeyToModifier( UINT _uiKey ) {
			switch ( _uiKey ) {
				case VK_MENU : {}
				case VK_LMENU : {}
				case VK_RMENU : { return MOD_ALT; }
				case VK_CONTROL : {}
				case VK_LCONTROL : {}
				case VK_RCONTROL : { return MOD_CONTROL; }
				case VK_SHIFT : {}
				case VK_LSHIFT : {}
				case VK_RSHIFT : { return MOD_SHIFT; }
				case VK_LWIN : {}
				case VK_RWIN : { return MOD_WIN; }
			}
			return 0;
		}

		/**
		 * Generates a new ID for a new hotkey.
		 * 
		 * \return Returns a new unique ID for a new hotkey.
		 **/
		inline int32_t								NewId() const {
			return int32_t( m_ui32Id++ + m_ui16HotkeyBase );
		}
#endif	// #ifdef _WIN32

	};

}	// namespace lsn
