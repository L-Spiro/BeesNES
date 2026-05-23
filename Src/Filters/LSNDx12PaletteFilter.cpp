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

		m_tIndex.reset();
		m_tLut.reset();
		m_rtInitial.reset();
		m_rtScanlined.reset();
		m_vbQuad.reset();
		
		m_rIndexUpload.reset();
		m_rLutUpload.reset();
		m_dhSrvHeap.reset();
		m_dhRtvHeap.reset();
		m_dhSamplerHeap.reset();

		m_psoIdxToColor.reset();
		m_psoVerticalNN.reset();
		m_psoCopy.reset();
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
		// SRV: 0=Index, 1=LUT, 2=InitialRT, 3=ScanlinedRT
		if LSN_UNLIKELY( !m_dhSrvHeap.get() ) {
			m_dhSrvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSrvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}
		// RTV: 0=InitialRT, 1=ScanlinedRT, 2=Backbuffer
		if LSN_UNLIKELY( !m_dhRtvHeap.get() ) {
			m_dhRtvHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 };
			if ( !m_dhRtvHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }
		}
		// Samplers: 0=Point, 1=Linear
		if LSN_UNLIKELY( !m_dhSamplerHeap.get() ) {
			m_dhSamplerHeap = std::make_unique<CDirectX12DescriptorHeap>();
			D3D12_DESCRIPTOR_HEAP_DESC dhdDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
			if ( !m_dhSamplerHeap->CreateDescriptorHeap( pd12Device, &dhdDesc ) ) { return false; }

			D3D12_SAMPLER_DESC sdPoint = {};
			sdPoint.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			sdPoint.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sdPoint.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sdPoint.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

			D3D12_SAMPLER_DESC sdLinear = sdPoint;
			sdLinear.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

			D3D12_CPU_DESCRIPTOR_HANDLE hSampler = m_dhSamplerHeap->Get()->GetCPUDescriptorHandleForHeapStart();
			pd12Device->CreateSampler( &sdPoint, hSampler );
			hSampler.ptr += m_uiSamplerDescriptorSize;
			pd12Device->CreateSampler( &sdLinear, hSampler );
		}

		if LSN_UNLIKELY( !m_tIndex.get() ) { m_tIndex = std::make_unique<CDirectX12Texture>(); }
		if LSN_UNLIKELY( !m_tLut.get() ) { m_tLut = std::make_unique<CDirectX12Texture>(); }
		if LSN_UNLIKELY( !m_rtInitial.get() ) { m_rtInitial = std::make_unique<CDirectX12Resource>(); }
		if LSN_UNLIKELY( !m_rtScanlined.get() ) { m_rtScanlined = std::make_unique<CDirectX12Resource>(); }
		if LSN_UNLIKELY( !m_vbQuad.get() ) { m_vbQuad = std::make_unique<CDirectX12Resource>(); }
		if LSN_UNLIKELY( !m_rIndexUpload.get() ) { m_rIndexUpload = std::make_unique<CDirectX12Resource>(); }
		if LSN_UNLIKELY( !m_rLutUpload.get() ) { m_rLutUpload = std::make_unique<CDirectX12Resource>(); }
		
		const uint32_t ui32ScanW = m_ui32SrcW * GetActualHorSharpness();
		const uint32_t ui32ScanH = m_ui32SrcH * GetActualVertSharpness();
		if ( !ui32ScanW || !ui32ScanH ) { return false; }
		const bool bOk = (m_ui32RsrcW == m_ui32SrcW) && (m_ui32RsrcH == m_ui32SrcH) && m_tIndex->Get() && m_rtInitial->Get() && m_rtScanlined->Get() && m_vbQuad->Get();

		if ( bOk ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
		D3D12_HEAP_PROPERTIES hpUpload = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
		
		// 1. Index Texture: R16_UNORM.
		D3D12_RESOURCE_DESC rdIndex = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, m_ui32SrcW, m_ui32SrcH, 1, 1, DXGI_FORMAT_R16_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE };
		if ( !m_tIndex->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdIndex, D3D12_RESOURCE_STATE_COPY_DEST ) ) { return false; }
		
		// Upload buffer for Index.
		UINT64 ui64IndexSize = 0;
		pd12Device->GetCopyableFootprints( &rdIndex, 0, 1, 0, nullptr, nullptr, nullptr, &ui64IndexSize );
		D3D12_RESOURCE_DESC rdIdxUpload = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, ui64IndexSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
		if ( !m_rIndexUpload->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdIdxUpload, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }

		// 2. Initial RT.
		DXGI_FORMAT fmtRt = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
		D3D12_RESOURCE_DESC rdInitial = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, m_ui32SrcW, m_ui32SrcH, 1, 1, fmtRt, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
		D3D12_CLEAR_VALUE cvClear = { fmtRt, { 0.0f, 0.0f, 0.0f, 1.0f } };
		if ( !m_rtInitial->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdInitial, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear ) ) { return false; }

		// 3. Scanlined RT.
		D3D12_RESOURCE_DESC rdScan = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, ui32ScanW, ui32ScanH, 1, 1, fmtRt, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
		if ( !m_rtScanlined->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdScan, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &cvClear ) ) { return false; }

		// Create SRVs and RTVs in heaps.
		D3D12_CPU_DESCRIPTOR_HANDLE hSrv = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateShaderResourceView( m_tIndex->Get(), nullptr, hSrv );
		hSrv.ptr += m_uiSrvDescriptorSize; // Slot 1 reserved for LUT

		hSrv.ptr += m_uiSrvDescriptorSize; // Slot 2 = InitialRT
		pd12Device->CreateShaderResourceView( m_rtInitial->Get(), nullptr, hSrv );

		hSrv.ptr += m_uiSrvDescriptorSize; // Slot 3 = ScanlinedRT
		pd12Device->CreateShaderResourceView( m_rtScanlined->Get(), nullptr, hSrv );

		D3D12_CPU_DESCRIPTOR_HANDLE hRtv = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		pd12Device->CreateRenderTargetView( m_rtInitial->Get(), nullptr, hRtv ); // Slot 0
		hRtv.ptr += m_uiRtvDescriptorSize;
		pd12Device->CreateRenderTargetView( m_rtScanlined->Get(), nullptr, hRtv ); // Slot 1


		// 4. Vertex Buffer (4 vertices). Use UPLOAD heap so CPU can update coordinates.
		D3D12_RESOURCE_DESC rdVb = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, sizeof( LSN_XYZRHWTEX1 ) * 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
		if ( !m_vbQuad->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdVb, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }
		m_vbView.BufferLocation = m_vbQuad->Get()->GetGPUVirtualAddress();
		m_vbView.StrideInBytes = sizeof( LSN_XYZRHWTEX1 );
		m_vbView.SizeInBytes = sizeof( LSN_XYZRHWTEX1 ) * 4;

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
			if ( !m_tLut->CreateCommittedResource( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdLut, D3D12_RESOURCE_STATE_COPY_DEST ) ) { return false; }

			UINT64 ui64LutSize = 0;
			pd12Device->GetCopyableFootprints( &rdLut, 0, 1, 0, nullptr, nullptr, nullptr, &ui64LutSize );
			D3D12_RESOURCE_DESC rdUpload = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, ui64LutSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
			if ( !m_rLutUpload->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdUpload, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }

			D3D12_CPU_DESCRIPTOR_HANDLE hSrv = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
			hSrv.ptr += m_uiSrvDescriptorSize; // Slot 1
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
			D3D12_DESCRIPTOR_RANGE drRanges[4];
			// 0: SRV Main Input
			drRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drRanges[0].NumDescriptors = 1; drRanges[0].BaseShaderRegister = 0; drRanges[0].RegisterSpace = 0; drRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			// 1: SRV LUT Input
			drRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; drRanges[1].NumDescriptors = 1; drRanges[1].BaseShaderRegister = 1; drRanges[1].RegisterSpace = 0; drRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			// 2: Sampler
			drRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER; drRanges[2].NumDescriptors = 1; drRanges[2].BaseShaderRegister = 0; drRanges[2].RegisterSpace = 0; drRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER rpParameters[4];
			rpParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rpParameters[0].Constants.ShaderRegister = 0; rpParameters[0].Constants.RegisterSpace = 0; rpParameters[0].Constants.Num32BitValues = 4; rpParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
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

		static const char * kPsVerticalNNHlsl =
			"Texture2D tSrc : register(t0);\n"
			"SamplerState sPoint : register(s0);\n"
			"cbuffer RootConstants : register(b0) { float4 c0; };\n" // x=srcH, y=1/srcH, z=0.5
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"    float v = (floor(input.Tex.y * c0.x) + c0.z) * c0.y;\n"
			"    return tSrc.Sample(sPoint, float2(input.Tex.x, v));\n"
			"}\n";

		static const char * kPsCopyHlsl =
			"Texture2D tSrc : register(t0);\n"
			"SamplerState sLinear : register(s0);\n"
			"struct PS_INPUT { float4 Pos : SV_POSITION; float2 Tex : TEXCOORD0; };\n"
			"float3 LinearToSrgb(float3 c) {\n"
			"  float3 lo = 12.92 * c;\n"
			"  float3 hi = 1.055 * pow(abs(c), 1.0 / 2.4) - 0.055;\n"
			"  float3 t = step(float3(0.0031308, 0.0031308, 0.0031308), c);\n"
			"  return lerp(lo, hi, t);\n"
			"}\n"
			"float4 main(PS_INPUT input) : SV_TARGET {\n"
			"  float4 c = tSrc.Sample(sLinear, input.Tex);\n"
			"  c.rgb = saturate(saturate(c.rgb));\n"
			"  return c;\n"
			"}\n";

		std::vector<uint8_t> vBcVs, vBcIdx, vBcVert, vBcCopy;
		if ( !CompileHlsl( kVsHlsl, "main", "vs_5_0", vBcVs ) ||
			 !CompileHlsl( kPsIdxToColorHlsl, "main", "ps_5_0", vBcIdx ) ||
			 !CompileHlsl( kPsVerticalNNHlsl, "main", "ps_5_0", vBcVert ) ||
			 !CompileHlsl( kPsCopyHlsl, "main", "ps_5_0", vBcCopy ) ) {
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC iedInputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		if ( !m_psoIdxToColor.get() ) { m_psoIdxToColor = std::make_unique<CDirectX12PipelineState>(); }
		if ( !m_psoVerticalNN.get() ) { m_psoVerticalNN = std::make_unique<CDirectX12PipelineState>(); }
		if ( !m_psoCopy.get() ) { m_psoCopy = std::make_unique<CDirectX12PipelineState>(); }

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

		// PSO 1: Idx to Color
		gpsdDesc.PS = { vBcIdx.data(), vBcIdx.size() };
		gpsdDesc.RTVFormats[0] = m_bUse16BitInitialTarget ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
		if ( !m_psoIdxToColor->Get() && !m_psoIdxToColor->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

		// PSO 2: Vertical NN
		gpsdDesc.PS = { vBcVert.data(), vBcVert.size() };
		if ( !m_psoVerticalNN->Get() && !m_psoVerticalNN->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

		// PSO 3: Copy
		gpsdDesc.PS = { vBcCopy.data(), vBcCopy.size() };
		gpsdDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // Assuming backbuffer is UNORM
		if ( !m_psoCopy->Get() && !m_psoCopy->CreateGraphicsPipelineState( pd12Device, &gpsdDesc ) ) { return false; }

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
		HRESULT hRes = pfnCompile( _pcszSource, std::strlen( _pcszSource ), nullptr, nullptr, nullptr, _pcszEntry, _pcszProfile, D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &pbCode, &pbErr );
		
		if ( FAILED( hRes ) || !pbCode ) { return false; }
		_vOutByteCode.resize( pbCode->GetBufferSize() );
		std::memcpy( _vOutByteCode.data(), pbCode->GetBufferPointer(), pbCode->GetBufferSize() );
		return true;
	}

	/**
	 * \brief Releases size-dependent resources.
	 */
	void CDx12PaletteFilter::ReleaseSizeDependents() {
		if LSN_LIKELY( m_tIndex.get() && m_tIndex->Get() ) { m_tIndex->Get()->Release(); }
		if LSN_LIKELY( m_rtInitial.get() && m_rtInitial->Get() ) { m_rtInitial->Get()->Release(); }
		if LSN_LIKELY( m_rtScanlined.get() && m_rtScanlined->Get() ) { m_rtScanlined->Get()->Release(); }
		if LSN_LIKELY( m_vbQuad.get() && m_vbQuad->Get() ) { m_vbQuad->Get()->Release(); }
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

		// 1. Copy uploads to default textures.
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

		// 2. Transition resources for rendering.
		D3D12_RESOURCE_BARRIER rbBarriers[4];
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tIndex->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		rbBarriers[1] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tLut->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		rbBarriers[2] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtInitial->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
		rbBarriers[3] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtScanlined->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET } };
		m_gclCommandList->Get()->ResourceBarrier( 4, rbBarriers );

		// Standard Pipeline Setup
		m_gclCommandList->Get()->SetGraphicsRootSignature( m_rsRootSignature->Get() );
		ID3D12DescriptorHeap * ppHeaps[] = { m_dhSrvHeap->Get(), m_dhSamplerHeap->Get() };
		m_gclCommandList->Get()->SetDescriptorHeaps( 2, ppHeaps );

		D3D12_CPU_DESCRIPTOR_HANDLE hSrvStart = m_dhSrvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		D3D12_CPU_DESCRIPTOR_HANDLE hRtvStart = m_dhRtvHeap->Get()->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hSrvGpu = m_dhSrvHeap->Get()->GetGPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hSampGpu = m_dhSamplerHeap->Get()->GetGPUDescriptorHandleForHeapStart();

		// ----- Pass 1: Indices + LUT -> Initial FP RT (1:1) -----
		m_gclCommandList->Get()->SetPipelineState( m_psoIdxToColor->Get() );
		m_gclCommandList->Get()->OMSetRenderTargets( 1, &hRtvStart, FALSE, nullptr );
		
		D3D12_VIEWPORT vp1 = { 0.0f, 0.0f, static_cast<float>(m_ui32SrcW), static_cast<float>(m_ui32SrcH), 0.0f, 1.0f };
		D3D12_RECT rScissor1 = { 0, 0, static_cast<LONG>(m_ui32SrcW), static_cast<LONG>(m_ui32SrcH) };
		m_gclCommandList->Get()->RSSetViewports( 1, &vp1 );
		m_gclCommandList->Get()->RSSetScissorRects( 1, &rScissor1 );

		float fConstants1[4] = { static_cast<float>(m_ui32SrcW), static_cast<float>(m_ui32SrcH), 0.0f, 0.0f };
		m_gclCommandList->Get()->SetGraphicsRoot32BitConstants( 0, 4, fConstants1, 0 );

		// Set SRVs (Index at Slot 0, LUT at Slot 1).
		D3D12_GPU_DESCRIPTOR_HANDLE hLutSrv = hSrvGpu; hLutSrv.ptr += m_uiSrvDescriptorSize;
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 1, hSrvGpu );
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 2, hLutSrv );
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 3, hSampGpu ); // Point Sampler

		FillQuad( (*m_vbQuad), 0.0f, 0.0f, float(m_ui32SrcW), float(m_ui32SrcH), 0.0f, 0.0f, 1.0f, 1.0f );
		m_gclCommandList->Get()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		m_gclCommandList->Get()->IASetVertexBuffers( 0, 1, &m_vbView );
		m_gclCommandList->Get()->DrawInstanced( 4, 1, 0, 0 );


		// ----- Pass 2: Initial FP -> Scanlined FP -----
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtInitial->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		m_gclCommandList->Get()->ResourceBarrier( 1, rbBarriers );

		D3D12_CPU_DESCRIPTOR_HANDLE hScanRtv = hRtvStart; hScanRtv.ptr += m_uiRtvDescriptorSize;
		m_gclCommandList->Get()->OMSetRenderTargets( 1, &hScanRtv, FALSE, nullptr );
		m_gclCommandList->Get()->SetPipelineState( m_psoVerticalNN->Get() );

		const UINT ui32DstW = m_ui32SrcW * GetActualHorSharpness();
		const UINT ui32DstH = m_ui32SrcH * GetActualVertSharpness();
		D3D12_VIEWPORT vp2 = { 0.0f, 0.0f, static_cast<float>(ui32DstW), static_cast<float>(ui32DstH), 0.0f, 1.0f };
		D3D12_RECT rScissor2 = { 0, 0, static_cast<LONG>(ui32DstW), static_cast<LONG>(ui32DstH) };
		m_gclCommandList->Get()->RSSetViewports( 1, &vp2 );
		m_gclCommandList->Get()->RSSetScissorRects( 1, &rScissor2 );

		float fConstants2[4] = { static_cast<float>(ui32DstW), static_cast<float>(ui32DstH), 0.0f, 0.0f };
		m_gclCommandList->Get()->SetGraphicsRoot32BitConstants( 0, 2, fConstants2, 0 ); // TargetSize
		float fC0[4] = { float( m_ui32SrcH ), 1.0f / float( m_ui32SrcH ), 0.5f, 0.0f };
		m_gclCommandList->Get()->SetGraphicsRoot32BitConstants( 0, 4, fC0, 0 ); // Pixel shader constants override

		D3D12_GPU_DESCRIPTOR_HANDLE hInitialSrv = hSrvGpu; hInitialSrv.ptr += m_uiSrvDescriptorSize * 2;
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 1, hInitialSrv );

		FillQuad( (*m_vbQuad), 0.0f, 0.0f, float(ui32DstW), float(ui32DstH), 0.0f, 1.0f, 1.0f, 0.0f );
		m_gclCommandList->Get()->DrawInstanced( 4, 1, 0, 0 );


		// ----- Pass 3: Composite to Backbuffer -----
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_rtScanlined->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		m_gclCommandList->Get()->ResourceBarrier( 1, rbBarriers );

		Microsoft::WRL::ComPtr<ID3D12Resource> rBackBuffer;
		m_pdx12dDevice->GetSwapChain()->GetBuffer( m_pdx12dDevice->GetSwapChain()->GetCurrentBackBufferIndex(), IID_PPV_ARGS( &rBackBuffer ) );

		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { rBackBuffer.Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET } };
		m_gclCommandList->Get()->ResourceBarrier( 1, rbBarriers );

		D3D12_CPU_DESCRIPTOR_HANDLE hBackRtv = hRtvStart; hBackRtv.ptr += m_uiRtvDescriptorSize * 2;
		m_pdx12dDevice->GetDevice()->CreateRenderTargetView( rBackBuffer.Get(), nullptr, hBackRtv );
		m_gclCommandList->Get()->OMSetRenderTargets( 1, &hBackRtv, FALSE, nullptr );

		float fClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_gclCommandList->Get()->ClearRenderTargetView( hBackRtv, fClearColor, 0, nullptr );

		D3D12_RESOURCE_DESC rdDesc = rBackBuffer->GetDesc();
		D3D12_VIEWPORT vp3 = { 0.0f, 0.0f, static_cast<float>(rdDesc.Width), static_cast<float>(rdDesc.Height), 0.0f, 1.0f };
		D3D12_RECT rScissor3 = { 0, 0, static_cast<LONG>(rdDesc.Width), static_cast<LONG>(rdDesc.Height) };
		m_gclCommandList->Get()->RSSetViewports( 1, &vp3 );
		m_gclCommandList->Get()->RSSetScissorRects( 1, &rScissor3 );

		m_gclCommandList->Get()->SetPipelineState( m_psoCopy->Get() );
		float fConstants3[4] = { static_cast<float>(rdDesc.Width), static_cast<float>(rdDesc.Height), 0.0f, 0.0f };
		m_gclCommandList->Get()->SetGraphicsRoot32BitConstants( 0, 4, fConstants3, 0 );

		D3D12_GPU_DESCRIPTOR_HANDLE hScanSrv = hSrvGpu; hScanSrv.ptr += m_uiSrvDescriptorSize * 3;
		D3D12_GPU_DESCRIPTOR_HANDLE hLinearSamp = hSampGpu; hLinearSamp.ptr += m_uiSamplerDescriptorSize;
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 1, hScanSrv );
		m_gclCommandList->Get()->SetGraphicsRootDescriptorTable( 3, hLinearSamp );

		FillQuad( (*m_vbQuad), static_cast<float>(_rOutput.left), static_cast<float>(_rOutput.top), static_cast<float>(_rOutput.right), static_cast<float>(_rOutput.bottom), 0.0f, 0.0f, 1.0f, 1.0f );
		m_gclCommandList->Get()->DrawInstanced( 4, 1, 0, 0 );

		// Final Transition Back for Presenting & Cleanup states
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { rBackBuffer.Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT } };
		rbBarriers[1] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tIndex->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST } };
		rbBarriers[2] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_tLut->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST } };
		m_gclCommandList->Get()->ResourceBarrier( 3, rbBarriers );

		m_gclCommandList->Get()->Close();

		ID3D12CommandList * ppCommandLists[] = { m_gclCommandList->Get() };
		m_pdx12dDevice->GetCommandQueue()->ExecuteCommandLists( 1, ppCommandLists );

		// Critical: Wait for the GPU to finish execution since the command allocator and mapped CPU data are reused instantly.
		m_pdx12dDevice->FlushCommandQueue();

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
