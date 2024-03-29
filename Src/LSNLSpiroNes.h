/**
 * Copyright L. Spiro 2020
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A NES emulator with the goal of being "micro-cycle–accurate." Many emulators claim to be "cycle-accurate"
 *	but are optimized to wait X cycles and then pump out all the work or otherwise do not actually divide all the work across
 *	all the cycles. In contrast, in this emulator, the work is correctly divided across the individual cycles for each
 *	instruction.
 * This means that multi-byte writes are correctly partitioned across cycles and partial data updates are possible, allowing
 *	for the more esoteric features of the system to be accurately emulated.  This means we should be able to support interrupt
 *	hijacking and any other cases that rely heavily on the cycle timing of the system.
 *
 * Additional options/features to facilitate accurate emulation:
 *	Start-Up: Start from known state or from random state.  Helps the random seed in some games.
 *	Hardware bugs will be emulated in both their buggy and fixed states (OAMADDR bugs (writing fewer than 8 ytes on the 2C02G)
 *		are examples of this).
 *	Unofficial opcodes used by games will be supported.
 *	The bus will be open and correctly maintain the last floating read.
 *	Etc.
 *
 * The CPU should be completely cycle-accurate, as every individual cycle is documented there. The same should apply to the
 *	PPU (questions surround PAL differences at the cycle level) and probably only loosely applies to the APU.  In the case of
 *	the APU, while each individual cycle may not be doing exactly the correct work, the goal is for the output to be exactly
 *	correct. This means envelope counters and sweep units etc. get updated within the correct cycles and then perhaps only
 *	minor adjustments to match the output exactly to the real thing.
 *
 * Timing will not be based off audio as is done in many emulators. We will use a real clock (with at minimum microsecond
 *	accuracy) and try to match real timings to real time units, which we can speed up and slow down as options.
 *
 * Performance should be decent enough for real-time. We are adding work to frames by adding a dereference and pointer-
 *	function call for each cycle, but the functions never leave instruction cache. If performance becomes a problem, it may
 *	warrant the creation of the approximate loops used in other emulators, leading to the scenario in which the approximate
 *	loops are used by most for gameplay and the slower original version of the emulator is used as a reference.
 *
 *
 * Other features will include:
 *	A debugger.
 *	A disassembler.
 *	An assembler.
 *	1-877-Tools-4-TAS.
 *		- Stepping and keylogging.
 *		- Movie-making.
 */


#pragma once

#include "OS/LSNOs.h"

#include <cassert>
#include <cstdint>
#include <memory>


#ifndef LSN_ELEMENTS
#define LSN_ELEMENTS( x )					((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof( 0[x] )))))
#endif	// #ifndef LSN_ELEMENTS


namespace lsn {

	// == Enumerations.
	/** Sizes.*/
	enum {
		// Sizes of areas of unique memory regions.
		LSN_INTERNAL_RAM					= 0x800,
		LSN_PPU								= 0x8,
		LSN_APU								= 0x18,
		LSN_APU_IO							= 0x8,
		LSN_CARTRIDGE						= 0xBFE0,

		// Full sizes of memory regions (including repeated sections).
		LSN_CPU_FULL_SIZE					= LSN_INTERNAL_RAM * 4,				/**< RAM + stack. */
		LSN_PPU_FULL_SIZE					= LSN_PPU * 1024,					/**< VROM. */
		LSN_APU_FULL_SIZE					= LSN_APU + LSN_APU_IO,				/**< Audio registers. */
		LSN_ROM_FULL_SIZE					= LSN_CARTRIDGE,					/**< Catridge ROM. */
		LSN_MEM_FULL_SIZE					= 0x10000,							/**< Both normal regions and maximum total extra cart ROM are this number. */

		// Starts of memory regions.
		LSN_CPU_START						= 0x0000,							/**< Start of the CPU memory. */
		LSN_PPU_START						= 0x2000,							/**< Start of the PPU memory. */
		LSN_APU_START						= 0x4000,							/**< Start of the APU memory. */
		LSN_APU_IO_START					= LSN_APU_START + LSN_APU,
		LSN_CART_START						= 0x4020,							/**< Start of the cartridge memory. */

