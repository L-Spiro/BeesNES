#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Displays a palette as rows of 16 colored squares, optionally with the index of each color.
 */

#pragma once

#include "../../LSNLSpiroNes.h"

#include <Widget/LSWWidget.h>

#include <vector>

namespace lsn {

	/**
	 * Class CPaletteDisplay
	 * \brief Displays a palette as rows of 16 colored squares.
	 *
	 * Description: Displays a palette as rows of 16 colored squares (24-by-24 pixels each), optionally with the index of each color in
	 *	the upper-right corner of its square.  The vertical scroll bar is always shown (it is disabled when every row fits).
	 */
	class CPaletteDisplay : public lsw::CWidget {
	public :
		CPaletteDisplay( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CPaletteDisplay();


		// == Enumerations.
		/** Sizes. */
		enum LSN_SIZES : int32_t {
			LSN_SQUARE										= 24,										/**< The width and height of each color, in pixels. */
			LSN_COLUMNS										= 16,										/**< The number of colors in each row. */
			LSN_TEXT_PAD									= 2,										/**< The space between an index and the edges of its square, in pixels. */
			LSN_FONT_H										= 11,										/**< The height of the index font, in pixels. */
		};


		// == Functions.
		/**
		 * Gets the name of the window class of the control, registering the class the first time.  The class uses ::DefWindowProcW()
		 *	(everything else is done by SubclassProc()).
		 *
		 * \return Returns the name of the window class.
		 */
		static const wchar_t *								ClassName();

		/**
		 * Attaches the window after the dialog creates it and subclasses it (dialog controls are not routed through CWidget::WindowProc()).
		 *
		 * \param _hWnd The created window.
		 */
		virtual void										InitControl( HWND _hWnd );

		/**
		 * Sets the colors to display.
		 *
		 * \param _vColors The colors to display.
		 */
		void												SetColors( const std::vector<COLORREF> &_vColors );

		/**
		 * Sets whether the index of each color is shown.
		 *
		 * \param _bShow If true, the index of each color is shown in the upper-right corner of its square.
		 */
		void												SetShowIndices( bool _bShow );

		/**
		 * Gets whether the index of each color is shown.
		 *
		 * \return Returns true if the index of each color is shown.
		 */
		inline bool											ShowIndices() const { return m_bShowIndices; }

		/**
		 * Updates the range, page, and position of the vertical scroll bar to fit the colors and the client area.
		 */
		void												UpdateScrollBar();

		/**
		 * The WM_ERASEBKGND handler.
		 *
		 * \param _hDc The device context.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									EraseBkgnd( HDC _hDc );

		/**
		 * The WM_PAINT handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Paint();

		/**
		 * The WM_SIZE handler.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		/**
		 * The WM_VSCROLL handler.
		 *
		 * \param _uScrollPos The current position of the scroll box if _uScrollType is SB_THUMBPOSITION or SB_THUMBTRACK.
		 * \param _uScrollType The scrolling request.
		 * \param _pwWidget The scroll-bar control, or nullptr for the scroll bar of this window.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									VScroll( USHORT _uScrollPos, USHORT _uScrollType, lsw::CWidget * _pwWidget );

		/**
		 * The WM_MOUSEWHEEL handler.
		 *
		 * \param _dwVirtKeys The virtual keys that are down (low word) and the distance the wheel was rotated (high word).
		 * \param _pCursorPos The position of the cursor, in screen coordinates.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									MouseWheel( DWORD _dwVirtKeys, const POINTS &_pCursorPos );


	protected :
		// == Members.
		static constexpr UINT_PTR							LSN_SUBCLASS_ID = 0x9A1E7E00;				/**< The ID of the subclass. */
		std::vector<COLORREF>								m_vColors;									/**< The colors to display. */
		HFONT												m_hFont = NULL;								/**< The font of the indices. */
		int32_t												m_i32Scroll = 0;							/**< The vertical scroll position, in pixels. */
		int32_t												m_i32Wheel = 0;								/**< Mouse-wheel rotation not yet turned into scrolling. */
		bool												m_bShowIndices = false;						/**< Whether the index of each color is shown. */


		// == Functions.
		/**
		 * Gets the height of all of the rows, in pixels.
		 *
		 * \return Returns the height of all of the rows, in pixels.
		 */
		int32_t												ContentHeight() const;

		/**
		 * Gets the height of the client area, in pixels.
		 *
		 * \return Returns the height of the client area, in pixels.
		 */
		int32_t												ClientHeight() const;

		/**
		 * Scrolls to a position (clamped to the scrollable range) and redraws.
		 *
		 * \param _i32Pos The new scroll position, in pixels.
		 */
		void												ScrollTo( int32_t _i32Pos );

		/**
		 * Gets the font of the indices, creating it from the font of the control the first time.
		 *
		 * \return Returns the font of the indices, or NULL if it could not be created.
		 */
		HFONT												IndexFont();

		/**
		 * Determines whether black text is easier to read than white text over a color.
		 *
		 * \param _crColor The color behind the text.
		 * \return Returns true if black text should be used.
		 */
		static bool											UseBlackText( COLORREF _crColor );

		/**
		 * The subclass procedure of the control.
		 *
		 * \param _hWnd The control.
		 * \param _uMsg The message.
		 * \param _wParam The WPARAM of the message.
		 * \param _lParam The LPARAM of the message.
		 * \param _uiptrId The ID of the subclass.
		 * \param _dwptrRef A pointer to the CPaletteDisplay.
		 * \return Returns the result of the message.
		 */
		static LRESULT CALLBACK								SubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uiptrId, DWORD_PTR _dwptrRef );


	private :
		typedef lsw::CWidget								Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
