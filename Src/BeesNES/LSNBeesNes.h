/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles all emulator functionality.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../Filters/LSNNtscBlarggFilter.h"
#include "../Filters/LSNNtscCrtFilter.h"
#include "../Filters/LSNNtscCrtFullFilter.h"
#include "../Filters/LSNPalBlarggFilter.h"
#include "../Filters/LSNBiLinearPostProcess.h"
#include "../Filters/LSNRgb24Filter.h"
#include "../Options/LSNOptions.h"
#include "../System/LSNSystem.h"
#include "../Utilities/LSNStream.h"

#define LSN_BEESNES_SETTINGS_VERSION			0

namespace lsn {

	/**
	 * Class CBeesNes
	 * \brief Handles all emulator functionality.
	 *
	 * Description: Handles all emulator functionality.
	 */
	class CBeesNes {
		typedef lsn::CDendySystem
												CRegionalSystem;
	public :
		CBeesNes( CDisplayHost * _pdhDisplayHost, CInputPoller * _pipPoller );
		~CBeesNes();


		// == Types.
		/** The current render target/filter. */
		struct LSN_CUR_FILTER_AND_RENDER_TARGET {
			uint64_t							ui64Frame = 0;								/**< The PPU frame count associated with the render target. */
			uint64_t							ui64RenderStartCycle = 0;					/**< The cycle at which rendering began. */
			CFilterBase *						pfbCurFilter = nullptr;						/**< The current filter. */
			CFilterBase *						pfbNextFilter = nullptr;					/**< The next filter. */
			CFilterBase *						pfbPrevFilter = nullptr;					/**< The previous filter. */
			uint8_t *							pui8CurRenderTarget = nullptr;				/**< The current render target. */
			uint8_t *							pui8LastFilteredResult = nullptr;			/**< The last filtered result. */
			uint32_t							ui32Width = 0;								/**< The current render target's width in pixels. */
			uint32_t							ui32Height = 0;								/**< The current render target's height in pixels. */
			uint32_t							ui32Stride = 0;								/**< The current render target's stride in bytes. */
			uint16_t							ui16Bits = 0;								/**< The current render target's bit depth. */
			bool								bDirty = true;								/**< The dirty flag. */
			bool								bMirrored = false;							/**< If true, the image was rendered up-side down and does not need to be flipped by ::StretchDIBits() or ::SetDIBitsToDevice() to render properly. */
		};


		// == Functions.
		/**
		 * Sets the current folder.
		 *
		 * \param _pwcFolder The current folder.
		 */
		void									SetFolder( const wchar_t * _pwcFolder ) { m_wsFolder = _pwcFolder; }

		/**
		 * Loads the settings file.
		 *
		 * \return Returns true if the settings file was loaded.
		 */
		bool									LoadSettings();

		/**
		 * Saves the settings file.
		 *
		 * \return Returns true if the settings file was created.
		 */
		bool									SaveSettings();

		/**
		 * Gets the screen scale.
		 *
		 * \return Returns the emulation screen scale.
		 */
		inline double							GetScale() const { return m_dScale; }

		/**
		 * Sets the screen scale.
		 *
		 * \param _dScale The new emulation output screen scale.
		 */
		inline void								SetScale( double _dScale ) { m_dScale = _dScale; }

		/**
		 * Gets the screen ratio.
		 *
		 * \return Returns the emulation screen ratio.
		 */
		inline double							GetRatio() const { return m_dRatio; }

		/**
		 * Sets the screen ratio.
		 *
		 * \param _dRatio The new screen ratio.
		 */
		inline void								SetRatio( double _dRatio ) { m_dRatio = _dRatio; }

		/**
		 * Gets the screen ratio (actual).  The actual ratio is the actual desired ratio, whereas GetRatio() returns the multiplier for the width after converting the actual ratio to a width multiplier.
		 *
		 * \return Returns the emulation screen actual ratio.
		 */
		inline double							GetRatioActual() const { return m_dRatioActual; }