		// Memory mapping.
		LSN_VROM_GRAN						= 0x400,
		LSN_VROM_PAGES						= LSN_PPU_FULL_SIZE / LSN_VROM_GRAN,
		LSN_ROM_GRAN						= 0x2000,
		LSN_ROM_PAGES						= LSN_MEM_FULL_SIZE / LSN_ROM_GRAN,	
		LSN_PACK_RAM_SIZE					= 0x2000,

		// PPU memory.
		LSN_PPU_MEM_FULL_SIZE				= 0x4000,							/**< Full size of the PPU memory. */
		LSN_PPU_PATTERN_TABLES				= 0x0000,							/**< The start of all pattern tables. */
		LSN_PPU_PATTERN_TABLE_SIZE			= 0x1000 * 2,						/**< The size of all pattern tables. */
		LSN_PPU_NAMETABLES					= 0x2000,							/**< The start of all nametables. */
		LSN_PPU_NAMETABLES_SCREEN			= 0x0400,							/**< A single nametable screen. */
		LSN_PPU_NAMETABLES_SIZE				= LSN_PPU_NAMETABLES_SCREEN * 4,	/**< The size of all nametables. */
		LSN_PPU_NAMETABLES_END				= LSN_PPU_NAMETABLES_SCREEN + LSN_PPU_NAMETABLES_SIZE,
		LSN_PPU_PALETTE_MEMORY				= 0x3F00,							/**< Start of the palette memory. */
		LSN_PPU_PALETTE_MEMORY_SIZE			= 0x20,								/**< Size of the palette memory. */
		LSN_PPU_ATTRIBUTE_TABLE_OFFSET		= 0x03C0,							/**< The offset of the attribute data in each nametable. */
	};

	/** Clock speeds. */
	enum LSN_CLOCK_SPEEDS : uint64_t {
		LSN_CS_NTSC_MASTER					= 236250000ULL,						/**< The master clock speed (236.25 MHz) without the 11 divisor. */
		LSN_CS_NTSC_MASTER_DIVISOR			= 11ULL,							/**< The master clock speed divisor. 236.25 MHz / 11 = 21477272.727272727272727272727273. */

		LSN_CS_PAL_MASTER					= 53203425ULL,						/**< The master clock speed (26.6017125 MHz * 2). */
		LSN_CS_PAL_MASTER_DIVISOR			= 2ULL,								/**< The master clock speed divisor. 53.203425 MHz / 2 = 26601712.5. */

		LSN_CS_DENDY_MASTER					= LSN_CS_PAL_MASTER,				/**< The master clock speed (26.6017125 MHz * 2). */
		LSN_CS_DENDY_MASTER_DIVISOR			= LSN_CS_PAL_MASTER_DIVISOR,		/**< The master clock speed divisor. 53.203425 MHz / 2 = 26601712.5. */

		LSN_CS_PALM_MASTER					= 3067875000ULL,					/**< The master clock speed (3.067875 GHz) without the 143 divisor. */
		LSN_CS_PALM_MASTER_DIVISOR			= 143ULL,							/**< The master clock speed divisor. 3.067875 GHz / 143 = 21453671.328671328671328671328671. */

		LSN_CS_PALN_MASTER					= 42984675ULL,						/**< The master clock speed (42.984675 MHz) without the 2 divisor. */
		LSN_CS_PALN_MASTER_DIVISOR			= 2ULL,								/**< The master clock speed divisor. 42.984675 MHz / 2 = 21492337.5. */

		LSN_CS_NTSC_CPU_DIVISOR				= 12ULL,							/**< 236.25 MHz / 11 / 12 = 1789772.7272727272727272727272727. */
		LSN_CS_PAL_CPU_DIVISOR				= 16ULL,							/**< 53.203425 MHz / 2 / 16 = 1662607.03125. */
		LSN_CS_DENDY_CPU_DIVISOR			= 15ULL,							/**< 53.203425 MHz / 2 / 15 = 1773447.5. */
		LSN_CS_PALM_CPU_DIVISOR				= 12ULL,							/**< 3.067875 GHz / 143 / 12 = 1787805.9440559440559440559440559. */
		LSN_CS_PALN_CPU_DIVISOR				= 12ULL,							/**< 42.984675 MHz / 2 / 12 = 1791028.125. */

