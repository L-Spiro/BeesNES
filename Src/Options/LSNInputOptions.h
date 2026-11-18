/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Input options.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Input/LSNInputEvent.h"
#include "../Input/LSNTurboPattern.h"


namespace lsn {

	/**
	 * Struct LSN_INPUT_OPTIONS
	 * \brief Input options.
	 *
	 * Description: Input options.
	 */
	struct LSN_INPUT_OPTIONS {
		LSN_INPUT_OPTIONS() {
			std::memset( ieButtonMap, 0, sizeof( ieButtonMap ) );
			std::memset( ieTurboButtonMap, 0, sizeof( ieTurboButtonMap ) );
			std::memset( u16TurboPattern, 0, sizeof( u16TurboPattern ) );
		}


		// == Members.
		uint8_t									ui8ConsoleType = uint8_t( LSN_CT_NES );																				/**< The console type (LSN_CT_NES or LSN_CT_FAMICOM). */
		uint8_t									ui8Expansion = uint8_t( LSN_CT_NONE );																				/**< From the LSN_CONTROLLER_TYPES enumeration. */
		uint8_t									ui8Player[4] = { uint8_t( LSN_CT_NONE ), uint8_t( LSN_CT_NONE ), uint8_t( LSN_CT_NONE ), uint8_t( LSN_CT_NONE ) };	/**< The 4 controllers, from the LSN_CONTROLLER_TYPES enumeration. */
		bool									bUseGlobal = true;																									/**< Game settings use global settings. */
		bool									bUseFourScore = false;																								/**< Use Four Score. */

		// Standard controller options for each player.
		LSN_INPUT_EVENT							ieButtonMap[4/** Up to 4 players. */][8/** 8 configurations. */][8/** 8 buttons.*/];								/**< The standard button map for up to 4 players, each with 8 alternatives and 8 buttons. */
		LSN_INPUT_EVENT							ieTurboButtonMap[4/** Up to 4 players. */][8/** 8 configurations. */][8/** 8 buttons.*/];							/**< The turbo button map for up to 4 players, each with 8 alternatives and 8 buttons. */
		char16_t								u16TurboPattern[4/** Up to 4 players. */][8/** 8 configurations. */][8/** 8 buttons.*/][LSN_TURBO_NAME_MAX+1];	/**< The names of the turbo patterns (presets) of the turbo buttons, terminated.  Empty for the standard pattern. */
		uint8_t									ui8TurboSpeed = 1;																									/**< The global turbo speed multiplier (1-LSN_TURBO_SPEED_MAX):  every state of a turbo pattern that uses it lasts this many times longer. */


		// == Functions.
		/**
		 * Gets the name of the turbo pattern of a turbo button.
		 *
		 * \param _sPlayer The player (0-3).
		 * \param _sConfig The configuration (0-7).
		 * \param _sButton The button (0-7).
		 * \return Returns the name of the pattern, or an empty string for the standard pattern.
		 **/
		inline std::u16string					TurboPattern( size_t _sPlayer, size_t _sConfig, size_t _sButton ) const {
			const char16_t * pu16Name = u16TurboPattern[_sPlayer&3][_sConfig&7][_sButton&7];
			size_t sLen = 0;
			while ( sLen < LSN_TURBO_NAME_MAX && pu16Name[sLen] ) { ++sLen; }
			return std::u16string( pu16Name, sLen );
		}

		/**
		 * Sets the name of the turbo pattern of a turbo button.
		 *
		 * \param _sPlayer The player (0-3).
		 * \param _sConfig The configuration (0-7).
		 * \param _sButton The button (0-7).
		 * \param _u16Name The name of the pattern, or an empty string for the standard pattern.  Names longer than LSN_TURBO_NAME_MAX are not stored.
		 **/
		inline void								SetTurboPattern( size_t _sPlayer, size_t _sConfig, size_t _sButton, const std::u16string &_u16Name ) {
			char16_t * pu16Name = u16TurboPattern[_sPlayer&3][_sConfig&7][_sButton&7];
			std::memset( pu16Name, 0, sizeof( u16TurboPattern[0][0][0] ) );
			if ( _u16Name.size() > LSN_TURBO_NAME_MAX ) { return; }
			std::memcpy( pu16Name, _u16Name.data(), _u16Name.size() * sizeof( char16_t ) );
		}

		/**
		 * Gets the global turbo speed multiplier.
		 *
		 * \return Returns the multiplier, 1-LSN_TURBO_SPEED_MAX.
		 **/
		inline uint32_t							TurboSpeed() const { return std::clamp<uint32_t>( ui8TurboSpeed, 1, LSN_TURBO_SPEED_MAX ); }
	};

}	// namespace lsn
