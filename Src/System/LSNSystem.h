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
			m_cCpu( &m_bBus ),
			m_bPaused( false ) {
			ResetState( false );
		}


		// == Functions.
		/**
		 * Resets all of the counters etc. to prepare for running a new emulation from the beginning.
		 * 
		 * \param _bAnalog If true, a soft reset is performed on the CPU, otherwise the CPU is reset to a known state.
		 */
		void											ResetState( bool _bAnalog ) {
			if ( _bAnalog ) {
				m_cCpu.ResetAnalog();
			}
			else {
				m_cCpu.ResetToKnown();
			}
			m_ui64AccumTime = 0;
			m_ui64MasterCounter = 0;
			m_ui64CpuCounter = 0;
			m_ui64PpuCounter = 0;
			m_ui64ApuCounter = 0;
			m_ui64LastRealTime = m_cClock.GetRealTick();
		}

		/**
		 * Performs an update of the system state.  This means getting the amount of time that has passed since this was last called,
		 *	determining how many cycles need to be run for each hardware component, and running all of them.
		 */
		void											Tick() {
			uint64_t ui64CurrealTime = m_cClock.GetRealTick();
			if ( !m_bPaused ) {
				uint64_t ui64Diff = ui64CurrealTime - m_ui64LastRealTime;
				m_ui64AccumTime += ui64Diff;
				m_ui64MasterCounter = m_ui64AccumTime * _tMasterClock / (m_cClock.GetResolution() * _tMasterDiv);


				//bool bFound = true;

#define LSN_CPU_SLOT											0
#define LSN_PPU_SLOT											1
#define LSN_APU_SLOT											2
				struct LSN_HW_SLOTS {
					CTickable *									ptHw;
					CTickable::PfTickFunc						pfTick;
					uint64_t 									ui64Counter;
					const uint64_t								ui64Inc;
				} hsSlots[3] = {
					{ &m_cCpu, &_cCpu::Tick, m_ui64CpuCounter + _tCpuDiv, _tCpuDiv },
					{ &m_cCpu, &_cCpu::Tick, m_ui64PpuCounter + _tPpuDiv, _tPpuDiv },	// Temp.  Just want a function call to be made in all 3 slots so that the below can be coded properly and to test performance of adding function calls.
					{ &m_cCpu, &_cCpu::Tick, m_ui64ApuCounter + _tApuDiv, _tApuDiv },	// Temp.
				};
				size_t stIdx;
				do {
					stIdx = ~size_t( 0 );
					uint64_t ui64Low = ~0ULL;
					// Looping over the 3 slots hax a small amount of overhead.  Unrolling the loop is easy.
					if ( hsSlots[LSN_CPU_SLOT].ui64Counter <= m_ui64MasterCounter && hsSlots[LSN_CPU_SLOT].ui64Counter < ui64Low ) {
						stIdx = LSN_CPU_SLOT;
						ui64Low = hsSlots[LSN_CPU_SLOT].ui64Counter;
					}
					if ( hsSlots[LSN_PPU_SLOT].ui64Counter <= m_ui64MasterCounter && hsSlots[LSN_PPU_SLOT].ui64Counter < ui64Low ) {
						stIdx = LSN_PPU_SLOT;
						ui64Low = hsSlots[LSN_PPU_SLOT].ui64Counter;
					}
					if ( hsSlots[LSN_APU_SLOT].ui64Counter <= m_ui64MasterCounter && hsSlots[LSN_APU_SLOT].ui64Counter < ui64Low ) {
						// If we come in here then we know that the APU will be the one to tick.
						//	This means we can optimize away the "if ( stIdx != ~size_t( 0 ) )" check
						//	as well as the dynamic array accesses ("hsSlots[stIdx]").
						// Testing showed this took the loop down from 0.71834220 cycles-per-tick to
						//	0.68499566 cycles-per-tick.
						hsSlots[LSN_APU_SLOT].ui64Counter += hsSlots[LSN_APU_SLOT].ui64Inc;
						(hsSlots[LSN_APU_SLOT].ptHw->*hsSlots[LSN_APU_SLOT].pfTick)();
						/*stIdx = LSN_APU_SLOT;
						ui64Low = hsSlots[LSN_APU_SLOT].ui64Counter;*/
					}
					else if ( stIdx != ~size_t( 0 ) ) {
						hsSlots[stIdx].ui64Counter += hsSlots[stIdx].ui64Inc;
						(hsSlots[stIdx].ptHw->*hsSlots[stIdx].pfTick)();
					}
					else { break; }
				} while ( true );
				m_ui64CpuCounter = hsSlots[LSN_CPU_SLOT].ui64Counter - _tCpuDiv;
				m_ui64PpuCounter = hsSlots[LSN_PPU_SLOT].ui64Counter - _tPpuDiv;
				m_ui64ApuCounter = hsSlots[LSN_APU_SLOT].ui64Counter - _tApuDiv;

#undef LSN_APU_SLOT
#undef LSN_PPU_SLOT
#undef LSN_CPU_SLOT
			}
			m_ui64LastRealTime = ui64CurrealTime;
		}

		/**
		 * Gets the accumulated real time.
		 *
		 * \return Returns the accumulated real time.
		 */
		inline uint64_t									GetAccumulatedRealTime() const { return m_ui64AccumTime; }

		/**
		 * Gets the master counter.
		 *
		 * \return Returns the master counter.
		 */
		inline uint64_t									GetMasterCounter() const { return m_ui64MasterCounter; }

		/**
		 * Gets the CPU counter.
		 *
		 * \return Returns the CPU counter.
		 */
		inline uint64_t									GetCpuCounter() const { return m_ui64CpuCounter; }

		/**
		 * Gets the PPU counter.
		 *
		 * \return Returns the PPU counter.
		 */
		inline uint64_t									GetPpuCounter() const { return m_ui64PpuCounter; }

		/**
		 * Gets the APU counter.
		 *
		 * \return Returns the APU counter.
		 */
		inline uint64_t									GetApuCounter() const { return m_ui64ApuCounter; }

		/**
		 * Gets the clock resolution.
		 *
		 * \return Returns the clock resolution.
		 */
		inline uint64_t									GetClockResolution() const { return m_cClock.GetResolution(); }

		/**
		 * Gets the clock.
		 *
		 * \return Returns the clock.
		 */
		inline const CClock &							GetClock() const { return m_cClock; }

		/**
		 * Gets the master Hz.
		 *
		 * \return Returns the master Hz.
		 */
		inline constexpr uint64_t						MasterHz() const { return _tMasterClock; }

		/**
		 * Gets the master divider.
		 *
		 * \return Returns the master divider.
		 */
		inline constexpr uint64_t						MasterDiv() const { return _tMasterDiv; }

		/**
		 * Gets the CPU divider.
		 *
		 * \return Returns the CPU divider.
		 */
		inline constexpr uint64_t						CpuDiv() const { return _tCpuDiv; }

		/**
		 * Gets the PPU divider.
		 *
		 * \return Returns the PPU divider.
		 */
		inline constexpr uint64_t						PpuDiv() const { return _tPpuDiv; }


		/**
		 * Gets the APU divider.
		 *
		 * \return Returns the APU divider.
		 */
		inline constexpr uint64_t						ApuDiv() const { return _tApuDiv; }


	protected :
		// == Members.
		CClock											m_cClock;							/**< The master clock. */
		uint64_t										m_ui64AccumTime;					/**< The master accumulated ral time. */
		uint64_t										m_ui64LastRealTime;					/**< The last real time value read from the clock. */
		uint64_t										m_ui64MasterCounter;				/**< Keeps track of how many master virtual cycles have accumulated. */
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