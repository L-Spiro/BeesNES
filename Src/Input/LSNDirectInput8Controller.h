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
#include "LSNDirectInputDevice8.h"
#include "LSNUsbControllerBase.h"


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
		 * \param _diInstance The device instance containing the GUID to use to create the controller.
		 * \param _pvData Platform-specific data.
		 * \return Returns true if the controller was created.
		 */
		bool													CreateController( const DIDEVICEINSTANCEW &_diInstance, void * _pvData );

		/**
		 * Tells the controller to poll its device.  The polled data should not be inspected yet to determine which keys are held, it simply
		 *	tells the object to poll the controller for its data to then be gathered immediately after.
		 *
		 * \return Returns true if polling was successful.
		 */
		virtual bool											Poll();

		/**
		 * Polls a button by its index.  Return true if the given button is pressed.
		 * 
		 * \param _ui8Idx The controller's button index to poll.
		 * \return Returns true if the button indexed by _ui8Idx is pressed, false otherwise.
		 **/
		virtual bool											PollButton( uint8_t _ui8Idx ) const;

		/**
		 * Polls an axis by its index.  Axis 0 = X, 1 = Y, 2 = Z, and further indices are extra axes[IDX-2].
		 * 
		 * \param _ui8Idx The controller's axis index to poll.
		 * \return Returns the axis value at the given axis index.
		 **/
		virtual long											PollAxis( uint8_t _ui8Idx ) const;

		/**
		 * Gets a POV value given its POV index.
		 * 
		 * \param _ui8Idx The controller's POV index to poll.
		 * \return Returns the POV value given the POV array index.
		 **/
		virtual uint32_t										PollPov( uint8_t _ui8Idx ) const;


		/**
		 * Gets the state of the controller.
		 * 
		 * \return Gets a constant reference to the controller state.
		 **/
		inline const DIJOYSTATE &								JoyState() const { return m_jsState; }


	protected :
		// == Members.
		/** The DirectInputDevice8 object. */
		CDirectInputDevice8										m_did8Device;
		/** The device instance. */
		DIDEVICEINSTANCEW										m_diDeviceInstance;
		/** The current state of the device, updated in Poll(). */
		DIJOYSTATE												m_jsState;
		/** The device capabilities. */
		DIDEVCAPS												m_dcCaps;


		// == Functions.
		/**
		 * The thread function.
		 * 
		 * \param _ptThread A pointer to the thread data.
		 * \return Return true to keep the thread going, false to stop the thread.
		 **/
		virtual bool											ThreadFunc( LSN_THREAD * _ptThread );

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
