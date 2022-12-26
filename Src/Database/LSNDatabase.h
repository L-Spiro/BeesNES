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
		// == Types.
		struct LSN_ENTRY {
			uint32_t							ui32Crc = 0;											/**< Game identifier. */
			LSN_MIRROR_MODE						mmMirrorOverride = LSN_MM_NO_OVERRIDE;					/**< A mirror override, or LSN_MM_NO_OVERRIDE. */
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
