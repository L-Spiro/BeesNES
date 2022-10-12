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
			m_ui64CpuCounter = 0;
			m_ui64PpuCounter = 0;
			m_ui64ApuCounter = 0;
			/*m_ui64CpuTicks = 0;
			m_ui64PpuTicks = 0;
			m_ui64ApuTicks = 0;*/
			m_ui64LastRealTime = m_cClock.GetRealTick();
		}

		/**
		 * Performs an update of the system state.  This means getting the amount of time that has passed since this was last called,
		 *	determining how many cycles need to be run for each hardware component, and running all of them.
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return RETURN
		 */
		void											Tick() {
			uint64_t ui64CurrealTime = m_cClock.GetRealTick();
			if ( !m_bPaused ) {
				uint64_t ui64Diff = ui64CurrealTime - m_ui64LastRealTime;
				m_ui64AccumTime += ui64Diff;
				uint64_t ui64MasterCyclesUntilNow = m_ui64AccumTime * _tMasterClock / (m_cClock.GetResolution() * _tMasterDiv);
				/*uint64_t ui64TotalCpuCyclesToDo = (ui64MasterCyclesUntilNow / _tCpuDiv) - m_ui64CpuTicks;
				uint64_t ui64TotalPpuCyclesToDo = (ui64MasterCyclesUntilNow / _tPpuDiv) - m_ui64PpuTicks;
				uint64_t ui64TotalApuCyclesToDo = (ui64MasterCyclesUntilNow / _tApuDiv) - m_ui64ApuTicks;*/

				uint64_t ui64NextCpuCycleCount = m_ui64CpuCounter + _tCpuDiv;
				uint64_t ui64NextPpuCycleCount = m_ui64PpuCounter + _tPpuDiv;
				uint64_t ui64NextApuCycleCount = m_ui64ApuCounter + _tApuDiv;
//#define LSN_PRINT_CYCLES
#ifdef LSN_PRINT_CYCLES
				char szDebug[1024*8];
				size_t stIdx = 0;
#endif	// LSN_PRINT_CYCLES
				bool bFound = true;
				do {
					char cWhichTicked = '\0';
					uint64_t ui64Low = ~0ULL;
					bFound = false;
					if ( ui64NextCpuCycleCount < ui64MasterCyclesUntilNow && ui64NextCpuCycleCount < ui64Low ) {
						ui64Low = ui64NextCpuCycleCount;
						cWhichTicked = 'C';
						bFound = true;
					}
					if ( ui64NextPpuCycleCount < ui64MasterCyclesUntilNow && ui64NextPpuCycleCount < ui64Low ) {
						ui64Low = ui64NextPpuCycleCount;
						cWhichTicked = 'P';
						bFound = true;
					}
					if ( ui64NextApuCycleCount < ui64MasterCyclesUntilNow && ui64NextApuCycleCount < ui64Low ) {
						ui64Low = ui64NextApuCycleCount;
						cWhichTicked = 'A';
						bFound = true;
					}
					switch ( cWhichTicked ) {
						case 'C' : {
							ui64NextCpuCycleCount += _tCpuDiv;
							break;
						}
						case 'P' : {
							ui64NextPpuCycleCount += _tPpuDiv;
							break;
						}
						case 'A' : {
							ui64NextApuCycleCount += _tApuDiv;
							break;
						}
					}
#ifdef LSN_PRINT_CYCLES
					if ( cWhichTicked != '\0' ) {
						szDebug[stIdx++] = cWhichTicked;
						if ( stIdx == sizeof( szDebug ) - 1 ) {
							szDebug[stIdx] = '\0';
							//std::printf( szDebug );
							::OutputDebugStringA( szDebug );
							stIdx = 0;
						}
					}
#endif	// LSN_PRINT_CYCLES
				}
				while ( bFound );
				m_ui64CpuCounter = ui64NextCpuCycleCount - _tCpuDiv;
				m_ui64PpuCounter = ui64NextPpuCycleCount - _tPpuDiv;
				m_ui64ApuCounter = ui64NextApuCycleCount - _tApuDiv;

#ifdef LSN_PRINT_CYCLES
				szDebug[stIdx] = '\0';
				//std::printf( szDebug );
				::OutputDebugStringA( szDebug );
#endif	// LSN_PRINT_CYCLES
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
		 * Gets the clock resolution.
		 *
		 * \return Returns the clock resolution.
		 */
		inline uint64_t									GetClockResolution() const { return m_cClock.GetResolution(); }


	protected :
		// == Members.
		CClock											m_cClock;							/**< The master clock. */
		uint64_t										m_ui64AccumTime;					/**< The master accumulated ral time. */
		uint64_t										m_ui64LastRealTime;					/**< The last real time value read from the clock. */
		uint64_t										m_ui64CpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the CPU.  Used for sorting. */
		uint64_t										m_ui64PpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the PPU.  Used for sorting. */
		uint64_t										m_ui64ApuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the APU.  Used for sorting. */
		//uint64_t										m_ui64CpuTicks;						/**< Keeps track of how many ticks have accumulated on the CPU.  Used to determine how many times to tick the component during an update. */
		//uint64_t										m_ui64PpuTicks;						/**< Keeps track of how many ticks have accumulated on the PPU.  Used to determine how many times to tick the component during an update. */
		//uint64_t										m_ui64ApuTicks;						/**< Keeps track of how many ticks have accumulated on the APU.  Used to determine how many times to tick the component during an update. */
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
