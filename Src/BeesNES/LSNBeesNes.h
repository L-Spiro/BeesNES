/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles all emulator functionality.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../Filters/LSNBiLinearPostProcess.h"
#include "../Filters/LSNBleedPostProcess.h"
#include "../Filters/LSNNesPalette.h"
#include "../Filters/LSNNtscBlarggFilter.h"
//#include "../Filters/LSNNtscCrtFilter.h"
#include "../Filters/LSNNtscCrtFullFilter.h"
//#include "../Filters/LSNNtscBisqwitFilter.h"
#include "../Filters/LSNNtscLSpiroFilter.h"
#include "../Filters/LSNPalBlarggFilter.h"
#include "../Filters/LSNPalCrtFullFilter.h"
#include "../Filters/LSNPalLSpiroFilter.h"
#include "../Filters/LSNRgb24Filter.h"
#include "../Filters/LSNSrgbPostProcess.h"
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
		 * Loads the per-game settings from the given path.
		 * 
		 * \param _u16Path The path from which to load per-game settings.
		 * \return Returns true if the file was found and the settings were loaded from it.
		 **/
		bool									LoadPerGameSettings( const std::u16string &_u16Path );

		/**
		 * Saves the per-game settings to a given path.
		 * 
		 * \param _u16Path The path to which to save per-game settings.
		 * \return Returns true if the file was created and fully written.
		 **/
		bool									SavePerGameSettings( const std::u16string &_u16Path );

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
		inline CFilterBase::LSN_FILTERS			GetCurFilter() const { return m_oOptions.fFilter; }

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
		inline CDisplayClient *					GetDisplayClient() { return m_psbSystem->GetDisplayClient(); }

		/**
		 * Gets the current display client.
		 *
		 * \return Returns the display client or nullptr.
		 */
		inline const CDisplayClient *			GetDisplayClient() const { return m_psbSystem->GetDisplayClient(); }

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
		CSystemBase *							GetSystem() { return m_psbSystem; }

		/**
		 * Gets the current system.
		 *
		 * \return Returns a constant pointer to the current console system.
		 */
		const CSystemBase *						GetSystem() const { return m_psbSystem; }

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
		 * Closes the current ROM.
		 **/
		void									CloseRom();

		/**
		 * Pauses the current ROM.
		 **/
		void									PauseRom();

		/**
		 * Unpauses the current ROM.
		 **/
		void									UnpauseRom();

		/**
		 * Toggles the current ROM's pause state.
		 **/
		void									TogglePauseRom();

		/**
		 * Determines whether the ROM is paused or not.
		 * 
		 * \return Returns true if the ROM is paused.
		 **/
		bool									RomIsPaused() const;

		/**
		 * Resets the ROM.
		 **/
		void									ResetRom();

		/**
		 * Resets the ROM.
		 **/
		void									PowerCycle();

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

		/**
		 * Gets the vector of recent ROM files loaded.
		 * 
		 * \return Returns a constant vector to the most recently loaded ROM files.
		 **/
		const std::vector<std::u16string> &		RecentFiles() const { return m_vRecentFiles; }

		/**
		 * Applies the current audio options.
		 **/
		void									ApplyAudioOptions();

		/**
		 * Applies the current palette.
		 * 
		 * \return Returns true if a palette was opened.
		 **/
		bool									ApplyPaletteOptions();

		/**
		 * Gets the default folder for palettes.
		 * 
		 * \return Returns the default folder where default palettes can be found.
		 **/
		std::wstring							DefaultPaletteFolder() const;

		/**
		 * Gets the default NTSC palette path.
		 * 
		 * \return Return the default NTSC palette path.
		 **/
		std::wstring							DefaultNtscPalette() const {
			return DefaultPaletteFolder() + L"2C02G_wiki.pal";
		}

		/**
		 * Gets the default PAL palette path.
		 * 
		 * \return Return the default PAL palette path.
		 **/
		std::wstring							DefaultPalPalette() const {
			return DefaultPaletteFolder() + L"2C07_wiki.pal";
		}

		/**
		 * Gets the default NTSC-J palette path.
		 * 
		 * \return Return the default NTSC-J palette path.
		 **/
		std::wstring							DefaultNtscJPalette() const {
			return DefaultPaletteFolder() + L"2C02G_phs_aps_ela_applysrgb_NTSC-J.pal";
		}

		/**
		 * Gets a reference to the current palette.
		 * 
		 * \return Returns a reference to the current palette.
		 **/
		CNesPalette &							Palette() { return m_npPalette; }

		/**
		 * Gets a constant reference to the current palette.
		 * 
		 * \return Returns a constant reference to the current palette.
		 **/
		const CNesPalette &						Palette() const { return m_npPalette; }

		/**
		 * Gets the CRT palette gamma.
		 * 
		 * \return Returns the CRT palette gamma.
		 **/
		CNesPalette::LSN_GAMMA					PaletteCrtGamma() const { return m_gCrtGamma; }

		/**
		 * Gets the monitor palette gamma.
		 * 
		 * \return Returns the monitor palette gamma.
		 **/
		CNesPalette::LSN_GAMMA					PaletteMonitorGamma() const { return m_gMonitorGamma; }


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
		/** L. Spiro's NTSC filter. */
		CNtscLSpiroFilter						m_nbfLSpiroNtscFilter;
		/** L. Spiro's PAL filter. */
		CPalLSpiroFilter						m_nbfLSpiroPalFilter;
		/** L. Spiro's Dendy filter. */
		CPalLSpiroFilter						m_nbfLSpiroDendyFilter;
		/** L. Spiro's PAL-M filter. */
		CPalLSpiroFilter						m_nbfLSpiroPalMFilter;
		/** L. Spiro's PAL-N filter. */
		CPalLSpiroFilter						m_nbfLSpiroPalNFilter;
		/** Blargg's NTSC filter. */
		CNtscBlarggFilter						m_nbfBlarggNtscFilter;
		/** EMMIR (LMP88959)'s NTSC-CRT filter. */
		CNtscCrtFullFilter						m_ncfEmmirNtscFullFilter;
		/** EMMIR (LMP88959)'s PAL-CRT filter. */
		CPalCrtFullFilter						m_ncfEmmirPalFullFilter;
		/** Blargg's NTSC filter (ad-hoc PAL-inated). */
		CPalBlarggFilter						m_nbfBlarggPalFilter;
		/** A filter table. */
		CFilterBase *							m_pfbFilterTable[CFilterBase::LSN_F_TOTAL][LSN_PM_CONSOLE_TOTAL];
		/** The default palettes for each system. */
		CNesPalette								m_npPalette;
		/** CRT gamma. */
		CNesPalette::LSN_GAMMA					m_gCrtGamma;
		/** Monitor gamma. */
		CNesPalette::LSN_GAMMA					m_gMonitorGamma;
		/** "NONE" post-processing. */
		CPostProcessBase						m_ppbNoPostProcessing;
		/** Bleed post-processing. */
		CBleedPostProcess						m_bppBleedPostProcessing;
		/** A bi-linear post-process filter. */
		CBiLinearPostProcess					m_blppBiLinearPost;
		/** Linear -> sRGB conversion. */
		CSrgbPostProcess						m_sppLinearTosRGBPost;
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
		/** The PAL-M console. */
		CPalMSystem								m_nsPalMSystem;
		/** The PAL-N console. */
		CPalNSystem								m_nsPalNSystem;
		/** The array of console pointers. */
		CSystemBase *							m_psbSystems[LSN_PM_CONSOLE_TOTAL];
		/** The console pointer. */
		CSystemBase *							m_psbSystem;
		/** The path to the executable folder. */
		std::wstring							m_wsFolder;
		/** The current system type. */
		LSN_PPU_METRICS							m_pmSystem;
		
		/** The current post-processing filter. */
		//CPostProcessBase::LSN_POST_PROCESSES	m_ppPostProcess;
		std::vector<CPostProcessBase::LSN_POST_PROCESSES>
												m_vPostProcesses;
		/** A temporary buffer for thread-safe copying of the PPU output for filtering. */
		std::vector<uint8_t>					m_vTmpBuffer;
		/** Rapid-fire buttons. */
		uint8_t									m_ui8RapidFires[8];
		/** The emulation options. */
		LSN_OPTIONS								m_oOptions;
		/** The per-game settings path. */
		std::u16string							m_u16PerGameSettings;

		/** The list of recently opened ROM's. */
		std::vector<std::u16string>				m_vRecentFiles;
		/** The total number of recently opened to allow. */
		uint32_t								m_ui32RecentLimit;

		/** The raw WAV stream. */
		static CWavFile							m_wfRawStream;
		/** The output-capture WAV stream. */
		static CWavFile							m_wfOutStream;


		// == Functions.
		/**
		 * Updates the current system with render information, display hosts, etc.
		 */
		void									UpdateCurrentSystem();

		/**
		 * Sets the stream-to-file options.
		 * 
		 * \param _stfoStreamOptions The stream-to-file options to set.
		 * \tparam _bIsRaw If true, the options are applied to the raw signal, otherwise to the output.
		 **/
		template <bool _bIsRaw>
		void									SetStreamToFileOptions( const CWavFile::LSN_STREAM_TO_FILE_OPTIONS &_stfoStreamOptions ) {
			uint32_t ui32Hz = _bIsRaw ? static_cast<uint32_t>(std::ceil( m_psbSystem->GetApuHz() )) : _stfoStreamOptions.ui32Hz;
			constexpr uint32_t ui32BufferSize = _bIsRaw ? 1024 * 1024 : 10 * 1024;
			try {
				CWavFile * pwfFile;
				if constexpr ( _bIsRaw ) {
					pwfFile = &m_wfRawStream;
				}
				else {
					pwfFile = &m_wfOutStream;
				}

				std::filesystem::path pAbsolutePath = _stfoStreamOptions.wsPath.size() ? std::filesystem::absolute( std::filesystem::path( _stfoStreamOptions.wsPath ) ) : std::filesystem::path();
				if ( pwfFile->GetStreamData().bStreaming ) {
					// If it is already exporting, determine if it needs to be stopped.
					if ( !_stfoStreamOptions.bEnabled ) {
						pwfFile->StopStream();
						return;
					}
					// Streaming is enabled.
					uint16_t ui16Bits = _stfoStreamOptions.fFormat == CWavFile::LSN_F_IEEE_FLOAT ? 32 : uint16_t( _stfoStreamOptions.ui32Bits );
					// Already streaming and will continue to stream, but maybe some parameters have changed.
					if ( pwfFile->GetStreamData().wsPath != pAbsolutePath.generic_wstring() ||
						pwfFile->GetStreamData().ui16Bits != ui16Bits ||
						pwfFile->GetStreamData().ui16Channels != 1 ||
						pwfFile->GetStreamData().fFormat != _stfoStreamOptions.fFormat ||
						pwfFile->GetStreamData().bDither != _stfoStreamOptions.bDither ) {
						// An entirely new recording should begin.
						if ( !pwfFile->StreamToFile( _stfoStreamOptions, ui32Hz, ui32BufferSize ) ) {
							pwfFile->StopStream();
							return;
						}
					}
					// TODO: Update starting and stopping condition.
				}
				else if ( _stfoStreamOptions.bEnabled ) {
					// Streaming is not being done.  Start a new stream.
					if ( !pwfFile->StreamToFile( _stfoStreamOptions, ui32Hz, ui32BufferSize ) ) {
						pwfFile->StopStream();
						return;
					}
				}
			}
			catch ( ... ) {}
		}

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

		/**
		 * Loads the recent-files list.
		 *
		 * \param _ui32Version The file version.
		 * \param _sFile The in-memory stream of the settings file.
		 * \return Returns true if the settings data was loaded.
		 */
		bool									LoadRecentFiles( uint32_t _ui32Version, CStream &_sFile );

		/**
		 * Saves the recent-files list.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \return Returns true if the settings data was saved.
		 */
		bool									SaveRecentFiles( CStream &_sFile );

		/**
		 * Loads audio settings.
		 *
		 * \param _ui32Version The file version.
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _aoAudioOptions The audio options into which to load the settings data.
		 * \return Returns true if the settings data was loaded.
		 */
		bool									LoadAudioSettings( uint32_t _ui32Version, CStream &_sFile, LSN_AUDIO_OPTIONS &_aoAudioOptions );

		/**
		 * Saves audio settings.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _aoAudioOptions The audio options to write to the settings data.
		 * \return Returns true if the settings data was saved.
		 */
		bool									SaveAudioSettings( CStream &_sFile, LSN_AUDIO_OPTIONS &_aoAudioOptions );

		/**
		 * Loads audio stream-to-file settings.
		 *
		 * \param _ui32Version The file version.
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _stfoAudioOptions The audio options into which to load the settings data.
		 * \return Returns true if the settings data was loaded.
		 */
		bool									LoadAudioStreamSettings( uint32_t _ui32Version, CStream &_sFile, CWavFile::LSN_STREAM_TO_FILE_OPTIONS &_stfoAudioOptions );

		/**
		 * Saves audio stream-to-file settings.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _stfoAudioOptions The audio options to write to the settings data.
		 * \return Returns true if the settings data was saved.
		 */
		bool									SaveAudioStreamSettings( CStream &_sFile, CWavFile::LSN_STREAM_TO_FILE_OPTIONS &_stfoAudioOptions );

		/**
		 * Loads WAV Editor window settings.
		 *
		 * \param _ui32Version The file version.
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _wewoOptions The WAV Editor window settings into which to load the settings data.
		 * \return Returns true if the settings data was loaded.
		 */
		bool									LoadWavEditorWindowSettings( uint32_t _ui32Version, CStream &_sFile, LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions );

		/**
		 * Saves WAV Editor window settings.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _wewoOptions The WAV Editor window settings to write to the settings data.
		 * \return Returns true if the settings data was saved.
		 */
		bool									SaveWavEditorWindowSettings( CStream &_sFile, const LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions );

		/**
		 * Loads palette settings.
		 *
		 * \param _ui32Version The file version.
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _poOptions The palette settings into which to load the settings data.
		 * \return Returns true if the settings data was loaded.
		 */
		bool									LoadPaletteSettings( uint32_t _ui32Version, CStream &_sFile, LSN_PALETTE_OPTIONS &_poOptions );

		/**
		 * Saves palette settings.
		 *
		 * \param _sFile The in-memory stream of the settings file.
		 * \param _poOptions The palette settings to write to the settings data.
		 * \return Returns true if the settings data was saved.
		 */
		bool									SavePaletteSettings( CStream &_sFile, const LSN_PALETTE_OPTIONS &_poOptions );

		/**
		 * Adds or move to the top a given file path.
		 * 
		 * \param _s16Path The file path to add or move to the top.
		 **/
		void									AddPath( const std::u16string &_s16Path );
	};

}	// namespace lsn
