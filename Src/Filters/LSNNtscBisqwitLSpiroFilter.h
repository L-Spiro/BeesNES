/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Bisqwit's NTSC filter with my own adjustments.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNFilterBase.h"


namespace lsn {

	/**
	 * Class CNtscBisqwitFilter
	 * \brief Bisqwit's NTSC filter.
	 *
	 * Description: Bisqwit's NTSC filter.
	 */
	class CNtscBisqwitLSpiroFilter : public CFilterBase {
	public :
		CNtscBisqwitLSpiroFilter();
		virtual ~CNtscBisqwitLSpiroFilter();


		// == Functions.
		/**
		 * Gets the PPU output format.
		 *
		 * \return Returns the output format from the PPU/input format for this filter.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const { return CDisplayClient::LSN_POF_9BIT_PALETTE; }

		/**
		 * If true, the PPU is requested to provide a frame that has been flipped vertically.
		 *
		 * \return Returns true to receive a vertically flipped image from the PPU, false to receive an unflipped image.
		 */
		virtual bool										FlipInput() const { return false; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() { return CurTarget(); }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const { return 32; }


	protected :
		// == Members.
		float												m_fBlack = 0.312f;						/**< Black level. */
		float												m_fWhite = 1.100f;						/**< White level. */
		static const float									m_fLevels[16];							/**< Output levels. */


		// == Functions.
		/**
		 * Converts a 9-bit PPU output index to an NTSC signal.
		 * 
		 * \param _ui16Pixel The PPU output index to convert.
		 * \param _ui16Phase The phase counter.
		 * \return Returns the signal produced by the PPU output index.
		 **/
		float												IndexToNtscSignal( uint16_t _ui16Pixel, uint16_t _ui16Phase );

	};

}	// namespace lsn
