/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A structure for storing extended M3U playlist data, primarily used for NSF tracks.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>

namespace lsn {

	/**
	 * Struct LSN_M3U_ENTRY
	 * \brief A single entry in an extended M3U playlist.
	 *
	 * Description: Stores the parsed fields of an NSF-style M3U playlist track.
	 * Expected format: filename::type,track,title,play_time,loop_count,fade_time
	 */
	struct LSN_M3U_ENTRY {
		std::string										sFile;						/**< The file path of the playlist entry. */
		std::string										sType;						/**< The specific type suffix (e.g., "NSF" from "::NSF"), if provided. */
		int32_t											i32Track;					/**< The track number, typically 1-based (-1 if unspecified). */
		std::string										sTitle;						/**< The title of the track. */
		int32_t											i32PlayTimeMs;				/**< The play duration in milliseconds (-1 for unknown or unspecified). */
		int32_t											i32LoopCount;				/**< The number of times the track should loop (-1 for default/unspecified). */
		int32_t											i32FadeTimeMs;				/**< The fade-out duration in milliseconds (-1 for unknown or unspecified). */
	};

	/**
	 * Struct LSN_M3U
	 * \brief A structure for storing M3U playlist data.
	 *
	 * Description: Stores a collection of M3U entries and state, typically used for NSF track listings or multi-disk images.
	 */
	struct LSN_M3U {
		std::vector<LSN_M3U_ENTRY>						vEntries;					/**< The list of tracks in the playlist. */
		std::vector<std::string>						vComments;					/**< Global comments and metadata parsed from '#' lines. */
		std::string										sBasePath;					/**< The base path of the M3U file, used to resolve relative paths. */
		size_t											stCurrentIndex;				/**< The index of the currently active track in the playlist. */
		bool											bIsExtended;				/**< Indicates if the playlist uses the EXTM3U format. */


		// == Functions.
		/**
		 * Clears the playlist data.
		 */
		inline void										Clear() {
			vEntries.clear();
			vComments.clear();
			sBasePath.clear();
			stCurrentIndex = 0;
			bIsExtended = false;
		}

		/**
		 * Adds a new track entry to the playlist.
		 *
		 * \param _sFile The file path of the track.
		 * \param _sType The type suffix (e.g., "NSF").
		 * \param _i32Track The track number.
		 * \param _sTitle The title of the track.
		 * \param _i32PlayTimeMs The play duration in milliseconds.
		 * \param _i32LoopCount The loop count.
		 * \param _i32FadeTimeMs The fade-out duration in milliseconds.
		 */
		inline void										AddEntry( const std::string &_sFile, const std::string &_sType = "", int32_t _i32Track = -1, const std::string &_sTitle = "", int32_t _i32PlayTimeMs = -1, int32_t _i32LoopCount = -1, int32_t _i32FadeTimeMs = -1 ) {
			LSN_M3U_ENTRY meEntry;
			meEntry.sFile = _sFile;
			meEntry.sType = _sType;
			meEntry.i32Track = _i32Track;
			meEntry.sTitle = _sTitle;
			meEntry.i32PlayTimeMs = _i32PlayTimeMs;
			meEntry.i32LoopCount = _i32LoopCount;
			meEntry.i32FadeTimeMs = _i32FadeTimeMs;
			vEntries.push_back( meEntry );
		}
		
		/**
		 * Adds a global comment or metadata line.
		 *
		 * \param _sComment The comment string to add.
		 */
		inline void										AddComment( const std::string &_sComment ) {
			vComments.push_back( _sComment );
		}

