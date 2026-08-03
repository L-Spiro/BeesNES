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
#include "../Cpu/LSNCpu6502.h"
#include "../Crc/LSNCrc.h"
#include "../Database/LSNDatabase.h"
#include "../Ppu/LSNPpu2C0X.h"
#include "../Roms/LSNNesHeader.h"
#include "../Roms/LSNRom.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNSystemBase.h"

#include <algorithm>
#include <chrono>
#ifdef LSN_WINDOWS
#include <immintrin.h>
#endif	// #ifdef LSN_WINDOWS
#include <thread>

// Slots are sorted for performance.  Since the PPU is fastest, checking for it first makes the other cases less likely, because their times then not only have to be less than the global tick count but also less than the PPU's tick count.
// Because the other cases become less likely, we can add meaningful branch-prediction signals to the compiler.
#define LSN_PPU_SLOT											0
#define LSN_CPU_SLOT											1
#define LSN_APU_SLOT											2
#define LSN_PPU_PHI2_SLOT										3
#define LSN_CPU_PHI2_SLOT										4
#define LSN_SLOTS												5

#pragma warning( push )
#pragma warning( disable : 4701 )	// warning C4701: potentially uninitialized local variable 'sCheckedSlot' used

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
	class CSystem : public CSystemBase {
	public :
		CSystem() :
			m_cCpu( &m_bBus, this ),
			m_pPpu( &m_bBus, &m_cCpu ),
			m_aApu( &m_bBus, &m_cCpu, &m_cCpu ) {
			
			InitComponentTable();


			m_dSkipTime = (m_cClock.GetResolution() * 0.25);
			ResetState( false );
		}
		~CSystem() {
			CloseRom();
		}


		// == Functions.
		/**
		 * Resets all of the counters etc. to prepare for running a new emulation from the beginning.
		 * 
		 * \param _bAnalog If true, a soft reset is performed on the CPU, otherwise the CPU is reset to a known state.
		 */
		virtual void									ResetState( bool _bAnalog ) override {
			if ( _bAnalog ) {
				m_cCpu.ResetAnalog();
				m_aApu.ResetAnalog();
				m_pPpu.ResetAnalog();
			}
			else {
				m_ui64LastRealTime = m_cClock.GetRealTick();
				m_bBus.ApplyMap();
			
			
				m_cCpu.ApplyMemoryMap();
				m_aApu.ApplyMemoryMap();
				m_pPpu.ApplyMemoryMap();

				if ( IsRomLoaded() ) {
					switch ( m_rRom.riInfo.mmMirroring ) {
						case LSN_MM_VERTICAL : {
							m_pPpu.ApplyVerticalMirroring();
							break;
						}
						case LSN_MM_HORIZONTAL : {
							m_pPpu.ApplyHorizontalMirroring();
							break;
						}
						case LSN_MM_4_SCREENS : {
							m_pPpu.ApplyFourScreensMirroring();
							break;
						}
						case LSN_MM_1_SCREEN_A : {
							m_pPpu.ApplyOneScreenMirroring();
							break;
						}
						case LSN_MM_1_SCREEN_B : {
							m_pPpu.ApplyOneScreenMirroring_B();
							break;
						}
					}

					if ( m_pmbMapper.get() ) {
						m_pmbMapper->ApplyMap( &m_bBus, &m_pPpu.GetBus() );
					}
				}

				m_cCpu.ResetToKnown();
				m_aApu.ResetToKnown();
				m_pPpu.ResetToKnown();

				m_ui64TickCount = 0;
				m_ui64AccumTime = 0;
				m_ui64MasterCounter = 0;
				m_ui64CurMasterCounter = 0;
				
				PreProcessTickOrder();
			}
		}

		/**
		 * Performs an update of the system state.  This means getting the amount of time that has passed since this was last called,
		 *	determining how many cycles need to be run for each hardware component, and running all of them.
		 */
		virtual void									Tick() override {
			m_ui64TickCount++;
			uint64_t ui64CurRealTime = m_cClock.GetRealTick();
			uint64_t ui64Diff = ui64CurRealTime - m_ui64LastRealTime;

			if LSN_UNLIKELY( double( ui64Diff ) >= m_dSkipTime ) {
				// Excessive stutter control.
				m_ui64LastRealTime = ui64CurRealTime;
				return;
			}
			if LSN_LIKELY( !m_bPaused && !m_i32PauseCount ) {	
				m_ui64AccumTime += ui64Diff;
				{
					uint64_t ui64Hi;
					uint64_t ui64Low = _umul128( m_ui64AccumTime, _tMasterClock, &ui64Hi );
					m_ui64MasterCounter = _udiv128( ui64Hi, ui64Low, m_cClock.GetResolution() * _tMasterDiv, nullptr );

					//m_ui64MasterCounter = m_ui64AccumTime * _tMasterClock / (m_cClock.GetResolution() * _tMasterDiv);
				}

#if 1
				LSN_HW_SLOTS * phsSlot = m_vRuntimeSlots[m_sTickIdx];
				while ( phsSlot->ui64Counter <= m_ui64MasterCounter ) {
					//m_ui64CurMasterCounter = phsSlot->ui64Counter;
					(phsSlot->ptHw->*phsSlot->pfTick)();
					phsSlot->ui64Counter += phsSlot->ui64Inc;

					m_sTickIdx = (m_sTickIdx + 1) % m_vRuntimeSlots.size();
					phsSlot = m_vRuntimeSlots[m_sTickIdx];
				}
#else
				LSN_HW_SLOTS * phsSlot = nullptr;
				do {
					phsSlot = nullptr;
					uint64_t ui64Low = ~0ULL;

					size_t sCheckedSlot;
					// Looping over the 3 slots adds a small amount of overhead.  Unrolling the loop is easy.
					// PPU slot.
					size_t sTmp = m_sSlotsToCheck[1];
					if LSN_LIKELY( m_hsSlots[sTmp].ui64Counter <= m_ui64MasterCounter ) {
						phsSlot = &m_hsSlots[sTmp];
						ui64Low = phsSlot->ui64Counter;
						sCheckedSlot = 1;
					}
					// CPU slot.
					sTmp = m_sSlotsToCheck[0];
					if LSN_UNLIKELY( m_hsSlots[sTmp].ui64Counter < ui64Low && m_hsSlots[sTmp].ui64Counter <= m_ui64MasterCounter ) {
						phsSlot = &m_hsSlots[sTmp];
						ui64Low = phsSlot->ui64Counter;
						sCheckedSlot = 0;
					}
					// By assuming the APU is not divided into PHI1 and PHI2 we can save just a bit of time here.
					if LSN_UNLIKELY( m_hsSlots[LSN_APU_SLOT].ui64Counter <= ui64Low && m_hsSlots[LSN_APU_SLOT].ui64Counter <= m_ui64MasterCounter ) {
						// If we come in here then we know that the APU will be the one to tick.
						//	This means we can optimize away the "if ( phsSlot != nullptr )" check
						//	as well as the pointer-access ("phsSlot").
						// Testing showed this took the loop down from 0.71834220 cycles-per-tick to
						//	0.68499566 cycles-per-tick.
						// Switching to function pointers inside the CPU Tick() function brought it
						//	down to 0.63103939.
						m_ui64CurMasterCounter = m_hsSlots[LSN_APU_SLOT].ui64Counter;
						(m_hsSlots[LSN_APU_SLOT].ptHw->*m_hsSlots[LSN_APU_SLOT].pfTick)();
						m_hsSlots[LSN_APU_SLOT].ui64Counter += m_hsSlots[LSN_APU_SLOT].ui64Inc;
						//m_hsSlots[LSN_APU_SLOT].ptHw->Tick();
						//(*m_hsSlots[LSN_APU_SLOT].pfTick)();
					}
					else if ( phsSlot != nullptr ) {
						m_ui64CurMasterCounter = phsSlot->ui64Counter;
						(phsSlot->ptHw->*phsSlot->pfTick)();
						phsSlot->ui64Counter += phsSlot->ui64Inc;
						m_sSlotsToCheck[sCheckedSlot] = phsSlot->sPartnerSlot;
						//phsSlot->ptHw->Tick();
					}
					else { break; }

				} while ( true );
#endif	
				//std::this_thread::yield();
				//std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			}
			else {
				std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
			}
			m_ui64LastRealTime = ui64CurRealTime;
		}

		/**
		 * Gets the master Hz.
		 *
		 * \return Returns the master Hz.
		 */
		virtual uint64_t								GetMasterHz() const override { return MasterHz(); }

		/**
		 * Gets the master divider.
		 *
		 * \return Returns the master divider.
		 */
		virtual uint64_t								GetMasterDiv() const override { return MasterDiv(); }

		/**
		 * Gets the CPU divider.
		 *
		 * \return Returns the CPU divider.
		 */
		virtual uint64_t								GetCpuDiv() const override { return CpuDiv(); }

		/**
		 * Gets the PPU divider.
		 *
		 * \return Returns the PPU divider.
		 */
		virtual uint64_t								GetPpuDiv() const override { return PpuDiv(); }

		/**
		 * Gets the APU divider.
		 *
		 * \return Returns the APU divider.
		 */
		virtual uint64_t								GetApuDiv() const override { return ApuDiv(); }

		/**
		 * Gets the APU Hz.
		 * 
		 * \return Returns the APU Hz.
		 **/
		virtual double									GetApuHz() const override { return m_aApu.Hz(); }

		/**
		 * Gets the PPU frame count
		 *
		 * \return Returns the PPU frame count.
		 */
		virtual uint64_t								GetPpuFrameCount() const override { return m_pPpu.FrameCount(); }

		/**
		 * Gets the current PPU cycle.
		 * 
		 * \return Returns the current PPU cycle.
		 **/
		virtual inline uint64_t							GetPpuCycle() const override { return m_pPpu.GetCycle(); }

		/**
		 * Gets the current counter for the CPU.
		 * 
		 * \return Returns the CPU's current counter.
		 **/
		virtual uint64_t								GetCpuCounter() const override { return m_hsSlots[LSN_CPU_PHI2_SLOT].ui64Counter; }

		/**
		 * Gets the current counter for the APU.
		 * 
		 * \return Returns the APU's current counter.
		 **/
		virtual uint64_t								GetApuCounter() const override { return m_hsSlots[LSN_APU_SLOT].ui64Counter; }

		/**
		 * Gets the current counter for the PPU.
		 * 
		 * \return Returns the PPU's current counter.
		 **/
		virtual uint64_t								GetPpuCounter() const override { return m_hsSlots[LSN_PPU_SLOT].ui64Counter; }

		/**
		 * Loads a ROM image.
		 *
		 * \param _vRom The ROM image to load.
		 * \param _s16Path The ROM file path.
		 * \return Returns true if the image was loaded, false otherwise.
		 */
		virtual bool									LoadRom( LSN_ROM &_rRom ) override {
			m_pmbMapper.reset();
			m_cCpu.SetMapper( nullptr );
			m_rRom = std::move( _rRom );

			//m_bBus.DGB_FillMemoryUi32( 0xFFFFFF00 );
			m_bBus.DGB_Randomize();
			m_pPpu.GetPpuBus().DGB_Randomize();

			/*m_bBus.DGB_FillMemory( 0x00 );
			m_pPpu.GetPpuBus().DGB_FillMemory( 0x00 );*/

			uint16_t ui16Addr = 0x8000;
			uint16_t ui16Size = uint16_t( 0x10000 - ui16Addr );
				
			if ( m_rRom.vPrgRom.size() < ui16Size ) {
				ui16Addr = uint16_t( 0x10000 - m_rRom.vPrgRom.size() );
				ui16Size = uint16_t( m_rRom.vPrgRom.size() );
			}
			size_t stOffset = m_rRom.vPrgRom.size() - ui16Size;
			m_bBus.CopyToMemory( m_rRom.vPrgRom.data() + stOffset, ui16Size, ui16Addr );

			m_rRom.riInfo.bMapperSupported = true;
			switch ( m_rRom.riInfo.ui16Mapper ) {
				case 0 : {
					m_pmbMapper = std::make_unique<CMapper000>();
					break;
				}
				case 1 : {
					m_pmbMapper = std::make_unique<CMapper001>();
					break;
				}
				case 2 : {
					m_pmbMapper = std::make_unique<CMapper002>();
					break;
				}
				case 3 : {
					m_pmbMapper = std::make_unique<CMapper003>();
					break;
				}
				case 4 : {
					m_pmbMapper = std::make_unique<CMapper004>();
					break;
				}
				case 7 : {
					m_pmbMapper = std::make_unique<CMapper007>();
					break;
				}
				case 9 : {
					m_pmbMapper = std::make_unique<CMapper009>();
					break;
				}
				case 10 : {
					m_pmbMapper = std::make_unique<CMapper010>();
					break;
				}
				case 11 : {
					m_pmbMapper = std::make_unique<CMapper011>();
					break;
				}
				case 13 : {
					m_pmbMapper = std::make_unique<CMapper013>();
					break;
				}
				case 18 : {
					m_pmbMapper = std::make_unique<CMapper018>();
					break;
				}
				case 19 : {
					m_pmbMapper = std::make_unique<CMapper019>();
					break;
				}
				case 21 : {
					m_pmbMapper = std::make_unique<CMapper023>();
					break;
				}
				case 22 : {
					m_pmbMapper = std::make_unique<CMapper023>();
					break;
				}
				case 23 : {
					m_pmbMapper = std::make_unique<CMapper023>();
					break;
				}
				case 24 : {
					m_pmbMapper = std::make_unique<CMapper024>();
					break;
				}
				case 25 : {
					m_pmbMapper = std::make_unique<CMapper023>();
					break;
				}
				case 26 : {
					m_pmbMapper = std::make_unique<CMapper024>();
					break;
				}
				case 28 : {
					m_pmbMapper = std::make_unique<CMapper028>();
					break;
				}
				case 32 : {
					m_pmbMapper = std::make_unique<CMapper032>();
					break;
				}
				case 33 : {
					m_pmbMapper = std::make_unique<CMapper033>();
					break;
				}
				case 34 : {
					m_pmbMapper = std::make_unique<CMapper034>();
					break;
				}
				case 36 : {
					m_pmbMapper = std::make_unique<CMapper036>();
					break;
				}
				case 37 : {
					m_pmbMapper = std::make_unique<CMapper037>();
					break;
				}
				case 38 : {
					m_pmbMapper = std::make_unique<CMapper038>();
					break;
				}
				case 41 : {
					m_pmbMapper = std::make_unique<CMapper041>();
					break;
				}
				case 46 : {
					m_pmbMapper = std::make_unique<CMapper046>();
					break;
				}
				case 56 : {
					m_pmbMapper = std::make_unique<CMapper056>();
					break;
				}
				case 61 : {
					m_pmbMapper = std::make_unique<CMapper061>();
					break;
				}
				case 65 : {
					m_pmbMapper = std::make_unique<CMapper065>();
					break;
				}
				case 66 : {
					m_pmbMapper = std::make_unique<CMapper066>();
					break;
				}
				case 67 : {
					m_pmbMapper = std::make_unique<CMapper067>();
					break;
				}
				case 69 : {
					m_pmbMapper = std::make_unique<CMapper069>();
					break;
				}
				case 70 : {
					m_pmbMapper = std::make_unique<CMapper070>();
					break;
				}
				case 71 : {
					m_pmbMapper = std::make_unique<CMapper071>();
					break;
				}
				case 72 : {
					m_pmbMapper = std::make_unique<CMapper072>();
					break;
				}
				case 73 : {
					m_pmbMapper = std::make_unique<CMapper073>();
					break;
				}
				case 75 : {
					m_pmbMapper = std::make_unique<CMapper075>();
					break;
				}
				case 76 : {
					m_pmbMapper = std::make_unique<CMapper076>();
					break;
				}
				case 77 : {
					m_pmbMapper = std::make_unique<CMapper077>();
					break;
				}
				case 78 : {
					m_pmbMapper = std::make_unique<CMapper078>();
					break;
				}
				case 79 : {
					m_pmbMapper = std::make_unique<CMapper079>();
					break;
				}
				case 80 : {
					m_pmbMapper = std::make_unique<CMapper080>();
					break;
				}
				case 81 : {
					m_pmbMapper = std::make_unique<CMapper081>();
					break;
				}
				case 87 : {
					m_pmbMapper = std::make_unique<CMapper087>();
					break;
				}
				case 88 : {
					m_pmbMapper = std::make_unique<CMapper088>();
					break;
				}
				case 89 : {
					m_pmbMapper = std::make_unique<CMapper089>();
					break;
				}
				case 92 : {
					m_pmbMapper = std::make_unique<CMapper092>();
					break;
				}
				case 93 : {
					m_pmbMapper = std::make_unique<CMapper093>();
					break;
				}
				case 94 : {
					m_pmbMapper = std::make_unique<CMapper094>();
					break;
				}
				case 95 : {
					m_pmbMapper = std::make_unique<CMapper095>();
					break;
				}
				case 97 : {
					m_pmbMapper = std::make_unique<CMapper097>();
					break;
				}
				case 101 : {
					m_pmbMapper = std::make_unique<CMapper101>();
					break;
				}
				case 107 : {
					m_pmbMapper = std::make_unique<CMapper107>();
					break;
				}
				case 112 : {
					m_pmbMapper = std::make_unique<CMapper112>();
					break;
				}
				case 113 : {
					m_pmbMapper = std::make_unique<CMapper113>();
					break;
				}
				case 118 : {
					m_pmbMapper = std::make_unique<CMapper118>();
					break;
				}
				case 119 : {
					m_pmbMapper = std::make_unique<CMapper119>();
					break;
				}
				case 132 : {
					m_pmbMapper = std::make_unique<CMapper132>();
					break;
				}
				case 133 : {
					m_pmbMapper = std::make_unique<CMapper133>();
					break;
				}
				case 140 : {
					m_pmbMapper = std::make_unique<CMapper140>();
					break;
				}
				case 142 : {
					m_pmbMapper = std::make_unique<CMapper142>();
					break;
				}
				case 144 : {
					m_pmbMapper = std::make_unique<CMapper144>();
					break;
				}
				case 148 : {
					m_pmbMapper = std::make_unique<CMapper148>();
					break;
				}
				case 152 : {
					m_pmbMapper = std::make_unique<CMapper152>();
					break;
				}
				case 154 : {
					m_pmbMapper = std::make_unique<CMapper154>();
					break;
				}
				/*case 155 : {
					m_pmbMapper = std::make_unique<CMapper001>();
					break;
				}*/
				case 157 : {
					m_pmbMapper = std::make_unique<CMapper157>();
					break;
				}
				case 172 : {
					m_pmbMapper = std::make_unique<CMapper172>();
					break;
				}
				case 173 : {
					m_pmbMapper = std::make_unique<CMapper173>();
					break;
				}
				case 184 : {
					m_pmbMapper = std::make_unique<CMapper184>();
					break;
				}
				case 185 : {
					m_pmbMapper = std::make_unique<CMapper185>();
					break;
				}
				case 201 : {
					m_pmbMapper = std::make_unique<CMapper201>();
					break;
				}
				case 203 : {
					m_pmbMapper = std::make_unique<CMapper203>();
					break;
				}
				case 206 : {
					m_pmbMapper = std::make_unique<CMapper206>();
					break;
				}
				case 212 : {
					m_pmbMapper = std::make_unique<CMapper212>();
					break;
				}
				case 225 : {
					m_pmbMapper = std::make_unique<CMapper225>();
					break;
				}
				case 226 : {
					m_pmbMapper = std::make_unique<CMapper226>();
					break;
				}
				case 228 : {
					m_pmbMapper = std::make_unique<CMapper228>();
					break;
				}
				case 229 : {
					m_pmbMapper = std::make_unique<CMapper229>();
					break;
				}
				case 232 : {
					m_pmbMapper = std::make_unique<CMapper232>();
					break;
				}
				case 234 : {
					m_pmbMapper = std::make_unique<CMapper234>();
					break;
				}
				case 240 : {
					m_pmbMapper = std::make_unique<CMapper240>();
					break;
				}
				case 255 : {
					m_pmbMapper = std::make_unique<CMapper225>();	// Not an error.  255 is a duplicate of 225.
					break;
				}
				case 271 : {
					m_pmbMapper = std::make_unique<CMapper271>();
					break;
				}
				case 340 : {
					m_pmbMapper = std::make_unique<CMapper340>();
					break;
				}
				case 550 : {
					m_pmbMapper = std::make_unique<CMapper550>();
					break;
				}
				case uint16_t( -1 ) : {
					m_pmbMapper = std::make_unique<CMapperNsf>();
					break;
				}
				default : {
					m_pmbMapper = std::make_unique<CMapperBase>();
					m_rRom.riInfo.bMapperSupported = false;
					std::string sText = "****** Mapper not handled: " + std::to_string( m_rRom.riInfo.ui16Mapper ) + ".\r\n";
					lsn::DebugA( sText.c_str() );
				}
			}
			{
				char szBuffer[128];
				std::sprintf( szBuffer, "****** CRC: 0x%.8X\r\n", m_rRom.riInfo.ui32Crc );
				lsn::DebugA( szBuffer );
				std::string sText;
				lsn::DebugW( reinterpret_cast<const wchar_t *>(CUtilities::NoExtension( m_rRom.riInfo.s16RomName ).c_str()) );
				lsn::DebugA( "\r\n" );
				sText = "****** Mapper: " + std::to_string( m_rRom.riInfo.ui16Mapper ) + ".\r\n";
				lsn::DebugA( sText.c_str() );
				sText = "****** Sub Mapper: " + std::to_string( m_rRom.riInfo.ui16SubMapper ) + ".\r\n";
				lsn::DebugA( sText.c_str() );
				sText = "****** PGM Size: " + std::to_string( m_rRom.vPrgRom.size() ) + ".\r\n";
				lsn::DebugA( sText.c_str() );
				sText = "****** CHR Size: " + std::to_string( m_rRom.vChrRom.size() ) + ".\r\n";
				lsn::DebugA( sText.c_str() );

				sText = "****** PGM RAM Size: " + std::to_string( m_rRom.i32WorkRamSize ) + ".\r\n";
				lsn::DebugA( sText.c_str() );
			}
			m_cCpu.SetMapper( m_pmbMapper.get() );
			m_aApu.SetMapper( m_pmbMapper.get() );
			if ( m_pmbMapper ) {
				m_pmbMapper->InitWithRom( m_rRom, &m_cCpu, &m_pPpu, &m_cCpu, &m_pPpu );
			}

			return true;
		}

		/**
		 * Closes the ROM image.
		 * 
		 * \return Returns true if no ROM was opened or if everything went as-expected.  False indicates both a ROM being loaded and some kind of failure during its closing process.  Typically it means the ROM will not have been able
		 *	to save some data to a file that it needed, such as its battery-backed RAM.
		 **/
		virtual bool									CloseRom() override {
			bool bRes = true;
			if ( m_pmbMapper.get() ) {
				bRes = m_pmbMapper->SaveBatteryBacked();
				m_pmbMapper.reset();
				m_cCpu.SetMapper( nullptr );
			}
			m_rRom.vPrgRom.clear();

			return bRes;
		}

		/**
		 * Pauses the current ROM.
		 **/
		virtual void									PauseRom() override { m_bPaused = true; }

		/**
		 * Unpauses the current ROM.
		 **/
		virtual void									UnpauseRom() override { m_bPaused = false; }

		/**
		 * Toggles the current ROM's pause state.
		 **/
		virtual void									TogglePauseRom() override { m_bPaused = !m_bPaused; }

		/**
		 * Determines whether the ROM is paused or not.
		 * 
		 * \return Returns true if the ROM is paused.
		 **/
		virtual bool									RomIsPaused() const override { return m_bPaused; }

		/**
		 * Reset the ROM.
		 **/
		virtual void									ResetRom() override {
			m_cCpu.ResetAnalog();
			m_pPpu.ResetAnalog();
			m_aApu.ResetAnalog();
		}

		/**
		 * Reset the ROM.
		 **/
		virtual void									PowerCycle() override {
			ResetState( false );
		}

		/**
		 * Sets the input poller.
		 *
		 * \param _pipPoller The input poller pointer.
		 */
		virtual void									SetInputPoller( CInputPoller * _pipPoller ) override {
			m_cCpu.SetInputPoller( _pipPoller );
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

		/**
		 * Gets a pointer to the palette.
		 *
		 * \return Returns a pointer to the palette.
		 */
		virtual LSN_PALETTE *							Palette() override { return &m_pPpu.Palette(); }

		/**
		 * Gets the PPU as a display client.
		 *
		 * \return Returns the PPU as a CDisplayClient *.
		 */
		virtual CDisplayClient *						GetDisplayClient() override { return &m_pPpu; }

		/**
		 * Sets the audio options.
		 * 
		 * \param _aoOptions The options to set.
		 **/
		virtual void									SetAudioOptions( const LSN_AUDIO_OPTIONS &_aoOptions ) override {
			m_aApu.SetOptions( _aoOptions );
		}

		/**
		 * Gets the $4010 register value.
		 * 
		 * \return Returns the $4010 register value.
		 **/
		virtual uint8_t									Get4010() const override { return m_aApu.Get4010(); }

		/**
		 * Gets the $4012 register value.
		 * 
		 * \return Returns the $4012 register value.
		 **/
		virtual uint8_t									Get4012() const override { return m_aApu.Get4012(); }

		/**
		 * Gets the $4013 register value.
		 * 
		 * \return Returns the $4013 register value.
		 **/
		virtual uint8_t									Get4013() const override { return m_aApu.Get4013(); }

		/**
		 * Gets the DMC DMA address.
		 * 
		 * \return Returns the DMC DMA address.
		 **/
		virtual uint16_t								DmcDmaAddress() const override { return m_aApu.GetDmcDmaAddress(); }

		/**
		 * Hands the DMC DMA value off to the APU.
		 * 
		 * \param _ui8Value The value to hand off to the APU.
		 **/
		virtual void									ReceiveDmcSample( uint8_t _ui8Value ) override { m_aApu.ReceiveDmcSample( _ui8Value ); }

		/**
		 * Sets the raw stream-to-file pointer.
		 * 
		 * \param _pfStream The stream-to-file pointer to set.
		 **/
		virtual void									SetRawStream( CWavFile * _pfStream ) override {
			m_aApu.SetRawStream( _pfStream );
		}

		/**
		 * Sets the output-capture stream-to-file pointer.
		 * 
		 * \param _pfStream The stream-to-file pointer to set.
		 **/
		virtual void									SetOutStream( CWavFile * _pfStream ) override {
			m_aApu.SetOutStream( _pfStream );
		}

		/**
		 * Sets as inactive (another system is being played).
		 **/
		virtual void									SetAsInactive() override {
			m_aApu.SetAsInactive();
		}

		/**
		 * Sets the parameters needed for metadata streaming via our APU.
		 * 
		 * \param _pvParm Holds the returned pointer to our APU as a void *.
		 * \param _pfAddMetaFunc Holds a pointer to the APU's add function.
		 * \param _pfAddMetaFuncRaw Holds a pointer to the APU's add function for the raw stream.
		 * \param _pfMetaThreadFunc Holds a pointer to the APU's thread function for either metadata stream.
		 **/
		virtual void									SetMetaDataStreamParms( void * &_pvParm, CWavFile::PfAddMetaDataFunc &_pfAddMetaFunc, CWavFile::PfAddMetaDataFunc &_pfAddMetaFuncRaw,
			CWavFile::PfMetaDataThreadFunc &_pfMetaThreadFunc ) override {
			_pvParm = &m_aApu;
			_pfAddMetaFunc = nullptr;
			_pfAddMetaFuncRaw = &m_aApu.AddMetaDataFunc_Raw;
			_pfMetaThreadFunc = &m_aApu.MetaDataThreadFunc;
		}


	protected :
		// == Types.
		/** The sorted tickable cycles. */
		struct LSN_HW_SLOTS {
			CTickable *									ptHw = nullptr;
			CTickable::PfTickFunc						pfTick = nullptr;
			uint64_t 									ui64Counter = 0;
			uint64_t									ui64Inc = 0;
			size_t										sPartnerSlot = 0;
		};


		// == Members.
		double											m_dSkipTime = 0.0;					/**< Maximum duration between ticks before ticks are simply skipped. */
		_cCpu											m_cCpu;								/**< The CPU. */
		_cPpu											m_pPpu;								/**< The PPU. */
		_cApu											m_aApu;								/**< The APU. */
		LSN_HW_SLOTS									m_hsSlots[LSN_SLOTS];				/**< Run-time tick states for each component. */
		size_t											m_sSlotsToCheck[3];					/**< Which slots to actually check.  PHI1 and PHI2 shouldn't be checked at the same time. */
		size_t											m_sTickIdx = 0;						/**< The tick index into m_vRuntimeSlots. */
		std::vector<LSN_HW_SLOTS *>						m_vRuntimeSlots;


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
				m_rRom.riInfo.ui16Mapper = pnhHeader->GetMapper();
				m_rRom.riInfo.ui16SubMapper = pnhHeader->GetSubMapper();
				m_rRom.riInfo.mmMirroring = pnhHeader->GetMirrorMode();
				m_rRom.riInfo.pmConsoleRegion = pnhHeader->GetGameRegion();
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

		/**
		 * Initializes the component tables for run-time.
		 **/
		void											InitComponentTable() {
			uint64_t ui64PpuStart = 0;
			uint64_t ui64CpuStart = 0;
			uint64_t ui64ApuStart = 0;
			LSN_HW_SLOTS hsSlots[LSN_SLOTS] = {
				// PHI1.
				{ &m_pPpu, static_cast<CTickable::PfTickFunc>(&_cPpu::Tick), ui64PpuStart, _tPpuDiv, LSN_PPU_SLOT },
				{ &m_cCpu, static_cast<CTickable::PfTickFunc>(&_cCpu::Tick), ui64CpuStart, _tCpuDiv, LSN_CPU_PHI2_SLOT },
				{ &m_aApu, static_cast<CTickable::PfTickFunc>(&_cApu::Tick), ui64ApuStart, _tApuDiv, LSN_APU_SLOT },

				// The PHI2 of the CPU is spaced out to half the distance from the PHI1 above to the next PHI1.
				{ &m_pPpu, static_cast<CTickable::PfTickFunc>(&_cPpu::TickPhi2), ui64PpuStart + (_tPpuDiv / 2), _tPpuDiv, LSN_PPU_SLOT },
				{ &m_cCpu, static_cast<CTickable::PfTickFunc>(&_cCpu::TickPhi2), ui64CpuStart + (_tCpuDiv / 2), _tCpuDiv, LSN_CPU_SLOT },
			};
			std::memcpy( m_hsSlots, hsSlots, sizeof( hsSlots ) );

			m_sSlotsToCheck[0] = LSN_CPU_SLOT;
			m_sSlotsToCheck[1] = LSN_PPU_SLOT;
			m_sSlotsToCheck[2] = LSN_APU_SLOT;
		}

		/**
		 * Builds the tick table.  Preprocesses the scheduler into an array of items that can simply be ticked in order.
		 *	Must be called within a try/catch block.
		 **/
		void											PreProcessTickOrder() {
			InitComponentTable();
			size_t sLoops = 1;
			while ( (_tCpuDiv * sLoops) % _tPpuDiv ) {
				++sLoops;
			}
			// All components are half-cycle, but the PPU and CPU divisors are expressed in full cycle times, while the APU is expressed in half-cycle times.
			//	Based off the times we are supplied (_tCpuDiv, _tPpuDiv, and _tApuDiv), the CPU and PPU need to be counted twice to account for the extra PHI2's.
			// Determine how many of each component will be in the array.
			m_vRuntimeSlots.resize(
				(_tCpuDiv * sLoops / _tCpuDiv) * 2 +	// CPU PHI1 and PHI2 ticks = (sLoops * 2).
				(_tCpuDiv * sLoops / _tPpuDiv) * 1/*2*/ +	// PPU PHI1 and PHI2 ticks.
				(_tCpuDiv * sLoops / _tApuDiv)			// APU PHI1 and PHI2.
			);

			uint64_t ui64Starts[std::size(m_hsSlots)];
			for ( size_t I = 0; I < std::size( m_hsSlots ); ++I ) {
				ui64Starts[I] = m_hsSlots[I].ui64Counter;
			}

			LSN_HW_SLOTS * phsSlot = nullptr;
			for ( size_t I = 0; I < m_vRuntimeSlots.size(); ++I ) {
				phsSlot = nullptr;
				uint64_t ui64Low = ~0ULL;

				size_t sCheckedSlot;

				// PPU slot.
				size_t sTmp = m_sSlotsToCheck[1];
				phsSlot = &m_hsSlots[sTmp];
				ui64Low = phsSlot->ui64Counter;
				sCheckedSlot = 1;			// PPU: m_sSlotsToCheck[1] = LSN_PPU_SLOT

				// CPU slot.
				sTmp = m_sSlotsToCheck[0];
				if LSN_UNLIKELY( m_hsSlots[sTmp].ui64Counter < ui64Low ) {
					phsSlot = &m_hsSlots[sTmp];
					ui64Low = phsSlot->ui64Counter;
					sCheckedSlot = 0;		// CPU: m_sSlotsToCheck[0] = LSN_CPU_SLOT
				}
				// By assuming the APU is not divided into PHI1 and PHI2 we can save just a bit of time here.
				if LSN_UNLIKELY( m_hsSlots[LSN_APU_SLOT].ui64Counter <= ui64Low ) {
					phsSlot = &m_hsSlots[LSN_APU_SLOT];
					ui64Low = phsSlot->ui64Counter;
					sCheckedSlot = 2;		// APU: m_sSlotsToCheck[2] = LSN_APU_SLOT
				}
				
				m_vRuntimeSlots[I] = phsSlot;

				phsSlot->ui64Counter += phsSlot->ui64Inc;
				m_sSlotsToCheck[sCheckedSlot] = phsSlot->sPartnerSlot;
			}

			for ( size_t I = 0; I < std::size( m_hsSlots ); ++I ) {
				m_hsSlots[I].ui64Counter = ui64Starts[I];
			}
			m_sTickIdx = 0;
		}


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
		CCpu6502, CNtscPpu, CApu2A0X<LSN_APU_TYPE( NTSC ), LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR, LSN_CS_NTSC_APU_DIVISOR, false>>
																							CNtscSystem;

	/**
	 * A PAL system.
	 */
	typedef CSystem<LSN_CS_PAL_MASTER, LSN_CS_PAL_MASTER_DIVISOR,
		LSN_CS_PAL_CPU_DIVISOR, LSN_CS_PAL_PPU_DIVISOR, LSN_CS_PAL_APU_DIVISOR,
		CCpu6502, CPalPpu, CApu2A0X<LSN_APU_TYPE( PAL ), LSN_CS_PAL_MASTER, LSN_CS_PAL_MASTER_DIVISOR, LSN_CS_PAL_APU_DIVISOR, false>>
																							CPalSystem;

	/**
	 * A Dendy system.
	 */
	typedef CSystem<LSN_CS_DENDY_MASTER, LSN_CS_DENDY_MASTER_DIVISOR,
		LSN_CS_DENDY_CPU_DIVISOR, LSN_CS_DENDY_PPU_DIVISOR, LSN_CS_DENDY_APU_DIVISOR,
		CCpu6502, CDendyPpu, CApu2A0X<LSN_APU_TYPE( NTSC ), LSN_CS_DENDY_MASTER, LSN_CS_DENDY_MASTER_DIVISOR, LSN_CS_DENDY_APU_DIVISOR, false>>
																							CDendySystem;

	/**
	 * A PAL-M system.
	 */
	typedef CSystem<LSN_CS_PALM_MASTER, LSN_CS_PALM_MASTER_DIVISOR,
		LSN_CS_PALM_CPU_DIVISOR, LSN_CS_PALM_PPU_DIVISOR, LSN_CS_PALM_APU_DIVISOR,
		CCpu6502, CPalMPpu, CApu2A0X<LSN_APU_TYPE( NTSC ), LSN_CS_PALM_MASTER, LSN_CS_PALM_MASTER_DIVISOR, LSN_CS_PALM_APU_DIVISOR, true>>
																							CPalMSystem;

	/**
	 * A PAL-N system.
	 */
	typedef CSystem<LSN_CS_PALN_MASTER, LSN_CS_PALN_MASTER_DIVISOR,
		LSN_CS_PALN_CPU_DIVISOR, LSN_CS_PALN_PPU_DIVISOR, LSN_CS_PALN_APU_DIVISOR,
		CCpu6502, CPalNPpu, CApu2A0X<LSN_APU_TYPE( NTSC ), LSN_CS_PALN_MASTER, LSN_CS_PALN_MASTER_DIVISOR, LSN_CS_PALN_APU_DIVISOR, true>>
																							CPalNSystem;

	/**
	 * An RGB (2C03) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502, CNtscPpu, CApu2A0X<LSN_APU_TYPE( NTSC ), LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR, LSN_CS_NTSC_APU_DIVISOR, false>>
																							CRgb2C03System;

	/**
	 * An RGB (2C04) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502, CNtscPpu, CApu2A0X<LSN_APU_TYPE( NTSC ), LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR, LSN_CS_NTSC_APU_DIVISOR, false>>
																							CRgb2C04System;

	/**
	 * An RGB (2C05) system.
	 */
	typedef CSystem<LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR,
		LSN_CS_NTSC_CPU_DIVISOR, LSN_CS_NTSC_PPU_DIVISOR, LSN_CS_NTSC_APU_DIVISOR,
		CCpu6502, CNtscPpu, CApu2A0X<LSN_APU_TYPE( NTSC ), LSN_CS_NTSC_MASTER, LSN_CS_NTSC_MASTER_DIVISOR, LSN_CS_NTSC_APU_DIVISOR, false>>
																							CRgb2C05System;

}	// namespace lsn

#undef LSN_SLOTS
#undef LSN_CPU_PHI2_SLOT
#undef LSN_APU_SLOT
#undef LSN_PPU_SLOT
#undef LSN_CPU_SLOT

#pragma warning( pop )
