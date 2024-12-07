/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 4-element vector.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNSimdTypes.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <immintrin.h>

#pragma warning( push )
#pragma warning( disable : 4702 )	// warning C4702: unreachable code

namespace lsn {

	/**
	 * Class CVector4
	 * \brief A 4-element vector.
	 *
	 * Description: A 4-element vector.
	 */
	template <unsigned _uSimd = LSN_ST_RAW>
	class CVector4 {
	public :
		CVector4() {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				//_mm_store_ps( m_fElements, _mm_setzero_ps() );
				//return;
			}
#endif	// __SSE4_1__
			//m_fElements[0] = m_fElements[1] = m_fElements[2] = m_fElements[3] = 0.0;
		}
		CVector4( float _fX, float _fY, float _fZ, float _fW ) {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				_mm_store_ps( m_fElements, _mm_set_ps( _fW, _fZ, _fY, _fX ) );
				return;
			}
#endif	// __SSE4_1__
			m_fElements[0] = _fX;
			m_fElements[1] = _fY;
			m_fElements[2] = _fZ;
			m_fElements[3] = _fW;
		}
		CVector4( const float * _pdData ) {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				_mm_store_ps( m_fElements, _mm_loadu_ps( _pdData ) );
				return;
			}
#endif	// __SSE4_1__
			m_fElements[0] = (*_pdData++);
			m_fElements[1] = (*_pdData++);
			m_fElements[2] = (*_pdData++);
			m_fElements[3] = (*_pdData++);
		}
		template <unsigned _uType>
		CVector4( const CVector4<_uType> &_vOther ) {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				_mm_store_ps( m_fElements, _mm_load_ps( _vOther.m_fElements ) );
			}
#endif	// #ifdef __SSE4_1__
			m_fElements[0] = _vOther[0];
			m_fElements[1] = _vOther[1];
			m_fElements[2] = _vOther[2];
			m_fElements[3] = _vOther[3];
		}

#ifdef __SSE4_1__
		CVector4( __m128 _vfVal ) {
			_mm_store_ps( m_fElements, _vfVal );
		}
#endif	// #ifdef __SSE4_1__


		// == Operators.
		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the dimension to get.
		 * \return Returns the value at the given dimension.
		 */
		inline float											operator [] ( size_t _sIndex ) const { return m_fElements[_sIndex]; }

		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the dimension to get.
		 * \return Returns the value at the given dimension.
		 */
		inline float &											operator [] ( size_t _sIndex ) { return m_fElements[_sIndex]; }

		/**
		 * The += operator.
		 * 
		 * \param _vOther The vector to add to this one.
		 * \return Returns a reference to this vector after the operation.
		 **/
		inline CVector4<_uSimd> &								operator += ( const CVector4<_uSimd> &_vOther ) {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_load_ps( _vOther.m_fElements );		// Load _vOther's elements into another SSE 4.1 register.
				mThisVec = _mm_add_ps( mThisVec, mOthrVec );				// Perform vectorized addition.
				_mm_store_ps( m_fElements, mThisVec );						// Store the result back into this object's elements.
				return (*this);
			}
#endif	// #ifdef __SSE4_1__
			m_fElements[0] += _vOther[0];
			m_fElements[1] += _vOther[1];
			m_fElements[2] += _vOther[2];
			m_fElements[3] += _vOther[3];
			return (*this);
		}

		/**
		 * The + operator.
		 * 
		 * \param _vOther The vector to add to this one.
		 * \return Returns a resulting vector.
		 **/
		inline CVector4<_uSimd>									operator + ( const CVector4<_uSimd> &_vOther ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_load_ps( _vOther.m_fElements );		// Load _vOther's elements into another SSE 4.1 register.
				return _mm_add_ps( mThisVec, mOthrVec );					// Perform vectorized addition.
			}
