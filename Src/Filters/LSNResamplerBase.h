/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base resampler for GPU implementation.
 */

#pragma once

#include "../Utilities/LSNAlignmentAllocator.h"
#include "../Utilities/LSNTextureAddressing.h"
#include "../Utilities/LSNUtilities.h"
#include "../Utilities/LSNVector4.h"

#include <cmath>
#include <numbers>
#include <vector>

namespace lsn {

	/**
	 * Class CResamplerBase
	 * \brief A base resampler for GPU implementation.
	 *
	 * Description: Generates weights and kernel structures to be passed to GPU shaders.
	 */
	class CResamplerBase {
	public :
		CResamplerBase();
		virtual ~CResamplerBase();


		// == Enumerations.
		/** Filter functions. */
		enum LSN_FILTER_FUNCS : size_t {
			LSN_FF_POINT,
			LSN_FF_LINEAR,
			LSN_FF_QUADRATICSHARP,
			LSN_FF_QUADRATIC,
			LSN_FF_QUADRATICAPPROX,
			LSN_FF_QUADRATICMIX,
			LSN_FF_KAISER,
			LSN_FF_LANCZOS2,
			LSN_FF_LANCZOS3,
			LSN_FF_LANCZOS4,
			LSN_FF_LANCZOS6,
			LSN_FF_LANCZOS8,
			LSN_FF_LANCZOS12,
			LSN_FF_LANCZOS64,
			LSN_FF_MITCHELL,
			LSN_FF_ROBIDOUX,
			LSN_FF_ROBIDOUXSHARP,
			LSN_FF_ROBIDOUXSOFT,
			LSN_FF_CATMULLROM,
			LSN_FF_BSPLINE,
			LSN_FF_ADOBEBICUBIC,
			LSN_FF_CARDINALSPLINEUNIFORM,
			LSN_FF_HERMITE,
			LSN_FF_HAMMING,
			LSN_FF_HANNING,
			LSN_FF_BLACKMAN,
			LSN_FF_GAUSSIANSHARP,
			LSN_FF_GAUSSIAN,
			LSN_FF_BELL,
		};


		// == Types.
		/** Contributions (weights and texel indices). */
		struct LSN_CONTRIBUTIONS {
			std::vector<float, CAlignmentAllocator<float, 64>>	fContributions;
			std::vector<int32_t>								i32Indices;
		};

		typedef float (*										PfFilterFunc)( float );

		/** Filter information. */
		typedef struct LSN_FILTER {
			/** The filter function to use. */
			PfFilterFunc										pfFunc = BilinearFilterFunc;

			/** Filter helper value. */
			float												fSupport = 1.0f;
		} * LPLSN_FILTER, * const LPCLSN_FILTER;

		/** Parameters for resampling. */
		struct LSN_RESAMPLE {
			float												fBorderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			CVector4<LSN_ST_RAW>								vBlendColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			float												fFilterScale = 1.0f;
			uint32_t											ui32W = 0;
			uint32_t											ui32H = 0;
			uint32_t											ui32D = 0;
			uint32_t											ui32NewW = 0;
			uint32_t											ui32NewH = 0;
			uint32_t											ui32NewD = 0;
			LSN_TEXTURE_ADDRESSING								taColorW = LSN_TA_NULL_BORDER;
			LSN_TEXTURE_ADDRESSING								taColorH = LSN_TA_NULL_BORDER;
			LSN_TEXTURE_ADDRESSING								taColorD = LSN_TA_NULL_BORDER;
			LSN_TEXTURE_ADDRESSING								taAlphaW = LSN_TA_NULL_BORDER;
			LSN_TEXTURE_ADDRESSING								taAlphaH = LSN_TA_NULL_BORDER;
			LSN_TEXTURE_ADDRESSING								taAlphaD = LSN_TA_NULL_BORDER;
			LSN_FILTER											fFilterW;
			LSN_FILTER											fFilterH;
			LSN_FILTER											fFilterD;
			LSN_FILTER											fAlphaFilterW;
			LSN_FILTER											fAlphaFilterH;
			LSN_FILTER											fAlphaFilterD;
			bool												bAlpha = true;
		};

