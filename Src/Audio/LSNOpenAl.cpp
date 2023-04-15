/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles global OpenAL functionality
 */


#include "LSNOpenAl.h"


namespace lsn {

	// == Functions
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
