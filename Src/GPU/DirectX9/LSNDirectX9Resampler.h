#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 hardware wrapper for 2-pass GPU resampling.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Filters/LSNResamplerBase.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9PixelShader.h"
#include "LSNDirectX9RenderTarget.h"
#include "LSNDirectX9Texture.h"
#include "LSNDirectX9VertexBuffer.h"

#include <Helpers/LSWHelpers.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CDirectX9Resampler
	 * \brief A Direct3D 9 hardware wrapper for 2-pass GPU resampling.
	 *
	 * Description: Uses CResamplerBase to generate kernel weights and source offsets, uploads them to 
	 * look-up textures, and executes a 2-pass shader operation to resample an image.
	 */
	class CDirectX9Resampler : public CResamplerBase {
	public :
		CDirectX9Resampler();
		virtual ~CDirectX9Resampler();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target surface utilizing a 2-pass resampling algorithm.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ptSrc The source texture to draw.
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _prtDst The destination render target.
		 * \param _ui32DstW The target width.
		 * \param _ui32DstH The target height.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, CDirectX9RenderTarget * _prtDst, uint32_t _ui32DstW, uint32_t _ui32DstH );

		/**
		 * Sets the current filter to use.
		 * 
		 * \param _ffFilter The filter to use.
		 **/
		inline void												SetFilter( LSN_FILTER_FUNCS _ffFilter ) {
			 if ( _ffFilter != m_ffFilter ) {
				m_ui32LastSrcW = m_ui32LastSrcH = 0;
				m_ffFilter = _ffFilter;
			 }
		}

		/**
		 * Gets the current filter.
		 * 
		 * \return Returns the current filter.
		 **/
		inline LSN_FILTER_FUNCS									GetFilter() const { return m_ffFilter; }


	protected :
		// == Members.
		/** The intermediate floating-point render target for pass 1. */
		std::unique_ptr<CDirectX9RenderTarget>					m_rtIntermediate;
		/** The X-axis weight and offset look-up texture. */
		std::unique_ptr<CDirectX9Texture>						m_ptLutX;
		/** The Y-axis weight and offset look-up texture. */
		std::unique_ptr<CDirectX9Texture>						m_ptLutY;
		/** Dynamic screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX9VertexBuffer>					m_pvbQuad;
		/** The pixel shader for the horizontal pass. */
		std::unique_ptr<CDirectX9PixelShader>					m_psResampleX;
		/** The pixel shader for the vertical pass. */
		std::unique_ptr<CDirectX9PixelShader>					m_psResampleY;

		/** The filter to use. */
		LSN_FILTER_FUNCS										m_ffFilter = LSN_FF_CARDINALSPLINEUNIFORM;
		
		/** Cached dimensions to prevent unneeded recalculations. */
		uint32_t												m_ui32LastSrcW = 0;
		uint32_t												m_ui32LastSrcH = 0;
		uint32_t												m_ui32LastDstW = 0;
		uint32_t												m_ui32LastDstH = 0;
		uint32_t												m_ui32MaxTapsX = 0;
		uint32_t												m_ui32MaxTapsY = 0;


		// == Functions.
		/**
		 * Ensures the vertex buffer, render targets, LUTs, and shaders are correctly built.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ui32SrcW The source width.
		 * \param _ui32SrcH The source height.
		 * \param _ui32DstW The target width.
		 * \param _ui32DstH The target height.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32DstW, uint32_t _ui32DstH );

		/**
		 * Builds a 1-D Look-Up Texture containing weights and source indices.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ui32SrcSize The size of the source dimension.
		 * \param _ui32DstSize The size of the destination dimension.
		 * \param _ptLut The unique pointer holding the texture to be created.
		 * \param _ui32OutMaxTaps Reference to store the maximum kernel size determined during generation.
		 * \return Returns true if the LUT was successfully built.
		 **/
		bool													BuildLUT( CDirectX9Device * _pdx9dDevice, uint32_t _ui32SrcSize, uint32_t _ui32DstSize, std::unique_ptr<CDirectX9Texture> &_ptLut, uint32_t &_ui32OutMaxTaps );

		/**
		 * Renders a full-size quad to the currently bound render target.
		 * 
		 * \param _pd3d9dDevice The raw IDirect3DDevice9 pointer.
		 * \param _ui32W The width of the viewport.
		 * \param _ui32H The height of the viewport.
		 * \return Returns true on success.
		 **/
		bool													DrawQuad( IDirect3DDevice9 * _pd3d9dDevice, uint32_t _ui32W, uint32_t _ui32H );

		/**
		 * Compiles an HLSL pixel shader using dynamically loaded D3DX.
		 * 
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name.
		 * \param _pcszProfile Null-terminated profile.
		 * \param _vOutByteCode Output vector to receive the compiled bytecode.
		 * \return Returns true if compilation succeeded.
		 **/
		bool													CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode );

	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
