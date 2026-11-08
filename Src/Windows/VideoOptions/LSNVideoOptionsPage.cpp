#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A page of fields in the Video Options dialog (the Host Machine page or the page of a filter).
 */

#include "LSNVideoOptionsPage.h"
#include "../../BeesNES/LSNBeesNes.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../MainWindow/LSNMainWindow.h"

#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>

#include <algorithm>
#include <commdlg.h>
#include <filesystem>
#include <string>


namespace lsn {

	CVideoOptionsPage::CVideoOptionsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_ppdData( reinterpret_cast<CVideoOptionsWindowLayout::LSN_PAGE_DATA *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoOptionsPage::InitDialog() {
		if ( !m_ppdData || !m_ppdData->pvodData || !m_ppdData->pvodData->poOptions ) { return Parent::InitDialog(); }

		// Gather the fields on the page.
		m_vFields.clear();
		for ( int I = 0; I < 2; ++I ) {
			size_t sGroups = 0;
			const CVideoOptionsFields::LSN_FIELD_GROUP * pfgGroups = I == 0 ? CVideoOptionsFields::LeftGroups( m_ppdData->vpkKind, sGroups ) :
				CVideoOptionsFields::RightGroups( m_ppdData->vpkKind, sGroups );
			for ( size_t J = 0; J < sGroups; ++J ) {
				for ( size_t K = 0; K < pfgGroups[J].sTotal; ++K ) {
					m_vFields.push_back( pfgGroups[J].pvfFields[K] );
				}
			}
		}

		// The regional presets.
		m_pmRegion = StartingRegion();
		size_t sPresets = 0;
		const LSN_PPU_METRICS * ppmPresets = CVideoOptionsFields::Presets( m_ppdData->vpkKind, sPresets );
		lsw::CComboBox * pcbPresets = static_cast<lsw::CComboBox *>(FindChild( CWinUtilities::LSN_VOWI_PRESET_COMBO ));
		if ( pcbPresets ) {
			m_bUpdating = true;
			for ( size_t I = 0; I < sPresets; ++I ) {
				INT iIdx = pcbPresets->AddString( CVideoOptionsFields::RegionName( ppmPresets[I] ) );
				if ( iIdx >= 0 ) { pcbPresets->SetItemData( iIdx, LPARAM( ppmPresets[I] ) ); }
			}
			pcbPresets->SetCurSelByItemData( LPARAM( m_pmRegion ) );
			m_bUpdating = false;
		}

		SizePaletteDisplay();
		if ( m_ppdData->vpkKind == CVideoOptionsFields::LSN_VPK_HOST ) { LayOutDescriptions(); }

		Refresh();
		return Parent::InitDialog();
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoOptionsPage::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		if ( m_bUpdating || !m_ppdData ) { return LSW_H_CONTINUE; }
		switch ( _wId ) {
			case CWinUtilities::LSN_VOWI_PRESET_COMBO : {
				if ( _wCtrlCode == CBN_SELENDOK && _pwSrc ) {
					m_pmRegion = LSN_PPU_METRICS( static_cast<lsw::CComboBox *>(_pwSrc)->GetCurSelItemData() );
					Refresh();
				}
				return LSW_H_CONTINUE;
			}
			case CWinUtilities::LSN_VOWI_DEFAULTS : {
				if ( _wCtrlCode == BN_CLICKED && CanEdit() ) {
					CVideoOptionsFields::LSN_FIELD_TARGET ftTarget = Target();
					CVideoOptionsFields::RestoreDefaults( ftTarget );
					ApplyChanges();
					Refresh();
				}
				return LSW_H_CONTINUE;
			}
			case CWinUtilities::LSN_VOWI_CONV_DEFAULTS : {
				if ( _wCtrlCode == BN_CLICKED && CanEdit() ) {
					CVideoOptionsFields::LSN_FIELD_TARGET ftTarget = Target();
					CVideoOptionsFields::RestoreConvolutionDefaults( ftTarget );
					ApplyChanges();
					Refresh();
				}
				return LSW_H_CONTINUE;
			}
		}

		CVideoOptionsFields::LSN_VIDEO_FIELD vfField;
		if ( !_pwSrc || !CVideoOptionsWindowLayout::FieldOfControl( _wId, vfField ) ) { return LSW_H_CONTINUE; }
		CVideoOptionsFields::LSN_FIELD_TARGET ftTarget = Target();
		switch ( CVideoOptionsFields::Info( vfField ).vftType ) {
			case CVideoOptionsFields::LSN_VFT_COMBO : {
				if ( _wCtrlCode != CBN_SELENDOK ) { break; }
				LPARAM lpSel = static_cast<lsw::CComboBox *>(_pwSrc)->GetCurSelItemData();
				if ( vfField == CVideoOptionsFields::LSN_VF_PALETTE_LOAD && lpSel < 0 ) {
					// From File... (or the current custom file, which changes nothing).
					if ( lpSel == -2 ) { LoadPaletteFromFile(); }
					Refresh();
					break;
				}
				if ( CVideoOptionsFields::SetValue( vfField, ftTarget, double( lpSel ) ) ) {
					ApplyChanges();
				}
				// Combo boxes can change other fields (TV and palette presets) and which fields can be edited.
				Refresh();
				break;
			}
			case CVideoOptionsFields::LSN_VFT_CHECK : {
				if ( _wCtrlCode != BN_CLICKED ) { break; }
				if ( CVideoOptionsFields::SetValue( vfField, ftTarget, static_cast<lsw::CCheckButton *>(_pwSrc)->IsChecked() ? 1.0 : 0.0 ) ) {
					ApplyChanges();
				}
				UpdateEnabled();
				break;
			}
			case CVideoOptionsFields::LSN_VFT_FLOAT : {}		LSN_FALLTHROUGH
			case CVideoOptionsFields::LSN_VFT_INT : {
				if ( _wCtrlCode == EN_KILLFOCUS ) {
					// Show the value actually in use (out-of-range values are clamped).
					double dValue = 0.0;
					if ( CVideoOptionsFields::GetValue( vfField, ftTarget, dValue ) ) {
						m_bUpdating = true;
						_pwSrc->SetTextW( CVideoOptionsFields::FormatValue( vfField, dValue ).c_str() );
						m_bUpdating = false;
					}
					break;
				}
				if ( _wCtrlCode != EN_CHANGE ) { break; }
				ee::CExpEvalContainer::EE_RESULT rRes;
				if ( _pwSrc->GetTextAsDoubleExpression( rRes ) ) {
					if ( CVideoOptionsFields::SetValue( vfField, ftTarget, rRes.u.dVal ) ) {
						ApplyChanges();
					}
				}
				UpdateEnabled();
				break;
			}
			default : {}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Reloads every control from the options.
	 */
	void CVideoOptionsPage::Refresh() {
		if ( !m_ppdData ) { return; }
		m_bUpdating = true;
		const CVideoOptionsFields::LSN_FIELD_TARGET ftTarget = Target();
		for ( auto vfField : m_vFields ) {
			CWidget * pwCtrl = FindChild( CVideoOptionsWindowLayout::ControlId( vfField ) );
			if ( !pwCtrl ) { continue; }
			double dValue = 0.0;
			const bool bHasValue = CVideoOptionsFields::GetValue( vfField, ftTarget, dValue );
			switch ( CVideoOptionsFields::Info( vfField ).vftType ) {
				case CVideoOptionsFields::LSN_VFT_COMBO : {
					lsw::CComboBox * pcbCombo = static_cast<lsw::CComboBox *>(pwCtrl);
					// Lists are refilled because some depend on the settings (the palette list shows the loaded file).
					CVideoOptionsFields::FillCombo( vfField, ftTarget, pcbCombo );
					if ( bHasValue ) { pcbCombo->SetCurSelByItemData( LPARAM( dValue ) ); }
					break;
				}
				case CVideoOptionsFields::LSN_VFT_CHECK : {
					static_cast<lsw::CCheckButton *>(pwCtrl)->SetCheck( bHasValue && dValue != 0.0 );
					break;
				}
				case CVideoOptionsFields::LSN_VFT_FLOAT : {}	LSN_FALLTHROUGH
				case CVideoOptionsFields::LSN_VFT_INT : {
					pwCtrl->SetTextW( bHasValue ? CVideoOptionsFields::FormatValue( vfField, dValue ).c_str() : L"" );
					break;
				}
				default : {}
			}
		}
		m_bUpdating = false;
		UpdateEnabled();
	}

	/**
	 * Gets the settings the page edits.
	 *
	 * \return Returns the settings the page edits.
	 */
	CVideoOptionsFields::LSN_FIELD_TARGET CVideoOptionsPage::Target() const {
		CVideoOptionsFields::LSN_FIELD_TARGET ftRet;
		if ( !m_ppdData || !m_ppdData->pvodData || !m_ppdData->pvodData->poOptions ) { return ftRet; }
		LSN_OPTIONS & oOptions = (*m_ppdData->pvodData->poOptions);
		ftRet.vpkKind = m_ppdData->vpkKind;
		ftRet.pmRegion = m_pmRegion;
		if ( m_ppdData->vpkKind == CVideoOptionsFields::LSN_VPK_HOST ) {
			ftRet.phdoHost = &oOptions.hdoHostDisplay;
			return ftRet;
		}
		if ( size_t( m_pmRegion ) >= LSN_PM_CONSOLE_TOTAL ) { return ftRet; }
		ftRet.pfroFilters = m_ppdData->bGlobal ? &oOptions.vfoGlobalFilters.froRegions[m_pmRegion] : &oOptions.vfoThisGameFilters.froRegions[m_pmRegion];
		ftRet.ppoPalette = m_ppdData->bGlobal ? &oOptions.poGlobalPalettes[m_pmRegion] : &oOptions.poThisGamePalette[m_pmRegion];
		return ftRet;
	}

	/**
	 * Determines whether the page can be edited (a per-game page can be edited only while per-game settings are in use).
	 *
	 * \return Returns true if the page can be edited.
	 */
	bool CVideoOptionsPage::CanEdit() const {
		if ( !m_ppdData || m_ppdData->bGlobal || m_ppdData->vpkKind == CVideoOptionsFields::LSN_VPK_HOST ) { return true; }
		const CVideoOptionsFields::LSN_FIELD_TARGET ftTarget = Target();
		return ftTarget.pfroFilters && !ftTarget.pfroFilters->bUseGlobal;
	}

	/**
	 * Enables or disables each control according to the current settings.
	 */
	void CVideoOptionsPage::UpdateEnabled() {
		const CVideoOptionsFields::LSN_FIELD_TARGET ftTarget = Target();
		const bool bCanEdit = CanEdit();
		for ( auto vfField : m_vFields ) {
			const BOOL bEnable = (bCanEdit && CVideoOptionsFields::IsEditable( vfField, ftTarget )) ? TRUE : FALSE;
			CWidget * pwCtrl = FindChild( CVideoOptionsWindowLayout::ControlId( vfField ) );
			if ( pwCtrl ) { pwCtrl->SetEnabled( bEnable ); }
			CWidget * pwLabel = FindChild( CVideoOptionsWindowLayout::LabelId( vfField ) );
			if ( pwLabel ) { pwLabel->SetEnabled( bEnable ); }
		}
		for ( WORD wId : { WORD( CWinUtilities::LSN_VOWI_DEFAULTS ), WORD( CWinUtilities::LSN_VOWI_CONV_DEFAULTS ) } ) {
			CWidget * pwDefaults = FindChild( wId );
			if ( pwDefaults ) { pwDefaults->SetEnabled( bCanEdit ? TRUE : FALSE ); }
		}
	}

	/**
	 * Applies the options to the emulator.
	 */
	void CVideoOptionsPage::ApplyChanges() {
		if ( m_ppdData && m_ppdData->pvodData && m_ppdData->pvodData->pmwMainWindow ) {
			m_ppdData->pvodData->pmwMainWindow->ApplyVideoOptions();
		}
	}

	/**
	 * Asks the user for a palette file and loads it.
	 */
	void CVideoOptionsPage::LoadPaletteFromFile() {
		CVideoOptionsFields::LSN_FIELD_TARGET ftTarget = Target();
		if ( !ftTarget.ppoPalette ) { return; }
		try {
			OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
			std::wstring wsFileName;
			wsFileName.resize( 0xFFFF + 2 );
			std::wstring wsInitialDir;
			try {
				wsInitialDir = ftTarget.ppoPalette->wsPath.empty() ? CBeesNes::DefaultPaletteFolder() :
					std::filesystem::path( ftTarget.ppoPalette->wsPath ).parent_path().wstring();
			}
			catch ( ... ) {}

			std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_STR_VO_PAL_TYPES ), std::size( LSN_LSTR( LSN_STR_VO_PAL_TYPES ) ) - 1 );
			ofnOpenFile.hwndOwner = Wnd();
			ofnOpenFile.lpstrFilter = wsFilter.c_str();
			ofnOpenFile.lpstrFile = wsFileName.data();
			ofnOpenFile.nMaxFile = DWORD( wsFileName.size() );
			ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
			ofnOpenFile.lpstrInitialDir = wsInitialDir.empty() ? nullptr : wsInitialDir.c_str();

			if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
				ftTarget.ppoPalette->wsPath = ofnOpenFile.lpstrFile;
				// A palette from a file is taken to be an sRGB palette.
				CVideoOptionsFields::ApplyCustomPalette( ftTarget );
				ApplyChanges();
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Sizes the palette display to its fixed client area (LSN_PALETTE_DISPLAY_W by LSN_PALETTE_DISPLAY_H pixels plus its scroll bar)
	 *	and makes the Load combo box end where the display ends.
	 */
	void CVideoOptionsPage::SizePaletteDisplay() {
		CWidget * pwDisplay = FindChild( CVideoOptionsWindowLayout::ControlId( CVideoOptionsFields::LSN_VF_PALETTE_DISPLAY ) );
		if ( !pwDisplay ) { return; }
		LSW_RECT rClient = { 0, 0, CVideoOptionsWindowLayout::LSN_PALETTE_DISPLAY_W, CVideoOptionsWindowLayout::LSN_PALETTE_DISPLAY_H };
		::AdjustWindowRectEx( &rClient, pwDisplay->GetStyle(), FALSE, pwDisplay->GetStyleEx() );
		rClient.right += ::GetSystemMetrics( SM_CXVSCROLL );
		::SetWindowPos( pwDisplay->Wnd(), NULL, 0, 0, rClient.Width(), rClient.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
		// A 512-entry palette needs a second screen of rows (each row is 16 colors of 24-by-24 pixels).
		SCROLLINFO siInfo = { sizeof( siInfo ) };
		siInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
		siInfo.nMin = 0;
		siInfo.nMax = CVideoOptionsWindowLayout::LSN_PALETTE_DISPLAY_H - 1;
		siInfo.nPage = UINT( CVideoOptionsWindowLayout::LSN_PALETTE_DISPLAY_H );
		siInfo.nPos = 0;
		::SetScrollInfo( pwDisplay->Wnd(), SB_VERT, &siInfo, TRUE );

		// The Load combo box ends where the display ends.
		CWidget * pwLoad = FindChild( CVideoOptionsWindowLayout::ControlId( CVideoOptionsFields::LSN_VF_PALETTE_LOAD ) );
		if ( pwLoad ) {
			const LSW_RECT rDisplay = ChildRect( pwDisplay );
			const LSW_RECT rLoad = ChildRect( pwLoad );
			if ( rDisplay.right > rLoad.left ) {
				::SetWindowPos( pwLoad->Wnd(), NULL, 0, 0, rDisplay.right - rLoad.left, rLoad.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
			}
		}
	}

	/**
	 * Fills in the descriptions of the Host Machine page and lays its rows out around their measured heights.
	 */
	void CVideoOptionsPage::LayOutDescriptions() {
		size_t sGroups = 0;
		const CVideoOptionsFields::LSN_FIELD_GROUP * pfgGroups = CVideoOptionsFields::LeftGroups( CVideoOptionsFields::LSN_VPK_HOST, sGroups );
		const POINT pGroupTop = DialogUnitsToPixels( LSN_GROUP_LEFT, LSN_GROUP_TOP );
		const POINT pGroupBottom = DialogUnitsToPixels( 0, LSN_GROUP_BOTTOM );
		const POINT pGap = DialogUnitsToPixels( 0, LSN_TOP_JUST * 2 );
		const POINT pJust = DialogUnitsToPixels( 0, LSN_TOP_JUST );
		LONG lBottom = 0;
		for ( size_t I = 0; I < sGroups; ++I ) {
			const CVideoOptionsFields::LSN_FIELD_GROUP & fgGroup = pfgGroups[I];
			CWidget * pwGroup = FindChild( WORD( CWinUtilities::LSN_VOWI_GROUP_FIRST + I ) );
			if ( !pwGroup ) { continue; }
			LSW_RECT rGroup = ChildRect( pwGroup );
			LONG lY = rGroup.top + pGroupTop.y;
			for ( size_t J = 0; J < fgGroup.sTotal; ++J ) {
				const CVideoOptionsFields::LSN_VIDEO_FIELD vfField = fgGroup.pvfFields[J];
				CWidget * pwLabel = FindChild( CVideoOptionsWindowLayout::LabelId( vfField ) );
				CWidget * pwCombo = FindChild( CVideoOptionsWindowLayout::ControlId( vfField ) );
				CWidget * pwDesc = FindChild( CVideoOptionsWindowLayout::DescriptionId( vfField ) );
				if ( !pwLabel || !pwCombo || !pwDesc ) { continue; }

				// The description is the tool tip of the field.
				std::wstring wsDesc;
				const char * pcDesc = CVideoOptionsFields::Info( vfField ).pcToolTip;
				if ( pcDesc ) {
					const int iLen = ::MultiByteToWideChar( CP_UTF8, 0, pcDesc, -1, nullptr, 0 );
					if ( iLen > 1 ) {
						wsDesc.resize( size_t( iLen ) );
						::MultiByteToWideChar( CP_UTF8, 0, pcDesc, -1, wsDesc.data(), iLen );
						wsDesc.resize( size_t( iLen - 1 ) );
					}
				}
				pwDesc->SetTextW( wsDesc.c_str() );

				// Measure the description at its width.
				LSW_RECT rDesc = ChildRect( pwDesc );
				RECT rCalc = { 0, 0, rDesc.Width(), 0 };
				HDC hDc = ::GetDC( pwDesc->Wnd() );
				if ( hDc ) {
					HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW( pwDesc->Wnd(), WM_GETFONT, 0, 0 ));
					HGDIOBJ hOld = hFont ? ::SelectObject( hDc, hFont ) : NULL;
					::DrawTextW( hDc, wsDesc.c_str(), int( wsDesc.size() ), &rCalc, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS );
					if ( hOld ) { ::SelectObject( hDc, hOld ); }
					::ReleaseDC( pwDesc->Wnd(), hDc );
				}
				const LSW_RECT rCombo = ChildRect( pwCombo );
				const LSW_RECT rLabel = ChildRect( pwLabel );
				// The first line of the description lines up with the label.
				const LONG lTextTop = lY + (rCombo.Height() - rLabel.Height()) / 2;
				const LONG lDescH = std::max<LONG>( rCalc.bottom - rCalc.top, rLabel.Height() );
				::SetWindowPos( pwCombo->Wnd(), NULL, rCombo.left, lY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
				::SetWindowPos( pwLabel->Wnd(), NULL, rLabel.left, lTextTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
				::SetWindowPos( pwDesc->Wnd(), NULL, rDesc.left, lTextTop, rDesc.Width(), lDescH, SWP_NOZORDER | SWP_NOACTIVATE );
				lY = std::max<LONG>( lY + rCombo.Height(), lTextTop + lDescH );

				// The separator.
				CWidget * pwSep = FindChild( CVideoOptionsWindowLayout::SeparatorId( vfField ) );
				if ( pwSep ) {
					const LSW_RECT rSep = ChildRect( pwSep );
					::SetWindowPos( pwSep->Wnd(), NULL, rSep.left, lY + pGap.y, rSep.Width(), 2, SWP_NOZORDER | SWP_NOACTIVATE );
					lY += pGap.y * 2 + 2;
				}
			}
			rGroup.bottom = lY + pGroupBottom.y + pJust.y;
			::SetWindowPos( pwGroup->Wnd(), NULL, 0, 0, rGroup.Width(), rGroup.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
			lBottom = std::max<LONG>( lBottom, rGroup.bottom );
		}

		// Restore Defaults below the group, then the page around it.
		CWidget * pwDefaults = FindChild( CWinUtilities::LSN_VOWI_DEFAULTS );
		if ( pwDefaults ) {
			const LSW_RECT rDefaults = ChildRect( pwDefaults );
			::SetWindowPos( pwDefaults->Wnd(), NULL, rDefaults.left, lBottom + pJust.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
			lBottom += pJust.y + rDefaults.Height();
		}
		const LSW_RECT rPage = WindowRect();
		::SetWindowPos( Wnd(), NULL, 0, 0, rPage.Width(), lBottom + pJust.y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
	}

	/**
	 * Gets the rectangle of a child control in the client coordinates of this page.
	 *
	 * \param _pwChild The child control.
	 * \return Returns the rectangle of the child control in the client coordinates of this page.
	 */
	LSW_RECT CVideoOptionsPage::ChildRect( CWidget * _pwChild ) const {
		LSW_RECT rRet = _pwChild->WindowRect();
		::MapWindowPoints( NULL, Wnd(), reinterpret_cast<POINT *>(&rRet), 2 );
		return rRet;
	}

	/**
	 * Converts dialog units to pixels using the font of this page.
	 *
	 * \param _iX The horizontal dialog units.
	 * \param _iY The vertical dialog units.
	 * \return Returns the size in pixels.
	 */
	POINT CVideoOptionsPage::DialogUnitsToPixels( INT _iX, INT _iY ) const {
		RECT rUnits = { 0, 0, _iX, _iY };
		::MapDialogRect( Wnd(), &rUnits );
		return POINT{ rUnits.right, rUnits.bottom };
	}

	/**
	 * Gets the starting region of the page:  the preset nearest the region of the running game.
	 *
	 * \return Returns the starting region of the page.
	 */
	LSN_PPU_METRICS CVideoOptionsPage::StartingRegion() const {
		const LSN_PPU_METRICS pmActive = (m_ppdData && m_ppdData->pvodData) ? m_ppdData->pvodData->pmActiveRegion : LSN_PM_NTSC_M;
		size_t sPresets = 0;
		const LSN_PPU_METRICS * ppmPresets = m_ppdData ? CVideoOptionsFields::Presets( m_ppdData->vpkKind, sPresets ) : nullptr;
		if ( !sPresets ) { return pmActive; }
		for ( size_t I = 0; I < sPresets; ++I ) {
			if ( ppmPresets[I] == pmActive ) { return pmActive; }
		}
		// The NTSC presets for PAL regions and vice versa:  the first one.
		return ppmPresets[0];
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