		LSN_CS_NTSC_PPU_DIVISOR				= 4ULL,								/**< 236.25 MHz / 11 / 4 = 5369318.1818181818181818181818182. */
		LSN_CS_PAL_PPU_DIVISOR				= 5ULL,								/**< 53.203425 MHz / 2 / 5 = 5320342.5. */
		LSN_CS_DENDY_PPU_DIVISOR			= LSN_CS_PAL_PPU_DIVISOR,			/**< 53.203425 MHz / 2 / 5 = 5320342.5. */
		LSN_CS_PALM_PPU_DIVISOR				= LSN_CS_NTSC_PPU_DIVISOR,			/**< 3.067875 GHz / 143 / 4 = 5363417.8321678321678321678321678. */
		LSN_CS_PALN_PPU_DIVISOR				= LSN_CS_NTSC_PPU_DIVISOR,			/**< 42.984675 MHz / 2 / 4 = 5373084.375. */

		LSN_CS_NTSC_APU_DIVISOR				= LSN_CS_NTSC_CPU_DIVISOR,			/**< 236.25 MHz / 11 / 12 = 1789772.7272727272727272727272727. */
		LSN_CS_PAL_APU_DIVISOR				= LSN_CS_PAL_CPU_DIVISOR,			/**< 53.203425 MHz / 2 / 16 = 1662607.03125. */
		LSN_CS_DENDY_APU_DIVISOR			= LSN_CS_DENDY_CPU_DIVISOR,			/**< 53.203425 MHz / 2 / 15 = 1773447.5. */
		LSN_CS_PALM_APU_DIVISOR				= LSN_CS_PALM_CPU_DIVISOR,			/**< 3.067875 GHz / 143 / 12 = 1787805.9440559440559440559440559. */
		LSN_CS_PALN_APU_DIVISOR				= LSN_CS_PALN_CPU_DIVISOR,			/**< 42.984675 MHz / 2 / 12 = 1791028.125. */

		/*
		 * To run clocks precisely, floating-point math must be avoided.  All inputs must be accumulated in integers.
		 * The formula to determine how many cycles go into a single tick is:
		 *	ELAPSED_TIME * MASTER / (ELAPSED_TIME_RESOLUTION * MASTER_DIVISOR * HW_DIVISOR).
		 * For example, if the system time resolution is 1000000 (microseconds), and the elapsed time is 2000000 (2 seconds), then the number of NTSC CPU cycles is:
		 *	2000000 * 236250000ULL / (1000000 * 11ULL * 12ULL) = 3579545 cycles.
		 * In PAL, that would be:
		 *	2000000 * 53203425ULL / (1000000 * 2ULL * 16ULL) = 3325214 cycles.
		 * 
		 * NTSC FPS: 60.098813897440515529533511098629.
		 * PAL FPS: 50.006978908188585607940446650124.
		 * Dendy FPS: 50.006978908188585607940446650124.
		 * RGB (2C03) FPS: 60.098477556112263192919547153838.
		 * RGB (Vs. 4) FPS: 60.098477556112263192919547153838.
		 * RGB (2C05) FPS: 60.098477556112263192919547153838.
		 * PAL-M (Brazil Famiclone) FPS: 60.032435273083568398202053145976.
		 * PAL-N (Argentina Famiclone) FPS: 50.502710495150011279043537108053.
		 * 
		 * True NTSC SMPTE TV FPS:	59.94005994005994005994005994006.
		 *							29.97002997002997002997002997003.
		 *							23.976023976023976023976023976024.
		 *							47.952047952047952047952047952048.
		 */
	};

