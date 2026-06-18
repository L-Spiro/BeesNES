#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 hardware wrapper for 2-pass GPU resampling.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Filters/LSNResamplerBase.h"
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
	 * Class CDirectX12Resampler
	 * \brief A Direct3D 12 hardware wrapper for 2-pass GPU resampling.
	 *
	 * Description: Uses CResamplerBase to generate kernel weights and source offsets, uploads them to 
	 * look-up textures, and executes a 2-pass compute-like pixel shader operation utilizing raw texel fetches.
	 */
	class CDirectX12Resampler : public CResamplerBase {
	public :
		CDirectX12Resampler();
		virtual ~CDirectX12Resampler();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Renders the input texture to the target surface utilizing a 2-pass resampling algorithm.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to execute the draw.
		 * \param _prSrc The source texture to draw.
		 * \param _ui32SrcW The width of the source texture.
		 * \param _ui32SrcH The height of the source texture.
		 * \param _p12rDst The destination render target resource.
		 * \param _cdhRtv The CPU descriptor handle pointing to the destination render target view.
		 * \param _ui32DstW The target width.
		 * \param _ui32DstH The target height.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true on success.
		 **/
		bool													Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, ID3D12Resource * _p12rDst, D3D12_CPU_DESCRIPTOR_HANDLE _cdhRtv, uint32_t _ui32DstW, uint32_t _ui32DstH, ID3DInclude * _piInclude = nullptr );

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
		std::unique_ptr<CDirectX12Resource>						m_prIntermediate;
		
		/** The X-axis weight and offset look-up texture. */
		std::unique_ptr<CDirectX12Resource>						m_prLutX;
		/** The upload buffer for the X-axis LUT. */
		std::unique_ptr<CDirectX12Resource>						m_prUploadLutX;
		
		/** The Y-axis weight and offset look-up texture. */
		std::unique_ptr<CDirectX12Resource>						m_prLutY;
		/** The upload buffer for the Y-axis LUT. */
		std::unique_ptr<CDirectX12Resource>						m_prUploadLutY;

		/** Dynamic screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX12Resource>						m_prVbQuad;
		/** Vertex buffer view. */
		D3D12_VERTEX_BUFFER_VIEW								m_vbView;

		/** Descriptor Heap for the SRVs (Capacity: 4 -> Src, LutX, Intermediate, LutY). */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhSrvHeap;
		/** Descriptor Heap for the internal RTV. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhRtvHeap;

		/** The root signature for both resampling passes. */
		std::unique_ptr<CDirectX12RootSignature>				m_prsRootSignature;
		/** The pixel shader for the horizontal pass. */
		std::unique_ptr<CDirectX12PipelineState>				m_ppsResampleX;
		/** The pixel shader for the vertical pass. */
		std::unique_ptr<CDirectX12PipelineState>				m_ppsResampleY;

		/** The filter to use. */
		LSN_FILTER_FUNCS										m_ffFilter = LSN_FF_CARDINALSPLINEUNIFORM;
		
		/** Cached dimensions and states to prevent unneeded recalculations. */
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
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to record upload and transition barriers.
		 * \param _ui32SrcW The source width.
		 * \param _ui32SrcH The source height.
		 * \param _ui32DstW The target width.
		 * \param _ui32DstH The target height.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32DstW, uint32_t _ui32DstH, ID3DInclude * _piInclude );

		/**
		 * Ensures the Pipeline State Objects and Root Signature are created.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _piInclude Optional #include handler for shader compilation.
		 * \return Returns true if the shaders are ready.
		 **/
		bool													EnsureShaders( CDirectX12Device * _pd12dDevice, ID3DInclude * _piInclude );

		/**
		 * Builds a 1-D Look-Up Texture containing weights and raw source integer indices.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to record the upload copy.
		 * \param _ui32SrcSize The size of the source dimension.
		 * \param _ui32DstSize The size of the destination dimension.
		 * \param _prLut The unique pointer holding the texture to be created in the DEFAULT heap.
		 * \param _prUpload The unique pointer holding the staging texture in the UPLOAD heap.
		 * \param _ui32SrvOffset The index in the SRV heap where this texture should be mapped.
		 * \param _ui32OutMaxTaps Reference to store the maximum kernel size determined during generation.
		 * \return Returns true if the LUT was successfully built.
		 **/
		bool													BuildLUT( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, uint32_t _ui32SrcSize, uint32_t _ui32DstSize, std::unique_ptr<CDirectX12Resource> &_prLut, std::unique_ptr<CDirectX12Resource> &_prUpload, uint32_t _ui32SrvOffset, uint32_t &_ui32OutMaxTaps );

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
