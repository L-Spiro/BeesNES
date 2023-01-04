#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The main window of the emulator.
 */

#pragma once

#include "../../Display/LSNDisplayHost.h"
#include "../../Filters/LSNRgb24Filter.h"
#include "../../Input/LSNInputPoller.h"
#include "../../System/LSNSystem.h"
#include <CriticalSection/LSWCriticalSection.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <MainWindow/LSWMainWindow.h>
#include <thread>

using namespace lsw;

namespace lsw {
	class CStatusBar;
}

namespace lsn {
	
	class CMainWindow : public lsw::CMainWindow, public lsn::CDisplayHost, public lsn::CInputPoller {
		typedef lsn::CNtscSystem
												CRegionalSystem;
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
		 * The WM_INPUT handler.
		 *
		 * \param _iCode The input code. Use GET_RAWINPUT_CODE_WPARAM macro to get the value. Can be one of the following values: RIM_INPUT, RIM_INPUTSINK.
		 * \param _hRawInput A HRAWINPUT handle to the RAWINPUT structure that contains the raw input from the device. To get the raw data, use this handle in the call to GetRawInputData.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						Input( INT _iCode, HRAWINPUT _hRawInput );

		/**
		 * The WM_INPUT_DEVICE_CHANGE handler.
		 *
		 * \param _iNotifCode This parameter can be one of the following values: GIDC_ARRIVAL, GIDC_REMOVAL.
		 * \param _hDevice The HANDLE to the raw input device.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						InputDeviceChanged( INT _iNotifCode, HANDLE _hDevice );

		/**
		 * The WM_SIZE handler.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED						Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		/**
		 * The WM_SIZING handler.
		 *
		 * \param _iEdge The edge of the window that is being sized.
		 * \param _prRect A pointer to a RECT structure with the screen coordinates of the drag rectangle. To change the size or position of the drag rectangle, an application must change the members of this structure.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED						Sizing( INT _iEdge, LSW_RECT * _prRect );

		/**
		 * Advances the emulation state by the amount of time that has passed since the last advancement.
		 */
		void									Tick();

		/**
		 * Informs the host that a frame has been rendered.  This typically causes a display update and a framebuffer swap.
		 */
		virtual void							Swap();

		/**
		 * Starts running the rom on a thread.  Tick() no longer becomes useful while the emulator is running in its own thread.
		 */
		virtual void							StartThread();

		/**
		 * Stops the game thread and waits for it to join before returning.
		 */
		virtual void							StopThread();

		/**
		 * Polls the given port and returns a byte containing the result of polling by combining the LSN_INPUT_BITS values.
		 *
		 * \param _ui8Port The port being polled (0 or 1).
		 * \return Returns the result of polling the given port.
		 */
		virtual uint8_t							PollPort( uint8_t _ui8Port );

		/**
		 * Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		 *
		 * \param _pwChild Optional child control.
		 * \return Returns the virtual client rectangle of this object or of the optional child object.
		 */
		virtual const LSW_RECT					VirtualClientRect( const CWidget * _pwChild ) const;

		/**
		 * Gets the render target width, accounting for any extra debug information to be displayed on the side.
		 *
		 * \return Returns the width of the console screen plsu any side debug information.
		 */
		LONG									RenderTargetWidth() const {
			return m_pdcClient ? LONG( m_pdcClient->DisplayWidth() + (m_pdcClient->DebugSideDisplay() ? 128 : 0) ) :
				0;
		}

		/**
		 * Gets the final display width.
		 *
		 * \param _dScale A scale override or -1.0 to use m_dScale.
		 * \return Returns the final display width (native width * scale * ratio).
		 */
		LONG									FinalWidth( double _dScale = -1.0 ) const {
			if ( _dScale == -1.0 ) { _dScale = m_dScale; }
			return LONG( std::round( RenderTargetWidth() * _dScale * m_dRatio ) );
		}

		/**
		 * Gets the final display height.
		 *
		 * \param _dScale A scale override or -1.0 to use m_dScale.
		 * \return Returns the final display height (native height * scale).
		 */
		LONG									FinalHeight( double _dScale = -1.0 ) const {
			if ( _dScale == -1.0 ) { _dScale = m_dScale; }
			return m_pdcClient ? LONG( std::round( m_pdcClient->DisplayHeight() * _dScale ) ) : 0;
		}

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
		// == Enumerations.
		/** Thread state. */
		enum LSN_THREAD_STATE : int32_t {
			LSN_TS_INACTIVE						= 0,
			LSN_TS_ACTIVE						= 1,
			LSN_TS_STOP							= -1,
		};


