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
#include <initializer_list>

#define LSN_MAT_EPSILON											1.192092896e-07

#pragma warning( push )
#pragma warning( disable : 4702 )	// warning C4702: unreachable code

namespace lsn {

	/**
	 * Class CMatrix4x4
	 * \brief A 4-element matrix.
	 *
	 * Description: A 4-element matrix.
	 */
	template <unsigned _uSimd = LSN_ST_RAW>
	class CMatrix4x4 {
	public :
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
		CMatrix4x4( const std::initializer_list<float> &_ilVec0, const std::initializer_list<float> &_ilVec1, const std::initializer_list<float> &_ilVec2 ) {
			size_t I = 0;
			for ( float fThis : _ilVec0 ) {
				if ( I >= 4 ) { break; }
				(*this)[0][I++] = fThis;
			}
			for ( I = _ilVec0.size(); I < 4; ++I ) { (*this)[0][I] = (I == 0) ? 1.0f : 0.0f; }

			I = 0;
			for ( float fThis : _ilVec1 ) {
				if ( I >= 4 ) { break; }
				(*this)[1][I++] = fThis;
			}
			for ( I = _ilVec1.size(); I < 4; ++I ) { (*this)[1][I] = (I == 1) ? 1.0f : 0.0f; }

			I = 0;
			for ( float fThis : _ilVec2 ) {
				if ( I >= 4 ) { break; }
				(*this)[2][I++] = fThis;
			}
			for ( I = _ilVec2.size(); I < 4; ++I ) { (*this)[2][I] = (I == 2) ? 1.0f : 0.0f; }

			std::memset( (*this)[3], 0, sizeof( CVector4<_uSimd> ) * 3 / 4 );
			(*this)[3][3] = 1.0f;
		}
		CMatrix4x4( const std::initializer_list<float> &_ilVec0, const std::initializer_list<float> &_ilVec1, const std::initializer_list<float> &_ilVec2, const std::initializer_list<float> &_ilVec3 ) {
			size_t I = 0;
			for ( float fThis : _ilVec0 ) {
				if ( I >= 4 ) { break; }
				(*this)[0][I++] = fThis;
			}
			for ( I = _ilVec0.size(); I < 4; ++I ) { (*this)[0][I] = (I == 0) ? 1.0f : 0.0f; }

			I = 0;
			for ( float fThis : _ilVec1 ) {
				if ( I >= 4 ) { break; }
				(*this)[1][I++] = fThis;
			}
			for ( I = _ilVec1.size(); I < 4; ++I ) { (*this)[1][I] = (I == 1) ? 1.0f : 0.0f; }

			I = 0;
			for ( float fThis : _ilVec2 ) {
				if ( I >= 4 ) { break; }
				(*this)[2][I++] = fThis;
			}
			for ( I = _ilVec2.size(); I < 4; ++I ) { (*this)[2][I] = (I == 2) ? 1.0f : 0.0f; }

			I = 0;
			for ( float fThis : _ilVec3 ) {
				if ( I >= 4 ) { break; }
				(*this)[3][I++] = fThis;
			}
			for ( I = _ilVec3.size(); I < 4; ++I ) { (*this)[3][I] = (I == 3) ? 1.0f : 0.0f; }
		}
		template <unsigned _uMatSimd>
		CMatrix4x4( const CMatrix4x4<_uMatSimd> &_mOther ) {
			std::memcpy( (*this)[0], _mOther[0], sizeof( CMatrix4x4<_uSimd> ) );
		}
		CMatrix4x4( const float * _pfValues ) {
			std::memcpy( (*this)[0], _pfValues, sizeof( CMatrix4x4<_uSimd> ) );
		}


		// == Operators.
		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the given row.
		 */
		inline const float *									operator [] ( size_t _sIndex ) const { return &m_fElements[_sIndex<<2]; }

		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the given row.
		 */
		inline float *											operator [] ( size_t _sIndex ) { return &m_fElements[_sIndex<<2]; }

		/**
		 * Multiplies this matrix with the given vector.
		 * 
		 * \param _vVec The vector to multiply against this matrix.
		 * \return Returns the multiplied vector.
		 **/
		template <unsigned _uVecSimd>
		inline CVector4<_uSimd>									operator * ( const CVector4<_uVecSimd> &_vVec ) const { return MultiplyVec4( (*this), _vVec ); }

