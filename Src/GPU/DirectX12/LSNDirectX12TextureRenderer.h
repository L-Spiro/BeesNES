#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for rendering a texture to a surface with bilinear sampling and gamma correction.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
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
	 * Class CDirectX12TextureRenderer
	 * \brief A generic helper class for rendering a texture to a surface.
	 *
	 * Description: Accepts an input texture and an output render target and simply renders the result from the input to the output using bilinear sampling and a basic copy/gamma shader.
	 */
	class CDirectX12TextureRenderer {
	public :
		CDirectX12TextureRenderer();
		~CDirectX12TextureRenderer();


		// == Functions.
		/**
		 * Resets the vertex buffer, shaders, and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target surface.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to execute the draw.
		 * \param _prSrc The source texture to draw.
		 * \param _prDst The destination surface resource (e.g., the swap chain backbuffer). Used for viewport dimension mapping.
		 * \param _cdhRtv The CPU descriptor handle pointing to the destination render target view.
		 * \param _rOutput The destination rectangle in client pixels.
		 * \param _fGamma The PC monitor's gamma parameter.
		 * \param _bClear If true, clears the destination surface to black before rendering.
		 * \param _bSrgb Set to true if the destination RTV was created with an _SRGB format.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, CDirectX12Resource * _prDst, D3D12_CPU_DESCRIPTOR_HANDLE _cdhRtv, const lsw::LSW_RECT &_rOutput, float _fGamma, bool _bClear = true, bool _bSrgb = false, ID3DInclude * _piInclude = nullptr );


	protected :
		// == Members.
		/** The dynamic screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX12Resource>						m_prVbQuad;
		/** Vertex buffer view. */
		D3D12_VERTEX_BUFFER_VIEW								m_vbView;
		/** The copy/gamma pixel shader. */
		std::unique_ptr<CDirectX12PipelineState>				m_ppsCopy;
		/** The root signature for the renderer. */
		std::unique_ptr<CDirectX12RootSignature>				m_prsRootSignature;
		
		/** Descriptor Heap for the Source SRV. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSrvHeap;
		/** Descriptor Heap for the Linear Sampler. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSamplerHeap;

		/** The current target format used to compile the PSO. */
		DXGI_FORMAT												m_fPsoFormat = DXGI_FORMAT_UNKNOWN;

		// == Functions.
		/**
		 * Ensures the vertex buffer and descriptor heaps are created.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX12Device * _pd12dDevice );

		/**
		 * Ensures the Pipeline State Object and Root Signature are created for the specified format.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _fTargetFormat The format of the RTV this shader will output to.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true if the shaders are ready.
		 **/
		bool													EnsureShaders( CDirectX12Device * _pd12dDevice, DXGI_FORMAT _fTargetFormat, ID3DInclude * _piInclude );

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
