#ifdef LSN_DX12

#include "LSNDx12PaletteFilter.h"

namespace lsn {

	CDx12PaletteFilter::CDx12PaletteFilter() {
	}
	CDx12PaletteFilter::~CDx12PaletteFilter() {
	}

	// == Functions.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx12PaletteFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( m_ui32OutputWidth * sizeof( uint32_t ) );

		ReleaseSizeDependents();
		return CParent::Init( _stBuffers, _ui16Width, _ui16Height );
	}

	/**
	 * Sets the palette.
	 * 
	 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
	 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.
	 **/
	bool CDx12PaletteFilter::SetLut( const float * _pfRgba512 ) {
		if ( !_pfRgba512 ) { return false; }
		try {
			m_vLut.resize( 4 * 512 );
			std::memcpy( m_vLut.data(), _pfRgba512, sizeof( float ) * 4 * 512 );
			m_bUpdatePalette = true;
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx12PaletteFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
		EnsureShaders();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx12PaletteFilter::DeActivate() {
		CParent::DeActivate();

		m_tpsScaler.Reset();
		m_rsResampler.Reset();
		m_trRenderer.Reset();

		m_tIndex.reset();
		m_tLut.reset();
		m_rtInitial.reset();
		m_rtResampled.reset();
		m_vbPass1.reset();
		
		m_rIndexUpload.reset();
		m_rLutUpload.reset();
		m_dhSrvHeap.reset();
		m_dhRtvHeap.reset();
		m_dhSamplerHeap.reset();

		m_psoIdxToColor.reset();
		m_rsRootSignature.reset();

		m_gclCommandList.reset();
		m_caAllocator.reset();

		m_bUpdatePalette = true;

		if ( m_pdx12dDevice ) {
			s_dgsState.DestroyDx12();
			m_pdx12dDevice = nullptr;
		}
	}

	/**
	 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.
	 *
	 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.  Its format will be that returned in InputFormat().
	 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
	 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
	 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
	 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
	 * \param _ui64PpuFrame The PPU frame associated with the input data.
	 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
	 * \param _i32DispLeft The display area left.
	 * \param _i32DispTop The display area top.
	 * \param _ui32DispWidth The display area width.
	 * \param _ui32DispHeight The display area height
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CDx12PaletteFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
			m_ui32SrcW = _ui32Width;
			m_ui32SrcH = _ui32Height;
			EnsureSizeAndResources();
			EnsureShaders();
			m_vOutputBuffer.resize( _ui32Width * _ui32Height * sizeof( uint32_t ) );
		}
		if LSN_UNLIKELY( m_bUpdatePalette ) {
			UpdateLut();
		}
		if LSN_UNLIKELY( !UploadIndices( reinterpret_cast<const uint16_t *>(_pui8Input), _ui32Width, _ui32Height, _ui32Stride ) ) {
			m_bValidState = false;
		}

		lsw::LSW_RECT rRect;
		rRect.left = LONG( _i32DispLeft );
		rRect.top = LONG( _i32DispTop );
		rRect.right = rRect.left + LONG( _ui32DispWidth );
		rRect.bottom = rRect.top + LONG( _ui32DispHeight );
		Render( rRect );

		_ui32Width = uint32_t( s_dgsState.rScreenRect.Width() );
		_ui32Height = uint32_t( s_dgsState.rScreenRect.Height() );
		_ui32Stride = _ui32Width * sizeof( uint32_t );
		return m_vOutputBuffer.data();
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CDx12PaletteFilter::FrameResize() {
		OnSizeDx12();

		EnsureSizeAndResources();
		EnsureShaders();
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * 
	 * \return Returns true on success.
	 */
	bool CDx12PaletteFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx12dDevice ) {
			if ( !s_dgsState.CreateDx12() ) { return false; }
			m_pdx12dDevice = &s_dgsState.dx12Device;
			m_bUpdatePalette = true;
			m_tpsScaler.Reset();
			m_rsResampler.Reset();
			m_trRenderer.Reset();
		}

		ID3D12Device * pd12Device = m_pdx12dDevice->GetDevice();
		if ( !pd12Device ) { return false; }

		m_uiSrvDescriptorSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		m_uiRtvDescriptorSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
		m_uiSamplerDescriptorSize = pd12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );

		if LSN_UNLIKELY( !m_caAllocator.get() ) {
			m_caAllocator = std::make_unique<CDirectX12CommandAllocator>();
			if ( !m_caAllocator->CreateCommandAllocator( pd12Device, D3D12_COMMAND_LIST_TYPE_DIRECT ) ) { return false; }
		}
		if LSN_UNLIKELY( !m_gclCommandList.get() ) {
			m_gclCommandList = std::make_unique<CDirectX12GraphicsCommandList>();
			if ( !m_gclCommandList->CreateCommandList( pd12Device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_caAllocator->Get(), nullptr ) ) { return false; }
			m_gclCommandList->Get()->Close();
		}

		// Ensure Descriptors Heaps. 
		if LSN_UNLIKELY( !m_dhSrvHeap.get() ) {
			m_dhSrvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSrvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}
		if LSN_UNLIKELY( !m_dhRtvHeap.get() ) {
			m_dhRtvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			// Capacity 3: [0: Initial RT, 1: BackBuffer, 2: Resampled Target].
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
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

			D3D12_CPU_DESCRIPTOR_HANDLE hSampler = m_dhSamplerHeap->Get()->GetCPUDescriptorHandleForHeapStart();
			pd12Device->CreateSampler( &sdPoint, hSampler );
		}

		if LSN_UNLIKELY( !m_tIndex.get() ) { m_tIndex = std::make_unique<CDirectX12Texture>(); }
		if LSN_UNLIKELY( !m_tLut.get() ) { m_tLut = std::make_unique<CDirectX12Texture>(); }
		if LSN_UNLIKELY( !m_rtInitial.get() ) { m_rtInitial = std::make_unique<CDirectX12Resource>(); }
		if LSN_UNLIKELY( !m_vbPass1.get() ) { m_vbPass1 = std::make_unique<CDirectX12Resource>(); }
		
		if LSN_UNLIKELY( !m_rIndexUpload.get() ) { m_rIndexUpload = std::make_unique<CDirectX12Resource>(); }
		if LSN_UNLIKELY( !m_rLutUpload.get() ) { m_rLutUpload = std::make_unique<CDirectX12Resource>(); }
		
		bool bOk = (m_ui32RsrcW == m_ui32SrcW) && (m_ui32RsrcH == m_ui32SrcH) && m_tIndex->Get() && m_rtInitial->Get() && m_vbPass1->Get();

		if ( bOk ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
		D3D12_HEAP_PROPERTIES hpUpload = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
		

		D3D12_RESOURCE_DESC rdIndex = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, m_ui32SrcW, m_ui32SrcH, 1, 1, DXGI_FORMAT_R16_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE };
		if ( !m_tIndex->CreateCommittedTexture( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdIndex, D3D12_RESOURCE_STATE_COPY_DEST ) ) { return false; }
		

		UINT64 ui64IndexSize = 0;
		pd12Device->GetCopyableFootprints( &rdIndex, 0, 1, 0, nullptr, nullptr, nullptr, &ui64IndexSize );
		D3D12_RESOURCE_DESC rdIdxUpload = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, ui64IndexSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
		if ( !m_rIndexUpload->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdIdxUpload, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }


		DXGI_FORMAT fmtRt = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
		D3D12_RESOURCE_DESC rdInitial = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, m_ui32SrcW, m_ui32SrcH, 1, 1, fmtRt, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
		D3D12_CLEAR_VALUE cvClear = { fmtRt, { 0.0f, 0.0f, 0.0f, 1.0f } };
		if ( !m_rtInitial->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdInitial, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear ) ) { return false; }


		D3D12_CPU_DESCRIPTOR_HANDLE hSrv = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateShaderResourceView( m_tIndex->Get(), nullptr, hSrv );
		hSrv.ptr += m_uiSrvDescriptorSize; // Slot 1 reserved for LUT.

		hSrv.ptr += m_uiSrvDescriptorSize; // Slot 2 = InitialRT.
		pd12Device->CreateShaderResourceView( m_rtInitial->Get(), nullptr, hSrv );

		D3D12_CPU_DESCRIPTOR_HANDLE hRtv = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateRenderTargetView( m_rtInitial->Get(), nullptr, hRtv ); // Slot 0.

		D3D12_RESOURCE_DESC rdVb = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, sizeof( LSN_XYZRHWTEX1 ) * 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
		
		if ( !m_vbPass1->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdVb, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }
		m_vbView1.BufferLocation = m_vbPass1->Get()->GetGPUVirtualAddress();
		m_vbView1.StrideInBytes = sizeof( LSN_XYZRHWTEX1 );
		m_vbView1.SizeInBytes = sizeof( LSN_XYZRHWTEX1 ) * 4;

		if ( !UpdateLut() ) { return false; }

		m_bValidState = true;
		m_ui32RsrcW = m_ui32SrcW;
		m_ui32RsrcH = m_ui32SrcH;
		return true;
	}

	/**
	 * \brief Updates the 512-entry float RGBA LUT in the upload heap.
	 * 
	 * \return Returns true on success.
	 */
	bool CDx12PaletteFilter::UpdateLut() {
		if LSN_UNLIKELY( !m_pdx12dDevice || !m_ui32SrcW || !m_ui32SrcH ) { return false; }
		ID3D12Device * pd12Device = m_pdx12dDevice->GetDevice();

		if LSN_UNLIKELY( !m_tLut->Get() ) {
			D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			D3D12_HEAP_PROPERTIES hpUpload = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			
			D3D12_RESOURCE_DESC rdLut = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 512, 1, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE };
			if ( !m_tLut->CreateCommittedTexture( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdLut, D3D12_RESOURCE_STATE_COPY_DEST ) ) { return false; }

			UINT64 ui64LutSize = 0;
			pd12Device->GetCopyableFootprints( &rdLut, 0, 1, 0, nullptr, nullptr, nullptr, &ui64LutSize );
			D3D12_RESOURCE_DESC rdUpload = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, ui64LutSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
			if ( !m_rLutUpload->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdUpload, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }

			D3D12_CPU_DESCRIPTOR_HANDLE hSrv = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
			hSrv.ptr += m_uiSrvDescriptorSize;	// Slot 1.
			pd12Device->CreateShaderResourceView( m_tLut->Get(), nullptr, hSrv );
		}

		if LSN_UNLIKELY( m_vLut.size() && m_bUpdatePalette ) {
			D3D12_RANGE rReadRange = { 0, 0 };
			void * pvData = nullptr;
			if ( SUCCEEDED( m_rLutUpload->Get()->Map( 0, &rReadRange, &pvData ) ) ) {
				std::memcpy( pvData, m_vLut.data(), sizeof( float ) * 4 * 512 );
				m_rLutUpload->Get()->Unmap( 0, nullptr );
				m_bUpdatePalette = false;
			}
		}
		return true;
	}

	/**
	 * \brief Uploads the 16-bit PPU indices to the upload heap.
	 * 
	 * \param _pui16Idx Source pointer to the index image (row-major).
	 * \param _ui32W Image width in pixels.
	 * \param _ui32H Image height in pixels.
	 * \param _ui32SrcPitch Source pitch in bytes; pass 0 for tightly packed.
	 * \return Returns true on success.
	 */
	bool CDx12PaletteFilter::UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch ) {
		if LSN_UNLIKELY( !m_pdx12dDevice || _ui32W != m_ui32SrcW || _ui32H != m_ui32SrcH || !m_tIndex->Get() || !m_rIndexUpload->Get() ) { return false; }
		if ( !_ui32SrcPitch ) { _ui32SrcPitch = _ui32W * sizeof( uint16_t ); }

		D3D12_RANGE rReadRange = { 0, 0 };
		void * pvData = nullptr;
		if LSN_UNLIKELY( FAILED( m_rIndexUpload->Get()->Map( 0, &rReadRange, &pvData ) ) ) { return false; }

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT psfFootprint;
		D3D12_RESOURCE_DESC rdDesc = m_tIndex->Get()->GetDesc();
		m_pdx12dDevice->GetDevice()->GetCopyableFootprints( &rdDesc, 0, 1, 0, &psfFootprint, nullptr, nullptr, nullptr );
		const UINT uiPitch = psfFootprint.Footprint.RowPitch;

		if LSN_LIKELY( uiPitch == _ui32SrcPitch ) {
			std::memcpy( pvData, _pui16Idx, _ui32H * _ui32SrcPitch );
		}
		else {
			for ( uint32_t Y = 0; Y < _ui32H; ++Y ) {
				const uint8_t * pSrcRow = reinterpret_cast<const uint8_t *>(_pui16Idx) + Y * _ui32SrcPitch;
				uint8_t * pDstRow = reinterpret_cast<uint8_t *>(pvData) + Y * uiPitch;
				std::memcpy( pDstRow, pSrcRow, _ui32W * sizeof( uint16_t ) );
			}
		}
		m_rIndexUpload->Get()->Unmap( 0, nullptr );
		return true;
	}

	/**
	 * \brief Ensures pixel shaders, vertex shaders, and PSOs are created.
	 * 
	 * \return Returns true if all shaders are ready.
	 */
	bool CDx12PaletteFilter::EnsureShaders() {
		if ( !m_pdx12dDevice || !m_pdx12dDevice->GetDevice() ) { return false; }
		ID3D12Device * pd12Device = m_pdx12dDevice->GetDevice();

		if ( !m_rsRootSignature.get() ) {
			m_rsRootSignature = std::make_unique<CDirectX12RootSignature>();
			D3D12_DESCRIPTOR_RANGE drRanges[3];
			drRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drRanges[0].NumDescriptors = 1; drRanges[0].BaseShaderRegister = 0; drRanges[0].RegisterSpace = 0; drRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			drRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drRanges[1].NumDescriptors = 1; drRanges[1].BaseShaderRegister = 1; drRanges[1].RegisterSpace = 0; drRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			drRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER; drRanges[2].NumDescriptors = 1; drRanges[2].BaseShaderRegister = 0; drRanges[2].RegisterSpace = 0; drRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER rpParameters[4];
			rpParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[0].Constants.ShaderRegister = 0; rpParameters[0].Constants.RegisterSpace = 0; rpParameters[0].Constants.Num32BitValues = 4; rpParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			
			rpParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[1].DescriptorTable.NumDescriptorRanges = 1; rpParameters[1].DescriptorTable.pDescriptorRanges = &drRanges[0]; rpParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			
			rpParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[2].DescriptorTable.NumDescriptorRanges = 1; rpParameters[2].DescriptorTable.pDescriptorRanges = &drRanges[1]; rpParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			
			rpParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rpParameters[3].DescriptorTable.NumDescriptorRanges = 1; rpParameters[3].DescriptorTable.pDescriptorRanges = &drRanges[2]; rpParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_ROOT_SIGNATURE_DESC rsdDesc = { 4, rpParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };

			Microsoft::WRL::ComPtr<ID3DBlob> pbSig, pbErr;
			typedef HRESULT( WINAPI * PFN_D3D12SerializeRootSignature )( const D3D12_ROOT_SIGNATURE_DESC *, D3D_ROOT_SIGNATURE_VERSION, ID3DBlob **, ID3DBlob ** );
			PFN_D3D12SerializeRootSignature pfnSer = reinterpret_cast<PFN_D3D12SerializeRootSignature>(::GetProcAddress( m_pdx12dDevice->Dll().hHandle, "D3D12SerializeRootSignature" ));
			
			if ( !pfnSer || FAILED( pfnSer( &rsdDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pbSig, &pbErr ) ) ) { return false; }
			if ( !m_rsRootSignature->CreateRootSignature( pd12Device, 0, pbSig->GetBufferPointer(), pbSig->GetBufferSize() ) ) { return false; }
		}

		static const char * kVsHlsl =
			"cbuffer RootConstants : register(b0) {\n"
			"    float2 TargetSize;\n"
			"    float2 Padding;\n"
			"};\n"
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

		static const char * kPsIdxToColorHlsl =
			"Texture2D tIdx : register(t0);\n"
			"Texture2D tLut : register(t1);\n"
			"SamplerState sPoint : register(s0);\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"    float raw = tIdx.Sample(sPoint, input.Tex).r * 65535.0;\n"
			"    float idx = clamp(floor(raw + 0.5), 0.0, 511.0);\n"
			"    float u = (idx + 0.5) / 512.0;\n"
			"    return tLut.Sample(sPoint, float2(u, 0.5));\n"
			"}\n";

		std::vector<uint8_t> vBcVs, vBcIdx;
		if ( !CompileHlsl( kVsHlsl, "main", "vs_5_0", vBcVs ) ||
			 !CompileHlsl( kPsIdxToColorHlsl, "main", "ps_5_0", vBcIdx ) ) {
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC iedInputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		if ( !m_psoIdxToColor.get() ) { m_psoIdxToColor = std::make_unique<CDirectX12PipelineState>(); }

		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsdDesc = {};
		gpsdDesc.pRootSignature = m_rsRootSignature->Get();
		gpsdDesc.VS = { vBcVs.data(), vBcVs.size() };
		gpsdDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		gpsdDesc.SampleMask = UINT_MAX;
		gpsdDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		gpsdDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gpsdDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpsdDesc.NumRenderTargets = 1;
		gpsdDesc.SampleDesc.Count = 1;
		gpsdDesc.InputLayout = { iedInputLayout, 2 };


		gpsdDesc.PS = { vBcIdx.data(), vBcIdx.size() };
		gpsdDesc.RTVFormats[0] = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
		if ( !m_psoIdxToColor->Get() && !m_psoIdxToColor->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

		return true;
	}

	/**
	 * \brief Compiles an HLSL shader using dynamically loaded d3dcompiler_47.dll.
	 *
	 * \param _pcszSource Null-terminated HLSL source code.
	 * \param _pcszEntry Null-terminated entry-point function name (e.g., "main").
	 * \param _pcszProfile Null-terminated profile (e.g., "ps_5_0").
	 * \param _vOutByteCode Output vector to receive the compiled bytecode (DWORD stream).
	 * \return Returns true if compilation succeeded and bytecode was produced.
	 */
	bool CDx12PaletteFilter::CompileHlsl( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<uint8_t> &_vOutByteCode ) {
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
			nullptr,
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

	/**
	 * \brief Releases size-dependent resources.
	 */
	void CDx12PaletteFilter::ReleaseSizeDependents() {
		if LSN_LIKELY( m_tIndex.get() && m_tIndex->Get() ) { m_tIndex->Reset(); }
		if LSN_LIKELY( m_rtInitial.get() && m_rtInitial->Get() ) { m_rtInitial->Reset(); }
		if LSN_LIKELY( m_rtResampled.get() && m_rtResampled->Get() ) { m_rtResampled->Reset(); }
		
		if LSN_LIKELY( m_vbPass1.get() && m_vbPass1->Get() ) { m_vbPass1->Reset(); }
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * \brief Renders the three-pass pipeline to the backbuffer with a black border outside _rOutput.
	 * 
	 * \param _rOutput The destination rectangle in client pixels where the NES image should appear.
	 * \return Returns true if the draw succeeded; false on failure.
	 */
	bool CDx12PaletteFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_gclCommandList.get() || !m_pdx12dDevice || !m_pdx12dDevice->GetSwapChain() || !m_psoIdxToColor->Get() ) { return false; }

		m_caAllocator->Get()->Reset();
		m_gclCommandList->Get()->Reset( m_caAllocator->Get(), nullptr );


		{
			D3D12_TEXTURE_COPY_LOCATION tclDestIdx = { m_tIndex->Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, { 0 } };
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT psfIdxFootprint;
			D3D12_RESOURCE_DESC rdIdxDesc = m_tIndex->Get()->GetDesc();
			m_pdx12dDevice->GetDevice()->GetCopyableFootprints( &rdIdxDesc, 0, 1, 0, &psfIdxFootprint, nullptr, nullptr, nullptr );
			D3D12_TEXTURE_COPY_LOCATION tclSrcIdx = { m_rIndexUpload->Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, { psfIdxFootprint } };
			m_gclCommandList->Get()->CopyTextureRegion( &tclDestIdx, 0, 0, 0, &tclSrcIdx, nullptr );

			D3D12_TEXTURE_COPY_LOCATION tclDestLut = { m_tLut->Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, { 0 } };
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT psfLutFootprint;
			D3D12_RESOURCE_DESC rdLutDesc = m_tLut->Get()->GetDesc();
			m_pdx12dDevice->GetDevice()->GetCopyableFootprints( &rdLutDesc, 0, 1, 0, &psfLutFootprint, nullptr, nullptr, nullptr );
			D3D12_TEXTURE_COPY_LOCATION tclSrcLut = { m_rLutUpload->Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, { psfLutFootprint } };
			m_gclCommandList->Get()->CopyTextureRegion( &tclDestLut, 0, 0, 0, &tclSrcLut, nullptr );
		}


		D3D12_RESOURCE_BARRIER rbBarriers[3];
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tIndex->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		rbBarriers[1] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tLut->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		rbBarriers[2] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtInitial->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
		m_gclCommandList->Get()->ResourceBarrier( 3, rbBarriers );


		m_gclCommandList->Get()->SetGraphicsRootSignature( m_rsRootSignature->Get() );
		ID3D12DescriptorHeap * ppHeaps[] = { m_dhSrvHeap->Get(), m_dhSamplerHeap->Get() };
		m_gclCommandList->Get()->SetDescriptorHeaps( 2, ppHeaps );

		D3D12_CPU_DESCRIPTOR_HANDLE hRtvStart = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hSrvGpu = m_dhSrvHeap->Get()->GetGPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hSampGpu = m_dhSamplerHeap->Get()->GetGPUDescriptorHandleForHeapStart();



		m_gclCommandList->Get()->SetPipelineState( m_psoIdxToColor->Get() );
		m_gclCommandList->Get()->OMSetRenderTargets( 1, &hRtvStart, FALSE, nullptr );
		
		D3D12_VIEWPORT vp1 = { 0.0f, 0.0f, static_cast<float>(m_ui32SrcW), static_cast<float>(m_ui32SrcH), 0.0f, 1.0f };
		D3D12_RECT rScissor1 = { 0, 0, static_cast<LONG>(m_ui32SrcW), static_cast<LONG>(m_ui32SrcH) };
		m_gclCommandList->Get()->RSSetViewports( 1, &vp1 );
		m_gclCommandList->Get()->RSSetScissorRects( 1, &rScissor1 );

		float fConstants1[4] = { static_cast<float>(m_ui32SrcW), static_cast<float>(m_ui32SrcH), 0.0f, 0.0f };
		m_gclCommandList->Get()->SetGraphicsRoot32BitConstants( 0, 4, fConstants1, 0 );

		D3D12_GPU_DESCRIPTOR_HANDLE hLutSrv = hSrvGpu; hLutSrv.ptr += m_uiSrvDescriptorSize;
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 1, hSrvGpu );
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 2, hLutSrv );
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 3, hSampGpu );

		D3D12_RANGE rReadRange = { 0, 0 };
		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_vbPass1->Get()->Map( 0, &rReadRange, reinterpret_cast<void **>(&pvP) ) ) ) {
			float fL = 0.0f;
			float fT = 0.0f;
			float fR = static_cast<float>(m_ui32SrcW);
			float fB = static_cast<float>(m_ui32SrcH);
			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, 0.0f };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, 0.0f };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, 1.0f };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, 1.0f };
			m_vbPass1->Get()->Unmap( 0, nullptr );
		}

		m_gclCommandList->Get()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		m_gclCommandList->Get()->IASetVertexBuffers( 0, 1, &m_vbView1 );
		m_gclCommandList->Get()->DrawInstanced( 4, 1, 0, 0 );

		D3D12_RESOURCE_BARRIER rbInitialToSrv[1];
		rbInitialToSrv[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtInitial->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		m_gclCommandList->Get()->ResourceBarrier( 1, rbInitialToSrv );

		if ( !m_tpsScaler.Render( m_pdx12dDevice, m_gclCommandList.get(), m_rtInitial.get(), m_ui32SrcW, m_ui32SrcH, GetActualHorSharpness(), GetActualVertSharpness(), CNesPalette::LSN_G_NONE, m_bUse16BitInitialTarget, true ) ) {
			return false;
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> rBackBuffer;
		if LSN_LIKELY( SUCCEEDED( m_pdx12dDevice->GetSwapChain()->GetBuffer( m_pdx12dDevice->GetSwapChain()->GetCurrentBackBufferIndex(), IID_PPV_ARGS( &rBackBuffer ) ) ) ) {
			D3D12_RESOURCE_BARRIER rbBack[1];
			rbBack[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { rBackBuffer.Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET } };
			m_gclCommandList->Get()->ResourceBarrier( 1, rbBack );

			D3D12_CPU_DESCRIPTOR_HANDLE hBackRtv = hRtvStart; hBackRtv.ptr += m_uiRtvDescriptorSize;
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			m_pdx12dDevice->GetDevice()->CreateRenderTargetView( rBackBuffer.Get(), &rtvDesc, hBackRtv );

			if ( m_bUseHighQualityResampler ) {
				uint32_t ui32DstW = static_cast<uint32_t>(_rOutput.Width());
				uint32_t ui32DstH = static_cast<uint32_t>(_rOutput.Height());

				m_rsResampler.SetFilter( GetPreferredConvolutionFilter( ui32DstW, ui32DstH ) );
				
				if LSN_UNLIKELY( !m_rtResampled.get() || !m_rtResampled->Get() || m_ui32ResampledTargetW != ui32DstW || m_ui32ResampledTargetH != ui32DstH ) {
					if LSN_LIKELY( m_rtResampled.get() && m_rtResampled->Get() ) { m_rtResampled->Reset(); }
					else if LSN_UNLIKELY( !m_rtResampled.get() ) { m_rtResampled = std::make_unique<CDirectX12Resource>(); }

					D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
					DXGI_FORMAT fmtRt = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
					D3D12_RESOURCE_DESC rdResampled = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, ui32DstW, ui32DstH, 1, 1, fmtRt, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
					D3D12_CLEAR_VALUE cvClear = { fmtRt, { 0.0f, 0.0f, 0.0f, 1.0f } };
					
					m_rtResampled->CreateCommittedResource( m_pdx12dDevice->GetDevice(), &hpDefault, D3D12_HEAP_FLAG_NONE, &rdResampled, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear );
					
					m_ui32ResampledTargetW = ui32DstW;
					m_ui32ResampledTargetH = ui32DstH;

					D3D12_CPU_DESCRIPTOR_HANDLE hResampledRtv = hRtvStart; hResampledRtv.ptr += m_uiRtvDescriptorSize * 2;
					m_pdx12dDevice->GetDevice()->CreateRenderTargetView( m_rtResampled->Get(), nullptr, hResampledRtv );
				}

				D3D12_CPU_DESCRIPTOR_HANDLE hResampledRtv = hRtvStart; hResampledRtv.ptr += m_uiRtvDescriptorSize * 2;

				D3D12_RESOURCE_BARRIER rbResampled[1];
				rbResampled[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtResampled->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
				m_gclCommandList->Get()->ResourceBarrier( 1, rbResampled );

				if ( m_rsResampler.Render( m_pdx12dDevice, m_gclCommandList.get(), m_tpsScaler.GetTexture(), m_tpsScaler.GetWidth(), m_tpsScaler.GetHeight(), m_rtResampled->Get(), hResampledRtv, ui32DstW, ui32DstH ) ) {
					rbResampled[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtResampled->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
					m_gclCommandList->Get()->ResourceBarrier( 1, rbResampled );

					m_trRenderer.Render( m_pdx12dDevice, m_gclCommandList.get(), m_rtResampled.get(), rBackBuffer.Get(), hBackRtv, _rOutput, 1.0f, false, true );
				}
				else {
					// Fallback to scaler if the 2-pass resampler aborts.
					rbResampled[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtResampled->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
					m_gclCommandList->Get()->ResourceBarrier( 1, rbResampled );

					m_trRenderer.Render( m_pdx12dDevice, m_gclCommandList.get(), m_tpsScaler.GetTexture(), rBackBuffer.Get(), hBackRtv, _rOutput, 1.0f, false, true );
				}
			}
			else {
				m_trRenderer.Render( m_pdx12dDevice, m_gclCommandList.get(), m_tpsScaler.GetTexture(), rBackBuffer.Get(), hBackRtv, _rOutput, 1.0f, false, true );
			}

			D3D12_RESOURCE_BARRIER rbPresent[1];
			rbPresent[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { rBackBuffer.Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT } };
			m_gclCommandList->Get()->ResourceBarrier( 1, rbPresent );
		}

		D3D12_RESOURCE_BARRIER rbCleanup[2];
		rbCleanup[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tIndex->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST } };
		rbCleanup[1] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tLut->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST } };
		m_gclCommandList->Get()->ResourceBarrier( 2, rbCleanup );

		m_gclCommandList->Get()->Close();

		ID3D12CommandList * ppCommandLists[] = { m_gclCommandList->Get() };
		m_pdx12dDevice->GetCommandQueue()->ExecuteCommandLists( 1, ppCommandLists );

		m_pdx12dDevice->FlushCommandQueue();

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
