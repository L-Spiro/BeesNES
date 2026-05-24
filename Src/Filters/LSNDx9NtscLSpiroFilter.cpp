#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#include "LSNDx9NtscLSpiroFilter.h"
#include "../GPU/DirectX9/LSNDirectX9DiskInclude.h"
#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

namespace lsn {

	// == Members.
	CDx9NtscLSpiroFilter::CDx9NtscLSpiroFilter() {
		SetMonitorGammaApply( false );
	}
	CDx9NtscLSpiroFilter::~CDx9NtscLSpiroFilter() {
		StopThreads();
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx9NtscLSpiroFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		StopThreads();
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_ui32FinalStride = RowStride( m_ui32OutputWidth, OutputBits() );
		AllocYiqBuffers( _ui16Width, _ui16Height, m_ui16WidthScale );

		m_tuUploader.Reset();
		m_trRenderer.Reset();
		ReleaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );
		m_stStride = size_t( m_ui32OutputWidth * sizeof( uint16_t ) );

		StartThreads();
		return pofOut;
	}

	/**
	 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.  The final buffer, along with
	 *	its width, height, bit-depth, and stride, are returned.
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
	uint8_t * CDx9NtscLSpiroFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		if LSN_UNLIKELY( !m_pdx9dDevice ) { return m_vBasicRenderTarget[0].data(); }
		if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
			m_ui32SrcW = _ui32Width;
			m_ui32SrcH = _ui32Height;
			EnsureSizeAndResources();
			EnsureShaders();
			AllocYiqBuffers( uint16_t( m_ui32SrcW ), uint16_t( m_ui32SrcH ), m_ui16WidthScale );
		}
		
		lsw::LSW_RECT rRect;
		rRect.left = LONG( _i32DispLeft );
		rRect.top = LONG( _i32DispTop );
		rRect.right = rRect.left + LONG( _ui32DispWidth );
		rRect.bottom = rRect.top + LONG( _ui32DispHeight );

		const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
		m_vRgbBuffer.resize( m_ui16ScaledWidth * m_ui32SrcH * 4 * sizeof( float ) );

		FilterFrame( _pui8Input, _ui64RenderStartCycle + 2 );

		m_tuUploader.UploadTexels( m_pdx9dDevice, m_vRgbBuffer.data(), m_ui16ScaledWidth, m_ui32SrcH, ui32Pitch, D3DFMT_A32B32G32R32F );

		Render( rRect );

		_ui32Width = uint32_t( s_dgsState.rScreenRect.Width() );
		_ui32Height = uint32_t( s_dgsState.rScreenRect.Height() );
		_ui32Stride = _ui32Width * sizeof( uint32_t );
		return m_vBasicRenderTarget[0].data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx9NtscLSpiroFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
		EnsureShaders();
		AllocYiqBuffers( uint16_t( m_ui32SrcW ), uint16_t( m_ui32SrcH ), m_ui16WidthScale );
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9NtscLSpiroFilter::DeActivate() {
		CParent::DeActivate();

		m_tuUploader.Reset();
		m_trRenderer.Reset();
		
		m_rtScanlined.reset();
		m_vbQuad.reset();
		m_psVerticalNN.reset();

		if ( m_pdx9dDevice ) {
			s_dgsState.DestroyDx9();
			m_pdx9dDevice = nullptr;
		}
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CDx9NtscLSpiroFilter::FrameResize() {
		s_dgsState.OnSizeDx9();
		EnsureSizeAndResources();
		EnsureShaders();
	}

	/**
	 * Sets the number of worker threads used by the filter.
	 *
	 * \param _stThreads Number of worker threads to use.  0 disables worker threads.
	 */
	void CDx9NtscLSpiroFilter::SetWorkerThreadCount( size_t _stThreads ) {
		if ( _stThreads == m_stWorkerThreadCount ) { return; }

		const bool bRestart = m_bThreadsStarted;
		if ( bRestart ) { StopThreads(); }
		m_stWorkerThreadCount = _stThreads;
		if ( bRestart ) { StartThreads(); }
	}

	/**
	 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
	 **/
	void CDx9NtscLSpiroFilter::FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle ) {
		const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
		if LSN_UNLIKELY( !m_vThreads.size() ) {
			RenderScanlineRange<false>( _pui8Pixels, 0, m_ui16Height, _ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );
			return;
		}

		const size_t stThreads = m_vThreads.size() + 1;
		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_jJob.pui8Pixels = _pui8Pixels;
			m_jJob.ui64RenderStartCycle = _ui64RenderStartCycle;
			m_jJob.stThreads = stThreads;
			++m_ui64JobId;
			m_ui32WorkersRemaining.store( uint32_t( m_vThreads.size() ) );
		}
		m_cvGo.notify_all();

		const uint16_t ui16Lines = m_ui16Height;
		const uint16_t ui16Start = 0;
		const uint16_t ui16End = uint16_t( (uint32_t( ui16Lines ) * 1U) / uint32_t( stThreads ) );
		RenderScanlineRange<false>( _pui8Pixels, ui16Start, ui16End, _ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );

		std::unique_lock<std::mutex> ulLock( m_mThreadMutex );
		m_cvDone.wait( ulLock, [&]() { return m_ui32WorkersRemaining.load() == 0; } );
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * 
	 * \return Returns true on success.
	 */
	bool CDx9NtscLSpiroFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx9dDevice ) {
			if ( !s_dgsState.CreateDx9() ) { return false; }
			m_pdx9dDevice = &s_dgsState.dx9Device;
			m_tuUploader.Reset();
			m_trRenderer.Reset();
			m_rtScanlined.reset();
			m_vbQuad.reset();
			m_psVerticalNN.reset();
		}

		if LSN_UNLIKELY( !m_rtScanlined.get() ) { m_rtScanlined = std::make_unique<CDirectX9RenderTarget>( m_pdx9dDevice ); }
		if LSN_UNLIKELY( !m_vbQuad.get() ) { m_vbQuad = std::make_unique<CDirectX9VertexBuffer>( m_pdx9dDevice ); }

		const uint32_t ui32ScanW = m_ui16ScaledWidth * GetActualHorSharpness();
		const uint32_t ui32ScanH = m_ui32SrcH * GetActualVertSharpness();
		if ( !ui32ScanW || !ui32ScanH ) { return false; }

		bool bOk = (m_ui32RsrcW == m_ui32SrcW) && (m_ui32RsrcH == m_ui32SrcH) && 
				   (m_ui32TargetScanW == ui32ScanW) && (m_ui32TargetScanH == ui32ScanH) &&
				   m_rtScanlined->Valid() && m_vbQuad->Valid();

		if ( bOk ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		const auto fmtRt = m_bUse16BitInitialTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F;
		if ( !m_rtScanlined->CreateColorTarget( ui32ScanW, ui32ScanH, fmtRt ) ) { return false; }
		if ( !m_vbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }

		m_bValidState = true;
		m_ui32RsrcW = m_ui32SrcW;
		m_ui32RsrcH = m_ui32SrcH;
		m_ui32TargetScanW = ui32ScanW;
		m_ui32TargetScanH = ui32ScanH;
		return true;
	}

	/**
	 * \brief Ensures pixel shaders (vertical NN) are created.
	 *
	 * Compiles the HLSL entry points with D3DX at runtime and creates pixel shaders from bytecode.
	 * If D3DX cannot be loaded, this function returns false.
	 *
	 * \return Returns true if all shaders are ready.
	 */
	bool CDx9NtscLSpiroFilter::EnsureShaders() {
		if ( !m_pdx9dDevice ) { return false; }

		static const char * kPsVerticalNNHlsl =
			"#include \"LSNGamma.hlsl\"\n"
			"sampler2D sSrc : register( s0 );\n"
			"float4 c0 : register( c0 );\n" // x=srcH, y=1/srcH, z=0.5
			"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
			"    float v = (floor( uv.y * c0.x ) + c0.z ) * c0.y; \n"
			"    return float4( CrtProperToLinear3( tex2D( sSrc, float2( uv.x, v ) ).xyz ), 1.0 );\n"
			//"    return float4( tex2D( sSrc, float2( uv.x, v ) ).xyz, 1.0 );\n"
			"}\n";

		if ( !m_psVerticalNN.get() ) { m_psVerticalNN = std::make_unique<CDirectX9PixelShader>( m_pdx9dDevice ); }

		CDirectX9DiskInclude diInclude( CDirectX9DiskInclude::GetExeShadersDir() );

		if ( !m_psVerticalNN->Valid() ) {
			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsVerticalNNHlsl, "main", "ps_2_0", vBc, &diInclude ) ) { return false; }
			if ( !m_psVerticalNN->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}
		return true;
	}

	/**
	 * \brief Compiles an HLSL pixel shader using dynamically loaded D3DX.
	 *
	 * \param _pcszSource Null-terminated HLSL source code.
	 * \param _pcszEntry Null-terminated entry-point function name (e.g., "main").
	 * \param _pcszProfile Null-terminated profile (e.g., "ps_2_0").
	 * \param _vOutByteCode Output vector to receive the compiled bytecode (DWORD stream).
	 * \param _piInclude Optional #include handler.
	 * \return Returns true if compilation succeeded and bytecode was produced.
	 */
	bool CDx9NtscLSpiroFilter::CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode, ID3DXInclude * _piInclude ) {
		static const wchar_t * kDlls[] = {
			L"d3dx9_43.dll", L"d3dx9_42.dll", L"d3dx9_41.dll", L"d3dx9_40.dll",
			L"d3dx9_39.dll", L"d3dx9_38.dll", L"d3dx9_37.dll", L"d3dx9_36.dll",
			L"d3dx9_35.dll", L"d3dx9_34.dll", L"d3dx9_33.dll", L"d3dx9_32.dll", L"d3dx9_31.dll",
		};
		typedef HRESULT (WINAPI * PFN_D3DXCompileShaderA)(
			LPCSTR, UINT, CONST D3DXMACRO*, LPD3DXINCLUDE,
			LPCSTR, LPCSTR, DWORD,
			LPD3DXBUFFER*, LPD3DXBUFFER*, LPD3DXCONSTANTTABLE * );

		lsw::LSW_HMODULE hD3dx;
		PFN_D3DXCompileShaderA pfnCompile = nullptr;
		for ( size_t I = 0; I < std::size( kDlls ); ++I ) {
			hD3dx = lsw::LSW_HMODULE( kDlls[I] );
			if ( hD3dx.Valid() ) {
				pfnCompile = reinterpret_cast<PFN_D3DXCompileShaderA>(::GetProcAddress( hD3dx.hHandle, "D3DXCompileShader" ));
				if ( pfnCompile ) { break; }
				hD3dx = lsw::LSW_HMODULE{};
			}
		}
		if ( !pfnCompile ) { return false; }

		ID3DXBuffer * pbCode = nullptr;
		ID3DXBuffer * pbErrs = nullptr;
		HRESULT hRes = pfnCompile(
			_pcszSource,
			static_cast<UINT>(std::strlen( _pcszSource )),
			nullptr, _piInclude,
			_pcszEntry, _pcszProfile,
			0,
			&pbCode, &pbErrs, nullptr );
		if ( FAILED( hRes ) || !pbCode ) {
			if ( pbErrs ) { pbErrs->Release(); }
			return false;
		}

		const size_t stBytes = pbCode->GetBufferSize();
		const size_t stDwords = stBytes / sizeof( DWORD );
		_vOutByteCode.resize( stDwords );
		std::memcpy( _vOutByteCode.data(), pbCode->GetBufferPointer(), stBytes );

		pbCode->Release();
		if ( pbErrs ) { pbErrs->Release(); }
		return true;
	}

	/**
	 * \brief Releases size-dependent resources (index texture, FP RTs, quad VB).
	 */
	void CDx9NtscLSpiroFilter::ReleaseSizeDependents() {
		if LSN_LIKELY( m_rtScanlined.get() && m_rtScanlined->Get() ) { m_rtScanlined->Reset(); }
		if LSN_LIKELY( m_vbQuad.get() && m_vbQuad->Get() ) { m_vbQuad->Reset(); }
		m_ui32RsrcW = m_ui32RsrcH = 0;
		m_ui32TargetScanW = m_ui32TargetScanH = 0;
	}

	/**
	 * Renders the final output to the backbuffer.
	 *
	 * \param _rOutput The destination rectangle.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx9NtscLSpiroFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_tuUploader.GetTexture() || !m_tuUploader.GetTexture()->Valid() || !m_rtScanlined.get() || !m_rtScanlined->Valid() || !m_psVerticalNN.get() || !m_psVerticalNN->Valid() || !m_vbQuad.get() || !m_vbQuad->Valid() ) { return false; }
		IDirect3DDevice9 * pd3d9dDevice = m_pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		// ----- Pass 1: Src -> Scanlined FP (height * factor, nearest vertically) -----
		IDirect3DSurface9 * pd3ds9Surf = m_rtScanlined->GetSurface();
		if LSN_UNLIKELY( !pd3ds9Surf ) { return false; }
		pd3d9dDevice->SetRenderTarget( 0, pd3ds9Surf );
		pd3ds9Surf->Release(); // GetSurface() calls AddRef(), so immediately release our local reference to avoid a leak.

		const UINT ui32DstW = m_ui16ScaledWidth * GetActualHorSharpness();
		const UINT ui32DstH = m_ui32SrcH * GetActualVertSharpness();

		D3DVIEWPORT9 vpViewport{};
		vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = ui32DstW; vpViewport.Height = ui32DstH; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
		pd3d9dDevice->SetViewport( &vpViewport );

		pd3d9dDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3d9dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );
		float fU0, fV0, fU1, fV1;
		HalfTexelUv( ui32DstW, ui32DstH, fU0, fV0, fU1, fV1 );
		if LSN_UNLIKELY( !FillQuad( (*m_vbQuad), 0.0f, 0.0f, float( ui32DstW ), float( ui32DstH ),
			fU0, fV0, fU1, fV1 ) ) { return false; }
		pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );

		pd3d9dDevice->SetTexture( 0, m_tuUploader.GetTexture()->Get() );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		//pd3d9dDevice->SetTexture( 1, nullptr );

		const float fC0[4] = { float( m_ui32SrcH ), 1.0f / float( m_ui32SrcH ), 0.5f, 0.0f };
		pd3d9dDevice->SetPixelShader( m_psVerticalNN->Get() );
		pd3d9dDevice->SetPixelShaderConstantF( 0, fC0, 1 );
		pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		// ----- Pass 2: Composite to backbuffer inside _rOutput -----
		IDirect3DSurface9 * psBackBuffer = nullptr;
		if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &psBackBuffer ) ) ) {
			m_trRenderer.Render( m_pdx9dDevice, m_rtScanlined->Texture()->Get(), psBackBuffer, _rOutput, 1.0f, false, true );
			psBackBuffer->Release();
		}

		return true;
	}

	/**
	 * \brief Starts the worker threads.
	 *
	 * Creates m_vThreads based on m_stWorkerThreadCount and resets the thread-control state.
	 * Safe to call multiple times; if threads are already started, this function does nothing.
	 */
	void CDx9NtscLSpiroFilter::StartThreads() {
		if ( m_bThreadsStarted ) { return; }

		const size_t stWorkers = m_stWorkerThreadCount;
		m_bStopThreads = false;
		m_ui64JobId = 0;
		m_ui32WorkersRemaining.store( 0 );
		m_vThreads.clear();

		if ( stWorkers ) {
			m_vThreads.reserve( stWorkers );
			for ( size_t I = 0; I < stWorkers; ++I ) {
				m_vThreads.emplace_back( &CDx9NtscLSpiroFilter::WorkerThread, this, I + 1 );
			}
		}

		m_bThreadsStarted = true;
	}

	/**
	 * \brief Stops the worker threads.
	 *
	 * Signals all worker threads to exit, wakes them, joins them, clears m_vThreads, and
	 * resets thread-control state.  Safe to call multiple times; if threads are not started,
	 * this function does nothing.
	 */
	void CDx9NtscLSpiroFilter::StopThreads() {
		if ( !m_bThreadsStarted ) { return; }

		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_bStopThreads = true;
		}
		m_cvGo.notify_all();

		for ( auto & T : m_vThreads ) {
			if ( T.joinable() ) {
				T.join();
			}
		}
		m_vThreads.clear();

		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_bStopThreads = false;
		}
		m_ui32WorkersRemaining.store( 0 );
		m_bThreadsStarted = false;
	}

	/**
	 * \brief The worker thread entry point.
	 *
	 * Waits for jobs signaled via m_cvGo, renders the scanline range assigned to this worker,
	 * then decrements m_ui32WorkersRemaining and notifies m_cvDone when the final worker
	 * finishes the job.
	 *
	 * \param _stThreadIdx The worker thread index in the range [1, stThreads - 1].
	 *	Index 0 is reserved for the calling thread.
	 */
	void CDx9NtscLSpiroFilter::WorkerThread( size_t _stThreadIdx ) {
		::SetThreadHighPriority();
		lsn::CScopedNoSubnormals snsNoSubnormals;

		uint64_t ui64LastJobId = 0;

		for ( ;; ) {
			LSN_JOB jJob;
			{
				std::unique_lock<std::mutex> ulLock( m_mThreadMutex );
				m_cvGo.wait( ulLock, [&]() { return m_bStopThreads || m_ui64JobId != ui64LastJobId; } );
				if ( m_bStopThreads ) { break; }

				ui64LastJobId = m_ui64JobId;
				jJob = m_jJob;
			}

			const uint16_t ui16Lines = m_ui16Height;
			const uint16_t ui16Start = uint16_t( (uint32_t( ui16Lines ) * uint32_t( _stThreadIdx )) / uint32_t( jJob.stThreads ) );
			const uint16_t ui16End = uint16_t( (uint32_t( ui16Lines ) * uint32_t( _stThreadIdx + 1 )) / uint32_t( jJob.stThreads ) );
			
			if ( ui16End > ui16Start ) {
				const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
				RenderScanlineRange<false>( jJob.pui8Pixels, ui16Start, ui16End, jJob.ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );
			}

			if ( m_ui32WorkersRemaining.fetch_sub( 1 ) == 1 ) {
				std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
				m_cvDone.notify_one();
			}
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
