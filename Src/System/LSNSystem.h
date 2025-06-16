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
#ifdef LSN_WINDOWS
#include <immintrin.h>
#endif	// #ifdef LSN_WINDOWS

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
			m_cCpu( &m_bBus ),
			m_pPpu( &m_bBus, &m_cCpu ),
			m_aApu( &m_bBus, &m_cCpu ) {
			LSN_HW_SLOTS hsSlots[LSN_SLOTS] = {
				// PHI1.
				{ &m_pPpu, static_cast<CTickable::PfTickFunc>(&_cPpu::Tick), 0 + _tPpuDiv, _tPpuDiv, LSN_PPU_SLOT },
				{ &m_cCpu, static_cast<CTickable::PfTickFunc>(&_cCpu::Tick), 0 + _tCpuDiv, _tCpuDiv, LSN_CPU_PHI2_SLOT },
				{ &m_aApu, static_cast<CTickable::PfTickFunc>(&_cApu::Tick), 0 + _tApuDiv, _tApuDiv, LSN_APU_SLOT },

				// The PHI2 of the CPU is spaced out to half the distance from the PHI1 above to the next PHI1.
				{ &m_pPpu, static_cast<CTickable::PfTickFunc>(&_cPpu::TickPhi2), 0 + _tPpuDiv + (_tPpuDiv / 2), _tPpuDiv, LSN_PPU_SLOT },
				{ &m_cCpu, static_cast<CTickable::PfTickFunc>(&_cCpu::TickPhi2), 0 + _tCpuDiv + (_tCpuDiv / 2), _tCpuDiv, LSN_CPU_SLOT },
			};
			std::memcpy( m_hsSlots, hsSlots, sizeof( hsSlots ) );

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
		void											ResetState( bool _bAnalog ) {
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
				m_hsSlots[LSN_CPU_SLOT].ui64Counter = 0 + _tCpuDiv;
				m_hsSlots[LSN_PPU_SLOT].ui64Counter = (_tPpuDiv / 2) + _tPpuDiv;
				m_hsSlots[LSN_APU_SLOT].ui64Counter = 0 + _tApuDiv;
				m_sSlotsToCheck[0] = LSN_CPU_SLOT;
				m_sSlotsToCheck[1] = LSN_PPU_SLOT;
				m_sSlotsToCheck[2] = LSN_APU_SLOT;
				m_hsSlots[LSN_CPU_PHI2_SLOT].ui64Counter = m_hsSlots[LSN_CPU_SLOT].ui64Counter + (_tCpuDiv / 2);
				
			}
		}

		/**
		 * Performs an update of the system state.  This means getting the amount of time that has passed since this was last called,
		 *	determining how many cycles need to be run for each hardware component, and running all of them.
		 */
		void											Tick() {
			m_ui64TickCount++;
			uint64_t ui64CurRealTime = m_cClock.GetRealTick();
			if LSN_LIKELY( !m_bPaused ) {
				uint64_t ui64Diff = ui64CurRealTime - m_ui64LastRealTime;
				m_ui64AccumTime += ui64Diff;
				{
					uint64_t ui64Hi;
					uint64_t ui64Low = _umul128( m_ui64AccumTime, _tMasterClock, &ui64Hi );
					m_ui64MasterCounter = _udiv128( ui64Hi, ui64Low, m_cClock.GetResolution() * _tMasterDiv, nullptr );

					//m_ui64MasterCounter = m_ui64AccumTime * _tMasterClock / (m_cClock.GetResolution() * _tMasterDiv);
				}


				LSN_HW_SLOTS * phsSlot = nullptr;
				do {
					phsSlot = nullptr;
					uint64_t ui64Low = ~0ULL;

					size_t sCheckedSlot;
					// Looping over the 4 slots adds a small amount of overhead.  Unrolling the loop is easy.
					// PPU slot.
					size_t sTmp = m_sSlotsToCheck[0];
					if LSN_LIKELY( m_hsSlots[sTmp].ui64Counter <= m_ui64MasterCounter ) {
						phsSlot = &m_hsSlots[sTmp];
						ui64Low = phsSlot->ui64Counter;
						sCheckedSlot = 0;
					}
					// CPU slot.
					sTmp = m_sSlotsToCheck[1];
					if LSN_UNLIKELY( m_hsSlots[sTmp].ui64Counter <= ui64Low && m_hsSlots[sTmp].ui64Counter <= m_ui64MasterCounter ) {
						phsSlot = &m_hsSlots[sTmp];
						ui64Low = phsSlot->ui64Counter;
						sCheckedSlot = 1;
					}
					// By assuming the APU is not divided into PHI1 and PHI2 we can save just a bit of time here.
					if LSN_UNLIKELY( m_hsSlots[LSN_APU_SLOT].ui64Counter < ui64Low && m_hsSlots[LSN_APU_SLOT].ui64Counter <= m_ui64MasterCounter ) {
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

			}
			m_ui64LastRealTime = ui64CurRealTime;
		}

		/**
		 * Gets the master Hz.
		 *
		 * \return Returns the master Hz.
		 */
		virtual uint64_t								GetMasterHz() const { return MasterHz(); }

		/**
		 * Gets the master divider.
		 *
		 * \return Returns the master divider.
		 */
		virtual uint64_t								GetMasterDiv() const { return MasterDiv(); }

		/**
		 * Gets the CPU divider.
		 *
		 * \return Returns the CPU divider.
		 */
		virtual uint64_t								GetCpuDiv() const { return CpuDiv(); }

		/**
		 * Gets the PPU divider.
		 *
		 * \return Returns the PPU divider.
		 */
		virtual uint64_t								GetPpuDiv() const { return PpuDiv(); }

		/**
		 * Gets the APU divider.
		 *
		 * \return Returns the APU divider.
		 */
		virtual uint64_t								GetApuDiv() const { return ApuDiv(); }

		/**
		 * Gets the APU Hz.
		 * 
		 * \return Returns the APU Hz.
		 **/
		virtual double									GetApuHz() const { return m_aApu.Hz(); }

		/**
		 * Gets the PPU frame count
		 *
		 * \return Returns the PPU frame count.
		 */
		virtual uint64_t								GetPpuFrameCount() const { return m_pPpu.FrameCount(); }

		/**
		 * Gets the current counter for the CPU.
		 * 
		 * \return Returns the CPU's current counter.
		 **/
		virtual uint64_t								GetCpuCounter() const { return m_hsSlots[LSN_CPU_PHI2_SLOT].ui64Counter; }

		/**
		 * Gets the current counter for the APU.
		 * 
		 * \return Returns the APU's current counter.
		 **/
		virtual uint64_t								GetApuCounter() const { return m_hsSlots[LSN_APU_SLOT].ui64Counter; }

		/**
		 * Gets the current counter for the PPU.
		 * 
		 * \return Returns the PPU's current counter.
		 **/
		virtual uint64_t								GetPpuCounter() const { return m_hsSlots[LSN_PPU_SLOT].ui64Counter; }

		/**
		 * Loads a ROM image.
		 *
		 * \param _vRom The ROM image to load.
		 * \param _s16Path The ROM file path.
		 * \return Returns true if the image was loaded, false otherwise.
		 */
		virtual bool									LoadRom( LSN_ROM &_rRom ) {
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
				case 25 : {
					m_pmbMapper = std::make_unique<CMapper023>();
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
				case 38 : {
					m_pmbMapper = std::make_unique<CMapper038>();
					break;
				}
				case 41 : {
					m_pmbMapper = std::make_unique<CMapper041>();
					break;
				}
				case 66 : {
					m_pmbMapper = std::make_unique<CMapper066>();
					break;
				}
				case 69 : {
					m_pmbMapper = std::make_unique<CMapper069>();
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
				case 112 : {
					m_pmbMapper = std::make_unique<CMapper112>();
					break;
				}
				case 113 : {
					m_pmbMapper = std::make_unique<CMapper113>();
					break;
				}
				/*case 155 : {
					m_pmbMapper = std::make_unique<CMapper001>();
					break;
				}*/
				case 184 : {
					m_pmbMapper = std::make_unique<CMapper184>();
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
				default : {
					m_pmbMapper = std::make_unique<CMapperBase>();
					m_rRom.riInfo.bMapperSupported = false;
					std::string sText = "****** Mapper not handled: " + std::to_string( m_rRom.riInfo.ui16Mapper ) + ".\r\n";
#ifdef LSN_WINDOWS
					::OutputDebugStringA( sText.c_str() );
#endif	// #ifdef LSN_WINDOWS
				}
			}
#ifdef LSN_WINDOWS
			{
				char szBuffer[128];
				std::sprintf( szBuffer, "****** CRC: 0x%.8X\r\n", m_rRom.riInfo.ui32Crc );
				::OutputDebugStringA( szBuffer );
				std::string sText;
				::OutputDebugStringW( reinterpret_cast<LPCWSTR>(CUtilities::NoExtension( m_rRom.riInfo.s16RomName ).c_str()) );
				::OutputDebugStringA( "\r\n" );
				sText = "****** Mapper: " + std::to_string( m_rRom.riInfo.ui16Mapper ) + ".\r\n";
				::OutputDebugStringA( sText.c_str() );
				sText = "****** Sub Mapper: " + std::to_string( m_rRom.riInfo.ui16SubMapper ) + ".\r\n";
				::OutputDebugStringA( sText.c_str() );
				sText = "****** PGM Size: " + std::to_string( m_rRom.vPrgRom.size() ) + ".\r\n";
				::OutputDebugStringA( sText.c_str() );
				sText = "****** CHR Size: " + std::to_string( m_rRom.vChrRom.size() ) + ".\r\n";
				::OutputDebugStringA( sText.c_str() );

				sText = "****** PGM RAM Size: " + std::to_string( m_rRom.i32WorkRamSize ) + ".\r\n";
				::OutputDebugStringA( sText.c_str() );
			}
#endif	// #ifdef LSN_WINDOWS
			m_cCpu.SetMapper( m_pmbMapper.get() );
			if ( m_pmbMapper ) {
				m_pmbMapper->InitWithRom( m_rRom, &m_cCpu, &m_cCpu, &m_pPpu );
			}

			return true;
		}

		/**
		 * Closes the ROM image.
		 * 
		 * \return Returns true if no ROM was opened or if everything went as-expected.  False indicates both a ROM being loaded and some kind of failure during its closing process.  Typically it means the ROM will not have been able
		 *	to save some data to a file that it needed, such as its battery-backed RAM.
		 **/
		virtual bool									CloseRom() {
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
		virtual void									PauseRom() { m_bPaused = true; }

		/**
		 * Unpauses the current ROM.
		 **/
		virtual void									UnpauseRom() { m_bPaused = false; }

		/**
		 * Toggles the current ROM's pause state.
		 **/
		virtual void									TogglePauseRom() { m_bPaused = !m_bPaused; }

		/**
		 * Determines whether the ROM is paused or not.
		 * 
		 * \return Returns true if the ROM is paused.
		 **/
		virtual bool									RomIsPaused() const { return m_bPaused; }

		/**
		 * Reset the ROM.
		 **/
		virtual void									ResetRom() {
			m_cCpu.ResetAnalog();
			m_pPpu.ResetAnalog();
			m_aApu.ResetAnalog();
		}

		/**
		 * Reset the ROM.
		 **/
		virtual void									PowerCycle() {
			ResetState( false );
		}

		/**
		 * Sets the input poller.
		 *
		 * \param _pipPoller The input poller pointer.
		 */
		virtual void									SetInputPoller( CInputPoller * _pipPoller ) {
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
		virtual LSN_PALETTE *							Palette() { return &m_pPpu.Palette(); }

		/**
		 * Gets the PPU as a display client.
		 *
		 * \return Returns the PPU as a CDisplayClient *.
		 */
		virtual CDisplayClient *						GetDisplayClient() { return &m_pPpu; }

		/**
		 * Sets the audio options.
		 * 
		 * \param _aoOptions The options to set.
		 **/
		virtual void									SetAudioOptions( const LSN_AUDIO_OPTIONS &_aoOptions ) {
			m_aApu.SetOptions( _aoOptions );
		}

		/**
		 * Sets the raw stream-to-file pointer.
		 * 
		 * \param _pfStream The stream-to-file pointer to set.
		 **/
		virtual void									SetRawStream( CWavFile * _pfStream ) {
			m_aApu.SetRawStream( _pfStream );
		}

		/**
		 * Sets the output-capture stream-to-file pointer.
		 * 
		 * \param _pfStream The stream-to-file pointer to set.
		 **/
		virtual void									SetOutStream( CWavFile * _pfStream ) {
			m_aApu.SetOutStream( _pfStream );
		}

		/**
		 * Sets as inactive (another system is being played).
		 **/
		virtual void									SetAsInactive() {
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
			CWavFile::PfMetaDataThreadFunc &_pfMetaThreadFunc ) {
			_pvParm = &m_aApu;
			_pfAddMetaFunc = nullptr;
			_pfAddMetaFuncRaw = &m_aApu.AddMetaDataFunc_Raw;
			_pfMetaThreadFunc = &m_aApu.MetaDataThreadFunc;
		}


	protected :
		// == Types.
		/** The sorted tickable cycles. */
		struct LSN_HW_SLOTS {
			CTickable *									ptHw;
			CTickable::PfTickFunc						pfTick;
			uint64_t 									ui64Counter;
			uint64_t									ui64Inc;
			size_t										sPartnerSlot;
		};


		// == Members.
		_cCpu											m_cCpu;								/**< The CPU. */
		_cPpu											m_pPpu;								/**< The PPU. */
		_cApu											m_aApu;								/**< The APU. */
		LSN_HW_SLOTS									m_hsSlots[LSN_SLOTS];				/**< Run-time tick states for each component. */
		size_t											m_sSlotsToCheck[3];					/**< Which slots to actually check.  PHI1 and PHI2 shouldn't be checked at the same time. */


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