		/**
		 * Multiplies this matrix with the given matrix.
		 * 
		 * \param _mMat The matrix to multiply against this matrix.
		 * \return Returns the result of the multiplication.
		 **/
		template <unsigned _uMatSimd>
		inline CMatrix4x4<_uSimd>								operator * ( const CMatrix4x4<_uMatSimd> &_mMat ) const { return MultiplyMatrix( (*this), _mMat ); }


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
		 * \return Returns the transformed vector.
		 */
		template <unsigned _uVecSimd>
		static inline CVector4<_uSimd>							MultiplyVec4( const CMatrix4x4<_uSimd> &_m44bMat, const CVector4<_uVecSimd> &_v4bIn ) {
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

		/**
		 * Transforms 2 vectors (x, y, z, w, x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vectors.
		 * \param _pv4bIn The vectors to transform.
		 * \param Returns the transformed vector.
		 */
		template <unsigned _uVecSimd>
		static inline void										MultiplyVec4_2( const CMatrix4x4<_uSimd> &_m44bMat, const CVector4<_uVecSimd> * _pv4bIn, CVector4<_uVecSimd> * _pv4bOut ) {
#ifdef __AVX__
			__m256 mVec = _mm256_load_ps( _pv4bIn->m_fElements );

			__m256i mMask = _mm256_set_epi32( 0, 0, 0, -1, 0, 0, 0, -1 );

			// Row 0.
			__m256 mMatRow = _mm256_broadcast_ps( reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[0]) );
			__m256 mMul = _mm256_mul_ps( mMatRow, mVec );
			__m256 mHAdd1 = _mm256_hadd_ps( mMul, mMul );		// Sum adjacent pairs.
			__m256 mHAdd2 = _mm256_hadd_ps( mHAdd1, mHAdd1 );	// Final sum within 128-bit lanes.
			_mm256_maskstore_ps( _pv4bOut->m_fElements, mMask, mHAdd2 );

			// Row 1.
			mMask = _mm256_set_epi32( 0, 0, -1, 0, 0, 0, -1, 0 );
			mMatRow = _mm256_broadcast_ps( reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[4]) );
			mMul = _mm256_mul_ps( mMatRow, mVec );
			mHAdd1 = _mm256_hadd_ps( mMul, mMul );				// Sum adjacent pairs.
			mHAdd2 = _mm256_hadd_ps( mHAdd1, mHAdd1 );			// Final sum within 128-bit lanes.
			_mm256_maskstore_ps( _pv4bOut->m_fElements, mMask, mHAdd2 );

			// Row 2.
			mMask = _mm256_set_epi32( 0, -1, 0, 0, 0, -1, 0, 0 );
			mMatRow = _mm256_broadcast_ps( reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[8]) );
			mMul = _mm256_mul_ps( mMatRow, mVec );
			mHAdd1 = _mm256_hadd_ps( mMul, mMul );				// Sum adjacent pairs.
			mHAdd2 = _mm256_hadd_ps( mHAdd1, mHAdd1 );			// Final sum within 128-bit lanes.
			_mm256_maskstore_ps( _pv4bOut->m_fElements, mMask, mHAdd2 );

			// Row 3.
			mMask = _mm256_set_epi32( -1, 0, 0, 0, -1, 0, 0, 0 );
			mMatRow = _mm256_broadcast_ps( reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[12]) );
			mMul = _mm256_mul_ps( mMatRow, mVec );
			mHAdd1 = _mm256_hadd_ps( mMul, mMul );				// Sum adjacent pairs.
			mHAdd2 = _mm256_hadd_ps( mHAdd1, mHAdd1 );			// Final sum within 128-bit lanes.
			_mm256_maskstore_ps( _pv4bOut->m_fElements, mMask, mHAdd2 );
#else
			_pv4bOut[0] = MultiplyVec4<_uVecSimd>( _m44bMat, _pv4bIn[0] );
			_pv4bOut[1] = MultiplyVec4<_uVecSimd>( _m44bMat, _pv4bIn[1] );
