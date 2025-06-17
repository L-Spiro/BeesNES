/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles all emulator functionality.
 */

#include "LSNBeesNes.h"
#include "../File/LSNStdFile.h"
#include <filesystem>


namespace lsn {

	/** The raw WAV stream. */
	CWavFile CBeesNes::m_wfRawStream;
	
	/** The output-capture WAV stream. */
	CWavFile CBeesNes::m_wfOutStream;

	CBeesNes::CBeesNes( CDisplayHost * _pdhDisplayHost, CInputPoller * _pipPoller ) :
		m_dScale( 3.0 ),
		m_dRatio( 4.0 / 3.0 ),
		m_dRatioActual( 4.0 / 3.0 ),
		//m_oOptions.fFilter( CFilterBase::LSN_F_AUTO_CRT_FULL ),
		//m_ppPostProcess( CPostProcessBase::LSN_PP_BILINEAR ),
		m_pmSystem( LSN_PM_NTSC ),
		m_pdhDisplayHost( _pdhDisplayHost ),
		m_pipPoller( _pipPoller ),
		m_ui32RecentLimit( 13 * 4 ) {

		std::memset( m_ui8RapidFires, 0, sizeof( m_ui8RapidFires ) );

		CUtilities::GenGaussianNoise( 0.0125f );
		CAudio::SetOutputSettings( Options().aoGlobalAudioOptions );
		
		//m_pfbFilterTable
		CFilterBase * pfbTmp[CFilterBase::LSN_F_TOTAL][LSN_PM_CONSOLE_TOTAL] = {
			//LSN_PM_NTSC					LSN_PM_PAL						LSN_PM_DENDY					LSN_PM_PALM						LSN_PM_PALN
			{ &m_r24fRgb24Filter,			&m_r24fRgb24Filter,				&m_r24fRgb24Filter,				&m_r24fRgb24Filter,				&m_r24fRgb24Filter },				// LSN_F_RGB24
			{ &m_nbfBlarggNtscFilter,		&m_nbfBlarggNtscFilter,			&m_nbfBlarggNtscFilter,			&m_nbfBlarggNtscFilter,			&m_nbfBlarggNtscFilter },			// LSN_F_NTSC_BLARGG
			{ &m_nbfBlarggPalFilter,		&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter },			// LSN_F_PAL_BLARGG
			{ &m_nbfLSpiroNtscFilter,		&m_nbfLSpiroNtscFilter,			&m_nbfLSpiroNtscFilter,			&m_nbfLSpiroNtscFilter,			&m_nbfLSpiroNtscFilter },			// LSN_F_NTSC_LSPIRO
			{ &m_nbfLSpiroPalFilter,		&m_nbfLSpiroPalFilter,			&m_nbfLSpiroPalFilter,			&m_nbfLSpiroPalFilter,			&m_nbfLSpiroPalFilter },			// LSN_F_PAL_LSPIRO
			{ &m_nbfLSpiroDendyFilter,		&m_nbfLSpiroDendyFilter,		&m_nbfLSpiroDendyFilter,		&m_nbfLSpiroDendyFilter,		&m_nbfLSpiroDendyFilter },			// LSN_F_DENDY_LSPIRO
			{ &m_nbfLSpiroPalMFilter,		&m_nbfLSpiroPalMFilter,			&m_nbfLSpiroPalMFilter,			&m_nbfLSpiroPalMFilter,			&m_nbfLSpiroPalMFilter },			// LSN_F_PALM_LSPIRO
			{ &m_nbfLSpiroPalNFilter,		&m_nbfLSpiroPalNFilter,			&m_nbfLSpiroPalNFilter,			&m_nbfLSpiroPalNFilter,			&m_nbfLSpiroPalNFilter },			// LSN_F_PALN_LSPIRO
			{ &m_ncfEmmirNtscFullFilter,	&m_ncfEmmirNtscFullFilter,		&m_ncfEmmirNtscFullFilter,		&m_ncfEmmirNtscFullFilter,		&m_ncfEmmirNtscFullFilter },		// LSN_F_NTSC_CRT_FULL
			{ &m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter },			// LSN_F_PAL_CRT_FULL
			{ &m_nbfBlarggNtscFilter,		&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter },			// LSN_F_AUTO_BLARGG
			{ &m_ncfEmmirNtscFullFilter,	&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter },			// LSN_F_AUTO_CRT_FULL
			{ &m_nbfLSpiroNtscFilter,		&m_nbfLSpiroPalFilter,			&m_nbfLSpiroDendyFilter,		&m_nbfLSpiroPalMFilter,			&m_nbfLSpiroPalNFilter },			// LSN_F_AUTO_LSPIRO
		};
		m_nbfLSpiroDendyFilter.SetGamma( 2.35f );
		m_nbfLSpiroPalMFilter.SetPixelToSignal( 8 );
		m_nbfLSpiroPalMFilter.SetGamma( 1.0f / 0.45f );
		m_nbfLSpiroPalNFilter.SetPixelToSignal( 8 );
		m_nbfLSpiroPalNFilter.SetFilterFunc( CUtilities::BoxFilterFunc );
		m_nbfLSpiroPalNFilter.SetGamma( 2.5f );

		// FPS settings.
		m_nbfLSpiroNtscFilter.SetFps( 60.098813897440515529533511098629f );
		m_nbfLSpiroPalFilter.SetFps( 50.006978908188585607940446650124f );
		m_nbfLSpiroDendyFilter.SetFps( 50.006978908188585607940446650124f );
		m_nbfLSpiroPalMFilter.SetFps( 60.032435273083568398202053145976f );
		m_nbfLSpiroPalNFilter.SetFps( 50.502710495150011279043537108053f );
		m_nbfLSpiroDendyFilter.SetPhosphorDecayPeriod( 2.0f );
		m_nbfLSpiroPalNFilter.SetPhosphorDecayPeriod( 1.9f );
		


		// Lowest quality.
		m_nbfLSpiroNtscFilter.SetWidthScale( 2 );
		m_nbfLSpiroNtscFilter.SetFilterFuncY( CUtilities::BoxFilterFunc );
		m_nbfLSpiroNtscFilter.SetFilterFunc( CUtilities::BoxFilterFunc );

		m_nbfLSpiroPalFilter.SetWidthScale( 2 );
		m_nbfLSpiroDendyFilter.SetWidthScale( 2 );
		m_nbfLSpiroPalMFilter.SetWidthScale( 2 );
		m_nbfLSpiroPalNFilter.SetWidthScale( 2 );

		// SSE 4.1 or NEON.
		if ( CUtilities::IsSse4Supported() ) {
			m_nbfLSpiroNtscFilter.SetWidthScale( 4 );
			m_nbfLSpiroNtscFilter.SetFilterFunc( CUtilities::BoxFilterFunc );

			m_nbfLSpiroPalFilter.SetWidthScale( 5 );
			m_nbfLSpiroDendyFilter.SetWidthScale( 5 );
			m_nbfLSpiroPalMFilter.SetWidthScale( 4 );
			m_nbfLSpiroPalNFilter.SetWidthScale( 4 );
		}
		//if ( CUtilities::IsAvxSupported() ) {
		//	m_nbfLSpiroNtscFilter.SetWidthScale( 8 );
		//	m_nbfLSpiroNtscFilter.SetKernelSize( 16 );
		//	m_nbfLSpiroNtscFilter.SetFilterFuncY( &CUtilities::CrtHumpFunc<1200, 480> );
		//	m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::CrtHumpFunc<1200, 480> );
		//	/*m_nbfLSpiroNtscFilter.SetKernelSize( 48 );
		//	m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::CardinalSplineUniformFilterFunc );*/
		//	/*m_nbfLSpiroNtscFilter.SetKernelSize( 48 );
		//	m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::GaussianXFilterFunc );*/


		//	m_nbfLSpiroPalFilter.SetWidthScale( 10 );
		//	m_nbfLSpiroPalFilter.SetKernelSize( 16 );
		//	m_nbfLSpiroPalFilter.SetFilterFuncY( &CUtilities::CrtHumpFunc<1200, 480> );
		//	m_nbfLSpiroPalFilter.SetFilterFunc( &CUtilities::CrtHumpFunc<1200, 480> );
		//	/*m_nbfLSpiroPalFilter.SetKernelSize( 48 );
		//	m_nbfLSpiroPalFilter.SetFilterFunc( &CUtilities::CardinalSplineUniformFilterFunc );*/
		//	

		//	m_nbfLSpiroDendyFilter.SetWidthScale( 10 );
		//	m_nbfLSpiroDendyFilter.SetKernelSize( 16 );
		//	m_nbfLSpiroDendyFilter.SetFilterFuncY( &CUtilities::CrtHumpFunc<1200, 480> );
		//	m_nbfLSpiroDendyFilter.SetFilterFunc( &CUtilities::CrtHumpFunc<1200, 480> );

		//	m_nbfLSpiroPalNFilter.SetWidthScale( 8 );
		//	m_nbfLSpiroPalNFilter.SetKernelSize( 16 );
		//	m_nbfLSpiroPalNFilter.SetFilterFuncY( &CUtilities::CrtHumpFunc<1200, 640> );
		//	m_nbfLSpiroPalNFilter.SetFilterFunc( &CUtilities::CrtHumpFunc<1200, 640> );

		//	m_nbfLSpiroPalMFilter.SetWidthScale( 8 );
		//	m_nbfLSpiroPalNFilter.SetKernelSize( 16 );
		//	m_nbfLSpiroPalNFilter.SetFilterFuncY( &CUtilities::CrtHumpFunc<1200, 640> );
		//	m_nbfLSpiroPalNFilter.SetFilterFunc( &CUtilities::CrtHumpFunc<1200, 640> );
		//}
		if ( CUtilities::IsAvx512FSupported() || CUtilities::IsAvxSupported() ) {
			m_nbfLSpiroNtscFilter.SetWidthScale( 8 );
			/*m_nbfLSpiroNtscFilter.SetKernelSize( 25 );
			m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::BartlettFilterFunc );*/
			m_nbfLSpiroNtscFilter.SetKernelSize( 48 );
			//m_nbfLSpiroNtscFilter.SetKernelSize( 64 );
			/*m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::LanczosXFilterFunc<11, 4> );
			m_nbfLSpiroNtscFilter.SetFilterFuncY( &CUtilities::GaussianXFilterFunc );*/
			m_nbfLSpiroNtscFilter.SetFilterFuncY( &CUtilities::LanczosXFilterFunc<100, 40> );
			//m_nbfLSpiroNtscFilter.SetFilterFuncY( &CUtilities::CardinalSplineUniformFilterFunc );
			//m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::GaussianXFilterFunc );
			//m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::CrtHumpFunc<2350, 1> );
			/*m_nbfLSpiroNtscFilter.SetKernelSize( 61 );
			m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::LanczosXFilterFunc<14, 4> );*/
			/*m_nbfLSpiroNtscFilter.SetKernelSize( 48 );
			m_nbfLSpiroNtscFilter.SetFilterFunc( &CUtilities::GaussianXFilterFunc );*/

			m_nbfLSpiroPalFilter.SetWidthScale( 10 );
			m_nbfLSpiroPalFilter.SetKernelSize( 48 );
			m_nbfLSpiroPalFilter.SetFilterFuncY( &CUtilities::LanczosXFilterFunc<5, 2> );
			m_nbfLSpiroPalFilter.SetFilterFunc( &CUtilities::GaussianXFilterFunc );

			m_nbfLSpiroDendyFilter.SetWidthScale( 10 );
			m_nbfLSpiroDendyFilter.SetKernelSize( 44 );
			m_nbfLSpiroDendyFilter.SetFilterFuncY( &CUtilities::CardinalSplineUniformFilterFunc );
			m_nbfLSpiroDendyFilter.SetFilterFunc( &CUtilities::GaussianXFilterFunc );

			m_nbfLSpiroPalMFilter.SetWidthScale( 8 );
			m_nbfLSpiroPalMFilter.SetKernelSize( 46 );
			m_nbfLSpiroPalMFilter.SetFilterFuncY( &CUtilities::CardinalSplineUniformFilterFunc );
			m_nbfLSpiroPalMFilter.SetFilterFunc( &CUtilities::GaussianXFilterFunc );

			m_nbfLSpiroPalNFilter.SetWidthScale( 8 );
			m_nbfLSpiroPalNFilter.SetKernelSize( 48 );
			m_nbfLSpiroPalNFilter.SetFilterFuncY( &CUtilities::GaussianXFilterFunc );
			m_nbfLSpiroPalNFilter.SetFilterFunc( &CUtilities::GaussianXFilterFunc );

			
		}


		std::memcpy( m_pfbFilterTable, pfbTmp, sizeof( pfbTmp ) );

		m_pppbPostTable[CPostProcessBase::LSN_PP_NONE] = &m_ppbNoPostProcessing;
		m_pppbPostTable[CPostProcessBase::LSN_PP_BLEED] = &m_bppBleedPostProcessing;
		m_pppbPostTable[CPostProcessBase::LSN_PP_BILINEAR] = &m_blppBiLinearPost;
		m_pppbPostTable[CPostProcessBase::LSN_PP_SRGB] = &m_sppLinearTosRGBPost;
		//m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_BLEED );
		m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_BILINEAR );
		//m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_SRGB );


