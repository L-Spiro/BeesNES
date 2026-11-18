/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Turbo patterns.  A turbo pattern is a sequence of up to 64 on/off states that a turbo button plays while it is held, with options
 *	for how it is played (looping or once, how many frames each state lasts, whether it restarts when the button is pressed, etc.)  Patterns are
 *	saved as presets (one text file each) in the Turbo folder and are selected by name for each turbo button.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../File/LSNStdFile.h"
#include "../Utilities/LSNUtilities.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>


/** The maximum number of characters in the name of a turbo pattern. */
#define LSN_TURBO_NAME_MAX							40

/** The extension of turbo-pattern files. */
#define LSN_TURBO_EXT								u".bntp"

/** The largest global turbo speed multiplier. */
#define LSN_TURBO_SPEED_MAX							60

namespace lsn {

	/**
	 * Struct LSN_TURBO_PLAYBACK
	 * \brief A turbo pattern and how it is played.  Plain data, copied into each turbo button.
	 *
	 * Description: A turbo pattern and how it is played.  Plain data, copied into each turbo button.
	 */
	struct LSN_TURBO_PLAYBACK {
		uint64_t									ui64Bits = 0x3333333333333333ULL;			/**< The states.  Bit N is state N (1 = pressed).  The default is 1100 repeated. */
		uint8_t										ui8Length = 64;								/**< The number of states used (1-64), from state 0. */
		uint8_t										ui8FramesPerState = 1;						/**< The number of frames each state lasts (1-255), before the global multiplier. */
		bool										bLoop = true;								/**< If true, the pattern repeats while the button is held, otherwise it plays once. */
		bool										bRestartOnPress = true;						/**< Looping patterns only:  if true, the pattern restarts each time the button is pressed, otherwise it continues where it left off.  One-shot patterns always restart. */
		bool										bHoldAfterEnd = false;						/**< One-shot patterns only:  if true, the button reads as pressed after the pattern ends (while it is still held), otherwise as released. */
		bool										bUseGlobalSpeed = true;						/**< If true, the frames per state are multiplied by the global turbo speed multiplier. */


		// == Functions.
		/**
		 * Gets a state.
		 *
		 * \param _ui32Idx The index of the state (0-63).
		 * \return Returns true if the state is pressed.
		 **/
		inline bool									State( uint32_t _ui32Idx ) const { return _ui32Idx < 64 && ((ui64Bits >> _ui32Idx) & 1); }

		/**
		 * Sets a state.
		 *
		 * \param _ui32Idx The index of the state (0-63).
		 * \param _bPressed The new state.
		 **/
		inline void									SetState( uint32_t _ui32Idx, bool _bPressed ) {
			if ( _ui32Idx >= 64 ) { return; }
			if ( _bPressed ) { ui64Bits |= (1ULL << _ui32Idx); }
			else { ui64Bits &= ~(1ULL << _ui32Idx); }
		}

		/**
		 * Gets the number of states actually used, clamped to 1-64.
		 *
		 * \return Returns the number of states used.
		 **/
		inline uint32_t								Length() const { return std::clamp<uint32_t>( ui8Length, 1, 64 ); }

		/**
		 * Gets the number of frames each state lasts after the global multiplier.
		 *
		 * \param _ui32GlobalSpeed The global turbo speed multiplier.
		 * \return Returns the number of frames each state lasts (at least 1).
		 **/
		inline uint32_t								FramesPerState( uint32_t _ui32GlobalSpeed ) const {
			uint32_t ui32Ret = std::max<uint32_t>( ui8FramesPerState, 1 );
			if ( bUseGlobalSpeed ) { ui32Ret *= std::clamp<uint32_t>( _ui32GlobalSpeed, 1, LSN_TURBO_SPEED_MAX ); }
			return ui32Ret;
		}

