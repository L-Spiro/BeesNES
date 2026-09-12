/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Converts cheat files of various types to an internal representation.
 */


 #include "LSNCheatConverter.h"
 #include "../Localization/LSNLocalization.h"
 #include "../Utilities/LSNUtilities.h"

 #include <EEExpEval.h>


 namespace lsn {

	// == Functions.
	/**
	 * Determines if a given cheat is already in the list of cheats.
	 * 
	 * \param _vCheats The list of cheats to check.
	 * \param _cCheat The cheat to check for being in _vCheats.
	 * \return Returns the index of the cheat if found or size_t( -1 ).
	 **/
	size_t CCheatConverter::CheatIsInList( const std::vector<LSN_CHEAT_ENTRY> &_vCheats, const LSN_CHEAT_ENTRY &_cCheat ) {
		for ( auto I = _vCheats.size(); I--; ) {
			if ( _vCheats[I].ctType != _cCheat.ctType ) { continue; }
			if ( _vCheats[I].vAddresses.size() != _cCheat.vAddresses.size() ) { continue; }
			if ( _vCheats[I].wsDescription != _cCheat.wsDescription ) { continue; }
			if ( _vCheats[I].wsNotes != _cCheat.wsNotes ) { continue; }
			if ( _vCheats[I].u16sFile != _cCheat.u16sFile ) { continue; }
			
			for ( auto J = _vCheats[I].vAddresses.size(); J--; ) {
				if ( _vCheats[I].vAddresses[J].sCode != _cCheat.vAddresses[J].sCode ) { continue; }
			}
			return I;
		}
		return size_t( -1 );
	}

	/**
	 * Determines if a cheat is already activated at a given address.
	 * 
	 * \param _vCheats The list of cheats to check.
	 * \param _ui16Address The address to check.
	 * \return Returns true if there is already a cheat active on a given address.
	 **/
	bool CCheatConverter::AddressIsActive( const std::vector<LSN_CHEAT_ENTRY> &_vCheats, uint16_t _ui16Address ) {
		for ( auto I = _vCheats.size(); I--; ) {
			if ( _vCheats[I].bEnabled ) {
				for ( auto J = _vCheats[I].vAddresses.size(); J--; ) {
					if ( _vCheats[I].vAddresses[J].ui16Address == _ui16Address ) { return true; }
				}
			}
		}
		return false;
	}

	/**
	 * Helper function to convert a Game Genie character to its 4-bit hexadecimal nibble.
	 * 
	 * \param _cChar The character to evaluate.
	 * \return Returns the 4-bit mapped value, or 0xFF if the character is invalid.
	 **/
	uint8_t CCheatConverter::DecodeGameGenieChar( char _cChar ) {
		const char * pcMap = "APZLGITYEOXUKSVN";
		for ( size_t stIndex = 0; stIndex < 16; ++stIndex ) {
			if ( pcMap[stIndex] == _cChar ) {
				return static_cast<uint8_t>( stIndex );
			}
			else if ( pcMap[stIndex] + 32 == _cChar ) {
				return static_cast<uint8_t>( stIndex );
			}
		}
		return 0xFF;
	}

	/**
	 * Decodes a 6-character NES Game Genie code into its target memory address and replacement data byte.
	 * 
	 * \param _sCode The 6-character Game Genie code string.
	 * \param _ui16Address A reference to store the decoded 15-bit CPU address.
	 * \param _ui8Data A reference to store the decoded 8-bit data value.
	 * \return Returns true if the string is exactly 6 characters and decoded successfully, false otherwise.
	 **/
	bool CCheatConverter::DecodeGameGenie6( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data ) {
		if ( _sCode.length() != 6 ) {
			return false;
		}

		uint8_t aui8Nibbles[6];
		for ( size_t stIndex = 0; stIndex < 6; ++stIndex ) {
			uint8_t ui8Nibble = DecodeGameGenieChar( _sCode[stIndex] );
			if ( ui8Nibble != 0xFF ) {
				aui8Nibbles[stIndex] = ui8Nibble;
			}
			else {
				return false;
			}
		}

		_ui16Address = 0x8000 | 
			((aui8Nibbles[3] & 0x7) << 12) | 
			((aui8Nibbles[4] & 0x8) << 8) | 
			((aui8Nibbles[5] & 0x7) << 8) | 
			((aui8Nibbles[1] & 0x8) << 4) | 
			((aui8Nibbles[2] & 0x7) << 4) | 
			(aui8Nibbles[3] & 0x8) | 
			(aui8Nibbles[4] & 0x7);

		_ui8Data = ((aui8Nibbles[0] & 0x8) << 4) | 
			((aui8Nibbles[1] & 0x7) << 4) | 
			(aui8Nibbles[5] & 0x8) | 
			(aui8Nibbles[0] & 0x7);

		return true;
	}

	/**
	 * Decodes an 8-character NES Game Genie code into its target memory address, replacement data byte, and compare byte.
	 * 
	 * \param _sCode The 8-character Game Genie code string.
	 * \param _ui16Address A reference to store the decoded 15-bit CPU address.
	 * \param _ui8Data A reference to store the decoded 8-bit data value.
	 * \param _ui8Compare A reference to store the decoded 8-bit compare value.
	 * \return Returns true if the string is exactly 8 characters and decoded successfully, false otherwise.
	 **/
	bool CCheatConverter::DecodeGameGenie8( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data, uint8_t &_ui8Compare ) {
		if ( _sCode.length() != 8 ) {
			return false;
		}

		uint8_t aui8Nibbles[8];
		for ( size_t stIndex = 0; stIndex < 8; ++stIndex ) {
			uint8_t ui8Nibble = DecodeGameGenieChar( _sCode[stIndex] );
			if ( ui8Nibble != 0xFF ) {
				aui8Nibbles[stIndex] = ui8Nibble;
			}
			else {
				return false;
			}
		}

		_ui16Address = 0x8000 | 
			((aui8Nibbles[3] & 0x7) << 12) | 
			((aui8Nibbles[4] & 0x8) << 8) | 
			((aui8Nibbles[5] & 0x7) << 8) | 
			((aui8Nibbles[1] & 0x8) << 4) | 
			((aui8Nibbles[2] & 0x7) << 4) | 
			(aui8Nibbles[3] & 0x8) | 
			(aui8Nibbles[4] & 0x7);

		_ui8Data = ((aui8Nibbles[0] & 0x8) << 4) | 
			((aui8Nibbles[1] & 0x7) << 4) | 
			(aui8Nibbles[7] & 0x8) | 
			(aui8Nibbles[0] & 0x7);

		_ui8Compare = ((aui8Nibbles[6] & 0x8) << 4) | 
			((aui8Nibbles[7] & 0x7) << 4) | 
			(aui8Nibbles[5] & 0x8) | 
			(aui8Nibbles[6] & 0x7);

		return true;
	}

	/**
	 * Encodes a 15-bit CPU address and an 8-bit data value into a 6-character NES Game Genie code.
	 * 
	 * \param _ui16Address The 15-bit CPU address (0x8000 to 0xFFFF).
	 * \param _ui8Data The 8-bit replacement data value.
	 * \return Returns the formatted 6-character Game Genie string.
	 **/
	std::string CCheatConverter::EncodeGameGenie6( uint16_t _ui16Address, uint8_t _ui8Data ) {
		const char * pcMap = "APZLGITYEOXUKSVN";
		std::string sCode = "      ";

		uint8_t aui8Nibbles[6];
		aui8Nibbles[0] = static_cast<uint8_t>(((_ui8Data >> 4) & 0x8) | (_ui8Data & 0x7));
		aui8Nibbles[1] = static_cast<uint8_t>(((_ui16Address >> 4) & 0x8) | ((_ui8Data >> 4) & 0x7));
		aui8Nibbles[2] = static_cast<uint8_t>((_ui16Address >> 4) & 0x7); 
		aui8Nibbles[3] = static_cast<uint8_t>((_ui16Address & 0x8) | ((_ui16Address >> 12) & 0x7));
		aui8Nibbles[4] = static_cast<uint8_t>(((_ui16Address >> 8) & 0x8) | (_ui16Address & 0x7));
		aui8Nibbles[5] = static_cast<uint8_t>((_ui8Data & 0x8) | ((_ui16Address >> 8) & 0x7));

		for ( size_t stIndex = 0; stIndex < 6; ++stIndex ) {
			sCode[stIndex] = pcMap[aui8Nibbles[stIndex]];
		}

		return sCode;
	}

	/**
	 * Encodes a 15-bit CPU address, an 8-bit data value, and an 8-bit compare byte into an 8-character NES Game Genie code.
	 * 
	 * \param _ui16Address The 15-bit CPU address (0x8000 to 0xFFFF).
	 * \param _ui8Data The 8-bit replacement data value.
	 * \param _ui8Compare The 8-bit compare value that must be present at the target address.
	 * \return Returns the formatted 8-character Game Genie string.
	 **/
	std::string CCheatConverter::EncodeGameGenie8( uint16_t _ui16Address, uint8_t _ui8Data, uint8_t _ui8Compare ) {
		const char * pcMap = "APZLGITYEOXUKSVN";
		std::string sCode = "        ";

		uint8_t aui8Nibbles[8];
		aui8Nibbles[0] = static_cast<uint8_t>(((_ui8Data >> 4) & 0x8) | (_ui8Data & 0x7));
		aui8Nibbles[1] = static_cast<uint8_t>(((_ui16Address >> 4) & 0x8) | ((_ui8Data >> 4) & 0x7));
		aui8Nibbles[2] = static_cast<uint8_t>((_ui16Address >> 4) & 0x7); 
		aui8Nibbles[3] = static_cast<uint8_t>((_ui16Address & 0x8) | ((_ui16Address >> 12) & 0x7));
		aui8Nibbles[4] = static_cast<uint8_t>(((_ui16Address >> 8) & 0x8) | (_ui16Address & 0x7));
		aui8Nibbles[5] = static_cast<uint8_t>((_ui8Compare & 0x8) | ((_ui16Address >> 8) & 0x7));
		aui8Nibbles[6] = static_cast<uint8_t>(((_ui8Compare >> 4) & 0x8) | (_ui8Compare & 0x7));
		aui8Nibbles[7] = static_cast<uint8_t>((_ui8Data & 0x8) | ((_ui8Compare >> 4) & 0x7));

		for ( size_t stIndex = 0; stIndex < 8; ++stIndex ) {
			sCode[stIndex] = pcMap[aui8Nibbles[stIndex]];
		}

		return sCode;
	}

	/**
	 * Parses a raw cheat code string in the "AAAA:VV" format.
	 * 
	 * \param _sCode The cheat code string to parse.
	 * \param _ui16Address A reference to store the decoded 16-bit CPU address.
	 * \param _ui8Data A reference to store the decoded 8-bit data value.
	 * \return Returns true if the string is exactly 7 characters and parsed successfully, false otherwise.
	 **/
	bool CCheatConverter::ParseRawCode( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data ) {
		if ( _sCode.length() != 7 ) {
			return false;
		}
    
		if ( _sCode[4] != ':' ) {
			return false;
		}

		uint8_t ui8Addr0 = uint8_t( ee::CExpEval::HexToUint32( _sCode[0] ) );
		uint8_t ui8Addr1 = uint8_t( ee::CExpEval::HexToUint32( _sCode[1] ) );
		uint8_t ui8Addr2 = uint8_t( ee::CExpEval::HexToUint32( _sCode[2] ) );
		uint8_t ui8Addr3 = uint8_t( ee::CExpEval::HexToUint32( _sCode[3] ) );

		if ( ui8Addr0 == 0xFF || ui8Addr1 == 0xFF || ui8Addr2 == 0xFF || ui8Addr3 == 0xFF ) {
			return false;
		}

		_ui16Address = static_cast<uint16_t>((ui8Addr0 << 12) | (ui8Addr1 << 8) | (ui8Addr2 << 4) | ui8Addr3);

		uint8_t ui8Data0 = uint8_t( ee::CExpEval::HexToUint32( _sCode[5] ) );
		uint8_t ui8Data1 = uint8_t( ee::CExpEval::HexToUint32( _sCode[6] ) );

		if ( ui8Data0 == 0xFF || ui8Data1 == 0xFF ) {
			return false;
		}

		_ui8Data = static_cast<uint8_t>((ui8Data0 << 4) | ui8Data1);

		return true;
	}

	/**
	 * Parses a conditional ROM cheat code string in the "AAAA:VV:CC" format.
	 * 
	 * \param _sCode The cheat code string to parse.
	 * \param _ui16Address A reference to store the decoded 16-bit CPU address.
	 * \param _ui8Data A reference to store the decoded 8-bit data value.
	 * \param _ui8Compare A reference to store the decoded 8-bit compare value.
	 * \return Returns true if the string is exactly 10 characters and parsed successfully, false otherwise.
	 **/
	bool CCheatConverter::ParseCompareCode( const std::string &_sCode, uint16_t &_ui16Address, uint8_t &_ui8Data, uint8_t &_ui8Compare ) {
		if ( _sCode.length() != 10 ) {
			return false;
		}
    
		if ( _sCode[4] != ':' || _sCode[7] != ':' ) {
			return false;
		}

		uint8_t ui8Addr0 = uint8_t( ee::CExpEval::HexToUint32( _sCode[0] ) );
		uint8_t ui8Addr1 = uint8_t( ee::CExpEval::HexToUint32( _sCode[1] ) );
		uint8_t ui8Addr2 = uint8_t( ee::CExpEval::HexToUint32( _sCode[2] ) );
		uint8_t ui8Addr3 = uint8_t( ee::CExpEval::HexToUint32( _sCode[3] ) );

		if ( ui8Addr0 == 0xFF || ui8Addr1 == 0xFF || ui8Addr2 == 0xFF || ui8Addr3 == 0xFF ) {
			return false;
		}

		_ui16Address = static_cast<uint16_t>((ui8Addr0 << 12) | (ui8Addr1 << 8) | (ui8Addr2 << 4) | ui8Addr3);

		uint8_t ui8Data0 = uint8_t( ee::CExpEval::HexToUint32( _sCode[5] ) );
		uint8_t ui8Data1 = uint8_t( ee::CExpEval::HexToUint32( _sCode[6] ) );

		if ( ui8Data0 == 0xFF || ui8Data1 == 0xFF ) {
			return false;
		}

		_ui8Data = static_cast<uint8_t>((ui8Data0 << 4) | ui8Data1);

		uint8_t ui8Comp0 = uint8_t( ee::CExpEval::HexToUint32( _sCode[8] ) );
		uint8_t ui8Comp1 = uint8_t( ee::CExpEval::HexToUint32( _sCode[9] ) );

		if ( ui8Comp0 == 0xFF || ui8Comp1 == 0xFF ) {
			return false;
		}

		_ui8Compare = static_cast<uint8_t>((ui8Comp0 << 4) | ui8Comp1);

		return true;
	}

	/**
	 * Reads a Mesen .JSON file and returns all contained cheats inside it.
	 * 
	 * \param _vJson The JSON file loaded into memory.
	 * \param _vCheats The returned cheat list for the given file.
	 * \param _u16sFileName The name of the cheat file.
	 * \param _wsError If false is returned, this contains the error to print for the user.
	 * \return Returns true if all cheats were loaded.  If false, _wsError is filled with an error string to present to the user.
	 **/
	bool CCheatConverter::LoadMesenJson( const std::vector<uint8_t> &_vJson, std::vector<LSN_CHEAT_ENTRY> &_vCheats, const std::u16string &_u16sFileName,
		std::wstring &_wsError ) {
		try {
			lson::CJson jSon;

			if ( !jSon.SetJson( reinterpret_cast<const char *>(_vJson.data()) ) ) {
				_wsError = LSN_LSTR( LSN_INVALID_JSON_FILE );
				return false;
			}

			const lson::CJsonContainer::LSON_JSON_VALUE & jvRoot = jSon.GetContainer()->GetValue( jSon.GetContainer()->GetRoot() );
			if ( jvRoot.vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
			if ( !jvRoot.oObject.vMembers.size() ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }

			size_t stCheats = jSon.GetContainer()->FindString( "Cheats" );
			size_t stDescription = jSon.GetContainer()->FindString( "Description" );
			size_t stType = jSon.GetContainer()->FindString( "Type" );
			size_t stEnabled = jSon.GetContainer()->FindString( "Enabled" );
			size_t stCodes = jSon.GetContainer()->FindString( "Codes" );

			for ( size_t J = 0; J < jvRoot.oObject.vMembers.size(); ++J ) {
				if ( jvRoot.oObject.vMembers[J].stName != stCheats ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
				auto aItem = jSon.GetContainer()->GetValue( jvRoot.oObject.vMembers[J].stValue );
				if ( aItem.vtType != lson::CJsonContainer::LSON_VT_ARRAY ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
				for ( size_t I = 0; I < aItem.vArray.size(); ++I ) {
					auto aThis = jSon.GetContainer()->GetValue( aItem.vArray[I] );
					if ( aThis.vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
					LSN_CHEAT_ENTRY ceEntry;
					for ( auto K = aThis.oObject.vMembers.size(); K--; ) {
						auto aTmp = jSon.GetContainer()->GetValue( aThis.oObject.vMembers[K].stValue );
					
						if ( aThis.oObject.vMembers[K].stName == stDescription ) {
							if ( aTmp.vtType != lson::CJsonContainer::LSON_VT_STRING ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
							auto aThisVal = jSon.GetContainer()->GetValue( aThis.oObject.vMembers[K].stValue );
							if ( aThisVal.vtType != lson::CJsonContainer::LSON_VT_STRING ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
							auto sTmpStr = jSon.GetContainer()->GetString( aThisVal.u.stString );
							ceEntry.wsDescription = ParseDescription( sTmpStr, ceEntry.wsNotes );
						}
						else if ( aThis.oObject.vMembers[K].stName == stType ) {
							if ( aTmp.vtType != lson::CJsonContainer::LSON_VT_STRING ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
							auto aThisVal = jSon.GetContainer()->GetValue( aThis.oObject.vMembers[K].stValue );
							if ( aThisVal.vtType != lson::CJsonContainer::LSON_VT_STRING ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
							auto sTmpStr = jSon.GetContainer()->GetString( aThisVal.u.stString );
							if ( sTmpStr == "NesGameGenie" ) {
								ceEntry.ctType = LSN_CHEAT_ENTRY::LSN_CT_GAME_GENIE;
							}
							else if ( sTmpStr == "NesCustom" ) {
								ceEntry.ctType = LSN_CHEAT_ENTRY::LSN_CT_CUSTOM;
							}
							else {
								lsn::DebugW( std::format( L"{}: Unknown Code type: {}.\r\n", CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() ), CUtilities::XStringToWString( sTmpStr.c_str(), sTmpStr.size() ) ).c_str() );
							}
						}
						else if ( aThis.oObject.vMembers[K].stName == stEnabled ) {
							if ( aTmp.vtType == lson::CJsonContainer::LSON_VT_TRUE ) {
								ceEntry.bEnabled = true;
							}
							else if ( aTmp.vtType == lson::CJsonContainer::LSON_VT_FALSE ) {
								ceEntry.bEnabled = false;
							}
							else { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
						}
						else if ( aThis.oObject.vMembers[K].stName == stCodes ) {
							if ( aTmp.vtType != lson::CJsonContainer::LSON_VT_STRING ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
							auto aThisVal = jSon.GetContainer()->GetValue( aThis.oObject.vMembers[K].stValue );
							if ( aThisVal.vtType != lson::CJsonContainer::LSON_VT_STRING ) { _wsError = LSN_LSTR( LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE ); return false; }
							std::string sEscaped;
							ee::CExpEval::ResolveAllEscapes( jSon.GetContainer()->GetString( aThisVal.u.stString ), sEscaped, true );
							std::string sCodes = CUtilities::Replace( sEscaped, '\r', '\n' );
							auto vCodes = ee::CExpEval::Tokenize<std::string>( sCodes, '\n', false );
							if ( !vCodes.size() ) { continue; }
							ceEntry.vAddresses.resize( vCodes.size() );
							for ( size_t G = 0; G < vCodes.size(); ++G ) {
								ceEntry.vAddresses[G].sCode = vCodes[G];
							}
						}
						else {
							auto sTmpStr = jSon.GetContainer()->GetString( aThis.oObject.vMembers[K].stValue );
							lsn::DebugW( std::format( L"{}: Unknown JSON type: {}.\r\n", CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() ), CUtilities::XStringToWString( sTmpStr.c_str(), sTmpStr.size() ) ).c_str() );
						}
					}
					ceEntry.u16sFile = std::filesystem::path( _u16sFileName ).replace_extension().generic_u16string();
					for ( auto K = ceEntry.vAddresses.size(); K--; ) {
						if ( ceEntry.ctType == LSN_CHEAT_ENTRY::LSN_CT_GAME_GENIE ) {
							if ( ceEntry.vAddresses[K].sCode.size() == 6 ) {
								if ( !DecodeGameGenie6( ceEntry.vAddresses[K].sCode, ceEntry.vAddresses[K].ui16Address, ceEntry.vAddresses[K].ui8Value ) ) {
									if ( _wsError.size() ) {
										_wsError += L"\r\n";
									}
									_wsError += CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() );
									_wsError += L"::";
									_wsError += ceEntry.wsDescription;
									_wsError += L": ";
									_wsError += LSN_LSTR( LSN_STR_INVALID_CODE );
									continue;
								}
								ceEntry.vAddresses[K].bUseCompare = false;
							}
							else if ( ceEntry.vAddresses[K].sCode.size() == 8 ) {
								if ( !DecodeGameGenie8( ceEntry.vAddresses[K].sCode, ceEntry.vAddresses[K].ui16Address, ceEntry.vAddresses[K].ui8Value, ceEntry.vAddresses[K].ui8CompareValue ) ) {
									if ( _wsError.size() ) {
										_wsError += L"\r\n";
									}
									_wsError += CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() );
									_wsError += L"::";
									_wsError += ceEntry.wsDescription;
									_wsError += L": ";
									_wsError += LSN_LSTR( LSN_STR_INVALID_CODE );
									continue;
								}
								ceEntry.vAddresses[K].bUseCompare = true;
							}
							else {
								if ( _wsError.size() ) {
									_wsError += L"\r\n";
								}
								_wsError += CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() );
								_wsError += L"::";
								_wsError += ceEntry.wsDescription;
								_wsError += L": ";
								_wsError += LSN_LSTR( LSN_STR_INVALID_CODE );
								continue;
							}
						}
						else if ( ceEntry.ctType == LSN_CHEAT_ENTRY::LSN_CT_CUSTOM ) {
							if ( ceEntry.vAddresses[K].sCode.size() == 7 ) {
								if ( !ParseRawCode( ceEntry.vAddresses[K].sCode, ceEntry.vAddresses[K].ui16Address, ceEntry.vAddresses[K].ui8Value ) ) {
									if ( _wsError.size() ) {
										_wsError += L"\r\n";
									}
									_wsError += CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() );
									_wsError += L"::";
									_wsError += ceEntry.wsDescription;
									_wsError += L": ";
									_wsError += LSN_LSTR( LSN_STR_INVALID_CODE );
									continue;
								}
								ceEntry.vAddresses[K].bUseCompare = false;
							}
							else if ( ceEntry.vAddresses[K].sCode.size() == 10 ) {
								if ( !ParseCompareCode( ceEntry.vAddresses[K].sCode, ceEntry.vAddresses[K].ui16Address, ceEntry.vAddresses[K].ui8Value, ceEntry.vAddresses[K].ui8CompareValue ) ) {
									if ( _wsError.size() ) {
										_wsError += L"\r\n";
									}
									_wsError += CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() );
									_wsError += L"::";
									_wsError += ceEntry.wsDescription;
									_wsError += L": ";
									_wsError += LSN_LSTR( LSN_STR_INVALID_CODE );
									continue;
								}
								ceEntry.vAddresses[K].bUseCompare = true;
							}
							else {
								if ( _wsError.size() ) {
									_wsError += L"\r\n";
								}
								_wsError += CUtilities::XStringToWString( _u16sFileName.c_str(), _u16sFileName.size() );
								_wsError += L"::";
								_wsError += ceEntry.wsDescription;
								_wsError += L": ";
								_wsError += LSN_LSTR( LSN_STR_INVALID_CODE );
								continue;
							}
						}
					}
					if ( size_t( -1 ) == CheatIsInList( _vCheats, ceEntry ) ) {
						_vCheats.emplace_back( ceEntry );
					}
				}
			}
		}
		catch ( ... ) { _wsError = LSN_LSTR( LSN_OUT_OF_MEMORY ); return false; }
		return _wsError.empty();
	}

	/**
	 * Parses and fixes the description of a cheat. Escapes will be handled, notes will be removed, and the description will be titlized.
	 * 
	 * \param _sDescription The cheat description to parse.
	 * \param _wsNote Parsed notes if any.
	 * \return Returns the parsed cheat description.
	 **/
	std::wstring CCheatConverter::ParseDescription( const std::string &_sDescription, std::wstring &_wsNote ) {
		std::string sEscaped, sEscaped2;
		ee::CExpEval::ResolveAllEscapes( _sDescription, sEscaped2, true );
		ee::CExpEval::ResolveAllHtmlXmlEscapes( sEscaped2, sEscaped, true );
		sEscaped = CUtilities::Replace( sEscaped, std::string( "\\/" ), std::string( "/" ) );
		// Gather until either the end of the string or {{...}} is found.
		std::string sTmp;
		size_t stNoteStart, stNoteEnd;
		if ( FindNoteString( sEscaped, stNoteStart, stNoteEnd ) ) {
			// Notes are not processed.
			_wsNote = ee::CExpEval::ToUtf16( sEscaped.substr( stNoteStart + 2, stNoteEnd - stNoteStart - 3 ) );
			sEscaped.erase( sEscaped.begin() + stNoteStart, sEscaped.begin() + stNoteEnd + 1 );
		}
		// Titlize the description part.
		auto vWords = ee::CExpEval::TokenizeUtf( sEscaped, ' ', false );
		/*static const struct LSN_REPLACE_TABLE {
			const char *		pcReplaceMe;
			const char *		pcWithMe;
		} rtTable[] = {
			{ "A", "a" },
			{ "An", "an" },
			{ "The", "the" },

			{ "And", "and" },
			{ "But", "but" },
			{ "Or", "or" },
			{ "Nor", "nor" },
			{ "So", "so" },
			{ "For", "for" },
			{ "Yet", "yet" },

			{ "In", "in" },
			{ "On", "on" },
			{ "At", "at" },
			{ "By", "by" },
			{ "To", "to" },
			{ "From", "from" },
			{ "Up", "up" },
			{ "Off", "off" },
			{ "In", "in" },
		};*/
		for ( auto I = vWords.size(); I--; ) {
			auto vCompounds = ee::CExpEval::TokenizeUtf( vWords[I], '-', false );
			for ( auto J = vCompounds.size(); J--; ) {
				size_t stIdx = 0;
				bool bFirst = false;
				while ( stIdx < vCompounds[J].size() - 1 && !ee::CExpEval::IsIdentifier( vCompounds[J][stIdx], bFirst ) ) {
					++stIdx;
				}
				vCompounds[J][stIdx] = std::string::value_type( std::toupper( vCompounds[J][stIdx] ) );
#pragma warning( push )
#pragma warning( disable : 4310 )	// warning C4310: cast truncates constant value
				if ( vCompounds[J][stIdx] == '\'' ) {
					vCompounds[J].erase( vCompounds[J].begin() );
					vCompounds[J].insert( vCompounds[J].begin(), char( 0x98 ) );
					vCompounds[J].insert( vCompounds[J].begin(), char( 0x80 ) );
					vCompounds[J].insert( vCompounds[J].begin(), char( 0xE2 ) );
				}
				else if ( vCompounds[J][stIdx] == '"' ) {
					vCompounds[J].erase( vCompounds[J].begin() );
					vCompounds[J].insert( vCompounds[J].begin(), char( 0x9C ) );
					vCompounds[J].insert( vCompounds[J].begin(), char( 0x80 ) );
					vCompounds[J].insert( vCompounds[J].begin(), char( 0xE2 ) );
				}
#pragma warning( pop )
			}
			vWords[I] = ee::CExpEval::Reconstitute<std::string>( vCompounds, '-' );
			
			vWords[I] = CUtilities::Replace( vWords[I], std::string( " / " ), std::string( "/" ) );

			vWords[I] = CUtilities::Replace( vWords[I], std::string( "'" ), std::string( reinterpret_cast<const std::string::value_type *>(u8"\u2019") ) );
			vWords[I] = CUtilities::Replace( vWords[I], std::string( "\"" ), std::string( reinterpret_cast<const std::string::value_type *>(u8"\u201D") ) );
		}

		sTmp = ee::CExpEval::Reconstitute<std::string>( vWords, ' ' );
		while ( sTmp.size() && std::iswspace( sTmp[0] ) ) {
			sTmp.erase( sTmp.begin() );
		}
		while ( sTmp.size() && std::iswspace( sTmp[sTmp.size()-1] ) ) {
			sTmp.pop_back();
		}
		return ee::CExpEval::ToUtf16( sTmp );
	}

	/**
	 * Finds the start and end of a note in a description.
	 * 
	 * \param _sDescription The description to parse for the note indices.
	 * \param _stLeft Upon returning true, this holds the left index of the note string, including the {{.
	 * \param _stRight Upon returning true, this holds the right index of the note string; the index of the closing } in }}.
	 * \return Returns true if a note string is embedded in the description.
	 **/
	bool CCheatConverter::FindNoteString( const std::string &_sDescription, size_t &_stLeft, size_t &_stRight ) {
		for ( size_t I = 0; I < _sDescription.size() - 1; ++I ) {
			if ( _sDescription[I] == '{' && _sDescription[I+1] == '{' ) {
				_stLeft = I;
				// Look for a closing }}.
				for ( size_t J = I + 2; J < _sDescription.size() - 1; ++J ) {
					if ( _sDescription[J] == '}' && _sDescription[J+1] == '}' ) {
						_stRight = J + 1;
						return true;
					}
				}
			}
		}
		return false;
	}

 }	// namespace lsn
