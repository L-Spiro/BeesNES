#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Video Options dialog.
 */

#include "LSNVideoOptionsWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"

#include <algorithm>


namespace lsn {

/** The width of the left column (the common settings). */
#define LSN_VO_LEFT_W									210
/** The smallest width of the right column (the filter-specific settings; see CVideoOptionsWindowLayout::RightColumnWidth()). */
#define LSN_VO_RIGHT_MIN_W								200
/** The width of the right column before the pages are fitted to the palette display. */
#define LSN_VO_RIGHT_W									245
/** The width of a label. */
#define LSN_VO_LABEL_W									88
/** The height of a row. */
#define LSN_VO_ROW_H									(LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST)
/** The height of the palette display (dialog units; the display is resized to 384-by-96 pixels when the page is created). */
#define LSN_VO_DISPLAY_H								64
/** The width of the Restore Defaults button. */
#define LSN_VO_DEFAULTS_W								(LSN_DEF_BUTTON_WIDTH + 24)
/** The width of the dialog before it is fitted to its pages. */
#define LSN_VO_W										(LSN_LEFT_JUST + LSN_VO_LEFT_W + LSN_LEFT_JUST + LSN_VO_RIGHT_W + LSN_LEFT_JUST)
/** The height of the dialog before it is fitted to its pages. */
#define LSN_VO_H										400
/** The height of a description on the Host Machine page before the page measures it. */
#define LSN_VO_DESC_H									(LSN_DEF_STATIC_HEIGHT * 3)

	static_assert( CVideoOptionsFields::LSN_VF_TOTAL * 4 <= CWinUtilities::LSN_VOWI_FIELD_LAST - CWinUtilities::LSN_VOWI_FIELD_FIRST + 1, "Too many fields for the control IDs." );

	// == Members.
	/** The layout for the window. */
	LSW_WIDGET_LAYOUT CVideoOptionsWindowLayout::m_wlWindow[] = {
		{
			LSN_LT_VIDEO_OPTIONS_DIALOG,				// ltType
			CWinUtilities::LSN_VOWI_DIALOG,				// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			0,											// iLeft
			0,											// iTop
			LSN_VO_W,									// dwWidth
			LSN_VO_H,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			LSN_LSTR( LSN_STR_VO_TITLE ),				// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VOWI_NONE,				// dwParentId
		},
		{
			LSW_LT_TAB,									// ltType
			CWinUtilities::LSN_VOWI_TAB,				// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			0,											// iLeft
			0,											// iTop
			LSN_VO_W,									// dwWidth
			LSN_VO_H - LSN_DEF_BUTTON_HEIGHT - (LSN_TOP_JUST * 2),	// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,	// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VOWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_BUTTON,								// ltType
			CWinUtilities::LSN_VOWI_OK,					// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			TRUE,										// bActive
			LSN_LEFT_JUST,								// iLeft
			LSN_VO_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,						// dwHeight
			LSN_DEFBUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_OK ),							// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VOWI_DIALOG,				// dwParentId

			LSN_LOCK_LEFT,								// pcLeftSizeExp
			nullptr, 0,									// pcRightSizeExp
			nullptr, 0,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,							// pcBottomSizeExp
			LSN_FIXED_WIDTH,							// pcWidthSizeExp
			LSN_FIXED_HEIGHT,							// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,								// ltType
			CWinUtilities::LSN_VOWI_CANCEL,				// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VO_W - LSN_DEF_BUTTON_WIDTH - LSN_LEFT_JUST,	// iLeft
			LSN_VO_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,						// dwHeight
			LSN_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_CANCEL ),						// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VOWI_DIALOG,				// dwParentId