		// == Types.
		typedef lsw::CMainWindow				Parent;

		/** The current render target/filter. */
		struct LSN_CUR_FILTER_AND_RENDER_TARGET {
			CFilterBase *						pfbCurFilter;								/**< The current filter. */
			CFilterBase *						pfbNextFilter;								/**< The next filter. */
			uint8_t *							pui8CurRenderTarget;						/**< The current render target. */
		};


		// == Members.
		/** The output scale. */
		double									m_dScale;
		/** The output ratio. */
		double									m_dRatio;
		/** Used to derive m_dRatio. */
		double									m_dRatioActual;
		/** Outside "is alive" atomic. */
		std::atomic_bool *						m_pabIsAlive;
		/** The current/next filter/render target. */
		LSN_CUR_FILTER_AND_RENDER_TARGET		m_cfartCurFilterAndTargets;
		//CFilterBase *							m_pfbCurFilter;
		/** The standard RGB filter. */
		CRgb24Filter							m_r24fRgb24Filter;
		/** A clock. */
		lsn::CClock								m_cClock;
		/** The console pointer. */
		std::unique_ptr<CSystemBase>			m_pnsSystem;
		/** Image list. */
		lsw::CImageList							m_iImages;
		/** Images. */
		lsw::CBitmap							m_bBitmaps[LSN_I_TOTAL];
		/** Image mapping. */
		INT										m_iImageMap[LSN_I_TOTAL];
		/** The BITMAPINFO header for blitting to the screen in software mode. */
		BITMAPINFO								m_biBlitInfo;
		/** The critical section for synchronizing Swap() and Paint(). */
		lsw::CCriticalSection					m_csRenderCrit;
		/** The emulator thread. */
		std::unique_ptr<std::thread>			m_ptThread;
		/** 0 = Thread Inactive. 1 = Thread Running. -1 = Thread Requested to Stop. */
		volatile std::atomic_int				m_aiThreadState;
		/** Rapid-fire buttons. */
		uint8_t									m_ui8RpidFires[8];
		


		// == Functions.
		/**
		 * Gets the window rectangle for correct output at a given scale and ratio.
		 *
		 * \param _dScale A scale override or -1.0 to use m_dScale.
		 * \return Returns the window rectangle for a given client area, derived from the desired output scale and ratio.
		 */
		LSW_RECT								FinalWindowRect( double _dScale = -1.0 ) const;

		/**
		 * Updates m_dRatio after a change to m_dRatioActual.
		 */
		void									UpdateRatio();

		/**
		 * Sends a given palette to the console.
		 *
		 * \param _vPalette The loaded palette file.  Must be (0x40 * 3) bytes.
		 */
		void									SetPalette( const std::vector<uint8_t> &_vPalette );

		/**
		 * Loads a ROM given its in-memory image and its file name.
		 *
		 * \param _vRom The in-memory ROM file.
		 * \param _s16Path The full path to the ROM.
		 * \return Returns true if loading of the ROM succeeded.
		 */
		bool									LoadRom( const std::vector<uint8_t> &_vRom, const std::u16string &_s16Path );

		/**
		 * Call when changing the m_pnsSystem pointer to hook everything (display client, input polling, etc.) back up to the new system.
		 * 
		 * \param _bMoveWindow If true, te window is resized.
		 */
		void									UpdatedConsolePointer( bool _bMoveWindow = true );

		/**
		 * Registers for raw input.
		 * 
		 * \return Returns true if registration for raw input succeeded.
		 */
		bool									RegisterRawInput();

		/**
		 * Scans for USB controllers.
		 */
		void									ScanInputDevices();

		/**
		 * Opens an HID device by its ID string.
		 *
		 * \param PARM The ID string of the device to open.
		 * \return Returns if the device was opened.  It handle will be stored in YYY.
		 */
		bool									OpenHidDevice();

		/**
		 * Gets a BITMAP stride given its row width in bytes.
		 *
		 * \param _ui32RowWidth The row width in RGB(A) pixels.
		 * \param _ui32BitDepth The total bits for a single RGB(A) pixel.
		 * \return Returns the byte width rounded up to the nearest DWORD.
		 */
		static inline DWORD						RowStride( uint32_t _ui32RowWidth, uint32_t _ui32BitDepth ) {
			return ((((_ui32RowWidth * _ui32BitDepth) + 31) & ~31) >> 3);
		}

		/**
		 * The emulator thread.
		 *
		 * \param _pmwWindow Pointer to this object.
		 */
		static void								EmuThread( lsn::CMainWindow * _pmwWindow );
		
	};

}

#endif	// #ifdef LSN_USE_WINDOWS