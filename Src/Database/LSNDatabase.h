/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A database of ROM files (IE which mappers/submappers each uses, mirroring modes, regions, etc.)
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Roms/LSNNesHeader.h"

#include <filesystem>
#include <map>


namespace lsn {

	/**
	 * Class CDatabase
	 * \brief A database of ROM files.
	 *
	 * Description: A database of ROM files (IE which mappers/submappers each uses, mirroring modes, regions, etc.)
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

			LSN_PC_VRC6a,
			LSN_PC_VRC6b,
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
			LSN_CONSOLE_TYPE					ctConsoleType = LSN_CT_NOCONSOLETYPE;					/**< The console type. */
			LSN_EXPANSION_DEVICE				edExpansion = LSN_ED_NONE;								/**< What kind of expansion device is necessary for the game? */
			bool								bBusConflicts = true;									/**< Allow bus conflicts? */
		};

		/** An entry in the XML database. */
		struct LSN_DATABASE_ENTRY {
			/** Represents the PRG-ROM data. */
			struct LSN_PRG_ROM {
				uint32_t						ui32Size = 0;											/**< Size of the PRG-ROM in bytes. */
				uint32_t						ui32Crc32 = 0;											/**< CRC32 checksum. */
				char							szSha1[41] = { 0 };										/**< SHA-1 hash string. */
				uint16_t						ui16Sum16 = 0;											/**< 16-bit sum. */
			}									prPrgRom;												/**< The PRG-ROM entry. */

			/** Represents the CHR-ROM data. */
			struct LSN_CHR_ROM {
				uint32_t						ui32Size = 0;											/**< Size of the CHR-ROM in bytes. */
				uint32_t						ui32Crc32 = 0;											/**< CRC32 checksum. */
				uint16_t						ui16Sum16 = 0;											/**< 16-bit sum. */
				char							szSha1[41] = { 0 };										/**< SHA-1 hash string. */
			}									crChrRom;												/**< The CHR-ROM entry. */

			/** Represents the entire ROM data. */
			struct LSN_ROM {
				uint32_t						ui32Size = 0;											/**< Total ROM size in bytes. */
				uint32_t						ui32Crc32 = 0;											/**< CRC32 checksum of the entire ROM. */
				char							szSha1[41] = { 0 };										/**< SHA-1 hash string of the entire ROM. */
			}									rRom;													/**< The combined ROM entry. */

			/**
			 * \brief Represents the PCB properties.
			 */
			struct LSN_PCB {
				uint16_t						ui16Mapper = 0;											/**< The mapper ID. */
				uint16_t						ui16SubMapper = 0;										/**< The sub-mapper ID. */
				uint8_t							ui8Mirroring = 0;										/**< Mirroring type (e.g., "H", "V", "4"). */
				bool							bBattery = false;										/**< True if battery is present. */
			}									pPcb;													/**< The PCB entry. */

			/**
			 * \brief Represents the console properties.
			 */
			struct LSN_CONSOLE {
				LSN_CONSOLE_TYPE				ctType = LSN_CT_NES_FAMICOM;							/**< The console type. */
				LSN_NES2_REGION					nrRegion = LSN_NR_NTSC;									/**< The region. */
			}									cConsole;												/**< The console entry. */

			/**
			 * \brief Represents the expansion device properties.
			 */
			struct LSN_EXPANSION {
				LSN_EXPANSION_DEVICE			edType = LSN_ED_UNSPECIFIED;							/**< The expansion device type. */
			}									eExpansion;												/**< The expansion entry. */
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

		/**
		 * Loads the converted XML database (LSDB) of ROM's.
		 * 
		 * \param _pPath The path to the database to load.
		 * \return Returns the number of items loaded from the database.
		 **/
		static size_t							LoadDatabase( const std::filesystem::path _pPath );

		/**
		 * Converts the XML database to a format that is faster to load and parse.
		 * 
		 * \param _pPathToXml The path to the XML file to load.
		 * \param _pPathToLsdb The path to the output file to which to save the loaded data.
		 * \return Returns the number of items loaded from the XML file and saved to the new location.
		 **/
		static size_t							ConvertXmlDatabase( const std::filesystem::path _pPathToXml, const std::filesystem::path _pPathToLsdb );


		// == Members.
		/** The database map.  CRC is the key. */
		static std::map<uint32_t, LSN_ENTRY>	m_mDatabase;
	};

}	// namespace lsn
