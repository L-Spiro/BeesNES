/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles global OpenAL functionality
 */


#include "LSNOpenAl.h"

#ifdef LSN_WINDOWS

namespace lsn {

	// == Members.
	/** The MONO24 format value. */
	//ALenum COpenAl::m_eMono24 = NULL;

	/** The MONO32 format value. */
	//ALenum COpenAl::m_eMono32 = NULL;

	// == Functions.
	/**
	 * Initializes OpenAL.  Retrieves enumeration values, function pointers, etc.
	 * 
	 * \param _oadDevice The OpenAL device created by the client.
	 * \return Returns true if initialization succeeded.
	 **/
	bool COpenAl::InitializeOpenAl( ALCdevice * /*_oadDevice*/ ) {
		//bool bVal = ::alIsExtensionPresent( "AL_EXT_float32" );
		/*m_eMono24 = ::alGetEnumValue( "AL_FORMAT_MONO24" );
		m_eMono32 = ::alGetEnumValue( "AL_FORMAT_MONO32" );*/
		return true;
	}

	/**
	 * Sets the distance model.  Values are:
	 *	AL_INVERSE_DISTANCE
	 *	AL_INVERSE_DISTANCE_CLAMPED
	 *	AL_LINEAR_DISTANCE
 	 *	AL_LINEAR_DISTANCE_CLAMPED
 	 *	AL_EXPONENT_DISTANCE
 	 *	AL_EXPONENT_DISTANCE_CLAMPED
 	 *	AL_NONE
	 * 
	 * \param _eModel The model being set.
	 * \return Returns true if the distance model was set successfully.
	 **/
	bool COpenAl::DistanceModel( ALenum _eModel ) {
		if ( COpenAl::alCall( ::alDistanceModel, _eModel ) ) {
			return true;
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_WINDOWS
