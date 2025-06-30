/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ROM data.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <map>


namespace lsn {

	/**
	 * Class LSN_ROM
	 * \brief ROM data.
	 *
	 * Description: ROM data.
	 */
	class CDatabase {
	public :
		// == Enumerations.
		/** Chips. */
		enum LSN_CHIP {
			LSN_C_UNKNOWN,

			LSN_C_MMC1							= 100,
			LSN_C_MMC1A,
			LSN_C_MMC1B1,
			LSN_C_MMC1B1H,
			LSN_C_MMC1B2,
			LSN_C_MMC1B2F,
			LSN_C_MMC1B3,
			LSN_C_MMC1C,
			LSN_C_MMC1_END,

			LSN_C_BF909X						= 200,
			LSN_C_BF9093,
			LSN_C_BF9096,
			LSN_C_BF9097,
			LSN_C_BF909X_END,

			LSN_C_UxROM							= 300,
			LSN_C_UOROM,
			LSN_C_UxROM_END,

			LSN_C_SUNSOFT_5A					= 400,
			LSN_C_SUNSOFT_5B
		};

		/** PCB class. */
		enum LSN_PCB_CLASS {
			LSN_PC_UNKNOWN,

			LSN_PC_SAROM						= 1000,
			LSN_PC_SBROM,
			LSN_PC_SCROM,
			LSN_PC_SC1ROM,
			LSN_PC_SEROM,
			LSN_PC_SFROM,
			LSN_PC_SF1ROM,
			LSN_PC_SFEXPROM,
			LSN_PC_SGROM,
			LSN_PC_SHROM,
			LSN_PC_SH1ROM,
			LSN_PC_SIROM,
			LSN_PC_SJROM,
			LSN_PC_SKROM,
			LSN_PC_SLROM,
			LSN_PC_SL1ROM,
			LSN_PC_SL2ROM,
			LSN_PC_SL3ROM,
			LSN_PC_SLRROM,
			LSN_PC_SMROM,
			LSN_PC_SNROM,
			LSN_PC_SOROM,
			LSN_PC_STROM,
			LSN_PC_SUROM,
			LSN_PC_SXROM,
			LSN_PC_SZROM,

			LSN_PC_VRC2a,
			LSN_PC_VRC2b,
			LSN_PC_VRC2c,

			LSN_PC_VRC4a,
			LSN_PC_VRC4b,
			LSN_PC_VRC4c,
			LSN_PC_VRC4d,
			LSN_PC_VRC4e,
			LSN_PC_VRC4f,
		};


		// == Types.
		/** A database entry. */
		struct LSN_ENTRY {
			uint32_t							ui32Crc = 0;											/**< Game identifier. */
			uint32_t							ui32PgmRomSize = 0;										/**< The actual ROM size (in the case of over-dumps). */
			LSN_MIRROR_MODE						mmMirrorOverride = LSN_MM_NO_OVERRIDE;					/**< A mirror override, or LSN_MM_NO_OVERRIDE. */
			LSN_PPU_METRICS						pmRegion = LSN_PM_UNKNOWN;								/**< The game region. */
			LSN_CHIP							cChip = LSN_C_UNKNOWN;									/**< The mapper chip. */
			LSN_PCB_CLASS						pcPcbClass = LSN_PC_UNKNOWN;							/**< the PCB class. */
			uint16_t							ui16Mapper = uint16_t( -1 );							/**< The mapper. */
			uint16_t							ui16SubMapper = uint16_t( -1 );							/**< The sub-mapper. */
		};


		// == Functions.
		/**
		 * Initializes the database with known ROMs.
		 */
		static void								Init();

		/**
		 * Frees all memory associated with this class.
		 */
		static void								Reset();


		// == Members.
		/** The database map.  CRC is the key. */
		static std::map<uint32_t, LSN_ENTRY>	m_mDatabase;
	};

}	// namespace lsn
