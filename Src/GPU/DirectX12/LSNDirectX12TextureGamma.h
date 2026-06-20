#ifdef LSN_DX12

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
	 * Class CDirectX12TextureGamma
	 * \brief A generic helper class for applying a gamma curve to a texture.
	 *
	 * Description: Performs a 1:1 texture fetch and applies a specified gamma curve, outputting to a render target.
	 */
	class CDirectX12TextureGamma {
	public :
		CDirectX12TextureGamma();
		~CDirectX12TextureGamma();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target, applying the selected gamma curve.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to execute the draw.
		 * \param _prSrc The source texture to draw.
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _cdhRtv The CPU descriptor handle pointing to the destination render target view.
		 * \param _gGamma The gamma curve to apply.
		 * \param _fTargetFormat The format of the RTV this shader will output to.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, D3D12_CPU_DESCRIPTOR_HANDLE _cdhRtv, CNesPalette::LSN_GAMMA _gGamma, DXGI_FORMAT _fTargetFormat, ID3DInclude * _piInclude = nullptr );


	protected :
		// == Members.
		/** Dynamic screen-space quad vertex buffer (XYZRHW|TEX1, 4 vertices). */
		std::unique_ptr<CDirectX12Resource>						m_prVbQuad;
		/** Vertex buffer view. */
		D3D12_VERTEX_BUFFER_VIEW								m_vbView;
		
		/** Descriptor Heap for the Source SRV. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSrvHeap;
		/** Descriptor Heap for the Point Sampler. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSamplerHeap;

		/** The pixel shader for gamma correction. */
		std::unique_ptr<CDirectX12PipelineState>				m_ppsShader;
		/** The root signature. */
		std::unique_ptr<CDirectX12RootSignature>				m_prsRootSignature;

		/** The current gamma setting used to compile the shader. */
		CNesPalette::LSN_GAMMA									m_gShaderGamma = CNesPalette::LSN_G_NONE;
		/** The format of the generated render target texture. */
		DXGI_FORMAT												m_fFormat = DXGI_FORMAT_UNKNOWN;

		// == Functions.
		/**
		 * Ensures the vertex buffer and descriptor heaps are created.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX12Device * _pd12dDevice );

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