#endif	// #ifdef __AVX__
		}

		/**
		 * Transforms 2 vectors (x, y, z, w, x, y, z, w) by a given matrix, but only transforms the x, y, and z.  No operations are performed on w to save time.
		 *
		 * \param _m44bMat The matrix by which to transform the given vectors.
		 * \param _pv4bIn The vectors to transform.
		 * \param Returns the transformed vector.
		 */
		template <unsigned _uVecSimd>
		static inline void										MultiplyVec4_2_XYZ( const CMatrix4x4<_uSimd> &_m44bMat, const CVector4<_uVecSimd> * _pv4bIn, CVector4<_uVecSimd> * _pv4bOut ) {
#ifdef __AVX__
			__m256 mVec = _mm256_load_ps( _pv4bIn->m_fElements );

			__m256i mMask = _mm256_set_epi32( 0, 0, 0, -1, 0, 0, 0, -1 );

			// Row 0.
			__m256 mMatRow = _mm256_broadcast_ps( reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[0]) );
			__m256 mMul = _mm256_mul_ps( mMatRow, mVec );
			__m256 mHAdd1 = _mm256_hadd_ps( mMul, mMul );		// Sum adjacent pairs.
			__m256 mHAdd2 = _mm256_hadd_ps( mHAdd1, mHAdd1 );	// Final sum within 128-bit lanes.
			_mm256_maskstore_ps( _pv4bOut->m_fElements, mMask, mHAdd2 );

			// Row 1.
			mMask = _mm256_set_epi32( 0, 0, -1, 0, 0, 0, -1, 0 );
			mMatRow = _mm256_broadcast_ps( reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[4]) );
			mMul = _mm256_mul_ps( mMatRow, mVec );
			mHAdd1 = _mm256_hadd_ps( mMul, mMul );				// Sum adjacent pairs.
			mHAdd2 = _mm256_hadd_ps( mHAdd1, mHAdd1 );			// Final sum within 128-bit lanes.
			_mm256_maskstore_ps( _pv4bOut->m_fElements, mMask, mHAdd2 );

			// Row 2.
			mMask = _mm256_set_epi32( 0, -1, 0, 0, 0, -1, 0, 0 );
			mMatRow = _mm256_broadcast_ps( reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[8]) );
			mMul = _mm256_mul_ps( mMatRow, mVec );
			mHAdd1 = _mm256_hadd_ps( mMul, mMul );				// Sum adjacent pairs.
			mHAdd2 = _mm256_hadd_ps( mHAdd1, mHAdd1 );			// Final sum within 128-bit lanes.
			_mm256_maskstore_ps( _pv4bOut->m_fElements, mMask, mHAdd2 );

			/*_pv4bOut->m_fElements[3] = _pv4bIn->m_fElements[3];
			_pv4bOut->m_fElements[7] = _pv4bIn->m_fElements[7];*/
#else
			_pv4bOut[0] = MultiplyVec4<_uVecSimd>( _m44bMat, _pv4bIn[0] );
			_pv4bOut[1] = MultiplyVec4<_uVecSimd>( _m44bMat, _pv4bIn[1] );