	/** PPU metrics. */
	enum LSN_PPU_METRICS {
		LSN_PM_NTSC							= 0,								/**< NTSC code. */
		LSN_PM_NTSC_DOTS_X					= 341,								/**< The number of dots horizontally across the screen (the pixel width of the screen including off-screen areas). */
		LSN_PM_NTSC_SCANLINES				= 262,								/**< The number of scanlines (the pixel height of the screen including off-screen areas). */
		LSN_PM_NTSC_PRERENDER				= 0,								/**< The number of pre-render scanlines. */
		LSN_PM_NTSC_RENDER_LINES			= 240,								/**< The number of render scanlines. */
		LSN_PM_NTSC_POSTRENDER_LINES		= 1,								/**< The number of post-render scanlines. */
		LSN_PM_NTSC_VBLANK_LINES			= 20,								/**< The number of v-blank scanlines. */
		LSN_PM_NTSC_POSTBLANK_LINES			= 1,								/**< The number of post-blank scanlines. */
		LSN_PM_NTSC_RENDER_WIDTH			= 256,								/**< The number of visible pixels.  The X resolution of the screen. */
		LSN_PM_NTSC_H_BORDER				= 0,								/**< The number of pixels to the left and right that are blackened out. */

		LSN_PM_PAL							= 1,								/**< PAL code. */
		LSN_PM_PAL_DOTS_X					= 341,								/**< The number of dots horizontally across the screen (the pixel width of the screen including off-screen areas). */
		LSN_PM_PAL_SCANLINES				= 312,								/**< The number of scanlines (the pixel height of the screen including off-screen areas). */
		LSN_PM_PAL_PRERENDER				= 1,								/**< The number of pre-render scanlines. */
		LSN_PM_PAL_RENDER_LINES				= 239,								/**< The number of render scanlines. */
		LSN_PM_PAL_POSTRENDER_LINES			= 1,								/**< The number of post-render scanlines. */
		LSN_PM_PAL_VBLANK_LINES				= 70,								/**< The number of v-blank scanlines. */
		LSN_PM_PAL_POSTBLANK_LINES			= 1,								/**< The number of post-blank scanlines. */
		LSN_PM_PAL_RENDER_WIDTH				= LSN_PM_NTSC_RENDER_WIDTH,			/**< The number of visible pixels.  The X resolution of the screen. */
		LSN_PM_PAL_H_BORDER					= 2,								/**< The number of pixels to the left and right that are blackened out. */

		LSN_PM_DENDY						= 2,								/**< Dendy code. */
		LSN_PM_DENDY_DOTS_X					= LSN_PM_NTSC_DOTS_X,				/**< The number of dots horizontally across the screen (the pixel width of the screen including off-screen areas). */
		LSN_PM_DENDY_SCANLINES				= LSN_PM_PAL_SCANLINES,				/**< The number of scanlines (the pixel height of the screen including off-screen areas). */
		LSN_PM_DENDY_PRERENDER				= LSN_PM_PAL_PRERENDER,				/**< The number of pre-render scanlines. */
		LSN_PM_DENDY_RENDER_LINES			= LSN_PM_PAL_RENDER_LINES,			/**< The number of render scanlines. */
		LSN_PM_DENDY_POSTRENDER_LINES		= 51,								/**< The number of post-render scanlines. */
		LSN_PM_DENDY_VBLANK_LINES			= LSN_PM_NTSC_VBLANK_LINES,			/**< The number of v-blank scanlines. */
		LSN_PM_DENDY_POSTBLANK_LINES		= LSN_PM_NTSC_POSTBLANK_LINES,		/**< The number of post-blank scanlines. */
		LSN_PM_DENDY_RENDER_WIDTH			= LSN_PM_NTSC_RENDER_WIDTH,			/**< The number of visible pixels.  The X resolution of the screen. */
		LSN_PM_DENDY_H_BORDER				= LSN_PM_PAL_H_BORDER,				/**< The number of pixels to the left and right that are blackened out. */

