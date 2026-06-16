/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Texture-addressing modes.
 */

#pragma once

#include <algorithm>
#include <cstdint>

namespace lsn {

	// == Enumerations.
	/** Errors. */
	enum LSN_TEXTURE_ADDRESSING : int16_t {
		LSN_TA_WRAP												= 0,	/**< The texture coordinates are wrapped around. This means that the texture repeats at every integer junction. */
		LSN_TA_MIRROR											= 1,	/**< The texture coordinates are mirrored at every integer junction, which results in a mirrored tiling effect. */
		LSN_TA_CLAMP											= 2,	/**< The texture coordinates are clamped to the range [0,1]. Coordinates outside this range are clamped to the edge values. */
		LSN_TA_BORDER											= 3,	/**< The texture coordinates outside the [0,1] range are set to a border color. */
		LSN_TA_MIRROR_ONCE										= 4,	/**< Similar to LSN_TA_MIRROR, but it mirrors only once. Any further coordinates outside the range are clamped to the edges. */
		LSN_TA_NULL_BORDER										= 5,	/**< No border at all.  Indexed as -2.  Resampler should not include this sample in convolution. */

		LSN_TA_TOTAL
	};


	/**
	 * Class CTextureAddressing
	 * \brief Texture-addressing modes.
	 *
	 * Description: Texture-addressing modes.
	 */
	class CTextureAddressing {
	public :
		// == Types.
		typedef uint32_t (*										PfAddressingFunc)( uint32_t, int32_t );


		// == Functions.
		/**
		 * Returns the new texture index after applying the LSN_TA_WRAP texture-addressing mode.
		 * 
		 * \param _ui32TextureSize The number of texels along the texture’s axis.
		 * \param _i32Idx The texel index to translate.
		 * \return Returns the wrapped texel index.
		 **/
		static inline uint32_t									Wrap( uint32_t _ui32TextureSize, int32_t _i32Idx ) {
			if ( _i32Idx < 0 ) { return _ui32TextureSize - (~_i32Idx % _ui32TextureSize) - 1; }
			return _i32Idx % _ui32TextureSize;
		}

		/**
		 * Returns the new texture index after applying the LSN_TA_MIRROR texture-addressing mode.
		 * 
		 * \param _ui32TextureSize The number of texels along the texture’s axis.
		 * \param _i32Idx The texel index to translate.
		 * \return Returns the mirrored texel index.
		 **/
		static inline uint32_t									Mirror( uint32_t _ui32TextureSize, int32_t _i32Idx ) {
			if ( _i32Idx < 0 ) { _i32Idx = -_i32Idx; }
			if ( (_i32Idx / _ui32TextureSize) % 2 == 0 ) {
				return _i32Idx % _ui32TextureSize;
			}
			return _ui32TextureSize - (_i32Idx % _ui32TextureSize - 1);
		}

		/**
		 * Returns the new texture index after applying the LSN_TA_CLAMP texture-addressing mode.
		 * 
		 * \param _ui32TextureSize The number of texels along the texture’s axis.
		 * \param _i32Idx The texel index to translate.
		 * \return Returns the clamped texel index.
		 **/
		static inline uint32_t									Clamp( uint32_t _ui32TextureSize, int32_t _i32Idx ) {
			return std::max( 0, std::min( _i32Idx, static_cast<int32_t>(_ui32TextureSize - 1) ) );
		}

		/**
		 * Returns the new texture index after applying the LSN_TA_BORDER texture-addressing mode.
		 * 
		 * \param _ui32TextureSize The number of texels along the texture’s axis.
		 * \param _i32Idx The texel index to translate.
		 * \return Returns the texel index or -1 to indicate using the border color.
		 **/
		static inline uint32_t									Border( uint32_t _ui32TextureSize, int32_t _i32Idx ) {
			return (_i32Idx >= 0 && static_cast<uint32_t>(_i32Idx) < _ui32TextureSize) ? _i32Idx : -1;
		}

		/**
		 * Returns the new texture index after applying the LSN_TA_MIRROR_ONCE texture-addressing mode.
		 * 
		 * \param _ui32TextureSize The number of texels along the texture’s axis.
		 * \param _i32Idx The texel index to translate.
		 * \return Returns the mirrored-once texel index.
		 **/
		static inline uint32_t									MirrorOnce( uint32_t _ui32TextureSize, int32_t _i32Idx ) {
			if ( _i32Idx < 0 ) { _i32Idx = -_i32Idx; }
			return (_i32Idx >= 0 && static_cast<uint32_t>(_i32Idx) < _ui32TextureSize) ? _i32Idx : _ui32TextureSize - 1;
		}

		/**
		 * Returns the new texture index after applying the LSN_TA_MIRROR_ONCE texture-addressing mode.
		 * 
		 * \param _ui32TextureSize The number of texels along the texture’s axis.
		 * \param _i32Idx The texel index to translate.
		 * \return Returns the mirrored-once texel index.
		 **/
		static inline uint32_t									NullBorder( uint32_t _ui32TextureSize, int32_t _i32Idx ) {
			if ( _i32Idx < 0 || static_cast<uint32_t>(_i32Idx) >= _ui32TextureSize ) { return static_cast<uint32_t>(-2); }
			return static_cast<uint32_t>(_i32Idx);
		}


		// == Members.
		/** Table of addressing functions, indexable via the LSN_TEXTURE_ADDRESSING enumeration. */
		static PfAddressingFunc									m_pfFuncs[LSN_TA_TOTAL];
	};

}	// namespace lsn
