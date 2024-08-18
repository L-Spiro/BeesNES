/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Colorspace functionality.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNSimdTypes.h"
#include "LSNVector4.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#define LSN_MAT_EPSILON											1.192092896e-07

namespace lsn {

	/**
	 * Class CMatrix4x4
	 * \brief A 4-element matrix.
	 *
	 * Description: A 4-element matrix.
	 */
	template <unsigned _uSimd = LSN_ST_RAW>
	class CMatrix4x4 {
		CMatrix4x4() {
			//std::memset( m_fElements, 0, sizeof( m_fElements ) );
		}
		CMatrix4x4( const CVector4<_uSimd> &_vVec0, const CVector4<_uSimd> &_vVec1, const CVector4<_uSimd> &_vVec2 ) {
			std::memcpy( (*this)[0], _vVec0.m_fElements, sizeof( CVector4<_uSimd> ) );
			std::memcpy( (*this)[1], _vVec1.m_fElements, sizeof( CVector4<_uSimd> ) );
			std::memcpy( (*this)[2], _vVec2.m_fElements, sizeof( CVector4<_uSimd> ) );
			std::memset( (*this)[3], 0, sizeof( CVector4<_uSimd> ) * 3 / 4 );
			(*this)[3][3] = 1.0f;
		}


		// == Operators.
		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the given row.
		 */
		inline const float *									operator [] ( size_t _sIndex ) const { return &m_fElements[_sIndex]; }

		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the given row.
		 */
		inline float *											operator [] ( size_t _sIndex ) { return &m_fElements[_sIndex]; }

		/**
		 * Multiplies this matrix with the given vector.
		 * 
		 * \param _vVec The vector to multiply against this matrix.
		 * \return Returns the multiplied vector.
		 **/
		inline CVector4<_uSimd>									operator * ( const CVector4<_uSimd> &_vVec ) const { return MultiplyVec4( (*this), _vVec ); }


		// == Functions.
		/**
		 * Creates an identity matrix.
		 *
		 * \return Returns this matrix after setting to identity.
		 **/
		inline CMatrix4x4<_uSimd> &								Identity() {
			for ( size_t I = 0; I < 4; ++I ) {
				for ( size_t J = 0; J < 4; ++J ) {
					(*this)[I][J] = I == J ? 1.0f : 0.0f;
				}
			}
			return (*this);
		}

