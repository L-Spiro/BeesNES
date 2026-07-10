#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A page for configuring the buttons on a standard controller.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Input/LSNControllerListener.h"
#include "../../Options/LSNOptions.h"

#include <Button/LSWButton.h>
#include <ComboBox/LSWComboBox.h>
#include <Helpers/LSWInputListenerBase.h>
#include <ListView/LSWListView.h>
#include <TrackBar/LSWTrackBar.h>
#include <MainWindow/LSWMainWindow.h>

#include <vector>

using namespace lsw;

namespace lsn {

	class											CMainWindow;

	/**
	 * Class CStdControllerPage
	 * \brief A template for dialog pages embedded into other dialogs/windows.
	 *
	 * Description: A template for dialog pages embedded into other dialogs/windows.
	 */
	class CStdControllerPage : public lsw::CWidget, public lsw::CInputListenerBase, public lsn::CControllerListener {
	public :
		CStdControllerPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
			m_pioOptions( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->pioOptions ),
			m_pmwMainWindow( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->pmwMainWindow ),
			m_stPlayerIdx( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->stConfigureIdx >> 16 ),
			m_stConfigIdx( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->stConfigureIdx & 0xFFFF ) {
		}
		~CStdControllerPage() {
		}


		/** The structure to pass to _ui64Data when creating this window. */
		struct LSN_CONTROLLER_SETUP_DATA {
			/** The options object. */
			LSN_INPUT_OPTIONS *						pioOptions;
			/** The main window, which allows us access to the USB controllers. */
			lsn::CMainWindow *						pmwMainWindow;
			/** The page index (IE the ieButtonMap configuration index). */
			size_t									stConfigureIdx;
		};


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED									InitDialog();

		/**
		 * Gets the text for the "Listening" state.
		 *
		 * \return Returns the text to be used during the listening state.
		 */
		virtual const wchar_t *						ListeningText() const { return LSN_LSTR( LSN_LISTENING_ ); }

		/**
		 * Gets the default text for the listenable controls.  This text is used after listening fails.
		 *
		 * \return Returns the text string to be used after failed listening.
		 */
		virtual const wchar_t *						DefaultText() const { return L""; }

		/**
		 * Detrmines if key cominations are allowed, such as Shift+Key, Alt+Key, or Control+Key.  If false, Shift, Control, and Alt
		 *	can be used as keys by themselves.
		 *
		 * \return Return false if Sift, Control, and Alt should be possible keys by themselves, or true if Shift, Control, and Alt should be
		 *	used as modifiers for other keys.
		 */
		virtual bool								AllowKeyCombos() { return false; }

		/**
		 * Called after focus on the control is lost or a key has been registered.
		 *
		 * \param _pwControl The control used for listening.
		 * \param _bSuccess Set to true if the call is due to a key being registered.
		 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
		 */
		virtual bool								StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess );

		/**
		 * Called when a controller input event is generated.
		 * 
		 * \param _pucbController A pointer to the controller that triggered the event.
		 * \param _ieEvent A constant reference to the input event data.
		 **/
		virtual void								OnInput( CUsbControllerBase * _pucbController, const CUsbControllerBase::LSN_INPUT_EVENT &_ieEvent );

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Fully updates the dialog based on current buttons and settings.
		 */
		void										UpdateDialog();

		/**
		 * The WM_DEVICECHANGE handler.
		 * 
		 * \param _wDbtEvent The event that has occurred.  One of the DBT_* values from the Dbt.h header file.
		 * \param _lParam A pointer to a structure that contains event-specific data. Its format depends on the value of the wParam parameter. For more information, refer to the documentation for each event.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED							DeviceChange( WORD _wDbtEvent, LPARAM _lParam );

		/**
		 * Gets an array of the main buttons.  There will be 8 values in the array.
		 * 
		 * \return Returns an array containing the 8 primary buttons.
		 **/
		std::vector<lsw::CButton *>					MainButtons();

