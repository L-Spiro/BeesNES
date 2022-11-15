/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The "system" is all of the components glued together and run under the master clock.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Apu/LSNApu2A0X.h"
#include "../Bus/LSNBus.h"
#include "../Cpu/LSNCpu6502.h"
#include "../Ppu/LSNPpu2C0X.h"
#include "../Roms/LSNNesHeader.h"
#include "../Roms/LSNRom.h"
#include "../Time/LSNClock.h"
#include "../Utilities/LSNUtilities.h"

#include <algorithm>
#include <immintrin.h>

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
		class _cCpu,
		class _cPpu,
		class _cApu>
	class CSystem {
	public :
		CSystem() :
			m_cCpu( &m_bBus ),
			m_pPpu( &m_bBus, &m_cCpu ),
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
				m_pPpu.ResetAnalog();
			}
			else {
				m_cCpu.ResetToKnown();
				m_pPpu.ResetToKnown();
			}
			m_ui64AccumTime = 0;
			m_ui64MasterCounter = 0;
			m_ui64CpuCounter = 0;
			m_ui64PpuCounter = 0;
			m_ui64ApuCounter = 0;
			m_ui64LastRealTime = m_cClock.GetRealTick();

			m_cCpu.ApplyMemoryMap();
			m_pPpu.ApplyMemoryMap();
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
				{
					uint64_t ui64Hi;
					uint64_t ui64Low = _umul128( m_ui64AccumTime, _tMasterClock, &ui64Hi );
					m_ui64MasterCounter = _udiv128( ui64Hi, ui64Low, m_cClock.GetResolution() * _tMasterDiv, nullptr );

					//m_ui64MasterCounter = m_ui64AccumTime * _tMasterClock / (m_cClock.GetResolution() * _tMasterDiv);
				}


