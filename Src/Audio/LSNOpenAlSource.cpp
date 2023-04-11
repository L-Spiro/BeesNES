/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps an ALuint source ID.
 */

#include "LSNOpenAlSource.h"
#include "LSNOpenAl.h"

namespace lsn {

	COpenAlSource::COpenAlSource() :
		m_uiId( 0 ) {
	}
	COpenAlSource::~COpenAlSource() {
		Reset();
	}


	// == Functions.
	/**
	 * Creates a source.
	 * 
	 * \return Returns true if the source was created.
	 **/
	bool COpenAlSource::CreateSource() {
		Reset();
		ALuint uiId = 0;
		if ( !COpenAl::alCall( ::alGenSources, ALsizei( 1 ), &uiId ) ) {
			return false;
		}
		m_uiId = uiId;
		return true;
	}

	/**
	 * Destroys the source.
	 * 
	 * \return Returns true if the source was destroyed.
	 **/
	bool COpenAlSource::Reset() {
		if ( m_uiId != 0 ) {
			if ( !COpenAl::alCall( ::alDeleteSources, ALsizei( 1 ), &m_uiId ) ) {
				return false;
			}

			m_uiId = 0;
		}
		return true;
	}

}	// namespace lsn
