/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Converts cheat files of various types to an internal representation.
 */


#pragma once


#include "../LSNLSpiroNes.h"
#include "LSNCheatEntry.h"

#include <LSONJson.h>

namespace lsn {

	/**
	 * Class CCheatConverter
	 * \brief Converts cheat files of various types to an internal representation.
	 *
	 * Description: Converts cheat files of various types to an internal representation.
	 */
	class CCheatConverter {
	public :
		/*CCheatConverter() {
		}
		~CCheatConverter() {
		}*/


		// == Functions.
		/**
		 * Determines if a given cheat is already in the list of cheats.
		 * 
		 * \param _vCheats The list of cheats to check.
		 * \param _cCheat The cheat to check for being in _vCheats.
		 * \return Returns the index of the cheat if found or size_t( -1 ).
		 **/
		static size_t									CheatIsInList( const std::vector<LSN_CHEAT_ENTRY> &_vCheats, const LSN_CHEAT_ENTRY &_cCheat );

		/**
		 * Determines if a cheat is already activated at a given address.
		 * 
		 * \param _vCheats The list of cheats to check.
		 * \param _ui16Address The address to check.
		 * \return Returns true if there is already a cheat active on a given address.
		 **/
		static bool										AddressIsActive( const std::vector<LSN_CHEAT_ENTRY> &_vCheats, uint16_t _ui16Address );

		/**
		 * Helper function to convert a Game Genie character to its 4-bit hexadecimal nibble.
		 * 
		 * \param _cChar The character to evaluate.
		 * \return Returns the 4-bit mapped value, or 0xFF if the character is invalid.
		 **/
		static uint8_t									DecodeGameGenieChar( char _cChar );

		/**
		 * Decodes a 6-character NES Game Genie code into its target memory address and replacement data byte.
		 * 
		 * \param _sCode The 6-character Game Genie code string.
		 * \param _ui16Address A reference to store the decoded 15-bit CPU address.
		 * \param _ui8Data A reference to store the decoded 8-bit data value.
		 * \return Returns true if the string is exactly 6 characters and decoded successfully, false otherwise.
		 **/
		static bool										DecodeGameGenie6( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data );

		/**
		 * Decodes an 8-character NES Game Genie code into its target memory address, replacement data byte, and compare byte.
		 * 
		 * \param _sCode The 8-character Game Genie code string.
		 * \param _ui16Address A reference to store the decoded 15-bit CPU address.
		 * \param _ui8Data A reference to store the decoded 8-bit data value.
		 * \param _ui8Compare A reference to store the decoded 8-bit compare value.
		 * \return Returns true if the string is exactly 8 characters and decoded successfully, false otherwise.
		 **/
		static bool										DecodeGameGenie8( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data, uint8_t &_ui8Compare );

		/**
		 * Encodes a 15-bit CPU address and an 8-bit data value into a 6-character NES Game Genie code.
		 * 
		 * \param _ui16Address The 15-bit CPU address (0x8000 to 0xFFFF).
		 * \param _ui8Data The 8-bit replacement data value.
		 * \return Returns the formatted 6-character Game Genie string.
		 **/
		static std::string								EncodeGameGenie6( uint16_t _ui16Address, uint8_t _ui8Data );

		/**
		 * Encodes a 15-bit CPU address, an 8-bit data value, and an 8-bit compare byte into an 8-character NES Game Genie code.
		 * 
		 * \param _ui16Address The 15-bit CPU address (0x8000 to 0xFFFF).
		 * \param _ui8Data The 8-bit replacement data value.
		 * \param _ui8Compare The 8-bit compare value that must be present at the target address.
		 * \return Returns the formatted 8-character Game Genie string.
		 **/
		static std::string								EncodeGameGenie8( uint16_t _ui16Address, uint8_t _ui8Data, uint8_t _ui8Compare );

		/**
		 * Parses a raw cheat code string in the "AAAA:VV" format.
		 * 
		 * \param _sCode The cheat code string to parse.
		 * \param _ui16Address A reference to store the decoded 16-bit CPU address.
		 * \param _ui8Data A reference to store the decoded 8-bit data value.
		 * \return Returns true if the string is exactly 7 characters and parsed successfully, false otherwise.
		 **/
		static bool										ParseRawCode( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data );

		/**
		 * Parses a conditional ROM cheat code string in the "AAAA:VV:CC" format.
		 * 
		 * \param _sCode The cheat code string to parse.
		 * \param _ui16Address A reference to store the decoded 16-bit CPU address.
		 * \param _ui8Data A reference to store the decoded 8-bit data value.
		 * \param _ui8Compare A reference to store the decoded 8-bit compare value.
		 * \return Returns true if the string is exactly 10 characters and parsed successfully, false otherwise.
		 **/
		static bool										ParseCompareCode( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data, uint8_t &_ui8Compare );

		/**
		 * Reads a Mesen .JSON file and returns all contained cheats inside it.
		 * 
		 * \param _vJson The JSON file loaded into memory.
		 * \param _vCheats The returned cheat list for the given file.
		 * \param _u16sFileName The name of the cheat file.
		 * \param _wsError If false is returned, this contains the error to print for the user.
		 * \return Returns true if all cheats were loaded.  If false, _wsError is filled with an error string to present to the user.
		 **/
		static bool										LoadMesenJson( const std::vector<uint8_t> &_vJson, std::vector<LSN_CHEAT_ENTRY> &_vCheats, const std::u16string &_u16sFileName,
			std::wstring &_wsError );

		/**
		 * Parses and fixes the description of a cheat. Escapes will be handled, notes will be removed, and the description will be titlized.
		 * 
		 * \param _sDescription The cheat description to parse.
		 * \param _wsNote Parsed notes if any.
		 * \return Returns the parsed cheat description.
		 **/
		static std::wstring								ParseDescription( const std::string &_sDescription, std::wstring &_wsNote );

		/**
		 * Finds the start and end of a note in a description.
		 * 
		 * \param _sDescription The description to parse for the note indices.
		 * \param _stLeft Upon returning true, this holds the left index of the note string, including the {{.
		 * \param _stRight Upon returning true, this holds the right index of the note string; the index of the closing } in }}.
		 * \return Returns true if a note string is embedded in the description.
		 **/
		static bool										FindNoteString( const std::string &_sDescription, size_t &_stLeft, size_t &_stRight );
	};

}	// namespace lsn
