#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Utility functions relating to the OS.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Utilities/LSNStream.h"
#include <ComboBox/LSWComboBox.h>
#include <Helpers/LSWInputListenerBase.h>
#include <Widget/LSWWidget.h>

#include <map>
#include <shlobj.h>
#include <vector>

using namespace lsw;

namespace lsn {

	/**
	 * Class CWinUtilities
	 * \brief Utility functions relating to the OS.
	 *
	 * Description: Utility functions relating to the OS.
	 */
	class CWinUtilities {
	public :
		// == Enumerations.
		/** Custom window messages. */
		enum LSN_MSG : uint16_t {
#if defined( LSN_WINDOWS )
			LSN_CLOSE_PATCHER									= WM_USER + 1,
			LSN_CLOSE_WAV_EDITOR,
			LSN_UPDATE_HZ,
			LSN_UPDATE_PATCHER_DESC,
#endif	// #if defined( LSN_WINDOWS )
		};


		// == Types.
		/** An entry in a combo box. */
		struct LSN_COMBO_ENTRY {
			const wchar_t *										pwcName;						/**< The name of the entry. */
			LPARAM												lpParm;							/**< The parameter of the entry. */
		};

		/** An ::OleInitialize() wrapper. */
		struct LSN_OLEINITIALIZE {
			LSN_OLEINITIALIZE( LPVOID _pvReserved = nullptr ) :
				bOle( CWinUtilities::OleInitialize( _pvReserved ) ) {
			}
			~LSN_OLEINITIALIZE() {
				if ( bOle ) {
					::OleUninitialize();
					bOle = false;
				}
			}


			// == Functions.
			/**
			 * Determines whether the call to ::OleInitialize() was successful or not.
			 * 
			 * \return Returns true if the call to ::OleInitialize() was successful.
			 **/
			inline bool											Success() const { return bOle; }

		protected :
			// == Members.
			bool												bOle = false;
		};


		// == Functions.
		/**
		 * Fills a combo box with console types (Nintendo Entertainment System(R) and Famicom(R)).
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithConsoleTypes( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with Famicom(R) controller types (Standard Controller only).
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithControllerType_Famicom( CWidget * _pwComboBox );

		/**
		 * Fills a combo box with Nintendo Entertainment System(R) controller types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithControllerType_NES( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup );

		/**
		 * Fills a combo box with Nintendo Entertainment System(R) Four Score expansion types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithControllerType_FourScore( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup );

		/**
		 * Fills a combo box with Famicom(R) expansion types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithExpansion_Famicom( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file formats.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavFormats( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file streaming start conditions.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavStartConditions( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file streaming end conditions.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavEndConditions( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file PCM bit values.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _bInclude32 If true, 32 bits is included.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavPcmBits( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, bool _bInclude32 = false );

		/**
		 * Fills a combo box with WAV metadata formats.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavMetaDataFormats( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor "Actual Hz".
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavActialHz( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor noise color.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavNoiseColor( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor noise type.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavNoiseType( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor filter type.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavFilterType( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor mono-stereo-surround output types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavStereoSettings( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV metadata formats.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		template <typename _tKey, typename _tType>
		static bool												FillComboWithMap( CWidget * _pwComboBox, const std::map<_tKey, _tType> &_mMap, LPARAM _lpDefaultSelect ) {
			std::vector<LSN_COMBO_ENTRY> vEntries;
			try {
				if ( _mMap.empty() ) { return FillComboBox( _pwComboBox, nullptr, 0, _lpDefaultSelect ); }

				vEntries.resize( _mMap.size() );
				for ( const auto & I : _mMap ) {
					vEntries[I].pwcName = reinterpret_cast<const wchar_t *>(I.second);
					vEntries[I].lpParm = LPARAM( I );
				}
				if ( FillComboBox( _pwComboBox, &vEntries[0], vEntries.size(), _lpDefaultSelect ) ) {
					lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(_pwComboBox);
					pcbCombo->SetSel( 0, 0 );
					return true;
				}
				return false;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Fills a combo box with an array of UTF-16 strings.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		template <typename _tType>
		static bool												FillComboWithStrings( CWidget * _pwComboBox, const std::vector<_tType> &_vStrings, LPARAM _lpDefaultSelect ) {
			std::vector<LSN_COMBO_ENTRY> vEntries;
			try {
				if ( !_vStrings.size() ) { return FillComboBox( _pwComboBox, nullptr, 0, _lpDefaultSelect ); }
				vEntries.resize( _vStrings.size() );
				for ( size_t I = 0; I < _vStrings.size(); ++I ) {
					vEntries[I].pwcName = reinterpret_cast<const wchar_t *>(_vStrings[I].c_str());
					vEntries[I].lpParm = LPARAM( I );
				}
				if ( FillComboBox( _pwComboBox, &vEntries[0], vEntries.size(), _lpDefaultSelect ) ) {
					lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(_pwComboBox);
					pcbCombo->SetSel( 0, 0 );
					return true;
				}
				return false;
			}
			catch ( ... ) { return false; }
			return true;
		}
		
		/**
		 * Fills a combo box with the given array of LSN_COMBO_ENTRY structures.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _pceEntries The array of combo-box entries.
		 * \param _stTotal The total number of entries to which _pceEntries points.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
		 */
		static bool												FillComboBox( CWidget * _pwComboBox, const LSN_COMBO_ENTRY * _pceEntries, size_t _stTotal, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup = 0 );

		/**
		 * Writes a UI key to a stream.
		 * 
		 * \param _kKey The key values to write.
		 * \param _sStream The stream to which to write the key values.
		 * \return Returns true if writing of the key information to the stream succeeded.
		 **/
		static bool												WriteUiKey( const lsw::LSW_KEY &_kKey, lsn::CStream &_sStream );

		/**
		 * Reads a UI key from a stream.
		 * 
		 * \param _kKey The key values to be filled by reading from the givens tream.
		 * \param _sStream The stream from which to read the key values.
		 * \return Returns true if reading of the key information from the stream succeeded.
		 **/
		static bool												ReadUiKey( lsw::LSW_KEY &_kKey, lsn::CStream &_sStream );

		/**
		 * Callback procedure for the folder browser dialog.
		 *
		 * \param _hWnd   The dialog window handle.
		 * \param _uMsg   The message.
		 * \param _lParam The message parameter.
		 * \param _lpData The application-defined data (initial path pointer).
		 * \return Returns 0 to continue default processing.
		 */
		static int CALLBACK										BrowseCallbackProc( HWND _hWnd, UINT _uMsg, LPARAM /*_lParam*/, LPARAM lpData ) {
			if ( _uMsg == BFFM_INITIALIZED && lpData ) {
				// lpData is a pointer to the initial folder path
				::SendMessageW( _hWnd, BFFM_SETSELECTIONW, TRUE, lpData );
			}
			return 0;
		}

		/**
		 * Initializes OLE for the current thread.
		 * 
		 * \param _pvReserved Reserved; must be nullptr.
		 * \return Returns true if OLE was initialized successfully, false otherwise.
		 */
		static bool												OleInitialize( LPVOID _pvReserved = nullptr ) {
			HRESULT hrResult = ::OleInitialize( _pvReserved );
			return (hrResult == S_OK || hrResult == S_FALSE);
		}


	protected :
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
