/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Archaic iNES, iNES 0.7, iNES, and iNES 2.0 header.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNRomConstants.h"
#include <cmath>

namespace lsn {

	// == Enumerations.
	/**
	 * \brief The NES 2.0 console type.
	 */
	enum LSN_CONSOLE_TYPE {
		LSN_CT_NES_FAMICOM					= 0,						/**< Standard Nintendo Entertainment System or Famicom. */
		LSN_CT_VS_SYSTEM					= 1,						/**< Nintendo Vs. System. */
		LSN_CT_PLAYCHOICE_10				= 2,						/**< Nintendo PlayChoice-10. */
		LSN_CT_FAMICOM_EPSON_EVA_ADPCM		= 3,						/**< Regular Famicom with EPSON EVA-ADPCM. */
		LSN_CT_VT01							= 4,						/**< V.R. Technology VT01 with red/cyan ADC. */
		LSN_CT_VT02							= 5,						/**< V.R. Technology VT02. */
		LSN_CT_VT03							= 6,						/**< V.R. Technology VT03. */
		LSN_CT_VT09							= 7,						/**< V.R. Technology VT09. */
		LSN_CT_VT32							= 8,						/**< V.R. Technology VT32. */
		LSN_CT_VT369						= 9,						/**< V.R. Technology VT369. */
		LSN_CT_UM6578						= 10,						/**< UMC UM6578. */
		LSN_CT_FAMICOM_NETWORK_SYSTEM		= 11,						/**< Famicom Network System. */
		LSN_CT_SECAM_EP1X					= 12,						/**< Computer & Entertainment SECAM Ep1x. */

		LSN_CT_NOCONSOLETYPE				= -1,						/**< Unspecified. */
	};

	/**
	 * \brief The NES 2.0 CPU/PPU timing region.
	 */
	enum LSN_NES2_REGION {
		LSN_NR_NTSC							= 0,						/**< RP2C02 (NTSC NES). */
		LSN_NR_PAL							= 1,						/**< RP2C07 (Licensed PAL NES). */
		LSN_NR_MULTI_REGION					= 2,						/**< Multiple-region. */
		LSN_NR_DENDY						= 3,						/**< UA6538 (Dendy/Famiclone). */

		LSN_NR_NONE							= 0xFFE2,					/**< Unspecified. */
	};