#endif	// #ifdef __SSE4_1__
			return CVector4<_uSimd>( m_fElements[0] + _vOther[0],
				m_fElements[1] + _vOther[1],
				m_fElements[2] + _vOther[2],
				m_fElements[3] + _vOther[3] );
		}

		/**
		 * The -= operator.
		 * 
		 * \param _vOther The vector to subtract from this one.
		 * \return Returns a reference to this vector after the operation.
		 **/
		inline CVector4<_uSimd> &								operator -= ( const CVector4<_uSimd> &_vOther ) {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_load_ps( _vOther.m_fElements );		// Load _vOther's elements into another SSE 4.1 register.
				mThisVec = _mm_sub_ps( mThisVec, mOthrVec );				// Perform vectorized subtraction.
				_mm_store_ps( m_fElements, mThisVec );						// Store the result back into this object's elements.
				return (*this);
			}
#endif	// #ifdef __SSE4_1__
			m_fElements[0] -= _vOther[0];
			m_fElements[1] -= _vOther[1];
			m_fElements[2] -= _vOther[2];
			m_fElements[3] -= _vOther[3];
			return (*this);
		}

		/**
		 * The - operator.
		 * 
		 * \param _vOther The vector to subtract from this one.
		 * \return Returns a resulting vector.
		 **/
		inline CVector4<_uSimd>									operator - ( const CVector4<_uSimd> &_vOther ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_load_ps( _vOther.m_fElements );		// Load _vOther's elements into another SSE 4.1 register.
				return _mm_sub_ps( mThisVec, mOthrVec );					// Perform vectorized subtraction.
			}
#endif	// #ifdef __SSE4_1__
			return CVector4<_uSimd>( m_fElements[0] - _vOther[0],
				m_fElements[1] - _vOther[1],
				m_fElements[2] - _vOther[2],
				m_fElements[3] - _vOther[3] );
		}

		/**
		 * The * operator.
		 * 
		 * \param _vOther The vector by which to multiply this vector.
		 * \return Returns a resulting vector.
		 **/
		inline CVector4<_uSimd>									operator * ( const CVector4<_uSimd> &_vOther ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_load_ps( _vOther.m_fElements );		// Load _vOther's elements into another SSE 4.1 register.
				return _mm_mul_ps( mThisVec, mOthrVec );					// Perform vectorized multiplication.
			}
#endif	// #ifdef __SSE4_1__
			return CVector4<_uSimd>( m_fElements[0] * _vOther[0],
				m_fElements[1] * _vOther[1],
				m_fElements[2] * _vOther[2],
				m_fElements[3] * _vOther[3] );
		}

		/**
		 * The * operator.
		 * 
		 * \param _fVal The value by which to multiply this vector.
		 * \return Returns a resulting vector.
		 **/
		inline CVector4<_uSimd>									operator * ( float _fVal ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_set1_ps( _fVal );						// Broadcast the scalar to all elements of the SSE 4.1 register.
				return _mm_mul_ps( mThisVec, mOthrVec );					// Perform vectorized multiplication.
			}
#endif	// #ifdef __SSE4_1__
			return CVector4<_uSimd>( m_fElements[0] * _fVal,
				m_fElements[1] * _fVal,
				m_fElements[2] * _fVal,
				m_fElements[3] * _fVal );
		}

		/**
		 * The / operator.
		 * 
		 * \param _vOther The vector by which to divide this one.
		 * \return Returns a resulting vector.
		 **/
		inline CVector4<_uSimd>									operator / ( const CVector4<_uSimd> &_vOther ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_load_ps( _vOther.m_fElements );		// Load _vOther's elements into another SSE 4.1 register.
				return _mm_div_ps( mThisVec, mOthrVec );					// Perform vectorized division.
			}
#endif	// #ifdef __SSE4_1__
			return CVector4<_uSimd>( m_fElements[0] / _vOther[0],
				m_fElements[1] / _vOther[1],
				m_fElements[2] / _vOther[2],
				m_fElements[3] / _vOther[3] );
		}

		/**
		 * The / operator.
		 * 
		 * \param _fVal The value by which to divide this one.
		 * \return Returns a reference to this vector after the operation.
		 **/
		inline CVector4<_uSimd>									operator / ( float _fVal ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mThisVec = _mm_load_ps( m_fElements );				// Load this object's elements into a 128-bit SSE 4.1 register.
				__m128 mOthrVec = _mm_set1_ps( _fVal );						// Broadcast the scalar to all elements of the SSE 4.1 register.
				return _mm_div_ps( mThisVec, mOthrVec );					// Perform vectorized division.
			}
