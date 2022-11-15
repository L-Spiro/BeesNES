#ifdef LSN_USE_WINDOWS

#pragma once

#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <MainWindow/LSWMainWindow.h>

namespace lsw {
	class CStatusBar;
}

namespace lsn {
	
	class CMainWindow : public lsw::CMainWindow {
	public :
		CMainWindow( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CMainWindow();


		// == Enumerations.
		// Images.
		enum LSN_IMAGES {
			LSN_I_OPENROM,
			LSN_I_OPTIONS,

			LSN_I_TOTAL,
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED						InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED						Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED						MenuCommand( WORD _Id ) { return Command( 0, _Id, nullptr ); }

		// WM_NCDESTROY.
		virtual LSW_HANDLED						NcDestroy();

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const lsw::LSW_RECT				VirtualClientRect( const lsw::CWidget * _pwChild ) const;

		/**
		 * Gets the status bar.
		 *
		 * \return the status bar.
		 */
		lsw::CStatusBar *						StatusBar();

		/**
		 * Gets the status bar.
		 *
		 * \return the status bar.
		 */
		const lsw::CStatusBar *					StatusBar() const;


	protected :
		// == Members.
		// Image list.
		lsw::CImageList							m_iImages;

		// Images.
		lsw::CBitmap							m_bBitmaps[LSN_I_TOTAL];

		// Image mapping.
		INT										m_iImageMap[LSN_I_TOTAL];
	};

}

#endif	// #ifdef LSN_USE_WINDOWS