/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles global OpenAL functionality.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNOpenAlGetError.h"
#include "LSNOpenAlInclude.h"


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
		 * Handler for ALenum-returning functions that can error.
		 *
		 * \param _pcFile The file calling the given function.
		 * \param _ui32Line The line number on which the given function is being called.
		 * \param _rtFunc The function being called.
		 * \param _pParms Parameters to pass to _rtFunc.
		 * \param _tRetType The return type of _rtFunc.
		 * \param _tParams Templetied parameter list.
		 * \return Returns the return value of _rtFunc( _pParms ).
		 */
		template <typename _tRetType, typename ... _tParams>
		auto												alCall( const char * _pcFile, const std::uint_fast32_t _ui32Line,
			_tRetType _rtFunc, _tParams ... _pParms )
			->typename std::enable_if_t<!std::is_same_v<void, decltype( _rtFunc( _pParms ... ) ) >, decltype( _rtFunc( _pParms ... ) )>
		{
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
		 * \param _tRetType The return type of _rtFunc, expected to be void.
		 * \param _tParams Templetied parameter list.
		 * \return Returns true if the function was called without generating an error.
		 */
		template <typename _tRetType, typename ... _tParams>
		auto												alCall( const char * _pcFile, const std::uint_fast32_t _ui32Line,
			_tRetType _rtFunc, _tParams ... _pParms )
			->typename std::enable_if_t<!std::is_same_v<void, decltype( _rtFunc( _pParms ... ) ) >, bool>
		{
			_rtFunc( std::forward<_tParams>( _pParms ) ... );
			return COpenAlGetError::CheckError_AL( _pcFile, _ui32Line );
		}
	};

}	// namespace lsn