#define LSN_CPU_SLOT											0
#define LSN_PPU_SLOT											1
#define LSN_APU_SLOT											2
				struct LSN_HW_SLOTS {
					CTickable *									ptHw;
					//CTickable::PfTickFunc						pfTick;
					uint64_t 									ui64Counter;
					const uint64_t								ui64Inc;
				} hsSlots[3] = {
					{ &m_cCpu, m_ui64CpuCounter + _tCpuDiv, _tCpuDiv },
					{ &m_pPpu, m_ui64PpuCounter + _tPpuDiv, _tPpuDiv },
					{ &m_pApu, m_ui64ApuCounter + _tApuDiv, _tApuDiv },
				};
				LSN_HW_SLOTS * phsSlot = nullptr;
				do {
					phsSlot = nullptr;
					uint64_t ui64Low = ~0ULL;
					// Looping over the 3 slots hax a small amount of overhead.  Unrolling the loop is easy.
					if ( hsSlots[LSN_CPU_SLOT].ui64Counter <= m_ui64MasterCounter && hsSlots[LSN_CPU_SLOT].ui64Counter < ui64Low ) {
						phsSlot = &hsSlots[LSN_CPU_SLOT];
						ui64Low = phsSlot->ui64Counter;
					}
					if ( hsSlots[LSN_PPU_SLOT].ui64Counter <= m_ui64MasterCounter && hsSlots[LSN_PPU_SLOT].ui64Counter < ui64Low ) {
						phsSlot = &hsSlots[LSN_PPU_SLOT];
						ui64Low = phsSlot->ui64Counter;
					}
					if ( hsSlots[LSN_APU_SLOT].ui64Counter <= m_ui64MasterCounter && hsSlots[LSN_APU_SLOT].ui64Counter < ui64Low ) {
						// If we come in here then we know that the APU will be the one to tick.
						//	This means we can optimize away the "if ( phsSlot != nullptr )" check
						//	as well as the pointer-access ("phsSlot").
						// Testing showed this took the loop down from 0.71834220 cycles-per-tick to
						//	0.68499566 cycles-per-tick.
						// Switching to function pointers inside the CPU Tick() function brought it
						//	down to 0.63103939.
						hsSlots[LSN_APU_SLOT].ui64Counter += hsSlots[LSN_APU_SLOT].ui64Inc;
						//(hsSlots[LSN_APU_SLOT].ptHw->*hsSlots[LSN_APU_SLOT].pfTick)();
						hsSlots[LSN_APU_SLOT].ptHw->Tick();
					}
					else if ( phsSlot != nullptr ) {
						phsSlot->ui64Counter += phsSlot->ui64Inc;
						//(phsSlot->ptHw->*phsSlot->pfTick)();
						phsSlot->ptHw->Tick();
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

		/**
		 * Loads a ROM image.
		 *
		 * \param _vRom The ROM image to load.
		 * \param _s16Path The ROM file path.
		 * \return Returns true if the image was loaded, false otherwise.
		 */
		bool											LoadRom( const std::vector<uint8_t> &_vRom, const std::u16string &_s16Path ) {
			m_rRom = LSN_ROM();
			m_rRom.riInfo.s16File = _s16Path;
			m_rRom.riInfo.s16RomName = CUtilities::GetFileName( _s16Path );

			if ( _vRom.size() >= 4 ) {
				const uint8_t ui8NesHeader[] = {
					0x4E, 0x45, 0x53, 0x1A
				};
				if ( std::memcmp( _vRom.data(), ui8NesHeader, sizeof( ui8NesHeader ) ) == 0 ) {
					// .NES.
					if ( !LoadNes( _vRom ) ) { return false; }
					//return true;
				}
				else {
					return false;
				}
				// ROM loaded.
				m_bBus.CopyToMemory( m_rRom.vPrgRom.data(), uint32_t( m_rRom.vPrgRom.size() ), uint16_t( LSN_MEM_FULL_SIZE - m_rRom.vPrgRom.size() ) );
				if ( m_rRom.vChrRom.size() ) {
					m_pPpu.GetPpuBus().CopyToMemory( m_rRom.vChrRom.data(), std::max<uint32_t>( uint32_t( m_rRom.vChrRom.size() ), 0x2000 ), 0 );
				}
				return true;
			}

			m_rRom = LSN_ROM();
			return false;
		}

		/**
		 * Determnines is a ROM is loaded or not.
		 *
		 * \return Returns true if a ROM is loaded.
		 */
		bool											IsRomLoaded() const {
			return !!m_rRom.vPrgRom.size();
		}

		/**
		 * Gets the PPU.
		 *
		 * \return Returns the PPU.
		 */
		inline const _cPpu &							GetPpu() const { return m_pPpu; }

		/**
		 * Gets the PPU.
		 *
		 * \return Returns the PPU.
		 */
		inline _cPpu &									GetPpu() { return m_pPpu; }


	protected :
		// == Members.
		CClock											m_cClock;							/**< The master clock. */
		uint64_t										m_ui64AccumTime;					/**< The master accumulated real time. */
		uint64_t										m_ui64LastRealTime;					/**< The last real time value read from the clock. */
		uint64_t										m_ui64MasterCounter;				/**< Keeps track of how many master virtual cycles have accumulated. */
		uint64_t										m_ui64CpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the CPU.  Used for sorting. */
		uint64_t										m_ui64PpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the PPU.  Used for sorting. */
		uint64_t										m_ui64ApuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the APU.  Used for sorting. */
		CCpuBus											m_bBus;								/**< The bus. */
		_cCpu											m_cCpu;								/**< The CPU. */
		_cPpu											m_pPpu;								/**< The PPU. */
		_cApu											m_pApu;								/**< The APU. */
		LSN_ROM											m_rRom;								/**< The current cartridge. */
		bool											m_bPaused;							/**< Pause flag. */


		// == Functions.
		/**
		 * Loads a ROM image in .NES format.
		 *
		 * \param _vRom The ROM image to load.
		 * \return Returns true if the image was loaded, false otherwise.
		 */
		bool											LoadNes( const std::vector<uint8_t> &_vRom ) {
			if ( _vRom.size() >= sizeof( LSN_NES_HEADER ) ) {
				size_t stDataSize = _vRom.size() - sizeof( LSN_NES_HEADER );
				const uint8_t * pui8Data = _vRom.data() + sizeof( LSN_NES_HEADER );
				const LSN_NES_HEADER * pnhHeader = reinterpret_cast<const LSN_NES_HEADER *>(_vRom.data());
				m_rRom.i32ChrRamSize = pnhHeader->GetChrRomSize();
				m_rRom.i32SaveChrRamSize = pnhHeader->GetSaveChrRamSize();
				m_rRom.i32WorkRamSize = pnhHeader->GetWorkRamSize();
				m_rRom.i32SaveRamSize = pnhHeader->GetSaveRamSize();

				if ( pnhHeader->HasTrainer() ) {
					if ( stDataSize < 512 ) { return false; }
					// Load trainer.
					stDataSize -= 512;
					pui8Data += 512;
				}

				uint32_t ui32PrgSize = pnhHeader->GetPgmRomSize();
				uint32_t ui32ChrSize = pnhHeader->GetChrRomSize();
				if ( size_t( ui32PrgSize ) + size_t( ui32ChrSize ) > stDataSize ) { return false; }
				m_rRom.vPrgRom.insert( m_rRom.vPrgRom.end(), pui8Data, pui8Data + ui32PrgSize );
				pui8Data += ui32PrgSize;
				m_rRom.vChrRom.insert( m_rRom.vChrRom.end(), pui8Data, pui8Data + ui32ChrSize );

				return true;
			}
			return false;
		}
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
		CCpu6502, CNtscPpu, CApu2A0X>														CNtscSystem;

	/**
	 * A PAL system.
	 */
	typedef CSystem<LSN_CS_PAL_MASTER, LSN_CS_PAL_MASTER_DIVISOR,
		LSN_CS_PAL_CPU_DIVISOR, LSN_CS_PAL_PPU_DIVISOR, LSN_CS_PAL_APU_DIVISOR,
		CCpu6502, CPalPpu, CApu2A0X>														CPalSystem;

	/**
	 * A Dendy system.
	 */
	typedef CSystem<LSN_CS_DENDY_MASTER, LSN_CS_DENDY_MASTER_DIVISOR,
		LSN_CS_DENDY_CPU_DIVISOR, LSN_CS_DENDY_PPU_DIVISOR, LSN_CS_DENDY_APU_DIVISOR,
		CCpu6502, CDendyPpu, CApu2A0X>														CDendySystem;

	/**
	 * An RGB (2C03) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502, CNtscPpu, CApu2A0X>														CRgb2C03System;

	/**
	 * An RGB (2C04) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502, CNtscPpu, CApu2A0X>														CRgb2C04System;

	/**
	 * An RGB (2C05) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502, CNtscPpu, CApu2A0X>														CRgb2C05System;

}	// namespace lsn
