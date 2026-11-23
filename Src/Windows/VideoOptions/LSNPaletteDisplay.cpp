#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Displays a palette as rows of 16 colored squares, optionally with the index of each color.
 */

#include "LSNPaletteDisplay.h"

#include <algorithm>
#include <cmath>
#include <format>


namespace lsn {

	CPaletteDisplay::CPaletteDisplay( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CPaletteDisplay::~CPaletteDisplay() {
		if ( m_hFont ) {
			::DeleteObject( m_hFont );
			m_hFont = NULL;
		}
	}

	// == Functions.
	/**
	 * Sets the colors to display.
	 *
	 * \param _vColors The colors to display.
	 */
	void CPaletteDisplay::SetColors( const std::vector<COLORREF> &_vColors ) {
		if ( _vColors == m_vColors ) { return; }
		try {
			m_vColors = _vColors;
		}
		catch ( ... ) { m_vColors.clear(); }
		UpdateScrollBar();
		if ( Wnd() ) { ::InvalidateRect( Wnd(), nullptr, FALSE ); }
	}

	/**
	 * Sets whether the index of each color is shown.
	 *
	 * \param _bShow If true, the index of each color is shown in the upper-right corner of its square.
	 */
	void CPaletteDisplay::SetShowIndices( bool _bShow ) {
		if ( _bShow == m_bShowIndices ) { return; }
		m_bShowIndices = _bShow;
		if ( Wnd() ) { ::InvalidateRect( Wnd(), nullptr, FALSE ); }
	}

	/**
	 * Updates the range, page, and position of the vertical scroll bar to fit the colors and the client area.
	 */
	void CPaletteDisplay::UpdateScrollBar() {
		if ( !Wnd() ) { return; }
		const int32_t i32Page = ClientHeight();
		const int32_t i32Content = ContentHeight();
		m_i32Scroll = std::clamp( m_i32Scroll, 0, std::max( i32Content - i32Page, 0 ) );

		SCROLLINFO siInfo = { sizeof( siInfo ) };
		siInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
		siInfo.nMin = 0;
		siInfo.nMax = std::max( i32Content, 1 ) - 1;
		siInfo.nPage = UINT( std::max( i32Page, 0 ) );
		siInfo.nPos = m_i32Scroll;
		::SetScrollInfo( Wnd(), SB_VERT, &siInfo, TRUE );
	}

	/**
	 * The WM_ERASEBKGND handler.
	 *
	 * \param _hDc The device context.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CPaletteDisplay::EraseBkgnd( HDC /*_hDc*/ ) {
		// Paint() covers the whole client area.
		return LSW_H_HANDLED;
	}

