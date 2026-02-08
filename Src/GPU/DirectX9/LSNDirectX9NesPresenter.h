#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Three-pass NES presenter (IndicesÅ®FP, FPÅ®Scanlined, ScanlinedÅ®Backbuffer).
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9PixelShader.h"
#include "LSNDirectX9RenderTarget.h"
#include "LSNDirectX9Texture.h"
#include "LSNDirectX9VertexBuffer.h"

#include <Helpers/LSWHelpers.h>

#include <vector>

namespace lsn {

	/**
	 * Class CDirectX9NesPresenter
	 * \brief Three-pass NES presenter (IndicesÅ®FP, FPÅ®Scanlined, ScanlinedÅ®Backbuffer).
	 *
	 * Description: Uploads a 16-bit index texture (9-bit effective values 0..511), performs a pixel-shader lookup
	 * through a 512Å~1 float RGBA LUT into a same-sized floating-point render target, expands it vertically to a
	 * ÅgscanlinedÅh texture using nearest-neighbor by a factor of 2 or 3, then draws that scanlined texture to the
	 * backbuffer inside the provided output rectangle with black borders elsewhere.
	 *
	 * Notes:
	 *  - Ownership/creation use DX9 wrappers exclusively; runtime binding uses wrapped interfaces via .Get().
	 *  - Pixel shaders are authored in HLSL and compiled at runtime by D3DX (loaded dynamically).
	 */
	class CDirectX9NesPresenter {
	public :
		CDirectX9NesPresenter( CDirectX9Device * _pdx9dDevice );
		~CDirectX9NesPresenter();


		// == Functions.
		/**
		 * \brief Initializes (or re-initializes) resources for a given PPU image size.
		 *
		 * Creates:
		 *  - Index texture (L16, DEFAULT|DYNAMIC)
		 *  - Initial FP render target (A16B16G16R16F or A32B32G32R32F)
		 *  - Scanlined FP render target (height = source height Å~ scanline factor)
		 *  - Screen-space quad vertex buffer
		 *  - Pixel shaders (HLSL Å® bytecode)
		 *
		 * \param _ui32SrcW The source (PPU) width in pixels.
		 * \param _ui32SrcH The source (PPU) height in pixels.
		 * \param _bUse16f If true, uses A16B16G16R16F for FP targets; otherwise A32B32G32R32F.
		 * \return Returns true if initialized successfully; false if resource/shader creation failed.
		 */
		bool											Init( uint32_t _ui32SrcW, uint32_t _ui32SrcH, bool _bUse16f = true );

		/**
		 * \brief Updates the vertical sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The vertical sharpness.
		 */
		void											SetVertSharpness( uint32_t _ui32Factor ) { m_ui32VertSharpness = _ui32Factor; }

		/**
		 * \brief Gets the current vertical sharpness factor.
		 * 
		 * \return Returns the vertical sharpness factor.
		 */
		inline uint32_t									GetVertSharpness() const { return m_ui32VertSharpness; }

		/**
		 * \brief Updates the horizontal sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The horizontal sharpness.
		 */
		void											SetHorSharpness( uint32_t _ui32Factor ) { m_ui32HorSharness = _ui32Factor; }

		/**
		 * \brief Gets the current horizontal sharpness factor.
		 * 
		 * \return Returns the horizontal sharpness factor.
		 */
		inline uint32_t									GetHorSharpness() const { return m_ui32HorSharness; }

		/**
		 * \brief Updates the 512-entry float RGBA LUT.
		 *
		 * The LUT is a 512Å~1 A32B32G32R32F MANAGED texture. Each entry is RGBA in linear space.
		 *
		 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
		 * \return Returns true on success.
		 */
		bool											UpdateLut( const float * _pfRgba512 );

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
		bool											UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch = 0 );

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
		bool											Render( const lsw::LSW_RECT &_rOutput );