		LSN_PM_PALM							= 3,								/**< PAL-M code. */
		LSN_PM_PALM_DOTS_X					= LSN_PM_NTSC_DOTS_X,				/**< The number of dots horizontally across the screen (the pixel width of the screen including off-screen areas). */
		LSN_PM_PALM_SCANLINES				= LSN_PM_NTSC_SCANLINES,			/**< The number of scanlines (the pixel height of the screen including off-screen areas). */
		LSN_PM_PALM_PRERENDER				= LSN_PM_NTSC_PRERENDER,			/**< The number of pre-render scanlines. */
		LSN_PM_PALM_RENDER_LINES			= LSN_PM_NTSC_RENDER_LINES,			/**< The number of render scanlines. */
		LSN_PM_PALM_POSTRENDER_LINES		= LSN_PM_NTSC_POSTRENDER_LINES,		/**< The number of post-render scanlines. */
		LSN_PM_PALM_VBLANK_LINES			= LSN_PM_NTSC_VBLANK_LINES,			/**< The number of v-blank scanlines. */
		LSN_PM_PALM_POSTBLANK_LINES			= LSN_PM_NTSC_POSTBLANK_LINES,		/**< The number of post-blank scanlines. */
		LSN_PM_PALM_RENDER_WIDTH			= LSN_PM_NTSC_RENDER_WIDTH,			/**< The number of visible pixels.  The X resolution of the screen. */
		LSN_PM_PALM_H_BORDER				= LSN_PM_NTSC_H_BORDER,				/**< The number of pixels to the left and right that are blackened out. */

		LSN_PM_PALN							= 4,								/**< PAL-N code. */
		LSN_PM_PALN_DOTS_X					= LSN_PM_NTSC_DOTS_X,				/**< The number of dots horizontally across the screen (the pixel width of the screen including off-screen areas). */
		LSN_PM_PALN_SCANLINES				= LSN_PM_PAL_SCANLINES,				/**< The number of scanlines (the pixel height of the screen including off-screen areas). */
		LSN_PM_PALN_PRERENDER				= LSN_PM_PAL_PRERENDER,				/**< The number of pre-render scanlines. */
		LSN_PM_PALN_RENDER_LINES			= LSN_PM_PAL_RENDER_LINES,			/**< The number of render scanlines. */
		LSN_PM_PALN_POSTRENDER_LINES		= 51,								/**< The number of post-render scanlines. */
		LSN_PM_PALN_VBLANK_LINES			= LSN_PM_NTSC_VBLANK_LINES,			/**< The number of v-blank scanlines. */
		LSN_PM_PALN_POSTBLANK_LINES			= LSN_PM_NTSC_POSTBLANK_LINES,		/**< The number of post-blank scanlines. */
		LSN_PM_PALN_RENDER_WIDTH			= LSN_PM_NTSC_RENDER_WIDTH,			/**< The number of visible pixels.  The X resolution of the screen. */
		LSN_PM_PALN_H_BORDER				= LSN_PM_PAL_H_BORDER,				/**< The number of pixels to the left and right that are blackened out. */


		LSN_PM_UNKNOWN						= 0xFFE2,							/**< Unknown code. */

		LSN_PM_CONSOLE_TOTAL				= 5,								/**< NTSC, PAL, Dendy, and PAL-M. */
	};

	/** Console types. */
	enum LSN_CONSOLE_TYPES {
		LSN_CT_NES,																/**< Nintendo Entertainment System. */
		LSN_CT_FAMICOM,															/**< Famicom. */
	};