		/**
		 * Gets the inverse of this matrix.  Upon failure, the identity matrix is returned.
		 *
		 * \return Returns the inverse of this matrix, or identity on failure.
		 */
		CMatrix4x4<_uSimd>										Inverse() const {
			CMatrix4x4<_uSimd> m44bOut;
			float fA0 = (*this)[0][0] * (*this)[1][1] - (*this)[0][1] * (*this)[1][0];
			float fA1 = (*this)[0][0] * (*this)[1][2] - (*this)[0][2] * (*this)[1][0];
			float fA2 = (*this)[0][0] * (*this)[1][3] - (*this)[0][3] * (*this)[1][0];
			float fA3 = (*this)[0][1] * (*this)[1][2] - (*this)[0][2] * (*this)[1][1];
			float fA4 = (*this)[0][1] * (*this)[1][3] - (*this)[0][3] * (*this)[1][1];
			float fA5 = (*this)[0][2] * (*this)[1][3] - (*this)[0][3] * (*this)[1][2];
			float fB0 = (*this)[2][0] * (*this)[3][1] - (*this)[2][1] * (*this)[3][0];
			float fB1 = (*this)[2][0] * (*this)[3][2] - (*this)[2][2] * (*this)[3][0];
			float fB2 = (*this)[2][0] * (*this)[3][3] - (*this)[2][3] * (*this)[3][0];
			float fB3 = (*this)[2][1] * (*this)[3][2] - (*this)[2][2] * (*this)[3][1];
			float fB4 = (*this)[2][1] * (*this)[3][3] - (*this)[2][3] * (*this)[3][1];
			float fB5 = (*this)[2][2] * (*this)[3][3] - (*this)[2][3] * (*this)[3][2];
			float fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
			
			const CMatrix4x4<_uSimd> * pmSrc = this;
			
			if ( std::abs( fDet ) > LSN_MAT_EPSILON ) {
				float fTemp = (*pmSrc)[1][3];
				m44bOut[0][0] = +(*pmSrc)[1][1] * fB5 - (*pmSrc)[1][2] * fB4 + fTemp * fB3;
					m44bOut[1][0] = -(*pmSrc)[1][0] * fB5 + (*pmSrc)[1][2] * fB2 - fTemp * fB1;
					m44bOut[2][0] = +(*pmSrc)[1][0] * fB4 - (*pmSrc)[1][1] * fB2 + fTemp * fB0;
					m44bOut[3][0] = -(*pmSrc)[1][0] * fB3 + (*pmSrc)[1][1] * fB1 - (*pmSrc)[1][2] * fB0;
				fTemp = (*pmSrc)[0][3];
				m44bOut[0][1] = -(*pmSrc)[0][1] * fB5 + (*pmSrc)[0][2] * fB4 - fTemp * fB3;
					m44bOut[1][1] = +(*pmSrc)[0][0] * fB5 - (*pmSrc)[0][2] * fB2 + fTemp * fB1;
					m44bOut[2][1] = -(*pmSrc)[0][0] * fB4 + (*pmSrc)[0][1] * fB2 - fTemp * fB0;
					m44bOut[3][1] = +(*pmSrc)[0][0] * fB3 - (*pmSrc)[0][1] * fB1 + (*pmSrc)[0][2] * fB0;
				fTemp = (*pmSrc)[3][3];
				m44bOut[0][2] = +(*pmSrc)[3][1] * fA5 - (*pmSrc)[3][2] * fA4 + fTemp * fA3;
					m44bOut[1][2] = -(*pmSrc)[3][0] * fA5 + (*pmSrc)[3][2] * fA2 - fTemp * fA1;
					m44bOut[2][2] = +(*pmSrc)[3][0] * fA4 - (*pmSrc)[3][1] * fA2 + fTemp * fA0;
					m44bOut[3][2] = -(*pmSrc)[3][0] * fA3 + (*pmSrc)[3][1] * fA1 - (*pmSrc)[3][2] * fA0;
				fTemp = (*pmSrc)[2][3];
				m44bOut[0][3] = -(*pmSrc)[2][1] * fA5 + (*pmSrc)[2][2] * fA4 - fTemp * fA3;
					m44bOut[1][3] = +(*pmSrc)[2][0] * fA5 - (*pmSrc)[2][2] * fA2 + fTemp * fA1;
					m44bOut[2][3] = -(*pmSrc)[2][0] * fA4 + (*pmSrc)[2][1] * fA2 - fTemp * fA0;
					m44bOut[3][3] = +(*pmSrc)[2][0] * fA3 - (*pmSrc)[2][1] * fA1 + (*pmSrc)[2][2] * fA0;
				
				float fInvDet = 1.0f / fDet;
				if ( fInvDet != 1.0f ) {
					float * pfThis = reinterpret_cast<float *>(&m44bOut);
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis) *= fInvDet;
				}
			}
			else {
				m44bOut.Identity();
			}
			
			return m44bOut;
		}