	/**
	 * The WM_PAINT handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CPaletteDisplay::Paint() {
		PAINTSTRUCT psPaint;
		HDC hDc = ::BeginPaint( Wnd(), &psPaint );
		if ( !hDc ) { return LSW_H_HANDLED; }

		RECT rClient;
		::GetClientRect( Wnd(), &rClient );
		const int iW = int( rClient.right - rClient.left ), iH = int( rClient.bottom - rClient.top );
		if ( iW > 0 && iH > 0 ) {
			// Drawn off-screen and copied over so that scrolling does not flicker.
			HDC hMem = ::CreateCompatibleDC( hDc );
			HBITMAP hBmp = hMem ? ::CreateCompatibleBitmap( hDc, iW, iH ) : NULL;
			HDC hTarget = hBmp ? hMem : hDc;
			HGDIOBJ hOldBmp = hBmp ? ::SelectObject( hMem, hBmp ) : NULL;

			::FillRect( hTarget, &rClient, ::GetSysColorBrush( COLOR_WINDOW ) );

			HGDIOBJ hDcBrush = ::GetStockObject( DC_BRUSH );
			HFONT hFont = m_bShowIndices ? IndexFont() : NULL;
			HGDIOBJ hOldFont = hFont ? ::SelectObject( hTarget, hFont ) : NULL;
			::SetBkMode( hTarget, TRANSPARENT );
			// 2 hexadecimal digits for 64 colors, 3 for 512.
			const bool bThreeDigits = m_vColors.size() > 0x100;

			const size_t sFirstRow = size_t( m_i32Scroll / LSN_SQUARE );
			const size_t sRows = (m_vColors.size() + LSN_COLUMNS - 1) / LSN_COLUMNS;
			for ( size_t R = sFirstRow; R < sRows; ++R ) {
				const int iTop = int( R ) * LSN_SQUARE - m_i32Scroll;
				if ( iTop >= iH ) { break; }
				for ( size_t C = 0; C < size_t( LSN_COLUMNS ); ++C ) {
					const size_t sIdx = R * LSN_COLUMNS + C;
					if ( sIdx >= m_vColors.size() ) { break; }
					RECT rSquare = { LONG( C * LSN_SQUARE ), LONG( iTop ), LONG( (C + 1) * LSN_SQUARE ), LONG( iTop + LSN_SQUARE ) };
					::SetDCBrushColor( hTarget, m_vColors[sIdx] );
					::FillRect( hTarget, &rSquare, static_cast<HBRUSH>(hDcBrush) );
					if ( hFont ) {
						std::wstring wsIdx = bThreeDigits ? std::format( L"{:03X}", sIdx ) : std::format( L"{:02X}", sIdx );
						RECT rText = { rSquare.left + LSN_TEXT_PAD, rSquare.top + LSN_TEXT_PAD, rSquare.right - LSN_TEXT_PAD, rSquare.bottom - LSN_TEXT_PAD };
						::SetTextColor( hTarget, UseBlackText( m_vColors[sIdx] ) ? RGB( 0x00, 0x00, 0x00 ) : RGB( 0xFF, 0xFF, 0xFF ) );
						::DrawTextW( hTarget, wsIdx.c_str(), int( wsIdx.size() ), &rText, DT_RIGHT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP );
					}
				}
			}

			if ( hOldFont ) { ::SelectObject( hTarget, hOldFont ); }
			if ( hBmp ) {
				::BitBlt( hDc, 0, 0, iW, iH, hMem, 0, 0, SRCCOPY );
				::SelectObject( hMem, hOldBmp );
				::DeleteObject( hBmp );
			}
			if ( hMem ) { ::DeleteDC( hMem ); }
		}
		::EndPaint( Wnd(), &psPaint );
		return LSW_H_HANDLED;
	}

	/**
	 * The WM_SIZE handler.
	 *
	 * \param _wParam The type of resizing requested.
	 * \param _lWidth The new width of the client area.
	 * \param _lHeight The new height of the client area.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CPaletteDisplay::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		UpdateScrollBar();
		::InvalidateRect( Wnd(), nullptr, FALSE );
		return Parent::Size( _wParam, _lWidth, _lHeight );
	}

	/**
	 * The WM_VSCROLL handler.
	 *
	 * \param _uScrollPos The current position of the scroll box if _uScrollType is SB_THUMBPOSITION or SB_THUMBTRACK.
	 * \param _uScrollType The scrolling request.
	 * \param _pwWidget The scroll-bar control, or nullptr for the scroll bar of this window.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CPaletteDisplay::VScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, lsw::CWidget * _pwWidget ) {
		if ( _pwWidget ) { return LSW_H_CONTINUE; }
		const int32_t i32Page = std::max( ClientHeight() / LSN_SQUARE, 1 ) * LSN_SQUARE;
		switch ( _uScrollType ) {
			case SB_LINEUP : { ScrollTo( m_i32Scroll - LSN_SQUARE ); break; }
			case SB_LINEDOWN : { ScrollTo( m_i32Scroll + LSN_SQUARE ); break; }
			case SB_PAGEUP : { ScrollTo( m_i32Scroll - i32Page ); break; }
			case SB_PAGEDOWN : { ScrollTo( m_i32Scroll + i32Page ); break; }
			case SB_TOP : { ScrollTo( 0 ); break; }
			case SB_BOTTOM : { ScrollTo( ContentHeight() ); break; }
			case SB_THUMBPOSITION : {}			LSN_FALLTHROUGH
			case SB_THUMBTRACK : {
				// The 32-bit position (the 16-bit one in the message can overflow).
				SCROLLINFO siInfo = { sizeof( siInfo ) };
				siInfo.fMask = SIF_TRACKPOS;
				if ( ::GetScrollInfo( Wnd(), SB_VERT, &siInfo ) ) { ScrollTo( siInfo.nTrackPos ); }
				break;
			}
			default : {}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * The WM_MOUSEWHEEL handler.
	 *
	 * \param _dwVirtKeys The virtual keys that are down (low word) and the distance the wheel was rotated (high word).
	 * \param _pCursorPos The position of the cursor, in screen coordinates.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CPaletteDisplay::MouseWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ ) {
		// One row per notch.
		m_i32Wheel += int16_t( HIWORD( _dwVirtKeys ) );
		int32_t i32Rows = m_i32Wheel / WHEEL_DELTA;
		if ( i32Rows ) {
			m_i32Wheel -= i32Rows * WHEEL_DELTA;
			ScrollTo( m_i32Scroll - i32Rows * LSN_SQUARE );
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Gets the height of all of the rows, in pixels.
	 *
	 * \return Returns the height of all of the rows, in pixels.
	 */
	int32_t CPaletteDisplay::ContentHeight() const {
		return int32_t( (m_vColors.size() + LSN_COLUMNS - 1) / LSN_COLUMNS ) * LSN_SQUARE;
	}

