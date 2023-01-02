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
			// ========
			// REGIONS
			// ========
			// 3 in 1 Supergun (Asia) (Unl)
			{ .ui32Crc = 0x789270E0, .pmRegion = LSN_PM_PAL },
			// 25th Anniversary Super Mario Bros. (Europe) (Promo, Virtual Console)
			{ .ui32Crc = 0x967A605F, .pmRegion = LSN_PM_PAL },
			// Action in New York (Europe)
			{ .ui32Crc = 0x2E1790A4, .pmRegion = LSN_PM_PAL },
			// Addams Family, The - Pugsley's Scavenger Hunt (Europe) (Beta)
			{ .ui32Crc = 0x6A741EC6, .pmRegion = LSN_PM_PAL },
			// Addams Family, The - Pugsley's Scavenger Hunt (Europe)
			{ .ui32Crc = 0x4FD3C549, .pmRegion = LSN_PM_PAL },
			// Addams Family, The (Europe) (En,Fr,De)
			{ .ui32Crc = 0x0FA94D88, .pmRegion = LSN_PM_PAL },
			// Adventures in the Magic Kingdom (Europe)
			{ .ui32Crc = 0x0E3A7F49, .pmRegion = LSN_PM_PAL },
			// Adventures of Bayou Billy, The (Europe)
			{ .ui32Crc = 0xFBFC6A6C, .pmRegion = LSN_PM_PAL },
			// Adventures of Lolo (Europe)
			{ .ui32Crc = 0x8E773E04, .pmRegion = LSN_PM_PAL },
			// Adventures of Rad Gravity, The (Europe)
			{ .ui32Crc = 0xE4C1A245, .pmRegion = LSN_PM_PAL },
			// Air Fortress (Europe)
			{ .ui32Crc = 0x991CBDF2, .pmRegion = LSN_PM_PAL },
			// Airwolf (Europe)
			{ .ui32Crc = 0xF71A9931, .pmRegion = LSN_PM_PAL },
			// Aladdin (Europe)
			{ .ui32Crc = 0x41D32FD7, .pmRegion = LSN_PM_PAL },
			// Alpha Mission (Europe)
			{ .ui32Crc = 0x3F8FCCF9, .pmRegion = LSN_PM_PAL },
			// Anticipation (Europe)
			{ .ui32Crc = 0xB391A86D, .pmRegion = LSN_PM_PAL },
			// Asterix (Europe) (En,Fr,De,Es,It)
			{ .ui32Crc = 0xED77B453, .pmRegion = LSN_PM_PAL },
			// Attack of the Killer Tomatoes (Europe)
			{ .ui32Crc = 0x58EC824F, .pmRegion = LSN_PM_PAL },
			// Aussie Rules Footy (Australia)
			{ .ui32Crc = 0xC004915A, .pmRegion = LSN_PM_PAL },
			// Barbie (Europe)
			{ .ui32Crc = 0x0538A4E9, .pmRegion = LSN_PM_PAL },
			// Barker Bill's Trick Shooting (Europe)
			{ .ui32Crc = 0x2970D05B, .pmRegion = LSN_PM_PAL },
			// Battle of Olympus, The (Europe)
			{ .ui32Crc = 0xA97567A4, .pmRegion = LSN_PM_PAL },
			// Battletoads (Europe)
			{ .ui32Crc = 0x524A5A32, .pmRegion = LSN_PM_PAL },
			// Battletoads-Double Dragon (Europe)
			{ .ui32Crc = 0x23D7D48F, .pmRegion = LSN_PM_PAL },
			// Bigfoot (Europe)
			{ .ui32Crc = 0x629E060B, .pmRegion = LSN_PM_PAL },
			// Bionic Commando (Europe)
			{ .ui32Crc = 0xFA7EE642, .pmRegion = LSN_PM_PAL },
			// Blaster Master (Europe)
			{ .ui32Crc = 0xB40870A2, .pmRegion = LSN_PM_PAL },
			// Boulder Dash (Europe)
			{ .ui32Crc = 0x54A0C2F0, .pmRegion = LSN_PM_PAL },
			// Bubble Bobble (Europe)
			{ .ui32Crc = 0x1F0D03F8, .pmRegion = LSN_PM_PAL },
			// Castlevania (Europe)
			{ .ui32Crc = 0xA93527E2, .pmRegion = LSN_PM_PAL },
			// Castlevania II - Simon's Quest (Europe)
			{ .ui32Crc = 0x72DDBD39, .pmRegion = LSN_PM_PAL },
			// Championship Rally (Europe)
			{ .ui32Crc = 0x10B4CE4D, .pmRegion = LSN_PM_PAL },
			// Chessmaster, The (Europe)
			{ .ui32Crc = 0x64C97986, .pmRegion = LSN_PM_PAL },
			// Chevaliers du Zodiaque, Les - La Legende d'Or (France)
			{ .ui32Crc = 0x98C546E0, .pmRegion = LSN_PM_DENDY },
			// Chip 'n Dale - Rescue Rangers (Europe)
			{ .ui32Crc = 0xAC7A54CC, .pmRegion = LSN_PM_PAL },
			// Chip 'n Dale - Rescue Rangers 2 (Europe)
			{ .ui32Crc = 0xCCE5A91F, .pmRegion = LSN_PM_PAL },
			// City Connection (Europe)
			{ .ui32Crc = 0xBBB3DE0A, .pmRegion = LSN_PM_PAL },
			// Corvette ZR-1 Challenge (Europe)
			{ .ui32Crc = 0x07637EE4, .pmRegion = LSN_PM_PAL },
			// Cosmos Cop (Asia) (Mega Soft) (Unl)
			{ .ui32Crc = 0x18EC3D59, .pmRegion = LSN_PM_PAL },
			// Creatom (Spain) (Gluk Video) (Unl)
			{ .ui32Crc = 0xA435A17F, .pmRegion = LSN_PM_PAL },
			// Crime Busters (Unknown) (Unl)
			{ .ui32Crc = 0x1A8B558E, .pmRegion = LSN_PM_PAL },
			// Darkman (Europe)
			{ .ui32Crc = 0x6D84EEE3, .pmRegion = LSN_PM_PAL },
			// Darkwing Duck (Europe)
			{ .ui32Crc = 0x895CBAF8, .pmRegion = LSN_PM_PAL },
			// David Crane's A Boy and His Blob - Trouble on Blobolonia (Europe) (Rev A)
			//{ .ui32Crc = 0xAD50E497, .pmRegion = LSN_PM_PAL },	// Is also the USA version.
			// Defender of the Crown (France)
			{ .ui32Crc = 0x2FD2E632, .pmRegion = LSN_PM_DENDY },
			// Defender of the Crown (Europe)
			{ .ui32Crc = 0x68F9B5F5, .pmRegion = LSN_PM_PAL },
			// Die Hard (Europe)
			{ .ui32Crc = 0xE45EC669, .pmRegion = LSN_PM_PAL },
			// Double Dragon (Europe)
			{ .ui32Crc = 0x144CA9E5, .pmRegion = LSN_PM_PAL },			
			// Double Dragon II - The Revenge (Europe)
			{ .ui32Crc = 0x9ED831E7, .pmRegion = LSN_PM_PAL },
			// Double Dragon III - The Sacred Stones (Europe)
			{ .ui32Crc = 0xC7198F2D, .pmRegion = LSN_PM_PAL },
			// Dynablaster (Europe)
			{ .ui32Crc = 0x34BB757B, .pmRegion = LSN_PM_PAL },
			// Eliminator Boat Duel (Europe)
			{ .ui32Crc = 0x5202FD30, .pmRegion = LSN_PM_PAL },
			// Elite (Europe) (En,Fr,De)
			{ .ui32Crc = 0xA4BDCC1D, .pmRegion = LSN_PM_PAL },
			// Faxanadu (Europe)
			{ .ui32Crc = 0x76C161E3, .pmRegion = LSN_PM_PAL },
			// Ferrari Grand Prix Challenge (Europe)
			{ .ui32Crc = 0x8B73FB1B, .pmRegion = LSN_PM_PAL },
			// Four Players' Tennis (Europe)
			{ .ui32Crc = 0xE16F25CC, .pmRegion = LSN_PM_PAL },
			// Guerrilla War (Europe)
			{ .ui32Crc = 0xAECDBE24, .pmRegion = LSN_PM_PAL },
			// Huang Di (Asia) (Unl)
			{ .ui32Crc = 0xBC19F17E, .pmRegion = LSN_PM_PAL },
			// Hudson Hawk (Europe)
			{ .ui32Crc = 0x16F4A933, .pmRegion = LSN_PM_PAL },
			// Ice Hockey (Europe)
			{ .ui32Crc = 0xCCCC1034, .pmRegion = LSN_PM_PAL },
			// Kid Icarus (Europe)
			{ .ui32Crc = 0xD67FD6A6, .pmRegion = LSN_PM_PAL },
			// Legend of Zelda, The (Europe)
			{ .ui32Crc = 0xED7F5555, .pmRegion = LSN_PM_PAL },
			// Lion King, The (Europe)
			{ .ui32Crc = 0x89984244, .pmRegion = LSN_PM_PAL },
			// Little Red Hood - Xiao Hong Mao (Asia) (Unl)
			{ .ui32Crc = 0x166D036B, .pmRegion = LSN_PM_PAL },
			// Maniac Mansion (Europe)
			{ .ui32Crc = 0xF59CFC3D, .pmRegion = LSN_PM_PAL },
			// Mega Man (Europe)
			{ .ui32Crc = 0x94476A70, .pmRegion = LSN_PM_PAL },
			// Metal Fighter (Asia) (Sachen) (Unl)
			{ .ui32Crc = 0x51062125, .pmRegion = LSN_PM_PAL },
			// Metal Gear (Europe)
			{ .ui32Crc = 0x84C4A12E, .pmRegion = LSN_PM_PAL },
			// Metroid (Europe)
			{ .ui32Crc = 0x7751588D, .pmRegion = LSN_PM_PAL },
			// Mind Blower Pak (Australia) (Unl)
			{ .ui32Crc = 0xE7933763, .pmRegion = LSN_PM_PAL },
			// Operation Wolf (Europe)
			{ .ui32Crc = 0x54C34223, .pmRegion = LSN_PM_PAL },
			// Parasol Stars - The Story of Bubble Bobble III (Europe) (Beta)
			{ .ui32Crc = 0x381AAEF6, .pmRegion = LSN_PM_PAL },
			// Policeman (Spain) (Gluk Video) (Unl)
			{ .ui32Crc = 0x65FE1590, .pmRegion = LSN_PM_PAL },
			// Probotector (Europe)
			{ .ui32Crc = 0xB13F00D4, .pmRegion = LSN_PM_PAL },
			// Qi Wang - Chinese Chess (Asia) (Unl)
			{ .ui32Crc = 0x0744648C, .pmRegion = LSN_PM_PAL },
			// San Guo Zhi - Qun Xiong Zheng Ba (Asia) (Unl)
			{ .ui32Crc = 0x81E8992C, .pmRegion = LSN_PM_PAL },
			// Solomon's Key (Europe)
			{ .ui32Crc = 0x3067E376, .pmRegion = LSN_PM_PAL },
			// Super Cartridge Ver 6 - 6 in 1 (Asia) (Unl)
			{ .ui32Crc = 0xA08C46F5, .pmRegion = LSN_PM_PAL },
			// Super Mario Bros. + Duck Hunt (Europe)
			{ .ui32Crc = 0xE8F8F7A5, .pmRegion = LSN_PM_PAL },
			// Total Funpak (Australia) (Unl)
			{ .ui32Crc = 0x853C368D, .pmRegion = LSN_PM_PAL },
			// Venice Beach Volleyball (Asia) (Unl)
			{ .ui32Crc = 0x271FB5A4, .pmRegion = LSN_PM_PAL },
			// Wei Lai Xiao Zi (Asia) (Unl)
			{ .ui32Crc = 0xB242E6B6, .pmRegion = LSN_PM_PAL },
			// Zhi Li Xiao Zhuang Yuan (China) (Unl)
			{ .ui32Crc = 0x346709B4, .pmRegion = LSN_PM_PAL },


			// ========
			// MIRROR
			// ========
			// Deathbots.
			//{ .ui32Crc = 0x8B781D39, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Doraemon.
			{ .ui32Crc = 0xB00ABE1C, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Dragon Warriors III.
			//{ .ui32Crc = 0xA86A5318, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Final Fantasy I, II.
			//{ .ui32Crc = 0xC9556B36, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Ms. Pac Man.
			{ .ui32Crc = 0x4B2DCE64, .mmMirrorOverride = LSN_MM_HORIZONTAL },
			// Perman Part 2.
			//{ .ui32Crc = 0xDBA3A02E, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Zippy Race.
			{ .ui32Crc = 0xE16BB5FE, .mmMirrorOverride = LSN_MM_HORIZONTAL },


			// ========
			// MAPPERS
			// ========
			// Wrecking Crew (JUE)
			{ .ui32Crc = 0x4328B273, .ui16Mapper = 0 },
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