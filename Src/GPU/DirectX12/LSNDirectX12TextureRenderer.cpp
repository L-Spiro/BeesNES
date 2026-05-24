#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for rendering a texture to a surface with bilinear sampling and gamma correction.
 */

#include "LSNDirectX12TextureRenderer.h"

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fW;
		float fU, fV;
	};
#pragma pack(pop)

	CDirectX12TextureRenderer::CDirectX12TextureRenderer() {
	}
	CDirectX12TextureRenderer::~CDirectX12TextureRenderer() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the vertex buffer, shaders, and internal states.
	 **/
	void CDirectX12TextureRenderer::Reset() {
		if LSN_LIKELY( m_prVbQuad.get() && m_prVbQuad->Get() ) { m_prVbQuad->Reset(); }
		m_prVbQuad.reset();
		if LSN_LIKELY( m_ppsCopy.get() && m_ppsCopy->Get() ) { m_ppsCopy->Reset(); }
		m_ppsCopy.reset();
		if LSN_LIKELY( m_prsRootSignature.get() && m_prsRootSignature->Get() ) { m_prsRootSignature->Reset(); }
		m_prsRootSignature.reset();
		
		m_dhSrvHeap.reset();
		m_dhSamplerHeap.reset();
		m_fPsoFormat = DXGI_FORMAT_UNKNOWN;
	}

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
	bool CDirectX12TextureRenderer::Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, CDirectX12Resource * _prDst, D3D12_CPU_DESCRIPTOR_HANDLE _cdhRtv, const lsw::LSW_RECT &_rOutput, float _fGamma, bool _bClear, bool _bSrgb, ID3DInclude * _piInclude ) {
		if LSN_UNLIKELY( !_pd12dDevice || !_pgclCommandList || !_prSrc || !_prDst ) { return false; }
		if LSN_UNLIKELY( !EnsureResources( _pd12dDevice ) ) { return false; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();
		ID3D12GraphicsCommandList * pCommandList = _pgclCommandList->Get();

		const DXGI_FORMAT fTargetFormat = _bSrgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		if LSN_UNLIKELY( !EnsureShaders( _pd12dDevice, fTargetFormat, _piInclude ) ) { return false; }

		// Write Source SRV into local heap.
		D3D12_CPU_DESCRIPTOR_HANDLE hSrvCpu = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateShaderResourceView( _prSrc->Get(), nullptr, hSrvCpu );

		pCommandList->OMSetRenderTargets( 1, &_cdhRtv, FALSE, nullptr );

		if ( _bClear ) {
			float fClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			pCommandList->ClearRenderTargetView( _cdhRtv, fClearColor, 0, nullptr );
		}

		D3D12_RESOURCE_DESC rdDesc = _prDst->Get()->GetDesc();
		D3D12_VIEWPORT vpViewport = { 0.0f, 0.0f, static_cast<float>(rdDesc.Width), static_cast<float>(rdDesc.Height), 0.0f, 1.0f };
		D3D12_RECT rScissor = { 0, 0, static_cast<LONG>(rdDesc.Width), static_cast<LONG>(rdDesc.Height) };
		pCommandList->RSSetViewports( 1, &vpViewport );
		pCommandList->RSSetScissorRects( 1, &rScissor );

		pCommandList->SetPipelineState( m_ppsCopy->Get() );
		pCommandList->SetGraphicsRootSignature( m_prsRootSignature->Get() );

		ID3D12DescriptorHeap * ppHeaps[] = { m_dhSrvHeap->Get(), m_dhSamplerHeap->Get() };
		pCommandList->SetDescriptorHeaps( 2, ppHeaps );

		float fVsConstants[4] = { static_cast<float>(rdDesc.Width), static_cast<float>(rdDesc.Height), 0.0f, 0.0f };
		pCommandList->SetGraphicsRoot32BitConstants( 0, 4, fVsConstants, 0 );

		float fPsConstants[4] = { _fGamma, 0.0f, 0.0f, 0.0f };
		pCommandList->SetGraphicsRoot32BitConstants( 1, 4, fPsConstants, 0 );

		D3D12_GPU_DESCRIPTOR_HANDLE hSrvGpu = m_dhSrvHeap->Get()->GetGPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hSampGpu = m_dhSamplerHeap->Get()->GetGPUDescriptorHandleForHeapStart();
		pCommandList->SetGraphicsRootDescriptorTable( 2, hSrvGpu );
		pCommandList->SetGraphicsRootDescriptorTable( 3, hSampGpu );

		D3D12_RANGE rReadRange = { 0, 0 };
		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_prVbQuad->Get()->Map( 0, &rReadRange, reinterpret_cast<void **>(&pvP) ) ) ) {
			float fL = static_cast<float>(_rOutput.left);
			float fT = static_cast<float>(_rOutput.top);
			float fR = static_cast<float>(_rOutput.right);
			float fB = static_cast<float>(_rOutput.bottom);

			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, 0.0f };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, 0.0f };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, 1.0f };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, 1.0f };
			m_prVbQuad->Get()->Unmap( 0, nullptr );
		}

		pCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList->IASetVertexBuffers( 0, 1, &m_vbView );
		pCommandList->DrawInstanced( 4, 1, 0, 0 );

		return true;
	}

	/**
	 * Ensures the vertex buffer and descriptor heaps are created.
	 * 
	 * \param _pd12dDevice The Direct3D 12 device.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX12TextureRenderer::EnsureResources( CDirectX12Device * _pd12dDevice ) {
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
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSrvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_dhSamplerHeap.get() ) {
			m_dhSamplerHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSamplerHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }

			D3D12_SAMPLER_DESC sdLinear = {};
			sdLinear.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			sdLinear.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sdLinear.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sdLinear.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			pd12Device->CreateSampler( &sdLinear, m_dhSamplerHeap->Get()->GetCPUDescriptorHandleForHeapStart() );
		}

		return true;
	}

	/**
	 * Ensures the Pipeline State Object and Root Signature are created for the specified format.
	 * 
	 * \param _pd12dDevice The Direct3D 12 device.
	 * \param _fTargetFormat The format of the RTV this shader will output to.
	 * \param _piInclude Optional #include handler for shader compilation.
	 * \return Returns true if the shaders are ready.
	 **/
	bool CDirectX12TextureRenderer::EnsureShaders( CDirectX12Device * _pd12dDevice, DXGI_FORMAT _fTargetFormat, ID3DInclude * _piInclude ) {
		if LSN_LIKELY( m_ppsCopy.get() && m_ppsCopy->Get() && m_fPsoFormat == _fTargetFormat ) { return true; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();
		m_ppsCopy = std::make_unique<CDirectX12PipelineState>();
		m_fPsoFormat = _fTargetFormat;

		if ( !m_prsRootSignature.get() ) {
			m_prsRootSignature = std::make_unique<CDirectX12RootSignature>();
			D3D12_DESCRIPTOR_RANGE drRanges[2];
			drRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drRanges[0].NumDescriptors = 1; drRanges[0].BaseShaderRegister = 0; drRanges[0].RegisterSpace = 0; drRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			drRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER; drRanges[1].NumDescriptors = 1; drRanges[1].BaseShaderRegister = 0; drRanges[1].RegisterSpace = 0; drRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER rpParameters[4];
			rpParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[0].Constants.ShaderRegister = 0; rpParameters[0].Constants.RegisterSpace = 0; rpParameters[0].Constants.Num32BitValues = 4; rpParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			
			rpParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[1].Constants.ShaderRegister = 0; rpParameters[1].Constants.RegisterSpace = 0; rpParameters[1].Constants.Num32BitValues = 4; rpParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			rpParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[2].DescriptorTable.NumDescriptorRanges = 1; rpParameters[2].DescriptorTable.pDescriptorRanges = &drRanges[0]; rpParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			
			rpParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[3].DescriptorTable.NumDescriptorRanges = 1; rpParameters[3].DescriptorTable.pDescriptorRanges = &drRanges[1]; rpParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_ROOT_SIGNATURE_DESC rsdDesc = { 4, rpParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };

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

		static const char * kPsCopyHlsl =
			"Texture2D tSrc : register(t0);\n"
			"SamplerState sLinear : register(s0);\n"
			"cbuffer PSConstants : register(b0) { float4 c0; };\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"  float4 c = tSrc.Sample(sLinear, input.Tex);\n"
			"  return saturate(c);\n"
			"}\n";

		std::vector<uint8_t> vBcVs, vBcCopy;
		if ( !CompileHlsl( _pd12dDevice, kVsHlsl, "main", "vs_5_0", vBcVs, _piInclude ) ||
			 !CompileHlsl( _pd12dDevice, kPsCopyHlsl, "main", "ps_5_0", vBcCopy, _piInclude ) ) {
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC iedInputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdDesc = {};
		gpsdDesc.pRootSignature = m_prsRootSignature->Get();
		gpsdDesc.VS = { vBcVs.data(), vBcVs.size() };
		gpsdDesc.PS = { vBcCopy.data(), vBcCopy.size() };
		gpsdDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		gpsdDesc.SampleMask = UINT_MAX;
		gpsdDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		gpsdDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gpsdDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpsdDesc.NumRenderTargets = 1;
		gpsdDesc.RTVFormats[0] = _fTargetFormat;
		gpsdDesc.SampleDesc.Count = 1;
		gpsdDesc.InputLayout = { iedInputLayout, 2 };

		if ( !m_ppsCopy->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

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
	bool CDirectX12TextureRenderer::CompileHlsl( CDirectX12Device * /*_pd12dDevice*/, const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<uint8_t> &_vOutByteCode, ID3DInclude * _piInclude ) {
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
