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
#include <vector>


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


		// == Enumerations.
		/** The types of filters. */
		enum LSN_FILTERS {
			LSN_F_RGB24,									/**< Standard RGB24. No filter. */
			LSN_F_NTSC_BLARGG,								/**< Blargg's NTSC filter. */
			LSN_F_PAL_BLARGG,								/**< Blargg's NTSC filter adjusted for PAL. */
			LSN_F_NTSC_LSPIRO,								/**< L. Spiro's NTSC filter. */
			LSN_F_PAL_LSPIRO,								/**< L. Spiro's PAL filter. */
			LSN_F_PALM_LSPIRO,								/**< L. Spiro's PAL-M filter. */
			LSN_F_PALN_LSPIRO,								/**< L. Spiro's PAL-N filter. */
			LSN_F_NTSC_CRT_FULL,							/**< EMMIR (LMP88959)'s NTSC-CRT (full) filter. */
			LSN_F_PAL_CRT_FULL,								/**< EMMIR (LMP88959)'s PAL-CRT (full) filter. */
			LSN_F_AUTO_BLARGG,								/**< Either NTSC or PAL Blargg. */
			//LSN_F_AUTO_CRT,									/**< NTSC-CRT for NTSC, Blargg PAL for PAL and Dendy. */
			LSN_F_AUTO_CRT_FULL,							/**< NTSC-CRT (full) for NTSC and PAL. */
			LSN_F_AUTO_LSPIRO,								/**< L. Spiro's NTSC/PAL filter. */

			LSN_F_TOTAL,									/**< The total number of filters.  Must be last. */
		};


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
		 * If true, the PPU is requested to provide a frame that has been flipped vertically.
		 *
		 * \return Returns true to receive a vertically flipped image from the PPU, false to receive an unflipped image.
		 */
		virtual bool										FlipInput() const { return true; }

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
		virtual void										Swap() { m_stBufferIdx = (m_stBufferIdx + 1) % m_vBasicRenderTarget.size(); }

		/**
		 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.  The final buffer, along with
		 *	its width, height, bit-depth, and stride, are returned.
		 *
		 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.  Its format will be that returned in InputFormat().
		 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
		 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
		 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
		 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
		 * \param _ui64PpuFrame The PPU frame associated with the input data.
		 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/,
			uint64_t /*_ui64RenderStartCycle*/ ) { return _pui8Input; }

		/**
		 * Gets the render-target stride.
		 *
		 * \return Returns the render-target stride.
		 */
		inline size_t										OutputStride() const { return m_stStride; }

		/**
		 * Gets the output buffer's width in pixels.
		 *
		 * \return Returns the output buffer's width in pixels.
		 */
		inline uint32_t										OutputWidth() const { return m_ui32OutputWidth; }

		/**
		 * Gets the output buffer's height in pixels.
		 *
		 * \return Returns the output buffer's height in pixels.
		 */
		inline uint32_t										OutputHeight() const { return m_ui32OutputHeight; }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const { return 24; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() { return nullptr; }

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate() {}

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
		/** The render target for very basic software rendering.  N-buffered. */
		std::vector<std::vector<uint8_t>>					m_vBasicRenderTarget;
		/** The current buffer index. */
		size_t												m_stBufferIdx;
		/** The render-target stride. */
		size_t												m_stStride;
		/** The render-target width in pixels. */
		uint32_t											m_ui32OutputWidth;
		/** The render-target height in pixels. */
		uint32_t											m_ui32OutputHeight;
	};

}	// namespace lsn
