#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Video Recording Settings dialog.
 */

#pragma once

#include "LSNVideoRecordingWindowLayout.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <MainWindow/LSWMainWindow.h>

#include <string>

using namespace lsw;

namespace lsn {

	/**
	 * Class CVideoRecordingWindow
	 * \brief The Video Recording Settings dialog.
	 *
	 * Description: The Video Recording Settings dialog.  Edits a copy of the video-export options (each video format keeps its own
	 *	settings) and copies them back only when closed with OK.
	 */
	class CVideoRecordingWindow : public lsw::CMainWindow {
	public :
		CVideoRecordingWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											Close();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND from a menu.
		 * \brief Invoked for menu command selections.
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									MenuCommand( WORD _wId );

		/**
		 * WM_HSCROLL.
		 *
		 * \param _uScrollPos Specifies the current position of the scroll box if the LOWORD is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise, this word is not used.
		 * \param _uScrollType Specifies a scroll bar value that indicates the user's scrolling request.
		 * \param _pwWidget The trackbar that scrolled.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED									HScroll( USHORT _uScrollPos, USHORT _uScrollType, CWidget * _pwWidget );


	protected :
		// == Members.
		/** The data passed to the dialog. */
		CVideoRecordingWindowLayout::LSN_VIDEO_RECORDING_DATA *
															m_pvrdData;
		/** The options being edited. */
		LSN_VIDEO_EXPORT_OPTIONS							m_veoWorking;
		/** The custom frame rate typed by the user, restored when Match Console is unchecked. */
		std::wstring										m_wsCustomFps;
		/** The video format whose settings are shown. */
		LSN_VIDEO_FORMAT									m_vfShown = LSN_VF_H264;
		/** Set while controls are being changed by the dialog itself (so their notifications are ignored). */
		bool												m_bUpdating = false;


		// == Functions.
		/**
		 * Verifies the inputs, copies them to the options, and closes the dialog.  If an input is invalid, the error is shown instead.
		 */
		void												SaveAndClose();

		/**
		 * Shows the settings of a video format.
		 *
		 * \param _vfFormat The video format whose settings (in m_veoWorking) are to be shown.
		 */
		void												LoadFormat( LSN_VIDEO_FORMAT _vfFormat );

		/**
		 * Reads the shown settings of a video format into m_veoWorking.
		 *
		 * \param _vfFormat The video format whose settings are shown.
		 * \param _wsError Receives an error message if an input is invalid.
		 * \return Returns the control with the invalid input or nullptr if every input is valid.
		 */
		CWidget *											ReadFormat( LSN_VIDEO_FORMAT _vfFormat, std::wstring &_wsError );

		/**
		 * Reads the settings shared by every video format (the output, the frame blending, and the conditions) into m_veoWorking.
		 *
		 * \param _wsError Receives an error message if an input is invalid.
		 * \return Returns the control with the invalid input or nullptr if every input is valid.
		 */
		CWidget *											ReadGlobal( std::wstring &_wsError );

		/**
		 * Fills the bitrate controls for an audio codec.
		 *
		 * \param _vacCodec The audio codec.
		 * \param _ui32Bitrate The bitrate to show.  It is replaced by the codec's best bitrate if the codec does not support it.
		 */
		void												LoadBitrate( LSN_VIDEO_AUDIO_CODEC _vacCodec, uint32_t _ui32Bitrate );

		/**
		 * Enables and disables controls and updates the hints to match the current selections.
		 */
		void												Update();

		/**
		 * Gets the video format selected in the Format combo box.
		 *
		 * \return Returns the selected video format.
		 */
		LSN_VIDEO_FORMAT									SelectedFormat() const;

		/**
		 * Gets the audio codec selected in the Codec combo box.
		 *
		 * \return Returns the selected audio codec or LSN_VAC_TOTAL.
		 */
		LSN_VIDEO_AUDIO_CODEC								SelectedCodec() const;

