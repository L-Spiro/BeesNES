/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for GPU filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNFilterBase.h"
#include <vector>


namespace lsn {

	/**
	 * Class CGpuFilterBase
	 * \brief The base class for GPU filters.
	 *
	 * Description: The base class for GPU filters.
	 */
	class CGpuFilterBase : public CFilterBase {
	public :
		CGpuFilterBase();
		virtual ~CGpuFilterBase();


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
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const { return CDisplayClient::LSN_POF_9BIT_PALETTE; }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const { return 32; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() { return CurTarget(); }

		/**
		 * If this is a GPU filter, some of the pipeline is different, and extra steps to manage the filter are needed.
		 * 
		 * \return Returns true.
		 **/
		virtual bool										IsGpuFilter() const { return true; }

	protected :
		// == Members.
		
	};

}	// namespace lsn
