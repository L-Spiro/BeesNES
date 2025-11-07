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
		void												SetVertSharpness( uint32_t _ui32Factor ) { m_ui32VertSharpness = _ui32Factor; }

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
		void												SetHorSharpness( uint32_t _ui32Factor ) { m_ui32HorSharness = _ui32Factor; }

		/**
		 * \brief Gets the current horizontal sharpness factor.
		 * 
		 * \return Returns the horizontal sharpness factor.
		 */
		inline uint32_t										GetHorSharpness() const { return m_ui32HorSharness; }

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
		/** Source width in pixels. */
		uint32_t											m_ui32SrcW = 0;
		/** Source height in pixels. */
		uint32_t											m_ui32SrcH = 0;
		/** Vertical sharpness factor. */
		uint32_t											m_ui32VertSharpness = 2;
		/** Horizontal sharpness factor. */
		uint32_t											m_ui32HorSharness = 2;


		// == Functions.
		/**
		 * \brief Updates the 512-entry float RGBA LUT.
		 *
		 * The LUT is a 512Å~1 A32B32G32R32F MANAGED texture. Each entry is RGBA in linear space.
		 *
		 * \return Returns true on success.
		 */
		bool												UpdateLut();

	private :
		typedef CDx9FilterBase								CParent;
	};

}	// namespace lsn


#endif	// #ifdef LSN_DX9
