#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 hardware wrapper for a phosphor-decay post-processing effect.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
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
	 * Class CDirectX9Phosphor
	 * \brief A Direct3D 9 hardware wrapper for a phosphor-decay post-processing effect.
	 *
	 * Description: Uses an internal ping-pong buffer to track phosphor decay over time, applying independent
	 * decay rates to the red, green, and blue channels, along with an initial decay scalar.
	 */
	class CDirectX9Phosphor {
	public :
		CDirectX9Phosphor();
		virtual ~CDirectX9Phosphor();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target surface, updating the internal phosphor decay buffer.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ptSrc The source texture to draw (the current frame).
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _prtDst The destination render target for the visible output.
		 * \param _fInitDecay The initial decay factor applied to the current frame when writing to the buffer.
		 * \param _fDecayR The decay factor for the red channel.
		 * \param _fDecayG The decay factor for the green channel.
		 * \param _fDecayB The decay factor for the blue channel.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, CDirectX9RenderTarget * _prtDst, float _fInitDecay, float _fDecayR, float _fDecayG, float _fDecayB );


	protected :
		// == Members.
		/** Ping-pong floating-point render targets for tracking phosphor state. */
		std::unique_ptr<CDirectX9RenderTarget>					m_rtPhosphor[2];
		
		/** Dynamic screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX9VertexBuffer>					m_pvbQuad;
		
		/** The pixel shader for the visible composite output to the screen. */
		std::unique_ptr<CDirectX9PixelShader>					m_psPhosphorOutput;
		/** The pixel shader for updating the internal phosphor history buffer. */
		std::unique_ptr<CDirectX9PixelShader>					m_psPhosphorUpdate;
		
		/** The index of the buffer currently being read from (0 or 1). */
		size_t													m_stReadIndex = 0;

		/** Cached dimensions to prevent unneeded recalculations. */
		uint32_t												m_ui32LastSrcW = 0;
		uint32_t												m_ui32LastSrcH = 0;


		// == Functions.
		/**
		 * Ensures the vertex buffer, render targets, and shaders are correctly built.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _ui32SrcW The source width.
		 * \param _ui32SrcH The source height.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32SrcW, uint32_t _ui32SrcH );

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
