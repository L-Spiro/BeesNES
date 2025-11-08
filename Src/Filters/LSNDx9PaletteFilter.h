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
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height );

		/**
		 * \brief Updates the vertical sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The vertical sharpness.
		 */
		void												SetVertSharpness( uint32_t _ui32Factor ) { m_ui32VertSharpness = std::max<uint32_t>( 1, _ui32Factor ); }

		/**
		 * \brief Gets the current vertical sharpness factor.
		 * 
		 * \return Returns the vertical sharpness factor.
		 */
		inline uint32_t										GetVertSharpness() const { return m_ui32VertSharpness; }

		/**
		 * \brief Updates the horizontal sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The horizontal sharpness.
		 */
		void												SetHorSharpness( uint32_t _ui32Factor ) { m_ui32HorSharness = std::max<uint32_t>( 1, _ui32Factor ); }

		/**
		 * \brief Gets the current horizontal sharpness factor.
		 * 
		 * \return Returns the horizontal sharpness factor.
		 */
		inline uint32_t										GetHorSharpness() const { return m_ui32HorSharness; }

		/**
		 * Sets whether to use a 16-bit render target for the initial pass.  Must be called before the filter is actually used.
		 * 
		 * \param _bUse16Bit If true, a 16-bit target is used, otherwise a 32-bit target is used.
		 **/
		inline void											Use16Target( bool _bUse16Bit ) { m_bUse16BitInitialTarget = _bUse16Bit; }

		/**
		 * Sets the palette.
		 * 
		 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
		 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.  False always indicates a memory failure if _pfRgba512 is not nullptr.
		 **/
		bool												SetLut( const float * _pfRgba512 );

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate();

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate();

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
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/ );


	protected :
		// == Types.
		/** The DirectX 9 device wrapper (non-owning). */
		CDirectX9Device *									m_pdx9dDevice = nullptr;
		/** Index texture (L16, DEFAULT|DYNAMIC). */
		std::unique_ptr<CDirectX9Texture>					m_tIndex;
		/** 512Å~1 LUT texture (A32B32G32R32F, MANAGED). */
		std::unique_ptr<CDirectX9Texture>					m_tLut;
		/** Initial floating-point render target (same size as indices). */
		std::unique_ptr<CDirectX9RenderTarget>				m_rtInitial;
		/** Scanlined floating-point render target (height = source height Å~ factor). */
		std::unique_ptr<CDirectX9RenderTarget>				m_rtScanlined;
		/** Dynamic screen-space quad vertex buffer (XYZRHW|TEX1, 4 vertices). */
		std::unique_ptr<CDirectX9VertexBuffer>				m_vbQuad;
		/** Pixel shader: indices + LUT Å® RGBA (pass 1). */
		std::unique_ptr<CDirectX9PixelShader>				m_psIdxToColor;
		/** Pixel shader: vertical nearest-neighbor upscale (pass 2). */
		std::unique_ptr<CDirectX9PixelShader>				m_psVerticalNN;
		/** Pixel shader: copy pass (pass 3). */
		std::unique_ptr<CDirectX9PixelShader>				m_psCopy;
		/** The palette look-up table. */
		std::vector<float>									m_vLut;
		/** The output buffer.  Not actually used, but provides a safe buffer for reading in some edges cases after filters are swapped. Also can be used for storing screenshots. */
		std::vector<uint8_t>								m_vOutputBuffer;
		/** Source width in pixels. */
		uint32_t											m_ui32SrcW = 0;
		/** Source height in pixels. */
		uint32_t											m_ui32SrcH = 0;
		/** Created resource width. */
		uint32_t											m_ui32RsrcW = 0;
		/** Created resource height. */
		uint32_t											m_ui32RsrcH = 0;
		/** Vertical sharpness factor. */
		uint32_t											m_ui32VertSharpness = 2;
		/** Horizontal sharpness factor. */
		uint32_t											m_ui32HorSharness = 2;
		/** Use a 16-bit initial render target? */
		bool												m_bUse16BitInitialTarget = true;
		/** Are we in a valid state? */
		bool												m_bValidState = false;
		/** Does the palette need to be uploaded? */
		bool												m_bUpdatePalette = true;
			

		// == Functions.
		/**
		 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
		 *
		 * Releases/creates the index texture, both FP render targets, and quad vertex buffer as needed.
		 *
		 * \return Returns true on success.
		 */
		bool												EnsureSizeAndResources();

		/**
		 * \brief Updates the 512-entry float RGBA LUT.
		 *
		 * The LUT is a 512Å~1 A32B32G32R32F MANAGED texture. Each entry is RGBA in linear space.
		 *
		 * \return Returns true on success.
		 */
		bool												UpdateLut();

		/**
		 * \brief Uploads the 16-bit PPU indices (9-bit effective values 0..511) to the L16 index texture.
		 *
		 * Values are mapped to L16 in [0..65535] so that sampling.r*511 + 0.5 floors back to the exact index in the shader.
		 *
		 * \param _pui16Idx Source pointer to the index image (row-major).
		 * \param _ui32W Image width in pixels (must equal the Init size).
		 * \param _ui32H Image height in pixels (must equal the Init size).
		 * \param _ui32SrcPitch Source pitch in bytes; pass 0 for tightly packed (= _ui32W * 2).
		 * \return Returns true on success.
		 */
		bool												UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch = 0 );

		/**
		 * \brief Ensures pixel shaders (indexÅ®color, vertical NN, copy) are created.
		 *
		 * Compiles the HLSL entry points with D3DX at runtime and creates pixel shaders from bytecode.
		 * If D3DX cannot be loaded, this function returns false.
		 *
		 * \return Returns true if all shaders are ready.
		 */
		bool												EnsureShaders();

		/**
		 * \brief Compiles an HLSL pixel shader using dynamically loaded D3DX.
		 *
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name (e.g., "main").
		 * \param _pcszProfile Null-terminated profile (e.g., "ps_2_0").
		 * \param _vOutByteCode Output vector to receive the compiled bytecode (DWORD stream).
		 * \return Returns true if compilation succeeded and bytecode was produced.
		 */
		bool												CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> & _vOutByteCode );

		/**
		 * \brief Releases size-dependent resources (index texture, FP RTs, quad VB).
		 */
		void												ReleaseSizeDependents();

		/**
		 * \brief Renders the three-pass pipeline to the backbuffer with a black border outside _rOutput.
		 *
		 * Precondition: The caller must have already called BeginScene() on the device.
		 * Postcondition: The scene remains open; the caller is responsible for EndScene() and Present().
		 *
		 * Pass 1 renders indexÅ®color into the FP RT with a 1:1 viewport.
		 * Pass 2 renders the FP RT into the scanlined RT (heightÅ~factor) using nearest-neighbor vertically.
		 * Pass 3 clears the backbuffer black and draws the scanlined RT into the destination rectangle.
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