	/**
	 * Gets the height of the client area, in pixels.
	 *
	 * \return Returns the height of the client area, in pixels.
	 */
	int32_t CPaletteDisplay::ClientHeight() const {
		RECT rClient = {};
		if ( !Wnd() || !::GetClientRect( Wnd(), &rClient ) ) { return 0; }
		return int32_t( rClient.bottom - rClient.top );
	}

	/**
	 * Scrolls to a position (clamped to the scrollable range) and redraws.
	 *
	 * \param _i32Pos The new scroll position, in pixels.
	 */
	void CPaletteDisplay::ScrollTo( int32_t _i32Pos ) {
		const int32_t i32Pos = std::clamp( _i32Pos, 0, std::max( ContentHeight() - ClientHeight(), 0 ) );
		if ( i32Pos == m_i32Scroll ) { return; }
		m_i32Scroll = i32Pos;
		::SetScrollPos( Wnd(), SB_VERT, m_i32Scroll, TRUE );
		::InvalidateRect( Wnd(), nullptr, FALSE );
	}

	/**
	 * Gets the font of the indices, creating it from the font of the control the first time.
	 *
	 * \return Returns the font of the indices, or NULL if it could not be created.
	 */
	HFONT CPaletteDisplay::IndexFont() {
		if ( m_hFont ) { return m_hFont; }
		LOGFONTW lfFont = {};
		HFONT hBase = reinterpret_cast<HFONT>(::SendMessageW( Wnd(), WM_GETFONT, 0, 0 ));
		if ( !hBase || !::GetObjectW( hBase, sizeof( lfFont ), &lfFont ) ) {
			NONCLIENTMETRICSW ncmMetrics = { sizeof( ncmMetrics ) };
			if ( !::SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, ncmMetrics.cbSize, &ncmMetrics, 0 ) ) { return NULL; }
			lfFont = ncmMetrics.lfMessageFont;
		}
		// The squares have a fixed size in pixels, so the font does too (3 digits must fit in a square).
		lfFont.lfHeight = -LSN_FONT_H;
		lfFont.lfWidth = 0;
		m_hFont = ::CreateFontIndirectW( &lfFont );
		return m_hFont;
	}

	/**
	 * Determines whether black text is easier to read than white text over a color.
	 *
	 * \param _crColor The color behind the text.
	 * \return Returns true if black text should be used.
	 */
	bool CPaletteDisplay::UseBlackText( COLORREF _crColor ) {
		// The relative luminance (WCAG); above ~0.179, black text has more contrast than white text.
		auto Linear = []( BYTE _bVal ) {
			const double dVal = _bVal / 255.0;
			return dVal <= 0.04045 ? dVal / 12.92 : std::pow( (dVal + 0.055) / 1.055, 2.4 );
		};
		const double dLum = 0.2126 * Linear( GetRValue( _crColor ) ) + 0.7152 * Linear( GetGValue( _crColor ) ) + 0.0722 * Linear( GetBValue( _crColor ) );
		return dLum > 0.179;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
