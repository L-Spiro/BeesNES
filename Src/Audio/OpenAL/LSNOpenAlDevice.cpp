/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wrapper for an OpenAL device.
 */


#include "LSNOpenAlDevice.h"
#include "LSNOpenAl.h"

#ifdef LSN_WINDOWS

namespace lsn {

	COpenAlDevice::COpenAlDevice() :
		m_pdDevice( nullptr ) {
	}
	COpenAlDevice::~COpenAlDevice() {
		Reset();
	}

	// == Functions.
	/**
	 * Creates a device given a device name.
	 * 
	 * \param _pcDevice The device name or nullptr to create a device using the default sound device.
	 * \return Returns true if the device was created.
	 **/
	bool COpenAlDevice::CreateDevice( const char * _pcDevice ) {
		if ( !Reset() ) { return false; }

		m_pdDevice = ::alcOpenDevice( _pcDevice );
		if ( !m_pdDevice ) {
			return false;
		}
		return true;
	}

	/**
	 * Destroys the device.
	 * 
	 * \return Returns true if the device was destroyed.
	 **/
	bool COpenAlDevice::Reset() {
		if ( m_pdDevice ) {
			ALCboolean bClosed = ::alcCloseDevice( m_pdDevice );
			/*if ( !COpenAl::alcCall( ::alcCloseDevice, bClosed, m_pdDevice, m_pdDevice ) ) {
				return false;
			}*/
			m_pdDevice = nullptr;
			return bClosed;
		}
		return true;
	}

	/**
	 * Moves the device to another output device without destroying it, so that its contexts, sources, and buffers remain valid.
	 *	Requires ALC_SOFT_reopen_device.  Also reconnects a device that has been disconnected.
	 * 
	 * \param _pcDevice The device name or nullptr to use the default sound device.
	 * \return Returns true if the device was moved to the given output device.
	 **/
	bool COpenAlDevice::ReopenDevice( const char * _pcDevice ) {
		if ( !m_pdDevice || !::alcIsExtensionPresent( m_pdDevice, "ALC_SOFT_reopen_device" ) ) { return false; }
		PfAlcReopenDeviceSoft pfReopen = reinterpret_cast<PfAlcReopenDeviceSoft>(::alcGetProcAddress( m_pdDevice, "alcReopenDeviceSOFT" ));
		if ( !pfReopen ) { return false; }
		return pfReopen( m_pdDevice, _pcDevice, nullptr ) == ALC_TRUE;
	}

	/**
	 * Gathers the sound devices into a vector with the default sound device in index 0.
	 * 
	 * \param _vRet Holds the returned vector of audio devices by name.
	 * \param _pdDevice The device.
	 * \return Returns true if the sound devices were successfully gathered into a vector.
	 **/
	bool COpenAlDevice::GetAudioDevices( std::vector<std::string> &_vRet, COpenAlDevice &_oadDevice ) {
		_vRet.clear();

		const ALCchar * pcDevices;
		if ( !COpenAl::alcCall( ::alcGetString, pcDevices, _oadDevice.Device(), nullptr, ALC_DEFAULT_DEVICE_SPECIFIER ) ) {
			return false;
		}
		try {
			_vRet.push_back( pcDevices );
		}
		catch ( ... ) { return false; }

		if ( !COpenAl::alcCall( ::alcGetString, pcDevices, _oadDevice.Device(), nullptr, ALC_DEVICE_SPECIFIER ) ) {
			return false;
		}
		try {
			const char * pcPtr = pcDevices;
			do {
				std::string sTmp = std::string( pcPtr );
				if ( sTmp != _vRet[0] ) {
					_vRet.push_back( sTmp );
				}
				pcPtr += sTmp.size() + 1;
			}
			while ( (*pcPtr) != '\0' );			// The list ends with an empty string.
		}
		catch ( ... ) { return false; }

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_WINDOWS
