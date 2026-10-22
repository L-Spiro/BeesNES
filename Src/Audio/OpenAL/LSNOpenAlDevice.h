/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wrapper for an OpenAL device.
 */


#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNOpenAlInclude.h"

#ifdef LSN_WINDOWS

#include <string>
#include <vector>


namespace lsn {

	/**
	 * Class COpenAlDevice
	 * \brief Wrapper for an OpenAL device.
	 *
	 * Description: Wrapper for an OpenAL device.
	 */
	class COpenAlDevice {
	public :
		COpenAlDevice();
		~COpenAlDevice();


		// == Functions.
		/**
		 * Gets the raw device pointer we wrap.
		 * 
		 * \return Returns the raw device pointer we wrap.
		 **/
		ALCdevice *							Device() { return m_pdDevice; }

		/**
		 * Creates a device given a device name.
		 * 
		 * \param _pcDevice The device name or nullptr to create a device using the default sound device.
		 * \return Returns true if the device was created.
		 **/
		bool								CreateDevice( const char * _pcDevice );

		/**
		 * Destroys the device.
		 * 
		 * \return Returns true if the device was destroyed.
		 **/
		bool								Reset();

		/**
		 * Moves the device to another output device without destroying it, so that its contexts, sources, and buffers remain valid.
		 *	Requires ALC_SOFT_reopen_device.  Also reconnects a device that has been disconnected.
		 * 
		 * \param _pcDevice The device name or nullptr to use the default sound device.
		 * \return Returns true if the device was moved to the given output device.
		 **/
		bool								ReopenDevice( const char * _pcDevice );

		/**
		 * Gathers the sound devices into a vector with the default sound device in index 0.
		 * 
		 * \param _vRet Holds the returned vector of audio devices by name.
		 * \param _oadDevice The device.
		 * \return Returns true if the sound devices were successfully gathered into a vector.
		 **/
		static bool							GetAudioDevices( std::vector<std::string> &_vRet, COpenAlDevice &_oadDevice );



	protected :
		// == Types.
		/** The alcReopenDeviceSOFT() function (ALC_SOFT_reopen_device). */
		typedef ALCboolean (ALC_APIENTRY *	PfAlcReopenDeviceSoft)( ALCdevice * _pdDevice, const ALCchar * _pcDeviceName, const ALCint * _piAttribs );


		// == Members.
		/** The OpenAL device we wrap. */
		ALCdevice *							m_pdDevice;
	};

}	// namespace lsn

#endif	// #ifdef LSN_WINDOWS
