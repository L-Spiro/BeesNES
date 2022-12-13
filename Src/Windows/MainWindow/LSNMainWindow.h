#ifdef LSN_USE_WINDOWS

#pragma once

#include "../../Display/LSNDisplayHost.h"
#include "../../System/LSNSystem.h"
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsw {
	class CStatusBar;
}

namespace lsn {
	
	class CMainWindow : public lsw::CMainWindow, public lsn::CDisplayHost {
		typedef lsn::CNtscSystem				CRegionalSystem;
	public :
		CMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
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

		// WM_GETMINMAXINFO.
		virtual LSW_HANDLED						GetMinMaxInfo( MINMAXINFO * _pmmiInfo );

		// WM_PAINT.
		virtual LSW_HANDLED						Paint();

		// WM_MOVE.
		virtual LSW_HANDLED						Move( LONG _lX, LONG _lY );

		/**
		 * Advances the emulation state by the amount of time that has passed since the last advancement.
		 */
		void									Tick();

		/**
		 * Informs the host that a frame has been rendered.  This typically causes a display update and a framebuffer swap.
		 */
		virtual void							Swap();

		/**
		 * Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		 *
		 * \param _pwChild Optional child control.
		 * \return Returns the virtual client rectangle of this object or of the optional child object.
		 */
		virtual const LSW_RECT					VirtualClientRect( const CWidget * _pwChild ) const;

		/**
		 * Gets the final display width.
		 *
		 * \return Returns the final display width (native width * scale * ratio).
		 */
		LONG									FinalWidth() const { return m_pdcClient ? LONG( std::round( m_pdcClient->DisplayWidth() * m_dScale * m_dRatio ) ) : 0; }

		/**
		 * Gets the final display height.
		 *
		 * \return Returns the final display height (native height * scale).
		 */
		LONG									FinalHeight() const { return m_pdcClient ? LONG( std::round( m_pdcClient->DisplayHeight() * m_dScale ) ) : 0; }

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
		uint64_t								m_ui64TickCount;
		// The output scale.
		double									m_dScale;
		// The output ratio.
		double									m_dRatio;
		// Outside "is alive" atomic.
		std::atomic_bool *						m_pabIsAlive;
		// A clock.
		lsn::CClock								m_cClock;
		// The console pointer.
		std::unique_ptr<CSystemBase>			m_pnsSystem;
		// Image list.
		lsw::CImageList							m_iImages;
		// Images.
		lsw::CBitmap							m_bBitmaps[LSN_I_TOTAL];
		/** Image mapping. */
		INT										m_iImageMap[LSN_I_TOTAL];
		/** The BITMAP reender target buffer cast to a BITMAPINFO object. */
		size_t									m_stBufferIdx;
		/** The BITMAP render target for very basic software rendering.  N-buffered. */
		std::vector<std::vector<uint8_t>>		m_vBasicRenderTarget;
		


		// == Functions.
		/**
		 * Gets the window rectangle for correct output at a given scale and ratio.
		 *
		 * \return Returns the window rectangle for a given client area, derived from the desired output scale and ratio.
		 */
		LSW_RECT								FinalWindowRect() const;

		/**
		 * Sends a given palette to the console.
		 *
		 * \param _vPalette The loaded palette file.  Must be (0x40 * 3) bytes.
		 */
		void									SetPalette( const std::vector<uint8_t> &_vPalette );

		/**
		 * Gets a BITMAP stride given its row width in bytes.
		 *
		 * \param _ui32RowWidth The row width in RGB(A) pixels.
		 * \param _ui32BitDepth The total bits for a single RGB(A) pixel.
		 * \return Reurns the byte width rounded up to the nearest DWORD.
		 */
		static inline DWORD						RowStride( uint32_t _ui32RowWidth, uint32_t _ui32BitDepth ) {
			return ((((_ui32RowWidth * _ui32BitDepth) + 31) & ~31) >> 3);
		}
		
	};

}

#endif	// #ifdef LSN_USE_WINDOWS