	/**
	 * \brief The NES 2.0 default expansion device.
	 */
	enum LSN_EXPANSION_DEVICE {
		LSN_ED_UNSPECIFIED					= 0,						/**< Unspecified expansion device. */
		LSN_ED_STANDARD_CONTROLLERS			= 1,						/**< Standard NES/Famicom controllers. */
		LSN_ED_NES_FOUR_SCORE				= 2,						/**< NES Four Score / Satellite. */
		LSN_ED_FAMICOM_FOUR_PLAYER			= 3,						/**< Famicom Four Player Adapter. */
		LSN_ED_VS_SYSTEM					= 4,						/**< Vs. System. */
		LSN_ED_VS_SYSTEM_REVERSED			= 5,						/**< Vs. System with reversed inputs. */
		LSN_ED_VS_PINBALL					= 6,						/**< Vs. Pinball. */
		LSN_ED_VS_ZAPPER					= 7,						/**< Vs. Zapper. */
		LSN_ED_ZAPPER						= 8,						/**< Zapper. */
		LSN_ED_TWO_ZAPPERS					= 9,						/**< Two Zappers. */
		LSN_ED_BANDAI_HYPER_SHOT			= 10,						/**< Bandai Hyper Shot. */
		LSN_ED_POWER_PAD_SIDE_A				= 11,						/**< Power Pad Side A. */
		LSN_ED_POWER_PAD_SIDE_B				= 12,						/**< Power Pad Side B. */
		LSN_ED_FAMILY_TRAINER_SIDE_A		= 13,						/**< Family Trainer Side A. */
		LSN_ED_FAMILY_TRAINER_SIDE_B		= 14,						/**< Family Trainer Side B. */
		LSN_ED_ARKANOID_VAUS_NES			= 15,						/**< Arkanoid Vaus Controller (NES). */
		LSN_ED_ARKANOID_VAUS_FAMICOM		= 16,						/**< Arkanoid Vaus Controller (Famicom). */
		LSN_ED_TWO_VAUS_CONTROLLERS			= 17,						/**< Two Vaus Controllers. */
		LSN_ED_KONAMI_HYPER_SHOT			= 18,						/**< Konami Hyper Shot. */
		LSN_ED_COCONUTS_PACHINKO			= 19,						/**< Coconuts Pachinko Controller. */
		LSN_ED_EXCITING_BOXING				= 20,						/**< Exciting Boxing Punching Board. */
		LSN_ED_JISSEN_MAHJONG				= 21,						/**< Jissen Mahjong Controller. */
		LSN_ED_PARTYTAP						= 22,						/**< PartyTap. */
		LSN_ED_OEKA_KIDS_TABLET				= 23,						/**< Oeka Kids Tablet. */
		LSN_ED_SUNSOFT_BARCODE_BATTLER		= 24,						/**< Sunsoft Barcode Battler. */
		LSN_ED_MIRACLE_PIANO				= 25,						/**< Miracle Piano Keyboard. */
		LSN_ED_POKKUN_MOGURAA				= 26,						/**< Pokkun Moguraa. */
		LSN_ED_TOP_RIDER					= 27,						/**< Top Rider. */
		LSN_ED_DOUBLE_FISTED				= 28,						/**< Double-Fisted. */
		LSN_ED_FAMICOM_3D_SYSTEM			= 29,						/**< Famicom 3D System. */
		LSN_ED_DOREMIKKO_KEYBOARD			= 30,						/**< Doremikko Keyboard. */
		LSN_ED_ROB_GYRO_SET					= 31,						/**< R.O.B. Gyro Set. */
		LSN_ED_ROB_STACK_UP					= 32,						/**< R.O.B. Stack-Up. */
		LSN_ED_ASCII_KEYBOARD				= 33,						/**< ASCII Enigma ROM (Keyboard) / Data Recorder. */
		LSN_ED_FAMILY_BASIC_KEYBOARD		= 34,						/**< Family BASIC Keyboard. */
		LSN_ED_PEC_586_KEYBOARD				= 35,						/**< PEC-586 Keyboard. */
		LSN_ED_BIT_CORP_KEYBOARD			= 36,						/**< Bit Corp. Keyboard. */
		LSN_ED_SUBOR_KEYBOARD				= 37,						/**< Subor Keyboard. */
		LSN_ED_SUBOR_KEYBOARD_MOUSE_3X8		= 38,						/**< Subor Keyboard + Mouse (3x8-bit). */
		LSN_ED_SUBOR_KEYBOARD_MOUSE_24		= 39,						/**< Subor Keyboard + Mouse (24-bit). */
		LSN_ED_SNES_MOUSE					= 40,						/**< SNES Mouse. */
		LSN_ED_MULTICART					= 41,						/**< Multicart. */
		LSN_ED_TWO_SNES_CONTROLLERS			= 42,						/**< Two Super Nintendo Controllers. */
		LSN_ED_RACERMATE_BICYCLE			= 43,						/**< RacerMate Bicycle. */
		LSN_ED_U_FORCE						= 44,						/**< U-Force. */
		LSN_ED_ROB							= 45,						/**< R.O.B. */
		LSN_ED_SNES_MOUSE_4017_D0			= 46,						/**< SNES Mouse ($4017.d0). */
		LSN_ED_ZAPPER_4017					= 47,						/**< Zapper ($4017). */
		LSN_ED_SHARP_C1_KEYBOARD			= 48,						/**< Sharp C1 Keyboard. */
		LSN_ED_STANDARD_CONTROLLER_SWAPPED	= 49,						/**< Standard Controller, swapped. */
		LSN_ED_EXCALIBUR_SUDOKU_PAD			= 50,						/**< Excalibur Sudoku Pad. */
		LSN_ED_ABL_PINBALL					= 51,						/**< ABL Pinball. */
		LSN_ED_GOLDEN_NUGGET_CASINO			= 52,						/**< Golden Nugget Casino. */
		LSN_ED_FAMICOM_NETWORK_CONTROLLER	= 53,						/**< Famicom Network Controller. */

		LSN_ED_NONE							= -1,						/**< Unspecified. */
	};

