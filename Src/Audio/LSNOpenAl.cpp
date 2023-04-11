/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles global OpenAL functionality
 */


#include "LSNOpenAl.h"


namespace lsn {

	/** The primary OpenAL device. */
	COpenAlDevice COpenAl::m_oadDevice;

	/** The context. */
	COpenAlContext COpenAl::m_oalContext;

	// == Functions.
	/**
	 * Initializes OpenAL functionality.
	 * 
	 * \return Returns true if initialization was successful.
	 **/
	bool COpenAl::InitializeOpenAl() {
		std::vector<std::string> vDevices;
		// We need a dummy device to enumerate the audio devices.
		COpenAlDevice oadDummy;
		if ( !COpenAlDevice::GetAudioDevices( vDevices, oadDummy ) ) {
			return false;
		}
		if ( !vDevices.size() ) { return false; }
		if ( !m_oadDevice.CreateDevice( vDevices[0].c_str() ) ) { return false; }
		if ( !m_oalContext.CreateContext( m_oadDevice, nullptr ) ) { return false; }
		if ( !m_oalContext.MakeCurrent() ) { return false; }

		return true;
	}

	/**
	 * Shuts down OpenAL.
	 * 
	 * \return Returns true if shutdown was successful.
	 **/
	bool COpenAl::ShutdownOpenAl() {
		m_oalContext.Reset();
		return m_oadDevice.Reset();
	}

}	// namespace lsn