		/**
		 * Gets an array of the turbo buttons.  There will be 8 values in the array.
		 * 
		 * \return Returns an array containing the 8 turbo buttons.
		 **/
		std::vector<lsw::CButton *>					TurboButtons();

		/**
		 * Gets an array of the turbo combos.  There will be 8 values in the array, which will correspond to the 8 turbo buttons returned by TurboButtons().
		 * 
		 * \return Returns an array containing the 8 turbo combos.
		 **/
		std::vector<lsw::CComboBox *>				TurboCombos();

		/**
		 * Gets an array of the dead-zone trackbars.  There will be 8 values in the array.
		 * 
		 * \return Returns an array containing the 8 dead-zone trackbars.
		 **/
		std::vector<lsw::CTrackBar *>				DeadZoneTrackBars();

		/**
		 * Saves the current input configuration.
		 */
		void										Save();


	protected :
		// == Members.
		/** The options object. */
		LSN_INPUT_OPTIONS *							m_pioOptions;
		/** The main window. */
		lsn::CMainWindow *							m_pmwMainWindow;

		/** The primary buttons. */
		LSN_INPUT_EVENT								m_ieMainButtons[8];
		/** The turbo buttons. */
		LSN_INPUT_EVENT								m_ieTurboButtons[8];
		/** The button to which we are currently listening. */
		size_t										m_stListeningIdx = 0;
		/** The listening control. */
		CWidget *									m_pwListenControl = nullptr;

		/** The array of registered controllers. */
		std::vector<CUsbControllerBase *>			m_vControllers;

		/** The play index. */
		size_t										m_stPlayerIdx;
		/** The configuration index. */
		size_t										m_stConfigIdx;

		/** The index of the name column. */
		INT											m_iNameCol = 0;
		/** The index of the type column. */
		INT											m_iTypeCol = 0;
		/** The index of the status column. */
		INT											m_iStatusCol = 0;


		// == Functions.
		/**
		 * Determines if a controller with the given CRC exists in m_vControllers.
		 * 
		 * \param _ui32Crc The CRC to check for a controller in m_vControllers having.
		 * \return Returns true if a controller in m_vControllers has a GUID with the given CRC32.
		 **/
		bool										ControllerHasCrc( uint32_t _ui32Crc ) const;

		/**
		 * Updates the main listview with controller and keyboard inputs.
		 **/
		void										UpdateInputList();

		/**
		 * Determines if any entry in m_vControllers has a GUID with a CRC32 matching the given CRC32.
		 * 
		 * \param _ui32Crc The CRC32 to find on any item in m_vControllers.
		 * \return Returns true if any item in m_vControllers has a GUID with the given CRC32.
		 **/
		bool										HaveControllerWithCrc32( uint32_t _ui32Crc ) const;

		/**
		 * Finds a CListView item with an LPARAM matching the given CRC32.
		 * 
		 * \param _plvListView The listview to scan.
		 * \param _ui32Crc The CRC32 to find among the items in the given listview.
		 * \return Returns the index of the first item with the given CRC32 or -1.
		 **/
		static int32_t								FindItemInListView( const lsw::CListView * _plvListView, uint32_t _ui32Crc );

		/**
		 * Finds a CListView item with a column-0 string matching the given name.
		 * 
		 * \param _plvListView The listview to scan.
		 * \param _wsName The name to find among the items in the given listview.
		 * \return Returns the index of the first item with the given name or -1.
		 **/
		static int32_t								FindItemInListView( const lsw::CListView * _plvListView, const std::wstring &_wsName );

		/**
		 * Takes a GUID and gets its CRC32.
		 * 
		 * \param _gId The GUID to convert.
		 * \return Returns the CRC32 of the string form of the given GUID.
		 **/
		static uint32_t								GuidToCrc32( const GUID &_gId );

	private :
		typedef class CStdControllerPageLayout		Layout;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
