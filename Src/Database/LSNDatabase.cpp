/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ROM data.
 */


#include "LSNDatabase.h"
#include <string>

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
			// 240pee
			/*{ .ui32Crc = 0x6599113A, .pmRegion = LSN_PM_PAL },
			{ .ui32Crc = 0x2B5A92FE, .pmRegion = LSN_PM_PAL },
			{ .ui32Crc = 0x76D3FCF0, .pmRegion = LSN_PM_PAL },*/
			// Super Mario Bros. (World)
			//{ .ui32Crc = 0x5CF548D3, .pmRegion = LSN_PM_PALM },

			// Super Mario Bros (E)
			//{ .ui32Crc = 0x967A605F, .pmRegion = LSN_PM_PALN },

			// 01.len_ctr
			{ .ui32Crc = 0xD1837AD3, .pmRegion = LSN_PM_PAL },
			// 02.len_table
			{ .ui32Crc = 0x87483778, .pmRegion = LSN_PM_PAL },
			// 03.irq_flag
			{ .ui32Crc = 0x47086DB9, .pmRegion = LSN_PM_PAL },
			// 04.clock_jitter
			{ .ui32Crc = 0x42D39BDC, .pmRegion = LSN_PM_PAL },
			// 05.len_timing_mode0
			{ .ui32Crc = 0x9464BB7A, .pmRegion = LSN_PM_PAL },
			// 06.len_timing_mode1
			{ .ui32Crc = 0x966EA9A4, .pmRegion = LSN_PM_PAL },
			// 07.irq_flag_timing
			{ .ui32Crc = 0xE4E03A3D, .pmRegion = LSN_PM_PAL },
			// 08.irq_timing
			{ .ui32Crc = 0xABF7617D, .pmRegion = LSN_PM_PAL },
			// 10.len_halt_timing
			{ .ui32Crc = 0xED21658A, .pmRegion = LSN_PM_PAL },
			// 11.len_reload_timing
			{ .ui32Crc = 0xA487CD04, .pmRegion = LSN_PM_PAL },
			// 3 in 1 Supergun (Asia) (Unl)
			{ .ui32Crc = 0x789270E0, .pmRegion = LSN_PM_PAL },
			// 3-in-1 (E) [!]
			{ .ui32Crc = 0x7EEF434C, .pmRegion = LSN_PM_PAL },
			// 25th Anniversary Super Mario Bros. (Europe) (Promo, Virtual Console)
			{ .ui32Crc = 0x967A605F, .pmRegion = LSN_PM_PAL },
			// Action 52 (E)
			{ .ui32Crc = 0x795D23EC, .pmRegion = LSN_PM_PAL },
			// Action in New York (Europe)
			{ .ui32Crc = 0x2E1790A4, .pmRegion = LSN_PM_PAL },
			// Addams Family, The - Pugsley's Scavenger Hunt (Europe) (Beta)
			{ .ui32Crc = 0x6A741EC6, .pmRegion = LSN_PM_PAL },
			// Addams Family, The - Pugsley's Scavenger Hunt (Europe)
			{ .ui32Crc = 0x4FD3C549, .pmRegion = LSN_PM_PAL },
			// Addams Family, The (Europe) (En,Fr,De)
			{ .ui32Crc = 0x0FA94D88, .pmRegion = LSN_PM_PAL },
			// Advanced Dungeons & Dragons - Dragons of Flame (Japan)
			{ .ui32Crc = 0x1E0C7EA3, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Advanced Dungeons & Dragons - Heroes of the Lance (Japan)
			{ .ui32Crc = 0x929C7B2F, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Adventures in the Magic Kingdom (Europe)
			{ .ui32Crc = 0x0E3A7F49, .pmRegion = LSN_PM_PAL },
			// Adventure Island Classic (E) [!]
			{ .ui32Crc = 0xBFD53541, .pmRegion = LSN_PM_PAL },
			// Adventures of Bayou Billy, The (Europe)
			{ .ui32Crc = 0xFBFC6A6C, .pmRegion = LSN_PM_PAL },
			// Adventures of Lolo (Europe)
			{ .ui32Crc = 0x8E773E04, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Adventures of Lolo (U)
			{ .ui32Crc = 0xE5682E52, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SEROM },
			// Adventures of Lolo (U) [!]
			{ .ui32Crc = 0x7F7156A7, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SEROM },
			// Adventures of Rad Gravity, The (Europe)
			{ .ui32Crc = 0xE4C1A245, .pmRegion = LSN_PM_PAL },
			// Air Fortress (Europe)
			{ .ui32Crc = 0x991CBDF2, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SJROM },
			// Air Fortress (U)
			{ .ui32Crc = 0xFBAAB554, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SJROM },
			// Airwolf (Europe)
			{ .ui32Crc = 0xF71A9931, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SHROM },
			// Airwolf (U)
			{ .ui32Crc = 0xAA174BC6, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SH1ROM },
			// Al Unser Jr Turbo Racing (U) [a1]
			{ .ui32Crc = 0x54197E45, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Al Unser Jr Turbo Racing (U)
			{ .ui32Crc = 0xE41AE491, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Aladdin (Europe)
			{ .ui32Crc = 0x41D32FD7, .pmRegion = LSN_PM_PAL },
			// Alpha Mission (Europe)
			{ .ui32Crc = 0x3F8FCCF9, .pmRegion = LSN_PM_PAL },
			// America Daitouryou Senkyo (Japan)
			{ .ui32Crc = 0x7831B2FF, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Anticipation (Europe)
			{ .ui32Crc = 0xB391A86D, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Asterix (Europe) (En,Fr,De,Es,It)
			{ .ui32Crc = 0xED77B453, .pmRegion = LSN_PM_PAL },
			// Attack of the Killer Tomatoes (Europe)
			{ .ui32Crc = 0x58EC824F, .pmRegion = LSN_PM_PAL },
			// Aussie Rules Footy (Australia)
			{ .ui32Crc = 0xC004915A, .pmRegion = LSN_PM_PAL },
			// Bad News Baseball (U)
			{ .ui32Crc = 0x607F9765, .pmRegion = LSN_PM_NTSC },
			// Battle Chess (U)
			{ .ui32Crc = 0x2C2DDFB4, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// BarbieBadDudes Vs DragonNinja (E) [!]
			{ .ui32Crc = 0x55B8A213, .pmRegion = LSN_PM_PAL },
			// Balloon Fight (E)
			{ .ui32Crc = 0x5E137C5B, .pmRegion = LSN_PM_PAL },
			// Barbie (Europe)
			{ .ui32Crc = 0x0538A4E9, .pmRegion = LSN_PM_PAL },
			// Barker Bill's Trick Shooting (Europe)
			{ .ui32Crc = 0x2970D05B, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SCROM },
			// Baseball Star - Mezase Sankanou!! (Japan)
			{ .ui32Crc = 0x479ECFB6, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Batman (E)
			{ .ui32Crc = 0x4076E7A6, .pmRegion = LSN_PM_PAL },
			// Battle of Olympus, The (Europe)
			{ .ui32Crc = 0xA97567A4, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Battle of Olympus, The (U)
			{ .ui32Crc = 0x6B53006A, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Battle Stadium - Senbatsu Pro Yakyuu (Japan)
			{ .ui32Crc = 0xF18180CB, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Battle Storm (Japan)
			{ .ui32Crc = 0x88739ADF, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SKROM },
			// Battletoads (Europe)
			{ .ui32Crc = 0x524A5A32, .pmRegion = LSN_PM_PAL },
			// Battletoads-Double Dragon (Europe)
			{ .ui32Crc = 0x23D7D48F, .pmRegion = LSN_PM_PAL },
			// Be-Bop-Highschool - Koukousei Gokuraku Densetsu (Japan)
			{ .ui32Crc = 0x190A3E11, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Beauty and the Beast (E) [!]
			{ .ui32Crc = 0xB42FEEB4, .pmRegion = LSN_PM_PAL },
			// Best Keiba - Derby Stallion (Japan) (Rev A)
			{ .ui32Crc = 0x9C94A0EC, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Best Keiba - Derby Stallion (Japan)
			{ .ui32Crc = 0x41482DA3, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Bigfoot (Europe)
			{ .ui32Crc = 0x629E060B, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SLROM },
			// Bigfoot (U)
			{ .ui32Crc = 0x98BB90D9, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SLROM },
			// Bionic Commando (Europe)
			{ .ui32Crc = 0xFA7EE642, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Blades of Steel (E)
			{ .ui32Crc = 0xC0EDEDD0, .pmRegion = LSN_PM_PAL },
			// Blaster Master (Europe)
			{ .ui32Crc = 0xB40870A2, .pmRegion = LSN_PM_PAL },
			// Blue Shadow (E)
			{ .ui32Crc = 0x075A69E6, .pmRegion = LSN_PM_PAL },
			// Boulder Dash (Europe)
			{ .ui32Crc = 0x54A0C2F0, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Boy and His Blob - Trouble on Blobolonia, A (E)
			//{ .ui32Crc = 0xAD50E497, .pmRegion = LSN_PM_PAL },	// Also the USA version.
			// Bram Stoker's Dracula (E)
			{ .ui32Crc = 0xF28A5B8D, .pmRegion = LSN_PM_PAL },
			// Bubble Bobble (Europe)
			{ .ui32Crc = 0x1F0D03F8, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Bucky O'Hare (E)
			{ .ui32Crc = 0x32C4B857, .pmRegion = LSN_PM_PAL },
			// Bugs Bunny Birthday Blowout, The (E) [!]
			{ .ui32Crc = 0xBE6D7AF7, .pmRegion = LSN_PM_PAL },
			// California Games (E) [!]
			{ .ui32Crc = 0xB400172A, .pmRegion = LSN_PM_PAL },
			// Castelian (E)
			{ .ui32Crc = 0xCF7CA9BD, .pmRegion = LSN_PM_PAL },
			// Castlevania (Europe)
			{ .ui32Crc = 0xA93527E2, .pmRegion = LSN_PM_PAL },
			// Castlevania II - Simon's Quest (Europe)
			{ .ui32Crc = 0x72DDBD39, .pmRegion = LSN_PM_PAL },
			// Castlevania 3 - Dracula's Curse (E)
			{ .ui32Crc = 0x637E366A, .pmRegion = LSN_PM_PAL },
			// Caveman Ninja (E)
			{ .ui32Crc = 0x948AD7F6, .pmRegion = LSN_PM_PAL },
			// Championship Rally (Europe)
			{ .ui32Crc = 0x10B4CE4D, .pmRegion = LSN_PM_PAL },
			// Chessmaster, The (Europe)
			{ .ui32Crc = 0x64C97986, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SJROM },
			// Chessmaster, The (USA)
			{ .ui32Crc = 0x3484AB0C, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SJROM },
			// Chester Field - Ankoku Shin e no Chousen (Japan)
			{ .ui32Crc = 0x6C70A17B, .pmRegion = LSN_PM_NTSC },
			// Chevaliers du Zodiaque, Les - La Legende d'Or (France)
			{ .ui32Crc = 0x98C546E0, .pmRegion = LSN_PM_DENDY },
			// Chip 'n Dale - Rescue Rangers (Europe)
			{ .ui32Crc = 0xAC7A54CC, .pmRegion = LSN_PM_PAL },
			// Chip 'n Dale - Rescue Rangers 2 (Europe)
			{ .ui32Crc = 0xCCE5A91F, .pmRegion = LSN_PM_PAL },
			// Choujin - Ultra Baseball (Japan)
			{ .ui32Crc = 0xBD2269AD, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// City Connection (Europe)
			{ .ui32Crc = 0xBBB3DE0A, .pmRegion = LSN_PM_PAL },
			// Cobra Triangle (E)
			{ .ui32Crc = 0x898E4232, .pmRegion = LSN_PM_PAL },
			// colorwin_pal
			{ .ui32Crc = 0x6348E921, .pmRegion = LSN_PM_PAL },
			// Conflict (Japan)
			{ .ui32Crc = 0x6A10ADD2, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Corvette ZR-1 Challenge (Europe)
			{ .ui32Crc = 0x07637EE4, .pmRegion = LSN_PM_PAL },
			// Cosmic Wars (Japan)
			{ .ui32Crc = 0x9EAB6B1F, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM, .ui16Mapper = 1 },	// Cool music.
			// Cosmos Cop (Asia) (Mega Soft) (Unl)
			{ .ui32Crc = 0x18EC3D59, .pmRegion = LSN_PM_PAL },
			// Crackout (E)
			{ .ui32Crc = 0x81AF4AF9, .pmRegion = LSN_PM_PAL },
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
			{ .ui32Crc = 0x68F9B5F5, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Deja Vu (SW)
			{ .ui32Crc = 0x07259BA7, .pmRegion = LSN_PM_PAL },
			// Demon Sword (U)
			{ .ui32Crc = 0x9DF89BE5, .pmRegion = LSN_PM_NTSC },
			// demo_pal
			{ .ui32Crc = 0x2747EE68, .pmRegion = LSN_PM_PAL },
			// Derby Stallion - Zenkoku Ban (Japan)
			{ .ui32Crc = 0x0B0D4D1B, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SKROM },
			// Devil World (E) [!]
			{ .ui32Crc = 0x60CB6FF6, .pmRegion = LSN_PM_PAL },
			// Die Hard (Europe)
			{ .ui32Crc = 0xE45EC669, .pmRegion = LSN_PM_PAL },
			// Digger T. Rock - The Legend of the Lost City (E)
			{ .ui32Crc = 0x96CFB4D8, .pmRegion = LSN_PM_PAL },
			// Double Dragon (Europe)
			{ .ui32Crc = 0x144CA9E5, .pmRegion = LSN_PM_PAL },
			// Double Dragon II - The Revenge (Europe)
			{ .ui32Crc = 0x9ED831E7, .pmRegion = LSN_PM_PAL },
			// Double Dragon III - The Sacred Stones (Europe)
			{ .ui32Crc = 0xC7198F2D, .pmRegion = LSN_PM_PAL },
			// Dr Mario (E)
			{ .ui32Crc = 0x6A80DE01, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Dragon's Lair (E)
			{ .ui32Crc = 0x32F85838, .pmRegion = LSN_PM_PAL },
			// Drop Zone (E)
			{ .ui32Crc = 0xD21DA4F7, .pmRegion = LSN_PM_PAL },
			// Duck Tales (E)
			{ .ui32Crc = 0xD029F841, .pmRegion = LSN_PM_PAL },			
			// Dungeon & Magic - Swords of Element (Japan)
			{ .ui32Crc = 0xFDA76F70, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Dungeon Magic - Sword of the Elements (USA)
			{ .ui32Crc = 0x4EBDB122, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Dynablaster (Europe)
			{ .ui32Crc = 0x34BB757B, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Eliminator Boat Duel (Europe)
			{ .ui32Crc = 0x5202FD30, .pmRegion = LSN_PM_PAL },
			// Elite (Europe) (En,Fr,De)
			{ .ui32Crc = 0xA4BDCC1D, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Emoyan no 10 Bai Pro Yakyuu (Japan)
			{ .ui32Crc = 0x5BF4DA62, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Excitebike (E)
			{ .ui32Crc = 0x0B5667E9, .pmRegion = LSN_PM_PAL },
			// F-1 Sensation (E) [!]
			{ .ui32Crc = 0xA369AA0F, .pmRegion = LSN_PM_PAL },
			// Famicom Meijin Sen (Japan)
			{ .ui32Crc = 0x740C8108, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Famicom Meijin Sen (Japan) (Rev A)
			{ .ui32Crc = 0x7DD82754, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Famicom Yakyuu Ban (Japan)
			{ .ui32Crc = 0xB6DC9AA7, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Family Feud (U)
			{ .ui32Crc = 0x26F2B268, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SHROM },
			// Faria - Fuuin no Tsurugi (Japan)
			{ .ui32Crc = 0xBF5E2513, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B1H, .pcPcbClass = LSN_PC_SKROM },
			// Faxanadu (Europe)
			{ .ui32Crc = 0x76C161E3, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Ferrari Grand Prix Challenge (Europe)
			{ .ui32Crc = 0x8B73FB1B, .pmRegion = LSN_PM_PAL },
			// Flintstones - The Rescue of Dino & Hoppy, The (E)
			{ .ui32Crc = 0x18C64981, .pmRegion = LSN_PM_PAL },
			// Four Players' Tennis (Europe)
			{ .ui32Crc = 0xE16F25CC, .pmRegion = LSN_PM_PAL },
			// Future Wars - Mirai Senshi Lios (Japan)
			{ .ui32Crc = 0xE94D5181, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Galaxy 5000 (E)
			{ .ui32Crc = 0xF5DFA4A2, .pmRegion = LSN_PM_PAL },
			// Gargoyle's Quest 2 - The Demon Darkness (E)
			{ .ui32Crc = 0x09E4C3E0, .pmRegion = LSN_PM_PAL },
			// Gauntlet 2 (E)
			{ .ui32Crc = 0x3B997543, .pmRegion = LSN_PM_PAL },
			// Ghostbusters (Japan)
			{ .ui32Crc = 0x6E4C0641, .pmRegion = LSN_PM_NTSC },
			// Ghosts'n Goblins (E)
			{ .ui32Crc = 0x9369A2F8, .pmRegion = LSN_PM_PAL },
			// Gilligan's Island (U)
			{ .ui32Crc = 0x55773880, .pmRegion = LSN_PM_NTSC },
			// Ginga Eiyuu Densetsu (Japan)
			{ .ui32Crc = 0x6E32F3C2, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Goal! (E)
			{ .ui32Crc = 0xAB547071, .pmRegion = LSN_PM_PAL },
			// GoalGoal! Two (E)
			{ .ui32Crc = 0xC76FC363, .pmRegion = LSN_PM_PAL },
			// Godzilla - Monster of Monsters! (E)
			{ .ui32Crc = 0xD76A57BF, .pmRegion = LSN_PM_PAL },
			// Golf (E)
			{ .ui32Crc = 0x10BBD4BA, .pmRegion = LSN_PM_PAL },
			// Goonies 2, The (E)
			{ .ui32Crc = 0x8897A8F1, .pmRegion = LSN_PM_PAL },
			// Gradius (E)
			{ .ui32Crc = 0xD52EBAA6, .pmRegion = LSN_PM_PAL },
			// Gremlins 2 - The New Batch (E) [!]
			{ .ui32Crc = 0x0E2FCB2E, .pmRegion = LSN_PM_PAL },
			// Guerrilla War (Europe)
			{ .ui32Crc = 0xAECDBE24, .pmRegion = LSN_PM_PAL },
			// Guerrilla War (U)
			{ .ui32Crc = 0x1554FD9D, .pmRegion = LSN_PM_NTSC },
			// Gun Smoke (E)
			{ .ui32Crc = 0xB79C320D, .pmRegion = LSN_PM_PAL },
			// Gunhed - Aratanaru Tatakai (Japan)
			{ .ui32Crc = 0xE362ECDC, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Gyruss (U)
			{ .ui32Crc = 0x48ECC48A, .pmRegion = LSN_PM_NTSC, .ui16Mapper = 3 },
			// Hammerin' Harry (E) [!]
			{ .ui32Crc = 0x065FF04F, .pmRegion = LSN_PM_PAL },
			// Hook (E)
			{ .ui32Crc = 0x80638505, .pmRegion = LSN_PM_PAL },
			// Huang Di (Asia) (Unl)
			{ .ui32Crc = 0xBC19F17E, .pmRegion = LSN_PM_PAL },
			// Hudson Hawk (Europe)
			{ .ui32Crc = 0x16F4A933, .pmRegion = LSN_PM_PAL },
			// Hudson's Adventure Island 2 (E)
			{ .ui32Crc = 0xAF2BB895, .pmRegion = LSN_PM_PAL },
			// Hunt for Red October, The (E) [!]
			{ .ui32Crc = 0x407B7EEB, .pmRegion = LSN_PM_PAL },
			// Hyakkiyagyou (Japan)
			{ .ui32Crc = 0x1E2F89C8, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Ice Hockey (Europe)
			{ .ui32Crc = 0xCCCC1034, .pmRegion = LSN_PM_PAL },
			// Igo - Kyuu Roban Taikyoku (Japan)
			{ .ui32Crc = 0xB37F48CD, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SIROM },
			// Ikari Warriors (E)
			{ .ui32Crc = 0x538218B2, .pmRegion = LSN_PM_PAL },
			// IkariInternational Cricket (E) [!]
			{ .ui32Crc = 0x2640DE27, .pmRegion = LSN_PM_PAL },
			// International Cricket (Prototype) (E)
			{ .ui32Crc = 0x9DB6A3ED, .pmRegion = LSN_PM_PAL },
			// Ironsword - Wizards & Warriors 2 (E) [!]
			{ .ui32Crc = 0x694C801F, .pmRegion = LSN_PM_PAL },
			// Iron Tank (U)
			{ .ui32Crc = 0x77540BB5, .pmRegion = LSN_PM_NTSC },
			// Isolated Warrior (E)
			{ .ui32Crc = 0xC3AEC9FA, .pmRegion = LSN_PM_PAL },
			// Itadaki Street - Watashi no Omise ni Yottette (Japan)
			{ .ui32Crc = 0x7E5D2F1A, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Jack Nicklaus' Greatest 18 Holes of Champ. Golf (E)
			{ .ui32Crc = 0x836FE2C2, .pmRegion = LSN_PM_PAL },
			// Jackal (U)
			{ .ui32Crc = 0x1D5B03A5, .pmRegion = LSN_PM_NTSC },
			// Jackie Chan's Action Kung Fu (E) [!]
			{ .ui32Crc = 0xAD706A0E, .pmRegion = LSN_PM_PAL },
			// James Bond Jr (E)
			{ .ui32Crc = 0x7E036525, .pmRegion = LSN_PM_PAL },
			// Jimmy Connor's Tennis (E)
			{ .ui32Crc = 0xB80192B7, .pmRegion = LSN_PM_PAL },
			// Jungle Book, The (E) [!]
			{ .ui32Crc = 0xA40E47F0, .pmRegion = LSN_PM_PAL },
			// Kabuki - Quantum Fighter (E)
			{ .ui32Crc = 0x6F860E89, .pmRegion = LSN_PM_PAL },
			// Kid Icarus (Europe)
			{ .ui32Crc = 0xD67FD6A6, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Kick Off (E)
			{ .ui32Crc = 0xD161888B, .pmRegion = LSN_PM_PAL },
			// Kickle Cubicle (E) [!]
			{ .ui32Crc = 0xDF411CF0, .pmRegion = LSN_PM_PAL },
			// Kid Kool (U)
			{ .ui32Crc = 0xAA6BB985, .pmRegion = LSN_PM_NTSC },
			// Kirby's Adventure (E)
			{ .ui32Crc = 0x014A755A, .pmRegion = LSN_PM_PAL },
			// Knight Rider (Europe)
			{ .ui32Crc = 0x329301C1, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SCROM },
			// Konami Hyper Soccer (E)
			{ .ui32Crc = 0xD7B35F7D, .pmRegion = LSN_PM_PAL },
			// Koushien (Japan)
			{ .ui32Crc = 0xA817D175, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Krusty's Fun House (E)
			{ .ui32Crc = 0x278DB9E3, .pmRegion = LSN_PM_PAL },
			// Kujaku Ou II (Japan)
			{ .ui32Crc = 0xA5781280, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },	// Cool music.
			// Legend of Prince Valiant, The (E)
			{ .ui32Crc = 0xFB40D76C, .pmRegion = LSN_PM_PAL },
			// Legend of Zelda, The (Europe)
			{ .ui32Crc = 0xED7F5555, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3 },
			// Legend of Zelda, The (Europe) (Rev A)
			{ .ui32Crc = 0xD44B412E, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3 },
			// Lifeforce (E) [!]
			{ .ui32Crc = 0x7002FE8D, .pmRegion = LSN_PM_PAL },
			// Lion King, The (Europe)
			{ .ui32Crc = 0x89984244, .pmRegion = LSN_PM_PAL },
			// Lipple Island (Japan)
			{ .ui32Crc = 0xA38857EB, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Little Nemo - The Dream Master (E)
			{ .ui32Crc = 0x5A2D1ABF, .pmRegion = LSN_PM_PAL },
			// Little Ninja Brothers (E)
			{ .ui32Crc = 0x4B2CC73E, .pmRegion = LSN_PM_PAL },
			// Little Red Hood - Xiao Hong Mao (Asia) (Unl)
			{ .ui32Crc = 0x166D036B, .pmRegion = LSN_PM_PAL },
			// Low G Man (E)
			{ .ui32Crc = 0x98CCD385, .pmRegion = LSN_PM_PAL },
			// Lunar Pool (E)
			{ .ui32Crc = 0xDEA0D843, .pmRegion = LSN_PM_PAL },
			// M82 Game Selectable Working Product Display (E)
			{ .ui32Crc = 0x7D56840A, .pmRegion = LSN_PM_PAL },
			// Mach Rider (E) [!]
			{ .ui32Crc = 0x8BBE9BEC, .mmMirrorOverride = LSN_MM_VERTICAL, .pmRegion = LSN_PM_PAL },
			// Maniac Mansion (France)
			{ .ui32Crc = 0xF4B70BFE, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Maniac Mansion (Europe)
			{ .ui32Crc = 0xF59CFC3D, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Maniac Mansion (Germany)
			{ .ui32Crc = 0x60EA98A0, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Maniac Mansion (Spain)
			{ .ui32Crc = 0xF5B2AFCA, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Maniac Mansion (SW)
			{ .ui32Crc = 0x3F2BDA65, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Marble Madness (E)
			{ .ui32Crc = 0x51BF28AF, .pmRegion = LSN_PM_PAL },
			// Mario & Yoshi (E) [!]
			{ .ui32Crc = 0x1406783D, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SFROM },
			// Maxi 15 (A)
			{ .ui32Crc = 0x5C0E2F89, .pmRegion = LSN_PM_PAL, .ui16Mapper = 234 },
			// Maxi 15 (U)
			{ .ui32Crc = 0x6467A5C4, .pmRegion = LSN_PM_NTSC, .ui16Mapper = 234 },
			// McDonaldland (E) [!]
			{ .ui32Crc = 0xF5C527B7, .pmRegion = LSN_PM_PAL },
			// Mega Man (Europe)
			{ .ui32Crc = 0x94476A70, .pmRegion = LSN_PM_PAL },
			// Mega Man 2 (E) [!]
			{ .ui32Crc = 0xA6638CBA, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Mega Man 4 (E)
			{ .ui32Crc = 0x1B932BEA, .pmRegion = LSN_PM_PAL },
			// Metal Fighter (Asia) (Sachen) (Unl)
			{ .ui32Crc = 0x51062125, .pmRegion = LSN_PM_PAL },
			// Metal Gear (Europe)
			{ .ui32Crc = 0x84C4A12E, .pmRegion = LSN_PM_PAL },
			// Metroid (Europe)
			{ .ui32Crc = 0x7751588D, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Mickey Mousecapade (USA)
			{ .ui32Crc = 0xF39FD253, .pmRegion = LSN_PM_NTSC },
			// Mike Tyson's Punch-Out!! (E) (PRG 0) [!]
			{ .ui32Crc = 0x4C09E6B8, .pmRegion = LSN_PM_PAL },
			// Mike Tyson's Punch-Out!! (E) (PRG 1) [!]
			{ .ui32Crc = 0xF2A43A8A, .pmRegion = LSN_PM_PAL },
			// Mike Tyson's Punch-Out!! (J)/Mike Tyson's Punch-Out!! (U) (PRG 0)
			{ .ui32Crc = 0x8DB6D11F, .pmRegion = LSN_PM_NTSC },
			// Mike Tyson's Punch-Out!! (U) (PRG 1)
			{ .ui32Crc = 0xF1D861EF, .pmRegion = LSN_PM_NTSC },
			// Mind Blower Pak (Australia) (Unl)
			{ .ui32Crc = 0xE7933763, .pmRegion = LSN_PM_PAL },
			// Miracle Piano Teaching System, The (France)
			{ .ui32Crc = 0x4DA4D863, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SJROM },
			// Miracle Piano Teaching System, The (Germany)
			{ .ui32Crc = 0xC19B77F5, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SJROM },
			// Miracle Piano Teaching System, The (U)
			{ .ui32Crc = 0x3349BA0E, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SJROM },
			// Mission Impossible (E) [!]
			{ .ui32Crc = 0x71BAECEC, .pmRegion = LSN_PM_PAL },
			// Monster in My Pocket (E)
			{ .ui32Crc = 0xC1CD15DD, .pmRegion = LSN_PM_PAL },
			// Moulin Rouge Senki - Melville no Honoo (Japan)
			{ .ui32Crc = 0x005682D5, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Mr. Gimmick (E) [!]
			{ .ui32Crc = 0xDF496FDF, .pmRegion = LSN_PM_PAL },
			// M.U.L.E. (U)
			{ .ui32Crc = 0x0939852F, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Magic of Scheherazade, The (U)
			{ .ui32Crc = 0xA848A2B1, .pmRegion = LSN_PM_NTSC },
			// Muppet Adventure - Chaos at the Carnival (U)
			{ .ui32Crc = 0x7156CB4D, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Musashi no Bouken (Japan)
			{ .ui32Crc = 0xFA4B1D72, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// NFL Football (U)
			{ .ui32Crc = 0x35B6FEBF, .pmRegion = LSN_PM_NTSC },
			// NES Open Tournament Golf (Europe)
			{ .ui32Crc = 0x2D020965, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// nes15-PAL
			{ .ui32Crc = 0x925A2BED, .pmRegion = LSN_PM_PAL },
			// New Ghostbusters 2 (E) [!]
			{ .ui32Crc = 0x480B35D1, .pmRegion = LSN_PM_PAL },
			// New Zealand Story (E)
			{ .ui32Crc = 0x332BACDF, .pmRegion = LSN_PM_PAL },
			// Nintendo World Cup (E) (REV 3)
			{ .ui32Crc = 0x14BCFA21, .pmRegion = LSN_PM_PAL },
			// Nintendo World Cup (E)
			{ .ui32Crc = 0x725B0234, .pmRegion = LSN_PM_PAL },
			// Noah's Ark (E)
			{ .ui32Crc = 0xB99085CE, .pmRegion = LSN_PM_PAL },
			// Nintendo - NTF2 System Cartridge (USA)
			{ .ui32Crc = 0xAF8F7059, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B1H, .pcPcbClass = LSN_PC_SIROM },
			// Obocchama-kun (Japan)
			{ .ui32Crc = 0x09EEFDE3, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Operation Wolf (Europe)
			{ .ui32Crc = 0x54C34223, .pmRegion = LSN_PM_PAL },
			// Over Horizon (E) [!]
			{ .ui32Crc = 0x9D07708D, .pmRegion = LSN_PM_PAL },
			// Pac-Man (Namco) (E) [!]
			{ .ui32Crc = 0x6FA1193B, .pmRegion = LSN_PM_PAL },
			// palphase
			{ .ui32Crc = 0xCB333C4F, .pmRegion = LSN_PM_PAL },
			// Parasol Stars - The Story of Bubble Bobble 3 (E) [!]
			{ .ui32Crc = 0x15382139, .pmRegion = LSN_PM_PAL },
			// Parasol Stars - The Story of Bubble Bobble III (Europe) (Beta)
			{ .ui32Crc = 0x381AAEF6, .pmRegion = LSN_PM_PAL },
			// Parodius (E)
			{ .ui32Crc = 0x5755A36C, .pmRegion = LSN_PM_PAL },
			// Pennant League!! - Home Run Nighter (Japan)
			{ .ui32Crc = 0x52449508, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Pinball (E) [!]
			{ .ui32Crc = 0xB8571339, .pmRegion = LSN_PM_PAL },
			// Pinbot (E)
			{ .ui32Crc = 0xE1CEFA12, .pmRegion = LSN_PM_PAL },
			// Policeman (Spain) (Gluk Video) (Unl)
			{ .ui32Crc = 0x65FE1590, .pmRegion = LSN_PM_PAL },
			// Pro Action Replay (E) [!]
			{ .ui32Crc = 0xD2A530FB, .pmRegion = LSN_PM_PAL },
			// Probotector (Europe)
			{ .ui32Crc = 0xB13F00D4, .pmRegion = LSN_PM_PAL },
			// Probotector 2 - Return of the Evil Forces (E)
			{ .ui32Crc = 0x5BF675BA, .pmRegion = LSN_PM_PAL },
			// Punch-Out!! (E) [!]
			{ .ui32Crc = 0xC9EB73F9, .pmRegion = LSN_PM_PAL },
			// Punch-Out!! (U)
			{ .ui32Crc = 0xC032E5B0, .pmRegion = LSN_PM_NTSC },
			// Rackets & Rivals (E) [!]
			{ .ui32Crc = 0x7F60BF49, .pmRegion = LSN_PM_PAL },
			// Rad Racer (E)
			{ .ui32Crc = 0x654F4E90, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Rainbow Islands - The Story of Bubble Bobble 2 (E) [!]
			{ .ui32Crc = 0xAE71768E, .pmRegion = LSN_PM_PAL },
			// RC Pro-Am (E) (PRG 0)
			{ .ui32Crc = 0xE5A972BE, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SEROM },
			// RC Pro-Am (E) (PRG 1)
			{ .ui32Crc = 0x2DBDDD11, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Reigen Doushi (Japan)
			{ .ui32Crc = 0x0021ED29, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Rescue - The Embassy Mission (E) [!]
			{ .ui32Crc = 0x7EEF2338, .pmRegion = LSN_PM_PAL },
			// Retrocoders - Years behind
			{ .ui32Crc = 0x573DFDFA, .pmRegion = LSN_PM_PAL },
			// RoadBlasters (U)
			{ .ui32Crc = 0x3AB1E983, .pmRegion = LSN_PM_NTSC },
			// Road Fighter (E) [!]
			{ .ui32Crc = 0xF51C7D02, .pmRegion = LSN_PM_PAL },
			// Robin Hood - Prince of Thieves (Germany)
			{ .ui32Crc = 0xD49DCA84, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Robin Hood - Prince of Thieves (Scandinavia)
			{ .ui32Crc = 0xB67D16F6, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Robin Hood - Prince of Thieves (Spain)
			{ .ui32Crc = 0xFD21F54D, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Robin Hood - Prince of Thieves (U)
			{ .ui32Crc = 0x86B0D1CF, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },	// Cool music.
			// Robocop (E) [!]
			{ .ui32Crc = 0x2706B3A1, .pmRegion = LSN_PM_PAL },
			// Rocket Ranger (U) [!]
			{ .ui32Crc = 0x67F77118, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Rocketeer, The (U)
			{ .ui32Crc = 0x1D6DECCC, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SGROM },
			// Rockman 2 - Dr. Wily no Nazo (Japan)
			{ .ui32Crc = 0x6150517C, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Rod Land (E) [!]
			{ .ui32Crc = 0x22AB9694, .pmRegion = LSN_PM_PAL },
			// Rollerball (Australia)
			{ .ui32Crc = 0x954C5ACB, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Romancia (Japan)
			{ .ui32Crc = 0xCF9CF7A2, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SGROM },	// Cool music.  TODO: Bad CHR after intro?
			// Rush'n Attack (E)
			{ .ui32Crc = 0xE0AC6242, .pmRegion = LSN_PM_PAL },
			// Rush'n Attack (U)
			{ .ui32Crc = 0xDE25B90F, .pmRegion = LSN_PM_NTSC },
			// Rygar (E) [!]
			{ .ui32Crc = 0x8F197B0A, .pmRegion = LSN_PM_PAL },
			// Qi Wang - Chinese Chess (Asia) (Unl)
			{ .ui32Crc = 0x0744648C, .pmRegion = LSN_PM_PAL },
			// Salad no Kuni no Tomato Hime (Japan)
			{ .ui32Crc = 0x016C93D8, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// San Guo Zhi - Qun Xiong Zheng Ba (Asia) (Unl)
			{ .ui32Crc = 0x81E8992C, .pmRegion = LSN_PM_PAL },
			// Sanada Juu Yuushi (Japan)
			{ .ui32Crc = 0x3403B1FC, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Satomi Hakkenden (Japan)
			{ .ui32Crc = 0x2B6D2447, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Sensha Senryaku - Sabaku no Kitsune (Japan)
			{ .ui32Crc = 0x93DC3C82, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },
			// Sesame Street ABC (U) [a1]/// Sesame Street ABC (U) [a2]
			{ .ui32Crc = 0xAC38CF63, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Shadowgate (E) [!]
			{ .ui32Crc = 0x0ADB2C4C, .pmRegion = LSN_PM_PAL },
			// Shadowgate (SW)
			{ .ui32Crc = 0x04D6B4F6, .pmRegion = LSN_PM_PAL },
			// Shadowgate (E) [!]
			{ .ui32Crc = 0x8ADE1526, .pmRegion = LSN_PM_PAL },
			// Shadowgate (J)
			{ .ui32Crc = 0x6B403C04, .pmRegion = LSN_PM_NTSC },
			// Shadowgate (U)
			{ .ui32Crc = 0x591364C9, .pmRegion = LSN_PM_NTSC },
			// Shadow Warriors (E)
			{ .ui32Crc = 0xD7679A0E, .pmRegion = LSN_PM_PAL },
			// Shadow Warriors 2 (E) [!]
			{ .ui32Crc = 0x687E25D6, .pmRegion = LSN_PM_PAL },
			// Shatterhand (E) [!]
			{ .ui32Crc = 0x97E2B312, .pmRegion = LSN_PM_PAL },
			// Shikinjou (Japan)
			{ .ui32Crc = 0x9B53F848, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Silent Service (E)
			{ .ui32Crc = 0xFA014BA1, .pmRegion = LSN_PM_PAL },
			// Simpsons - Bart Vs the Space Mutants, The (E) [!]
			{ .ui32Crc = 0xBB2E87AA, .pmRegion = LSN_PM_PAL },
			// Skate or Die! (E) [!]
			{ .ui32Crc = 0x66EBDB64, .pmRegion = LSN_PM_PAL },
			// Sky Kid (U)
			{ .ui32Crc = 0xAD7F9480, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SHROM },
			// Slalom (E)
			{ .ui32Crc = 0x8A65BAFF, .pmRegion = LSN_PM_PAL },
			// Smash TV (E) [!]
			{ .ui32Crc = 0x5460529A, .pmRegion = LSN_PM_PAL },
			// Smurfs, The (E) [!]
			{ .ui32Crc = 0x4022C94E, .pmRegion = LSN_PM_PAL },
			// Snake Rattle'n Roll (E)
			{ .ui32Crc = 0x533F5707, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SEROM },
			// Snake's Revenge (E) [!]
			{ .ui32Crc = 0x27ACEC9E, .pmRegion = LSN_PM_PAL },
			// Soccer (E) [!]
			{ .ui32Crc = 0x80E59BC3, .pmRegion = LSN_PM_PAL },
			// Solar Jetman - Hunt for the Golden Warpship (E)
			{ .ui32Crc = 0x8904149E, .pmRegion = LSN_PM_PAL },
			// Solomon's Key (Europe)
			{ .ui32Crc = 0x3067E376, .pmRegion = LSN_PM_PAL },
			// Solstice (E) [!]
			{ .ui32Crc = 0x7CB0D70D, .pmRegion = LSN_PM_PAL },
			// Space Harrier (Japan)
			{ .ui32Crc = 0x43539A3C, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Space Shuttle Project (U)
			{ .ui32Crc = 0x2220E14A, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Spy Vs Spy (E) [!]
			{ .ui32Crc = 0xDC719391, .pmRegion = LSN_PM_PAL },
			// Star Wars (E)
			{ .ui32Crc = 0x70F4DADB, .pmRegion = LSN_PM_PAL },
			// Startropics (U)
			{ .ui32Crc = 0xBEB88304, .pmRegion = LSN_PM_NTSC },
			// Stealth ATF (E)
			{ .ui32Crc = 0x632EDF4E, .pmRegion = LSN_PM_PAL },
			// Sted - Iseki Wakusei no Yabou (Japan)
			{ .ui32Crc = 0x33159EAC, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Street Gangs (E)
			{ .ui32Crc = 0x30A174AC, .pmRegion = LSN_PM_PAL },
			// Strider (U)
			{ .ui32Crc = 0x02EE3706, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },	// Cool music.
			// Super Cartridge Ver 6 - 6 in 1 (Asia) (Unl)
			{ .ui32Crc = 0xA08C46F5, .pmRegion = LSN_PM_PAL },
			// Super Mario Bros. + Duck Hunt (Europe)
			{ .ui32Crc = 0xE8F8F7A5, .pmRegion = LSN_PM_PAL },
			// Super Mario Bros (E) [a1]
			{ .ui32Crc = 0xC04DBDBC, .pmRegion = LSN_PM_PAL },
			// Super Mario Bros 2 (E) [!]
			{ .ui32Crc = 0x08AF16A0, .pmRegion = LSN_PM_PAL },
			// Super Mario Bros 3 (E)
			{ .ui32Crc = 0x80CD1919, .pmRegion = LSN_PM_PAL },
			// Super Off-Road (E) [!]
			{ .ui32Crc = 0x05104517, .pmRegion = LSN_PM_PAL },
			// Super Spike V'Ball (E) [!]
			{ .ui32Crc = 0xEBB9DF3D, .pmRegion = LSN_PM_PAL },
			// Super Sports Challenge (Aladdin) (E)
			{ .ui32Crc = 0xA045FE1D, .pmRegion = LSN_PM_PAL },
			// Super Turrican (E)
			{ .ui32Crc = 0xDCBA4A78, .pmRegion = LSN_PM_PAL },
			// Sword Master (E) [!]
			{ .ui32Crc = 0x36002ED2, .pmRegion = LSN_PM_PAL },
			// Swords and Serpents (E) [!]
			{ .ui32Crc = 0xD153CAF6, .pmRegion = LSN_PM_PAL },
			// Taiyou no Shinden (Japan)
			{ .ui32Crc = 0x50CCDA33, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// TaleSpin (E) [!]
			{ .ui32Crc = 0x29C15923, .pmRegion = LSN_PM_PAL },
			// Tatakae!! Rahmen Man - Sakuretsu Choujin 102 Gei (Japan)
			{ .ui32Crc = 0x15D53E78, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SKROM },	// TODO: Just produces audio fuzz and a black screen.
			// Tecmo Baseball (U)
			{ .ui32Crc = 0xAD12A34F, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Tecmo World Wrestling (E)
			{ .ui32Crc = 0x790D2916, .pmRegion = LSN_PM_PAL },
			// Teenage Mutant Hero Turtles (E)
			{ .ui32Crc = 0x857DBC36, .pmRegion = LSN_PM_PAL },
			// Teenage Mutant Hero Turtles 2 (E) [!]
			{ .ui32Crc = 0x355DDEE6, .pmRegion = LSN_PM_PAL },
			// Tennis (E) [!]
			{ .ui32Crc = 0x108F732E, .pmRegion = LSN_PM_PAL },
			// Terminator 2 - Judgement Day (E) [!]
			{ .ui32Crc = 0x75DC25A7, .pmRegion = LSN_PM_PAL },
			// Tetris (Europe)
			{ .ui32Crc = 0xD3DEA841, .pmRegion = LSN_PM_PAL, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SEROM },
			// Tetris 2 (E)
			{ .ui32Crc = 0xCD8B279F, .pmRegion = LSN_PM_PAL },
			// Tiger-Heli (E) [!]
			{ .ui32Crc = 0x7925EC62, .pmRegion = LSN_PM_PAL },
			// Time Lord (E)
			{ .ui32Crc = 0x9198279E, .pmRegion = LSN_PM_PAL },														// TODO: Fire Hawk, Mig 29 Soviet Fighter, and Time Lord: These need accurate DMC timing because they abuse APU DMC IRQ to split the screen.
			// Tombs and Treasure (U)
			{ .ui32Crc = 0x50D296B3, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Top Gun - The Second Mission (E)
			{ .ui32Crc = 0xBCEA2BA6, .pmRegion = LSN_PM_PAL },
			// Top Gun (E) [!]
			{ .ui32Crc = 0xD78BFB28, .pmRegion = LSN_PM_PAL },
			// Totally Rad (E) [!]
			{ .ui32Crc = 0xF73A0988, .pmRegion = LSN_PM_PAL },
			// Total Funpak (Australia) (Unl)
			{ .ui32Crc = 0x853C368D, .pmRegion = LSN_PM_PAL },
			// Track & Field 2 (E)
			{ .ui32Crc = 0x8ACAFE51, .pmRegion = LSN_PM_PAL },
			// Track & Field in Barcelona (E)
			{ .ui32Crc = 0x7413E8FF, .pmRegion = LSN_PM_PAL },
			// Trojan (E)
			{ .ui32Crc = 0x4F48B240, .pmRegion = LSN_PM_PAL },
			// Trolls in Crazyland (E)
			{ .ui32Crc = 0xA37B767D, .pmRegion = LSN_PM_PAL },
			// Tsuppari Wars (Japan)
			{ .ui32Crc = 0xD54F5DA9, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SKROM },
			// Turbo Racing (E)
			{ .ui32Crc = 0x75B9C0DB, .pmRegion = LSN_PM_PAL },
			// Ufouria (E)
			{ .ui32Crc = 0x0E0C4221, .pmRegion = LSN_PM_PAL },
			// Win, Lose or Draw (U)
			{ .ui32Crc = 0xF74DFC91, .pmRegion = LSN_PM_NTSC, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// window_old_pal
			{ .ui32Crc = 0xD1806D48, .pmRegion = LSN_PM_PAL },
			// window2_pal
			{ .ui32Crc = 0x9E373CA7, .pmRegion = LSN_PM_PAL },
			// Wizards & Warriors (E)
			{ .ui32Crc = 0x5E6D9975, .pmRegion = LSN_PM_PAL },
			// Wrath of the Black Manta (E) [!]
			{ .ui32Crc = 0xCF8E4CA4, .pmRegion = LSN_PM_PAL },
			// Wrestlemania Challenge (E) [!]
			{ .ui32Crc = 0x138862C5, .pmRegion = LSN_PM_PAL },
			// WWF Steel Cage Challenge (E)
			{ .ui32Crc = 0x449E6557, .pmRegion = LSN_PM_PAL },
			// Venice Beach Volleyball (Asia) (Unl)
			{ .ui32Crc = 0x271FB5A4, .pmRegion = LSN_PM_PAL },
			// Wei Lai Xiao Zi (Asia) (Unl)
			{ .ui32Crc = 0xB242E6B6, .pmRegion = LSN_PM_PAL },
			// Xevious (E)
			{ .ui32Crc = 0xA8104FB2, .pmRegion = LSN_PM_PAL },
			// Zelda 2 - The Adventure of Link (E) (PRG 0)
			{ .ui32Crc = 0xF3F3A491, .pmRegion = LSN_PM_PAL },
			// Zelda 2 - The Adventure of Link (E) (PRG 2)
			{ .ui32Crc = 0xC15A50FC, .pmRegion = LSN_PM_PAL },
			// Zhi Li Xiao Zhuang Yuan (China) (Unl)
			{ .ui32Crc = 0x346709B4, .pmRegion = LSN_PM_PAL },


			// ========
			// MIRROR
			// ========
			// Doraemon.
			{ .ui32Crc = 0xB00ABE1C, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Mach Rider (Japan, USA) (Rev A)
			{ .ui32Crc = 0x3ACD4BF1, .mmMirrorOverride = LSN_MM_VERTICAL },
			// Ms. Pac Man.
			{ .ui32Crc = 0x4B2DCE64, .mmMirrorOverride = LSN_MM_HORIZONTAL },
			// Zippy Race.
			{ .ui32Crc = 0xE16BB5FE, .mmMirrorOverride = LSN_MM_HORIZONTAL },


			// ========
			// MAPPERS
			// ========
			// Deathbots (USA) (Unl)
			{ .ui32Crc = 0x8B781D39, .ui16Mapper = 79 },
			// Don Doko Don 2 (Japan)
			{ .ui32Crc = 0x49C84B4E, .ui16Mapper = 48 },
			// Fantastic Adventures of Dizzy, The (Aladdin) (UE)	// Duplicated as Quattro Adventure (Aladdin) (U)?
			//{ .ui32Crc = 0x6C040686, .ui16Mapper = 71 },
			// Krazy Kreatures (USA) (Unl)
			{ .ui32Crc = 0x1D873633, .ui16Mapper = 79 },
			// Micro Machines (U)
			{ .ui32Crc = 0x6B523BD7, .cChip = LSN_C_BF9093, .ui16Mapper = 71, },
			// Micro Machines (U)
			{ .ui32Crc = 0x9235B57B, .cChip = LSN_C_BF9093, .ui16Mapper = 71, },
			// Micro Machines (Aladdin) (U)
			{ .ui32Crc = 0x24BA12DD, .cChip = LSN_C_BF9093, .ui16Mapper = 71, .ui16SubMapper = 1 },
			// Momotarou Dentetsu (Japan)
			{ .ui32Crc = 0x1027C432, .cChip = LSN_C_UOROM, .ui16Mapper = 2, },
			// Pipe Dream (U)
			{ .ui32Crc = 0xDB70A67C, .pmRegion = LSN_PM_NTSC, .ui16Mapper = 3, },
			// Quattro Adventure (Aladdin) (U)
			{ .ui32Crc = 0x6C040686, .ui16Mapper = 232, .ui16SubMapper = 1, },		// 232: 1 Aladdin Deck Enhancer
			// Quattro Sports (Aladdin) (U)
			{ .ui32Crc = 0x62EF6C79, .ui16Mapper = 232, .ui16SubMapper = 1, },		// 232: 1 Aladdin Deck Enhancer
			// Ultimate Stuntman (U)
			{ .ui32Crc = 0x892434DD, .cChip = LSN_C_BF9093, .ui16Mapper = 71, },
			// Wrecking Crew (JUE)
			{ .ui32Crc = 0x4328B273, .ui16Mapper = 0 },


			// ========
			// CHIPS
			// ========
			// '89 Dennou Kyuusei Uranai (Japan)
			{ .ui32Crc = 0xBA58ED29, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// 720 (U)
			{ .ui32Crc = 0x49F745E0, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// A Ressha de Ikou (Japan)
			{ .ui32Crc = 0x30CA59C8, .pcPcbClass = LSN_PC_SZROM },
			// AD&D Hillsfar (Japan)
			{ .ui32Crc = 0x2C33161D, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// AD&D Hillsfar (U) [!]
			{ .ui32Crc = 0x5DE61639, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Akagawa Jirou no Yuurei Ressha (Japan)
			{ .ui32Crc = 0x9D976153, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// American Dream (Japan)
			{ .ui32Crc = 0xFAF802D1, .cChip = LSN_C_MMC1B2 },
			// Ankoku Shinwa - Yamato Takeru Densetsu (Japan)
			{ .ui32Crc = 0x644E312B, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Anticipation (U)
			{ .ui32Crc = 0x79D8C39D, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SEROM },
			// Aoki Ookami to Shiroki Mejika - Genghis Khan (Japan)
			{ .ui32Crc = 0xFB69743A, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SOROM },

			// Artelius (Japan)
			{ .ui32Crc = 0xEBAC24E9, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SJROM },

			// Bad Street Brawler (USA)
			{ .ui32Crc = 0x1AE7B933, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Baken Hisshou Gaku - Gate In (Japan)
			{ .ui32Crc = 0x27C16011, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Bakushou! Star Monomane Shitennou (Japan)
			{ .ui32Crc = 0x6CD9CC23, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Bard's Tale - Tales of the Unknown, The (U) [!]
			{ .ui32Crc = 0x9B821A83, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Bard's Tale II, The - The Destiny Knight (Japan)
			{ .ui32Crc = 0xC4E1886F, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Barker Bill's Trick Shooting (U)
			{ .ui32Crc = 0xF2641AD0, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SCROM },
			// Bases Loaded (U)
			{ .ui32Crc = 0x778AAF25, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },

			// Bee 52 (U)
			{ .ui32Crc = 0x6C93377C, .cChip = LSN_C_BF9093, },

			// Big Nose Freaks Out (U)
			{ .ui32Crc = 0x5B2B72CB, .cChip = LSN_C_BF9093, },								// TODO: Stops after a bit.
			// Big Nose Freaks Out (Aladdin) (U)
			{ .ui32Crc = 0xCCDCBFC6, .cChip = LSN_C_BF9093, .ui16SubMapper = 1 },			// TODO: Stops after a bit.

			// Big Nose the Caveman (U)
			{ .ui32Crc = 0xBD154C3E, .cChip = LSN_C_BF9093, },

			// Best Play Pro Yakyuu (Japan)
			{ .ui32Crc = 0xF79D684A, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SJROM },
			// Best Play Pro Yakyuu '90 (Japan)
			{ .ui32Crc = 0x974E8840, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Best Play Pro Yakyuu Special (Japan)
			{ .ui32Crc = 0xB8747ABF, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SXROM },
			// Best Play Pro Yakyuu Special (Japan) (Rev A)
			{ .ui32Crc = 0xC3DE7C69, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SXROM },
			// Best Play Pro Yakyuu II (Japan)
			{ .ui32Crc = 0xC2EF3422, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Bikkuriman World - Gekitou Sei Senshi (Japan)
			{ .ui32Crc = 0xC6224026, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Bill & Ted's Excellent Video Game Adventure (U)
			{ .ui32Crc = 0xD65C0697, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SLROM },
			// Bionic Commando (U)
			{ .ui32Crc = 0xD2574720, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Bloody Warriors - Shan-Go no Gyakushuu (Japan)
			{ .ui32Crc = 0x391AA1B8, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Bomberman 2 (U)
			{ .ui32Crc = 0x1EBB5B42, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Boulder Dash (Japan)
			{ .ui32Crc = 0xC6475C2A, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Boulder Dash (U)
			{ .ui32Crc = 0x62DFC064, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Break Time (U)
			{ .ui32Crc = 0x50059012, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Bubble Bobble (USA)
			{ .ui32Crc = 0xB1378C99, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Bugs Bunny Crazy Castle, The (U)
			{ .ui32Crc = 0xB10429AA, .cChip = LSN_C_MMC1B1, .pcPcbClass = LSN_PC_SBROM },
			// Captain ED (Japan)
			{ .ui32Crc = 0x3F56A392, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Casino Derby (Japan)
			{ .ui32Crc = 0xE44001D8, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },

			// Daisenryaku (Japan)
			{ .ui32Crc = 0x53A94738, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SJROM },

			// Dance Aerobics (U)
			{ .ui32Crc = 0xD836A90B, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SBROM },
			// Defender of the Crown (U)
			{ .ui32Crc = 0x28FB71AE, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Destiny of an Emperor (U)
			{ .ui32Crc = 0xA558FB52, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },

			// Dizzy The Adventurer (Aladdin) (U)
			{ .ui32Crc = 0xDB99D0CB, .cChip = LSN_C_BF909X, .ui16SubMapper = 1 },

			// Doraemon - Giga Zombie no Gyakushuu (Japan)
			{ .ui32Crc = 0xD7215873, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Dr. Jekyll and Mr. Hyde (USA)
			{ .ui32Crc = 0x5E33B189, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SFROM },
			// Dr. Mario (Japan, USA)/Dr Mario (JU) [a1]/Dr Mario (JU) [a2]
			{ .ui32Crc = 0xCB02A930, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Dr. Mario (Japan, USA) (Rev A)
			{ .ui32Crc = 0x4D72A3E4, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Dragon Quest III - Soshite Densetsu e... (Japan) (Rev 0A)
			{ .ui32Crc = 0xA49B48B8, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Dragon Quest IV - Michibikareshi Monotachi (Japan)
			{ .ui32Crc = 0x2DD71ACB, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SUROM },
			// Dragon Warrior (U) (PRG 0)
			{ .ui32Crc = 0x5B838CE2, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SAROM },
			// Dragon Warrior (U) (PRG 1)
			{ .ui32Crc = 0xC38B1AAE, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SAROM },
			// Dragon Warrior 2 (U)
			{ .ui32Crc = 0x8C5A784E, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Dragon Warrior 3 (U)
			{ .ui32Crc = 0xA86A5318, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SUROM },
			// Dragon Warrior 4 (U)
			{ .ui32Crc = 0x506E259D, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SUROM },
			// Dungeon Kid (Japan)
			{ .ui32Crc = 0xD68A6F33, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			
			// Fantastic Adventures of Dizzy, The (U)
			{ .ui32Crc = 0x38FBCC85, .cChip = LSN_C_BF9093, },

			// Famicom Top Management (Japan)
			{ .ui32Crc = 0x58507BC9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Faxanadu (Japan)
			{ .ui32Crc = 0xA80FA181, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SGROM },
			// Faxanadu (U)
			{ .ui32Crc = 0x57DD23D1, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Final Fantasy (U) [a1]
			{ .ui32Crc = 0xB8B88130, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Final Fantasy (U)
			{ .ui32Crc = 0xCEBD2A31, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Final Fantasy I, II (Japan)
			{ .ui32Crc = 0xC9556B36, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SXROM },
			// Final Fantasy II (Japan)
			{ .ui32Crc = 0xD29DB3C7, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },

			// Fire Hawk (U)
			{ .ui32Crc = 0x1BC686A8, .cChip = LSN_C_BF9097, },									// TODO: Fire Hawk, Mig 29 Soviet Fighter, and Time Lord: These need accurate DMC timing because they abuse APU DMC IRQ to split the screen.

			// Fox's Peter Pan & the Pirates - The Revenge of Captain Hook (USA)
			{ .ui32Crc = 0x84D51076, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Gambler Jiko Chuushinha - Mahjong Game (Japan)
			{ .ui32Crc = 0x2E2ACAE9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Gambler Jiko Chuushinha 2 (Japan)
			{ .ui32Crc = 0x85F12D37, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Genghis Khan (U)
			{ .ui32Crc = 0x2225C20F, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SOROM },
			// Gimmi a Break - Shijou Saikyou no Quiz Ou Ketteisen (Japan)
			{ .ui32Crc = 0xD9F45BE9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Gimmi a Break - Shijou Saikyou no Quiz Ou Ketteisen 2 (Japan)
			{ .ui32Crc = 0x1545BD13, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SGROM },
			// Greg Norman's Golf Power (U)
			{ .ui32Crc = 0x1352F1B9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Hanjuku Hero (Japan)
			{ .ui32Crc = 0xCD7A2FD7, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Hatris (Japan)
			{ .ui32Crc = 0xB4113F3C, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Hatris (U)
			{ .ui32Crc = 0x841B69B6, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Highway Star (Japan)
			{ .ui32Crc = 0x02589598, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SGROM },
			// Hirake! Ponkikki (Japan)
			{ .ui32Crc = 0x772513F4, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Hissatsu Doujou Yaburi (Japan)
			{ .ui32Crc = 0x565A4681, .cChip = LSN_C_MMC1B1H, .pcPcbClass = LSN_PC_SNROM },
			// Hitler no Fukkatsu - Top Secret (Japan)
			{ .ui32Crc = 0x16A0A3A3, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Hokuto no Ken 3 - Shin Seiki Souzou Seiken Restuden (Japan)
			{ .ui32Crc = 0x771CE357, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Hokuto no Ken 4 - Shichisei Haken Den - Hokuto Shinken no Kanata e (Japan)
			{ .ui32Crc = 0x63469396, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Hototogisu (Japan)
			{ .ui32Crc = 0x9EBDC94E, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Ide Yousuke Meijin no Jissen Mahjong II (Japan)
			{ .ui32Crc = 0x7B44FB2A, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },	// No input?  Bad ROM?
			// Igo Shinan '91 (Japan)
			{ .ui32Crc = 0xBE00966B, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Ikari Warriors 2 - Victory Road (U)
			{ .ui32Crc = 0x4F467410, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Indiana Jones and the Last Crusade (Taito) (U)
			{ .ui32Crc = 0x8BCA5146, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Indora no Hikari (Japan)
			{ .ui32Crc = 0x174F860A, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SNROM },
			// Isaki Shuugorou no Keiba Hisshou Gaku (Japan)
			{ .ui32Crc = 0x18D44BBA, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Jesus - Kyoufu no Bio Monster (Japan)
			{ .ui32Crc = 0x0DC53188, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },

			// Jumbo Ozaki no Hole in One Professional (Japan)
			{ .ui32Crc = 0xFB26FF02, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SJROM },

			// Kaettekita! Gunjin Shougi - Nanya Sore! (Japan)
			{ .ui32Crc = 0xE74AA15A, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Kaguya Hime Densetsu (Japan)
			{ .ui32Crc = 0x26CEC726, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Keroppi to Keroriinu no Splash Bomb! (Japan)
			{ .ui32Crc = 0x0C198D4F, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Kid Icarus (UE)
			{ .ui32Crc = 0xD770C1A9, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Kid Icarus (USA, Europe)
			{ .ui32Crc = 0xD9F0749F, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Kid Niki - Radical Ninja (U) (PRG 0)
			{ .ui32Crc = 0x8192D2E7, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Kid Niki - Radical Ninja (U) (PRG 1) [!]
			{ .ui32Crc = 0xA9415562, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Knight Rider (U)
			{ .ui32Crc = 0xE5F49166, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SC1ROM },
			// Kujaku Ou (Japan)
			{ .ui32Crc = 0x71C9ED1E, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Legend of Zelda, The (USA) (Rev A) (GameCube Edition)
			{ .ui32Crc = 0x46E0D37D, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Legend of Zelda, The (USA)/Legend of Zelda, The (U) (PRG 0)
			{ .ui32Crc = 0x3FE272FB, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Legend of Zelda, The (U) (PRG 1)
			{ .ui32Crc = 0xEAF7ED72, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },

			// Linus Spacehead's Cosmic Crusade (Aladdin) (U)
			{ .ui32Crc = 0x9E379698, .cChip = LSN_C_BF9093, .ui16SubMapper = 1 },			// Cool music.

			// Mahjong Taikai (Japan)
			{ .ui32Crc = 0xF714FAE3, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Maison Ikkoku (Japan)
			{ .ui32Crc = 0x46F30F2D, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Magic Johnson's Fast Break (U)
			{ .ui32Crc = 0xA2F826F1, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SCROM },
			// Magic Johnson's Fast Break (U)
			{ .ui32Crc = 0x3A9DD55C, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SCROM },
			// Mashin Eiyuu Den Wataru Gaiden (Japan)
			{ .ui32Crc = 0xD0CC5EC8, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Maniac Mansion (U)
			{ .ui32Crc = 0x0D9F5BD1, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Mario Open Golf (Japan)
			{ .ui32Crc = 0xBAEBA201, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Matsumoto Tooru no Kabushiki Hisshou Gaku - Vol. 1 (Japan)
			{ .ui32Crc = 0xABAA6F78, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Matsumoto Tooru no Kabushiki Hisshou Gaku - Part II (Japan)
			{ .ui32Crc = 0x1208E754, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Mechanized Attack (U)
			{ .ui32Crc = 0x9EEF47AA, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SCROM },
			// Meitantei Holmes - M kara no Chousenjou (Japan)
			{ .ui32Crc = 0x0A73A792, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// Mega Man 2 (U)
			{ .ui32Crc = 0x0FCFC04D, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Metroid (USA)
			{ .ui32Crc = 0x70080810, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Mezase Pachi Pro - Pachio-kun (Japan)
			{ .ui32Crc = 0x9C3E8FC0, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },

			// Mig-29 Soviet Fighter (U)
			{ .ui32Crc = 0xE62E3382, .cChip = LSN_C_BF9093, },

			// Mizushima Shinji no Daikoushien (Japan)
			{ .ui32Crc = 0x09C1FC7D, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Momotarou Densetsu (Japan)
			{ .ui32Crc = 0xB5576820, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },

			// Money Game, The (Japan)
			{ .ui32Crc = 0x8CE9C87B, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SJROM },	// TODO: Does nothing.

			// Monopoly (Japan)
			{ .ui32Crc = 0x86759C0F, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SGROM },
			// Monster Maker - 7 Tsu no Hihou (Japan)
			{ .ui32Crc = 0xFB1C0551, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Morita Shougi (Japan)
			{ .ui32Crc = 0xEC47296D, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SNROM },
			// Mutant Virus, The (U)
			{ .ui32Crc = 0xA139009C, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SLROM },
			// Navy Blue (Japan)
			{ .ui32Crc = 0x69BCDB8B, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// NES Open Tournament Golf (U)
			{ .ui32Crc = 0xF6B9799C, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// NES Open Tournament Golf (U) [a1]
			{ .ui32Crc = 0x5A4549A9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Ninjara Hoi! (Japan)
			{ .ui32Crc = 0xCEE5857B, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SUROM },
			// Nobunaga no Yabou - Zenkoku Ban (Japan)
			{ .ui32Crc = 0x3F7AD415, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SOROM },
			// Nobunaga no Yabou - Zenkoku Ban (Japan) (Rev A)
			{ .ui32Crc = 0x2B11E0B0, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SOROM },			
			// Nobunaga's Ambition (U)
			{ .ui32Crc = 0x4642DDA6, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SOROM },
			// Orb 3D (U)
			{ .ui32Crc = 0x84B930A9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SCROM },
			// Overlord (U)
			{ .ui32Crc = 0x2856111F, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Palamedes (Japan)
			{ .ui32Crc = 0x35E3EEA2, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Palamedes (U)
			{ .ui32Crc = 0x17C111E0, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Palamedes II - Star Twinkle, Hoshi no Mabataki (Japan)
			{ .ui32Crc = 0x1E9EBB00, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Perfect Bowling (Japan)
			{ .ui32Crc = 0x10327E0E, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Phantom Fighter (U)
			{ .ui32Crc = 0xCC37094C, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// President no Sentaku (Japan)
			{ .ui32Crc = 0x4640EBE0, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Princess Tomato in Salad Kingdom (U)
			{ .ui32Crc = 0x56756615, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SGROM },
			// Pro Yakyuu Satsujin Jiken! (Japan)
			{ .ui32Crc = 0xC9484BB3, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Qix (U)
			{ .ui32Crc = 0x95E4E594, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Rad Racer (U)
			{ .ui32Crc = 0x8B9D3E9C, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SGROM },
			// RC Pro-Am (U) (PRG 1)
			{ .ui32Crc = 0x82CFDE25, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SEROM },			
			// Rollerball (Japan)
			{ .ui32Crc = 0xBF3BB6D5, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SFROM },
			// Rollerball (U)
			{ .ui32Crc = 0xAAAA17BD, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Romance of the Three Kingdoms (U)
			{ .ui32Crc = 0xC6182024, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SOROM },
			// Sangokushi (Japan) (Rev A)
			{ .ui32Crc = 0xCCF35C02, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SOROM },
			// Sesame Street 123 (U)
			{ .ui32Crc = 0x0847C623, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Sesame Street ABC (U)
			{ .ui32Crc = 0x2E6C3CA9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Satsui no Kaisou - Soft House Renzoku Satsujin Jiken (Japan)
			{ .ui32Crc = 0x2858933B, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Shin Satomi Hakken-Den - Hikari to Yami no Tatakai (Japan)
			{ .ui32Crc = 0x23E9C736, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Shingen The Ruler (U)
			{ .ui32Crc = 0xBE3BF3B3, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Shingen The Ruler (U) [a1]
			{ .ui32Crc = 0x195A0585, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Shinsenden (Japan)
			{ .ui32Crc = 0xBB435255, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Shogun (Japan)
			{ .ui32Crc = 0x44B060DA, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Short Order + Egg-Splode! (USA)
			{ .ui32Crc = 0xFD37CA4C, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SBROM },
			// Short Order - Eggsplode (U)
			{ .ui32Crc = 0x96C4CE38, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SBROM },
			// Shoukoushi Ceddie (Japan)
			{ .ui32Crc = 0xC30C9EC9, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Snake Rattle'n Roll (U)
			{ .ui32Crc = 0x406FE900, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Snake Rattle'n Roll (U)
			{ .ui32Crc = 0x8E9C4F74, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Spot (U)
			{ .ui32Crc = 0xCFAE9DFA, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Square no Tom Sawyer (Japan)
			{ .ui32Crc = 0xCB0A76B1, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },

			// Stunt Kids (U)
			{ .ui32Crc = 0x3A990EE0, .cChip = LSN_C_BF9093, },

			// Super Black Onyx (Japan)
			{ .ui32Crc = 0xDFC0CE21, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Super Mario Bros - Duck Hunt - Track Meet (U)
			{ .ui32Crc = 0xD4F018F5, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SFROM },
			// Super Momotarou Dentetsu (Japan)
			{ .ui32Crc = 0x09FFDF45, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Sweet Home (Japan)
			{ .ui32Crc = 0x252FFD12, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Taboo - The Sixth Sense (U)
			{ .ui32Crc = 0x05FE773B, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SEROM },
			// Takeda Shingen 2 (Japan)
			{ .ui32Crc = 0x0E997CF6, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Tamura Koushou Mahjong Seminar (Japan)
			{ .ui32Crc = 0x34DEBDFD, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Tanigawa Kouji no Shougi Shinan II (Japan)
			{ .ui32Crc = 0x3836EEAC, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Tanigawa Kouji no Shougi Shinan III (Japan)
			{ .ui32Crc = 0xE63D9193, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Tenchi o Kurau (Japan)
			{ .ui32Crc = 0x1AC701B5, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Tenchi o Kurau (Japan) (Rev A)
			{ .ui32Crc = 0x637A7ACB, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Tetris (U) [!]
			{ .ui32Crc = 0x943DFBBE, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SEROM },
			// Tetris 2 + Bombliss (Japan)
			{ .ui32Crc = 0x3FF44F87, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Tetris 2 + Bombliss (Japan) (Rev A)
			{ .ui32Crc = 0xFD45E9C1, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Titan (Japan)
			{ .ui32Crc = 0x75901B18, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Top Rider (Japan)
			{ .ui32Crc = 0x20D22251, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SFROM },
			// Touch Down Fever (Japan)
			{ .ui32Crc = 0x96277A43, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SFROM },
			// Touch Down Fever (U)
			{ .ui32Crc = 0x6E6490CD, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Ultima - Exodus (U)
			{ .ui32Crc = 0xA4062017, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Ultima - Exodus (Japan)
			{ .ui32Crc = 0x250F7913, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SNROM },
			// Ultima - Quest of the Avatar (U)
			{ .ui32Crc = 0xA25A750F, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Ultima - Seija e no Michi (Japan)
			{ .ui32Crc = 0x71D8C6E9, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Ultima - Warriors of Destiny (U)
			{ .ui32Crc = 0x4823EEFE, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Vegas Connection - Casino kara Ai o Komete (Japan)
			{ .ui32Crc = 0x57E9B21C, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SNROM },
			// Xenophobe (U)
			{ .ui32Crc = 0x9ECA0941, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SFROM },
			// Yoshi (U)
			{ .ui32Crc = 0xFE5F17F0, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SFROM },
			// Yoshi no Tamago (Japan)
			{ .ui32Crc = 0x8C37A7D5, .cChip = LSN_C_MMC1B2, .pcPcbClass = LSN_PC_SFROM },
			// Ys (Japan)
			{ .ui32Crc = 0x92547F1C, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Zelda no Densetsu 1 - The Hyrule Fantasy (Japan)
			{ .ui32Crc = 0x7AE0BF3C, .cChip = LSN_C_MMC1B3, .pcPcbClass = LSN_PC_SNROM },
			// Zoids 2 - Zenebas no Gyakushuu (Japan)
			{ .ui32Crc = 0x89E085FE, .cChip = LSN_C_MMC1A, .pcPcbClass = LSN_PC_SNROM },
			// Zombie Hunter (Japan)
			{ .ui32Crc = 0xC35E8D34, .cChip = LSN_C_MMC1, .pcPcbClass = LSN_PC_SJROM },



			// ========
			// OVERSIZE
			// ========
			// Galaxian (Japan).
			{ .ui32Crc = 0x084F61CD, .ui32PgmRomSize = 8 * 1024 },
		};
		for ( auto I = LSN_ELEMENTS( eEntries ); I--; ) {
			if ( m_mDatabase.end() != m_mDatabase.find( eEntries[I].ui32Crc ) ) {
				char szBuffer[128];
				std::sprintf( szBuffer, "************ Duplicate Entry in Database: 0x%.8X.\r\n", eEntries[I].ui32Crc );
#ifdef LSN_WINDOWS
				::OutputDebugStringA( szBuffer );
#else
				::fprintf( stderr, "%s", szBuffer );
#endif	// #ifdef LSN_WINDOWS
			}
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