	/** Controller types. */
	enum LSN_CONTROLLER_TYPES {
		LSN_CT_NONE,															/**< No controller attached. */
		LSN_CT_STANDARD,														/**< A standard controller. */
		LSN_CT_ARKANOID,														/**< An Arkanoid controller. */
		LSN_CT_POWER_PAD,														/**< A Power Pad. */
		LSN_CT_SNES,															/**< A Super Nintendo Entertainment System controller. */
		LSN_CT_SNES_MOUSE,														/**< A Super Nintendo Entertainment System mouse. */
		LSN_CT_SUBOR_MOUSE,														/**< A Subor mouse. */
		LSN_CT_ZAPPER,															/**< A Zapper lightgun. */
		LSN_CT_BANDAI_HYPERSHOT,												/**< A Bandai Hypershot. */
		LSN_CT_BARCODE_BATTLER,													/**< A Barcode Battler. */
		LSN_CT_BATTLE_BOX,														/**< A Battle Box. */
		LSN_CT_EXCITING_BOXING_PUNCHING_BAG,									/**< An Exciting Boxing Punching Bag. */
		LSN_CT_FAMILY_BASIC_KEYBOARD,											/**< A Family Basic keyboard. */
		LSN_CT_FAMILY_TRAINER,													/**< A Family Trainer. */
		LSN_CT_FOUR_PLAYER_ADAPTER,												/**< A Four Player adapter. */
		LSN_CT_HORI_TRACK,														/**< A Hori Track. */
		LSN_CT_JISSEN_MAHJONG_CONTROLLER,										/**< A Jissen Mahjng controller. */
		LSN_CT_KONAMI_HYPER_SHOT,												/**< A Konami Hyper Shot controller. */
		LSN_CT_OEKA_KIDS_TABLET,												/**< An Oeka Kids Tablet. */
		LSN_CT_PACHINKO_CONTROLLER,												/**< A Pachinko controller. */
		LSN_CT_PARTYTAP,														/**< A Partytap. */
		LSN_CT_SUBOR_KEYBOARD,													/**< A Subor keyboard. */
		LSN_CT_TURBO_FILE,														/**< A Turbo File. */
	};

	/** PPU registers. */
	enum LSN_PPU_REGISTERS : uint16_t {
		LSN_PR_PPUCTRL						= 0x2000,							/**< The PPUCTRL register. */
		LSN_PR_PPUMASK						= 0x2001,							/**< The PPUMASK register. */
		LSN_PR_PPUSTATUS					= 0x2002,							/**< The PPUSTATUS register. */
		LSN_PR_OAMADDR						= 0x2003,							/**< The OAMADDR register. */
		LSN_PR_OAMDATA						= 0x2004,							/**< The OAMDATA register. */
		LSN_PR_PPUSCROLL					= 0x2005,							/**< The PPUSCROLL register. */
		LSN_PR_PPUADDR						= 0x2006,							/**< The PPUADDR register. */
		LSN_PR_PPUDATA						= 0x2007,							/**< The PPUDATA register. */
	};

	/** Mirroring modes. */
	enum LSN_MIRROR_MODE {
		LSN_MM_VERTICAL,														/**< Vertical mirroring. */
		LSN_MM_HORIZONTAL,														/**< Horizontal mirroring. */
		LSN_MM_4_SCREENS,														/**< 4 screens. */
		LSN_MM_1_SCREEN_A,														/**< 1 screen. */
		LSN_MM_1_SCREEN_B,														/**< 1 screen. */

		LSN_MM_NO_OVERRIDE					= -1,								/**< No mirror override (databse only). */
	};


	// == Functions.
	/**
	 * Sets a bit in a flag to 1 or 0.
	 *
	 * \param _uBit Bit mask.
	 * \param _ui8Val The value to modify in-place.
	 * \param _bOn If true, the bit is set, otherwise it is unset.
	 */
	template <unsigned _uBit>
	inline uint8_t							SetBit( uint8_t &_ui8Val, const bool _bOn ) {
		if ( _bOn ) {
			_ui8Val |= _uBit;
		}
		else {
			_ui8Val &= ~_uBit;
		}
		return _ui8Val;
	}

	/**
	 * Sets a bit in a flag to 1 or 0.
	 *
	 * \param _uBit Bit mask.
	 * \param _bVal The bit value to write (0 or 1).
	 * \param _ui8Val The value to modify in-place.
	 */
	template <unsigned _uBit, bool _bVal>
	inline uint8_t							SetBit( uint8_t &_ui8Val ) {
		if constexpr ( _bVal != 0 ) {
			_ui8Val |= _uBit;
		}
		else {
			_ui8Val &= ~_uBit;
		}
		return _ui8Val;
	}

	/**
	 * Checks for a bit being set.
	 *
	 * \param _ui8Val The value to check.
	 * \param _ui8Bit The bit to check.
	 * \return Returns true if the bit is set, otherwise false.
	 */
	inline bool								CheckBit( const uint8_t _ui8Val, const uint8_t _ui8Bit ) {
		return (_ui8Val & _ui8Bit) ? true : false;
	}

}	// namespace lsn
