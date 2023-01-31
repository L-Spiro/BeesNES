/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for post-processing filters.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <vector>


namespace lsn {

	/**
	 * Class CPostProcessBase
	 * \brief The base class for all filters.
	 *
	 * Description: The base class for all filters.
	 */
	class CPostProcessBase {
	public :
		CPostProcessBase() {
		}
		virtual ~CPostProcessBase() {
		}


		// == Enumerations.
		/** The types of post-processing filters. */
		enum LSN_POST_PROCESSES {
			LSN_PP_NONE,									/**< No post-processing. */
			LSN_PP_BILINEAR,								/**< Bilinear scaling. */
			LSN_PP_SRGB,									/**< linear -> sRGB. */

			LSN_PP_TOTAL,									/**< The total number of filters.  Must be last. */
		};


		// == Functions.
		/**
		 * Applies the filter to the given input
		 *
		 * \param _ui32ScreenWidth The screen width.
		 * \param _ui32ScreenHeight The screen height.
		 * \param _bFlipped Indicates if the image is flipped on input and set to true to indicate that the image is in a flipped state on output.
		 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
		 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
		 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
		 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
		 * \param _ui64PpuFrame The PPU frame associated with the input data.
		 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input,
			uint32_t /*_ui32ScreenWidth*/, uint32_t /*_ui32ScreenHeight*/, bool &/*_bFlipped*/,
			uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/,
			uint64_t /*_ui64RenderStartCycle*/ ) { return _pui8Input; }


	protected :
		// == Members.
		/** A buffer that can be used as the final output. */
		std::vector<uint8_t>								m_vFinalBuffer;
	};

}	// namespace lsn
