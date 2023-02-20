/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles all emulator functionality.
 */

#include "LSNBeesNes.h"
#include "../File/LSNStdFile.h"


namespace lsn {

	CBeesNes::CBeesNes( CDisplayHost * _pdhDisplayHost, CInputPoller * _pipPoller ) :
		m_dScale( 3.0 ),
		m_dRatio( 4.0 / 3.0 ),
		m_dRatioActual( 4.0 / 3.0 ),
		m_fFilter( CFilterBase::LSN_F_AUTO_CRT_FULL ),
		//m_ppPostProcess( CPostProcessBase::LSN_PP_BILINEAR ),
		m_pmSystem( LSN_PM_NTSC ),
		m_pdhDisplayHost( _pdhDisplayHost ),
		m_pipPoller( _pipPoller ) {

		std::memset( m_ui8RapidFires, 0, sizeof( m_ui8RapidFires ) );
		
		//m_pfbFilterTable
		CFilterBase * pfbTmp[CFilterBase::LSN_F_TOTAL][LSN_PM_CONSOLE_TOTAL] = {
			//LSN_PM_NTSC					LSN_PM_PAL						LSN_PM_DENDY
			{ &m_r24fRgb24Filter,			&m_r24fRgb24Filter,				&m_r24fRgb24Filter },				// LSN_F_RGB24
			{ &m_nbfBlarggNtscFilter,		&m_nbfBlarggNtscFilter,			&m_nbfBlarggNtscFilter },			// LSN_F_NTSC_BLARGG
			{ &m_nbfBlarggPalFilter,		&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter },			// LSN_F_PAL_BLARGG
			{ &m_nbfBisqwitNtscFilter,		&m_nbfBisqwitNtscFilter,		&m_nbfBisqwitNtscFilter },			// LSN_F_NTSC_BISQWIT
			{ &m_ncfEmmirNtscFilter,		&m_ncfEmmirNtscFilter,			&m_ncfEmmirNtscFilter },			// LSN_F_NTSC_CRT
			{ &m_ncfEmmirNtscFullFilter,	&m_ncfEmmirNtscFullFilter,		&m_ncfEmmirNtscFullFilter },		// LSN_F_NTSC_CRT_FULL
			{ &m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter },			// LSN_F_PAL_CRT_FULL
			{ &m_nbfBlarggNtscFilter,		&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter },			// LSN_F_AUTO_BLARGG
			{ &m_ncfEmmirNtscFilter,		&m_nbfBlarggPalFilter,			&m_nbfBlarggPalFilter },			// LSN_F_AUTO_CRT
			{ &m_ncfEmmirNtscFullFilter,	&m_ncfEmmirPalFullFilter,		&m_ncfEmmirPalFullFilter },			// LSN_F_AUTO_CRT_FULL
		};
		std::memcpy( m_pfbFilterTable, pfbTmp, sizeof( pfbTmp ) );

		m_pppbPostTable[CPostProcessBase::LSN_PP_NONE] = &m_ppbNoPostProcessing;
		m_pppbPostTable[CPostProcessBase::LSN_PP_BLEED] = &m_bppBleedPostProcessing;
		m_pppbPostTable[CPostProcessBase::LSN_PP_BILINEAR] = &m_blppBiLinearPost;
		m_pppbPostTable[CPostProcessBase::LSN_PP_SRGB] = &m_sppLinearTosRGBPost;
		m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_BLEED );
		m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_BILINEAR );
		//m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_SRGB );


		m_psbSystems[LSN_PM_NTSC] = &m_nsNtscSystem;
		m_psbSystems[LSN_PM_PAL] = &m_nsPalSystem;
		m_psbSystems[LSN_PM_DENDY] = &m_nsDendySystem;


		m_pnsSystem = m_psbSystems[m_pmSystem];


		const size_t stBuffers = 3;
		m_r24fRgb24Filter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBisqwitNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBlarggNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBlarggPalFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirNtscFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirPalFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );

		UpdateCurrentSystem();
	}
	CBeesNes::~CBeesNes() {
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
	 * Sets the current filter.  Changes are not applied to rendering output until Swap() is called.
	 *
	 * \param _fFilter The new filter to be applied after the next Swap().
	 */
	void CBeesNes::SetCurFilter( CFilterBase::LSN_FILTERS _fFilter ) {
		if ( m_fFilter != _fFilter ) {
			CFilterBase * pfbPrev = m_pfbFilterTable[m_fFilter][GetCurPpuRegion()];
			m_fFilter = _fFilter;
			m_cfartCurFilterAndTargets.pfbNextFilter = m_pfbFilterTable[m_fFilter][GetCurPpuRegion()];
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
		return m_pnsSystem->GetDisplayClient()->PpuRegion();
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
		LSN_ROM rTmp;
		if ( CSystemBase::LoadRom( _vRom, rTmp, _s16Path ) ) {
			//m_pnsSystem.reset();
			LSN_PPU_METRICS pmReg = _pmRegion;
			m_pmSystem = pmReg;
			if ( m_pmSystem == LSN_PPU_METRICS::LSN_PM_UNKNOWN ) {
				m_pmSystem = rTmp.riInfo.pmConsoleRegion;
			}
			if ( m_pmSystem == LSN_PPU_METRICS::LSN_PM_UNKNOWN ) {
				m_pmSystem = LSN_PPU_METRICS::LSN_PM_NTSC;
			}
			m_pnsSystem = m_psbSystems[m_pmSystem];
			UpdateCurrentSystem();
			if ( m_pnsSystem->LoadRom( rTmp ) ) {
				m_pnsSystem->ResetState( false );
				return true;
			}
		}
		return false;
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
		m_cfartCurFilterAndTargets.pfbCurFilter = m_pfbFilterTable[m_fFilter][GetDisplayClient()->PpuRegion()];
					
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
		m_pnsSystem->SetInputPoller( m_pipPoller );
	}

	/**
	 * Loads the settings file.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings file was loaded.
	 */
	bool CBeesNes::LoadSettings( CStream &_sFile ) {
		// Get the file version.
		uint32_t ui32Version;
		if ( !_sFile.ReadUi32( ui32Version ) ) { return false; }

		if ( !LoadInputSettings( ui32Version, _sFile, m_oOptions.ioGlobalInputOptions ) ) { return false; }
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

}	// namespace lsn