		/**
		 * Determines whether two playbacks are the same.
		 *
		 * \param _tpOther The playback to compare against.
		 * \return Returns true if all values are the same.
		 **/
		inline bool									operator == ( const LSN_TURBO_PLAYBACK &_tpOther ) const {
			return ui64Bits == _tpOther.ui64Bits && ui8Length == _tpOther.ui8Length && ui8FramesPerState == _tpOther.ui8FramesPerState &&
				bLoop == _tpOther.bLoop && bRestartOnPress == _tpOther.bRestartOnPress && bHoldAfterEnd == _tpOther.bHoldAfterEnd &&
				bUseGlobalSpeed == _tpOther.bUseGlobalSpeed;
		}
	};

	/**
	 * Struct LSN_TURBO_STATE
	 * \brief The playback state of one turbo button.
	 *
	 * Description: The playback state of one turbo button.
	 */
	struct LSN_TURBO_STATE {
		uint64_t									ui64LastFrame = 0;							/**< The frame on which the button was last polled while held. */
		uint32_t									ui32FrameInState = 0;						/**< Frames spent in the current state. */
		uint8_t										ui8State = 0;								/**< The current state. */
		bool										bHeld = false;								/**< The button was held the last time it was polled. */
		bool										bDone = false;								/**< A one-shot pattern has ended. */


		// == Functions.
		/**
		 * Advances the pattern for a poll of a held turbo button and gets whether the button reads as pressed.  The pattern advances once per
		 *	frame no matter how many times the game polls the controller in a frame.
		 *
		 * \param _tpPlayback The pattern.
		 * \param _ui64Frame The current frame.
		 * \param _ui32GlobalSpeed The global turbo speed multiplier.
		 * \return Returns true if the button reads as pressed.
		 **/
		inline bool									Held( const LSN_TURBO_PLAYBACK &_tpPlayback, uint64_t _ui64Frame, uint32_t _ui32GlobalSpeed ) {
			const uint32_t ui32Len = _tpPlayback.Length();
			const uint32_t ui32Fps = _tpPlayback.FramesPerState( _ui32GlobalSpeed );
			if ( !bHeld ) {
				// Just pressed.  One-shot patterns always restart; looping patterns restart if set to (or if they have never been played).
				if ( !_tpPlayback.bLoop || _tpPlayback.bRestartOnPress || bDone || ui8State >= ui32Len ) {
					ui8State = 0;
					ui32FrameInState = 0;
					bDone = false;
				}
				bHeld = true;
				ui64LastFrame = _ui64Frame;
			}
			else {
				// Frames since the last poll (0 if polled again on the same frame).  A frame counter that went backwards (reset) counts as 0.
				uint64_t ui64Frames = _ui64Frame >= ui64LastFrame ? (_ui64Frame - ui64LastFrame) : 0;
				ui64LastFrame = _ui64Frame;
				if ( _tpPlayback.bLoop && ui64Frames ) {
					// Skip whole loops.
					ui64Frames %= uint64_t( ui32Len ) * ui32Fps;
				}
				while ( ui64Frames && !bDone ) {
					const uint64_t ui64Left = ui32Fps - std::min( ui32FrameInState, ui32Fps - 1 );
					if ( ui64Frames < ui64Left ) {
						ui32FrameInState += uint32_t( ui64Frames );
						break;
					}
					ui64Frames -= ui64Left;
					ui32FrameInState = 0;
					if ( ++ui8State >= ui32Len ) {
						if ( _tpPlayback.bLoop ) { ui8State = 0; }
						else { bDone = true; }
					}
				}
			}
			if ( bDone ) { return _tpPlayback.bHoldAfterEnd; }
			return _tpPlayback.State( ui8State );
		}

		/**
		 * Informs the state that the button was polled while not held.
		 **/
		inline void									Released() { bHeld = false; }
	};

	/**
	 * Struct LSN_TURBO_PATTERN
	 * \brief A named turbo pattern (a preset).
	 *
	 * Description: A named turbo pattern (a preset).
	 */
	struct LSN_TURBO_PATTERN {
		std::u16string								u16Name;									/**< The unique name of the pattern. */
		LSN_TURBO_PLAYBACK							tpPlayback;									/**< The pattern and how it is played. */
		std::filesystem::path						pPath;										/**< The file holding the pattern. */
	};