		// == Functions.
		/**
		 * Creates a new contribution list.
		 *
		 * \param _ui32SrcSize Size of the source.
		 * \param _ui32DstSize Size of the destination.
		 * \param _taAddressMode Texture addressing mode.
		 * \param _pfFilter Filter function.
		 * \param _fFilterSupport Filter support value.
		 * \param _fFilterScale Filter scale.
		 * \return Returns true if all allocations succeeded.
		 */
		bool													CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
			LSN_TEXTURE_ADDRESSING _taAddressMode,
			PfFilterFunc _pfFilter, float _fFilterSupport, float _fFilterScale );

		/**
		 * Standard sinc() function.
		 * 
		 * \param _fX The operand.
		 * \return Returns sin(x) / x.
		 **/
		static inline float										Sinc( float _fX ) {
			_fX *= std::numbers::pi_v<float>;
			if ( _fX < 0.01f && _fX > -0.01f ) {
				return 1.0f + _fX * _fX * (-1.0f / 6.0f + _fX * _fX * 1.0f / 120.0f);
			}

			return std::sin( _fX ) / _fX;
		}

		/**
		 * A helper function.
		 *
		 * \param _fX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline float										Bessel0( float _fX ) {
			constexpr float fEspiRatio = 1.0e-16f;
			float fSum = 1.0f, fPow = 1.0f, fDs = 1.0f, fXh = _fX * 0.5f;
			uint32_t ui32K = 0;

			while ( fDs > fSum * fEspiRatio ) {
				++ui32K;
				fPow *= (fXh / float( ui32K ));
				fDs = fPow * fPow;
				fSum += fDs;
			}

			return fSum;
		}

		/**
		 * A helper function for the Kaiser filter.
		 *
		 * \param _fAlpha A happy parameter.
		 * \param _fHalfWidth A happy parameter.
		 * \param _fX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline float										KaiserHelper( float _fAlpha, float _fHalfWidth, float _fX ) {
			const float fRatio = _fX / _fHalfWidth;
			return Bessel0( _fAlpha * std::sqrt( 1.0f - fRatio * fRatio ) ) / Bessel0( _fAlpha );
		}

		/**
		 * A helper function for the Mitchell filter.
		 *
		 * \param _fT The filter input.
		 * \param _fB A happy parameter.
		 * \param _fC A happy parameter.
		 * \return Returns happiness.
		 */
		static inline float										MitchellFilterHelper( float _fT, float _fB, float _fC ) {
			float fTt = _fT * _fT;
			_fT = std::fabs( _fT );
			if ( _fT < 1.0f ) {
				_fT = (((12.0f - 9.0f * _fB - 6.0f * _fC) * (_fT * fTt))
					+ ((-18.0f + 12.0f * _fB + 6.0f * _fC) * fTt)
					+ (6.0f - 2.0f * _fB));
				return (_fT / 6.0f);
			}
			else if ( _fT < 2.0f ) {
				_fT = (((-1.0f * _fB - 6.0f * _fC) * (_fT * fTt))
					+ ((6.0f * _fB + 30.0f * _fC) * fTt)
					+ ((-12.0f * _fB - 48.0f * _fC) * _fT)
					+ (8.0f * _fB + 24.0f * _fC));

				return (_fT / 6.0f);
			}

			return 0.0f;
		}

		/**
		 * A helper function.
		 *
		 * \param _fX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline float										BlackmanWindow( float _fX ) {
			// The truncated coefficients do not null the sidelobes as well, but have an improved 18 dB/oct fall-off.
			constexpr float fA = 0.16f;
			constexpr float fA0 = (1.0f - fA) / 2.0f;
			constexpr float fA1 = 1.0f / 2.0f;
			constexpr float fA2 = fA / 2.0f;
			return fA0 + fA1 * std::cos( std::numbers::pi_v<float> * _fX ) + fA2 * std::cos( 2.0f * std::numbers::pi_v<float> * _fX );
		}

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fB Helper value.
		 * \return Returns the filtered value.
		 */
		static inline float										QuadraticSharpFilterFunc( float _fT, float _fB ) {
			_fT = std::fabs( _fT );
			if ( _fT < 1.0f ) {
				float fTt = _fT * _fT;
				return _fT < 0.5f ? (-2.0f * _fB) * fTt + 0.5f * (_fB + 1.0f) :
					(_fB * fTt) + (-2.0f * _fB - 0.5f) * _fT + (3.0f / 4.0f) * (_fB + 1.0f);
			}
			return 0.0f;
		}

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fB Helper value.
		 * \return Returns the filtered value.
		 */
		static inline float										QuadraticFilterFunc2( float _fT, float _fB ) {
			_fT = std::fabs( _fT );
			constexpr float fThresh = 1.5f;
			if ( _fT < fThresh ) {
				float fTt = _fT * _fT;
				if ( _fT < 0.5f ) {
					return (-2.0f * _fB) * fTt + 0.5f * (_fB + 1.0f);
				}
				else {
					return (_fB * fTt) + (-2.0f * _fB - 0.5f) * _fT + (3.0f / 4.0f) * (_fB + 1.0f);
				}
			}
			return 0.0f;
		}

