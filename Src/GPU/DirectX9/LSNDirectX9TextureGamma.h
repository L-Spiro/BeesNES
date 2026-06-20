#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying a gamma curve to a texture.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Filters/LSNNesPalette.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9PixelShader.h"
#include "LSNDirectX9RenderTarget.h"
#include "LSNDirectX9VertexBuffer.h"

#include <Helpers/LSWHelpers.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CDirectX9TextureGamma
	 * \brief A generic helper class for applying a gamma curve to a texture.
	 *
	 * Description: Performs a 1:1 texture fetch and applies a specified gamma curve, outputting to a render target.
	 */
	class CDirectX9TextureGamma {
	public :
		CDirectX9TextureGamma();
		~CDirectX9TextureGamma();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target, applying the selected gamma curve.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ptSrc The source texture to draw.
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _prtDst The destination render target.
		 * \param _gGamma The gamma curve to apply.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, CDirectX9RenderTarget * _prtDst, CNesPalette::LSN_GAMMA _gGamma );


	protected :
		// == Members.
		/** Dynamic screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX9VertexBuffer>					m_vbQuad;
		/** The pixel shader for gamma correction. */
		std::unique_ptr<CDirectX9PixelShader>					m_psShader;
		/** The current gamma setting used to compile the shader. */
		CNesPalette::LSN_GAMMA									m_gShaderGamma = CNesPalette::LSN_G_NONE;


		// == Functions.
		/**
		 * Ensures the vertex buffer is created.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX9Device * _pdx9dDevice );

		/**
		 * Ensures the pixel shader is compiled with the correct gamma function.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _gGamma The gamma curve to apply.
		 * \return Returns true if the shader is ready.
		 **/
		bool													EnsureShader( CDirectX9Device * _pdx9dDevice, CNesPalette::LSN_GAMMA _gGamma );

		/**
		 * Compiles an HLSL pixel shader using dynamically loaded D3DX.
		 * 
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name.
		 * \param _pcszProfile Null-terminated profile.
		 * \param _vOutByteCode Output vector to receive the compiled bytecode.
		 * \param _piInclude Optional #include handler.
		 * \return Returns true if compilation succeeded.
		 **/
		bool													CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode, ID3DXInclude * _piInclude );

	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