		/**
		 * Sets the screen actual ratio.
		 *
		 * \param _dRatio The new screen actual ratio.
		 */
		inline void								SetRatioActual( double _dRatio ) { m_dRatioActual = _dRatio; }

		/**
		 * Gets the current filter.
		 *
		 * \return Returns the current filter.
		 */
		inline CFilterBase::LSN_FILTERS			GetCurFilter() const { return m_fFilter; }

		/**
		 * Sets the current filter.  Changes are not applied to rendering output until Swap() is called.
		 *
		 * \param _fFilter The new filter to be applied after the next Swap().
		 */
		void									SetCurFilter( CFilterBase::LSN_FILTERS _fFilter );

		/**
		 * Gets the current region.
		 *
		 * \return Returns the current region.
		 */
		LSN_PPU_METRICS							GetCurPpuRegion() const;

		/**
		 * Gets the current display client.
		 *
		 * \return Returns the display client or nullptr.
		 */
		inline CDisplayClient *					GetDisplayClient() { return m_pnsSystem->GetDisplayClient(); }

		/**
		 * Gets the current display client.
		 *
		 * \return Returns the display client or nullptr.
		 */
		inline const CDisplayClient *			GetDisplayClient() const { return m_pnsSystem->GetDisplayClient(); }

		/**
		 * Gets the render target width, accounting for any extra debug information to be displayed on the side.
		 *
		 * \return Returns the width of the console screen plus any side debug information.
		 */
		uint32_t								RenderTargetWidth() const {
			return GetDisplayClient() ? uint32_t( GetDisplayClient()->DisplayWidth() + (GetDisplayClient()->DebugSideDisplay() ? 128 : 0) ) :
				0;
		}

		/**
		 * Gets the current render target height.
		 *
		 * \return Returns the height of the console screen.
		 */
		uint32_t								RenderTargetHeight() const {
			return GetDisplayClient() ? GetDisplayClient()->DisplayHeight() :
				0;
		}

		/**
		 * Gets the current system.
		 *
		 * \return Returns a pointer to the current console system.
		 */
		CSystemBase *							GetSystem() { return m_pnsSystem; }

		/**
		 * Gets the current system.
		 *
		 * \return Returns a constant pointer to the current console system.
		 */
		const CSystemBase *						GetSystem() const { return m_pnsSystem; }

		/**
		 * Gets the current render information.
		 *
		 * \return Returns a constant reference to the render state.
		 */
		const LSN_CUR_FILTER_AND_RENDER_TARGET &RenderInfo() const { return m_cfartCurFilterAndTargets; }

		/**
		 * If the render buffer is dirty, a render is performed (PPU buffer -> Filters -> Post-Processing).
		 *	This should be called inside the same critical section/mutex that calls Swap().
		 *
		 * \param _ui32FinalW The final display width.
		 * \param _ui32FinalH The final display height.
		 */
		void									Render( uint32_t _ui32FinalW, uint32_t _ui32FinalH );

		/**
		 * Swaps PPU render targets and switches to the next filter if a new filter has been set.
		 *	Should be called inside the same citical section/mutex as Render().
		 */
		void									Swap();

		/**
		 * Loads a ROM.
		 *
		 * \param _vRom The in-memory ROM file.
		 * \param _s16Path The full path to the ROM.
		 * \param _pmRegion The region to use when loading the ROM.  If LSN_PM_UNKNOWN, the ROM data is used to determine the region.
		 * \return Returns true if the ROM was loaded successfully.
		 */
		bool									LoadRom( const std::vector<uint8_t> &_vRom, const std::u16string &_s16Path, LSN_PPU_METRICS _pmRegion = LSN_PM_UNKNOWN );

		/**
		 * Gets the rapid-fire patterns.
		 *
		 * \return Returns a pointer to the 8 rapid-fire values.
		 */
		uint8_t *								RapidFire() { return m_ui8RapidFires; }

		/**
		 * Gets the program options.
		 *
		 * \return Returns a constant reference to the program options.
		 */
		const LSN_OPTIONS &						Options() const { return m_oOptions; }