	/**
	 * Struct LSN_NES_HEADER
	 * \brief The Archaic iNES, iNES 0.7, iNES, and iNES 2.0 header.
	 *
	 * Description: The Archaic iNES, iNES 0.7, iNES, and iNES 2.0 header.
	 */
	struct LSN_NES_HEADER {
		char								cNesString[4];				/**< 0-3: Constant $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file). */
		uint8_t								ui8PrgRomSize;				/**< 4: Size of PRG ROM in 16 KB units. */
		uint8_t								ui8ChrRomSize;				/**< 5: Size of CHR ROM in 8 KB units (0 means the board uses CHR RAM). */
		uint8_t								ui8Byte6;					/**< 6: Flags 6 - Mapper, mirroring, battery, trainer. */
		uint8_t								ui8Byte7;					/**< 7: Flags 7 - Mapper, VS/Playchoice, NES 2.0. */
		uint8_t								ui8Byte8;					/**< 8: Flags 8 - PRG-RAM size (rarely used extension). */
		uint8_t								ui8Byte9;					/**< 9: Flags 9 - TV system (rarely used extension). */
		uint8_t								ui8Byte10;					/**< 10: Flags 10 - TV system, PRG-RAM presence (unofficial, rarely used extension). */
		uint8_t								ui8Byte11;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t								ui8Byte12;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t								ui8Byte13;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t								ui8Byte14;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */
		uint8_t								ui8Byte15;					/**< 11-15: Unused padding (should be filled with zero, but some rippers put their names across bytes 7-15). */


		// == Functions.
		/**
		 * Gets the ROM header version.
		 *
		 * \return Returns the ROM header version.
		 */
		inline LSN_ROM_HEADER_VERSION		GetHeaderVersion() const;

		/**
		 * Gets the game console region.
		 *
		 * \return Returns the game console region.
		 */
		inline LSN_PPU_METRICS				GetGameRegion() const;

		/**
		 * Determines if a battery is present.
		 *
		 * \return Returns true if a battery is present, false otherwise.
		 */
		inline bool							HasBattery() const;

		/**
		 * Determines if a trainer is present.
		 *
		 * \return Returns true if a trainer is present, false otherwise.
		 */
		inline bool							HasTrainer() const;

		/**
		 * Gets the mapper.
		 *
		 * \return Returns the mapper ID.
		 */
		inline uint16_t						GetMapper() const;

		/**
		 * Gets the sub-mapper.
		 *
		 * \return Returns the sub-mapper ID.
		 */
		inline uint16_t						GetSubMapper() const;

		/**
		 * Gets the ROM mirror mode.
		 *
		 * \return Gets the mirroring mode for the ROM.
		 */
		inline LSN_MIRROR_MODE				GetMirrorMode() const;

		/**
		 * Gets the PGM ROM size.
		 *
		 * \return Returns the PGM ROM size.
		 */
		inline uint32_t						GetPgmRomSize() const;

		/**
		 * Gets the CHR ROM size.
		 *
		 * \return Returns the CHR ROM size.
		 */
		inline uint32_t						GetChrRomSize() const;

		/**
		 * Gets the work RAM size.
		 *
		 * \return Returns the work RAM size.
		 */
		inline uint32_t						GetWorkRamSize() const;

		/**
		 * Gets the save RAM size.
		 *
		 * \return Returns the save RAM size.
		 */
		inline uint32_t						GetSaveRamSize() const;

		/**
		 * Gets the CHR RAM size.
		 *
		 * \return Returns the CHR RAM size.
		 */
		inline uint32_t						GetChrRamSize() const;

		/**
		 * Gets the save CHR RAM size.
		 *
		 * \return Returns the save CHR RAM size.
		 */
		inline uint32_t						GetSaveChrRamSize() const;