		m_psbSystems[LSN_PM_NTSC] = &m_nsNtscSystem;
		m_psbSystems[LSN_PM_PAL] = &m_nsPalSystem;
		m_psbSystems[LSN_PM_DENDY] = &m_nsDendySystem;
		m_psbSystems[LSN_PM_PALM] = &m_nsPalMSystem;
		m_psbSystems[LSN_PM_PALN] = &m_nsPalNSystem;


		m_psbSystem = m_psbSystems[m_pmSystem];


		const size_t stBuffers = 3;
		m_r24fRgb24Filter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroPalFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroDendyFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroPalMFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroPalNFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBlarggNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBlarggPalFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirNtscFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirPalFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );

		UpdateCurrentSystem();
	}
	CBeesNes::~CBeesNes() {
		if ( m_psbSystem && !m_psbSystem->CloseRom() ) {
		}
	}

	// == Functions.
	/**
	 * Loads the settings file.
	 *
	 * \return Returns true if the settings file was loaded.
	 */
	bool CBeesNes::LoadSettings() {
		std::wstring wsPath = m_wsFolder + L"Settings.lsn";
		CStdFile sfFile;
		if ( sfFile.Open( reinterpret_cast<const char16_t *>(wsPath.c_str()) ) ) {
			std::vector<uint8_t> vFile;
			if ( sfFile.LoadToMemory( vFile ) ) {
				sfFile.Close();
				CStream sStream( vFile );
				return LoadSettings( sStream );
			}
		}
		return false;
	}

	/**
	 * Saves the settings file.
	 *
	 * \return Returns true if the settings file was created.
	 */
	bool CBeesNes::SaveSettings() {
		std::vector<uint8_t> vFile;
		CStream sStream( vFile );
		if ( !SaveSettings( sStream ) ) { return false; }
		std::wstring wsPath = m_wsFolder + L"Settings.lsn";
		CStdFile sfFile;
		if ( !sfFile.Create( reinterpret_cast<const char16_t *>(wsPath.c_str()) ) ) { return false; }
		return sfFile.WriteToFile( vFile );
	}

	/**
	 * Loads the per-game settings from the given path.
	 * 
	 * \param _u16Path The path from which to load per-game settings.
	 * \return Returns true if the file was found and the settings were loaded from it.
	 **/
	bool CBeesNes::LoadPerGameSettings( const std::u16string &_u16Path ) {
		Options().aoThisGameAudioOptions = Options().aoGlobalAudioOptions;
		Options().aoThisGameAudioOptions.bUseGlobal = true;
		Options().ioThisGameInputOptions = Options().ioGlobalInputOptions;
		Options().ioThisGameInputOptions.bUseGlobal = true;
		if ( _u16Path.size() ) {
			CStdFile sfFile;
			if ( sfFile.Open( _u16Path.c_str() ) ) {
				std::vector<uint8_t> vFile;
				if ( sfFile.LoadToMemory( vFile ) ) {
					sfFile.Close();
					CStream sStream( vFile );
					uint32_t ui32Version = 0;
					if ( !sStream.Read( ui32Version ) ) { return false; }
					if ( !LoadAudioSettings( ui32Version, sStream, Options().aoThisGameAudioOptions ) ) { return false; }
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * Saves the per-game settings to a given path.
	 * 
	 * \param _u16Path The path to which to save per-game settings.
	 * \return Returns true if the file was created and fully written.
	 **/
	bool CBeesNes::SavePerGameSettings( const std::u16string &_u16Path ) {
		std::vector<uint8_t> vFile;
		CStream sStream( vFile );
		uint32_t ui32Version = 0;
		if ( !sStream.Write( ui32Version ) ) { return false; }
		if ( !SaveAudioSettings( sStream, Options().aoThisGameAudioOptions ) ) { return false; }

		CStdFile sfFile;
		if ( !sfFile.Create( _u16Path.c_str() ) ) { return false; }
		return sfFile.WriteToFile( vFile );
	}

	/**
	 * Sets the current filter.  Changes are not applied to rendering output until Swap() is called.
	 *
	 * \param _fFilter The new filter to be applied after the next Swap().
	 */
	void CBeesNes::SetCurFilter( CFilterBase::LSN_FILTERS _fFilter ) {
		if ( m_oOptions.fFilter != _fFilter ) {
			CFilterBase * pfbPrev = m_pfbFilterTable[m_oOptions.fFilter][GetCurPpuRegion()];
			m_oOptions.fFilter = _fFilter;
			m_cfartCurFilterAndTargets.pfbNextFilter = m_pfbFilterTable[m_oOptions.fFilter][GetCurPpuRegion()];
			if ( pfbPrev != m_cfartCurFilterAndTargets.pfbNextFilter ) {
				m_cfartCurFilterAndTargets.pfbNextFilter->Activate();
			}
		}
	}

	/**
	 * Gets the current region.
	 *
	 * \return Returns the current region.
	 */
	LSN_PPU_METRICS CBeesNes::GetCurPpuRegion() const {
		return m_psbSystem->GetDisplayClient()->PpuRegion();
	}

	/**
	 * If the render buffer is dirty, a render is performed (PPU buffer -> Filters -> Post-Processing).
	 *	This should be called inside the same critical section/mutex that calls Swap().
	 *
	 * \param _ui32FinalW The final display width.
	 * \param _ui32FinalH The final display height.
	 */
	void CBeesNes::Render( uint32_t _ui32FinalW, uint32_t _ui32FinalH ) {
		if ( m_cfartCurFilterAndTargets.bDirty && m_cfartCurFilterAndTargets.pfbPrevFilter ) {
			m_cfartCurFilterAndTargets.bDirty = false;
			m_cfartCurFilterAndTargets.pui8LastFilteredResult = m_cfartCurFilterAndTargets.pfbPrevFilter->ApplyFilter( m_cfartCurFilterAndTargets.pui8CurRenderTarget,
				m_cfartCurFilterAndTargets.ui32Width, m_cfartCurFilterAndTargets.ui32Height, m_cfartCurFilterAndTargets.ui16Bits, m_cfartCurFilterAndTargets.ui32Stride,
				m_cfartCurFilterAndTargets.ui64Frame, m_cfartCurFilterAndTargets.ui64RenderStartCycle );

			for ( size_t I = 0; I < m_vPostProcesses.size(); ++I ) {
				m_cfartCurFilterAndTargets.pui8LastFilteredResult = m_pppbPostTable[m_vPostProcesses[I]]->ApplyFilter( m_cfartCurFilterAndTargets.pui8LastFilteredResult,
					_ui32FinalW, _ui32FinalH, m_cfartCurFilterAndTargets.bMirrored,
					m_cfartCurFilterAndTargets.ui32Width, m_cfartCurFilterAndTargets.ui32Height, m_cfartCurFilterAndTargets.ui16Bits, m_cfartCurFilterAndTargets.ui32Stride,
					m_cfartCurFilterAndTargets.ui64Frame, m_cfartCurFilterAndTargets.ui64RenderStartCycle );
			}
		}
	}

	/**
	 * Swaps PPU render targets and switches to the next filter if a new filter has been set.
	 *	Should be called inside the same citical section/mutex as Render().
	 */
	void CBeesNes::Swap() {
		m_cfartCurFilterAndTargets.pfbPrevFilter = m_cfartCurFilterAndTargets.pfbCurFilter;
		m_cfartCurFilterAndTargets.pui8CurRenderTarget = m_cfartCurFilterAndTargets.pfbCurFilter->OutputBuffer();
		m_cfartCurFilterAndTargets.ui16Bits = uint16_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputBits() );
		m_cfartCurFilterAndTargets.ui32Width = m_cfartCurFilterAndTargets.pfbCurFilter->OutputWidth();
		m_cfartCurFilterAndTargets.ui32Height = m_cfartCurFilterAndTargets.pfbCurFilter->OutputHeight();
		m_cfartCurFilterAndTargets.ui32Stride = uint32_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride() );
		m_cfartCurFilterAndTargets.ui64Frame = GetDisplayClient()->FrameCount();
		m_cfartCurFilterAndTargets.ui64RenderStartCycle = GetDisplayClient()->GetRenderStartCycle();
		m_cfartCurFilterAndTargets.bDirty = true;
		m_cfartCurFilterAndTargets.bMirrored = m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput();

		m_cfartCurFilterAndTargets.pfbCurFilter = m_cfartCurFilterAndTargets.pfbNextFilter;
			
		m_cfartCurFilterAndTargets.pfbCurFilter->Swap();
		GetDisplayClient()->SetRenderTarget( m_cfartCurFilterAndTargets.pfbCurFilter->CurTarget(), m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride(), m_cfartCurFilterAndTargets.pfbCurFilter->InputFormat(), m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput() );
	}

	/**
	 * Loads a ROM.
	 *
	 * \param _vRom The in-memory ROM file.
	 * \param _s16Path The full path to the ROM.
	 * \param _pmRegion The region to use when loading the ROM.  If LSN_PM_UNKNOWN, the ROM data is used to determine the region.
	 * \return Returns true if the ROM was loaded successfully.
	 */
	bool CBeesNes::LoadRom( const std::vector<uint8_t> &_vRom, const std::u16string &_s16Path, LSN_PPU_METRICS _pmRegion ) {
		CloseRom();
		LSN_ROM rTmp;
		if ( m_u16PerGameSettings.size() ) {
			SavePerGameSettings( m_u16PerGameSettings );
			m_u16PerGameSettings.clear();
		}
		if ( CSystemBase::LoadRom( _vRom, rTmp, _s16Path ) ) {
			//m_psbSystem.reset();
			LSN_PPU_METRICS pmReg = _pmRegion;
			m_pmSystem = pmReg;
			if ( m_pmSystem == LSN_PPU_METRICS::LSN_PM_UNKNOWN ) {
				m_pmSystem = rTmp.riInfo.pmConsoleRegion;
			}
			if ( m_pmSystem == LSN_PPU_METRICS::LSN_PM_UNKNOWN ) {
				m_pmSystem = LSN_PPU_METRICS::LSN_PM_NTSC;
			}
			for ( auto I = LSN_ELEMENTS( m_psbSystems ); I--; ) {
				if ( I != size_t( m_pmSystem ) ) {
					m_psbSystems[I]->SetAsInactive();
				}
			}
			m_psbSystem = m_psbSystems[m_pmSystem];
			rTmp.wsSaveFolder = m_wsFolder + L"GameSettings\\";
			rTmp.u16SaveFilePrefix = CUtilities::PerRomSettingsPath( rTmp.wsSaveFolder, rTmp.riInfo.ui32Crc, rTmp.riInfo.s16RomName );
			m_u16PerGameSettings = rTmp.u16SaveFilePrefix;
			if ( rTmp.u16SaveFilePrefix.size() ) {
				std::filesystem::path pPath( rTmp.u16SaveFilePrefix );
				rTmp.u16SaveFilePrefix = pPath.replace_extension( "" ).generic_u16string();
			}
			
			LoadPerGameSettings( m_u16PerGameSettings );
			UpdateCurrentSystem();
			if ( m_psbSystem->LoadRom( rTmp ) ) {
				//m_psbSystem->ResetState( false );
				AddPath( _s16Path );
				return true;
			}
		}
		return false;
	}

	/**
	 * Closes the current ROM.
	 **/
	void CBeesNes::CloseRom() {
		if ( m_psbSystem && !m_psbSystem->CloseRom() ) {
		}
	}

	/**
	 * Pauses the current ROM.
	 **/
	void CBeesNes::PauseRom() {
		if LSN_LIKELY( m_psbSystem ) {
			m_psbSystem->PauseRom();
		}
	}

	/**
	 * Unpauses the current ROM.
	 **/
	void CBeesNes::UnpauseRom() {
		if LSN_LIKELY( m_psbSystem ) {
			m_psbSystem->UnpauseRom();
		}
	}

	/**
	 * Toggles the current ROM's pause state.
	 **/
	void CBeesNes::TogglePauseRom() {
		if LSN_LIKELY( m_psbSystem ) {
			m_psbSystem->TogglePauseRom();
		}
	}

	/**
	 * Determines whether the ROM is paused or not.
	 * 
	 * \return Returns true if the ROM is paused.
	 **/
	bool CBeesNes::RomIsPaused() const {
		if LSN_LIKELY( m_psbSystem ) {
			return m_psbSystem->RomIsPaused();
		}
		return true;
	}

	/**
	 * Resets the ROM.
	 **/
	void CBeesNes::ResetRom() {
		if LSN_LIKELY( m_psbSystem ) {
			m_psbSystem->ResetRom();
		}
	}

	/**
	 * Resets the ROM.
	 **/
	void CBeesNes::PowerCycle() {
		if LSN_LIKELY( m_psbSystem ) {
			m_psbSystem->PowerCycle();
		}
	}

	/**
	 * Applies the current audio options.
	 **/
	void CBeesNes::ApplyAudioOptions() {
		LSN_AUDIO_OPTIONS & aoOptions = Options().aoThisGameAudioOptions.bUseGlobal ? Options().aoGlobalAudioOptions : Options().aoThisGameAudioOptions;
		CAudio::SetOutputSettings( aoOptions );
		if ( m_psbSystem ) {
			Options().stfStreamOptionsOutCapture.ui32Hz = aoOptions.ui32OutputHz;
			Options().stfStreamOptionsRaw.ui32Hz = uint32_t( std::ceil( m_psbSystem->GetApuHz() ) );
			Options().stfStreamOptionsOutCapture.wsMetaPath = Options().stfStreamOptionsOutCapture.wsPath + L".txt";
			Options().stfStreamOptionsRaw.wsMetaPath = Options().stfStreamOptionsRaw.wsPath + L".txt";

			auto pfTmp = Options().stfStreamOptionsOutCapture.pfMetaFunc;
			Options().stfStreamOptionsOutCapture.pfMetaFunc = nullptr;
			Options().stfStreamOptionsOutCapture.pfMetaThreadFunc = nullptr;
			//m_psbSystem->SetMetaDataStreamParms( Options().stfStreamOptionsOutCapture.pvMetaParm, Options().stfStreamOptionsOutCapture.pfMetaFunc, pfTmp, Options().stfStreamOptionsOutCapture.pfMetaThreadFunc );
			m_psbSystem->SetMetaDataStreamParms( Options().stfStreamOptionsRaw.pvMetaParm, pfTmp, Options().stfStreamOptionsRaw.pfMetaFunc, Options().stfStreamOptionsRaw.pfMetaThreadFunc );
			m_psbSystem->SetAudioOptions( aoOptions );
			SetStreamToFileOptions<true>( Options().stfStreamOptionsRaw );
			SetStreamToFileOptions<false>( Options().stfStreamOptionsOutCapture );

			m_psbSystem->SetRawStream( Options().stfStreamOptionsRaw.bEnabled ? &m_wfRawStream : nullptr );
			m_psbSystem->SetOutStream( Options().stfStreamOptionsOutCapture.bEnabled ? &m_wfOutStream : nullptr );
			
		}

		if ( m_u16PerGameSettings.size() ) {
			SavePerGameSettings( m_u16PerGameSettings );
		}
	}

	/**
	 * Updates the current system with render information, display hosts, etc.
	 */
	void CBeesNes::UpdateCurrentSystem() {
		if ( !GetDisplayClient() ) { return; }
		GetDisplayClient()->SetDisplayHost( m_pdhDisplayHost );

		// Set ratios.
		m_dRatioActual = GetDisplayClient()->DisplayRatio();

		// Prepare filters.
		m_cfartCurFilterAndTargets.pfbCurFilter = m_pfbFilterTable[m_oOptions.fFilter][GetDisplayClient()->PpuRegion()];
					
		m_cfartCurFilterAndTargets.pui8CurRenderTarget = m_cfartCurFilterAndTargets.pfbCurFilter->OutputBuffer();
		m_cfartCurFilterAndTargets.ui16Bits = uint16_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputBits() );
		m_cfartCurFilterAndTargets.ui32Width = m_cfartCurFilterAndTargets.pfbCurFilter->OutputWidth();
		m_cfartCurFilterAndTargets.ui32Height = m_cfartCurFilterAndTargets.pfbCurFilter->OutputHeight();
		m_cfartCurFilterAndTargets.ui32Stride = uint32_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride() );
		m_cfartCurFilterAndTargets.ui64Frame = GetDisplayClient()->FrameCount();
		m_cfartCurFilterAndTargets.ui64RenderStartCycle = GetDisplayClient()->GetRenderStartCycle();
		m_cfartCurFilterAndTargets.bDirty = true;
		m_cfartCurFilterAndTargets.bMirrored = m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput();
		m_cfartCurFilterAndTargets.pui8LastFilteredResult = nullptr;

		m_cfartCurFilterAndTargets.pfbNextFilter = m_cfartCurFilterAndTargets.pfbCurFilter;
		m_cfartCurFilterAndTargets.pfbPrevFilter = m_cfartCurFilterAndTargets.pfbCurFilter;

		m_cfartCurFilterAndTargets.pfbCurFilter->Swap();
		GetDisplayClient()->SetRenderTarget( m_cfartCurFilterAndTargets.pfbCurFilter->CurTarget(), m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride(), m_cfartCurFilterAndTargets.pfbCurFilter->InputFormat(), m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput() );

		// Set up input.
		m_psbSystem->SetInputPoller( m_pipPoller );

		ApplyAudioOptions();
	}

	/**
	 * Loads the settings file.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings file was loaded.
	 */
	bool CBeesNes::LoadSettings( CStream &_sFile ) {
		// Defaults.
#ifdef LSN_WINDOWS
		m_oOptions.wpMainWindowPlacement = { 0 };
#endif	// #ifdef LSN_WINDOWS

		// Get the file version.
		uint32_t ui32Version;
		if ( !_sFile.ReadUi32( ui32Version ) ) { return false; }

		if ( !LoadInputSettings( ui32Version, _sFile, m_oOptions.ioGlobalInputOptions ) ) { return false; }
		if ( !LoadRecentFiles( ui32Version, _sFile ) ) { return false; }

		if ( !_sFile.ReadStringU16( m_oOptions.wInRomInitPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wOutRomInitPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wPatchInitPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wDefaultRomPath ) ) { return false; }

		if ( !_sFile.Read( m_oOptions.pmRegion ) ) { return false; }

		if ( !LoadAudioSettings( ui32Version, _sFile, m_oOptions.aoGlobalAudioOptions ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wRawAudioPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wOutAudioPath ) ) { return false; }

		if ( !LoadAudioStreamSettings( ui32Version, _sFile, m_oOptions.stfStreamOptionsRaw ) ) { return false; }
		if ( !LoadAudioStreamSettings( ui32Version, _sFile, m_oOptions.stfStreamOptionsOutCapture ) ) { return false; }
		
		try {
			auto aSize = m_oOptions.vRawStartHistory.size();
			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vRawStartHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vRawStartHistory[I] ) ) { return false; }
			}
			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vRawEndHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vRawEndHistory[I] ) ) { return false; }
			}

			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vOutStartHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vOutStartHistory[I] ) ) { return false; }
			}
			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vOutEndHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vOutEndHistory[I] ) ) { return false; }
			}
		}
		catch ( ... ) { return false; }

		if ( !_sFile.Read( m_oOptions.fFilter ) ) { return false; }

