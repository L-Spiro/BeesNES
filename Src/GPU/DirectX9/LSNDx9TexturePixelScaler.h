#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying integer nearest-neighbor scaling and gamma to a texture.
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
	 * Class CDx9TexturePixelScaler
	 * \brief A generic helper class for applying integer nearest-neighbor scaling and gamma to a texture.
	 *
	 * Description: Scales a texture by integer factors using nearest-neighbor sampling. Can also apply a specified gamma curve to the output.
	 */
	class CDx9TexturePixelScaler {
	public :
		CDx9TexturePixelScaler();
		~CDx9TexturePixelScaler();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the internal scaled target.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ptSrc The source texture to draw.
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _ui32ScaleX The horizontal scaling factor.
		 * \param _ui32ScaleY The vertical scaling factor.
		 * \param _gGamma The gamma curve to apply.
		 * \param _bUse16BitTarget If true, a 16-bit target is used, otherwise a 32-bit target is used.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32ScaleX, uint32_t _ui32ScaleY, CNesPalette::LSN_GAMMA _gGamma, bool _bUse16BitTarget );

		/**
		 * Gets the scaled output texture.
		 * 
		 * \return Returns a pointer to the generated render target texture.
		 **/
		inline CDirectX9Texture *								GetTexture() { return m_rtTarget ? m_rtTarget->Texture() : nullptr; }

		/**
		 * Gets the width of the scaled output texture.
		 * 
		 * \return Returns the width of the generated render target texture.
		 **/
		inline uint32_t											GetWidth() const { return m_ui32TargetW; }

		/**
		 * Gets the height of the scaled output texture.
		 * 
		 * \return Returns the height of the generated render target texture.
		 **/
		inline uint32_t											GetHeight() const { return m_ui32TargetH; }


	protected :
		// == Members.
		/** The scaled floating-point render target. */
		std::unique_ptr<CDirectX9RenderTarget>					m_rtTarget;
		/** Dynamic screen-space quad vertex buffer (XYZRHW|TEX1, 4 vertices). */
		std::unique_ptr<CDirectX9VertexBuffer>					m_vbQuad;
		/** The pixel shader for scaling and gamma. */
		std::unique_ptr<CDirectX9PixelShader>					m_psShader;
		/** The current gamma setting used to compile the shader. */
		CNesPalette::LSN_GAMMA									m_gShaderGamma = CNesPalette::LSN_G_NONE;
		/** The width of the generated render target texture. */
		uint32_t												m_ui32TargetW = 0;
		/** The height of the generated render target texture. */
		uint32_t												m_ui32TargetH = 0;
		/** The format of the generated render target texture. */
		D3DFORMAT												m_fFormat = D3DFMT_UNKNOWN;


		// == Functions.
		/**
		 * Ensures the vertex buffer and render target are created and properly sized.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ui32DstW The target width.
		 * \param _ui32DstH The target height.
		 * \param _fFormat The target format.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32DstW, uint32_t _ui32DstH, D3DFORMAT _fFormat );

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