#endif	// #ifdef __SSE4_1__
			float dDiv = 1.0f / _fVal;
			return CVector4<_uSimd>( m_fElements[0] * dDiv,
				m_fElements[1] * dDiv,
				m_fElements[2] * dDiv,
				m_fElements[3] * dDiv );
		}


		// == Functions.
		/**
		 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
		 *	unit length (1 unit in length) while maintaining its direction.
		 */
		inline CVector4<_uSimd>									Normalize() {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				// Load the elements into an SSE register.
				__m128 mVec = _mm_load_ps( m_fElements );

				// Perform the dot product of the vector with itself.
				__m128 vDot = _mm_mul_ps( mVec, mVec ); // Multiply element-wise: [x*x, y*y, z*z, w*w]

				// Horizontal add to sum all elements: x*x + y*y + z*z + w*w.
				vDot = _mm_hadd_ps( vDot, vDot );
				vDot = _mm_hadd_ps( vDot, vDot );

				// Compute the inverse square root of the sum.
				__m128 mInvLen = _mm_rsqrt_ps( vDot );

				// Multiply the original vector by the inverse length to normalize.
				__m128 vResult = _mm_mul_ps( mVec, mInvLen );
				return vResult;
			}
#endif	// __SSE4_1__
			CVector4<_uSimd> vCopy;
			float dInvLen = 1.0 / std::sqrt( m_fElements[0] * m_fElements[0] + m_fElements[1] * m_fElements[1] + m_fElements[2] * m_fElements[2] + m_fElements[3] * m_fElements[3] );
			vCopy.m_fElements[0] = m_fElements[0] * dInvLen;
			vCopy.m_fElements[1] = m_fElements[1] * dInvLen;
			vCopy.m_fElements[2] = m_fElements[2] * dInvLen;
			vCopy.m_fElements[3] = m_fElements[3] * dInvLen;
			return vCopy;
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v4bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		inline float 											Dot( const CVector4<_uSimd> &_v4bOther ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				// Load the elements of both vectors into SSE registers.
				__m128 mVec1 = _mm_load_ps( m_fElements );
				__m128 mVec2 = _mm_load_ps( _v4bOther.m_fElements );
				__m128 mMul = _mm_dp_ps( mVec1, mVec2, 0xF1 );
				return _mm_cvtss_f32( mMul );
			}
#endif	// #ifdef __SSE4_1__
			return m_fElements[0] * _v4bOther.m_fElements[0] + m_fElements[1] * _v4bOther.m_fElements[1] + m_fElements[2] * _v4bOther.m_fElements[2] + m_fElements[3] * _v4bOther.m_fElements[3];
		}

		/**
		 * Gets the maximum component.
		 *
		 * \return Returns the maximum component.
		 */
		inline float 											Max() const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				// Load the elements into an SSE register.
				__m128 mVec = _mm_loadu_ps( m_fElements );

				// Perform pairwise max operations.
				__m128 mMax1 = _mm_max_ps( mVec, _mm_shuffle_ps( mVec, mVec, _MM_SHUFFLE( 2, 3, 0, 1 ) ) );
				__m128 mMax2 = _mm_max_ps( mMax1, _mm_shuffle_ps( mMax1, mMax1, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );

				// Extract the maximum value and return it.
				return _mm_cvtss_f32( mMax2 );
			}
#endif	// #ifdef __SSE4_1__
			return std::max( std::max( std::max( m_fElements[0], m_fElements[1] ), m_fElements[2] ), m_fElements[3] );
		}

		/**
		 * Gets the minimum component.
		 *
		 * \return Returns the minimum component.
		 */
		inline float 											Min() const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				// Load the elements into an SSE register.
				__m128 mVec = _mm_loadu_ps( m_fElements );

				// Perform pairwise max operations.
				__m128 mMax1 = _mm_min_ps( mVec, _mm_shuffle_ps( mVec, mVec, _MM_SHUFFLE( 2, 3, 0, 1 ) ) );
				__m128 mMax2 = _mm_min_ps( mMax1, _mm_shuffle_ps( mMax1, mMax1, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );

				// Extract the maximum value and return it.
				return _mm_cvtss_f32( mMax2 );
			}
