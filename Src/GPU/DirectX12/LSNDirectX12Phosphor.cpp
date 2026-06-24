#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 hardware wrapper for a phosphor-decay post-processing effect.
 */

#include "LSNDirectX12Phosphor.h"
#include "LSNDirectX12DiskInclude.h"
#include <algorithm>
#include <string>

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fW;
		float fU, fV;
	};
#pragma pack( pop )


	CDirectX12Phosphor::CDirectX12Phosphor() {
	}
	CDirectX12Phosphor::~CDirectX12Phosphor() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CDirectX12Phosphor::Reset() {
		if LSN_LIKELY( m_prPhosphor[0].get() && m_prPhosphor[0]->Get() ) { m_prPhosphor[0]->Reset(); }
		if LSN_LIKELY( m_prPhosphor[1].get() && m_prPhosphor[1]->Get() ) { m_prPhosphor[1]->Reset(); }
		if LSN_LIKELY( m_prVbQuad.get() && m_prVbQuad->Get() ) { m_prVbQuad->Reset(); }
		
		m_prPhosphor[0].reset();
		m_prPhosphor[1].reset();
		m_prVbQuad.reset();
		
		m_dhSrvHeap.reset();
		m_dhRtvHeap.reset();
		m_dhSamplerHeap.reset();
		
		m_ppsPhosphorOutput.reset();
		m_ppsPhosphorUpdate.reset();
		m_prsRootSignature.reset();
		
		m_ui32LastSrcW = 0;
		m_ui32LastSrcH = 0;
		m_fLastTargetFormat = DXGI_FORMAT_UNKNOWN;
		m_stReadIndex = 0;
	}

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
	bool CDirectX12Phosphor::Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, ID3D12Resource * _p12rDst, D3D12_CPU_DESCRIPTOR_HANDLE _cdhRtv, DXGI_FORMAT _fTargetFormat, float _fInitDecay, float _fDecayR, float _fDecayG, float _fDecayB, ID3DInclude * _piInclude ) {
		if LSN_UNLIKELY( !_pd12dDevice || !_pgclCommandList || !_prSrc || !_prSrc->Get() || !_p12rDst || !_ui32SrcW || !_ui32SrcH ) { return false; }
		if LSN_UNLIKELY( !EnsureResources( _pd12dDevice, _pgclCommandList, _ui32SrcW, _ui32SrcH ) ) { return false; }
		if LSN_UNLIKELY( !EnsureShaders( _pd12dDevice, _fTargetFormat, _piInclude ) ) { return false; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();
		ID3D12GraphicsCommandList * pCommandList = _pgclCommandList->Get();

		// Write Source SRV into local heap at Offset 0.
		D3D12_CPU_DESCRIPTOR_HANDLE hSrvCpu = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateShaderResourceView( _prSrc->Get(), nullptr, hSrvCpu );

		ID3D12DescriptorHeap * ppHeaps[] = { m_dhSrvHeap->Get(), m_dhSamplerHeap->Get() };
		pCommandList->SetDescriptorHeaps( 2, ppHeaps );

		pCommandList->SetGraphicsRootSignature( m_prsRootSignature->Get() );
		
		D3D12_RANGE rReadRange = { 0, 0 };
		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_prVbQuad->Get()->Map( 0, &rReadRange, reinterpret_cast<void **>(&pvP) ) ) ) {
			float fL = 0.0f;
			float fT = 0.0f;
			float fR = static_cast<float>(_ui32SrcW);
			float fB = static_cast<float>(_ui32SrcH);

			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, 0.0f };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, 0.0f };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, 1.0f };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, 1.0f };
			m_prVbQuad->Get()->Unmap( 0, nullptr );
		}

		pCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList->IASetVertexBuffers( 0, 1, &m_vbView );

		const SIZE_T stSrvSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		D3D12_GPU_DESCRIPTOR_HANDLE hSrvGpuBase = m_dhSrvHeap->Get()->GetGPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hSampGpu = m_dhSamplerHeap->Get()->GetGPUDescriptorHandleForHeapStart();

		float fVsConstants[4] = { static_cast<float>(_ui32SrcW), static_cast<float>(_ui32SrcH), 0.0f, 0.0f };
		pCommandList->SetGraphicsRoot32BitConstants( 0, 4, fVsConstants, 0 );

		float fPsConstants[4] = { _fDecayR, _fDecayG, _fDecayB, _fInitDecay };
		pCommandList->SetGraphicsRoot32BitConstants( 1, 4, fPsConstants, 0 );

		D3D12_GPU_DESCRIPTOR_HANDLE hSrcSrv = { hSrvGpuBase.ptr };
		D3D12_GPU_DESCRIPTOR_HANDLE hReadSrv = { hSrvGpuBase.ptr + (m_stReadIndex + 1) * stSrvSize };
		
		pCommandList->SetGraphicsRootDescriptorTable( 2, hSrcSrv );
		pCommandList->SetGraphicsRootDescriptorTable( 3, hReadSrv );
		pCommandList->SetGraphicsRootDescriptorTable( 4, hSampGpu );

		D3D12_VIEWPORT vpViewport = { 0.0f, 0.0f, static_cast<float>(_ui32SrcW), static_cast<float>(_ui32SrcH), 0.0f, 1.0f };
		D3D12_RECT rScissor = { 0, 0, static_cast<LONG>(_ui32SrcW), static_cast<LONG>(_ui32SrcH) };
		pCommandList->RSSetViewports( 1, &vpViewport );
		pCommandList->RSSetScissorRects( 1, &rScissor );

		size_t stWriteIndex = m_stReadIndex ^ 1;

		// The Write buffer needs to be transitioned to RENDER_TARGET for Pass 2.
		// (The Read buffer is already in PIXEL_SHADER_RESOURCE from the previous frame).
		D3D12_RESOURCE_BARRIER rbWriteToRtv[1];
		rbWriteToRtv[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_prPhosphor[stWriteIndex]->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
		pCommandList->ResourceBarrier( 1, rbWriteToRtv );

		// ---- Pass 1: Render Visible Composite to _p12rDst ----
		pCommandList->OMSetRenderTargets( 1, &_cdhRtv, FALSE, nullptr );
		pCommandList->SetPipelineState( m_ppsPhosphorOutput->Get() );
		pCommandList->DrawInstanced( 4, 1, 0, 0 );

		// ---- Pass 2: Update Phosphor History Buffer ----
		const SIZE_T stRtvSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
		D3D12_CPU_DESCRIPTOR_HANDLE hWriteRtv = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		hWriteRtv.ptr += stWriteIndex * stRtvSize;

		pCommandList->OMSetRenderTargets( 1, &hWriteRtv, FALSE, nullptr );
		pCommandList->SetPipelineState( m_ppsPhosphorUpdate->Get() );
		pCommandList->DrawInstanced( 4, 1, 0, 0 );

		// Transition the Write buffer back to PIXEL_SHADER_RESOURCE so it's ready to be read next frame.
		D3D12_RESOURCE_BARRIER rbWriteToSrv[1];
		rbWriteToSrv[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_prPhosphor[stWriteIndex]->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		pCommandList->ResourceBarrier( 1, rbWriteToSrv );

		// Swap read/write for the next frame.
		m_stReadIndex = stWriteIndex;

		return true;
	}

	/**
	 * Ensures the vertex buffer and ping-pong render targets are correctly built and cleared.
	 * 
	 * \param _pd12dDevice The Direct3D 12 device.
	 * \param _pgclCommandList The command list used to clear the resources on creation.
	 * \param _ui32SrcW The source width.
	 * \param _ui32SrcH The source height.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX12Phosphor::EnsureResources( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH ) {
		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();

		if LSN_UNLIKELY( !m_prVbQuad.get() || !m_prVbQuad->Get() ) {
			m_prVbQuad = std::make_unique<CDirectX12Resource>();
			D3D12_HEAP_PROPERTIES hpUpload = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			D3D12_RESOURCE_DESC rdVb = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, sizeof( LSN_XYZRHWTEX1 ) * 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
			if ( !m_prVbQuad->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdVb, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }
			
			m_vbView.BufferLocation = m_prVbQuad->Get()->GetGPUVirtualAddress();
			m_vbView.StrideInBytes = sizeof( LSN_XYZRHWTEX1 );
			m_vbView.SizeInBytes = sizeof( LSN_XYZRHWTEX1 ) * 4;
		}

		if LSN_UNLIKELY( !m_dhSrvHeap.get() ) {
			m_dhSrvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			// Capacity for 3 SRVs: [0: Src, 1: Phosphor0, 2: Phosphor1]
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSrvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_dhRtvHeap.get() ) {
			m_dhRtvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			// Capacity for 2 RTVs: [0: Phosphor0, 1: Phosphor1]
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
			if ( !m_dhRtvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_dhSamplerHeap.get() ) {
			m_dhSamplerHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSamplerHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }

			D3D12_SAMPLER_DESC sdPoint = {};
			sdPoint.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			sdPoint.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sdPoint.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sdPoint.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			pd12Device->CreateSampler( &sdPoint, m_dhSamplerHeap->Get()->GetCPUDescriptorHandleForHeapStart() );
		}

		const SIZE_T stSrvSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		const SIZE_T stRtvSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

		bool bRebuild = !m_prPhosphor[0].get() || !m_prPhosphor[0]->Get() || !m_prPhosphor[1].get() || !m_prPhosphor[1]->Get();
		if ( !bRebuild ) {
			D3D12_RESOURCE_DESC rdDesc = m_prPhosphor[0]->Get()->GetDesc();
			if ( rdDesc.Width != _ui32SrcW || rdDesc.Height != _ui32SrcH ) {
				bRebuild = true;
			}
		}

		if LSN_UNLIKELY( bRebuild ) {
			D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			D3D12_RESOURCE_DESC rdTex = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, _ui32SrcW, _ui32SrcH, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
			D3D12_CLEAR_VALUE cvClear = { DXGI_FORMAT_R16G16B16A16_FLOAT, { 0.0f, 0.0f, 0.0f, 0.0f } };

			for ( size_t I = 0; I < 2; ++I ) {
				if LSN_LIKELY( m_prPhosphor[I].get() && m_prPhosphor[I]->Get() ) { m_prPhosphor[I]->Reset(); }
				m_prPhosphor[I] = std::make_unique<CDirectX12Resource>();

				if ( !m_prPhosphor[I]->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdTex, D3D12_RESOURCE_STATE_RENDER_TARGET, &cvClear ) ) { return false; }
				
				D3D12_CPU_DESCRIPTOR_HANDLE hSrv = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
				hSrv.ptr += (I + 1) * stSrvSize; // Slot 1 and 2
				pd12Device->CreateShaderResourceView( m_prPhosphor[I]->Get(), nullptr, hSrv );

				D3D12_CPU_DESCRIPTOR_HANDLE hRtv = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
				hRtv.ptr += I * stRtvSize;
				pd12Device->CreateRenderTargetView( m_prPhosphor[I]->Get(), nullptr, hRtv );

				// Clear to black on creation.
				_pgclCommandList->Get()->ClearRenderTargetView( hRtv, cvClear.Color, 0, nullptr );

				// Transition to PIXEL_SHADER_RESOURCE so it's ready for the first Read/Write cycle.
				D3D12_RESOURCE_BARRIER rbInitToSrv[1];
				rbInitToSrv[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_prPhosphor[I]->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
				_pgclCommandList->Get()->ResourceBarrier( 1, rbInitToSrv );
			}

			m_ui32LastSrcW = _ui32SrcW;
			m_ui32LastSrcH = _ui32SrcH;
			m_stReadIndex = 0;
		}

		return true;
	}

	/**
	 * Ensures the Pipeline State Objects and Root Signature are created for the specified format.
	 * 
	 * \param _pd12dDevice The Direct3D 12 device.
	 * \param _fTargetFormat The format of the RTV this shader will output to.
	 * \param _piInclude Optional #include handler for shader compilation.
	 * \return Returns true if the shaders are ready.
	 **/
	bool CDirectX12Phosphor::EnsureShaders( CDirectX12Device * _pd12dDevice, DXGI_FORMAT _fTargetFormat, ID3DInclude * _piInclude ) {
		if LSN_LIKELY( m_ppsPhosphorOutput.get() && m_ppsPhosphorOutput->Get() && m_ppsPhosphorUpdate.get() && m_ppsPhosphorUpdate->Get() && m_fLastTargetFormat == _fTargetFormat ) { return true; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();

		if ( !m_prsRootSignature.get() ) {
			m_prsRootSignature = std::make_unique<CDirectX12RootSignature>();
			
			D3D12_DESCRIPTOR_RANGE drSrcRange;
			drSrcRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drSrcRange.NumDescriptors = 1; drSrcRange.BaseShaderRegister = 0; drSrcRange.RegisterSpace = 0; drSrcRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_DESCRIPTOR_RANGE drHistRange;
			drHistRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drHistRange.NumDescriptors = 1; drHistRange.BaseShaderRegister = 1; drHistRange.RegisterSpace = 0; drHistRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_DESCRIPTOR_RANGE drSampRange;
			drSampRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER; drSampRange.NumDescriptors = 1; drSampRange.BaseShaderRegister = 0; drSampRange.RegisterSpace = 0; drSampRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


			D3D12_ROOT_PARAMETER rpParameters[5];
			rpParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[0].Constants.ShaderRegister = 0; rpParameters[0].Constants.RegisterSpace = 0; rpParameters[0].Constants.Num32BitValues = 4; rpParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			
			rpParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[1].Constants.ShaderRegister = 1; rpParameters[1].Constants.RegisterSpace = 0; rpParameters[1].Constants.Num32BitValues = 4; rpParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			rpParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[2].DescriptorTable.NumDescriptorRanges = 1; rpParameters[2].DescriptorTable.pDescriptorRanges = &drSrcRange; rpParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			rpParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[3].DescriptorTable.NumDescriptorRanges = 1; rpParameters[3].DescriptorTable.pDescriptorRanges = &drHistRange; rpParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			rpParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[4].DescriptorTable.NumDescriptorRanges = 1; rpParameters[4].DescriptorTable.pDescriptorRanges = &drSampRange; rpParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


			D3D12_ROOT_SIGNATURE_DESC rsdDesc = { 5, rpParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };

			Microsoft::WRL::ComPtr<ID3DBlob> pbSig, pbErr;
			typedef HRESULT( WINAPI * PFN_D3D12SerializeRootSignature )( const D3D12_ROOT_SIGNATURE_DESC *, D3D_ROOT_SIGNATURE_VERSION, ID3DBlob **, ID3DBlob ** );
			PFN_D3D12SerializeRootSignature pfnSer = reinterpret_cast<PFN_D3D12SerializeRootSignature>(::GetProcAddress( _pd12dDevice->Dll().hHandle, "D3D12SerializeRootSignature" ));
			
			if ( !pfnSer || FAILED( pfnSer( &rsdDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pbSig, &pbErr ) ) ) { return false; }
			if ( !m_prsRootSignature->CreateRootSignature( pd12Device, 0, pbSig->GetBufferPointer(), pbSig->GetBufferSize() ) ) { return false; }
		}

		static const char * kVsHlsl =
			"cbuffer VSConstants : register(b0) { float2 TargetSize; float2 Padding; };\n"
			"struct VS_INPUT { float4 Pos : POSITION; float2 Tex : TEXCOORD0; };\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"PS_INPUT main(VS_INPUT input) {\n"
			"    PS_INPUT output;\n"
			"    output.Pos.x = (input.Pos.x / TargetSize.x) * 2.0 - 1.0;\n"
			"    output.Pos.y = 1.0 - (input.Pos.y / TargetSize.y) * 2.0;\n"
			"    output.Pos.z = 0.5; output.Pos.w = 1.0;\n"
			"    output.Tex = input.Tex;\n"
			"    return output;\n"
			"}\n";

		static const char * kPsOutputHlsl =
			"Texture2D tSrc : register(t0);\n"
			"Texture2D tHistory : register(t1);\n"
			"SamplerState sPoint : register(s0);\n"
			"cbuffer PSConstants : register(b1) { float3 DecayRates; float InitDecay; };\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"    float4 curColor = tSrc.Sample(sPoint, input.Tex);\n"
			"    float4 oldColor = tHistory.Sample(sPoint, input.Tex);\n"
			"    float3 oldDecayed = oldColor.rgb * DecayRates;\n"
			"    return float4(max(oldDecayed, curColor.rgb), curColor.a);\n"
			"}\n";

		static const char * kPsUpdateHlsl =
			"Texture2D tSrc : register(t0);\n"
			"Texture2D tHistory : register(t1);\n"
			"SamplerState sPoint : register(s0);\n"
			"cbuffer PSConstants : register(b1) { float3 DecayRates; float InitDecay; };\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"    float4 curColor = tSrc.Sample(sPoint, input.Tex);\n"
			"    float4 oldColor = tHistory.Sample(sPoint, input.Tex);\n"
			"    float3 oldDecayed = oldColor.rgb * DecayRates;\n"
			"    float3 scaledCurrent = curColor.rgb * InitDecay;\n"
			"    return float4(max(scaledCurrent, oldDecayed), curColor.a);\n"
			"}\n";

		CDirectX12DiskInclude diDefaultInclude( CDirectX12DiskInclude::GetExeShadersDir() );
		ID3DInclude * pInc = _piInclude ? _piInclude : &diDefaultInclude;

		std::vector<uint8_t> vBcVs, vBcPsOutput, vBcPsUpdate;
		if ( !CompileHlsl( _pd12dDevice, kVsHlsl, "main", "vs_5_0", vBcVs, pInc ) ||
			 !CompileHlsl( _pd12dDevice, kPsOutputHlsl, "main", "ps_5_0", vBcPsOutput, pInc ) ||
			 !CompileHlsl( _pd12dDevice, kPsUpdateHlsl, "main", "ps_5_0", vBcPsUpdate, pInc ) ) {
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
		gpsdDesc.SampleDesc.Count = 1;
		gpsdDesc.InputLayout = { iedInputLayout, 2 };

		// Compile Screen Output Pass (dynamic format)
		if ( m_ppsPhosphorOutput.get() ) { m_ppsPhosphorOutput->Reset(); }
		m_ppsPhosphorOutput = std::make_unique<CDirectX12PipelineState>();
		gpsdDesc.PS = { vBcPsOutput.data(), vBcPsOutput.size() };
		gpsdDesc.RTVFormats[0] = _fTargetFormat;
		if ( !m_ppsPhosphorOutput->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

		// Compile Buffer Update Pass (fixed format to internal A16B16G16R16F buffer)
		if ( m_ppsPhosphorUpdate.get() ) { m_ppsPhosphorUpdate->Reset(); }
		m_ppsPhosphorUpdate = std::make_unique<CDirectX12PipelineState>();
		gpsdDesc.PS = { vBcPsUpdate.data(), vBcPsUpdate.size() };
		gpsdDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		if ( !m_ppsPhosphorUpdate->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

		m_fLastTargetFormat = _fTargetFormat;

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
	bool CDirectX12Phosphor::CompileHlsl( CDirectX12Device * /*_pd12dDevice*/, const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<uint8_t> &_vOutByteCode, ID3DInclude * _piInclude ) {
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
