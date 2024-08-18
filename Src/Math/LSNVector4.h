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
				_mm_store_ps( m_fElements, _mm_load_ps( _pdData ) );
				return;
			}
#endif	// __SSE4_1__
			m_fElements[0] = (*_pdData++);
			m_fElements[1] = (*_pdData++);
			m_fElements[2] = (*_pdData++);
			m_fElements[3] = (*_pdData++);
		}


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


		// == Functions.
		/**
		 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
		 *	unit length (1 unit in length) while maintaining its direction.
		 * Accuracy/speed depends on the LSM_PERFORMANCE macro.
		 */
		inline CVector4											Normalize() {
			CVector4 vCopy;
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

				// Store the vResult back into vCopy.
				_mm_store_ps( vCopy, vResult );
				return vCopy.
			}
#endif	// __SSE4_1__
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
		inline float 											Dot( const CVector4 &_v4bOther ) const {
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				// Load the elements of both vectors into SSE registers.
				__m128 mVec1 = _mm_load_ps( m_fElements );
				__m128 mVec2 = _mm_load_ps( _v4bOther );

				// Multiply the vectors element-wise.
				__m128 mMul = _mm_mul_ps( mVec1, mVec2 );

				// Horizontally add the results to get the dot product.
				mMul = _mm_hadd_ps( mMul, mMul );
				mMul = _mm_hadd_ps( mMul, mMul );

				// Extract the result and return it.
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


		// == Members.
		/** The components. */
		LSN_ALIGN( 64 )
		float													m_fElements[4];
	};

}	// namespace lsn
