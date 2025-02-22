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
			LSN_CLOSE_PATCHER									= WM_USER + 1
#endif	// #if defined( LSN_WINDOWS )
		};


		// == Types.
		/** An entry in a combo box. */
		struct LSN_COMBO_ENTRY {
			const wchar_t *										pwcName;						/**< The name of the entry. */
			LPARAM												lpParm;							/**< The parameter of the entry. */
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


	protected :
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