		/**
		 * Translates the iNES 2.0 size, returning 0 in the case of error.
		 *
		 * \param _ui32Exp The exponent.
		 * \param _ui32Mult The multiplier.
		 * \return Returns the translated size using the iNES 2.0 formula: 2^E *(MM*2+1).
		 */
		static inline uint32_t				TranslateSize( uint32_t _ui32Exp, uint32_t _ui32Mult );
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
	 * Gets the game console region.
	 *
	 * \return Returns the game console region.
	 */
	inline LSN_PPU_METRICS LSN_NES_HEADER::GetGameRegion() const {
		switch ( GetHeaderVersion() ) {
			case LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 : {
				switch ( ui8Byte12 & 0x03 ) {
					case 0 : { return LSN_PPU_METRICS::LSN_PM_NTSC; }
					case 1 : { return LSN_PPU_METRICS::LSN_PM_PAL; }
					case 2 : { return LSN_PPU_METRICS::LSN_PM_NTSC; }
					case 3 : { return LSN_PPU_METRICS::LSN_PM_DENDY; }
				}
				break;
			}
			case LSN_ROM_HEADER_VERSION::LSN_RHV_ARCHAIC_INES : {
				return (ui8Byte9 & 0x01) ? LSN_PPU_METRICS::LSN_PM_PAL : LSN_PPU_METRICS::LSN_PM_UNKNOWN;
			}
			default : { break; }
		}
		return LSN_PPU_METRICS::LSN_PM_UNKNOWN;
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
	 * Gets the mapper.
	 *
	 * \return Returns the mapper ID.
	 */
	inline uint16_t LSN_NES_HEADER::GetMapper() const {
		switch( GetHeaderVersion() ) {
			case LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 : {
				return ((ui8Byte8 & 0x0F) << 8) | (ui8Byte7 & 0xF0) | (ui8Byte6 >> 4);
			}
			case LSN_ROM_HEADER_VERSION::LSN_RHV_ARCHAIC_INES : {
				return (ui8Byte6 >> 4);
			}
			default : {
				return (ui8Byte7 & 0xF0) | (ui8Byte6 >> 4);
			}
		}
	}

	/**
	 * Gets the sub-mapper.
	 *
	 * \return Returns the sub-mapper ID.
	 */
	inline uint16_t LSN_NES_HEADER::GetSubMapper() const {
		switch( GetHeaderVersion() ) {
			case LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 : {
				return (ui8Byte8 & 0xF0) >> 4;
			}
			default : { return 0; }
		}
	}

	/**
	 * Gets the ROM mirror mode.
	 *
	 * \return Gets the mirroring mode for the ROM.
	 */
	inline LSN_MIRROR_MODE LSN_NES_HEADER::GetMirrorMode() const {
		if ( ui8Byte6 & 0x08 ) { return LSN_MM_4_SCREENS; }
		return (ui8Byte6 & 0x01) ? LSN_MM_VERTICAL : LSN_MM_HORIZONTAL;
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
	 * Gets the CHR ROM size.
	 *
	 * \return Returns the CHR ROM size.
	 */
	inline uint32_t LSN_NES_HEADER::GetChrRomSize() const {
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
	 * Gets the work RAM size.
	 *
	 * \return Returns the work RAM size.
	 */
	uint32_t LSN_NES_HEADER::GetWorkRamSize() const {
		if ( GetHeaderVersion() == LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 ) {
			uint8_t ui8Val = ui8Byte10 & 0x0F;
			return ui8Val == 0 ? 0 : uint32_t( std::pow( 2.0, ui8Val - 1.0 ) ) << 7;
		}
		return 0;
	}

	/**
	 * Gets the save RAM size.
	 *
	 * \return Returns the save RAM size.
	 */
	uint32_t LSN_NES_HEADER::GetSaveRamSize() const {
		if ( GetHeaderVersion() == LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 ) {
			uint8_t ui8Val = (ui8Byte10 & 0xF0) >> 4;
			return ui8Val == 0 ? 0 : uint32_t( std::pow( 2.0, ui8Val - 1.0 ) ) << 7;
		}
		return 0;
	}

	/**
	 * Gets the CHR RAM size.
	 *
	 * \return Returns the CHR RAM size.
	 */
	uint32_t LSN_NES_HEADER::GetChrRamSize() const {
		if ( GetHeaderVersion() == LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 ) {
			uint8_t ui8Val = ui8Byte11 & 0x0F;
			return ui8Val == 0 ? 0 : uint32_t( std::pow( 2.0, ui8Val - 1.0 ) ) << 7;
		}
		return 0;
	}

	/**
	 * Gets the save CHR RAM size.
	 *
	 * \return Returns the save CHR RAM size.
	 */
	uint32_t LSN_NES_HEADER::GetSaveChrRamSize() const {
		if ( GetHeaderVersion() == LSN_ROM_HEADER_VERSION::LSN_RHV_INES_2 ) {
			uint8_t ui8Val = (ui8Byte11 & 0xF0) >> 4;
			return ui8Val == 0 ? 0 : uint32_t( std::pow( 2.0, ui8Val - 1.0 ) ) << 7;
		}
		return 0;
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
