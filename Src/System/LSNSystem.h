/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The "system" is all of the components glued together and run under the master clock.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Cpu/LSNCpu6502.h"
#include "../Time/LSNClock.h"

namespace lsn {

	/**
	 * Class CSystem
	 * \brief The "system" is all of the components glued together and run under the master clock.
	 *
	 * Description: The "system" is all of the components glued together and run under the master clock.
	 */
	template <unsigned _tMasterClock, unsigned _tMasterDiv,
		unsigned _tCpuDiv,
		unsigned _tPpuDiv,
		unsigned _tApuDiv,
		class _cCpu>
	class CSystem {
	public :
		CSystem() :
			m_ui64AccumCycles( 0 ),
			m_ui64CpuCounter( 0 ),
			m_ui64PpuCounter( 0 ),
			m_ui64ApuCounter( 0 ),
			m_cCpu( &m_bBus ),
			m_bPaused( false ) {
			m_ui64LastRealTime = cClock.GetRealTick();
		}


	protected :
		// == Members.
		CClock											cClock;								/**< The master clock. */
		uint64_t										m_ui64AccumCycles;					/**< The master accumulated cycles. */
		uint64_t										m_ui64LastRealTime;					/**< The last real time value read from the clock. */
		uint64_t										m_ui64CpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the CPU.  Used for sorting. */
		uint64_t										m_ui64PpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the PPU.  Used for sorting. */
		uint64_t										m_ui64ApuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the APU.  Used for sorting. */
		CBus											m_bBus;								/**< The bus. */
		_cCpu											m_cCpu;								/**< The CPU. */
		bool											m_bPaused;							/**< Pause flag. */
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/**
	 * An NTSC system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502>																			CNtscSystem;

	/**
	 * A PAL system.
	 */
	typedef CSystem<LSN_CS_PAL_MASTER, LSN_CS_PAL_MASTER_DIVISOR,
		LSN_CS_PAL_CPU_DIVISOR, LSN_CS_PAL_PPU_DIVISOR, LSN_CS_PAL_APU_DIVISOR,
		CCpu6502>																			CPalSystem;

	/**
	 * A Dendy system.
	 */
	typedef CSystem<LSN_CS_DENDY_MASTER, LSN_CS_DENDY_MASTER_DIVISOR,
		LSN_CS_DENDY_CPU_DIVISOR, LSN_CS_DENDY_PPU_DIVISOR, LSN_CS_DENDY_APU_DIVISOR,
		CCpu6502>																			CDendySystem;

	/**
	 * An RGB (2C03) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502>																			CRgb2C03System;

	/**
	 * An RGB (2C04) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502>																			CRgb2C04System;

	/**
	 * An RGB (2C05) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502>																			CRgb2C05System;

}	// namespace lsn