		/**
		 * Parses a single line of text from an M3U file and populates the structure.
		 *
		 * \param _sLine The raw line of text to parse.
		 */
		inline void										ParseLine( const std::string &_sLine ) {
			std::string sClean = _sLine;
			
			// Strip trailing carriage returns and newlines.
			while ( sClean.size() && (sClean.back() == '\r' || sClean.back() == '\n') ) {
				sClean.pop_back();
			}
			
			if ( sClean.empty() ) { return; }

			// Handle comments and EXTM3U tags.
			if ( sClean[0] == '#' ) {
				AddComment( sClean );
				if ( sClean.find( "EXTM3U" ) != std::string::npos ) {
					bIsExtended = true;
				}
				return;
			}

			// Tokenize by comma.
			std::vector<std::string> vTokens;
			size_t stPos = 0;
			while ( stPos <= sClean.length() ) {
				size_t stNext = sClean.find( ',', stPos );
				if ( stNext == std::string::npos ) {
					vTokens.push_back( sClean.substr( stPos ) );
					break;
				}
				vTokens.push_back( sClean.substr( stPos, stNext - stPos ) );
				stPos = stNext + 1;
			}

			if ( vTokens.empty() ) { return; }

			LSN_M3U_ENTRY meEntry;
			meEntry.i32Track = -1;
			meEntry.i32PlayTimeMs = -1;
			meEntry.i32LoopCount = -1;
			meEntry.i32FadeTimeMs = -1;

			// Field 0: Target file and explicit type suffix.
			size_t stTypePos = vTokens[0].rfind( "::" );
			if ( stTypePos != std::string::npos ) {
				meEntry.sFile = vTokens[0].substr( 0, stTypePos );
				meEntry.sType = vTokens[0].substr( stTypePos + 2 );
			}
			else {
				meEntry.sFile = vTokens[0];
			}

			// Field 1: Track index.
			if ( vTokens.size() > 1 && !vTokens[1].empty() ) {
				meEntry.i32Track = std::atoi( vTokens[1].c_str() );
			}

			// Field 2: Track title.
			if ( vTokens.size() > 2 ) {
				meEntry.sTitle = vTokens[2];
			}

			// Field 3: Play duration.
			if ( vTokens.size() > 3 && !vTokens[3].empty() ) {
				meEntry.i32PlayTimeMs = TimeStrToMs( vTokens[3] );
			}

			// Field 4: Loop count.
			if ( vTokens.size() > 4 && !vTokens[4].empty() ) {
				meEntry.i32LoopCount = std::atoi( vTokens[4].c_str() );
			}

			// Field 5: Fade-out duration.
			if ( vTokens.size() > 5 && !vTokens[5].empty() ) {
				meEntry.i32FadeTimeMs = TimeStrToMs( vTokens[5] );
			}

			vEntries.push_back( meEntry );
		}


		// == Functions.
		/**
		 * Safely converts a standard duration string to milliseconds.
		 * Supports standard integer seconds or precision strings (e.g., "1:14.595", "19.55").
		 *
		 * \param _sTime The time string to evaluate.
		 * \return Returns the parsed time in milliseconds, or -1 if the string is empty.
		 */
		static inline int32_t							TimeStrToMs( const std::string &_sTime ) {
			if ( _sTime.empty() ) { return -1; }
			
			int32_t i32Ms = 0;
			size_t stDot = _sTime.find( '.' );
			std::string sMain = _sTime;
			
			if ( stDot != std::string::npos ) {
				sMain = _sTime.substr( 0, stDot );
				std::string sFraction = _sTime.substr( stDot + 1 );
				
				while ( sFraction.length() < 3 ) {
					sFraction += '0';
				}
				if ( sFraction.length() > 3 ) {
					sFraction = sFraction.substr( 0, 3 );
				}
				i32Ms = std::atoi( sFraction.c_str() );
			}
			
			int32_t i32SecondsBase = 0;
			int32_t i32Multiplier = 1;
			
			size_t stStart = sMain.length();
			while ( stStart > 0 ) {
				size_t stPrev = sMain.rfind( ':', stStart - 1 );
				size_t stSubStart = (stPrev == std::string::npos) ? 0 : stPrev + 1;
				
				std::string sVal = sMain.substr( stSubStart, stStart - stSubStart );
				i32SecondsBase += std::atoi( sVal.c_str() ) * i32Multiplier;
				i32Multiplier *= 60;
				
				stStart = (stPrev == std::string::npos) ? 0 : stPrev;
			}
			
			return i32Ms + (i32SecondsBase * 1000);
		}
	};

}	// namespace lsn
