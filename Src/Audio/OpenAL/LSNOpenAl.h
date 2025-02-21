/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles global OpenAL functionality.
 */


#pragma once

#include "../../LSNLSpiroNes.h"

#ifdef LSN_WINDOWS
#include "LSNOpenAlContext.h"
#include "LSNOpenAlDevice.h"
#include "LSNOpenAlGetError.h"
#include "LSNOpenAlInclude.h"
#include <algorithm>
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
		 * Initializes OpenAL.  Retrieves enumeration values, function pointers, etc.
		 * 
		 * \param _oadDevice The OpenAL device created by the client.
		 * \return Returns true if initialization succeeded.
		 **/
		static bool											InitializeOpenAl( ALCdevice * _oadDevice );

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
		static bool											DistanceModel( ALenum _eModel );

		/**
		 * Converts a sample from a floating-point format to a uint8_t.  8-bit PCM data is expressed as an unsigned value over the range 0 to 255, 128 being an
		 *	audio output level of zero.
		 * 
		 * \param _fSample The sample to convert.
		 * \return Returns the converted sample.
		 **/
		static inline uint8_t								SampleToUi8( float _fSample );

		/**
		 * Converts a sample from a floating-point format to an int16_t.  16-bit PCM data is expressed as a signed value over the
		 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
		 *	conversion never generates -32768.
		 * 
		 * \param _fSample The sample to convert.
		 * \return Returns the converted sample.
		 **/
		static inline int16_t								SampleToI16( float _fSample );

		/**
		 * Converts a sample from a floating-point format to an int32_t.  24-bit PCM data is expressed as a signed value over the
		 *	range -8388607 to 8388607, 0 being an audio output level of zero.  Note that both -8388608 and -8388607 are -1.0; a proper
		 *	conversion never generates -8388608.
		 *
		 * \param _fSample The sample to convert.
		 * \return Returns the converted sample.
		 **/
		static inline int32_t								SampleToI24( float _fSample );

#ifdef __AVX__
		/**
		 * Converts a sample from a floating-point format to a uint8_t.  8-bit PCM data is expressed as an unsigned value over the range 0 to 255, 128 being an
		 *	audio output level of zero.
		 * 
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pui8Dst Pointer to the output.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToUi8_AVX( const float * _pfSample, uint8_t * _pui8Dst );

		/**
		 * Converts a sample from a floating-point format to an int16_t.  16-bit PCM data is expressed as a signed value over the
		 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
		 *	conversion never generates -32768.
		 * 
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pi16Dst Pointer to the output.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToI16_AVX( const float * _pfSample, int16_t * _pi16Dst );

		/**
		 * Converts a sample from a floating-point format to an int32_t.  24-bit PCM data is expressed as a signed value over the
		 *	range -8388607 to 8388607, 0 being an audio output level of zero.  Note that both -8388608 and -8388607 are -1.0; a proper
		 *	conversion never generates -8388608.
		 *
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pi32Dst Pointer to the output.  Must be aligned to a 32-byte boundary.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToI24_AVX( const float * _pfSample, int32_t * _pi32Dst );
#endif	// #ifdef __AVX__

		/**
		 * Gets the enumerated value for the mono-24 format.
		 * 
		 * \return Returns the enumerated value for the mono-24 format or NULL.
		 **/
		//const static inline ALenum							AL_FORMAT_MONO24() { return m_eMono24; }

		/**
		 * Gets the enumerated value for the mono-32 format.
		 * 
		 * \return Returns the enumerated value for the mono-32 format or NULL.
		 **/
		//const static inline ALenum							AL_FORMAT_MONO32() { return m_eMono32; }

	protected :
		// == Members.
		/** The MONO24 format value. */
		//static ALenum										m_eMono24;
		/** The MONO32 format value. */
		//static ALenum										m_eMono32;
		
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Converts a sample from a floating-point format to a uint8_t.  8-bit PCM data is expressed as an unsigned value over the range 0 to 255, 128 being an
	 *	audio output level of zero.
	 * 
	 * \param _fSample The sample to convert.
	 * \return Returns the converted sample.
	 **/
	inline uint8_t COpenAl::SampleToUi8( float _fSample ) {
		float fClampedSample;
		if LSN_UNLIKELY( _fSample < -1.0f ) { fClampedSample = -1.0f; }
		else if LSN_UNLIKELY( _fSample > 1.0f ) { fClampedSample = 1.0f; }
		else { fClampedSample = _fSample; }
		//float fClampedSample = std::clamp( _fSample, -1.0f, 1.0f );
		float fScaledSample = (fClampedSample + 1.0f) * 0.5f * 255.0f;
		return static_cast<uint8_t>(std::round( fScaledSample ));
	}

	/**
	 * Converts a sample from a floating-point format to an int16_t.  16-bit PCM data is expressed as a signed value over the
	 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
	 *	conversion never generates -32768.
	 * 
	 * \param _fSample The sample to convert.
	 * \return Returns the converted sample.
	 **/
	inline int16_t COpenAl::SampleToI16( float _fSample ) {
		float fClampedSample;
		if LSN_UNLIKELY( _fSample < -1.0f ) { fClampedSample = -1.0f; }
		else if LSN_UNLIKELY( _fSample > 1.0f ) { fClampedSample = 1.0f; }
		else { fClampedSample = _fSample; }
		//float fClampedSample = std::clamp( _fSample, -1.0f, 1.0f );
		float fScaledSample = fClampedSample * 32767.0f;
		return static_cast<int16_t>(std::round( fScaledSample ));
	}

	/**
	 * Converts a sample from a floating-point format to an int32_t.  24-bit PCM data is expressed as a signed value over the
	 *	range -8388607 to 8388607, 0 being an audio output level of zero.  Note that both -8388608 and -8388607 are -1.0; a proper
	 *	conversion never generates -8388608.
	 *
	 * \param _fSample The sample to convert.
	 * \return Returns the converted sample.
	 **/
	inline int32_t COpenAl::SampleToI24( float _fSample ) {
		double dClampedSample;
		if LSN_UNLIKELY( _fSample < -1.0f ) { dClampedSample = -1.0; }
		else if LSN_UNLIKELY( _fSample > 1.0f ) { dClampedSample = 1.0; }
		else { dClampedSample = _fSample; }
		//float fClampedSample = std::clamp( _fSample, -1.0f, 1.0f );
		double dScaledSample = dClampedSample * 8388607.0;
		return static_cast<int32_t>(std::round( dScaledSample ));
	}

