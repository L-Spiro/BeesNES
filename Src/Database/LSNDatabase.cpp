/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ROM data.
 */


#include "LSNDatabase.h"

namespace lsn {

	// == Members.
	/** The database map.  CRC is the key. */
	std::map<uint32_t, CDatabase::LSN_ENTRY> CDatabase::m_mDatabase;

	// == Functions.
	/**
	* Initializes the database with known ROMs.
	*/
	void CDatabase::Init() {
		LSN_ENTRY eEntries[] = {
			// Deathbots.
			{ .ui32Crc = 0x8B781D39, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Ms. Pac Man.
			{ .ui32Crc = 0x4B2DCE64, .mmMirrorOverride = LSN_MM_HORIZONTAL },
			// Perman Part 2.
			{ .ui32Crc = 0xDBA3A02E, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Zippy Race.
			{ .ui32Crc = 0xE16BB5FE, .mmMirrorOverride = LSN_MM_HORIZONTAL },
		};
		for ( auto I = LSN_ELEMENTS( eEntries ); I--; ) {
			m_mDatabase.insert( std::pair<uint32_t, LSN_ENTRY>( eEntries[I].ui32Crc, eEntries[I] ) );
		}
	}

	/**
	 * Frees all memory associated with this class.
	 */
	void CDatabase::Reset() {
		m_mDatabase = std::map<uint32_t, LSN_ENTRY>();
	}


}	// namespace lsn