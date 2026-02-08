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
		m_lrRect.pBits = nullptr;
		if LSN_UNLIKELY( !m_tSrc->LockRect( 0, m_lrRect, nullptr, D3DLOCK_DISCARD ) || nullptr == m_lrRect.pBits ) { return m_vBasicRenderTarget[0].data(); }

		FilterFrame( _pui8Input, _ui64RenderStartCycle + 2 );

		m_tSrc->UnlockRect( 0 );
		m_lrRect.pBits = nullptr;

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

		m_tSrc.reset();
		m_rtScanlined.reset();
		m_vbQuad.reset();
		m_psVerticalNN.reset();
		m_psCopy.reset();

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
		// If there are no worker threads, render the whole frame on the calling thread.
		if LSN_UNLIKELY( !m_vThreads.size() ) {
			RenderScanlineRange<false>( _pui8Pixels, 0, m_ui16Height, _ui64RenderStartCycle, reinterpret_cast<uint8_t *>(m_lrRect.pBits), m_lrRect.Pitch );
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

		// Render the calling thread's portion.
		const uint16_t ui16Lines = m_ui16Height;
		const uint16_t ui16Start = 0;
		const uint16_t ui16End = uint16_t( (uint32_t( ui16Lines ) * 1U) / uint32_t( stThreads ) );
		RenderScanlineRange<false>( _pui8Pixels, ui16Start, ui16End, _ui64RenderStartCycle, reinterpret_cast<uint8_t *>(m_lrRect.pBits), m_lrRect.Pitch );

		// Wait for all worker threads.
		std::unique_lock<std::mutex> ulLock( m_mThreadMutex );
		m_cvDone.wait( ulLock, [&]() { return m_ui32WorkersRemaining.load() == 0; } );
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 *
	 * Releases/creates the index texture, both FP render targets, and quad vertex buffer as needed.
	 *
	 * \return Returns true on success.
	 */
	bool CDx9NtscLSpiroFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx9dDevice ) {
			if ( !s_dgsState.CreateDx9() ) { return false; }
			m_pdx9dDevice = &s_dgsState.dx9Device;
		}

		if LSN_UNLIKELY( !m_rtScanlined.get() ) {
			m_rtScanlined = std::make_unique<CDirectX9RenderTarget>( m_pdx9dDevice );
			if ( !m_rtScanlined.get() ) { return false; }
		}
		if LSN_UNLIKELY( !m_vbQuad.get() ) {
			m_vbQuad = std::make_unique<CDirectX9VertexBuffer>( m_pdx9dDevice );
			if ( !m_vbQuad.get() ) { return false; }
		}
		
		const uint32_t ui32ScanW = m_ui16ScaledWidth * GetActualHorSharpness();
		const uint32_t ui32ScanH = m_ui32SrcH * GetActualVertSharpness();
		if ( !ui32ScanW || !ui32ScanH ) { return false; }
		const bool bOk =
			(m_ui32RsrcW == m_ui32SrcW) &&
			(m_ui32RsrcH == m_ui32SrcH) &&
			m_tSrc.get() && m_tSrc->Valid() &&
			m_rtScanlined->Valid() && m_vbQuad->Valid();

		if ( bOk ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		// Initial FP RT: same size as source.
		const auto fmtRt = m_bUse16BitInitialTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F;
		//if ( !m_rtInitial->CreateColorTarget( m_ui32SrcW, m_ui32SrcH, fmtRt ) ) { return false; }

		// Scanlined FP RT: height scaled by factor.
		if ( !m_rtScanlined->CreateColorTarget( ui32ScanW, ui32ScanH, fmtRt ) ) { return false; }

		// Dynamic quad VB: 4 vertices XYZRHW|TEX1.
		if ( !m_vbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }

		if ( !PrepaerSrcTexture() ) { return false; }

		m_bValidState = true;
		m_ui32RsrcW = m_ui32SrcW;
		m_ui32RsrcH = m_ui32SrcH;
		return true;
	}

	/**
	 * Creates the upload texture.
	 * 
	 * \return Returns true on success.
	 **/
	bool CDx9NtscLSpiroFilter::PrepaerSrcTexture() {
		if LSN_UNLIKELY( !m_pdx9dDevice || !m_ui32SrcW || !m_ui32SrcH ) { return false; }

		if LSN_UNLIKELY( !m_tSrc.get() ) {
			m_tSrc = std::make_unique<CDirectX9Texture>( m_pdx9dDevice );
			if ( !m_tSrc.get() ) { return false; }
		}
		if LSN_UNLIKELY( !m_tSrc->Valid() ) {
			//if ( !m_tSrc->Create2D( m_ui32SrcW * m_ui16ScaledWidth, m_ui32SrcH, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ) {
			if ( !m_tSrc->Create2D( m_ui16ScaledWidth, m_ui32SrcH, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT ) ) {
				return false;
			}
		}
		return true;
	}

	/**
	 * \brief Ensures pixel shaders (indexÅ®color, vertical NN, copy) are created.
	 *
	 * Compiles the HLSL entry points with D3DX at runtime and creates pixel shaders from bytecode.
	 * If D3DX cannot be loaded, this function returns false.
	 *
	 * \return Returns true if all shaders are ready.
	 */
	bool CDx9NtscLSpiroFilter::EnsureShaders() {
		// Pass 1: vertical nearest-neighbor (ps_2_0). c0 = [srcH, 1/srcH, 0.5, 0].
		static const char * kPsVerticalNNHlsl =
			"#include \"LSNGamma.hlsl\"\n"
			"sampler2D sSrc : register( s0 );\n"
			"float4 c0 : register( c0 );\n" // x=srcH, y=1/srcH, z=0.5
			"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
			"    float v = (floor( uv.y * c0.x ) + c0.z ) * c0.y; \n"
			//"    return tex2D( sSrc, float2( uv.x, v ) );\n"
			"    return float4( CrtProperToLinear3( tex2D( sSrc, float2( uv.x, v ) ).xyz ), 1.0 );\n"
			"}\n";

		// Pass 2: simple copy (ps_2_0).
		static const char * kPsCopyHlsl =
			"sampler2D sSrc : register( s0 );\n"
			"float3 LinearToSrgb( float3 c ) {\n"
			"  float3 lo = 12.92 * c;\n"
			"  float3 hi = 1.055 * pow( c, 1.0 / 2.4 ) - 0.055;\n"
			"  float3 t = step( float3( 0.0031308, 0.0031308, 0.0031308 ), c );\n"
			"  return lerp( lo, hi, t );\n"
			"}\n"
			"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
			"  float4 c = tex2D( sSrc, uv );\n"
			"  c.rgb = saturate( saturate( c.rgb ) );\n"
			"  return c;\n"
			"}\n";
		// c0 = [srcW, srcH, 1/srcW, 1/srcH]
		//static const char * kPsCopyHlsl =
		//	"sampler2D sSrc : register(s0);\n"
		//	"float4 c0 : register(c0);\n" // x=srcW, y=srcH, z=1/srcW, w=1/srcH
		//	"float w_cubic(float x){\n"
		//	"  x = abs(x);\n"
		//	"  if (x < 1.0) return (1.5*x - 2.5)*x*x + 1.0;\n"
		//	"  if (x < 2.0) return ((-0.5*x + 2.5)*x - 4.0)*x + 2.0;\n"
		//	"  return 0.0;\n"
		//	"}\n"
		//	"float4 main(float2 uv : TEXCOORD0) : COLOR {\n"
		//	"  float2 texSz   = c0.xy;\n"
		//	"  float2 invTex  = c0.zw;\n"
		//	"  // Map uv to source texel space (center-based)\n"
		//	"  float2 coord   = uv * texSz - 0.5;\n"
		//	"  float2 base    = floor(coord);\n"
		//	"  float2 f       = coord - base;\n"
		//	"  float4 sum     = 0;\n"
		//	"  float  wsum    = 0;\n"
		//	"  // 4x4 taps around base\n"
		//	"  [unroll] for (int j = -1; j <= 2; ++j){\n"
		//	"    float wy = w_cubic(j - f.y);\n"
		//	"    float v  = (base.y + j + 0.5) * invTex.y;\n"
		//	"    [unroll] for (int i = -1; i <= 2; ++i){\n"
		//	"      float wx = w_cubic(i - f.x);\n"
		//	"      float u  = (base.x + i + 0.5) * invTex.x;\n"
		//	"      float2 tuv = float2(u,v);\n"
		//	"      // Clamp at edges.\n"
		//	"      tuv = saturate(tuv);\n"
		//	"      float w = wx * wy;\n"
		//	"      sum  += tex2D(sSrc, tuv) * w;\n"
		//	"      wsum += w;\n"
		//	"    }\n"
		//	"  }\n"
		//	"  float4 c = sum / wsum;\n"
		//	"  return c;\n"
		//	"}\n";

		if ( !m_psVerticalNN.get() ) {
			m_psVerticalNN = std::make_unique<CDirectX9PixelShader>( m_pdx9dDevice );
			if ( !m_psVerticalNN.get() ) { return false; }
		}
		if ( !m_psCopy.get() ) {
			m_psCopy = std::make_unique<CDirectX9PixelShader>( m_pdx9dDevice );
			if ( !m_psCopy.get() ) { return false; }
		}

		CDirectX9DiskInclude diInclude( CDirectX9DiskInclude::GetExeShadersDir() );

		if ( !m_psVerticalNN->Valid() ) {
			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsVerticalNNHlsl, "main", "ps_2_0", vBc, &diInclude ) ) { return false; }
			if ( !m_psVerticalNN->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}
		if ( !m_psCopy->Valid() ) {
			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsCopyHlsl, "main", "ps_2_0", vBc, &diInclude ) ) { return false; }
			if ( !m_psCopy->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
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
		if LSN_LIKELY( m_tSrc.get() ) { m_tSrc->Reset(); }
		if LSN_LIKELY( m_rtScanlined.get() ) { m_rtScanlined->Reset(); }
		if LSN_LIKELY( m_vbQuad.get() ) { m_vbQuad->Reset(); }
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * Renders the final output to the backbuffer.
	 *
	 * \param _rOutput The destination rectangle.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx9NtscLSpiroFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_tSrc.get() || !m_rtScanlined.get() || !m_psVerticalNN.get() || !m_psCopy.get() || !m_vbQuad.get() ) { return false; }
		if LSN_UNLIKELY( !m_pdx9dDevice || !m_tSrc->Valid() || !m_rtScanlined->Valid() || !m_psVerticalNN->Valid() || !m_psCopy->Valid() || !m_vbQuad->Valid() ) { return false; }
		IDirect3DDevice9 * pd3d9dDevice = m_pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		// ----- Pass 1: Src -> Scanlined FP (height * factor, nearest vertically) -----
		{
			IDirect3DSurface9 * pd3ds9Surf = m_rtScanlined->GetSurface();
			if LSN_UNLIKELY( !pd3ds9Surf ) { return false; }
			pd3d9dDevice->SetRenderTarget( 0, pd3ds9Surf );
			pd3ds9Surf->Release();

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
			const uint32_t uiSrcW = ui32DstW;
			const uint32_t uiSrcH = ui32DstH;
			HalfTexelUv( uiSrcW, uiSrcH, fU0, fV0, fU1, fV1 );
			if LSN_UNLIKELY( !FillQuad( (*m_vbQuad), 0.0f, 0.0f, float( ui32DstW ), float( ui32DstH ),
				fU0, fV0, fU1, fV1 ) ) { return false; }
			pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );

			// s0 = CPU-updated source texture (POINT/CLAMP).
			pd3d9dDevice->SetTexture( 0, m_tSrc->Get() );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			pd3d9dDevice->SetTexture( 1, nullptr );

			// Set constants: fC0 = [srcH, 1/srcH, 0.5, 0]
			const float fC0[4] = { float( m_ui32SrcH ), 1.0f / float( m_ui32SrcH ), 0.5f, 0.0f };
			pd3d9dDevice->SetPixelShader( m_psVerticalNN->Get() );
			pd3d9dDevice->SetPixelShaderConstantF( 0, fC0, 1 );
			pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		}

		// ----- Pass 2: Composite to backbuffer inside _rOutput, black elsewhere -----
		{
			IDirect3DSurface9 * pd3ds9Back = nullptr;
			if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3ds9Back ) ) ) {
				pd3d9dDevice->SetRenderTarget( 0, pd3ds9Back );
				pd3ds9Back->Release();
			}

			pd3d9dDevice->Clear( 0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

			D3DSURFACE_DESC sdBb{};
			IDirect3DSurface9 * pBB = nullptr;
			if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB ) ) ) {
				pBB->GetDesc( &sdBb );
				pBB->Release();
			}
			D3DVIEWPORT9 vpViewport{};
			vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = sdBb.Width; vpViewport.Height = sdBb.Height; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
			pd3d9dDevice->SetViewport( &vpViewport );

			pd3d9dDevice->SetTexture( 0, m_rtScanlined->Texture()->Get() /*m_tSrc->Get()*/ );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
			pd3d9dDevice->SetTexture( 1, nullptr );

			pd3d9dDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );

			pd3d9dDevice->SetPixelShader( m_psCopy->Get() );
			pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );

			float fU0, fV0, fU1, fV1;
			const uint32_t uiSrcW = m_ui32SrcW * GetActualHorSharpness();
			const uint32_t uiSrcH = m_ui32SrcH * GetActualVertSharpness();
			HalfTexelUv( uiSrcW, uiSrcH, fU0, fV0, fU1, fV1 );
			if LSN_UNLIKELY( !FillQuad( (*m_vbQuad), static_cast<float>(_rOutput.left),  static_cast<float>(_rOutput.top),
				static_cast<float>(_rOutput.right), static_cast<float>(_rOutput.bottom),
				fU0, fV0, fU1, fV1 ) ) { return false; }

			pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );
			pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
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
				RenderScanlineRange<false>( jJob.pui8Pixels, ui16Start, ui16End, jJob.ui64RenderStartCycle, reinterpret_cast<uint8_t *>(m_lrRect.pBits), m_lrRect.Pitch );
			}

			if ( m_ui32WorkersRemaining.fetch_sub( 1 ) == 1 ) {
				std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
				m_cvDone.notify_one();
			}
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
