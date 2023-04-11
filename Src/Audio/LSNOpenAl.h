/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles global OpenAL functionality.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNOpenAlContext.h"
#include "LSNOpenAlDevice.h"
#include "LSNOpenAlGetError.h"
#include "LSNOpenAlInclude.h"
#include <string>
#include <vector>


#define alCall( FUNC, ...)									alCallImpl( __FILE__, __LINE__, FUNC, __VA_ARGS__ )
#define alcCall( FUNC, DEVICE, ... )						alcCallImpl( __FILE__, __LINE__, FUNC, DEVICE, __VA_ARGS__ )


namespace lsn {

	/**
	 * Class COpenAl
	 * \brief Handles global OpenAL functionality.
	 *
	 * Description: Handles global OpenAL functionality.
	 */
	class COpenAl {
	public :
		// == Functions.
		/**
		 * Initializes OpenAL functionality.
		 * 
		 * \return Returns true if initialization was successful.
		 **/
		static bool											InitializeOpenAl();

		/**
		 * Shuts down OpenAL.
		 * 
		 * \return Returns true if shutdown was successful.
		 **/
		static bool											ShutdownOpenAl();

		/**
		 * Gets the global OpenAL device.
		 * 
		 * \return Returns the global OpenAL device.
		 **/
		static COpenAlDevice &								Device() { return m_oadDevice; }

		/**
		 * Handler for ALenum-returning functions that can error.
		 *
		 * \param _pcFile The file calling the given function.
		 * \param _ui32Line The line number on which the given function is being called.
		 * \param _rtFunc The function being called.
		 * \param _pParms Parameters to pass to _rtFunc.
		 * \param _tFunction The function type of _rtFunc.
		 * \param _tParams Templetized parameter list.
		 * \return Returns the return value of _rtFunc( _pParms ).
		 */
		template <typename _tFunction, typename ... _tParams>
		static auto											alCallImpl( const char * _pcFile, const std::uint_fast32_t _ui32Line,
			_tFunction _rtFunc, _tParams ... _pParms )
			->typename std::enable_if_t<!std::is_same_v<void, decltype( _rtFunc( _pParms ... ) ) >, decltype( _rtFunc( _pParms ... ) )> {
			auto aRet = _rtFunc( std::forward<_tParams>( _pParms ) ... );
			COpenAlGetError::CheckError_AL( _pcFile, _ui32Line );
			return aRet;
		}

		/**
		 * Handler for void functions that can error.  Returns true if the function was called without generating an error.
		 *
		 * \param _pcFile The file calling the given function.
		 * \param _ui32Line The line number on which the given function is being called.
		 * \param _rtFunc The function being called.
		 * \param _pParms Parameters to pass to _rtFunc.
		 * \param _tFunction The function type of _rtFunc.
		 * \param _tParams Templetized parameter list.
		 * \return Returns true if the function was called without generating an error.
		 */
		template <typename _tFunction, typename ... _tParams>
		static auto											alCallImpl( const char * _pcFile, const std::uint_fast32_t _ui32Line,
			_tFunction _rtFunc, _tParams ... _pParms )
			->typename std::enable_if_t<std::is_same_v<void, decltype( _rtFunc( _pParms ... ) ) >, bool> {
			_rtFunc( std::forward<_tParams>( _pParms ) ... );
			return COpenAlGetError::CheckError_AL( _pcFile, _ui32Line );
		}

		/**
		 * Handler for ALCenum-returning functions that can error.
		 *
		 * \param _pcFile The file calling the given function.
		 * \param _ui32Line The line number on which the given function is being called.
		 * \param _rtFunc The function being called.
		 * \param _pdDevice A pointer to the device.
		 * \param _pParms Parameters to pass to _rtFunc.
		 * \param _tFunction The function type of _rtFunc.
		 * \param _tParams Templetized parameter list.
		 * \return Returns true if the function was called without generating an error.
		 */
		template<typename _tFunction, typename ... _tParams>
		static auto											alcCallImpl( const char * _pcFile, const std::uint_fast32_t _ui32Line, 
			_tFunction _rtFunc, 
			ALCdevice * _pdDevice, 
			_tParams ... _pParms )
			->typename std::enable_if_t<std::is_same_v<void, decltype( _rtFunc( _pParms ... ) ) >, bool> {
			_rtFunc( std::forward<_tParams>( _pParms ) ... );
			return COpenAlGetError::CheckError_ALC( _pcFile, _ui32Line, _pdDevice );
		}

		template<typename _tFunction, typename _tReturnType, typename ... _tParams>
		static auto											alcCallImpl( const char * _pcFile, const std::uint_fast32_t _ui32Line,
			_tFunction _rtFunc,
			_tReturnType &_rtRet,
			ALCdevice * _pdDevice, 
			_tParams ... _pParms )
		->typename std::enable_if_t<!std::is_same_v<void, decltype( _rtFunc( _pParms ... ) ) >, bool> {
			_rtRet = _rtFunc( std::forward<_tParams>( _pParms ) ... );
			return COpenAlGetError::CheckError_ALC( _pcFile, _ui32Line, _pdDevice );
		}


	protected :
		// == Members.
		/** The primary OpenAL device. */
		static COpenAlDevice								m_oadDevice;
		/** The context. */
		static COpenAlContext								m_oalContext;
	};

}	// namespace lsn