		/**
		 * Reject values below a specific epsilon.
		 *
		 * \param _fVal The value to test.
		 * \return Returns the given value if it is above a certain epsilon or 0.
		 */
		static inline float										Clean( float _fVal ) {
			constexpr float fEps = 0.0000125f;
			return std::fabs( _fVal ) >= fEps ? _fVal : 0.0f;
		}

		/**
		 * The point filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										PointFilterFunc( float _fT ) {
			return (_fT >= -0.5f && _fT < 0.5f) ? 1.0f : 0.0f;
		}

		/**
		 * The tent/bilinear filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										BilinearFilterFunc( float _fT ) {
			_fT = std::fabs( _fT );
			if ( _fT < 1.0f ) { return 1.0f - _fT; }
			return 0.0f;
		}

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										QuadraticSharpFilterFunc( float _fT ) { return QuadraticSharpFilterFunc( _fT, 1.0f ); }

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										QuadraticInterpolFilterFunc( float _fT ) { return QuadraticFilterFunc2( _fT, 1.0f ); }

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										QuadraticApproxFilterFunc( float _fT ) { return QuadraticFilterFunc2( _fT, 0.5f ); }

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										QuadraticMixFilterFunc( float _fT ) { return QuadraticFilterFunc2( _fT, 0.8f ); }

		/**
		 * The Kaiser filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										KaiserFilterFunc( float _fT ) {
			_fT = std::fabs( _fT );
			if ( _fT < 3.0f ) {
				static const float fAtt = 40.0f;
				static const float fAlpha = std::exp( std::log( 0.58417f * (fAtt - 20.96f) ) * 0.4f ) + 0.07886f * (fAtt - 20.96f);
				return Clean( Sinc( _fT ) * KaiserHelper( fAlpha, 3.0f, _fT ) );
			}
			return 0.0f;
		}

		/**
		 * The Lanczos filter function with X samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		template <unsigned _uX>
		static inline float										LanczosXFilterFunc( float _fT ) {
			_fT = std::fabs( _fT );
			if ( _fT <= float( _uX ) ) {
				return Clean( Sinc( _fT ) * Sinc( _fT / float( _uX ) ) );
			}
			return 0.0f;
		}

		/**
		 * The Mitchell filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										MitchellFilterFunc( float _fT ) {
			return MitchellFilterHelper( _fT, 1.0f / 3.0f, 1.0f / 3.0f );
		}

		/**
		 * The Robidoux filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										RobidouxFilterFunc( float _fT ) {
			constexpr float fB = 0.378215730190277099609375f;			// 12.0 / (19.0 + 9.0 * std::sqrt( 2.0 ))		// 0.37821575509399866698199807932338
			constexpr float fC = 0.3108921051025390625f;				// 113.0 / (58.0 + 216.0 * std::sqrt( 2.0 ))	// 0.31089212245300066650900096033831
			return MitchellFilterHelper( _fT,
				fB,
				fC );
		}

		/**
		 * The RobidouxSharp filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										RobidouxSharpFilterFunc( float _fT ) {
			constexpr float fB = 0.26201450824737548828125f;			// 6.0 / (13.0 + 7.0 * std::sqrt( 2.0 ))		// 0.26201451239901419648350807863652
			constexpr float fC = 0.3689927160739898681640625f;			// 7.0 / (2.0 + 12.0 * std::sqrt( 2.0 ))		// 0.36899274380049290175824596068174
			return MitchellFilterHelper( _fT,
				fB,
				fC );
		}

		/**
		 * The RobidouxSoft filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										RobidouxSoftFilterFunc( float _fT ) {
			constexpr float fB = 0.679622709751129150390625f;			// (9.0 - 3.0 * std::sqrt( 2.0 )) / 7.0			// 0.67962275898295926479927626105299
			constexpr float fC = 0.160188615322113037109375f;			// (-2.0 + 3.0 * std::sqrt( 2.0 )) / 14.0		// 0.16018862050852036760036186947351
			return MitchellFilterHelper( _fT,
				fB,
				fC );
		}

		/**
		 * The Catmull-Rom filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										CatmullRomFilterFunc( float _fT ) {
			return MitchellFilterHelper( _fT, 0.0f, 0.5f );
		}

		/**
		 * The B-Spline filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										BSplineFilterFunc( float _fT ) {
			return MitchellFilterHelper( _fT, 1.0f, 0.0f );
		}

		/**
		 * The Adobe bicubic filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										AdobeBicubicFilterFunc( float _fT ) {
			return MitchellFilterHelper( _fT, 0.0f, 0.75f );
		}

		/**
		 * The Cardinal Spline Uniform filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										CardinalSplineUniformFilterFunc( float _fT ) {
			return MitchellFilterHelper( _fT, 0.0f, 1.0f );
		}

		/**
		 * The Blackman filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										BlackmanFilterFunc( float _fT ) {
			if ( _fT < 0.0f ) { _fT = -_fT; }
			if ( _fT < 3.0f ) {
				return Clean( Sinc( _fT ) * BlackmanWindow( _fT / 3.0f ) );
			}
			return 0.0f;
		}

		/**
		 * The Gaussian filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										GaussianSharpFilterFunc( float _fT ) {
			if ( _fT < 0.0f ) { _fT = -_fT; }
			if ( _fT < 1.25f ) {
				return Clean( std::exp( -2.0f * _fT * _fT ) * std::sqrt( 2.0f / std::numbers::pi_v<float> ) * BlackmanWindow( _fT / 1.25f ) );
			}
			return 0.0f;
		}

		/**
		 * The Gaussian Sharp filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										GaussianFilterFunc( float _fT ) {
			if ( _fT < 0.0f ) { _fT = -_fT; }
			if ( _fT < 1.25f ) {
				const float fSigma = 0.52f;
				return Clean( std::exp( -(_fT * _fT) / (2.0f * fSigma * fSigma) ) * (1.0f / (fSigma * std::sqrt( 2.0f * std::numbers::pi_v<float> ))) * BlackmanWindow( _fT / 1.25f ) );
			}
			return 0.0f;
		}

		/**
		 * The Bell filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										BellFilterFunc( float _fT ) {
			if ( _fT < 0.0f ) { _fT = -_fT; }
			if ( _fT < 0.5f ) {
				return 0.75f - _fT * _fT;
			}
			if ( _fT < 1.5f ) {
				_fT = _fT - 1.5f;
				return 0.5f * _fT * _fT;
			}
			return 0.0f;
		}

		/**
		 * The Hermite filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										HermiteFilterFunc( float _fT ) {
			if ( _fT < 0.0f ) { _fT = -_fT; }
			if ( _fT < 1.0f ) {
				return (2.0f * _fT - 3.0f) * _fT * _fT + 1.0f;
			}
			return 0.0f;
		}

		/**
		 * The Hamming filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										HammingFilterFunc( float _fT ) {
			if ( _fT < 0.0f ) { _fT = -_fT; }
			if ( _fT < 1.0f ) {
				return 0.54f + 0.46f * std::cos( 2.0f * std::numbers::pi_v<float> * _fT );
			}
			return 0.0f;
		}

		/**
		 * The Hanning filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float										HanningFilterFunc( float _fT ) {
			if ( _fT < 0.0f ) { _fT = -_fT; }
			if ( _fT < 1.0f ) {
				return 0.5f + 0.5f * std::cos( 2.0f * std::numbers::pi_v<float> * _fT );
			}
			return 0.0f;
		}


		// == Members.
		/** Filter parameters. */
		static LSN_FILTER										m_fFilter[];

	protected :
		// == Types.
		/** Contribution bounds. */
		typedef struct LSN_CONTRIB_BOUNDS {
			/** The center. */
			float												fCenter;

			/** Left. */
			int32_t												i32Left;

			/** Right. */
			int32_t												i32Right;
		} * LPLSN_CONTRIB_BOUNDS, * const LPCLSN_CONTRIB_BOUNDS;


		// == Members.
		/** Our array of contributions. */
		std::vector<LSN_CONTRIBUTIONS>							m_cContribs;

	};

}	// namespace lsn
