/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles all emulator functionality.
 */

#include "LSNBeesNes.h"

namespace lsn {

	CBeesNes::CBeesNes( CDisplayHost * _pdhDisplayHost, CInputPoller * _pipPoller ) :
		m_dScale( 3.0 ),
		m_dRatio( 4.0 / 3.0 ),
		m_dRatioActual( 4.0 / 3.0 ),
		m_fFilter( CFilterBase::LSN_F_AUTO_CRT ),
		m_ppPostProcess( CPostProcessBase::LSN_PP_BILINEAR ),
		m_pmSystem( LSN_PM_NTSC ),
		m_pdhDisplayHost( _pdhDisplayHost ),
		m_pipPoller( _pipPoller ) {

		std::memset( m_ui8RapidFires, 0, sizeof( m_ui8RapidFires ) );
		
		//m_pfbFilterTable
		CFilterBase * pfbTmp[CFilterBase::LSN_F_TOTAL][LSN_PM_CONSOLE_TOTAL] = {
			//LSN_PM_NTSC					LSN_PM_PAL						LSN_PM_DENDY
			{ &m_r24fRgb24Filter,			&m_r24fRgb24Filter,				&m_r24fRgb24Filter },			// LSN_F_RGB24
			{ &m_nbfBlargNtscFilter,		&m_nbfBlargNtscFilter,			&m_nbfBlargNtscFilter },		// LSN_F_NTSC_BLARGG
			{ &m_nbfBlargPalFilter,			&m_nbfBlargPalFilter,			&m_nbfBlargPalFilter },			// LSN_F_PAL_BLARGG
			{ &m_ncfEmmirNtscFilter,		&m_ncfEmmirNtscFilter,			&m_ncfEmmirNtscFilter },		// LSN_F_NTSC_CRT
			{ &m_nbfBlargNtscFilter,		&m_nbfBlargPalFilter,			&m_nbfBlargPalFilter },			// LSN_F_AUTO_BLARGG
			{ &m_ncfEmmirNtscFilter,		&m_nbfBlargPalFilter,			&m_nbfBlargPalFilter },			// LSN_F_AUTO_CRT
		};
		std::memcpy( m_pfbFilterTable, pfbTmp, sizeof( pfbTmp ) );

		m_pppbPostTable[CPostProcessBase::LSN_PP_NONE] = &m_ppbNoPostProcessing;
		m_pppbPostTable[CPostProcessBase::LSN_PP_BILINEAR] = &m_blppBiLinearPost;

		m_psbSystems[LSN_PM_NTSC] = &m_nsNtscSystem;
		m_psbSystems[LSN_PM_PAL] = &m_nsPalSystem;
		m_psbSystems[LSN_PM_DENDY] = &m_nsDendySystem;


		m_pnsSystem = m_psbSystems[m_pmSystem];


		const size_t stBuffers = 2;
		m_r24fRgb24Filter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBlargNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBlargPalFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );

		UpdateCurrentSystem();
	}
	CBeesNes::~CBeesNes() {
	}

	// == Functions.
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

			m_cfartCurFilterAndTargets.pui8LastFilteredResult = m_pppbPostTable[m_ppPostProcess]->ApplyFilter( m_cfartCurFilterAndTargets.pui8LastFilteredResult,
				_ui32FinalW, _ui32FinalH, m_cfartCurFilterAndTargets.bMirrored,
				m_cfartCurFilterAndTargets.ui32Width, m_cfartCurFilterAndTargets.ui32Height, m_cfartCurFilterAndTargets.ui16Bits, m_cfartCurFilterAndTargets.ui32Stride,
				m_cfartCurFilterAndTargets.ui64Frame, m_cfartCurFilterAndTargets.ui64RenderStartCycle );
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

}	// namespace lsn