#ifdef __AVX__
	/**
	 * Converts a sample from a floating-point format to a uint8_t.  8-bit PCM data is expressed as an unsigned value over the range 0 to 255, 128 being an
	 *	audio output level of zero.
	 * 
	 * \param _pfSample Pointer to the samples to convert.
	 * \param _pui8Dst Pointer to the output.
	 * \return Returns the converted sample.
	 **/
	inline void COpenAl::SampleToUi8_AVX( const float * _pfSample, uint8_t * _pui8Dst ) {
		auto vSamples = _mm256_loadu_ps( _pfSample );
		auto vClamped = _mm256_max_ps( _mm256_set1_ps( -1.0f ), _mm256_min_ps( vSamples, _mm256_set1_ps( 1.0f ) ) );
		auto vScaled = _mm256_mul_ps( _mm256_mul_ps( _mm256_add_ps( vClamped, _mm256_set1_ps( 1.0f ) ), _mm256_set1_ps( 0.5f ) ), _mm256_set1_ps( 255.0f ) );
		auto vRounded = _mm256_round_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );
		// Paack into 8-bit PCM values.
		auto vInt32Vals = _mm256_cvtps_epi32( vRounded );

		auto vLo = _mm256_castsi256_si128( vInt32Vals );				// Lower 128 bits.
		auto vHi = _mm256_extracti128_si256( vInt32Vals, 1 );			// Upper 128 bits.
		auto vPacked16 = _mm_packus_epi32( vLo, vHi );

		// Finally, pack the 16-bit integers into 8-bit integers.
		auto vPacked8 = _mm_packus_epi16( vPacked16, vPacked16 );
		_mm_storeu_epi8( _pui8Dst, vPacked8 );
	}

	/**
	 * Converts a sample from a floating-point format to an int16_t.  16-bit PCM data is expressed as a signed value over the
	 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
	 *	conversion never generates -32768.
	 * 
	 * \param _pfSample Pointer to the samples to convert.
	 * \param _pi16Dst Pointer to the output.
	 * \return Returns the converted sample.
	 **/
	inline void COpenAl::SampleToI16_AVX( const float * _pfSample, int16_t * _pi16Dst ) {
		auto vSamples = _mm256_loadu_ps( _pfSample );
		auto vClamped = _mm256_max_ps( _mm256_set1_ps( -1.0f ), _mm256_min_ps( vSamples, _mm256_set1_ps( 1.0f ) ) );
		auto vScaled = _mm256_mul_ps( vClamped, _mm256_set1_ps( 32767.0f ) );
		auto vRounded = _mm256_round_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );

		auto vInt32Vals = _mm256_cvtps_epi32( vRounded );
    
		// Extract the lower and upper 128-bit lanes.
		auto vLo  = _mm256_castsi256_si128( vInt32Vals );				// Lower 4 integers.
		auto vHi = _mm256_extracti128_si256( vInt32Vals, 1 );			// Upper 4 integers.
		auto vPacked16 = _mm_packs_epi32( vLo, vHi );
    
		_mm_storeu_epi16( _pi16Dst, vPacked16 );
	}

	/**
	 * Converts a sample from a floating-point format to an int32_t.  24-bit PCM data is expressed as a signed value over the
	 *	range -8388607 to 8388607, 0 being an audio output level of zero.  Note that both -8388608 and -8388607 are -1.0; a proper
	 *	conversion never generates -8388608.
	 *
	 * \param _pfSample Pointer to the samples to convert.
	 * \param _pi32Dst Pointer to the output.  Must be aligned to a 32-byte boundary.
	 * \return Returns the converted sample.
	 **/
	inline void COpenAl::SampleToI24_AVX( const float * _pfSample, int32_t * _pi32Dst ) {
		auto vSamples = _mm256_loadu_ps( _pfSample );
		auto vClamped = _mm256_max_ps( _mm256_set1_ps( -1.0f ), _mm256_min_ps( vSamples, _mm256_set1_ps( 1.0f ) ) );
		auto vScaled = _mm256_mul_ps( vClamped, _mm256_set1_ps( 8388607.0f ) );
		auto vRounded = _mm256_round_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );

		auto vInt32Vals = _mm256_cvtps_epi32( vRounded );
		_mm256_store_si256( reinterpret_cast<__m256i *>(_pi32Dst), vInt32Vals );
	}
#endif	// #ifdef __AVX__

}	// namespace lsn

#endif	// #ifdef LSN_WINDOWS