	private :
		// == Members.
		/** The DirectX 9 device wrapper (non-owning). */
		CDirectX9Device *								m_pdx9dDevice = nullptr;
		/** Index texture (L16, DEFAULT|DYNAMIC). */
		CDirectX9Texture								m_tIndex;
		/** 512Å~1 LUT texture (A32B32G32R32F, MANAGED). */
		CDirectX9Texture								m_tLut;
		/** Initial floating-point render target (same size as indices). */
		CDirectX9RenderTarget							m_rtInitial;
		/** Scanlined floating-point render target (height = source height Å~ factor). */
		CDirectX9RenderTarget							m_rtScanlined;
		/** Dynamic screen-space quad vertex buffer (XYZRHW|TEX1, 4 vertices). */
		CDirectX9VertexBuffer							m_vbQuad;
		/** Pixel shader: indices + LUT Å® RGBA (pass 1). */
		CDirectX9PixelShader							m_psIdxToColor;
		/** Pixel shader: vertical nearest-neighbor upscale (pass 2). */
		CDirectX9PixelShader							m_psVerticalNN;
		/** Pixel shader: copy pass (pass 3). */
		CDirectX9PixelShader							m_psCopy;
		/** Source width in pixels. */
		uint32_t										m_ui32SrcW = 0;
		/** Source height in pixels. */
		uint32_t										m_ui32SrcH = 0;
		/** True to use A16B16G16R16F for FP RTs; false for A32B32G32R32F. */
		bool											m_bUse16f = true;
		/** Vertical sharpness factor. */
		uint32_t										m_ui32VertSharpness = 2;
		/** Horizontal sharpness factor. */
		uint32_t										m_ui32HorSharness = 2;


		// == Functions.
		/**
		 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
		 *
		 * Releases/creates the index texture, both FP render targets, and quad vertex buffer as needed.
		 *
		 * \param _ui32W New width in pixels.
		 * \param _ui32H New height in pixels.
		 * \param _bUse16f True to use A16B16G16R16F RTs; false for A32B32G32R32F.
		 * \return Returns true on success.
		 */
		bool											EnsureSizeAndResources( uint32_t _ui32W, uint32_t _ui32H, bool _bUse16f );

		/**
		 * \brief Ensures pixel shaders (indexÅ®color, vertical NN, copy) are created.
		 *
		 * Compiles the HLSL entry points with D3DX at runtime and creates pixel shaders from bytecode.
		 * If D3DX cannot be loaded, this function returns false.
		 *
		 * \return Returns true if all shaders are ready.
		 */
		bool											EnsureShaders();

		/**
		 * \brief Compiles an HLSL pixel shader using dynamically loaded D3DX.
		 *
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name (e.g., "main").
		 * \param _pcszProfile Null-terminated profile (e.g., "ps_2_0").
		 * \param _vOutByteCode Output vector to receive the compiled bytecode (DWORD stream).
		 * \return Returns true if compilation succeeded and bytecode was produced.
		 */
		bool											CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode );

		/**
		 * \brief Releases size-dependent resources (index texture, FP RTs, quad VB).
		 */
		void											ReleaseSizeDependents();

		/**
		 * \brief Fills the screen-space quad vertex buffer with an XYZRHW|TEX1 quad.
		 *
		 * Applies a -0.5f XY bias to align texel centers with pixel centers in D3D9 when using XYZRHW.
		 *
		 * \param _fL Left X in pixels.
		 * \param _fT Top Y in pixels.
		 * \param _fR Right X in pixels.
		 * \param _fB Bottom Y in pixels.
		 * \param _fU0 Left U coordinate.
		 * \param _fV0 Top V coordinate.
		 * \param _fU1 Right U coordinate.
		 * \param _fV1 Bottom V coordinate.
		 * \return Returns true on success.
		 */
		bool											FillQuad( float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 );

		/**
		 * \brief Computes half-texel-correct UVs for a WÅ~H texture in D3D9.
		 *
		 * Centers sampling on texels by offsetting UV edges by 0.5/W and 0.5/H.
		 * Required when using POINT sampling with pre-transformed XYZRHW quads,
		 * otherwise you'll hit the gaps between texels (black columns/rows).
		 *
		 * \param _uiW Texture width in texels.
		 * \param _uiH Texture height in texels.
		 * \param _fU0 Receives the left U (with half-texel offset applied).
		 * \param _fV0 Receives the top V (with half-texel offset applied).
		 * \param _fU1 Receives the right U (with half-texel offset applied).
		 * \param _fV1 Receives the bottom V (with half-texel offset applied).
		 */
		static inline void								HalfTexelUv( uint32_t _uiW, uint32_t _uiH,
			float &_fU0, float &_fV0, float &_fU1, float &_fV1 ) {
			const float fInvW = 1.0f / static_cast<float>(_uiW);
			const float fInvH = 1.0f / static_cast<float>(_uiH);
			_fU0 = 0.5f * fInvW;
			_fV0 = 0.5f * fInvH;
			_fU1 = 1.0f - _fU0;
			_fV1 = 1.0f - _fV0;
		}

		
	};

} // namespace lsn

#endif // LSN_DX9