#endif	// #ifdef __SSE4_1__
			return std::min( std::min( std::min( m_fElements[0], m_fElements[1] ), m_fElements[2] ), m_fElements[3] );
		}

		/**
		 * Are any of the components NaN?
		 * 
		 * \return Returns true if any of the components are NaN.
		 **/
		inline bool												IsNan() const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 vElements = _mm_load_ps( m_fElements );
				// Compare each element with itself to check for NaN (NaN is not equal to itself).
				__m128 vResult = _mm_cmp_ps( vElements, vElements, _CMP_UNORD_Q );
				// Move the result mask to integer and check if any comparison returned true (NaN detected).
				return (_mm_movemask_ps( vResult ) != 0);
			}
#endif	// #ifdef __SSE4_1__
			return std::isnan( m_fElements[0] ) ||
				std::isnan( m_fElements[1] ) ||
				std::isnan( m_fElements[2] ) ||
				std::isnan( m_fElements[3] );
		}

		/**
		 * Sets the vector to 0.
		 **/
		inline CVector4<_uSimd>	&								Zero() {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				_mm_store_ps( m_fElements, _mm_setzero_ps() );
				return (*this);
			}
#endif	// #ifdef __SSE4_1__
			m_fElements[0] = m_fElements[1] = m_fElements[2] = m_fElements[3] = 0.0f;
			return (*this);
		}

		/**
		 * Clamps all components of the vector into the given range.
		 * 
		 * \param _fMin The min value.
		 * \param _fMax The max value.
		 * \return Returns a copy that has been clamped between _fMin and _fMax.
		 **/
		inline CVector4<_uSimd>									Clamp( float _fMin, float _fMax ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 vMin = _mm_set1_ps( _fMin );
				__m128 vMax = _mm_set1_ps( _fMax );
				__m128 vElems = _mm_load_ps( m_fElements );
				return _mm_max_ps( vMin, _mm_min_ps( vElems, vMax ) );
			}
#endif	// #ifdef __SSE4_1__
			return CVector4<_uSimd>( std::clamp( m_fElements[0], _fMin, _fMax ),
				std::clamp( m_fElements[1], _fMin, _fMax ),
				std::clamp( m_fElements[2], _fMin, _fMax ),
				std::clamp( m_fElements[3], _fMin, _fMax ) );
		}

		/**
		 * Applies the given function to only the X, Y, and Z components.
		 * 
		 * \param _fFunc The function to call.  Takes a single input and returns a single output.
		 * \return Returns a copy that has been clamped between _fMin and _fMax.
		 **/
		template <typename _tFunc>
		inline CVector4<_uSimd>									XyzFunc( _tFunc _fFunc ) {
			return CVector4<_uSimd>( float( _fFunc( X() ) ), float( _fFunc( Y() ) ), float( _fFunc( Z() ) ), W() );
		}

		/**
		 * Applies the given function to all components.
		 * 
		 * \param _fFunc The function to call.  Takes a single input and returns a single output.
		 * \return Returns a copy that has been clamped between _fMin and _fMax.
		 **/
		template <typename _tFunc>
		inline CVector4<_uSimd>									XyzwFunc( _tFunc _fFunc ) {
			return CVector4<_uSimd>( float( _fFunc( X() ) ), float( _fFunc( Y() ) ), float( _fFunc( Z() ) ), float( _fFunc( W() ) ) );
		}

		/**
		 * X.
		 * 
		 * \return Returns X.
		 **/
		inline float 											X() const { return m_fElements[0]; }

		/**
		 * Y.
		 * 
		 * \return Returns X.
		 **/
		inline float 											Y() const { return m_fElements[1]; }

		/**
		 * Z.
		 * 
		 * \return Returns X.
		 **/
		inline float 											Z() const { return m_fElements[2]; }

		/**
		 * W.
		 * 
		 * \return Returns X.
		 **/
		inline float 											W() const { return m_fElements[3]; }


		// == Members.
		/** The components. */
		LSN_ALIGN( 16 )
		float													m_fElements[4];
	};

}	// namespace lsn

#pragma warning( pop )
