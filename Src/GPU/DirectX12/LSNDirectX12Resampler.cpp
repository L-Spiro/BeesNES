#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 hardware wrapper for 2-pass GPU resampling.
 */

#include "LSNDirectX12Resampler.h"
#include "LSNDirectX12DiskInclude.h"
#include <algorithm>
#include <string>

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fW;
		float fU, fV;
	};
#pragma pack(pop)


	CDirectX12Resampler::CDirectX12Resampler() {
	}
	CDirectX12Resampler::~CDirectX12Resampler() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CDirectX12Resampler::Reset() {
		if LSN_LIKELY( m_prIntermediate.get() && m_prIntermediate->Get() ) { m_prIntermediate->Reset(); }
		if LSN_LIKELY( m_prLutX.get() && m_prLutX->Get() ) { m_prLutX->Reset(); }
		if LSN_LIKELY( m_prUploadLutX.get() && m_prUploadLutX->Get() ) { m_prUploadLutX->Reset(); }
		if LSN_LIKELY( m_prLutY.get() && m_prLutY->Get() ) { m_prLutY->Reset(); }
		if LSN_LIKELY( m_prUploadLutY.get() && m_prUploadLutY->Get() ) { m_prUploadLutY->Reset(); }
		if LSN_LIKELY( m_prVbQuad.get() && m_prVbQuad->Get() ) { m_prVbQuad->Reset(); }
		if LSN_LIKELY( m_ppsResampleX.get() && m_ppsResampleX->Get() ) { m_ppsResampleX->Reset(); }
		if LSN_LIKELY( m_ppsResampleY.get() && m_ppsResampleY->Get() ) { m_ppsResampleY->Reset(); }
		if LSN_LIKELY( m_prsRootSignature.get() && m_prsRootSignature->Get() ) { m_prsRootSignature->Reset(); }
		
		m_prIntermediate.reset();
		m_prLutX.reset();
		m_prUploadLutX.reset();
		m_prLutY.reset();
		m_prUploadLutY.reset();
		m_prVbQuad.reset();
		
		m_dhSrvHeap.reset();
		m_dhRtvHeap.reset();
		
		m_ppsResampleX.reset();
		m_ppsResampleY.reset();
		m_prsRootSignature.reset();
		
		m_ui32LastSrcW = 0;
		m_ui32LastSrcH = 0;
		m_ui32LastDstW = 0;
		m_ui32LastDstH = 0;
		m_ui32MaxTapsX = 0;
		m_ui32MaxTapsY = 0;
	}

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
	bool CDirectX12Resampler::Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, ID3D12Resource * _p12rDst, D3D12_CPU_DESCRIPTOR_HANDLE _cdhRtv, uint32_t _ui32DstW, uint32_t _ui32DstH, ID3DInclude * _piInclude ) {
		if LSN_UNLIKELY( !_pd12dDevice || !_pgclCommandList || !_prSrc || !_prSrc->Get() || !_p12rDst || !_ui32SrcW || !_ui32SrcH || !_ui32DstW || !_ui32DstH ) { return false; }
		if LSN_UNLIKELY( !EnsureResources( _pd12dDevice, _pgclCommandList, _ui32SrcW, _ui32SrcH, _ui32DstW, _ui32DstH, _piInclude ) ) { return false; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();
		ID3D12GraphicsCommandList * pCommandList = _pgclCommandList->Get();

		D3D12_CPU_DESCRIPTOR_HANDLE hSrvCpu = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateShaderResourceView( _prSrc->Get(), nullptr, hSrvCpu );

		ID3D12DescriptorHeap * ppHeaps[] = { m_dhSrvHeap->Get() };
		pCommandList->SetDescriptorHeaps( 1, ppHeaps );

		pCommandList->SetGraphicsRootSignature( m_prsRootSignature->Get() );
		pCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList->IASetVertexBuffers( 0, 1, &m_vbView );

		const SIZE_T stSrvSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		D3D12_GPU_DESCRIPTOR_HANDLE hSrvGpuBase = m_dhSrvHeap->Get()->GetGPUDescriptorHandleForHeapStart();

		// ---- Pass 1: Horizontal Resampling (Src -> Intermediate) ----
		{
			D3D12_RESOURCE_BARRIER rbBarriers[1];
			rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_prIntermediate->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
			pCommandList->ResourceBarrier( 1, rbBarriers );

			D3D12_CPU_DESCRIPTOR_HANDLE hIntermediateRtv = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
			pCommandList->OMSetRenderTargets( 1, &hIntermediateRtv, FALSE, nullptr );

			D3D12_VIEWPORT vpViewport = { 0.0f, 0.0f, static_cast<float>(_ui32DstW), static_cast<float>(_ui32SrcH), 0.0f, 1.0f };
			D3D12_RECT rScissor = { 0, 0, static_cast<LONG>(_ui32DstW), static_cast<LONG>(_ui32SrcH) };
			pCommandList->RSSetViewports( 1, &vpViewport );
			pCommandList->RSSetScissorRects( 1, &rScissor );

			pCommandList->SetPipelineState( m_ppsResampleX->Get() );

			float fVsConstants[4] = { static_cast<float>(_ui32DstW), static_cast<float>(_ui32SrcH), 0.0f, 0.0f };
			pCommandList->SetGraphicsRoot32BitConstants( 0, 4, fVsConstants, 0 );

			uint32_t ui32PsConstantsX[4] = { m_ui32MaxTapsX, 0, 0, 0 };
			pCommandList->SetGraphicsRoot32BitConstants( 1, 4, ui32PsConstantsX, 0 );

			// Offset 0 maps to (Src [0], LutX [1])
			D3D12_GPU_DESCRIPTOR_HANDLE hSrvPass1 = { hSrvGpuBase.ptr };
			pCommandList->SetGraphicsRootDescriptorTable( 2, hSrvPass1 );

			pCommandList->DrawInstanced( 4, 1, 0, 0 );
		}

		// ---- Pass 2: Vertical Resampling (Intermediate -> Dest) ----
		{
			D3D12_RESOURCE_BARRIER rbBarriers[1];
			rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_prIntermediate->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
			pCommandList->ResourceBarrier( 1, rbBarriers );

			pCommandList->OMSetRenderTargets( 1, &_cdhRtv, FALSE, nullptr );

			D3D12_VIEWPORT vpViewport = { 0.0f, 0.0f, static_cast<float>(_ui32DstW), static_cast<float>(_ui32DstH), 0.0f, 1.0f };
			D3D12_RECT rScissor = { 0, 0, static_cast<LONG>(_ui32DstW), static_cast<LONG>(_ui32DstH) };
			pCommandList->RSSetViewports( 1, &vpViewport );
			pCommandList->RSSetScissorRects( 1, &rScissor );

			pCommandList->SetPipelineState( m_ppsResampleY->Get() );

			float fVsConstants[4] = { static_cast<float>(_ui32DstW), static_cast<float>(_ui32DstH), 0.0f, 0.0f };
			pCommandList->SetGraphicsRoot32BitConstants( 0, 4, fVsConstants, 0 );

			uint32_t ui32PsConstantsY[4] = { m_ui32MaxTapsY, 0, 0, 0 };
			pCommandList->SetGraphicsRoot32BitConstants( 1, 4, ui32PsConstantsY, 0 );

			// Offset 2 maps to (Intermediate [2], LutY [3])
			D3D12_GPU_DESCRIPTOR_HANDLE hSrvPass2 = { hSrvGpuBase.ptr + 2 * stSrvSize };
			pCommandList->SetGraphicsRootDescriptorTable( 2, hSrvPass2 );

			pCommandList->DrawInstanced( 4, 1, 0, 0 );
		}

		return true;
	}

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
	bool CDirectX12Resampler::EnsureResources( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32DstW, uint32_t _ui32DstH, ID3DInclude * _piInclude ) {
		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();

		if LSN_UNLIKELY( !EnsureShaders( _pd12dDevice, _piInclude ) ) { return false; }

		if LSN_UNLIKELY( !m_prVbQuad.get() || !m_prVbQuad->Get() ) {
			m_prVbQuad = std::make_unique<CDirectX12Resource>();
			D3D12_HEAP_PROPERTIES hpUpload = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			D3D12_RESOURCE_DESC rdVb = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, sizeof( LSN_XYZRHWTEX1 ) * 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
			if ( !m_prVbQuad->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdVb, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }
			
			m_vbView.BufferLocation = m_prVbQuad->Get()->GetGPUVirtualAddress();
			m_vbView.StrideInBytes = sizeof( LSN_XYZRHWTEX1 );
			m_vbView.SizeInBytes = sizeof( LSN_XYZRHWTEX1 ) * 4;

			D3D12_RANGE rReadRange = { 0, 0 };
			LSN_XYZRHWTEX1 * pvP = nullptr;
			if ( SUCCEEDED( m_prVbQuad->Get()->Map( 0, &rReadRange, reinterpret_cast<void **>(&pvP) ) ) ) {
				pvP[0] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
				pvP[1] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f };
				pvP[2] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
				pvP[3] = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f };
				m_prVbQuad->Get()->Unmap( 0, nullptr );
			}
		}

		if LSN_UNLIKELY( !m_dhSrvHeap.get() ) {
			m_dhSrvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			// Capacity for 4 SRVs: [0: Src, 1: LutX, 2: Intermediate, 3: LutY].
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSrvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_dhRtvHeap.get() ) {
			m_dhRtvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
			if ( !m_dhRtvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		const SIZE_T stSrvSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		D3D12_CPU_DESCRIPTOR_HANDLE hSrvCpuBase = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();

		// Ensure intermediate float32 texture fits Pass 1 output (DstW x SrcH).
		bool bRebuildInt = !m_prIntermediate.get() || !m_prIntermediate->Get();
		if ( !bRebuildInt ) {
			D3D12_RESOURCE_DESC rdDesc = m_prIntermediate->Get()->GetDesc();
			if ( rdDesc.Width != _ui32DstW || rdDesc.Height != _ui32SrcH ) {
				bRebuildInt = true;
			}
		}

		if LSN_UNLIKELY( bRebuildInt ) {
			if LSN_LIKELY( m_prIntermediate.get() && m_prIntermediate->Get() ) { m_prIntermediate->Reset(); }
			m_prIntermediate = std::make_unique<CDirectX12Resource>();

			D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			D3D12_RESOURCE_DESC rdTex = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, _ui32DstW, _ui32SrcH, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
			D3D12_CLEAR_VALUE cvClear = { DXGI_FORMAT_R32G32B32A32_FLOAT, { 0.0f, 0.0f, 0.0f, 0.0f } };
			
			if ( !m_prIntermediate->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear ) ) { return false; }
			
			pd12Device->CreateRenderTargetView( m_prIntermediate->Get(), nullptr, m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart() );

			D3D12_CPU_DESCRIPTOR_HANDLE hIntSrv = { hSrvCpuBase.ptr + 2 * stSrvSize };
			pd12Device->CreateShaderResourceView( m_prIntermediate->Get(), nullptr, hIntSrv );
		}

		// Check if Horizontal LUT needs to be rebuilt.
		if LSN_UNLIKELY( !m_prLutX.get() || !m_prLutX->Get() || m_ui32LastSrcW != _ui32SrcW || m_ui32LastDstW != _ui32DstW ) {
			if ( !BuildLUT( _pd12dDevice, _pgclCommandList, _ui32SrcW, _ui32DstW, m_prLutX, m_prUploadLutX, 1, m_ui32MaxTapsX ) ) { return false; }
			m_ui32LastSrcW = _ui32SrcW;
			m_ui32LastDstW = _ui32DstW;
		}

		// Check if Vertical LUT needs to be rebuilt.
		if LSN_UNLIKELY( !m_prLutY.get() || !m_prLutY->Get() || m_ui32LastSrcH != _ui32SrcH || m_ui32LastDstH != _ui32DstH ) {
			if ( !BuildLUT( _pd12dDevice, _pgclCommandList, _ui32SrcH, _ui32DstH, m_prLutY, m_prUploadLutY, 3, m_ui32MaxTapsY ) ) { return false; }
			m_ui32LastSrcH = _ui32SrcH;
			m_ui32LastDstH = _ui32DstH;
		}

		return true;
	}

	/**
	 * Ensures the Pipeline State Objects and Root Signature are created.
	 * 
	 * \param _pd12dDevice The Direct3D 12 device.
	 * \param _piInclude Optional #include handler for shader compilation.
	 * \return Returns true if the shaders are ready.
	 **/
	bool CDirectX12Resampler::EnsureShaders( CDirectX12Device * _pd12dDevice, ID3DInclude * _piInclude ) {
		if LSN_LIKELY( m_ppsResampleX.get() && m_ppsResampleX->Get() && m_ppsResampleY.get() && m_ppsResampleY->Get() ) { return true; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();

		if ( !m_prsRootSignature.get() ) {
			m_prsRootSignature = std::make_unique<CDirectX12RootSignature>();
			
			// Single descriptor table handling both SRVs needed for a pass.
			D3D12_DESCRIPTOR_RANGE drRange;
			drRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			drRange.NumDescriptors = 2;
			drRange.BaseShaderRegister = 0;
			drRange.RegisterSpace = 0;
			drRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER rpParameters[3];
			rpParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[0].Constants.ShaderRegister = 0; rpParameters[0].Constants.RegisterSpace = 0; rpParameters[0].Constants.Num32BitValues = 4; rpParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			
			rpParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[1].Constants.ShaderRegister = 1; rpParameters[1].Constants.RegisterSpace = 0; rpParameters[1].Constants.Num32BitValues = 4; rpParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			rpParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[2].DescriptorTable.NumDescriptorRanges = 1; rpParameters[2].DescriptorTable.pDescriptorRanges = &drRange; rpParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_ROOT_SIGNATURE_DESC rsdDesc = { 3, rpParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };

			Microsoft::WRL::ComPtr<ID3DBlob> pbSig, pbErr;
			typedef HRESULT( WINAPI * PFN_D3D12SerializeRootSignature )( const D3D12_ROOT_SIGNATURE_DESC *, D3D_ROOT_SIGNATURE_VERSION, ID3DBlob **, ID3DBlob ** );
			PFN_D3D12SerializeRootSignature pfnSer = reinterpret_cast<PFN_D3D12SerializeRootSignature>(::GetProcAddress( _pd12dDevice->Dll().hHandle, "D3D12SerializeRootSignature" ));
			
			if ( !pfnSer || FAILED( pfnSer( &rsdDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pbSig, &pbErr ) ) ) { return false; }
			if ( !m_prsRootSignature->CreateRootSignature( pd12Device, 0, pbSig->GetBufferPointer(), pbSig->GetBufferSize() ) ) { return false; }
		}

		static const char * kVsHlsl =
			"cbuffer VSConstants : register(b0) { float2 TargetSize; float2 Padding; };\n"
			"struct VS_INPUT { float4 Pos : POSITION; float2 Tex : TEXCOORD0; };\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; };\n"
			"PS_INPUT main(VS_INPUT input) {\n"
			"    PS_INPUT output;\n"
			"    output.Pos.x = input.Pos.x * TargetSize.x;\n"
			"    output.Pos.y = input.Pos.y * TargetSize.y;\n"
			"    output.Pos.z = 0.5; output.Pos.w = 1.0;\n"
			"    // Convert correctly to normalized projection space for D3D.\n"
			"    output.Pos.x = (output.Pos.x / TargetSize.x) * 2.0 - 1.0;\n"
			"    output.Pos.y = 1.0 - (output.Pos.y / TargetSize.y) * 2.0;\n"
			"    return output;\n"
			"}\n";

		static const char * kPsResampleXHlsl =
			"Texture2D<float4> tSrc : register(t0);\n"
			"Texture2D<float4> tLut : register(t1);\n"
			"cbuffer PSConstants : register(b1) { uint MaxTaps; float3 Padding; };\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"    float4 cFinal = float4( 0.0, 0.0, 0.0, 0.0 );\n"
			"    int dstX = (int)input.Pos.x;\n"
			"    int srcY = (int)input.Pos.y;\n"
			"    for ( uint i = 0; i < MaxTaps; ++i ) {\n"
			"        float4 vLut = tLut.Load( int3( dstX, i, 0 ) );\n"
			"        int srcX = (int)vLut.g;\n"
			"        cFinal += tSrc.Load( int3( srcX, srcY, 0 ) ) * vLut.r;\n"
			"    }\n"
			"    return cFinal;\n"
			"}\n";

		static const char * kPsResampleYHlsl =
			"Texture2D<float4> tSrc : register(t0);\n"
			"Texture2D<float4> tLut : register(t1);\n"
			"cbuffer PSConstants : register(b1) { uint MaxTaps; float3 Padding; };\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"    float4 cFinal = float4( 0.0, 0.0, 0.0, 0.0 );\n"
			"    int dstX = (int)input.Pos.x;\n"
			"    int dstY = (int)input.Pos.y;\n"
			"    for ( uint i = 0; i < MaxTaps; ++i ) {\n"
			"        float4 vLut = tLut.Load( int3( dstY, i, 0 ) );\n"
			"        int srcY = (int)vLut.g;\n"
			"        cFinal += tSrc.Load( int3( dstX, srcY, 0 ) ) * vLut.r;\n"
			"    }\n"
			"    return cFinal;\n"
			"}\n";

		CDirectX12DiskInclude diDefaultInclude( CDirectX12DiskInclude::GetExeShadersDir() );
		ID3DInclude * pInc = _piInclude ? _piInclude : &diDefaultInclude;

		std::vector<uint8_t> vBcVs, vBcPsX, vBcPsY;
		if ( !CompileHlsl( _pd12dDevice, kVsHlsl, "main", "vs_5_0", vBcVs, pInc ) ||
			 !CompileHlsl( _pd12dDevice, kPsResampleXHlsl, "main", "ps_5_0", vBcPsX, pInc ) ||
			 !CompileHlsl( _pd12dDevice, kPsResampleYHlsl, "main", "ps_5_0", vBcPsY, pInc ) ) {
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC iedInputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdDesc = {};
		gpsdDesc.pRootSignature = m_prsRootSignature->Get();
		gpsdDesc.VS = { vBcVs.data(), vBcVs.size() };
		gpsdDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		gpsdDesc.SampleMask = UINT_MAX;
		gpsdDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		gpsdDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gpsdDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpsdDesc.NumRenderTargets = 1;
		gpsdDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; // All targets utilize this precision.
		gpsdDesc.SampleDesc.Count = 1;
		gpsdDesc.InputLayout = { iedInputLayout, 2 };

		// Width pass.
		m_ppsResampleX = std::make_unique<CDirectX12PipelineState>();
		gpsdDesc.PS = { vBcPsX.data(), vBcPsX.size() };
		if ( !m_ppsResampleX->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

		// Height pass.
		m_ppsResampleY = std::make_unique<CDirectX12PipelineState>();
		gpsdDesc.PS = { vBcPsY.data(), vBcPsY.size() };
		if ( !m_ppsResampleY->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

		return true;
	}

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
	bool CDirectX12Resampler::BuildLUT( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, uint32_t _ui32SrcSize, uint32_t _ui32DstSize, std::unique_ptr<CDirectX12Resource> &_prLut, std::unique_ptr<CDirectX12Resource> &_prUpload, uint32_t _ui32SrvOffset, uint32_t &_ui32OutMaxTaps ) {
		if LSN_UNLIKELY( !CreateContribList( _ui32SrcSize, _ui32DstSize, LSN_TA_CLAMP, CResamplerBase::m_fFilter[m_ffFilter].pfFunc, CResamplerBase::m_fFilter[m_ffFilter].fSupport, 1.0f ) ) { return false; }

		uint32_t ui32MaxTaps = 0;
		for ( size_t I = 0; I < m_cContribs.size(); ++I ) {
			ui32MaxTaps = std::max( ui32MaxTaps, static_cast<uint32_t>(m_cContribs[I].fContributions.size()) );
		}

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();
		if LSN_LIKELY( _prLut.get() && _prLut->Get() ) { _prLut->Reset(); }
		if LSN_LIKELY( _prUpload.get() && _prUpload->Get() ) { _prUpload->Reset(); }

		_prLut = std::make_unique<CDirectX12Resource>();
		_prUpload = std::make_unique<CDirectX12Resource>();

		D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
		D3D12_HEAP_PROPERTIES hpUpload = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			
		D3D12_RESOURCE_DESC rdTex = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, _ui32DstSize, ui32MaxTaps, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE };
		if ( !_prLut->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdTex, D3D12_RESOURCE_STATE_COPY_DEST ) ) { return false; }

		UINT64 ui64UploadSize = 0;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT psfFootprint;
		pd12Device->GetCopyableFootprints( &rdTex, 0, 1, 0, &psfFootprint, nullptr, nullptr, &ui64UploadSize );
		
		D3D12_RESOURCE_DESC rdUpload = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, ui64UploadSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
		if ( !_prUpload->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdUpload, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }

		D3D12_RANGE rReadRange = { 0, 0 };
		void * pvData = nullptr;
		if LSN_UNLIKELY( FAILED( _prUpload->Get()->Map( 0, &rReadRange, &pvData ) ) ) { return false; }

		const UINT uiPitch = psfFootprint.Footprint.RowPitch;
		for ( uint32_t Y = 0; Y < ui32MaxTaps; ++Y ) {
			float * pfRow = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(pvData) + Y * uiPitch);
			for ( uint32_t X = 0; X < _ui32DstSize; ++X ) {
				if ( Y < m_cContribs[X].fContributions.size() ) {
					pfRow[X*4+0] = m_cContribs[X].fContributions[Y];					// R: Weight
					pfRow[X*4+1] = static_cast<float>(m_cContribs[X].i32Indices[Y]);	// G: Raw Source Integer Coordinate
				}
				else {
					pfRow[X*4+0] = 0.0f;
					pfRow[X*4+1] = 0.0f;
				}
				pfRow[X*4+2] = 0.0f;													// B
				pfRow[X*4+3] = 0.0f;													// A
			}
		}

		_prUpload->Get()->Unmap( 0, nullptr );

		D3D12_TEXTURE_COPY_LOCATION tclDest = { _prLut->Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, { 0 } };
		D3D12_TEXTURE_COPY_LOCATION tclSrc = { _prUpload->Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, { psfFootprint } };
		_pgclCommandList->Get()->CopyTextureRegion( &tclDest, 0, 0, 0, &tclSrc, nullptr );

		D3D12_RESOURCE_BARRIER rbBarriers[1];
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { _prLut->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		_pgclCommandList->Get()->ResourceBarrier( 1, rbBarriers );

		// Register SRV in the managed heap at the appropriate offset.
		const SIZE_T stSrvSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		D3D12_CPU_DESCRIPTOR_HANDLE hSrvCpu = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		hSrvCpu.ptr += _ui32SrvOffset * stSrvSize;
		pd12Device->CreateShaderResourceView( _prLut->Get(), nullptr, hSrvCpu );

		m_cContribs.clear();
		m_cContribs.shrink_to_fit();

		_ui32OutMaxTaps = ui32MaxTaps;
		return true;
	}

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
	bool CDirectX12Resampler::CompileHlsl( CDirectX12Device * /*_pd12dDevice*/, const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<uint8_t> &_vOutByteCode, ID3DInclude * _piInclude ) {
		lsw::LSW_HMODULE hCompiler( L"d3dcompiler_47.dll" );
		if ( !hCompiler.Valid() ) { return false; }
		typedef HRESULT( WINAPI * PFN_D3DCOMPILE )( LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO *, ID3DInclude *, LPCSTR, LPCSTR, UINT, UINT, ID3DBlob **, ID3DBlob ** );
		PFN_D3DCOMPILE pfnCompile = reinterpret_cast<PFN_D3DCOMPILE>(::GetProcAddress( hCompiler.hHandle, "D3DCompile" ));
		if ( !pfnCompile ) { return false; }

		Microsoft::WRL::ComPtr<ID3DBlob> pbCode, pbErr;
		HRESULT hRes = pfnCompile(
			_pcszSource,
			std::strlen( _pcszSource ),
			nullptr,
			nullptr,
			_piInclude,
			_pcszEntry,
			_pcszProfile,
			D3DCOMPILE_OPTIMIZATION_LEVEL3,
			0,
			&pbCode,
			&pbErr );
		
		if ( FAILED( hRes ) || !pbCode ) { return false; }
		_vOutByteCode.resize( pbCode->GetBufferSize() );
		std::memcpy( _vOutByteCode.data(), pbCode->GetBufferPointer(), pbCode->GetBufferSize() );
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
