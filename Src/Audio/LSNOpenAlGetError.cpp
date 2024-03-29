/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wrapper for error-handling in OpenAL.
 */

#include "LSNOpenAlGetError.h"
#include "../Localization/LSNLocalization.h"

#ifdef LSN_WINDOWS

namespace lsn {

	/**
	 * Gets an OpenAL error string given the error code.
	 *
	 * \param _eError The error code to turn into a string.
	 * \return Returns a pointer to a constant string containing the string form of the error code and its descripton.
	 */
	const char * COpenAlGetError::ErrorCodeToString_AL( ALenum _eError ) {
		switch ( _eError ) {
			case AL_NO_ERROR : { return "AL_NO_ERROR: " LSN_STR( LSN_OPENAL_AL_NO_ERROR_DESC ); }
			case AL_INVALID_NAME : { return "AL_INVALID_NAME: " LSN_STR( LSN_OPENAL_AL_INVALID_NAME_DESC ); }
			case AL_INVALID_ENUM : { return "AL_INVALID_ENUM: " LSN_STR( LSN_OPENAL_AL_INVALID_ENUM_DESC ); }
			case AL_INVALID_VALUE : { return "AL_INVALID_VALUE: " LSN_STR( LSN_OPENAL_AL_INVALID_VALUE_DESC ); }
			case AL_INVALID_OPERATION : { return "AL_INVALID_OPERATION: " LSN_STR( LSN_OPENAL_AL_INVALID_OPERATION_DESC ); }
			case AL_OUT_OF_MEMORY : { return "AL_OUT_OF_MEMORY: " LSN_STR( LSN_OPENAL_AL_OUT_OF_MEMORY_DESC ); }
			default : { return LSN_STR( LSN_OPENAL_ERROR_UNKNOWN ); }
		}
	}

	/**
	 * Gets an OpenAL context error string given the error code.
	 *
	 * \param _eError The error code to turn into a string.
	 * \return Returns a pointer to a constant string containing the string form of the error code and its descripton.
	 */
	const char * COpenAlGetError::ErrorCodeToString_ALC( ALCenum _eError ) {
		switch ( _eError ) {
			case ALC_NO_ERROR : { return "ALC_NO_ERROR: " LSN_STR( LSN_OPENAL_ALC_NO_ERROR_DESC ); }
			case ALC_INVALID_DEVICE : { return "ALC_INVALID_DEVICE: " LSN_STR( LSN_OPENAL_ALC_INVALID_DEVICE_DESC ); }
			case ALC_INVALID_CONTEXT : { return "ALC_INVALID_CONTEXT: " LSN_STR( LSN_OPENAL_ALC_INVALID_CONTEXT_DESC ); }
			case ALC_INVALID_ENUM : { return "ALC_INVALID_ENUM: " LSN_STR( LSN_OPENAL_ALC_INVALID_ENUM_DESC ); }
			case ALC_INVALID_VALUE : { return "ALC_INVALID_VALUE: " LSN_STR( LSN_OPENAL_ALC_INVALID_VALUE_DESC ); }
			case ALC_OUT_OF_MEMORY : { return "ALC_OUT_OF_MEMORY: " LSN_STR( LSN_OPENAL_ALC_OUT_OF_MEMORY_DESC ); }
			default : { return LSN_STR( LSN_OPENAL_ERROR_UNKNOWN ); }
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_WINDOWS
