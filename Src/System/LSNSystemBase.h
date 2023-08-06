/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: System base class.  Provides the interface for all regional systems.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Display/LSNDisplayClient.h"
#include "../Input/LSNInputPoller.h"
#include "../Mappers/LSNAllMappers.h"
#include "../Palette/LSNPalette.h"
#include "../Time/LSNClock.h"


namespace lsn {

	/**
	 * Class CSystemBase
	 * \brief System base class.
	 *
	 * Description: System base class.  Provides the interface for all regional systems.
	 */
	class CSystemBase {
	public :
		CSystemBase() :
			m_ui64TickCount( 0 ),
			m_ui64AccumTime( 0 ),
			m_ui64LastRealTime( 0 ),
			m_ui64MasterCounter( 0 ),
			m_ui64CpuCounter( 0 ),
			m_ui64PpuCounter( 0 ),
			m_ui64ApuCounter( 0 ),
			m_ui64CpuCounterPhi2( 0 ),
			m_bPaused( false ) {
		}
		virtual ~CSystemBase() {
		}


		// == Functions.
		/**
		 * Resets all of the counters etc. to prepare for running a new emulation from the beginning.
		 * 
		 * \param _bAnalog If true, a soft reset is performed on the CPU, otherwise the CPU is reset to a known state.
		 */
		virtual void									ResetState( bool /*_bAnalog*/ ) = 0 { }

		/**
		 * Performs an update of the system state.  This means getting the amount of time that has passed since this was last called,
		 *	determining how many cycles need to be run for each hardware component, and running all of them.
		 */
		virtual void									Tick() = 0 {}

		/**
		 * Loads a ROM image.
		 *
		 * \param _rRom The ROM data to load, previously populated by a call to LoadRom().
		 * \return Returns true if the image was loaded, false otherwise.
		 */
		virtual bool									LoadRom( LSN_ROM &/*_rRom*/ ) = 0 { return false; }

		/**
		 * Sets the input poller.
		 *
		 * \param _pipPoller The input poller pointer.
		 */
		virtual void									SetInputPoller( CInputPoller * /*_pipPoller*/ ) {}

		/**
		 * Gets the accumulated real time.
		 *
		 * \return Returns the accumulated real time.
		 */
		inline uint64_t									GetAccumulatedRealTime() const { return m_ui64AccumTime; }

		/**
		 * Gets the total number of ticks.
		 *
		 * \return Returns the total number of ticks.
		 */
		inline uint64_t									GetTickCount() const { return m_ui64TickCount; }

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
		 * Determnines is a ROM is loaded or not.
		 *
		 * \return Returns true if a ROM is loaded.
		 */
		bool											IsRomLoaded() const {
			return !!m_rRom.vPrgRom.size();
		}

		/**
		 * Gets the master Hz.
		 *
		 * \return Returns the master Hz.
		 */
		virtual uint64_t								GetMasterHz() const { return 0; }

		/**
		 * Gets the master divider.
		 *
		 * \return Returns the master divider.
		 */
		virtual uint64_t								GetMasterDiv() const { return 0; }

		/**
		 * Gets the CPU divider.
		 *
		 * \return Returns the CPU divider.
		 */
		virtual uint64_t								GetCpuDiv() const { return 0; }

		/**
		 * Gets the PPU divider.
		 *
		 * \return Returns the PPU divider.
		 */
		virtual uint64_t								GetPpuDiv() const { return 0; }

		/**
		 * Gets the APU divider.
		 *
		 * \return Returns the APU divider.
		 */
		virtual uint64_t								GetApuDiv() const { return 0; }

		/**
		 * Gets the PPU frame count
		 *
		 * \return Returns the PPU frame count.
		 */
		virtual uint64_t								GetPpuFrameCount() const { return 0; }

		/**
		 * Gets the PPU as a display client.
		 *
		 * \return Returns the PPU as a CDisplayClient *.
		 */
		virtual CDisplayClient *						GetDisplayClient() { return nullptr; }

		/**
		 * Gets a pointer to the palette.
		 *
		 * \return Returns a pointer to the palette.
		 */
		virtual LSN_PALETTE *							Palette() { return nullptr; }

		/**
		 * Gets a pointer to loaded ROM or nullptr if no ROM is loaded.
		 *
		 * \return Returns a pointer to the palette.
		 */
		virtual const LSN_ROM *							GetRom() const { return IsRomLoaded() ? &m_rRom : nullptr; }

		/**
		 * Loads a ROM into the given LSN_ROM object.
		 *
		 * \param _vRom The in-memory ROM fille.
		 * \param _rRom The LSN_ROM target object.
		 * \param _s16Path The ROM file path.
		 * \return Returns true if the ROM was loaded.
		 */
		static bool										LoadRom( const std::vector<uint8_t> &_vRom, LSN_ROM &_rRom, const std::u16string &_s16Path );


	protected :
		// == Members.
		CClock											m_cClock;							/**< The master clock. */
		uint64_t										m_ui64TickCount;					/**< The number of times the emulator has ticked. */
		uint64_t										m_ui64AccumTime;					/**< The master accumulated real time. */
		uint64_t										m_ui64LastRealTime;					/**< The last real time value read from the clock. */
		uint64_t										m_ui64MasterCounter;				/**< Keeps track of how many master virtual cycles have accumulated. */
		uint64_t										m_ui64CpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the CPU.  Used for sorting. */
		uint64_t										m_ui64PpuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the PPU.  Used for sorting. */
		uint64_t										m_ui64ApuCounter;					/**< Keeps track of how many virtual cycles have accumulated on the APU.  Used for sorting. */
		uint64_t										m_ui64CpuCounterPhi2;				/**< Keeps track of how many virtual cycles have accumulated on the CPU.  Used for sorting. */
		CCpuBus											m_bBus;								/**< The bus. */
		LSN_ROM											m_rRom;								/**< The current cartridge. */
		std::unique_ptr<CMapperBase>					m_pmbMapper;						/**< The mapper. */
		bool											m_bPaused;							/**< Pause flag. */


		// == Functions.
		/**
		 * Loads a ROM image in .NES format.
		 *
		 * \param _vRom The ROM image to load.
		 * \return Returns true if the image was loaded, false otherwise.
		 */
		static bool										LoadNes( const std::vector<uint8_t> &_vRom, LSN_ROM &_rRom );
	};

}	// namespace lsn