	/**
	 * Class CTurboPatterns
	 * \brief The turbo-pattern presets (the files in the Turbo folder).
	 *
	 * Description: The turbo-pattern presets (the files in the Turbo folder).  Each preset is a small UTF-8 text file:
	 *	BeesNES Turbo Pattern
	 *	Version=1
	 *	Name=...
	 *	Pattern=0101...					(the states in order, 1 to 64 characters; the number of characters is the length)
	 *	FramesPerState=1
	 *	Loop=1
	 *	RestartOnPress=1
	 *	HoldAfterEnd=0
	 *	UseGlobalSpeed=1
	 */
	class CTurboPatterns {
	public :
		// == Functions.
		/**
		 * Loads every preset in a folder (replacing any loaded presets).  Unreadable files and files whose names are already taken are skipped.
		 *
		 * \param _pFolder The folder (the Turbo folder).
		 * \return Returns the number of presets loaded.
		 **/
		size_t										Load( const std::filesystem::path &_pFolder ) {
			m_pFolder = _pFolder;
			m_vPatterns.clear();
			try {
				std::error_code ecErr;
				if ( !std::filesystem::is_directory( _pFolder, ecErr ) ) { return 0; }
				for ( const auto & deEntry : std::filesystem::directory_iterator( _pFolder, ecErr ) ) {
					if ( !deEntry.is_regular_file( ecErr ) ) { continue; }
					if ( CUtilities::ToLower( deEntry.path().extension().u16string() ) != LSN_TURBO_EXT ) { continue; }
					LSN_TURBO_PATTERN tpPattern;
					if ( !LoadFile( deEntry.path(), tpPattern ) ) { continue; }
					if ( Find( tpPattern.u16Name ) ) { continue; }
					m_vPatterns.push_back( std::move( tpPattern ) );
				}
				Sort();
			}
			catch ( ... ) {}
			return m_vPatterns.size();
		}

		/**
		 * Gets the presets, sorted by name.
		 *
		 * \return Returns the presets.
		 **/
		inline const std::vector<LSN_TURBO_PATTERN> &
													Patterns() const { return m_vPatterns; }

		/**
		 * Finds a preset by name (not case-sensitive).
		 *
		 * \param _u16Name The name to find.
		 * \return Returns the preset or nullptr.
		 **/
		const LSN_TURBO_PATTERN *					Find( const std::u16string &_u16Name ) const {
			if ( _u16Name.empty() ) { return nullptr; }
			try {
				std::u16string u16Lower = CUtilities::ToLower( _u16Name );
				for ( const auto & tpThis : m_vPatterns ) {
					if ( CUtilities::ToLower( tpThis.u16Name ) == u16Lower ) { return &tpThis; }
				}
			}
			catch ( ... ) {}
			return nullptr;
		}

		/**
		 * Gets the playback of a preset by name, or the standard pattern if the name is empty or not found.
		 *
		 * \param _u16Name The name of the preset.
		 * \return Returns the playback to use.
		 **/
		LSN_TURBO_PLAYBACK							Playback( const std::u16string &_u16Name ) const {
			const LSN_TURBO_PATTERN * ptpPattern = Find( _u16Name );
			return ptpPattern ? ptpPattern->tpPlayback : LSN_TURBO_PLAYBACK();
		}

