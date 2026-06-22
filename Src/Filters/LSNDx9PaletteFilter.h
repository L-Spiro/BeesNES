#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Direct3D 9 filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../GPU/DirectX9/LSNDirectX9PixelShader.h"
#include "../GPU/DirectX9/LSNDirectX9RenderTarget.h"
#include "../GPU/DirectX9/LSNDirectX9Texture.h"
#include "../GPU/DirectX9/LSNDirectX9VertexBuffer.h"
#include "LSNDx9FilterBase.h"

#include <Helpers/LSWHelpers.h>

#include <algorithm>
#include <memory>


namespace lsn {

	/**
	 * Class CDx9PaletteFilter
	 * \brief A standard 24-bit RGB filter.
	 *
	 * Description: A standard 24-bit RGB filter.
	 */
	class CDx9PaletteFilter : public CDx9FilterBase {
	public :
		CDx9PaletteFilter();
		virtual ~CDx9PaletteFilter();
		
		
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
			if ( fResolutionFactor < 2.5 ) { return CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM; }
			if ( fResolutionFactor < 3.5 ) { return CResamplerBase::LSN_FF_ROBIDOUXSHARP; }
			return CResamplerBase::LSN_FF_LINEAR;
		}

		/**
		 * Sets the palette.
		 * 
		 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
		 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.
		 **/
		bool												SetLut( const float * _pfRgba512 );

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate() override;

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
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
			int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) override;

		/**
		 * Informs the filter of a window resize.
		 **/
		virtual void										FrameResize() override;


	protected :
		// == Members.
		/** Index texture (D3DFMT_L16). */
		std::unique_ptr<CDirectX9Texture>					m_tIndex;
		/** 512x1 LUT texture (D3DFMT_A32B32G32R32F). */
		std::unique_ptr<CDirectX9Texture>					m_tLut;
		/** Initial floating-point render target. */
		std::unique_ptr<CDirectX9RenderTarget>				m_rtInitial;
		
		/** Screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX9VertexBuffer>				m_vbPass1;
		/** The pixel shader for Pass 1 (Indices to Colors). */
		std::unique_ptr<CDirectX9PixelShader>				m_psIdxToColor;

		/** The palette look-up table. */
		std::vector<float>									m_vLut;
		/** The output buffer. */
		std::vector<uint8_t>								m_vOutputBuffer;

		/** Source width in pixels. */
		uint32_t											m_ui32SrcW = 0;
		/** Source height in pixels. */
		uint32_t											m_ui32SrcH = 0;
		bool												m_bValidState = false;
		bool												m_bUpdatePalette = true;
			

		// == Functions.
		/**
		 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
		 * 
		 * \return Returns true on success.
		 */
		bool												EnsureSizeAndResources();

		/**
		 * \brief Updates the 512-entry float RGBA LUT.
		 * 
		 * \return Returns true on success.
		 */
		bool												UpdateLut();

		/**
		 * \brief Uploads the 16-bit PPU indices to the index texture.
		 * 
		 * \param _pui16Idx Source pointer to the index image (row-major).
		 * \param _ui32W Image width in pixels.
		 * \param _ui32H Image height in pixels.
		 * \param _ui32SrcPitch Source pitch in bytes; pass 0 for tightly packed.
		 * \return Returns true on success.
		 */
		bool												UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch = 0 );

		/**
		 * \brief Ensures pixel shaders, vertex shaders, and PSOs are created.
		 * 
		 * \return Returns true if all shaders are ready.
		 */
		bool												EnsureShaders();

		/**
		 * \brief Compiles an HLSL shader using dynamically loaded d3dcompiler_47.dll.
		 *
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name (e.g., "main").
		 * \param _pcszProfile Null-terminated profile (e.g., "ps_2_0").
		 * \param _vOutByteCode Output vector to receive the compiled bytecode (DWORD stream).
		 * \return Returns true if compilation succeeded and bytecode was produced.
		 */
		bool												CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode );

		/**
		 * \brief Releases size-dependent resources.
		 */
		void												ReleaseSizeDependents();

		/**
		 * \brief Renders the three-pass pipeline to the backbuffer with a black border outside _rOutput.
		 *
		 * Precondition: The caller must have already called BeginScene() on the device.
		 * Postcondition: The scene remains open; the caller is responsible for EndScene() and Present().
		 *
		 * Pass 1 renders index/color into the FP RT with a 1:1 viewport.
		 * Pass 2 renders the FP RT into the scanlined RT (height * factor) using nearest-neighbor vertically.
		 * Pass 3 clears the backbuffer black and draws the resampled RT into the destination rectangle.
		 *
		 * \param _rOutput The destination rectangle in client pixels where the NES image should appear.
		 * \return Returns true if the draw succeeded; false on failure.
		 */
		bool												Render( const lsw::LSW_RECT &_rOutput );

	private :
		typedef CDx9FilterBase								CParent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
