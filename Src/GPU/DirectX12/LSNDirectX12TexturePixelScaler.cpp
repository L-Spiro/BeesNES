#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying integer nearest-neighbor scaling and gamma to a texture.
 */

#include "LSNDirectX12TexturePixelScaler.h"
#include "LSNDirectX12DiskInclude.h"

#include <string>

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fW;
		float fU, fV;
	};
#pragma pack(pop)

	CDirectX12TexturePixelScaler::CDirectX12TexturePixelScaler() {
	}
	CDirectX12TexturePixelScaler::~CDirectX12TexturePixelScaler() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CDirectX12TexturePixelScaler::Reset() {
		if LSN_LIKELY( m_prTarget.get() && m_prTarget->Get() ) { m_prTarget->Reset(); }
		if LSN_LIKELY( m_prVbQuad.get() && m_prVbQuad->Get() ) { m_prVbQuad->Reset(); }
		if LSN_LIKELY( m_ppsShader.get() && m_ppsShader->Get() ) { m_ppsShader->Reset(); }
		if LSN_LIKELY( m_prsRootSignature.get() && m_prsRootSignature->Get() ) { m_prsRootSignature->Reset(); }
		
		m_prTarget.reset();
		m_prVbQuad.reset();
		m_ppsShader.reset();
		m_prsRootSignature.reset();
		m_dhSrvHeap.reset();
		m_dhRtvHeap.reset();
		m_dhSamplerHeap.reset();

		m_gShaderGamma = CNesPalette::LSN_G_NONE;
		m_ui32TargetW = 0;
		m_ui32TargetH = 0;
		m_fFormat = DXGI_FORMAT_UNKNOWN;
	}

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
	bool CDirectX12TexturePixelScaler::Render( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32ScaleX, uint32_t _ui32ScaleY, CNesPalette::LSN_GAMMA _gGamma, bool _bUse16BitTarget, bool _bFlipY, ID3DInclude * _piInclude ) {
		if LSN_UNLIKELY( !_pd12dDevice || !_pgclCommandList || !_prSrc || !_ui32SrcW || !_ui32SrcH || !_ui32ScaleX || !_ui32ScaleY ) { return false; }

		const uint32_t ui32DstW = _ui32SrcW * _ui32ScaleX;
		const uint32_t ui32DstH = _ui32SrcH * _ui32ScaleY;
		const DXGI_FORMAT fFormat = _bUse16BitTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;

		if LSN_UNLIKELY( !EnsureResources( _pd12dDevice, ui32DstW, ui32DstH, fFormat ) ) { return false; }
		if LSN_UNLIKELY( !EnsureShader( _pd12dDevice, _gGamma, fFormat, _piInclude ) ) { return false; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();
		ID3D12GraphicsCommandList * pCommandList = _pgclCommandList->Get();

		// Write Source SRV into local heap.
		D3D12_CPU_DESCRIPTOR_HANDLE hSrvCpu = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateShaderResourceView( _prSrc->Get(), nullptr, hSrvCpu );

		D3D12_RESOURCE_BARRIER rbBarriers[1];
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_prTarget->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
		pCommandList->ResourceBarrier( 1, rbBarriers );

		D3D12_CPU_DESCRIPTOR_HANDLE hRtvCpu = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pCommandList->OMSetRenderTargets( 1, &hRtvCpu, FALSE, nullptr );

		D3D12_VIEWPORT vpViewport = { 0.0f, 0.0f, static_cast<float>(ui32DstW), static_cast<float>(ui32DstH), 0.0f, 1.0f };
		D3D12_RECT rScissor = { 0, 0, static_cast<LONG>(ui32DstW), static_cast<LONG>(ui32DstH) };
		pCommandList->RSSetViewports( 1, &vpViewport );
		pCommandList->RSSetScissorRects( 1, &rScissor );

		pCommandList->SetPipelineState( m_ppsShader->Get() );
		pCommandList->SetGraphicsRootSignature( m_prsRootSignature->Get() );

		ID3D12DescriptorHeap * ppHeaps[] = { m_dhSrvHeap->Get(), m_dhSamplerHeap->Get() };
		pCommandList->SetDescriptorHeaps( 2, ppHeaps );

		float fVsConstants[4] = { static_cast<float>(ui32DstW), static_cast<float>(ui32DstH), 0.0f, 0.0f };
		pCommandList->SetGraphicsRoot32BitConstants( 0, 4, fVsConstants, 0 );

		// c0 = [srcH, 1/srcH, 0.5, 0] (Mimics D3D9 layout requirement).
		float fPsConstants[4] = { static_cast<float>(_ui32SrcH), 1.0f / static_cast<float>(_ui32SrcH), 0.5f, 0.0f };
		pCommandList->SetGraphicsRoot32BitConstants( 1, 4, fPsConstants, 0 );

		D3D12_GPU_DESCRIPTOR_HANDLE hSrvGpu = m_dhSrvHeap->Get()->GetGPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hSampGpu = m_dhSamplerHeap->Get()->GetGPUDescriptorHandleForHeapStart();
		pCommandList->SetGraphicsRootDescriptorTable( 2, hSrvGpu );
		pCommandList->SetGraphicsRootDescriptorTable( 3, hSampGpu );

		D3D12_RANGE rReadRange = { 0, 0 };
		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_prVbQuad->Get()->Map( 0, &rReadRange, reinterpret_cast<void **>(&pvP) ) ) ) {
			float fL = 0.0f;
			float fT = 0.0f;
			float fR = static_cast<float>(ui32DstW);
			float fB = static_cast<float>(ui32DstH);

			float fVTop = _bFlipY ? 1.0f : 0.0f;
			float fVBot = _bFlipY ? 0.0f : 1.0f;

			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, fVTop };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, fVTop };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, fVBot };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, fVBot };
			m_prVbQuad->Get()->Unmap( 0, nullptr );
		}

		pCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList->IASetVertexBuffers( 0, 1, &m_vbView );
		pCommandList->DrawInstanced( 4, 1, 0, 0 );

		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_prTarget->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		pCommandList->ResourceBarrier( 1, rbBarriers );

		return true;
	}

	/**
	 * Ensures the vertex buffer, descriptor heaps, and render target are created and properly sized.
	 * 
	 * \param _pd12dDevice The Direct3D 12 device.
	 * \param _ui32DstW The target width.
	 * \param _ui32DstH The target height.
	 * \param _fFormat The target format.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX12TexturePixelScaler::EnsureResources( CDirectX12Device * _pd12dDevice, uint32_t _ui32DstW, uint32_t _ui32DstH, DXGI_FORMAT _fFormat ) {
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

		if LSN_UNLIKELY( !m_dhRtvHeap.get() ) {
			m_dhRtvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
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

		if LSN_UNLIKELY( !m_prTarget.get() || !m_prTarget->Get() || m_ui32TargetW != _ui32DstW || m_ui32TargetH != _ui32DstH || m_fFormat != _fFormat ) {
			if LSN_LIKELY( m_prTarget.get() && m_prTarget->Get() ) { m_prTarget->Reset(); }
			m_prTarget = std::make_unique<CDirectX12Resource>();

			D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			D3D12_RESOURCE_DESC rdTex = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, _ui32DstW, _ui32DstH, 1, 1, _fFormat, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
			D3D12_CLEAR_VALUE cvClear = { _fFormat, { 0.0f, 0.0f, 0.0f, 1.0f } };
			if ( !m_prTarget->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear ) ) { return false; }

			pd12Device->CreateRenderTargetView( m_prTarget->Get(), nullptr, m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart() );
			
			m_ui32TargetW = _ui32DstW;
			m_ui32TargetH = _ui32DstH;
			m_fFormat = _fFormat;
		}

		return true;
	}

	/**
	 * Ensures the Pipeline State Object is compiled with the correct gamma function.
	 * 
	 * \param _pd12dDevice The Direct3D 12 device.
	 * \param _gGamma The gamma curve to apply.
	 * \param _fFormat The target format for the pipeline.
	 * \param _piInclude Optional #include handler for shader compilation.
	 * \return Returns true if the shader is ready.
	 **/
	bool CDirectX12TexturePixelScaler::EnsureShader( CDirectX12Device * _pd12dDevice, CNesPalette::LSN_GAMMA _gGamma, DXGI_FORMAT _fFormat, ID3DInclude * _piInclude ) {
		if LSN_LIKELY( m_ppsShader.get() && m_ppsShader->Get() && m_gShaderGamma == _gGamma ) { return true; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();
		m_ppsShader = std::make_unique<CDirectX12PipelineState>();
		m_gShaderGamma = _gGamma;

		if ( !m_prsRootSignature.get() ) {
			m_prsRootSignature = std::make_unique<CDirectX12RootSignature>();
			D3D12_DESCRIPTOR_RANGE drRanges[2];
			drRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drRanges[0].NumDescriptors = 1; drRanges[0].BaseShaderRegister = 0; drRanges[0].RegisterSpace = 0; drRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			drRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER; drRanges[1].NumDescriptors = 1; drRanges[1].BaseShaderRegister = 0; drRanges[1].RegisterSpace = 0; drRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER rpParameters[4];
			rpParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[0].Constants.ShaderRegister = 0; rpParameters[0].Constants.RegisterSpace = 0; rpParameters[0].Constants.Num32BitValues = 4; rpParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			
			rpParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[1].Constants.ShaderRegister = 1; rpParameters[1].Constants.RegisterSpace = 0; rpParameters[1].Constants.Num32BitValues = 4; rpParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

		std::string sGammaCall = "c.rgb";
		switch ( _gGamma ) {
			case CNesPalette::LSN_G_CRT1 :			{ sGammaCall = "CrtProperToLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_CRT2 :			{ sGammaCall = "CrtProper2ToLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_sRGB :			{ sGammaCall = "sRGBtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE170M :		{ sGammaCall = "SMPTE170MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_DCIP3 :			{ sGammaCall = "DCIP3toLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_ADOBERGB :		{ sGammaCall = "AdobeRGBtoLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE240M :		{ sGammaCall = "SMPTE240MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_POW_1_96 :		{ sGammaCall = "pow( c.rgb, 1.96 )"; break; }
			case CNesPalette::LSN_G_POW_2_0 :		{ sGammaCall = "pow( c.rgb, 2.0 )"; break; }
			case CNesPalette::LSN_G_POW_2_2 :		{ sGammaCall = "pow( c.rgb, 2.22222222222222232090871330001391470432281494140625 )"; break; }
			case CNesPalette::LSN_G_POW_2_35 :		{ sGammaCall = "pow( c.rgb, 2.35 )"; break; }
			case CNesPalette::LSN_G_POW_2_4 :		{ sGammaCall = "pow( c.rgb, 2.4 )"; break; }
			case CNesPalette::LSN_G_POW_2_5 :		{ sGammaCall = "pow( c.rgb, 2.5 )"; break; }
			case CNesPalette::LSN_G_POW_2_7 :		{ sGammaCall = "pow( c.rgb, 2.7 )"; break; }
			case CNesPalette::LSN_G_POW_2_8 :		{ sGammaCall = "pow( c.rgb, 2.8 )"; break; }
			default: break;
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

		std::string sPsHlsl =
			"#include \"LSNGamma.hlsl\"\n"
			"Texture2D tSrc : register(t0);\n"
			"SamplerState sPoint : register(s0);\n"
			"cbuffer PSConstants : register(b1) { float4 c0; };\n" // c0 = [srcH, 1/srcH, 0.5, 0]
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"    float v = (floor(input.Tex.y * c0.x) + c0.z) * c0.y;\n"
			"    float4 c = tSrc.Sample(sPoint, float2(input.Tex.x, v));\n"
			"    c.rgb = " + sGammaCall + ";\n"
			"    return c;\n"
			"}\n";

		CDirectX12DiskInclude diDefaultInclude( CDirectX12DiskInclude::GetExeShadersDir() );
		ID3DInclude * pInc = _piInclude ? _piInclude : &diDefaultInclude;

		std::vector<uint8_t> vBcVs, vBcPs;
		if ( !CompileHlsl( _pd12dDevice, kVsHlsl, "main", "vs_5_0", vBcVs, pInc ) ||
			 !CompileHlsl( _pd12dDevice, sPsHlsl.c_str(), "main", "ps_5_0", vBcPs, pInc ) ) {
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC iedInputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdDesc = {};
		gpsdDesc.pRootSignature = m_prsRootSignature->Get();
		gpsdDesc.VS = { vBcVs.data(), vBcVs.size() };
		gpsdDesc.PS = { vBcPs.data(), vBcPs.size() };
		gpsdDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		gpsdDesc.SampleMask = UINT_MAX;
		gpsdDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		gpsdDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gpsdDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpsdDesc.NumRenderTargets = 1;
		gpsdDesc.RTVFormats[0] = _fFormat;
		gpsdDesc.SampleDesc.Count = 1;
		gpsdDesc.InputLayout = { iedInputLayout, 2 };

		if ( !m_ppsShader->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

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
	bool CDirectX12TexturePixelScaler::CompileHlsl( CDirectX12Device * /*_pd12dDevice*/, const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<uint8_t> &_vOutByteCode, ID3DInclude * _piInclude ) {
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
