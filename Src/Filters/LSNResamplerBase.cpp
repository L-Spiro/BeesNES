/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Base GPU resampler setup.
 */

#include "LSNResamplerBase.h"
#include "../OS/LSNOs.h"
#include "../Utilities/LSNUtilities.h"


namespace lsn {

	// == Members.
	/** Filter parameters. */
	CResamplerBase::LSN_FILTER CResamplerBase::m_fFilter[] = {
		{ CResamplerBase::PointFilterFunc,					0.5f },
		{ CResamplerBase::BilinearFilterFunc,				1.0f },
		{ CResamplerBase::QuadraticSharpFilterFunc,			1.0f },
		{ CResamplerBase::QuadraticInterpolFilterFunc,		1.5f },
		{ CResamplerBase::QuadraticApproxFilterFunc,		1.5f },
		{ CResamplerBase::QuadraticMixFilterFunc,			1.5f },
		{ CResamplerBase::KaiserFilterFunc,					3.0f },
		{ CResamplerBase::LanczosXFilterFunc<2>,			2.0f },
		{ CResamplerBase::LanczosXFilterFunc<3>,			3.0f },
		{ CResamplerBase::LanczosXFilterFunc<4>,			4.0f },
		{ CResamplerBase::LanczosXFilterFunc<6>,			6.0f },
		{ CResamplerBase::LanczosXFilterFunc<8>,			8.0f },
		{ CResamplerBase::LanczosXFilterFunc<12>,			12.0f },
		{ CResamplerBase::LanczosXFilterFunc<64>,			64.0f },
		{ CResamplerBase::MitchellFilterFunc,				2.0f },
		{ CResamplerBase::RobidouxFilterFunc,				2.0f },
		{ CResamplerBase::RobidouxSharpFilterFunc,			2.0f },
		{ CResamplerBase::RobidouxSoftFilterFunc,			2.0f },
		{ CResamplerBase::CatmullRomFilterFunc,				2.0f },
		{ CResamplerBase::BSplineFilterFunc,				2.0f },
		{ CResamplerBase::AdobeBicubicFilterFunc,			2.0f },
		{ CResamplerBase::CardinalSplineUniformFilterFunc,	2.0f },
		{ CResamplerBase::HermiteFilterFunc,				1.0f },
		{ CResamplerBase::HammingFilterFunc,				1.0f },
		{ CResamplerBase::HanningFilterFunc,				1.0f },
		{ CResamplerBase::BlackmanFilterFunc,				3.0f },
		{ CResamplerBase::JincFilterFunc,					3.0f },
		{ CResamplerBase::GaussianSharpFilterFunc,			1.25f },
		{ CResamplerBase::GaussianFilterFunc,				1.25f },
		{ CResamplerBase::BellFilterFunc,					1.5f },
	};

	CResamplerBase::CResamplerBase() {
	}
	CResamplerBase::~CResamplerBase() {
	}

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
	bool CResamplerBase::CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
		LSN_TEXTURE_ADDRESSING _taAddressMode,
		PfFilterFunc _pfFilter, float _fFilterSupport, float _fFilterScale ) {

		std::vector<LSN_CONTRIB_BOUNDS> vBounds;
		try {
			m_cContribs.clear();
			m_cContribs.resize( _ui32DstSize );
			vBounds.resize( _ui32DstSize );
		}
		catch ( ... ) { return false; }


		// For fast division.
		const float fInvFilterScale = 1.0f / _fFilterScale;
		constexpr float fNudge = 0.5f;

		float fScale = static_cast<float>(_ui32DstSize) / static_cast<float>(_ui32SrcSize);
		float fOrigScale = fScale;

		fScale = std::min( fScale, 1.0f );

		float fHalfWidth = (_fFilterSupport / fScale) * fInvFilterScale;


		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			// Convert from discrete to continuous coordinates, then back.
			vBounds[I].fCenter = (static_cast<float>(I) + fNudge) / fOrigScale;
			vBounds[I].fCenter -= fNudge;
			vBounds[I].i32Left = static_cast<int32_t>(std::floor( vBounds[I].fCenter - fHalfWidth ));
			vBounds[I].i32Right = static_cast<int32_t>(std::ceil( vBounds[I].fCenter + fHalfWidth ));
		}

		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			int32_t i32MaxK = -1;
			float fMaxW = -1.0e20f;
			//float fCenter = vBounds[I].fCenter;
			int32_t i32Left = vBounds[I].i32Left;
			int32_t i32Right = vBounds[I].i32Right;

			try {
				m_cContribs[I].fContributions.resize( i32Right - i32Left + 1 );
				m_cContribs[I].i32Indices.resize( i32Right - i32Left + 1 );
			}
			catch ( ... ) { return false; }

			float fTotalWeight = 0.0f;
			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				if ( CTextureAddressing::m_pfFuncs[_taAddressMode]( _ui32SrcSize, J ) != -2 ) {
					fTotalWeight += (*_pfFilter)( (vBounds[I].fCenter - J) * fScale * _fFilterScale );
				}
			}

			const float fNorm = 1.0f / fTotalWeight;
			fTotalWeight = 0.0f;

			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				float fThisWeight = (*_pfFilter)( (vBounds[I].fCenter - J) * fScale * _fFilterScale ) * fNorm;

				m_cContribs[I].i32Indices[J-i32Left] = CTextureAddressing::m_pfFuncs[_taAddressMode]( _ui32SrcSize, J );
				if ( m_cContribs[I].i32Indices[J-i32Left] == -2 ) {
					fThisWeight = 0.0f;
				}

				if ( fThisWeight == 0.0f ) { continue; }

				m_cContribs[I].fContributions[J-i32Left] = fThisWeight;

				fTotalWeight += fThisWeight;

				if ( fThisWeight > fMaxW ) {
					fMaxW = fThisWeight;
					i32MaxK = J - i32Left;
				}
			}
			
			if ( i32MaxK == -1 ) { return false; }

			if ( fTotalWeight != 1.0f && i32MaxK >= 0 ) {
				m_cContribs[I].fContributions[i32MaxK] += 1.0f - fTotalWeight;
			}

			// Trim 0 values.
			for ( size_t J = m_cContribs[I].fContributions.size(); J--; ) {
				if ( 0.0f == m_cContribs[I].fContributions[J] ) {
					m_cContribs[I].fContributions.erase( m_cContribs[I].fContributions.begin() + J );
					m_cContribs[I].i32Indices.erase( m_cContribs[I].i32Indices.begin() + J );
				}
			}
		}

		return true;
	}

}	// namespace lsn