#ifdef LSN_WINDOWS
		if ( !_sFile.Read( m_oOptions.wpMainWindowPlacement ) ) { return false; }
#endif	// #ifdef LSN_WINDOWS
		
		if ( !LoadWavEditorWindowSettings( ui32Version, _sFile, m_oOptions.wewoWavEditorWindow ) ) { return false; }

		return true;
	}

	/**
	 * Saves the settings file.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings file was saved.
	 */
	bool CBeesNes::SaveSettings( CStream &_sFile ) {
		if ( !_sFile.WriteUi32( LSN_BEESNES_SETTINGS_VERSION ) ) { return false; }
		if ( !SaveInputSettings( _sFile, m_oOptions.ioGlobalInputOptions ) ) { return false; }
		if ( !SaveRecentFiles( _sFile ) ) { return false; }

		if ( !_sFile.WriteStringU16( m_oOptions.wInRomInitPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wOutRomInitPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wPatchInitPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wDefaultRomPath ) ) { return false; }

		if ( !_sFile.Write( m_oOptions.pmRegion ) ) { return false; }

		if ( !SaveAudioSettings( _sFile, m_oOptions.aoGlobalAudioOptions ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wRawAudioPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wOutAudioPath ) ) { return false; }
		
		if ( !SaveAudioStreamSettings( _sFile, m_oOptions.stfStreamOptionsRaw ) ) { return false; }
		if ( !SaveAudioStreamSettings( _sFile, m_oOptions.stfStreamOptionsOutCapture ) ) { return false; }
		
		if ( !_sFile.Write( m_oOptions.vRawStartHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vRawStartHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vRawStartHistory[I] ) ) { return false; }
		}
		if ( !_sFile.Write( m_oOptions.vRawEndHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vRawEndHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vRawEndHistory[I] ) ) { return false; }
		}

		if ( !_sFile.Write( m_oOptions.vOutStartHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vOutStartHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vOutStartHistory[I] ) ) { return false; }
		}
		if ( !_sFile.Write( m_oOptions.vOutEndHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vOutEndHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vOutEndHistory[I] ) ) { return false; }
		}

		if ( !_sFile.Write( m_oOptions.fFilter ) ) { return false; }

