#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for rendering a texture to a surface with bilinear sampling and gamma correction.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9PixelShader.h"
#include "LSNDirectX9VertexBuffer.h"

#include <Helpers/LSWHelpers.h>
#include <memory>

namespace lsn {

	/**
	 * Class CDx9TextureRenderer
	 * \brief A generic helper class for rendering a texture to a surface.
	 *
	 * Description: Accepts an input texture and an output render target and simply renders the result from the input to the output using bilinear sampling and a basic copy/gamma shader.
	 */
	class CDx9TextureRenderer {
	public :
		CDx9TextureRenderer();
		~CDx9TextureRenderer();


		// == Functions.
		/**
		 * Resets the vertex buffer, shaders, and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target surface.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ptSrc The source texture to draw.
		 * \param _psDst The destination surface (e.g., the swap chain backbuffer).
		 * \param _rOutput The destination rectangle in client pixels.
		 * \param _fGamma The PC monitor's gamma parameter.
		 * \param _bClear If true, clears the destination surface to black before rendering.
		 * \param _bSrgb Enable or disable hardware sRGB conversion.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, IDirect3DSurface9 * _psDst, const lsw::LSW_RECT &_rOutput, float _fGamma, bool _bClear = true, bool _bSrgb = false );


	protected :
		// == Members.
		/** The dynamic screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX9VertexBuffer>					m_pvbQuad;
		/** The copy/gamma pixel shader. */
		std::unique_ptr<CDirectX9PixelShader>					m_ppsCopy;

		// == Functions.
		/**
		 * Ensures the vertex buffer and pixel shaders are created.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX9Device * _pdx9dDevice );

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