		/**
		 * Gets the program options.
		 *
		 * \return Returns a reference to the program options.
		 */
		LSN_OPTIONS &							Options() { return m_oOptions; }


	protected :
		// == Members.
		/** The output scale. */
		double									m_dScale;
		/** The output ratio. */
		double									m_dRatio;
		/** Used to derive m_dRatio. */
		double									m_dRatioActual;
		/** The current/next filter/render target. */
		LSN_CUR_FILTER_AND_RENDER_TARGET		m_cfartCurFilterAndTargets;
		/** The standard RGB filter. */
		CRgb24Filter							m_r24fRgb24Filter;
		/** Blargg’s NTSC filter. */
		CNtscBlarggFilter						m_nbfBlargNtscFilter;
		/** EMMIR (LMP88959)’s NTSC-CRT filter. */
		CNtscCrtFilter							m_ncfEmmirNtscFilter;
		/** EMMIR (LMP88959)’s NTSC-CRT filter. */
		CNtscCrtFullFilter						m_ncfEmmirNtscFullFilter;
		/** Blargg’s NTSC filter (ad-hoc PAL-inated). */
		CPalBlarggFilter						m_nbfBlargPalFilter;
		/** A filter table. */
		CFilterBase *							m_pfbFilterTable[CFilterBase::LSN_F_TOTAL][LSN_PM_CONSOLE_TOTAL];
		/** "NONE" post-processing. */
		CPostProcessBase						m_ppbNoPostProcessing;
		/** A bi-linear post-process filter. */
		CBiLinearPostProcess					m_blppBiLinearPost;
		/** A post-processing table. */
		CPostProcessBase *						m_pppbPostTable[CPostProcessBase::LSN_PP_TOTAL];
		/** The display host. */
		CDisplayHost *							m_pdhDisplayHost;
		/** The input poller. */
		CInputPoller *							m_pipPoller;
		/** The NTSC console. */
		CNtscSystem								m_nsNtscSystem;
		/** The PAL console. */
		CPalSystem								m_nsPalSystem;
		/** The Dendy console. */
		CDendySystem							m_nsDendySystem;
		/** The array of console pointers. */
		CSystemBase *							m_psbSystems[LSN_PM_CONSOLE_TOTAL];
		/** The console pointer. */
		CSystemBase *							m_pnsSystem;
		/** The path to the executable folder. */
		std::wstring							m_wsFolder;
		/** The current system type. */
		LSN_PPU_METRICS							m_pmSystem;
		/** The current filter ID. */
		CFilterBase::LSN_FILTERS				m_fFilter;
		/** The current post-processing filter. */
		CPostProcessBase::LSN_POST_PROCESSES	m_ppPostProcess;
		/** Rapid-fire buttons. */
		uint8_t									m_ui8RapidFires[8];
		/** The emulation options. */
		LSN_OPTIONS								m_oOptions;


		// == Functions.
		/**
		 * Updates the current system with render information, display hosts, etc.
		 */
		void									UpdateCurrentSystem();

		/**
		 * Loads the settings file.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \return Returns true if the settings file was loaded.
		 */
		bool									LoadSettings( CStream &_sFile );

		/**
		 * Saves the settings file.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \return Returns true if the settings file was saved.
		 */
		bool									SaveSettings( CStream &_sFile );

		/**
		 * Loads input settings.
		 *
		 * \param _ui32Version The file version.
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _ioInputOptions The input options into which to load the settings data.
		 * \return Returns true if the settings data was loaded.
		 */
		bool									LoadInputSettings( uint32_t _ui32Version, CStream &_sFile, LSN_INPUT_OPTIONS &_ioInputOptions );

		/**
		 * Saves input settings.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _ioInputOptions The input options to write to the settings data.
		 * \return Returns true if the settings data was saved.
		 */
		bool									SaveInputSettings( CStream &_sFile, LSN_INPUT_OPTIONS &_ioInputOptions );
	};

}	// namespace lsn
