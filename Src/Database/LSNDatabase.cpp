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
			//{ .ui32Crc = 0x5CF548D3, .pmRegion = LSN_PM_PALN },

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
			// Adventures in the Magic Kingdom (Europe)
			{ .ui32Crc = 0x0E3A7F49, .pmRegion = LSN_PM_PAL },
			// Adventure Island Classic (E) [!]
			{ .ui32Crc = 0xBFD53541, .pmRegion = LSN_PM_PAL },
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
			// Bad News Baseball (U)
			{ .ui32Crc = 0x607F9765, .pmRegion = LSN_PM_NTSC },
			// BarbieBadDudes Vs DragonNinja (E) [!]
			{ .ui32Crc = 0x55B8A213, .pmRegion = LSN_PM_PAL },
			// Balloon Fight (E)
			{ .ui32Crc = 0x5E137C5B, .pmRegion = LSN_PM_PAL },
			// Barbie (Europe)
			{ .ui32Crc = 0x0538A4E9, .pmRegion = LSN_PM_PAL },
			// Barker Bill's Trick Shooting (Europe)
			{ .ui32Crc = 0x2970D05B, .pmRegion = LSN_PM_PAL },
			// Batman (E)
			{ .ui32Crc = 0x4076E7A6, .pmRegion = LSN_PM_PAL },
			// Battle of Olympus, The (Europe)
			{ .ui32Crc = 0xA97567A4, .pmRegion = LSN_PM_PAL },
			// Battletoads (Europe)
			{ .ui32Crc = 0x524A5A32, .pmRegion = LSN_PM_PAL },
			// Battletoads-Double Dragon (Europe)
			{ .ui32Crc = 0x23D7D48F, .pmRegion = LSN_PM_PAL },
			// Beauty and the Beast (E) [!]
			{ .ui32Crc = 0xB42FEEB4, .pmRegion = LSN_PM_PAL },
			// Bigfoot (Europe)
			{ .ui32Crc = 0x629E060B, .pmRegion = LSN_PM_PAL },
			// Bionic Commando (Europe)
			{ .ui32Crc = 0xFA7EE642, .pmRegion = LSN_PM_PAL },
			// Blades of Steel (E)
			{ .ui32Crc = 0xC0EDEDD0, .pmRegion = LSN_PM_PAL },
			// Blaster Master (Europe)
			{ .ui32Crc = 0xB40870A2, .pmRegion = LSN_PM_PAL },
			// Blue Shadow (E)
			{ .ui32Crc = 0x075A69E6, .pmRegion = LSN_PM_PAL },
			// Boulder Dash (Europe)
			{ .ui32Crc = 0x54A0C2F0, .pmRegion = LSN_PM_PAL },
			// Boy and His Blob - Trouble on Blobolonia, A (E)
			//{ .ui32Crc = 0xAD50E497, .pmRegion = LSN_PM_PAL },	// Also the USA version.
			// Bram Stoker's Dracula (E)
			{ .ui32Crc = 0xF28A5B8D, .pmRegion = LSN_PM_PAL },
			// Bubble Bobble (Europe)
			{ .ui32Crc = 0x1F0D03F8, .pmRegion = LSN_PM_PAL },
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
			{ .ui32Crc = 0x64C97986, .pmRegion = LSN_PM_PAL },
			// Chester Field - Ankoku Shin e no Chousen (Japan)
			{ .ui32Crc = 0x6C70A17B, .pmRegion = LSN_PM_NTSC },
			// Chevaliers du Zodiaque, Les - La Legende d'Or (France)
			{ .ui32Crc = 0x98C546E0, .pmRegion = LSN_PM_DENDY },
			// Chip 'n Dale - Rescue Rangers (Europe)
			{ .ui32Crc = 0xAC7A54CC, .pmRegion = LSN_PM_PAL },
			// Chip 'n Dale - Rescue Rangers 2 (Europe)
			{ .ui32Crc = 0xCCE5A91F, .pmRegion = LSN_PM_PAL },
			// City Connection (Europe)
			{ .ui32Crc = 0xBBB3DE0A, .pmRegion = LSN_PM_PAL },
			// Cobra Triangle (E)
			{ .ui32Crc = 0x898E4232, .pmRegion = LSN_PM_PAL },
			// colorwin_pal
			{ .ui32Crc = 0x6348E921, .pmRegion = LSN_PM_PAL },
			// Corvette ZR-1 Challenge (Europe)
			{ .ui32Crc = 0x07637EE4, .pmRegion = LSN_PM_PAL },
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
			{ .ui32Crc = 0x68F9B5F5, .pmRegion = LSN_PM_PAL },
			// Deja Vu (SW)
			{ .ui32Crc = 0x07259BA7, .pmRegion = LSN_PM_PAL },
			// Demon Sword (U)
			{ .ui32Crc = 0x9DF89BE5, .pmRegion = LSN_PM_NTSC },
			// demo_pal
			{ .ui32Crc = 0x2747EE68, .pmRegion = LSN_PM_PAL },
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
			{ .ui32Crc = 0x6A80DE01, .pmRegion = LSN_PM_PAL },
			// Dragon's Lair (E)
			{ .ui32Crc = 0x32F85838, .pmRegion = LSN_PM_PAL },
			// Drop Zone (E)
			{ .ui32Crc = 0xD21DA4F7, .pmRegion = LSN_PM_PAL },
			// Duck Tales (E)
			{ .ui32Crc = 0xD029F841, .pmRegion = LSN_PM_PAL },
			// Dynablaster (Europe)
			{ .ui32Crc = 0x34BB757B, .pmRegion = LSN_PM_PAL },
			// Eliminator Boat Duel (Europe)
			{ .ui32Crc = 0x5202FD30, .pmRegion = LSN_PM_PAL },
			// Elite (Europe) (En,Fr,De)
			{ .ui32Crc = 0xA4BDCC1D, .pmRegion = LSN_PM_PAL },
			// Excitebike (E)
			{ .ui32Crc = 0x0B5667E9, .pmRegion = LSN_PM_PAL },
			// F-1 Sensation (E) [!]
			{ .ui32Crc = 0xA369AA0F, .pmRegion = LSN_PM_PAL },
			// Faxanadu (Europe)
			{ .ui32Crc = 0x76C161E3, .pmRegion = LSN_PM_PAL },
			// Ferrari Grand Prix Challenge (Europe)
			{ .ui32Crc = 0x8B73FB1B, .pmRegion = LSN_PM_PAL },
			// Flintstones - The Rescue of Dino & Hoppy, The (E)
			{ .ui32Crc = 0x18C64981, .pmRegion = LSN_PM_PAL },
			// Four Players' Tennis (Europe)
			{ .ui32Crc = 0xE16F25CC, .pmRegion = LSN_PM_PAL },
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
			// Ice Hockey (Europe)
			{ .ui32Crc = 0xCCCC1034, .pmRegion = LSN_PM_PAL },
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
			// Jack Nicklaus' Greatest 18 Holes of Champ. Golf (E)
			{ .ui32Crc = 0x836FE2C2, .pmRegion = LSN_PM_PAL },
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
			{ .ui32Crc = 0xD67FD6A6, .pmRegion = LSN_PM_PAL },
			// Kick Off (E)
			{ .ui32Crc = 0xD161888B, .pmRegion = LSN_PM_PAL },
			// Kickle Cubicle (E) [!]
			{ .ui32Crc = 0xDF411CF0, .pmRegion = LSN_PM_PAL },
			// Kirby's Adventure (E)
			{ .ui32Crc = 0x014A755A, .pmRegion = LSN_PM_PAL },
			// Konami Hyper Soccer (E)
			{ .ui32Crc = 0xD7B35F7D, .pmRegion = LSN_PM_PAL },
			// Krusty's Fun House (E)
			{ .ui32Crc = 0x278DB9E3, .pmRegion = LSN_PM_PAL },
			// Legend of Prince Valiant, The (E)
			{ .ui32Crc = 0xFB40D76C, .pmRegion = LSN_PM_PAL },
			// Legend of Zelda, The (Europe)
			{ .ui32Crc = 0xED7F5555, .pmRegion = LSN_PM_PAL },
			// Lifeforce (E) [!]
			{ .ui32Crc = 0x7002FE8D, .pmRegion = LSN_PM_PAL },
			// Lion King, The (Europe)
			{ .ui32Crc = 0x89984244, .pmRegion = LSN_PM_PAL },
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
			// Maniac Mansion (Europe)
			{ .ui32Crc = 0xF59CFC3D, .pmRegion = LSN_PM_PAL },
			// Maniac Mansion (SW)
			{ .ui32Crc = 0x3F2BDA65, .pmRegion = LSN_PM_PAL },
			// Marble Madness (E)
			{ .ui32Crc = 0x51BF28AF, .pmRegion = LSN_PM_PAL },
			// Mario & Yoshi (E) [!]
			{ .ui32Crc = 0x1406783D, .pmRegion = LSN_PM_PAL },
			// McDonaldland (E) [!]
			{ .ui32Crc = 0xF5C527B7, .pmRegion = LSN_PM_PAL },
			// Mega Man (Europe)
			{ .ui32Crc = 0x94476A70, .pmRegion = LSN_PM_PAL },
			// Mega Man 2 (E) [!]
			{ .ui32Crc = 0xA6638CBA, .pmRegion = LSN_PM_PAL },
			// Mega Man 4 (E)
			{ .ui32Crc = 0x1B932BEA, .pmRegion = LSN_PM_PAL },
			// Metal Fighter (Asia) (Sachen) (Unl)
			{ .ui32Crc = 0x51062125, .pmRegion = LSN_PM_PAL },
			// Metal Gear (Europe)
			{ .ui32Crc = 0x84C4A12E, .pmRegion = LSN_PM_PAL },
			// Metroid (Europe)
			{ .ui32Crc = 0x7751588D, .pmRegion = LSN_PM_PAL },
			// Mickey Mousecapade (USA)
			{ .ui32Crc = 0xF39FD253, .pmRegion = LSN_PM_NTSC },
			// Mike Tyson's Punch-Out!! (E) (PRG 0) [!]
			{ .ui32Crc = 0x4C09E6B8, .pmRegion = LSN_PM_PAL },
			// Mike Tyson's Punch-Out!! (E) (PRG 1) [!]
			{ .ui32Crc = 0xF2A43A8A, .pmRegion = LSN_PM_PAL },
			// Mind Blower Pak (Australia) (Unl)
			{ .ui32Crc = 0xE7933763, .pmRegion = LSN_PM_PAL },
			// Mission Impossible (E) [!]
			{ .ui32Crc = 0x71BAECEC, .pmRegion = LSN_PM_PAL },
			// Monster in My Pocket (E)
			{ .ui32Crc = 0xC1CD15DD, .pmRegion = LSN_PM_PAL },
			// Mr. Gimmick (E) [!]
			{ .ui32Crc = 0xDF496FDF, .pmRegion = LSN_PM_PAL },
			// M.U.L.E. (U)
			{ .ui32Crc = 0x0939852F, .pmRegion = LSN_PM_NTSC },
			// Muppet Adventure - Chaos at the Carnival (U)
			{ .ui32Crc = 0x7156CB4D, .pmRegion = LSN_PM_NTSC },
			// NFL Football (U)
			{ .ui32Crc = 0x35B6FEBF, .pmRegion = LSN_PM_NTSC },
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
			// Rackets & Rivals (E) [!]
			{ .ui32Crc = 0x7F60BF49, .pmRegion = LSN_PM_PAL },
			// Rad Racer (E)
			{ .ui32Crc = 0x654F4E90, .pmRegion = LSN_PM_PAL },
			// Rainbow Islands - The Story of Bubble Bobble 2 (E) [!]
			{ .ui32Crc = 0xAE71768E, .pmRegion = LSN_PM_PAL },
			// RC Pro-Am (E) (PRG 0)
			{ .ui32Crc = 0xE5A972BE, .pmRegion = LSN_PM_PAL },
			// RC Pro-Am (E) (PRG 1)
			{ .ui32Crc = 0x2DBDDD11, .pmRegion = LSN_PM_PAL },
			// Rescue - The Embassy Mission (E) [!]
			{ .ui32Crc = 0x7EEF2338, .pmRegion = LSN_PM_PAL },
			// Retrocoders - Years behind
			{ .ui32Crc = 0x573DFDFA, .pmRegion = LSN_PM_PAL },
			// RoadBlasters (U)
			{ .ui32Crc = 0x3AB1E983, .pmRegion = LSN_PM_NTSC },
			// Road Fighter (E) [!]
			{ .ui32Crc = 0xF51C7D02, .pmRegion = LSN_PM_PAL },
			// Robocop (E) [!]
			{ .ui32Crc = 0x2706B3A1, .pmRegion = LSN_PM_PAL },
			// Rod Land (E) [!]
			{ .ui32Crc = 0x22AB9694, .pmRegion = LSN_PM_PAL },
			// Rush'n Attack (E)
			{ .ui32Crc = 0xE0AC6242, .pmRegion = LSN_PM_PAL },
			// Rush'n Attack (U)
			{ .ui32Crc = 0xDE25B90F, .pmRegion = LSN_PM_NTSC },
			// Rygar (E) [!]
			{ .ui32Crc = 0x8F197B0A, .pmRegion = LSN_PM_PAL },
			// Qi Wang - Chinese Chess (Asia) (Unl)
			{ .ui32Crc = 0x0744648C, .pmRegion = LSN_PM_PAL },
			// San Guo Zhi - Qun Xiong Zheng Ba (Asia) (Unl)
			{ .ui32Crc = 0x81E8992C, .pmRegion = LSN_PM_PAL },
			// Shadow Warriors (E)
			{ .ui32Crc = 0xD7679A0E, .pmRegion = LSN_PM_PAL },
			// Shadow Warriors 2 (E) [!]
			{ .ui32Crc = 0x687E25D6, .pmRegion = LSN_PM_PAL },
			// Shadowgate (E) [!]
			{ .ui32Crc = 0x0ADB2C4C, .pmRegion = LSN_PM_PAL },
			// Shadowgate (SW)
			{ .ui32Crc = 0x04D6B4F6, .pmRegion = LSN_PM_PAL },
			// Shatterhand (E) [!]
			{ .ui32Crc = 0x97E2B312, .pmRegion = LSN_PM_PAL },
			// Silent Service (E)
			{ .ui32Crc = 0xFA014BA1, .pmRegion = LSN_PM_PAL },
			// Simpsons - Bart Vs the Space Mutants, The (E) [!]
			{ .ui32Crc = 0xBB2E87AA, .pmRegion = LSN_PM_PAL },
			// Skate or Die! (E) [!]
			{ .ui32Crc = 0x66EBDB64, .pmRegion = LSN_PM_PAL },
			// Slalom (E)
			{ .ui32Crc = 0x8A65BAFF, .pmRegion = LSN_PM_PAL },
			// Smash TV (E) [!]
			{ .ui32Crc = 0x5460529A, .pmRegion = LSN_PM_PAL },
			// Smurfs, The (E) [!]
			{ .ui32Crc = 0x4022C94E, .pmRegion = LSN_PM_PAL },
			// Snake Rattle'n Roll (E)
			{ .ui32Crc = 0x533F5707, .pmRegion = LSN_PM_PAL },
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
			// Spy Vs Spy (E) [!]
			{ .ui32Crc = 0xDC719391, .pmRegion = LSN_PM_PAL },
			// Star Wars (E)
			{ .ui32Crc = 0x70F4DADB, .pmRegion = LSN_PM_PAL },
			// Startropics (U)
			{ .ui32Crc = 0xBEB88304, .pmRegion = LSN_PM_NTSC },
			// Stealth ATF (E)
			{ .ui32Crc = 0x632EDF4E, .pmRegion = LSN_PM_PAL },
			// Street Gangs (E)
			{ .ui32Crc = 0x30A174AC, .pmRegion = LSN_PM_PAL },
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
			// TaleSpin (E) [!]
			{ .ui32Crc = 0x29C15923, .pmRegion = LSN_PM_PAL },
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
			// Tetris 2 (E)
			{ .ui32Crc = 0xCD8B279F, .pmRegion = LSN_PM_PAL },
			// Tiger-Heli (E) [!]
			{ .ui32Crc = 0x7925EC62, .pmRegion = LSN_PM_PAL },
			// Time Lord (E)
			{ .ui32Crc = 0x9198279E, .pmRegion = LSN_PM_PAL },
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
			// Turbo Racing (E)
			{ .ui32Crc = 0x75B9C0DB, .pmRegion = LSN_PM_PAL },
			// Ufouria (E)
			{ .ui32Crc = 0x0E0C4221, .pmRegion = LSN_PM_PAL },
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
			// Momotarou Dentetsu (Japan)
			{ .ui32Crc = 0x1027C432, .cChip = LSN_C_UOROM, .ui16Mapper = 2, },
			// Quattro Adventure (Aladdin) (U)
			{ .ui32Crc = 0x6C040686, .ui16Mapper = 232, .ui16SubMapper = 1 },		// 232: 1 Aladdin Deck Enhancer
			// Quattro Sports (Aladdin) (U)
			{ .ui32Crc = 0x62EF6C79, .ui16Mapper = 232, .ui16SubMapper = 1 },		// 232: 1 Aladdin Deck Enhancer
			// Ultimate Stuntman (U)
			{ .ui32Crc = 0x892434DD, .ui16Mapper = 71 },
			// Wrecking Crew (JUE)
			{ .ui32Crc = 0x4328B273, .ui16Mapper = 0 },


			// ========
			// CHIPS
			// ========
			// American Dream (Japan)
			{ .ui32Crc = 0xFAF802D1, .cChip = LSN_C_MMC1B2 },
			// Best Play Pro Yakyuu (Japan)
			{ .ui32Crc = 0xF79D684A, .cChip = LSN_C_MMC1A },
			// Mutant Virus, The (U)
			{ .ui32Crc = 0xA139009C, .cChip = LSN_C_MMC1B3 },


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