		/**
		 * Determines whether a check box is checked.
		 *
		 * \param _wId The ID of the check box.
		 * \return Returns true if the check box exists and is checked.
		 */
		bool												IsBoxChecked( WORD _wId ) const;

		/**
		 * Checks or unchecks a check box.
		 *
		 * \param _wId The ID of the check box.
		 * \param _bChecked Whether to check the box.
		 */
		void												SetBoxChecked( WORD _wId, bool _bChecked );

		/**
		 * Sets the text of a control without reacting to its change notification.
		 *
		 * \param _wId The ID of the control.
		 * \param _wsText The text to set.
		 */
		void												SetText( WORD _wId, const std::wstring &_wsText );

		/**
		 * Sets up a trackbar and its edit control for a range of values.
		 *
		 * \param _wTrackBar The ID of the trackbar.
		 * \param _wEdit The ID of the edit control.
		 * \param _i32Min The minimum value.
		 * \param _i32Max The maximum value.
		 * \param _i32Step The step between trackbar positions.
		 * \param _i32Value The value to show.
		 */
		void												SetUpTrackBar( WORD _wTrackBar, WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t _i32Step, int32_t _i32Value );

		/**
		 * Moves a trackbar to match the value in its edit control.
		 *
		 * \param _wTrackBar The ID of the trackbar.
		 * \param _wEdit The ID of the edit control.
		 * \param _i32Min The minimum value.
		 * \param _i32Max The maximum value.
		 * \param _i32Step The step between trackbar positions.
		 */
		void												TrackBarFromEdit( WORD _wTrackBar, WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t _i32Step );

		/**
		 * Sets an edit control to the value of its trackbar.
		 *
		 * \param _wTrackBar The ID of the trackbar.
		 * \param _wEdit The ID of the edit control.
		 * \param _i32Min The minimum value.
		 * \param _i32Max The maximum value.
		 * \param _i32Step The step between trackbar positions.
		 */
		void												EditFromTrackBar( WORD _wTrackBar, WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t _i32Step );

		/**
		 * Reads a whole number in a range from an edit control.
		 *
		 * \param _wEdit The ID of the edit control.
		 * \param _i32Min The minimum value.
		 * \param _i32Max The maximum value.
		 * \param _i32Value Receives the value.
		 * \return Returns true if the edit control holds a whole number from _i32Min to _i32Max.
		 */
		bool												ReadRangedValue( WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t &_i32Value ) const;

		/**
		 * Gets the text shown for a frame rate:  the number of frames per second if it is a whole number, otherwise {numerator,denominator}.
		 *
		 * \param _ui32Num The numerator.
		 * \param _ui32Den The denominator.
		 * \return Returns the text for the frame rate.
		 */
		static std::wstring									FrameRateText( uint32_t _ui32Num, uint32_t _ui32Den );

		/**
		 * Parses a frame rate:  a whole number of frames per second or a {numerator,denominator} pair of whole numbers.
		 *
		 * \param _wsText The text to parse.
		 * \param _ui32Num Receives the numerator.
		 * \param _ui32Den Receives the denominator.
		 * \return Returns true if the text is a valid frame rate.
		 */
		static bool											ParseFrameRate( const std::wstring &_wsText, uint32_t &_ui32Num, uint32_t &_ui32Den );

		/**
		 * Removes the whitespace from the start and end of a string.
		 *
		 * \param _wsText The string to trim.
		 * \return Returns the trimmed string.
		 */
		static std::wstring									Trim( const std::wstring &_wsText );

		/**
		 * Gets the hint that describes a quality setting:  its name and range (or the name of its value).
		 *
		 * \param _vqdQuality The quality setting.
		 * \param _i32Value The current value.
		 * \return Returns the hint.
		 */
		static std::wstring									QualityHint( const LSN_VIDEO_QUALITY_DATA &_vqdQuality, int32_t _i32Value );


	private :
		typedef CWinUtilities								Layout;
		typedef lsw::CMainWindow							Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
