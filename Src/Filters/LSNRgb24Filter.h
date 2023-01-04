/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard 24-bit RGB filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNFilterBase.h"
#include "nes_ntsc/nes_ntsc.h"
#include <vector>


namespace lsn {

	/**
	 * Class CRgb24Filter
	 * \brief A standard 24-bit RGB filter.
	 *
	 * Description: A standard 24-bit RGB filter.
	 */
	class CRgb24Filter : public CFilterBase {
	public :
		CRgb24Filter();
		virtual ~CRgb24Filter();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height );

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
		virtual uint8_t *									CurTarget() { return m_vBasicRenderTarget[m_stBufferIdx].data(); }

		/**
		 * Gets a pointer to the next render target.
		 *
		 * \return Returns a pointer to the next render target.
		 */
		virtual uint8_t *									NextTarget() { return m_vBasicRenderTarget[(m_stBufferIdx+1)%m_vBasicRenderTarget.size()].data(); }

		/**
		 * Swaps to the next render target.
		 */
		virtual void										Swap();

	protected :
		// == Members.
		/** The render target for very basic software rendering.  N-buffered. */
		std::vector<std::vector<uint8_t>>					m_vBasicRenderTarget;
		
	};

}	// namespace lsn