#endif	// #ifdef __AVX__
		}

		/**
		 * Transforms 4 vectors (x, y, z, w, x, y, z, w, x, y, z, w, x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vectors.
		 * \param _pv4bIn The vectors to transform.
		 * \param _pv4bOut The transformed vectors.
		 */
		template <unsigned _uVecSimd>
		static inline void										MultiplyVec4_4( const CMatrix4x4<_uSimd> & _m44bMat, const CVector4<_uVecSimd> * _pv4bIn, CVector4<_uVecSimd> * _pv4bOut ) {
#ifdef __AVX512F__
			// Load four vectors (16 floats) into an AVX-512 register
			__m512 mVec = _mm512_load_ps( _pv4bIn->m_fElements );

			// Row 0
			__mmask16 mMask = 0x1111; // Store only 0th, 4th, 8th, and 12th elements
			__m512 mMatRow = _mm512_broadcast_f32x4( (*reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[0])) );
			__m512 mMul = _mm512_mul_ps( mMatRow, mVec );
			__m512 mSum = _mm512_add_ps(
				_mm512_add_ps( _mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 2, 3, 0, 1 ) ), mMul ),
				_mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
			_mm512_mask_store_ps( _pv4bOut->m_fElements, mMask, mSum );

			// Row 1
			mMask = 0x2222; // Store only 1st, 5th, 9th, and 13th elements
			mMatRow = _mm512_broadcast_f32x4( (*reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[4])) );
			mMul = _mm512_mul_ps( mMatRow, mVec );
			mSum = _mm512_add_ps(
				_mm512_add_ps( _mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 2, 3, 0, 1 ) ), mMul ),
				_mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
			_mm512_mask_store_ps( _pv4bOut->m_fElements, mMask, mSum );

			// Row 2
			mMask = 0x4444; // Store only 2nd, 6th, 10th, and 14th elements
			mMatRow = _mm512_broadcast_f32x4( (*reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[8])) );
			mMul = _mm512_mul_ps( mMatRow, mVec );
			mSum = _mm512_add_ps(
				_mm512_add_ps( _mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 2, 3, 0, 1 ) ), mMul ),
				_mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
			_mm512_mask_store_ps( _pv4bOut->m_fElements, mMask, mSum );

			// Row 3
			mMask = 0x8888; // Store only 3rd, 7th, 11th, and 15th elements
			mMatRow = _mm512_broadcast_f32x4( (*reinterpret_cast<const __m128 *>(&_m44bMat.m_fElements[12])) );
			mMul = _mm512_mul_ps( mMatRow, mVec );
			mSum = _mm512_add_ps(
				_mm512_add_ps( _mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 2, 3, 0, 1 ) ), mMul ),
				_mm512_shuffle_f32x4( mMul, mMul, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
			_mm512_mask_store_ps( _pv4bOut->m_fElements, mMask, mSum );
#else
			// Fallback to handling each vector individually
			_pv4bOut[0] = MultiplyVec4<_uVecSimd>( _m44bMat, &_pv4bIn[0] );
			_pv4bOut[1] = MultiplyVec4<_uVecSimd>( _m44bMat, &_pv4bIn[1] );
			_pv4bOut[2] = MultiplyVec4<_uVecSimd>( _m44bMat, &_pv4bIn[2] );
			_pv4bOut[3] = MultiplyVec4<_uVecSimd>( _m44bMat, &_pv4bIn[3] );
#endif // #ifdef __AVX512F__
		}

		/**
		 * Transforms a matrix by another matrix as A * B.
		 *
		 * \param _m44bA The left operand.
		 * \param _m44bB The right operand.
		 * \return Returns the transformed matrix.
		 */
		template <unsigned _uMatSimd>
		static CMatrix4x4<_uSimd>								MultiplyMatrix( const CMatrix4x4<_uSimd> &_m44bA, const CMatrix4x4<_uMatSimd> &_m44bB ) {
			CMatrix4x4<_uSimd> mC;
#ifdef __SSE4_1__
			if constexpr ( _uSimd >= LSN_ST_SSE4_1 ) {
				__m128 mRow0 = _mm_loadu_ps( _m44bA[0] );
				__m128 mRow1 = _mm_loadu_ps( _m44bA[1] );
				__m128 mRow2 = _mm_loadu_ps( _m44bA[2] );
				__m128 mRow3 = _mm_loadu_ps( _m44bA[3] );

				for ( int I = 0; I < 4; ++I ) {
					__m128 mCol = _mm_set_ps( _m44bB[3][I], _m44bB[2][I], _m44bB[1][I], _m44bB[0][I] );
					mC[0][I] = _mm_cvtss_f32( _mm_dp_ps( mRow0, mCol, 0xF1 ) );
					mC[1][I] = _mm_cvtss_f32( _mm_dp_ps( mRow1, mCol, 0xF1 ) );
					mC[2][I] = _mm_cvtss_f32( _mm_dp_ps( mRow2, mCol, 0xF1 ) );
					mC[3][I] = _mm_cvtss_f32( _mm_dp_ps( mRow3, mCol, 0xF1 ) );
				}

				return mC;
			}
#endif	// #ifdef __SSE4_1__
			for ( int I = 0; I < 4; ++I ) {
				for ( int J = 0; J < 4; ++J ) {
					mC[I][J] = _m44bA[I][0] * _m44bB[0][J] +
						_m44bA[I][1] * _m44bB[1][J] +
						_m44bA[I][2] * _m44bB[2][J] +
						_m44bA[I][3] * _m44bB[3][J];
				}
			}
			return mC;
		}


		// == Members.
		/** The components. */
		LSN_ALIGN( 64 )
		float													m_fElements[4*4];
	};

}	// namespace lsn

#pragma warning( pop )
