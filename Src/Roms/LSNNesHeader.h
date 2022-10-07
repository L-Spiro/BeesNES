/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Archaic iNES, iNES 0.7, iNES, and iNES 2.0 header.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNRomConstants.h"

namespace lsn {

	/**
	 * Class LSN_NES_HEADER
	 * \brief The Archaic iNES, iNES 0.7, iNES, and iNES 2.0 header.
	 *
	 * Description: The Archaic iNES, iNES 0.7, iNES, and iNES 2.0 header.
	 */
	struct LSN_NES_HEADER {
		char									cNesString[4];				/**< 0-3: Constant $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file). */
		uint8_t									ui8PrgRomSize;				/**< 4: Size of PRG ROM in 16 KB units. */
		uint8_t									ui8ChrRomSize;				/**< 5: Size of CHR ROM in 8 KB units (0 means the board uses CHR RAM). */
		uint8_t									ui8Byte6;					/**< 6: Flags 6 - Mapper, mirroring, battery, trainer. */
		uint8_t									ui8Byte7;					/**< 7: Flags 7 - Mapper, VS/Playchoice, NES 2.0. */
		uint8_t									ui8Byte8;					/**< 8: Flags 8 - PRG-RAM size (rarely used extension). */
		uint8_t									ui8Byte9;					/**< 9: Flags 9 - TV system (rarely used extension). */
		uint8_t									ui8Byte10;					/**< 10: Flags 10 - TV system, PRG-RAM presence (unofficial, rarely used extension). */
		uint8_t									ui8Byte11;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t									ui8Byte12;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t									ui8Byte13;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t									ui8Byte14;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t									ui8Byte15;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */


		// == Functions.
		/**
		 * Gets the ROM header version.
		 *
		 * \return Returns the ROM header version.
		 */
		inline LSN_ROM_HEADER_VERSION			GetHeaderVersion() const;

		/**
		 * Determines if a battery is present.
		 *
		 * \return Returns true if a battery is present, false otherwise.
		 */
		inline bool								HasBattery() const;

		/**
		 * Determines if a trainer is present.
		 *
		 * \return Returns true if a trainer is present, false otherwise.
		 */
		inline bool								HasTrainer() const;

		/**
		 * Gets the PGM ROM size.
		 *
		 * \return Returns the PGM ROM size.
		 */
		inline uint32_t							GetPgmRomSize() const;

		/**
		 * Translates the iNES 2.0 size, returning 0 in the case of error.
		 *
		 * \param _ui32Exp The exponent.
		 * \param _ui32Mult The multiplier.
		 * \return Returns the translated size using the iNES 2.0 formula: 2^E *(MM*2+1).
		 */
		static inline uint32_t					TranslateSize( uint32_t _ui32Exp, uint32_t _ui32Mult );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the ROM header version.
	 *
	 * \return Returns the ROM header version.
	 */
	inline LSN_ROM_HEADER_VERSION LSN_NES_HEADER::GetHeaderVersion() const {
		// If byte 7 AND $0C = $08, and the size taking into account byte 9 does not exceed the actual size of the ROM image, then NES 2.0.
		if ( (ui8Byte7 & 0x0C) == 0x08 ) { return LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2; }	// Can't check the ROM size here.

		// If byte 7 AND $0C = $04, archaic iNES.
		if ( (ui8Byte7 & 0x0C) == 0x40 ) { return LSN_ROM_HEADER_VERSION::LSN_RHV_ARCHAIC_INES; }

		// If byte 7 AND $0C = $00, and bytes 12-15 are all 0, then iNES.
		if ( (ui8Byte7 & 0x0C) == 0x00 &&
			ui8Byte12 == 0x00 && ui8Byte13 == 0x00 && ui8Byte14 == 0x00 && ui8Byte15 == 0x00 ) { return LSN_ROM_HEADER_VERSION::LSN_RHV_INES; }

		// Otherwise, iNES 0.7 or archaic iNES.
		return LSN_ROM_HEADER_VERSION::LSN_RHV_ARCHAIC_INES;
	}

	/**
	 * Determines if a battery is present.
	 *
	 * \return Returns true if a battery is present, false otherwise.
	 */
	inline bool LSN_NES_HEADER::HasBattery() const {
		return (ui8Byte6 & 0x02) == 0x02;
	}

	/**
	 * Determines if a trainer is present.
	 *
	 * \return Returns true if a trainer is present, false otherwise.
	 */
	inline bool LSN_NES_HEADER::HasTrainer() const {
		return (ui8Byte6 & 0x04) == 0x04;
	}

	/**
	 * Gets the PGM ROM size.
	 *
	 * \return Returns the PGM ROM size.
	 */
	inline uint32_t LSN_NES_HEADER::GetPgmRomSize() const {
		switch ( GetHeaderVersion() ) {
			case LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 : {
				// If the MSB nibble is $F, an exponent-multiplier notation is used.
				if ( (ui8Byte9 & 0x0F) == 0x0F ) {
					return TranslateSize( ui8PrgRomSize >> 2, ui8PrgRomSize & 0x03 );
				}
				// Header byte 4 (LSB) and bits 0-3 of Header byte 9 (MSB) together specify its size. If the MSB nibble is $0-E, LSB and MSB together simply specify the PRG-ROM size in 16 KiB units.
				return (((ui8Byte9 & 0x0F) << 8) | ui8PrgRomSize) * 0x4000;
			}
			default : {
				if ( ui8PrgRomSize == 0 ) { return 256 * 0x4000; }
				return ui8PrgRomSize * 0x4000;
			}
		}
	}

	/**
	 * Gets the PGM ROM size.
	 *
	 * \return Returns the PGM ROM size.
	 */
	inline uint32_t LSN_NES_HEADER::GetPgmRomSize() const {
		switch ( GetHeaderVersion() ) {
			case LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 : {
				// If the MSB nibble is $F, an exponent-multiplier notation is used.
				if ( (ui8Byte9 & 0xF0) == 0xF0 ) {
					return TranslateSize( ui8ChrRomSize >> 2, ui8ChrRomSize & 0x03 );
				}
				// Header byte 5 (LSB) and bits 4-7 of Header byte 9 (MSB) specify its size. If the MSB nibble is $0-E, LSB and MSB together simply specify the CHR-ROM size in 8 KiB units.
				return (((ui8Byte9 & 0xF0) << 4) | ui8ChrRomSize) * 0x2000;
			}
			default : {
				return ui8ChrRomSize * 0x2000;
			}
		}
	}

	/**
	 * Translates the iNES 2.0 size, returning 0 in the case of error.
	 *
	 * \param _ui32Exp The exponent.
	 * \param _ui32Mult The multiplier.
	 * \return Returns the translated size using the iNES 2.0 formula: 2^E *(MM*2+1).
	 */
	inline uint32_t LSN_NES_HEADER::TranslateSize( uint32_t _ui32Exp, uint32_t _ui32Mult ) {
		if ( _ui32Exp > 31 ) { return 0; }	// Sanitation.
		uint64_t ui64Size = (1ULL << _ui32Exp) * (_ui32Mult * 2ULL + 1ULL);
		if ( ui64Size > UINT32_MAX ) { return 0; }
		return static_cast<uint32_t>(ui64Size);
	}

}	// namespace lsn
