#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: LMP88959’s implementation of a PAL filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../GPU/DirectX9/LSNDirectX9TextureUploader.h"
#include "LSNDx9FilterBase.h"

#include <mutex>
#include <vector>


namespace lsn {

	/**
	 * Class CDx9PalCrtFullFilter
	 * \brief LMP88959’s implementation of a PAL filter.
	 *
	 * Description: LMP88959’s implementation of a PAL filter.
	 */
	class CDx9PalCrtFullFilter : public CDx9FilterBase {
	public :
		CDx9PalCrtFullFilter();
		virtual ~CDx9PalCrtFullFilter();
		
		
		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) override;

		/**
		 * Gets the convolution sampler to use for resampling.
		 * 
		 * \param _ui32Width The target width.
		 * \param _ui32Height The target height.
		 * \return Returns the desired convolution sampler to use.
		 **/
		virtual inline CResamplerBase::LSN_FILTER_FUNCS		GetPreferredConvolutionFilter( uint32_t _ui32Width, uint32_t _ui32Height ) override {
			// For low resolutions, use the sharpest-possible filter.
			float fResolutionFactor = std::min( static_cast<float>(_ui32Width) / static_cast<float>(m_ui32SrcW), static_cast<float>(_ui32Height) / static_cast<float>(m_ui32SrcH) );
			if ( fResolutionFactor < 2.5 ) { return CResamplerBase::LSN_FF_ROBIDOUX; }
			return CResamplerBase::LSN_FF_LINEAR;
		}

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
		 * \param _i32DispLeft The display area left.
		 * \param _i32DispTop The display area top.
		 * \param _ui32DispWidth The display area width.
		 * \param _ui32DispHeight The display area height
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
			int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) override;

		/**
		 * Gets the PPU output format.
		 *
		 * \return Returns the output format from the PPU/input format for this filter.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const override { return CDisplayClient::LSN_POF_9BIT_PALETTE; }

		/**
		 * If true, the PPU is requested to provide a frame that has been flipped vertically.
		 *
		 * \return Returns true to receive a vertically flipped image from the PPU, false to receive an unflipped image.
		 */
		virtual bool										FlipInput() const override { return false; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() override { return CurTarget(); }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const override { return 32; }

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate() override;

		/**
		 * Informs the filter of a window resize.
		 **/
		virtual void										FrameResize() override;


	protected :
		// == Members.
		/** Generically uploads CPU texel arrays to GPU textures. */
		CDirectX9TextureUploader							m_tuUploader;

		/** Source width in pixels. */
		uint32_t											m_ui32SrcW = 0;
		/** Source height in pixels. */
		uint32_t											m_ui32SrcH = 0;
		/** Are we in a valid state? */
		bool												m_bValidState = false;

		/** The EMMIR (LMP88959) PAL emulation. */
		std::vector<uint8_t>								m_vSettings;
		/** The CRT structure. */
		std::vector<uint8_t>								m_vCrtPal;
		/** The phase table. */
		int													m_iPhaseRef[4];
		/** The final width. PAL is CRT_HRES pixels wide. */
		uint32_t											m_ui32FinalWidth;
		/** The final height. */
		uint32_t											m_ui32FinalHeight;

		/** The final stride. */
		uint32_t											m_ui32FinalStride;
		/** The output created by calling FilterFrame(). */
		std::vector<uint8_t>								m_vRgbBuffer;


		// == Functions.
		/**
		 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
		 * 
		 * \return Returns true on success.
		 */
		bool												EnsureSizeAndResources();

		/**
		 * Renders the final output to the backbuffer.
		 *
		 * \param _rOutput The destination rectangle.
		 * \return Returns true if rendering succeeded.
		 */
		bool												Render( const lsw::LSW_RECT &_rOutput );

	private :
		typedef CDx9FilterBase								CParent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
