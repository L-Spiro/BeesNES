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

#include <cassert>
#include <cstdint>
#include <memory>


#ifndef LSN_ELEMENTS
#define LSN_ELEMENTS( x )							((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
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
	};

	/** Clock speeds. */
	enum LSN_CLOCK_SPEEDS : uint64_t {
		LSN_CS_NTSC_MASTER					= 236250000ULL,						/**< The master clock speed (236.25 MHz) without the 11 divisor. */
		LSN_CS_NTSC_MASTER_DIVISOR			= 11ULL,							/**< The master clock speed divisor. 236.25 MHz / 11 = 21477272.727272727272727272727273. */

		LSN_CS_PAL_MASTER					= 53203425ULL,						/**< The master clock speed (26.6017125 MHz * 2). */
		LSN_CS_PAL_MASTER_DIVISOR			= 2ULL,								/**< The master clock speed divisor. 53.203425 MHz / 2 = 26601712.5. */

		LSN_CS_DENDY_MASTER					= 53203425ULL,						/**< The master clock speed (26.6017125 MHz * 2). */
		LSN_CS_DENDY_MASTER_DIVISOR			= 2ULL,								/**< The master clock speed divisor. 53.203425 MHz / 2 = 26601712.5. */

		LSN_CS_NTSC_CPU_DIVISOR				= 12ULL,							/**< 236.25 MHz / 11 / 12 = 1789772.7272727272727272727272727. */
		LSN_CS_PAL_CPU_DIVISOR				= 16ULL,							/**< 53.203425 MHz / 2 / 16 = 1662607.03125. */
		LSN_CS_DENDY_CPU_DIVISOR			= 15ULL,							/**< 53.203425 MHz / 2 / 15 = 1773447.5. */

		LSN_CS_NTSC_PPU_DIVISOR				= 4ULL,								/**< 236.25 MHz / 11 / 4 = 5369318.1818181818181818181818183. */
		LSN_CS_PAL_PPU_DIVISOR				= 5ULL,								/**< 53.203425 MHz / 2 / 5 = 5320342.5. */
		LSN_CS_DENDY_PPU_DIVISOR			= 5ULL,								/**< 53.203425 MHz / 2 / 5 = 5320342.5. */

		/*
		 * To run clocks precisely, floating-point math must be avoided.  All inputs must be accumulated in integers.
		 * The formula to determine how many cycles go into a single tick is:
		 *	ELAPSED_TIME * MASTER / (ELAPSED_TIME_RESOLUTION * MASTER_DIVISOR * HW_DIVISOR).
		 * For example, if the system time resolution is 1000000 (microseconds), and the elapsed time is 2000000 (2 seconds), then the number of NTSC CPU cycles is:
		 *	2000000 * 236250000ULL / (1000000 * 11ULL * 12ULL) = 3579545 cycles.
		 * In PAL, that would be:
		 *	2000000 * 53203425ULL / (1000000 * 2ULL * 16ULL) = 3325214 cycles.
		 * 
		 * NTSC FPS: 60.098813897440515529533511098628.
		 * PAL FPS: 50.006978908188585607940446650124.
		 * Dendy FPS: 50.006978908188585607940446650124.
		 */
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
	inline void								SetBit( uint8_t &_ui8Val, const bool _bOn ) {
		if ( _bOn ) {
			_ui8Val |= _uBit;
		}
		else {
			_ui8Val &= ~_uBit;
		}
	}

	/**
	 * Sets a bit in a flag to 1 or 0.
	 *
	 * \param _uBit Bit mask.
	 * \param _bVal The bit valu to write (0 or 1).
	 * \param _ui8Val The value to modify in-place.
	 * \param _bOn If true, the bit is set, otherwise it is unset.
	 */
	template <unsigned _uBit, bool _bVal>
	inline void								SetBit( uint8_t &_ui8Val ) {
		if constexpr ( _bVal != 0 ) {
			_ui8Val |= _uBit;
		}
		else {
			_ui8Val &= ~_uBit;
		}
	}

	/**
	 * Checks for a bit being set.
	 *
	 * \param _ui8Val The value to check.
	 * \param _ui8Bit The bit to check.
	 * \return Returns true if the bit is set, otherwise false.
	 */
	inline bool								CheckBit( const uint8_t _ui8Val, const uint8_t _ui8Bit ) {
		return _ui8Val & (1 << _ui8Bit) ? true : false;
	}

}	// namespace lsn
