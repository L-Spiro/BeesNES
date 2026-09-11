/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Converts cheat files of various types to an internal representation.
 */


 #include "LSNCheatConverter.h"
 #include "../Localization/LSNLocalization.h"


 namespace lsn {

	// == Functions.
	/**
	 * Reads a Mesen .JSON file and returns all contained cheats inside it.
	 * 
	 * \param _vJson The JSON file loaded into memory.
	 * \param _vCheats The returned cheat list for the given file.
	 * \param _wsError If false is returned, this contains the error to print for the user.
	 * \return Returns true if all cheats were loaded.  If false, _wsError is filled with an error string to present to the user.
	 **/
	bool CCheatConverter::LoadMesenJson( const std::vector<uint8_t> &_vJson, std::vector<LSN_CHEAT_ENTRY> &_vCheats,
		std::wstring &_wsError ) {

		lson::CJson jSon;

		if ( !jSon.SetJson( reinterpret_cast<const char *>(_vJson.data()) ) ) {
			_wsError = LSN_LSTR( LSN_INVALID_JSON_FILE );
			return false;
		}

		return true;
	}

 }	// namespace lsn
