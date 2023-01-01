/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wrapper for error-handling in OpenAL.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNOpenAlInclude.h"
#include <iomanip>
#include <iostream>


namespace lsn {

	/**
	 * Class CCpu6502
	 * \brief Wrapper for error-handling in OpenAL.
	 *
	 * Description: Wrapper for error-handling in OpenAL.
	 */
	class COpenAlGetError {
	public :
		// == Functions.
		/**
		 * Gets an OpenAL error string given the error code.
		 *
		 * \param _eError The error code to turn into a string.
		 * \return Returns a pointer to a constant string containing the string form of the error code and its descripton.
		 */
		const char *									ErrorCodeToString_AL( ALenum _eError );

		/**
		 * Gets an OpenAL context error string given the error code.
		 *
		 * \param _eError The error code to turn into a string.
		 * \return Returns a pointer to a constant string containing the string form of the error code and its descripton.
		 */
		const char *									ErrorCodeToString_ALC( ALCenum _eError );

		/**
		 * Calls ::alGetError().  If there is an error, it is printed and false is returned, otherwise true is returned.
		 *
		 * \param _pcFile The file name to use during error-reporting.
		 * \param _ui32Line The line number for error-reporting.
		 * \return Return true if ::alGetError() returns AL_NO_ERROR.
		 */
		inline bool										CheckError_AL( const char * _pcFile, uint32_t _ui32Line );

		/**
		 * Calls ::alcGetError().  If there is an error, it is printed and false is returned, otherwise true is returned.
		 *
		 * \param _pcFile The file name to use during error-reporting.
		 * \param _ui32Line The line number for error-reporting.
		 * \param _pdDevice The device whose error status is to be checked.
		 * \return Return true if ::alcGetError() returns ALC_NO_ERROR.
		 */
		inline bool										CheckError_ALC( const char * _pcFile, uint32_t _ui32Line, ALCdevice * _pdDevice );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Fuctions.
	/**
	 * Calls ::alGetError().  If there is an error, it is printed and false is returned, otherwise true is returned.
	 *
	 * \param _pcFile The file name to use during error-reporting.
	 * \param _ui32Line The line number for error-reporting.
	 * \return Return true if ::alGetError() returns AL_NO_ERROR.
	 */
	inline bool COpenAlGetError::CheckError_AL( const char * _pcFile, uint32_t _ui32Line ) {
		ALenum aError = ::alGetError();
		if ( aError != AL_NO_ERROR ) {
			std::cerr << _pcFile << "::" << _ui32Line << ": " << ErrorCodeToString_AL( aError ) << " (" << std::setiosflags( std::ios_base::hex | std::ios_base::uppercase ) << aError << ")" << std::endl;
			return false;
		}
		return true;
	}

	/**
	 * Calls ::alcGetError().  If there is an error, it is printed and false is returned, otherwise true is returned.
	 *
	 * \param _pcFile The file name to use during error-reporting.
	 * \param _ui32Line The line number for error-reporting.
	 * \param _pdDevice The device whose error status is to be checked.
	 * \return Return true if ::alcGetError() returns ALC_NO_ERROR.
	 */
	inline bool COpenAlGetError::CheckError_ALC( const char * _pcFile, uint32_t _ui32Line, ALCdevice * _pdDevice ) {
		ALCenum aError = ::alcGetError( _pdDevice );
		if ( aError != ALC_NO_ERROR ) {
			std::cerr << _pcFile << "::" << _ui32Line << ": " << ErrorCodeToString_ALC( aError ) << " (" << std::setiosflags( std::ios_base::hex | std::ios_base::uppercase ) << aError << ")" << std::endl;
			return false;
		}
		return true;
	}

}	// namespace lsn
