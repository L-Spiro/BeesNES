#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 hardware wrapper for a phosphor-decay post-processing effect.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"
#include "LSNDirectX12DescriptorHeap.h"
#include "LSNDirectX12GraphicsCommandList.h"
#include "LSNDirectX12PipelineState.h"
#include "LSNDirectX12Resource.h"
#include "LSNDirectX12RootSignature.h"

#include <Helpers/LSWHelpers.h>
#include <d3dcompiler.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CDirectX12Phosphor
	 * \brief A Direct3D 12 hardware wrapper for a phosphor-decay post-processing effect.
	 *
	 * Description: Uses an internal ping-pong buffer to track phosphor decay over time, applying independent
	 * decay rates to the red, green, and blue channels, along with an initial decay scalar.
	 */
	class CDirectX12Phosphor {
	public :
		CDirectX12Phosphor();
		virtual ~CDirectX12Phosphor();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target surface, updating the internal phosphor decay buffer.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to execute the draw.
		 * \param _prSrc The source texture to draw (the current frame).
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _p12rDst The destination render target resource.
		 * \param _cdhRtv The CPU descriptor handle pointing to the destination render target view.
		 * \param _fTargetFormat The format of the RTV this shader will output to.
		 * \param _fInitDecay The initial decay factor applied to the current frame when writing to the buffer.
		 * \param _fDecayR The decay factor for the red channel.
		 * \param _fDecayG The decay factor for the green channel.
		 * \param _fDecayB The decay factor for the blue channel.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, ID3D12Resource * _p12rDst, D3D12_CPU_DESCRIPTOR_HANDLE _cdhRtv, DXGI_FORMAT _fTargetFormat, float _fInitDecay, float _fDecayR, float _fDecayG, float _fDecayB, ID3DInclude * _piInclude = nullptr );


	protected :
		// == Members.
		/** Ping-pong floating-point render targets for tracking phosphor state. */
		std::unique_ptr<CDirectX12Resource>						m_prPhosphor[2];

		/** Dynamic screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX12Resource>						m_prVbQuad;
		/** Vertex buffer view. */
		D3D12_VERTEX_BUFFER_VIEW								m_vbView;

		/** Descriptor Heap for the SRVs (Capacity: 3 -> Src, Phosphor[0], Phosphor[1]). */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSrvHeap;
		/** Descriptor Heap for the RTVs (Capacity: 2 -> Phosphor[0], Phosphor[1]). */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhRtvHeap;
		/** Descriptor Heap for the Sampler. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSamplerHeap;

		/** The root signature. */
		std::unique_ptr<CDirectX12RootSignature>				m_prsRootSignature;
		
		/** The pixel shader for the visible composite output to the screen. */
		std::unique_ptr<CDirectX12PipelineState>				m_ppsPhosphorOutput;
		/** The pixel shader for updating the internal phosphor history buffer. */
		std::unique_ptr<CDirectX12PipelineState>				m_ppsPhosphorUpdate;

		/** The index of the buffer currently being read from (0 or 1). */
		size_t													m_stReadIndex = 0;

		/** Cached states to prevent unneeded recalculations. */
		uint32_t												m_ui32LastSrcW = 0;
		uint32_t												m_ui32LastSrcH = 0;
		DXGI_FORMAT												m_fLastTargetFormat = DXGI_FORMAT_UNKNOWN;


		// == Functions.
		/**
		 * Ensures the vertex buffer and ping-pong render targets are correctly built and cleared.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to clear the resources on creation.
		 * \param _ui32SrcW The source width.
		 * \param _ui32SrcH The source height.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH );

		/**
		 * Ensures the Pipeline State Objects and Root Signature are created for the specified format.
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