			nullptr, 0,									// pcLeftSizeExp
			LSN_LOCK_RIGHT,								// pcRightSizeExp
			nullptr, 0,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,							// pcBottomSizeExp
			LSN_FIXED_WIDTH,							// pcWidthSizeExp
			LSN_FIXED_HEIGHT,							// pcHeightSizeExp
		},
	};

	/** The layout for a settings page (it is fitted to its filter pages when it is created). */
	LSW_WIDGET_LAYOUT CVideoOptionsWindowLayout::m_wlSettingsPage[] = {
		{
			LSN_LT_VIDEO_OPTIONS_SETTINGS_PAGE,			// ltType
			CWinUtilities::LSN_VOWI_SETTINGS_PAGE,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			0,											// iLeft
			0,											// iTop
			LSN_VO_W,									// dwWidth
			LSN_VO_H,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,	// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VOWI_NONE,				// dwParentId
		},
		{
			LSW_LT_TAB,									// ltType
			CWinUtilities::LSN_VOWI_SETTINGS_TAB,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			0,											// iLeft
			0,											// iTop
			LSN_VO_W,									// dwWidth
			LSN_VO_H,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,	// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VOWI_SETTINGS_PAGE,		// dwParentId
		},
	};

	// == Functions.
	/**
	 * Creates the Video Options dialog.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \param _vodData The data of the dialog.
	 * \return Returns TRUE if the dialog was closed with OK.
	 */
	BOOL CVideoOptionsWindowLayout::CreateVideoOptionsDialog( CWidget * _pwParent, LSN_VIDEO_OPTIONS_DATA &_vodData ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlWindow, std::size( m_wlWindow ), _pwParent, reinterpret_cast<uint64_t>(&_vodData) );
		return ipProc != 0 ? TRUE : FALSE;
	}

	/**
	 * Creates a page of fields (the Host Machine page or a filter page).
	 *
	 * \param _pwParent The parent widget.
	 * \param _ppdData The data of the page.  Must remain valid for the lifetime of the page.
	 * \param _pwMetrics A dialog with the font of the pages, used to size the right column (see RightColumnWidth()), or nullptr to use _pwParent.
	 * \return Returns the created page.
	 */
	CWidget * CVideoOptionsWindowLayout::CreateFieldPage( CWidget * _pwParent, LSN_PAGE_DATA * _ppdData, CWidget * _pwMetrics ) {
		if ( !_ppdData ) { return nullptr; }
		try {
			std::vector<LSW_WIDGET_LAYOUT> vLayout;
			const INT iRightW = RightColumnWidth( _pwMetrics ? _pwMetrics : _pwParent );
			if ( _ppdData->vpkKind == CVideoOptionsFields::LSN_VPK_HOST ) { BuildHostPage( iRightW, vLayout ); }
			else { BuildFieldPage( _ppdData->vpkKind, iRightW, vLayout ); }
			lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
			return plmLayout->CreateDialogX( vLayout.data(), vLayout.size(), _pwParent, reinterpret_cast<uint64_t>(_ppdData) );
		}
		catch ( ... ) { return nullptr; }
	}

	/**
	 * Creates a settings page (the Global Settings or Per-Game Settings page, which holds a tab for each filter).
	 *
	 * \param _pwParent The parent widget.
	 * \param _ppdData The data of the page.  Must remain valid for the lifetime of the page.
	 * \return Returns the created page.
	 */
	CWidget * CVideoOptionsWindowLayout::CreateSettingsPage( CWidget * _pwParent, LSN_PAGE_DATA * _ppdData ) {
		if ( !_ppdData ) { return nullptr; }
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		return plmLayout->CreateDialogX( m_wlSettingsPage, std::size( m_wlSettingsPage ), _pwParent, reinterpret_cast<uint64_t>(_ppdData) );
	}

	/**
	 * Builds the layout of a page of fields.
	 *
	 * \param _vpkKind The kind of page.
	 * \param _iRightW The width of the right column, in dialog units.
	 * \param _vLayout Holds the returned layout.
	 */
	void CVideoOptionsWindowLayout::BuildFieldPage( CVideoOptionsFields::LSN_VIDEO_PAGE_KIND _vpkKind, INT _iRightW, std::vector<LSW_WIDGET_LAYOUT> &_vLayout ) {
		_vLayout.clear();
		size_t sLeftGroups = 0, sRightGroups = 0, sPresets = 0;
		const CVideoOptionsFields::LSN_FIELD_GROUP * pfgLeft = CVideoOptionsFields::LeftGroups( _vpkKind, sLeftGroups );
		const CVideoOptionsFields::LSN_FIELD_GROUP * pfgRight = CVideoOptionsFields::RightGroups( _vpkKind, sRightGroups );
		CVideoOptionsFields::Presets( _vpkKind, sPresets );

		const INT iLeftL = LSN_LEFT_JUST;
		const INT iRightL = iLeftL + LSN_VO_LEFT_W + LSN_LEFT_JUST;
		const INT iPageW = sRightGroups ? (iRightL + _iRightW + LSN_LEFT_JUST) : (iLeftL + LSN_VO_LEFT_W + LSN_LEFT_JUST);

		// The page itself (its height is set once the columns are known).
		LSW_WIDGET_LAYOUT wlPage = {};
		wlPage.ltType = LSN_LT_VIDEO_OPTIONS_FIELD_PAGE;
		wlPage.wId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
		wlPage.bEnabled = TRUE;
		wlPage.dwWidth = DWORD( iPageW );
		wlPage.dwStyle = WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL;
		wlPage.dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT;
		wlPage.dwParentId = CWinUtilities::LSN_VOWI_NONE;
		_vLayout.push_back( wlPage );

		// The regional preset.
		INT iLeftTop = LSN_TOP_JUST;
		if ( sPresets ) {
			LSW_WIDGET_LAYOUT wlLabel = {};
			wlLabel.ltType = LSW_LT_LABEL;
			wlLabel.wId = CWinUtilities::LSN_VOWI_PRESET_LABEL;
			wlLabel.bEnabled = TRUE;
			wlLabel.iLeft = iLeftL + LSN_GROUP_LEFT;
			wlLabel.iTop = iLeftTop + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1);
			wlLabel.dwWidth = LSN_VO_LABEL_W;
			wlLabel.dwHeight = LSN_DEF_STATIC_HEIGHT;
			wlLabel.dwStyle = LSN_STATICSTYLE;
			wlLabel.pwcText = LSN_LSTR( LSN_STR_VO_REGIONAL_PRESET );
			wlLabel.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
			wlLabel.pcToolTip = reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_REGIONAL_PRESET ));
			wlLabel.dwToolTipStyleEx = WS_EX_TOPMOST;
			wlLabel.dwToolTipStyle = LSN_TOOLTIP_STYLE;
			_vLayout.push_back( wlLabel );

			LSW_WIDGET_LAYOUT wlCombo = wlLabel;
			wlCombo.ltType = LSW_LT_COMBOBOX;
			wlCombo.wId = CWinUtilities::LSN_VOWI_PRESET_COMBO;
			wlCombo.lpwcClass = WC_COMBOBOXW;
			wlCombo.iLeft = iLeftL + LSN_GROUP_LEFT + LSN_VO_LABEL_W;
			wlCombo.iTop = iLeftTop;
			wlCombo.dwWidth = DWORD( LSN_VO_LEFT_W - LSN_GROUP_LEFT * 2 - LSN_VO_LABEL_W );
			wlCombo.dwHeight = LSN_DEF_COMBO_HEIGHT;
			wlCombo.dwStyle = LSN_COMBOSTYLE_LIST;
			wlCombo.dwStyleEx = LSN_COMBOSTYLEEX_LIST;
			wlCombo.pwcText = nullptr;
			_vLayout.push_back( wlCombo );
			iLeftTop += LSN_VO_ROW_H + LSN_TOP_JUST;
		}

		INT iBottom = AddColumn( pfgLeft, sLeftGroups, iLeftL, iLeftTop, LSN_VO_LEFT_W, WORD( CWinUtilities::LSN_VOWI_GROUP_FIRST ), _vLayout );
		if ( sRightGroups ) {
			iBottom = std::max( iBottom, AddColumn( pfgRight, sRightGroups, iRightL, LSN_TOP_JUST, _iRightW,
				WORD( CWinUtilities::LSN_VOWI_GROUP_FIRST + sLeftGroups ), _vLayout ) );
		}

		// Restore Defaults.
		LSW_WIDGET_LAYOUT wlDefaults = DefaultsButton( CWinUtilities::LSN_VOWI_DEFAULTS, iPageW - LSN_LEFT_JUST - LSN_VO_DEFAULTS_W, iBottom + LSN_TOP_JUST,
			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_RESTORE_DEFAULTS )) );
		_vLayout.push_back( wlDefaults );

		_vLayout[0].dwHeight = DWORD( wlDefaults.iTop + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST );
	}

	/**
	 * Builds the layout of the Host Machine page:  each setting on the left with its description on the right.  The page lays the
	 *	rows out again once it knows the heights of the descriptions (see CVideoOptionsPage::LayOutDescriptions()).
	 *
	 * \param _iRightW The width of the right column, in dialog units.
	 * \param _vLayout Holds the returned layout.
	 */
	void CVideoOptionsWindowLayout::BuildHostPage( INT _iRightW, std::vector<LSW_WIDGET_LAYOUT> &_vLayout ) {
		_vLayout.clear();
		size_t sGroups = 0;
		const CVideoOptionsFields::LSN_FIELD_GROUP * pfgGroups = CVideoOptionsFields::LeftGroups( CVideoOptionsFields::LSN_VPK_HOST, sGroups );

		const INT iLeftL = LSN_LEFT_JUST;
		const INT iRightL = iLeftL + LSN_VO_LEFT_W + LSN_LEFT_JUST;
		const INT iPageW = iRightL + _iRightW + LSN_LEFT_JUST;
		const INT iGroupW = iPageW - LSN_LEFT_JUST * 2;
		const INT iInnerL = iLeftL + LSN_GROUP_LEFT;
		const INT iInnerW = iGroupW - LSN_GROUP_LEFT * 2;
		const INT iCtrlW = LSN_VO_LEFT_W - LSN_GROUP_LEFT * 2 - LSN_VO_LABEL_W;
		const INT iDescL = iRightL + LSN_GROUP_LEFT;
		const INT iDescW = iLeftL + iGroupW - LSN_GROUP_LEFT - iDescL;

		LSW_WIDGET_LAYOUT wlPage = {};
		wlPage.ltType = LSN_LT_VIDEO_OPTIONS_FIELD_PAGE;
		wlPage.wId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
		wlPage.bEnabled = TRUE;
		wlPage.dwWidth = DWORD( iPageW );
		wlPage.dwStyle = WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL;
		wlPage.dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT;
		wlPage.dwParentId = CWinUtilities::LSN_VOWI_NONE;
		_vLayout.push_back( wlPage );

		INT iTop = LSN_TOP_JUST;
		for ( size_t I = 0; I < sGroups; ++I ) {
			const CVideoOptionsFields::LSN_FIELD_GROUP & fgGroup = pfgGroups[I];
			const size_t sGroupIdx = _vLayout.size();
			LSW_WIDGET_LAYOUT wlGroup = {};
			wlGroup.ltType = LSW_LT_GROUPBOX;
			wlGroup.wId = WORD( CWinUtilities::LSN_VOWI_GROUP_FIRST + I );
			wlGroup.lpwcClass = WC_BUTTONW;
			wlGroup.bEnabled = TRUE;
			wlGroup.iLeft = iLeftL;
			wlGroup.iTop = iTop;
			wlGroup.dwWidth = DWORD( iGroupW );
			wlGroup.dwStyle = LSN_GROUPSTYLE;
			wlGroup.dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
			wlGroup.pwcText = fgGroup.pwcLabel;
			wlGroup.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
			_vLayout.push_back( wlGroup );

			INT iRowTop = iTop + LSN_GROUP_TOP;
			for ( size_t J = 0; J < fgGroup.sTotal; ++J ) {
				const CVideoOptionsFields::LSN_VIDEO_FIELD vfField = fgGroup.pvfFields[J];
				const CVideoOptionsFields::LSN_FIELD_INFO & fiInfo = CVideoOptionsFields::Info( vfField );

				// The label and the combo box (the description below replaces their tool tips).
				LSW_WIDGET_LAYOUT wlLabel = {};
				wlLabel.ltType = LSW_LT_LABEL;
				wlLabel.wId = LabelId( vfField );
				wlLabel.bEnabled = TRUE;
				wlLabel.iLeft = iInnerL;
				wlLabel.iTop = iRowTop + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1);
				wlLabel.dwWidth = LSN_VO_LABEL_W;
				wlLabel.dwHeight = LSN_DEF_STATIC_HEIGHT;
				wlLabel.dwStyle = LSN_STATICSTYLE;
				wlLabel.pwcText = fiInfo.pwcLabel;
				wlLabel.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
				_vLayout.push_back( wlLabel );

				LSW_WIDGET_LAYOUT wlCombo = {};
				wlCombo.ltType = LSW_LT_COMBOBOX;
				wlCombo.wId = ControlId( vfField );
				wlCombo.lpwcClass = WC_COMBOBOXW;
				wlCombo.bEnabled = TRUE;
				wlCombo.iLeft = iInnerL + LSN_VO_LABEL_W;
				wlCombo.iTop = iRowTop;
				wlCombo.dwWidth = DWORD( iCtrlW );
				wlCombo.dwHeight = LSN_DEF_COMBO_HEIGHT;
				wlCombo.dwStyle = LSN_COMBOSTYLE_LIST;
				wlCombo.dwStyleEx = LSN_COMBOSTYLEEX_LIST;
				wlCombo.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
				_vLayout.push_back( wlCombo );

				// The description (its text is set and its height measured by the page).
				LSW_WIDGET_LAYOUT wlDesc = {};
				wlDesc.ltType = LSW_LT_LABEL;
				wlDesc.wId = DescriptionId( vfField );
				wlDesc.bEnabled = TRUE;
				wlDesc.iLeft = iDescL;
				wlDesc.iTop = iRowTop + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1);
				wlDesc.dwWidth = DWORD( iDescW );
				wlDesc.dwHeight = LSN_VO_DESC_H;
				wlDesc.dwStyle = LSN_STATICSTYLE | SS_NOPREFIX;
				wlDesc.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
				_vLayout.push_back( wlDesc );
				iRowTop += std::max<INT>( LSN_VO_ROW_H, LSN_VO_DESC_H );

				// A separator between settings.
				if ( J + 1 < fgGroup.sTotal ) {
					LSW_WIDGET_LAYOUT wlSep = {};
					wlSep.ltType = LSW_LT_LABEL;
					wlSep.wId = SeparatorId( vfField );
					wlSep.bEnabled = TRUE;
					wlSep.iLeft = iInnerL;
					wlSep.iTop = iRowTop + LSN_TOP_JUST * 2;
					wlSep.dwWidth = DWORD( iInnerW );
					wlSep.dwHeight = 1;
					wlSep.dwStyle = WS_CHILDWINDOW | WS_VISIBLE | SS_ETCHEDHORZ;
					wlSep.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
					_vLayout.push_back( wlSep );
					iRowTop += LSN_TOP_JUST * 5;
				}
			}
			_vLayout[sGroupIdx].dwHeight = DWORD( iRowTop + LSN_GROUP_BOTTOM - iTop );
			iTop += INT( _vLayout[sGroupIdx].dwHeight );
		}

		LSW_WIDGET_LAYOUT wlDefaults = DefaultsButton( CWinUtilities::LSN_VOWI_DEFAULTS, iPageW - LSN_LEFT_JUST - LSN_VO_DEFAULTS_W, iTop + LSN_TOP_JUST,
			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_RESTORE_HOST_DEFAULTS )) );
		_vLayout.push_back( wlDefaults );
		_vLayout[0].dwHeight = DWORD( wlDefaults.iTop + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST );
	}

	/**
	 * Gets the width of the right column of the pages, in dialog units:  just wide enough for the palette display (384 pixels plus its
	 *	border and scroll bar) inside its group.
	 *
	 * \param _pwParent The parent of the pages (a dialog with the same font as the pages).
	 * \return Returns the width of the right column, in dialog units.
	 */
	INT CVideoOptionsWindowLayout::RightColumnWidth( CWidget * _pwParent ) {
		if ( !_pwParent || !_pwParent->Wnd() ) { return LSN_VO_RIGHT_W; }
		// Pixels per 1,000 horizontal dialog units.
		RECT rUnits = { 0, 0, 1000, 0 };
		if ( !::MapDialogRect( _pwParent->Wnd(), &rUnits ) || rUnits.right <= 0 ) { return LSN_VO_RIGHT_W; }
		const LONG lPixels = LSN_PALETTE_DISPLAY_W + ::GetSystemMetrics( SM_CXVSCROLL ) + ::GetSystemMetrics( SM_CXEDGE ) * 2;
		// Rounded up (plus 1 for the rounding of the dialog units into pixels).
		const INT iDisplayW = INT( (lPixels * 1000 + rUnits.right - 1) / rUnits.right ) + 1;
		return std::max<INT>( LSN_VO_RIGHT_MIN_W, iDisplayW + LSN_GROUP_LEFT * 2 );
	}

	/**
	 * Creates the layout of a Restore Defaults button.
	 *
	 * \param _wId The control ID of the button.
	 * \param _iLeft The left of the button.
	 * \param _iTop The top of the button.
	 * \param _pcToolTip The tool tip of the button (UTF-8).
	 * \return Returns the layout of the button.
	 */
	LSW_WIDGET_LAYOUT CVideoOptionsWindowLayout::DefaultsButton( WORD _wId, INT _iLeft, INT _iTop, const char * _pcToolTip ) {
		LSW_WIDGET_LAYOUT wlDefaults = {};
		wlDefaults.ltType = LSW_LT_BUTTON;
		wlDefaults.wId = _wId;
		wlDefaults.lpwcClass = WC_BUTTONW;
		wlDefaults.bEnabled = TRUE;
		wlDefaults.iLeft = _iLeft;
		wlDefaults.iTop = _iTop;
		wlDefaults.dwWidth = LSN_VO_DEFAULTS_W;
		wlDefaults.dwHeight = LSN_DEF_BUTTON_HEIGHT;
		wlDefaults.dwStyle = LSN_BUTTONSTYLE;
		wlDefaults.dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
		wlDefaults.pwcText = LSN_LSTR( LSN_STR_VO_RESTORE_DEFAULTS );
		wlDefaults.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
		wlDefaults.pcToolTip = _pcToolTip;
		wlDefaults.dwToolTipStyleEx = WS_EX_TOPMOST;
		wlDefaults.dwToolTipStyle = LSN_TOOLTIP_STYLE;
		return wlDefaults;
	}

	/**
	 * Adds a column of groups to the layout of a page.
	 *
	 * \param _pfgGroups The groups.
	 * \param _sTotal The number of groups.
	 * \param _iLeft The left of the column.
	 * \param _iTop The top of the column.
	 * \param _iWidth The width of the column.
	 * \param _wFirstGroupId The control ID of the first group box.
	 * \param _vLayout The layout to which to add the column.
	 * \return Returns the bottom of the column.
	 */
	INT CVideoOptionsWindowLayout::AddColumn( const CVideoOptionsFields::LSN_FIELD_GROUP * _pfgGroups, size_t _sTotal, INT _iLeft, INT _iTop, INT _iWidth,
		WORD _wFirstGroupId, std::vector<LSW_WIDGET_LAYOUT> &_vLayout ) {
		INT iTop = _iTop;
		const INT iInnerL = _iLeft + LSN_GROUP_LEFT;
		const INT iInnerW = _iWidth - LSN_GROUP_LEFT * 2;
		const INT iCtrlL = iInnerL + LSN_VO_LABEL_W;
		const INT iCtrlW = iInnerW - LSN_VO_LABEL_W;
		for ( size_t I = 0; I < _sTotal; ++I ) {
			const CVideoOptionsFields::LSN_FIELD_GROUP & fgGroup = _pfgGroups[I];
			// The height of the group.
			INT iRows = 0;
			for ( size_t J = 0; J < fgGroup.sTotal; ++J ) {
				iRows += (CVideoOptionsFields::Info( fgGroup.pvfFields[J] ).vftType == CVideoOptionsFields::LSN_VFT_DISPLAY) ? (LSN_VO_DISPLAY_H + LSN_TOP_JUST) : LSN_VO_ROW_H;
			}
			// The group's own Restore Defaults button.
			if ( fgGroup.wDefaultsId ) { iRows += LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST; }
			LSW_WIDGET_LAYOUT wlGroup = {};
			wlGroup.ltType = LSW_LT_GROUPBOX;
			wlGroup.wId = WORD( _wFirstGroupId + I );
			wlGroup.lpwcClass = WC_BUTTONW;
			wlGroup.bEnabled = TRUE;
			wlGroup.iLeft = _iLeft;
			wlGroup.iTop = iTop;
			wlGroup.dwWidth = DWORD( _iWidth );
			wlGroup.dwHeight = DWORD( LSN_GROUP_TOP + iRows + LSN_GROUP_BOTTOM );
			wlGroup.dwStyle = LSN_GROUPSTYLE;
			wlGroup.dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
			wlGroup.pwcText = fgGroup.pwcLabel;
			wlGroup.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
			_vLayout.push_back( wlGroup );

			INT iRowTop = iTop + LSN_GROUP_TOP;
			for ( size_t J = 0; J < fgGroup.sTotal; ++J ) {
				const CVideoOptionsFields::LSN_VIDEO_FIELD vfField = fgGroup.pvfFields[J];
				const CVideoOptionsFields::LSN_FIELD_INFO & fiInfo = CVideoOptionsFields::Info( vfField );

				LSW_WIDGET_LAYOUT wlCtrl = {};
				wlCtrl.wId = ControlId( vfField );
				wlCtrl.bEnabled = TRUE;
				wlCtrl.dwParentId = CWinUtilities::LSN_VOWI_FIELD_PAGE;
				wlCtrl.pcToolTip = fiInfo.pcToolTip;
				wlCtrl.dwToolTipStyleEx = WS_EX_TOPMOST;
				wlCtrl.dwToolTipStyle = LSN_TOOLTIP_STYLE;

				switch ( fiInfo.vftType ) {
					case CVideoOptionsFields::LSN_VFT_CHECK : {
						wlCtrl.ltType = LSW_LT_CHECK;
						wlCtrl.lpwcClass = WC_BUTTONW;
						wlCtrl.iLeft = iInnerL;
						wlCtrl.iTop = iRowTop + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1);
						wlCtrl.dwWidth = DWORD( iInnerW );
						wlCtrl.dwHeight = LSN_DEF_CHECK_HEIGHT;
						wlCtrl.dwStyle = LSN_CHECKSTYLE;
						wlCtrl.dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
						wlCtrl.pwcText = fiInfo.pwcLabel;
						_vLayout.push_back( wlCtrl );
						iRowTop += LSN_VO_ROW_H;
						break;
					}
					case CVideoOptionsFields::LSN_VFT_DISPLAY : {
						wlCtrl.ltType = LSW_LT_LABEL;
						wlCtrl.iLeft = iInnerL;
						wlCtrl.iTop = iRowTop;
						wlCtrl.dwWidth = DWORD( iInnerW );
						wlCtrl.dwHeight = LSN_VO_DISPLAY_H;
						wlCtrl.dwStyle = WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | SS_NOTIFY;
						wlCtrl.dwStyleEx = WS_EX_CLIENTEDGE;
						_vLayout.push_back( wlCtrl );
						iRowTop += LSN_VO_DISPLAY_H + LSN_TOP_JUST;
						break;
					}
					default : {
						// A label and a control.
						LSW_WIDGET_LAYOUT wlLabel = wlCtrl;
						wlLabel.ltType = LSW_LT_LABEL;
						wlLabel.wId = LabelId( vfField );
						wlLabel.iLeft = iInnerL;
						wlLabel.iTop = iRowTop + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1);
						wlLabel.dwWidth = LSN_VO_LABEL_W;
						wlLabel.dwHeight = LSN_DEF_STATIC_HEIGHT;
						wlLabel.dwStyle = LSN_STATICSTYLE;
						wlLabel.pwcText = fiInfo.pwcLabel;
						_vLayout.push_back( wlLabel );

						wlCtrl.iLeft = iCtrlL;
						wlCtrl.iTop = iRowTop;
						wlCtrl.dwWidth = DWORD( iCtrlW );
						if ( fiInfo.vftType == CVideoOptionsFields::LSN_VFT_COMBO ) {
							wlCtrl.ltType = LSW_LT_COMBOBOX;
							wlCtrl.lpwcClass = WC_COMBOBOXW;
							wlCtrl.dwHeight = LSN_DEF_COMBO_HEIGHT;
							wlCtrl.dwStyle = LSN_COMBOSTYLE_LIST;
							wlCtrl.dwStyleEx = LSN_COMBOSTYLEEX_LIST;
						}
						else {
							wlCtrl.ltType = LSW_LT_EDIT;
							wlCtrl.dwHeight = LSN_DEF_EDIT_HEIGHT;
							wlCtrl.dwStyle = LSN_EDITSTYLE;
							wlCtrl.dwStyleEx = WS_EX_CLIENTEDGE;
						}
						_vLayout.push_back( wlCtrl );
						iRowTop += LSN_VO_ROW_H;
					}
				}
			}
			if ( fgGroup.wDefaultsId ) {
				_vLayout.push_back( DefaultsButton( fgGroup.wDefaultsId, _iLeft + _iWidth - LSN_GROUP_LEFT - LSN_VO_DEFAULTS_W, iRowTop + LSN_TOP_JUST,
					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_RESTORE_CONV_DEFAULTS )) ) );
			}
			iTop += INT( wlGroup.dwHeight );
		}
		return iTop;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
