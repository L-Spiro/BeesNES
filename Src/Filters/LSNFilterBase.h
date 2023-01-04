/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for all filters.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Display/LSNDisplayClient.h"


namespace lsn {

	/**
	 * Class CFilterBase
	 * \brief The base class for all filters.
	 *
	 * Description: The base class for all filters.
	 */
	class CFilterBase {
	public :
		CFilterBase();
		virtual ~CFilterBase();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t /*_stBuffers*/, uint16_t /*_ui16Width*/, uint16_t /*_ui16Height*/ ) { return InputFormat(); }

		/**
		 * Gets the PPU output format.
		 *
		 * \return Returns the output format from the PPU/input format for this filter.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const { return CDisplayClient::LSN_POF_RGB; }

		/**
		 * Gets a pointer to the current render target.
		 *
		 * \return Returns a pointer to the current render target.
		 */
		virtual uint8_t *									CurTarget() { return nullptr; }

		/**
		 * Gets a pointer to the next render target.
		 *
		 * \return Returns a pointer to the next render target.
		 */
		virtual uint8_t *									NextTarget() { return nullptr; }

		/**
		 * Swaps to the next render target.
		 */
		virtual void										Swap() {}

		/**
		 * Gets the render-target stride.
		 *
		 * \return Returns the render-target stride.
		 */
		inline size_t										Stride() const { return m_stStride; }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const { return 24; }

		/**
		 * Gets a BITMAP stride given its row width in bytes.
		 *
		 * \param _ui32RowWidth The row width in RGB(A) pixels.
		 * \param _ui32BitDepth The total bits for a single RGB(A) pixel.
		 * \return Returns the byte width rounded up to the nearest DWORD.
		 */
		static inline uint32_t								RowStride( uint32_t _ui32RowWidth, uint32_t _ui32BitDepth ) {
			return ((((_ui32RowWidth * _ui32BitDepth) + 31) & ~31) >> 3);
		}


	protected :
		// == Members.
		/** The current buffer index. */
		size_t												m_stBufferIdx;
		/** The render-target stride. */
		size_t												m_stStride;
	};

}	// namespace lsn
