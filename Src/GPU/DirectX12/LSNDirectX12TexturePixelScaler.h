#ifdef LSN_DX12

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
#include "LSNDirectX12Device.h"
#include "LSNDirectX12Resource.h"
#include "LSNDirectX12PipelineState.h"
#include "LSNDirectX12RootSignature.h"
#include "LSNDirectX12DescriptorHeap.h"
#include "LSNDirectX12GraphicsCommandList.h"

#include <Helpers/LSWHelpers.h>
#include <d3dcompiler.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CDirectX12TexturePixelScaler
	 * \brief A generic helper class for applying integer nearest-neighbor scaling and gamma to a texture.
	 *
	 * Description: Scales a texture by integer factors using nearest-neighbor sampling. Can also apply a specified gamma curve to the output.
	 */
	class CDirectX12TexturePixelScaler {
	public :
		CDirectX12TexturePixelScaler();
		~CDirectX12TexturePixelScaler();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the internal scaled target.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to execute the draw.
		 * \param _prSrc The source texture to draw.
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _ui32ScaleX The horizontal scaling factor.
		 * \param _ui32ScaleY The vertical scaling factor.
		 * \param _gGamma The gamma curve to apply.
		 * \param _bUse16BitTarget If true, a 16-bit target is used, otherwise a 32-bit target is used.
		 * \param _bFlipY If true, the rendered image is flipped vertically.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32ScaleX, uint32_t _ui32ScaleY, CNesPalette::LSN_GAMMA _gGamma, bool _bUse16BitTarget, bool _bFlipY = false, ID3DInclude * _piInclude = nullptr );

		/**
		 * Gets the scaled output texture.
		 * 
		 * \return Returns a pointer to the generated render target resource.
		 **/
		inline CDirectX12Resource *								GetTexture() { return m_prTarget.get(); }

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
		std::unique_ptr<CDirectX12Resource>						m_prTarget;
		/** Dynamic screen-space quad vertex buffer (XYZRHW|TEX1, 4 vertices). */
		std::unique_ptr<CDirectX12Resource>						m_prVbQuad;
		/** Vertex buffer view. */
		D3D12_VERTEX_BUFFER_VIEW								m_vbView;
		
		/** Descriptor Heap for the Source SRV. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSrvHeap;
		/** Descriptor Heap for the internal RTV. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhRtvHeap;
		/** Descriptor Heap for the Point Sampler. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSamplerHeap;

		/** The pixel shader for scaling and gamma. */
		std::unique_ptr<CDirectX12PipelineState>				m_ppsShader;
		/** The root signature for the scaler. */
		std::unique_ptr<CDirectX12RootSignature>				m_prsRootSignature;

		/** The current gamma setting used to compile the shader. */
		CNesPalette::LSN_GAMMA									m_gShaderGamma = CNesPalette::LSN_G_NONE;
		/** The width of the generated render target texture. */
		uint32_t												m_ui32TargetW = 0;
		/** The height of the generated render target texture. */
		uint32_t												m_ui32TargetH = 0;
		/** The format of the generated render target texture. */
		DXGI_FORMAT												m_fFormat = DXGI_FORMAT_UNKNOWN;

		// == Functions.
		/**
		 * Ensures the vertex buffer, descriptor heaps, and render target are created and properly sized.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _ui32DstW The target width.
		 * \param _ui32DstH The target height.
		 * \param _fFormat The target format.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX12Device * _pd12dDevice, uint32_t _ui32DstW, uint32_t _ui32DstH, DXGI_FORMAT _fFormat );

		/**
		 * Ensures the Pipeline State Object is compiled with the correct gamma function.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _gGamma The gamma curve to apply.
		 * \param _fFormat The target format for the pipeline.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true if the shader is ready.
		 **/
		bool													EnsureShader( CDirectX12Device * _pd12dDevice, CNesPalette::LSN_GAMMA _gGamma, DXGI_FORMAT _fFormat, ID3DInclude * _piInclude );

		/**
		 * Compiles an HLSL pixel shader using dynamically loaded d3dcompiler_47.dll.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name.
		 * \param _pcszProfile Null-terminated profile.
		 * \param _vOutByteCode Output vector to receive the compiled bytecode.
		 * \param _piInclude Optional #include handler.
		 * \return Returns true if compilation succeeded.
		 **/
		bool													CompileHlsl( CDirectX12Device * _pd12dDevice, const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<uint8_t> &_vOutByteCode, ID3DInclude * _piInclude );

	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