		/**
		 * Saves a preset (creating or replacing its file).  The name must be valid and unique (other than the preset being replaced).
		 *
		 * \param _tpPattern The preset to save.  Its path is ignored.
		 * \param _u16OldName The name of the preset being edited (renamed), or an empty string for a new preset.
		 * \return Returns true if the preset was saved.
		 **/
		bool										Save( const LSN_TURBO_PATTERN &_tpPattern, const std::u16string &_u16OldName ) {
			try {
				if ( !ValidName( _tpPattern.u16Name ) ) { return false; }
				const LSN_TURBO_PATTERN * ptpOld = Find( _u16OldName );
				const LSN_TURBO_PATTERN * ptpSame = Find( _tpPattern.u16Name );
				if ( ptpSame && ptpSame != ptpOld ) { return false; }

				std::error_code ecErr;
				std::filesystem::create_directories( m_pFolder, ecErr );
				std::filesystem::path pOldPath = ptpOld ? ptpOld->pPath : std::filesystem::path();
				std::filesystem::path pPath = FileFor( _tpPattern.u16Name, pOldPath );
				std::vector<uint8_t> vFile = Serialize( _tpPattern );
				if ( !CStdFile::WriteToFile( pPath.native().c_str(), vFile ) ) { return false; }
				if ( !pOldPath.empty() && pOldPath != pPath ) {
					std::filesystem::remove( pOldPath, ecErr );
				}

				LSN_TURBO_PATTERN tpNew = _tpPattern;
				tpNew.pPath = pPath;
				if ( ptpOld ) {
					(*const_cast<LSN_TURBO_PATTERN *>(ptpOld)) = std::move( tpNew );
				}
				else {
					m_vPatterns.push_back( std::move( tpNew ) );
				}
				Sort();
				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Deletes a preset and its file.
		 *
		 * \param _u16Name The name of the preset.
		 * \return Returns true if the preset existed and was deleted.
		 **/
		bool										Delete( const std::u16string &_u16Name ) {
			try {
				const LSN_TURBO_PATTERN * ptpPattern = Find( _u16Name );
				if ( !ptpPattern ) { return false; }
				std::error_code ecErr;
				std::filesystem::remove( ptpPattern->pPath, ecErr );
				if ( ecErr ) { return false; }
				m_vPatterns.erase( m_vPatterns.begin() + (ptpPattern - m_vPatterns.data()) );
				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Determines whether a name can be used for a preset:  1 to LSN_TURBO_NAME_MAX characters, not only spaces, and no line breaks.
		 *
		 * \param _u16Name The name to check.
		 * \return Returns true if the name is valid.
		 **/
		static bool									ValidName( const std::u16string &_u16Name ) {
			if ( _u16Name.empty() || _u16Name.size() > LSN_TURBO_NAME_MAX ) { return false; }
			if ( _u16Name.find_first_not_of( u" \t" ) == std::u16string::npos ) { return false; }
			return _u16Name.find_first_of( u"\r\n" ) == std::u16string::npos;
		}

		/**
		 * Converts the used states of a pattern to a string of 1's and 0's.
		 *
		 * \param _tpPlayback The pattern.
		 * \return Returns the states as text (one character per used state).
		 **/
		static std::u16string						ToString( const LSN_TURBO_PLAYBACK &_tpPlayback ) {
			std::u16string u16Ret;
			for ( uint32_t I = 0; I < _tpPlayback.Length(); ++I ) {
				u16Ret.push_back( _tpPlayback.State( I ) ? u'1' : u'0' );
			}
			return u16Ret;
		}

		/**
		 * Reads states from a string of 1's and 0's (other characters, such as spaces, are ignored).
		 *
		 * \param _u16Text The text.
		 * \param _tpPlayback The pattern whose states and length are set.
		 * \return Returns true if the text has 1 to 64 states and nothing but 1's, 0's, and whitespace.
		 **/
		static bool									FromString( const std::u16string &_u16Text, LSN_TURBO_PLAYBACK &_tpPlayback ) {
			uint64_t ui64Bits = 0;
			uint32_t ui32Len = 0;
			for ( auto cThis : _u16Text ) {
				if ( cThis == u' ' || cThis == u'\t' || cThis == u'\r' || cThis == u'\n' ) { continue; }
				if ( cThis != u'0' && cThis != u'1' ) { return false; }
				if ( ui32Len >= 64 ) { return false; }
				if ( cThis == u'1' ) { ui64Bits |= (1ULL << ui32Len); }
				++ui32Len;
			}
			if ( !ui32Len ) { return false; }
			_tpPlayback.ui64Bits = ui64Bits;
			_tpPlayback.ui8Length = uint8_t( ui32Len );
			return true;
		}


	protected :
		// == Members.
		/** The folder of the presets. */
		std::filesystem::path						m_pFolder;
		/** The presets, sorted by name. */
		std::vector<LSN_TURBO_PATTERN>				m_vPatterns;


		// == Functions.
		/**
		 * Sorts the presets by name (not case-sensitive).
		 **/
		void										Sort() {
			std::sort( m_vPatterns.begin(), m_vPatterns.end(), []( const LSN_TURBO_PATTERN &_tpLeft, const LSN_TURBO_PATTERN &_tpRight ) {
				return CUtilities::ToLower( _tpLeft.u16Name ) < CUtilities::ToLower( _tpRight.u16Name );
			} );
		}

		/**
		 * Picks the file for a preset:  its name with the characters that cannot be in file names replaced, made unique if another preset
		 *	already uses that file.
		 *
		 * \param _u16Name The name of the preset.
		 * \param _pKeep A file that may be reused (the file of the preset being replaced).
		 * \return Returns the path of the file.
		 **/
		std::filesystem::path						FileFor( const std::u16string &_u16Name, const std::filesystem::path &_pKeep ) const {
			std::u16string u16Base;
			for ( auto cThis : _u16Name ) {
				switch ( cThis ) {
					case u'<' : {}			LSN_FALLTHROUGH
					case u'>' : {}			LSN_FALLTHROUGH
					case u':' : {}			LSN_FALLTHROUGH
					case u'"' : {}			LSN_FALLTHROUGH
					case u'/' : {}			LSN_FALLTHROUGH
					case u'\\' : {}			LSN_FALLTHROUGH
					case u'|' : {}			LSN_FALLTHROUGH
					case u'?' : {}			LSN_FALLTHROUGH
					case u'*' : { u16Base.push_back( u'_' ); break; }
					default : { u16Base.push_back( cThis < u' ' ? u'_' : cThis ); }
				}
			}
			// No trailing dots or spaces (Windows drops them).
			while ( u16Base.size() && (u16Base.back() == u'.' || u16Base.back() == u' ') ) { u16Base.pop_back(); }
			if ( u16Base.empty() ) { u16Base = u"Pattern"; }

			for ( uint32_t I = 1; ; ++I ) {
				std::u16string u16File = u16Base;
				if ( I > 1 ) {
					std::string sNum = std::to_string( I );
					u16File += u" (";
					for ( auto cThis : sNum ) { u16File.push_back( char16_t( cThis ) ); }
					u16File += u")";
				}
				u16File += LSN_TURBO_EXT;
				std::filesystem::path pPath = m_pFolder / std::filesystem::path( u16File );
				if ( !_pKeep.empty() && pPath == _pKeep ) { return pPath; }
				std::error_code ecErr;
				if ( !std::filesystem::exists( pPath, ecErr ) ) { return pPath; }
			}
		}

		/**
		 * Creates the contents of a preset file.
		 *
		 * \param _tpPattern The preset.
		 * \return Returns the UTF-8 text of the file.
		 **/
		static std::vector<uint8_t>					Serialize( const LSN_TURBO_PATTERN &_tpPattern ) {
			std::u16string u16Text = u"BeesNES Turbo Pattern\r\nVersion=1\r\nName=";
			u16Text += _tpPattern.u16Name;
			u16Text += u"\r\nPattern=";
			u16Text += ToString( _tpPattern.tpPlayback );
			std::string sFps = std::to_string( uint32_t( std::max<uint8_t>( _tpPattern.tpPlayback.ui8FramesPerState, 1 ) ) );
			u16Text += u"\r\nFramesPerState=";
			for ( auto cThis : sFps ) { u16Text.push_back( char16_t( cThis ) ); }
			u16Text += u"\r\nLoop=";
			u16Text += _tpPattern.tpPlayback.bLoop ? u"1" : u"0";
			u16Text += u"\r\nRestartOnPress=";
			u16Text += _tpPattern.tpPlayback.bRestartOnPress ? u"1" : u"0";
			u16Text += u"\r\nHoldAfterEnd=";
			u16Text += _tpPattern.tpPlayback.bHoldAfterEnd ? u"1" : u"0";
			u16Text += u"\r\nUseGlobalSpeed=";
			u16Text += _tpPattern.tpPlayback.bUseGlobalSpeed ? u"1" : u"0";
			u16Text += u"\r\n";
			std::u8string u8Text = CUtilities::Utf16ToUtf8( u16Text.c_str() );
			// The conversion can leave extra terminators at the end.
			const size_t sLen = std::char_traits<char8_t>::length( u8Text.c_str() );
			return std::vector<uint8_t>( reinterpret_cast<const uint8_t *>(u8Text.data()), reinterpret_cast<const uint8_t *>(u8Text.data()) + sLen );
		}

		/**
		 * Loads a preset file.
		 *
		 * \param _pPath The file.
		 * \param _tpPattern Holds the loaded preset.
		 * \return Returns true if the file is a valid preset.
		 **/
		static bool									LoadFile( const std::filesystem::path &_pPath, LSN_TURBO_PATTERN &_tpPattern ) {
			try {
				std::vector<uint8_t> vFile;
				if ( !CStdFile::LoadToMemory( _pPath.native().c_str(), vFile ) ) { return false; }
				size_t sStart = 0;
				if ( vFile.size() >= 3 && vFile[0] == 0xEF && vFile[1] == 0xBB && vFile[2] == 0xBF ) { sStart = 3; }	// BOM.
				std::u8string u8Text( reinterpret_cast<const char8_t *>(vFile.data()) + sStart, vFile.size() - sStart );
				bool bErrored = false;
				std::u16string u16Text = CUtilities::Utf8ToUtf16( u8Text.c_str(), &bErrored );
				if ( bErrored ) { return false; }

				bool bHeader = false, bName = false, bPattern = false;
				size_t sPos = 0;
				while ( sPos <= u16Text.size() ) {
					size_t sEnd = u16Text.find( u'\n', sPos );
					if ( sEnd == std::u16string::npos ) { sEnd = u16Text.size(); }
					std::u16string u16Line = u16Text.substr( sPos, sEnd - sPos );
					sPos = sEnd + 1;
					while ( u16Line.size() && (u16Line.back() == u'\r' || u16Line.back() == u' ' || u16Line.back() == u'\t') ) { u16Line.pop_back(); }
					if ( !bHeader ) {
						if ( u16Line == u"BeesNES Turbo Pattern" ) { bHeader = true; continue; }
						return false;
					}
					size_t sEq = u16Line.find( u'=' );
					if ( sEq == std::u16string::npos ) { continue; }
					std::u16string u16Key = CUtilities::ToLower( u16Line.substr( 0, sEq ) );
					std::u16string u16Val = u16Line.substr( sEq + 1 );
					auto aNum = [&]() {
						uint32_t ui32Ret = 0;
						for ( auto cThis : u16Val ) {
							if ( cThis < u'0' || cThis > u'9' ) { break; }
							ui32Ret = std::min<uint32_t>( ui32Ret * 10 + uint32_t( cThis - u'0' ), 0xFFFF );
						}
						return ui32Ret;
					};
					if ( u16Key == u"name" ) {
						_tpPattern.u16Name = u16Val;
						bName = ValidName( u16Val );
					}
					else if ( u16Key == u"pattern" ) { bPattern = FromString( u16Val, _tpPattern.tpPlayback ); }
					else if ( u16Key == u"framesperstate" ) { _tpPattern.tpPlayback.ui8FramesPerState = uint8_t( std::clamp<uint32_t>( aNum(), 1, 255 ) ); }
					else if ( u16Key == u"loop" ) { _tpPattern.tpPlayback.bLoop = aNum() != 0; }
					else if ( u16Key == u"restartonpress" ) { _tpPattern.tpPlayback.bRestartOnPress = aNum() != 0; }
					else if ( u16Key == u"holdafterend" ) { _tpPattern.tpPlayback.bHoldAfterEnd = aNum() != 0; }
					else if ( u16Key == u"useglobalspeed" ) { _tpPattern.tpPlayback.bUseGlobalSpeed = aNum() != 0; }
				}
				_tpPattern.pPath = _pPath;
				return bHeader && bName && bPattern;
			}
			catch ( ... ) { return false; }
		}
	};

}	// namespace lsn
