#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard USB controller using DirectInput8.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Event/LSNEvent.h"
#include "LSNDirectInputDevice8.h"
#include "LSNUsbControllerBase.h"
#include <thread>


namespace lsn {

	/**
	 * Class CDirectInput8Controller
	 * \brief A standard USB controller using DirectInput8.
	 *
	 * Description: A standard USB controller using DirectInput8.
	 */
	class CDirectInput8Controller : public CUsbControllerBase {
	public :
		CDirectInput8Controller();
		virtual ~CDirectInput8Controller();


		// == Functions.
		/**
		 * Creates the device based off a GUID.
		 *
		 * \param _guId The GUID to use to create the controller.
		 * \param _pvData Platform-specific data.
		 * \return Returns true if the controller was created.
		 */
		bool													CreateController( const GUID &_guId, void * _pvData );

		/**
		 * Tells the controller to poll its device.  The polled data should not be inspected yet to determine which keys are held, it simply
		 *	tells the object to poll the controller for its data to then be gathered immediately after.
		 *
		 * \return Returns true if polling was successful.
		 */
		virtual bool											Poll();

		/**
		 * Determines if the A button is pressed.
		 *
		 * \return Returns true if the A button is pressed.
		 */
		virtual bool											IsAPressed();

		/**
		 * Determines if the B button is pressed.
		 *
		 * \return Returns true if the B button is pressed.
		 */
		virtual bool											IsBPressed();

		/**
		 * Determines if the Select button is pressed.
		 *
		 * \return Returns true if the Select button is pressed.
		 */
		virtual bool											IsSelectPressed();

		/**
		 * Determines if the Start button is pressed.
		 *
		 * \return Returns true if the Start button is pressed.
		 */
		virtual bool											IsStartPressed();

		/**
		 * Determines if the Up button is pressed.
		 *
		 * \return Returns true if the Up button is pressed.
		 */
		virtual bool											IsUpPressed();

		/**
		 * Determines if the Down button is pressed.
		 *
		 * \return Returns true if the Down button is pressed.
		 */
		virtual bool											IsDownPressed();

		/**
		 * Determines if the Left button is pressed.
		 *
		 * \return Returns true if the Left button is pressed.
		 */
		virtual bool											IsLeftPressed();

		/**
		 * Determines if the Right button is pressed.
		 *
		 * \return Returns true if the Right button is pressed.
		 */
		virtual bool											IsRightPressed();

		/**
		 * Determines if the turbo A button is pressed.
		 *
		 * \return Returns true if the A button is pressed.
		 */
		virtual bool											IsATurboPressed();

		/**
		 * Determines if the turbo B button is pressed.
		 *
		 * \return Returns true if the B button is pressed.
		 */
		virtual bool											IsBTurboPressed();

		/**
		 * Determines if the turbo Select button is pressed.
		 *
		 * \return Returns true if the Select button is pressed.
		 */
		virtual bool											IsSelectTurboPressed();

		/**
		 * Determines if the turbo Start button is pressed.
		 *
		 * \return Returns true if the Start button is pressed.
		 */
		virtual bool											IsStartTurboPressed();

		/**
		 * Determines if the turbo Up button is pressed.
		 *
		 * \return Returns true if the Up button is pressed.
		 */
		virtual bool											IsUpTurboPressed();

		/**
		 * Determines if the turbo Down button is pressed.
		 *
		 * \return Returns true if the Down button is pressed.
		 */
		virtual bool											IsDownTurboPressed();

		/**
		 * Determines if the turbo Left button is pressed.
		 *
		 * \return Returns true if the Left button is pressed.
		 */
		virtual bool											IsLeftTurboPressed();

		/**
		 * Determines if the turbo Right button is pressed.
		 *
		 * \return Returns true if the Right button is pressed.
		 */
		virtual bool											IsRightTurboPressed();

		/**
		 * Gets the state of the controller.
		 * 
		 * \return Gets a constant reference to the controller state.
		 **/
		inline const DIJOYSTATE &								JoyState() const { return m_jsState; }


	protected :
		// == Types.
		/** The thread data. */
		struct LSN_THREAD {
			CDirectInput8Controller *							m_pci8cThis;
		};


		// == Members.
		/** The DirectInputDevice8 object. */
		CDirectInputDevice8										m_did8Device;
		/** The current state of the device, updated in Poll(). */
		DIJOYSTATE												m_jsState;
		/** The device capabilities. */
		DIDEVCAPS												m_dcCaps;
		/** The even-listening thread. */
		std::unique_ptr<std::thread>							m_ptThread;
		/** The thread event. */
		CEvent													m_eThreadClose;
		/** The event marking the closing of the thread. */
		CEvent													m_eThreadClosed;
		/** Thread data. */
		LSN_THREAD												m_tThreadData;
		/** Tells the thread to stop. */
		std::atomic<bool>										m_bStopThread;


		// == Functions.
		/**
		 * Starts the thread.
		 **/
		void													BeginThread();

		/**
		 * Stops the thread.
		 **/
		void													StopThread();

		/**
		 * The resizing thread.
		 *
		 * \param _pblppFilter Pointer to this object.
		 */
		static void												Thread( LSN_THREAD * _ptThread );

		/**
		 * Application-defined callback function that receives DirectInputDevice objects as a result of a call to the IDirectInputDevice8::EnumObjects method.
		 * 
		 * \param _lpddoiInstance DIDEVICEOBJECTINSTANCE structure that describes the object being enumerated.
		 * \param _pvRef The application-defined value passed to IDirectInputDevice8::EnumObjects as the _pvRef parameter.
		 * \return Returns DIENUM_CONTINUE to continue the enumeration or DIENUM_STOP to stop the enumeration.
		 **/
		static BOOL CALLBACK									EnumDeviceObjectsCallback( LPCDIDEVICEOBJECTINSTANCE _lpddoiInstance,  LPVOID _pvRef );

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