#ifdef LSN_WINDOWS
		if ( !_sFile.Write( m_oOptions.wpMainWindowPlacement ) ) { return false; }
#endif	// #ifdef LSN_WINDOWS
		
		if ( !SaveWavEditorWindowSettings( _sFile, m_oOptions.wewoWavEditorWindow ) ) { return false; }
		return true;
	}

	/**
	 * Loads input settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _ioInputOptions The input options into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadInputSettings( uint32_t /*_ui32Version*/, CStream &_sFile, LSN_INPUT_OPTIONS &_ioInputOptions ) {
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8ConsoleType ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Expansion ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[0] ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[1] ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[2] ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[3] ) ) { return false; }
		if ( !_sFile.ReadBool( _ioInputOptions.bUseFourScore ) ) { return false; }
		return true;
	}

	/**
	 * Saves input settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _ioInputOptions The input options to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveInputSettings( CStream &_sFile, LSN_INPUT_OPTIONS &_ioInputOptions ) {
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8ConsoleType ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Expansion ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[0] ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[1] ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[2] ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[3] ) ) { return false; }
		if ( !_sFile.WriteBool( _ioInputOptions.bUseFourScore ) ) { return false; }
		return true;
	}

	/**
	 * Loads the recent-files list.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadRecentFiles( uint32_t /*_ui32Version*/, CStream &_sFile ) {
		m_vRecentFiles.clear();
		uint8_t ui8Total = 0;
		if ( !_sFile.ReadUi8( ui8Total ) ) { return false; }
		m_ui32RecentLimit = ui8Total;
		if ( !_sFile.ReadUi8( ui8Total ) ) { return false; }
		std::u16string u16Tmp;
		for ( uint8_t I = 0; I < ui8Total; ++I ) {
			if ( !_sFile.ReadStringU16( u16Tmp ) ) { return false; }
			AddPath( u16Tmp );
		}
		return true;
	}

	/**
	 * Saves the recent-files list.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveRecentFiles( CStream &_sFile ) {
		uint8_t ui8Total = uint8_t( std::min( m_ui32RecentLimit, uint32_t( 100 ) ) );
		if ( !_sFile.WriteUi8( ui8Total ) ) { return false; }

		ui8Total = uint8_t( std::min( m_vRecentFiles.size(), size_t( ui8Total ) ) );
		if ( !_sFile.WriteUi8( ui8Total ) ) { return false; }
		for ( uint8_t I = ui8Total; I--; ) {
			if ( !_sFile.WriteStringU16( m_vRecentFiles[I] ) ) { return false; }
		}
		return true;
	}

	/**
	 * Loads audio settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _aoAudioOptions The audio options into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadAudioSettings( uint32_t /*_ui32Version*/, CStream &_sFile, LSN_AUDIO_OPTIONS &_aoAudioOptions ) {
		_aoAudioOptions = LSN_AUDIO_OPTIONS();	// Set default values.
		if ( !_sFile.Read( _aoAudioOptions.afFormat.sfFormat ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.ui32OutputHz ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.bUseGlobal ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.bEnabled ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.fVolume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.fBgVol ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bLpfEnable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fLpf ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bHpf0Enable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fHpf0 ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bHpf1Enable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fHpf1 ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bHpf2Enable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fHpf2 ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fVolume ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fP1Volume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fP2Volume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fTVolume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fNVolume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fDmcVolume ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bNoise ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bInvert ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bRp2A02 ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fmFilterMode ) ) { return false; }
		return true;
	}

	/**
	 * Saves audio settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _aoAudioOptions The audio options to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveAudioSettings( CStream &_sFile, LSN_AUDIO_OPTIONS &_aoAudioOptions ) {
		if ( !_sFile.Write( _aoAudioOptions.afFormat.sfFormat ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.ui32OutputHz ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.bUseGlobal ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.bEnabled ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.fVolume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.fBgVol ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bLpfEnable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fLpf ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bHpf0Enable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fHpf0 ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bHpf1Enable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fHpf1 ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bHpf2Enable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fHpf2 ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fVolume ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fP1Volume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fP2Volume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fTVolume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fNVolume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fDmcVolume ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bNoise ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bInvert ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bRp2A02 ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fmFilterMode ) ) { return false; }
		return true;
	}

	/**
	 * Loads audio stream-to-file settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _stfoAudioOptions The audio options into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadAudioStreamSettings( uint32_t /*_ui32Version*/, CStream &_sFile, CWavFile::LSN_STREAM_TO_FILE_OPTIONS &_stfoAudioOptions ) {
		_stfoAudioOptions = CWavFile::LSN_STREAM_TO_FILE_OPTIONS();
		if ( !_sFile.ReadStringU16( _stfoAudioOptions.wsPath ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.bEnabled ) ) { return false; }
		_stfoAudioOptions.bEnabled = false;	// It should not be enabled via loading of the settings file, only via the dialog.
		if ( !_sFile.Read( _stfoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.fFormat ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.ui32Bits ) ) { return false; }

		if ( !_sFile.Read( _stfoAudioOptions.scStartCondition ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.ui64StartParm ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.dStartParm ) ) { return false; }

		if ( !_sFile.Read( _stfoAudioOptions.seEndCondition ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.ui64EndParm ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.dEndParm ) ) { return false; }

		if ( !_sFile.Read( _stfoAudioOptions.ui64MetaParm ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.i32MetaFormat ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.bMetaEnabled ) ) { return false; }
		return true;
	}

	/**
	 * Saves audio stream-to-file settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _stfoAudioOptions The audio options to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveAudioStreamSettings( CStream &_sFile, CWavFile::LSN_STREAM_TO_FILE_OPTIONS &_stfoAudioOptions ) {
		if ( !_sFile.WriteStringU16( _stfoAudioOptions.wsPath ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.bEnabled ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.fFormat ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.ui32Bits ) ) { return false; }

		if ( !_sFile.Write( _stfoAudioOptions.scStartCondition ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.ui64StartParm ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.dStartParm ) ) { return false; }

		if ( !_sFile.Write( _stfoAudioOptions.seEndCondition ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.ui64EndParm ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.dEndParm ) ) { return false; }

		if ( !_sFile.Write( _stfoAudioOptions.ui64MetaParm ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.i32MetaFormat ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.bMetaEnabled ) ) { return false; }
		return true;
	}

	/**
	 * Loads WAV Editor window settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _wewoOptions The WAV Editor window settings into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadWavEditorWindowSettings( uint32_t /*_ui32Version*/, CStream &_sFile, LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions ) {
		_wewoOptions = LSN_WAV_EDITOR_WINDOW_OPTIONS();
		try {
			_wewoOptions.vPerFileOptions.resize( 1 );
		}
		catch ( ... ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsPreFadeDur ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsFadeDur ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsOpeningSilence ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsTrailingSilence ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsMainsHumVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsWhiteNoiseVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsGuassianNoiseBandwidth ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsGuassianNoiseTemperature ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsGuassianNoiseResistance ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsActualHz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharLpfHz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf0Hz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf1Hz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf2Hz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharLpfFall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf0Fall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf1Fall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf2Fall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsArtist ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsAlbum ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsYear ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsComment ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsAbsoluteVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsNormalizeVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsLoudnessVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsOutputHz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsOutputFolder ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bLoop ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui32ActualHz ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui32CharPreset ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bInvert ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bLpf ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bHpf0 ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bHpf1 ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bHpf2 ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8LpfType ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8Hpf0Type ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8Hpf1Type ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8Hpf2Type ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.bMainsHum ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bWhiteNoise ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bDither ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.bAbsolute ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bNormalize ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bLoudness ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.ui32MainsHum ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32WhiteNoise ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32OutFormat ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32OutBits ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32Stereo ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.bNumbered ) ) { return false; }

		if ( !_sFile.ReadStringU16( _wewoOptions.wsLastWavFolder ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsLastMetaFolder ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bLockVol ) ) { return false; }

		if ( !_sFile.ReadStringU16( _wewoOptions.wsLastProjectsFolder ) ) { return false; }
		return true;
	}

	/**
	 * Saves WAV Editor window settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _wewoOptions The WAV Editor window settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveWavEditorWindowSettings( CStream &_sFile, const LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions ) {
		LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE pfTmp = LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE();
		if ( _wewoOptions.vPerFileOptions.size() ) {
			pfTmp = _wewoOptions.vPerFileOptions[0];
		}
		if ( !_sFile.WriteStringU16( pfTmp.wsPreFadeDur ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsFadeDur ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsOpeningSilence ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsTrailingSilence ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsMainsHumVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsWhiteNoiseVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsGuassianNoiseBandwidth ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsGuassianNoiseTemperature ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsGuassianNoiseResistance ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsActualHz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharLpfHz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf0Hz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf1Hz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf2Hz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharLpfFall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf0Fall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf1Fall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf2Fall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsArtist ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsAlbum ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsYear ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsComment ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsAbsoluteVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsNormalizeVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsLoudnessVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsOutputHz ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsOutputFolder ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bLoop ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui32ActualHz ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui32CharPreset ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bInvert ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bLpf ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bHpf0 ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bHpf1 ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bHpf2 ) ) { return false; }

		if ( !_sFile.Write( pfTmp.ui8LpfType ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui8Hpf0Type ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui8Hpf1Type ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui8Hpf2Type ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.bMainsHum ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bWhiteNoise ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bDither ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.bAbsolute ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bNormalize ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bLoudness ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.ui32MainsHum ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32WhiteNoise ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32OutFormat ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32OutBits ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32Stereo ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.bNumbered ) ) { return false; }

		if ( !_sFile.WriteStringU16( _wewoOptions.wsLastWavFolder ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsLastMetaFolder ) ) { return false; }

		if ( !_sFile.Write( pfTmp.bLockVol ) ) { return false; }

		if ( !_sFile.WriteStringU16( _wewoOptions.wsLastProjectsFolder ) ) { return false; }
		return true;
	}

	/**
	 * Adds or move to the top a given file path.
	 * 
	 * \param _s16Path The file path to add or move to the top.
	 **/
	void CBeesNes::AddPath( const std::u16string &_s16Path ) {
		std::u16string s16Normalized = CUtilities::Replace( _s16Path, u'/', u'\\' );
		// 2 types of culling owing to the use of ZIP files.
#ifdef LSN_WINDOWS
		for ( auto I = m_vRecentFiles.size(); I--; ) {
			if ( CSTR_EQUAL == ::CompareStringEx( LOCALE_NAME_INVARIANT, NORM_IGNORECASE, reinterpret_cast<LPCWCH>(s16Normalized.c_str()), -1, reinterpret_cast<LPCWCH>(m_vRecentFiles[I].c_str()), -1, NULL, NULL, NULL ) ) {
				m_vRecentFiles.erase( m_vRecentFiles.begin() + I );
			}
		}
#else
		while ( true ) {
			auto I = std::find( m_vRecentFiles.begin(), m_vRecentFiles.end(), s16Normalized );
			if ( m_vRecentFiles.end() != I ) {
				m_vRecentFiles.erase( I );
			}
			else { break; }
		}
#endif	// #ifdef LSN_WINDOWS

		
		if ( CUtilities::LastChar( s16Normalized ) == u'}' ) {
			CUtilities::LSN_FILE_PATHS fpDecPathI;
			CUtilities::DeconstructFilePath( s16Normalized, fpDecPathI );
			fpDecPathI.u16sPath.pop_back();
			std::filesystem::path pPathI( fpDecPathI.u16sPath );
			for ( size_t I = 0; I < m_vRecentFiles.size(); ++I ) {
				if ( CUtilities::LastChar( m_vRecentFiles[I] ) == u'}' ) {
					CUtilities::LSN_FILE_PATHS fpDecPathJ;
					CUtilities::DeconstructFilePath( m_vRecentFiles[I], fpDecPathJ );
					fpDecPathJ.u16sPath.pop_back();
					std::filesystem::path pPathJ( fpDecPathJ.u16sPath );

					try {
						if ( std::filesystem::equivalent( pPathI, pPathJ ) ) {
							m_vRecentFiles.erase( m_vRecentFiles.begin() + I-- );
						}
					}
					catch ( ... ) {}

#ifdef LSN_WINDOWS
					if ( CSTR_EQUAL == ::CompareStringEx( LOCALE_NAME_INVARIANT, NORM_IGNORECASE, reinterpret_cast<LPCWCH>(fpDecPathI.u16sFile.c_str()), -1, reinterpret_cast<LPCWCH>(fpDecPathJ.u16sFile.c_str()), -1, NULL, NULL, NULL ) ) {
						m_vRecentFiles.erase( m_vRecentFiles.begin() + I );
					}
#else
					if ( fpDecPathI.u16sFile == fpDecPathJ.u16sFile ) {
						m_vRecentFiles.erase( m_vRecentFiles.begin() + I );
					}
					else {
						try {
							if ( std::filesystem::equivalent( std::filesystem::path( fpDecPathI.u16sFile ), std::filesystem::path( fpDecPathJ.u16sFile ) ) ) {
								m_vRecentFiles.erase( m_vRecentFiles.begin() + I-- );
							}
						}
						catch ( ... ) {}
					}
#endif	// #ifdef LSN_WINDOWS


				}
			}
		}
		else {
			std::filesystem::path pPathI( s16Normalized );
			for ( size_t I = 0; I < m_vRecentFiles.size(); ++I ) {
				if ( CUtilities::LastChar( m_vRecentFiles[I] ) != u'}' ) {
					std::filesystem::path pPathJ( m_vRecentFiles[I] );
					try {
						if ( std::filesystem::equivalent( pPathI, pPathJ ) ) {
							m_vRecentFiles.erase( m_vRecentFiles.begin() + I-- );
						}
					}
					catch ( ... ) {}
				}
			}
		}
		m_vRecentFiles.insert( m_vRecentFiles.begin(), s16Normalized );
		while ( m_vRecentFiles.size() > m_ui32RecentLimit ) { m_vRecentFiles.pop_back(); }
	}

}	// namespace lsn