		/**
		 * Gets the determinant.
		 *
		 * \return Returns the determinant of this matrix.
		 */
		float													Determ() const {
			float fA0 = (*this)[0][0] * (*this)[1][1] - (*this)[0][1] * (*this)[1][0];
			float fA1 = (*this)[0][0] * (*this)[1][2] - (*this)[0][2] * (*this)[1][0];
			float fA2 = (*this)[0][0] * (*this)[1][3] - (*this)[0][3] * (*this)[1][0];
			float fA3 = (*this)[0][1] * (*this)[1][2] - (*this)[0][2] * (*this)[1][1];
			float fA4 = (*this)[0][1] * (*this)[1][3] - (*this)[0][3] * (*this)[1][1];
			float fA5 = (*this)[0][2] * (*this)[1][3] - (*this)[0][3] * (*this)[1][2];
			float fB0 = (*this)[2][0] * (*this)[3][1] - (*this)[2][1] * (*this)[3][0];
			float fB1 = (*this)[2][0] * (*this)[3][2] - (*this)[2][2] * (*this)[3][0];
			float fB2 = (*this)[2][0] * (*this)[3][3] - (*this)[2][3] * (*this)[3][0];
			float fB3 = (*this)[2][1] * (*this)[3][2] - (*this)[2][2] * (*this)[3][1];
			float fB4 = (*this)[2][1] * (*this)[3][3] - (*this)[2][3] * (*this)[3][1];
			float fB5 = (*this)[2][2] * (*this)[3][3] - (*this)[2][3] * (*this)[3][2];
			return fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
		}

		/**
		 * Transpose a matrix in place.
		 *
		 * \return Returns this object.
		 */
		inline CMatrix4x4<_uSimd> &								Transpose() {
			std::swap( (*this)[0][1], (*this)[1][0] );
			std::swap( (*this)[0][2], (*this)[2][0] );
			std::swap( (*this)[0][3], (*this)[3][0] );
			std::swap( (*this)[1][2], (*this)[2][1] );
			std::swap( (*this)[1][3], (*this)[3][1] );
			std::swap( (*this)[2][3], (*this)[3][2] );
			return (*this);
		}

		/**
		 * Transforms vector (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _v4bIn The vector to transform.
		 */
		static CVector4<_uSimd>									MultiplyVec4( const CMatrix4x4<_uSimd> &_m44bMat, const CVector4<_uSimd> &_v4bIn ) {
			CVector4<_uSimd> _v4bOut;
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				// Load the vector into an SSE register.
				__m128 mVec = _mm_load_ps( _v4bIn.m_fElements );

				for ( int I = 0; I < 4; ++I ) {
					// Load the I-th row of the matrix into an SSE register.
					__m128 mMatRow = _mm_load_ps( _m44bMat[I] );

					// Perform dot product of the matrix row with the vector.
					__m128 mMul = _mm_mul_ps( mMatRow, mVec );
					__m128 mSm1 = _mm_hadd_ps( mMul, mMul );
					__m128 mSm2 = _mm_hadd_ps( mSm1, mSm1 );

					// Store the result in the output vector.
					_v4bOut.m_fElements[I] = _mm_cvtss_f32( mSm2 );
				}

				return _v4bOut;
			}
#endif	// __SSE4_1__
			
			const float * pfIn = _v4bIn.m_fElements;
			_v4bOut[0] = _m44bMat[0][0] * pfIn[0] + _m44bMat[1][0] * pfIn[1] + 
				_m44bMat[2][0] * pfIn[2] + _m44bMat[3][0] * pfIn[3];
			_v4bOut[1] = _m44bMat[0][1] * pfIn[0] + _m44bMat[1][1] * pfIn[1] +
				_m44bMat[2][1] * pfIn[2] + _m44bMat[3][1] * pfIn[3];
			_v4bOut[2] = _m44bMat[0][2] * pfIn[0] + _m44bMat[1][2] * pfIn[1] +
				_m44bMat[2][2] * pfIn[2] + _m44bMat[3][2] * pfIn[3];
			_v4bOut[3] = _m44bMat[0][3] * pfIn[0] + _m44bMat[1][3] * pfIn[1] +
				_m44bMat[2][3] * pfIn[2] + _m44bMat[3][3] * pfIn[3];
			return _v4bOut;
		}


		// == Members.
		/** The components. */
		LSN_ALIGN( 64 )
		float													m_fElements[4*4];
	};

}	// namespace lsn
