#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The main window of the emulator.
 */

#pragma once

#include "../../BeesNES/LSNBeesNes.h"
#include "../../Display/LSNDisplayHost.h"
#include "../../Input/LSNDirectInput8Controller.h"
#include "../../Input/LSNInputPoller.h"
#include "../../Options/LSNWindowOptions.h"

#ifdef LSN_DX9
#include "../../GPU/DirectX9/LSNDirectX9.h"
#include "../../GPU/DirectX9/LSNDirectX9Device.h"
#include "../../GPU/DirectX9/LSNDirectX9NesPresenter.h"
#endif	// #ifdef LSN_DX9

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
	
	/**
	 * Class CMainWindow
	 * \brief The main window of the emulator.
	 *
	 * Description: The main window of the emulator.
	 */
	class CMainWindow : public lsw::CMainWindow, public lsn::CDisplayHost, public lsn::CInputPoller {
		
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
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						InitDialog();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND from a menu.
		 * \brief Invoked for menu command selections.
		 *
		 * \param _Id The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						MenuCommand( WORD _wId ) { return Command( 0, _wId, nullptr ); }

		/**
		 * Handles WM_USER/custom messages.
		 * 
		 * \param _uMsg The message to handle.
		 * \param _wParam Additional message-specific information.
		 * \param _lParam Additional message-specific information.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED						CustomPrivateMsg( UINT _uMsg, WPARAM /*_wParam*/, LPARAM /*_lParam*/ );

		/**
		 * Handles WM_NCDESTROY.
		 * \brief Final cleanup after the non-client area is destroyed.
		 *
		 * This is the last message a window receives. Use to clear pointers stored in GWLP_USERDATA
		 * and to finalize per-window allocations.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED						NcDestroy();

		/**
		 * Handles WM_GETMINMAXINFO.
		 * \brief Provides minimum/maximum tracking sizes.
		 *
		 * Override to fill *(_pmmiInfo) with size constraints.
		 *
		 * \param _pmmiInfo Pointer to a MINMAXINFO structure to populate.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						GetMinMaxInfo( MINMAXINFO * _pmmiInfo );

		/**
		 * Handles WM_PAINT.
		 * \brief Performs painting for the client area.
		 *
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						Paint();

		/**
		 * Handles WM_MOVE.
		 * \brief Notified when the window is moved.
		 *
		 * \param _lX New x-position of the window (screen coordinates).
		 * \param _lY New y-position of the window (screen coordinates).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						Move( LONG _lX, LONG _lY );

		/**
		 * The WM_KEYDOWN handler.
		 *
		 * \param _uiKeyCode The virtual-key code of the nonsystem key.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						KeyDown( UINT _uiKeyCode, UINT _uiFlags );

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
		 * The WM_EXITSIZEMOVE handler.
		 * 
		 * \return Returns a LSW_HANDLED enumeration.
		 **/
		virtual LSW_HANDLED						ExitSizeMove();

		/**
		 * The WM_SIZING handler.
		 *
		 * \param _iEdge The edge of the window that is being sized.
		 * \param _prRect A pointer to a RECT structure with the screen coordinates of the drag rectangle. To change the size or position of the drag rectangle, an application must change the members of this structure.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED						Sizing( INT _iEdge, LSW_RECT * _prRect );

		/**
		 * The WM_INITMENUPOPUP handler.
		 *
		 * \param _hMenu A handle to the drop-down menu or submenu.
		 * \param _wPos The zero-based relative position of the menu item that opens the drop-down menu or submenu.
		 * \param _bIsWindowMenu Indicates whether the drop-down menu is the window menu. If the menu is the window menu, this parameter is TRUE; otherwise, it is FALSE.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						InitMenuPopup( HMENU /*_hMenu*/, WORD /*_wPos*/, BOOL /*_bIsWindowMenu*/ );

		/**
		 * The WM_SYSCOMMAND handler.
		 * 
		 * \param _wCommand The type of system command requested.  One of the SC_* values.
		 * \param _iMouseX Specifies the horizontal position of the cursor, in screen coordinates, if a window menu command is chosen with the mouse. Otherwise, this parameter is not used.
		 * \param _iMouseY Specifies the vertical position of the cursor, in screen coordinates, if a window menu command is chosen with the mouse. This parameter is 1 if the command is chosen using a system accelerator, or zero if using a mnemonic.
		 * \return Returns an LSW_HANDLED code.  Return LSW_H_HANDLED to prevent the command from being sent to the default procedure.
		 **/
		virtual LSW_HANDLED						SysCommand( WORD _wCommand, int /*_iMouseX*/, int /*_iMouseY*/ );

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
		 * Locks the controller array for external access.  Always pair with a call to UnlockControllers().
		 **/
		inline void								LockControllers() const {
			m_csControllerCrit.EnterCriticalSection();
		}

		/**
		 * Unlocks the controller array after external access is concluded.  Always pair with a call to LockControllers().
		 **/
		inline void								UnlockControllers() const {
			m_csControllerCrit.LeaveCriticalSection();
		}

		/**
		 * Gets the current controller array.  Must have previously called LockControllers().  Call LockControllers() when done with the controller array.
		 * 
		 * \return Returns a constant reference to the controller array.
		 **/
		inline const std::vector<CUsbControllerBase *> &
												Controllers() const {
			return m_pdi8cControllers;
		}

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
			return m_bnEmulator.RenderTargetWidth();
		}

		/**
		 * Gets the final display width.
		 *
		 * \param _dScale A scale override or -1.0 to use m_dScale.
		 * \return Returns the final display width (native width * scale * ratio).
		 */
		LONG									FinalWidth( double _dScale = -1.0 ) const {
			if ( _dScale == -1.0 ) { _dScale = m_bnEmulator.GetScale(); }
			return LONG( std::round( RenderTargetWidth() * _dScale * m_bnEmulator.GetRatio() ) );
		}

		/**
		 * Gets the final display height.
		 *
		 * \param _dScale A scale override or -1.0 to use m_dScale.
		 * \return Returns the final display height (native height * scale).
		 */
		LONG									FinalHeight( double _dScale = -1.0 ) const {
			if ( _dScale == -1.0 ) { _dScale = m_bnEmulator.GetScale(); }
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

#ifdef LSN_DX9
		/**
		 * \brief Initializes the DirectX 9 device for this window.
		 * 
		 * Dynamically loads d3d9.dll via the DX9 wrapper and creates a device bound to this HWND.
		 * Call once (e.g., after the window is created) and, if successful, the Paint() path will
		 * render using DirectX 9 instead of the software blitter.
		 *
		 * \return Returns true if the DX9 device was created and is ready.
		 **/
		bool									CreateDx9();

		/**
		 * Destroys the DirectX 9 device and all filters.
		 **/
		void									DestroyDx9();

		/**
		 * \brief Resizes the DX9 backbuffer and reinitializes size-dependent presenter resources.
		 *
		 * Updates cached D3DPRESENT_PARAMETERS with the new client size and resets the device.
		 * Then re-initializes the presenter so its DEFAULT-pool resources (index/LUT targets, VBs)
		 * are recreated against the new device state.
		 *
		 * \param _ui32ClientW New client width in pixels.
		 * \param _ui32ClientH New client height in pixels.
		 * \return True on success; false if the DX9 path is disabled or reset failed.
		 */
		bool									OnSizeDx9( uint32_t _ui32ClientW, uint32_t _ui32ClientH );

#endif	// #ifdef LSN_DX9


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


		// == Members.
		/** The emulator object. */
		CBeesNes								m_bnEmulator;
		/** DirectInput 8 controller inputs. */
		std::vector<CUsbControllerBase *>		m_pdi8cControllers;
		/** Outside "is alive" atomic. */
		std::atomic_bool *						m_pabIsAlive;
		/** A clock. */
		lsn::CClock								m_cClock;
		/** Maximized resolution. */
		LSW_RECT								m_rMaxRect;
		/** Image list. */
		lsw::CImageList							m_iImages;
		/** Images. */
		lsw::CBitmap							m_bBitmaps[LSN_I_TOTAL];
		/** Image mapping. */
		INT										m_iImageMap[LSN_I_TOTAL];
		/** The BITMAPINFO header for blitting to the screen in software mode. */
		BITMAPINFO								m_biBlitInfo;
		/** The BITMAPINFO header for blitting the black bars around the screen when maximized. */
		BITMAPINFO								m_biBarInfo;
		/** Full-screen information. */
		LSW_WINDOW_PLACEMENT					m_wpPlacement;
		/** The bar pixels. */
		std::vector<uint8_t>					m_vBars;
		/** The critical section for synchronizing Swap() and Paint(). */
		lsw::CCriticalSection					m_csRenderCrit;
		/** The critical section for reading controllers. */
		lsw::CCriticalSection					m_csControllerCrit;
		/** The emulator thread. */
		std::unique_ptr<std::thread>			m_ptThread;
		/** 0 = Thread Inactive. 1 = Thread Running. -1 = Thread Requested to Stop. */
		volatile std::atomic_int				m_aiThreadState;
		/** Is the window maximized? */
		bool									m_bMaximized = false;
		/** The Patch window. */
		lsw::CWidget *							m_pwPatchWindow = nullptr;
		/** The WAV Editor window. */
		lsw::CWidget *							m_pwWavEditorWindow = nullptr;
		/** The window/UI settings/options. */
		LSN_WINDOW_OPTIONS						m_woWindowOptions;

		/** Cached pointer to the status bar along the bottom. */
		lsw::CStatusBar *						m_psbCachedBar;
		/** Cached statuc-bar rectangle. */
		LSW_RECT								m_rStatusBarRect;
		/** Palettes. */
		LSN_PALETTE_OPTIONS						m_poPalettes[LSN_PM_CONSOLE_TOTAL];


#ifdef LSN_DX9
		/** DirectX 9 device (created on demand). */
		CDirectX9Device							m_dx9Device;
		/** The palette renderer. */
		std::unique_ptr<CDirectX9NesPresenter>	m_upDx9PaletteRender;
		/** If true, Paint() renders via DX9 instead of software. */
		bool									m_bUseDx9 = false;
		/** Small counter for flashing clear color. */
		uint8_t									m_ui8Flash = 0;
#endif	// #ifdef LSN_DX9
		


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
		 * Loads a ROM given its in-memory image and its file name.
		 *
		 * \param _vRom The in-memory ROM file.
		 * \param _s16Path The full path to the ROM.
		 * \return Returns true if loading of the ROM succeeded.
		 */
		bool									LoadRom( const std::vector<uint8_t> &_vRom, const std::u16string &_s16Path );

		/**
		 * Loads a ROM file given a path to a ZIP and a file name inside the ZIP.
		 * 
		 * \param _s16ZipPath The path to the ZIP file.
		 * \param _s16File The file name inside the ZIP path to load.
		 * \return Returns true if the file was loaded.
		 **/
		bool									LoadZipRom( const std::u16string &_s16ZipPath, const std::u16string &_s16File );

		/**
		 * Call when changing the m_psbSystem pointer to hook everything (display client, input polling, etc.) back up to the new system.
		 * 
		 * \param _bMoveWindow If true, te window is resized.
		 */
		void									UpdatedConsolePointer( bool _bMoveWindow = true );

		/**
		 * Updates the "Open Recent" menu.
		 **/
		void									UpdateOpenRecent();

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
		 * Destroys all controller inputs.
		 **/
		void									DestroyControllers();

		/**
		 * Informs that a child was removed from a child control (IE this control's child had a child control removed from it).
		 *	Is also called on the control from which a child was removed for convenience.
		 * 
		 * \param _pwChild The child being removed.
		 **/
		virtual void							ChildWasRemoved( const CWidget * _pwChild );

		/**
		 * The WM_DEVICECHANGE handler.
		 * 
		 * \param _wDbtEvent The event that has occurred.  One of the DBT_* values from the Dbt.h header file.
		 * \param _lParam A pointer to a structure that contains event-specific data. Its format depends on the value of the wParam parameter. For more information, refer to the documentation for each event.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED						DeviceChange( WORD _wDbtEvent, LPARAM _lParam );

		/**
		 * Opens an HID device by its ID string.
		 *
		 * \param PARM The ID string of the device to open.
		 * \return Returns if the device was opened.  It handle will be stored in YYY.
		 */
		//bool									OpenHidDevice();

		/**
		 * The emulator thread.
		 *
		 * \param _pmwWindow Pointer to this object.
		 */
		static void								EmuThread( lsn::CMainWindow * _pmwWindow );
		
	};

}

#endif	// #ifdef LSN_USE_WINDOWS