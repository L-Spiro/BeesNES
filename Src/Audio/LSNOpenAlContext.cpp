/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps ALCcontext pointers.
 */


#include "LSNOpenAlContext.h"
#include "LSNOpenAl.h"

#ifdef LSN_WINDOWS

namespace lsn {

	COpenAlContext::COpenAlContext() :
		m_pcContext( nullptr ),
		m_poalDevice( nullptr ) {
	}
	COpenAlContext::~COpenAlContext() {
		Reset();
	}

	// == Functions.
	/**
	 * Creates a context given a context name.
	 * 
	 * \param _oadDevice The context name or nullptr to create a context using the default sound context.
	 * \param _piAttributeList The attribute list can be NULL, or a zero terminated list of integer pairs composed of valid ALC attribute tokens and requested values. 
	 * \return Returns true if the context was created.
	 **/
	bool COpenAlContext::CreateContext( COpenAlDevice &_oadDevice, const ALCint * _piAttributeList ) {
		Reset();

		if ( !COpenAl::alcCall( ::alcCreateContext, m_pcContext, _oadDevice.Device(), _oadDevice.Device(), const_cast<ALCint *>(_piAttributeList) ) || m_pcContext == nullptr ) {
			return false;
		}
		m_poalDevice = &_oadDevice;

		return true;
	}

	/**
	 * Destroys the context.
	 * 
	 * \return Returns true if the context was destroyed.
	 **/
	bool COpenAlContext::Reset() {
		if ( m_pcContext && m_poalDevice ) {
			ALCboolean bSuccess = ALC_FALSE;
			if ( !COpenAl::alcCall( ::alcMakeContextCurrent, bSuccess, m_poalDevice->Device(), nullptr ) || bSuccess != ALC_TRUE ) {}

			if ( !COpenAl::alcCall( ::alcDestroyContext, m_poalDevice->Device(), m_pcContext ) ) {
				return false;
			}
			m_poalDevice = nullptr;
			m_pcContext = nullptr;
			return true;
		}
		return true;
	}

	/**
	 * Makes the context current.
	 * 
	 * \return Returns true if the context was made current.
	 **/
	bool COpenAlContext::MakeCurrent() {
		if ( m_poalDevice ) {
			ALCboolean bSuccess = ALC_FALSE;
			if ( COpenAl::alcCall( ::alcMakeContextCurrent, bSuccess, m_poalDevice->Device(), m_pcContext ) && bSuccess == ALC_TRUE ) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Suspends the context.
	 * 
	 * \return Returns true if the context was suspended successfully.
	 **/
	bool COpenAlContext::Suspend() {
		if ( m_pcContext && m_poalDevice) {
			if ( COpenAl::alcCall( ::alcSuspendContext, m_poalDevice->Device(), m_pcContext ) ) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Processes the context.
	 * 
	 * \return Returns true if the context was processed successfully.
	 **/
	bool COpenAlContext::Process() {
		if ( m_pcContext && m_poalDevice ) {
			if ( COpenAl::alcCall( ::alcProcessContext, m_poalDevice->Device(), m_pcContext ) ) {
				return true;
			}
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_WINDOWS
