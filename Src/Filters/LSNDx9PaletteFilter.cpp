#ifdef LSN_DX9

#include "LSNDx9PaletteFilter.h"


namespace lsn {

	CDx9PaletteFilter::CDx9PaletteFilter() {
	}
	CDx9PaletteFilter::~CDx9PaletteFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx9PaletteFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
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
	 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.  False always indicates a memory failure if _pfRgba512 is not nullptr.
	 **/
	bool CDx9PaletteFilter::SetLut( const float * _pfRgba512 ) {
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
	void CDx9PaletteFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
		EnsureShaders();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9PaletteFilter::DeActivate() {
		CParent::DeActivate();

		m_tIndex.reset();
		m_tLut.reset();
		m_rtInitial.reset();
		m_rtScanlined.reset();
		m_vbQuad.reset();
		m_psIdxToColor.reset();
		m_psVerticalNN.reset();
		m_psCopy.reset();

		m_bUpdatePalette = true;

		if ( m_pdx9dDevice ) {
			s_dgsState.DestroyDx9();
			m_pdx9dDevice = nullptr;
		}
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
	uint8_t * CDx9PaletteFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
			m_ui32SrcW = _ui32Width;
			m_ui32SrcH = _ui32Height;
			EnsureSizeAndResources();
			EnsureShaders();
			m_vOutputBuffer.resize( _ui32Width * _ui32Height * sizeof( uint32_t ) );
		}
		if LSN_UNLIKELY( m_bUpdatePalette ) {
			UpdateLut();	// Failure isn't technically critical.
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
		return m_vOutputBuffer.data();		// Unused except in edge cases; we present to the screen directly.
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CDx9PaletteFilter::FrameResize() {
		s_dgsState.OnSizeDx9();

		EnsureSizeAndResources();
		EnsureShaders();
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 *
	 * Releases/creates the index texture, both FP render targets, and quad vertex buffer as needed.
	 *
	 * \return Returns true on success.
	 */
	bool CDx9PaletteFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx9dDevice ) {
			if ( !s_dgsState.CreateDx9() ) { return false; }
			m_pdx9dDevice = &s_dgsState.dx9Device;
		}

		if LSN_UNLIKELY( !m_tIndex.get() ) {
			m_tIndex = std::make_unique<CDirectX9Texture>( m_pdx9dDevice );
			if ( !m_tIndex.get() ) { return false; }
		}
		if LSN_UNLIKELY( !m_rtInitial.get() ) {
			m_rtInitial = std::make_unique<CDirectX9RenderTarget>( m_pdx9dDevice );
			if ( !m_rtInitial.get() ) { return false; }
		}
		if LSN_UNLIKELY( !m_rtScanlined.get() ) {
			m_rtScanlined = std::make_unique<CDirectX9RenderTarget>( m_pdx9dDevice );
			if ( !m_rtScanlined.get() ) { return false; }
		}
		if LSN_UNLIKELY( !m_vbQuad.get() ) {
			m_vbQuad = std::make_unique<CDirectX9VertexBuffer>( m_pdx9dDevice );
			if ( !m_vbQuad.get() ) { return false; }
		}
		
		const uint32_t ui32ScanW = m_ui32SrcW * GetActualHorSharpness();
		const uint32_t ui32ScanH = m_ui32SrcH * GetActualVertSharpness();
		if ( !ui32ScanW || !ui32ScanH ) { return false; }
		const bool bOk =
			(m_ui32RsrcW == m_ui32SrcW) &&
			(m_ui32RsrcH == m_ui32SrcH) &&
			m_tIndex->Valid() && m_rtInitial->Valid() && m_rtScanlined->Valid() && m_vbQuad->Valid();

		if ( bOk ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		// Index texture: L16, DEFAULT|DYNAMIC.
		if ( !m_tIndex->Create2D( m_ui32SrcW, m_ui32SrcH, 1, D3DUSAGE_DYNAMIC, D3DFMT_L16, D3DPOOL_DEFAULT ) ) { return false; }

		// Initial FP RT: same size as source.
		const auto fmtRt = m_bUse16BitInitialTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F;
		if ( !m_rtInitial->CreateColorTarget( m_ui32SrcW, m_ui32SrcH, fmtRt ) ) { return false; }

		// Scanlined FP RT: height scaled by factor.
		if ( !m_rtScanlined->CreateColorTarget( ui32ScanW, ui32ScanH, fmtRt ) ) { return false; }

		// Dynamic quad VB: 4 vertices XYZRHW|TEX1.
		if ( !m_vbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }

		if ( !UpdateLut() ) { return false; }

		m_bValidState = true;
		m_ui32RsrcW = m_ui32SrcW;
		m_ui32RsrcH = m_ui32SrcH;
		return true;
	}

	/**
	 * \brief Updates the 512-entry float RGBA LUT.
	 *
	 * The LUT is a 512Å~1 A32B32G32R32F MANAGED texture. Each entry is RGBA in linear space.
	 *
	 * \return Returns true on success.
	 */
	bool CDx9PaletteFilter::UpdateLut() {
		if LSN_UNLIKELY( !m_pdx9dDevice || !m_ui32SrcW || !m_ui32SrcH ) { return false; }
		// Create once: 512x1 A32B32G32R32F MANAGED texture.
		if LSN_UNLIKELY( !m_tLut.get() ) {
			m_tLut = std::make_unique<CDirectX9Texture>( m_pdx9dDevice );
			if ( !m_tLut.get() ) { return false; }
		}
		if LSN_UNLIKELY( !m_tLut->Valid() ) {
			if ( !m_tLut->Create2D( 512, 1, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED ) ) {
				return false;
			}
		}
		if LSN_UNLIKELY( m_vLut.size() && m_bUpdatePalette ) {
			D3DLOCKED_RECT lrLocked{};
			if LSN_UNLIKELY( !m_tLut->LockRect( 0, lrLocked, nullptr, 0 ) ) { return false; }
			std::memcpy( lrLocked.pBits, m_vLut.data(), sizeof( float ) * 4 * 512 );
			m_tLut->UnlockRect( 0 );
			m_bUpdatePalette = false;
		}
		return true;
	}

	/**
	 * \brief Uploads the 16-bit PPU indices (9-bit effective values 0..511) to the L16 index texture.
	 *
	 * Values are mapped to L16 in [0..65535] so that sampling.r*511 + 0.5 floors back to the exact index in the shader.
	 *
	 * \param _pui16Idx Source pointer to the index image (row-major).
	 * \param _ui32W Image width in pixels (must equal the Init size).
	 * \param _ui32H Image height in pixels (must equal the Init size).
	 * \param _ui32SrcPitch Source pitch in bytes; pass 0 for tightly packed (= _ui32W * 2).
	 * \return Returns true on success.
	 */
	bool CDx9PaletteFilter::UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch ) {
		if LSN_UNLIKELY( !m_pdx9dDevice || _ui32W != m_ui32SrcW || _ui32H != m_ui32SrcH ) { return false; }

		if LSN_UNLIKELY( !m_tIndex.get() ) {
			m_tIndex = std::make_unique<CDirectX9Texture>( m_pdx9dDevice );
			if LSN_UNLIKELY( !m_tIndex.get() ) { return false; }
		}

		if LSN_UNLIKELY( !m_tIndex->Valid() ) { return false; }
		if ( !_ui32SrcPitch ) { _ui32SrcPitch = _ui32W * sizeof( uint16_t ); }

		D3DLOCKED_RECT lrRect{};
		if LSN_UNLIKELY( !m_tIndex->LockRect( 0, lrRect, nullptr, D3DLOCK_DISCARD ) || nullptr == lrRect.pBits ) { return false; }

		if LSN_LIKELY( lrRect.Pitch == INT( _ui32SrcPitch ) ) {
			// Just copy it all at once.
			std::memcpy( lrRect.pBits, _pui16Idx, _ui32H * _ui32SrcPitch );
			m_tIndex->UnlockRect( 0 );
			return true;
		}

		for ( uint32_t Y = 0; Y < _ui32H; ++Y ) {
			const uint8_t * pSrcRow = reinterpret_cast<const uint8_t *>(_pui16Idx) + Y * _ui32SrcPitch;
			uint8_t * pDstRow = reinterpret_cast<uint8_t *>(lrRect.pBits) + Y * lrRect.Pitch;
			const uint16_t * pSrc = reinterpret_cast<const uint16_t *>(pSrcRow);
			uint16_t * pDst = reinterpret_cast<uint16_t *>(pDstRow);
			std::memcpy( pDst, pSrc, _ui32W * sizeof( uint16_t ) );
		}
		m_tIndex->UnlockRect( 0 );
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
	bool CDx9PaletteFilter::EnsureShaders() {
		// Pass 1: indices + LUT -> RGBA (ps_2_0). L16 is normalized to 0..1; un-normalize to 0..65535 then clamp to 0..511.
		static const char * kPsIdxToColorHlsl =
			"sampler2D sIdx : register( s0 );\n"
			"sampler2D sLut : register( s1 );\n"
			"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
			"    float raw = tex2D( sIdx, uv ).r * 65535.0;\n"
			"    float idx = clamp( floor( raw + 0.5 ), 0.0, 511.0 );\n"
			"    float u   = (idx + 0.5) / 512.0;\n"
			"    return tex2D( sLut, float2( u, 0.5 ) );\n"
			"}\n";

		/*static const char* kPsShowIndex =
			"sampler2D sIdx:register(s0);\n"
			"float4 main(float2 uv:TEXCOORD0):COLOR {\n"
			"  float r = tex2D(sIdx, uv).r; // L16 normalized 0..1\n"
			"  return float4(r,r,r,1);\n"
			"}\n";*/

		// Pass 2: vertical nearest-neighbor (ps_2_0). c0 = [srcH, 1/srcH, 0.5, 0].
		static const char * kPsVerticalNNHlsl =
			"sampler2D sSrc : register( s0 );\n"
			"float4 c0 : register( c0 );\n" // x=srcH, y=1/srcH, z=0.5
			"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
			"    float v = (floor( uv.y * c0.x ) + c0.z ) * c0.y; \n"
			"    return tex2D( sSrc, float2( uv.x, v ) );\n"
			"}\n";

		// Pass 3: simple copy (ps_2_0).
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

		if ( !m_psIdxToColor.get() ) {
			m_psIdxToColor = std::make_unique<CDirectX9PixelShader>( m_pdx9dDevice );
			if ( !m_psIdxToColor.get() ) { return false; }
		}
		if ( !m_psVerticalNN.get() ) {
			m_psVerticalNN = std::make_unique<CDirectX9PixelShader>( m_pdx9dDevice );
			if ( !m_psVerticalNN.get() ) { return false; }
		}
		if ( !m_psCopy.get() ) {
			m_psCopy = std::make_unique<CDirectX9PixelShader>( m_pdx9dDevice );
			if ( !m_psCopy.get() ) { return false; }
		}


		if ( !m_psIdxToColor->Valid() ) {
			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsIdxToColorHlsl, "main", "ps_2_0", vBc ) ) { return false; }
			if ( !m_psIdxToColor->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}
		if ( !m_psVerticalNN->Valid() ) {
			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsVerticalNNHlsl, "main", "ps_2_0", vBc ) ) { return false; }
			if ( !m_psVerticalNN->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}
		if ( !m_psCopy->Valid() ) {
			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsCopyHlsl, "main", "ps_2_0", vBc ) ) { return false; }
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
	 * \return Returns true if compilation succeeded and bytecode was produced.
	 */
	bool CDx9PaletteFilter::CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode ) {
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
			nullptr, nullptr,
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
	void CDx9PaletteFilter::ReleaseSizeDependents() {
		if LSN_LIKELY( m_tIndex.get() ) { m_tIndex->Reset(); }
		if LSN_LIKELY( m_rtInitial.get() ) { m_rtInitial->Reset(); }
		if LSN_LIKELY( m_rtScanlined.get() ) { m_rtScanlined->Reset(); }
		if LSN_LIKELY( m_vbQuad.get() ) { m_vbQuad->Reset(); }
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * \brief Renders the three-pass pipeline to the backbuffer with a black border outside _rOutput.
	 *
	 * Precondition: The caller must have already called BeginScene() on the device.
	 * Postcondition: The scene remains open; the caller is responsible for EndScene() and Present().
	 *
	 * Pass 1 renders indexÅ®color into the FP RT with a 1:1 viewport.
	 * Pass 2 renders the FP RT into the scanlined RT (heightÅ~factor) using nearest-neighbor vertically.
	 * Pass 3 clears the backbuffer black and draws the scanlined RT into the destination rectangle.
	 *
	 * \param _rOutput The destination rectangle in client pixels where the NES image should appear.
	 * \return Returns true if the draw succeeded; false on failure.
	 */
	bool CDx9PaletteFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_tIndex.get() || !m_tLut.get() || !m_rtInitial.get() || !m_rtScanlined.get() || !m_psIdxToColor.get() || !m_psVerticalNN.get() || !m_psCopy.get() || !m_vbQuad.get() ) {
			return false;
		}
		if LSN_UNLIKELY( !m_pdx9dDevice || !m_tIndex->Valid() || !m_tLut->Valid() || !m_rtInitial->Valid() || !m_rtScanlined->Valid() || !m_psIdxToColor->Valid() || !m_psVerticalNN->Valid() || !m_psCopy->Valid() || !m_vbQuad->Valid() ) {
			return false;
		}
		IDirect3DDevice9 * pd3d9dDevice = m_pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		// ----- Pass 1: Indices + LUT Å® Initial FP RT (1:1) -----
		{
			IDirect3DSurface9 * pd3ds9Surf = m_rtInitial->GetSurface();
			if LSN_UNLIKELY( !pd3ds9Surf ) { return false; }
			pd3d9dDevice->SetRenderTarget( 0, pd3ds9Surf );
			pd3ds9Surf->Release();
			pd3d9dDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );

			D3DVIEWPORT9 vpViewport{};
			vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = m_ui32SrcW; vpViewport.Height = m_ui32SrcH; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
			pd3d9dDevice->SetViewport( &vpViewport );

			pd3d9dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
			pd3d9dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			pd3d9dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );
			float fU0, fV0, fU1, fV1;
			const uint32_t uiSrcW = m_ui32SrcW;
			const uint32_t uiSrcH = m_ui32SrcH;
			HalfTexelUv( uiSrcW, uiSrcH, fU0, fV0, fU1, fV1 );
			if LSN_UNLIKELY( !FillQuad( (*m_vbQuad), 0.0f, 0.0f, float( m_ui32SrcW ), float( m_ui32SrcH ),
				fU0, fV0, fU1, fV1 ) ) { return false; }
			//if LSN_UNLIKELY( !FillQuad( 0.0f, 0.0f, float( m_ui32SrcW ), float( m_ui32SrcH ), 0.0f, 0.0f, 1.0f, 1.0f ) ) { return false; }
			pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );

			// s0 = index, s1 = LUT (both POINT/CLAMP)
			pd3d9dDevice->SetTexture( 0, m_tIndex->Get() );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			pd3d9dDevice->SetTexture( 1, m_tLut->Get() );
			pd3d9dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			pd3d9dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			pd3d9dDevice->SetPixelShader( m_psIdxToColor->Get() );
			pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		}

		// ----- Pass 2: Initial FP Å® Scanlined FP (height Å~ factor, nearest vertically) -----
		{
			IDirect3DSurface9 * pd3ds9Surf = m_rtScanlined->GetSurface();
			if LSN_UNLIKELY( !pd3ds9Surf ) { return false; }
			pd3d9dDevice->SetRenderTarget( 0, pd3ds9Surf );
			pd3ds9Surf->Release();


			const UINT ui32DstW = m_ui32SrcW * GetActualHorSharpness();
			const UINT ui32DstH = m_ui32SrcH * GetActualVertSharpness();

			D3DVIEWPORT9 vpViewport{};
			vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = ui32DstW; vpViewport.Height = ui32DstH; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
			pd3d9dDevice->SetViewport( &vpViewport );

			pd3d9dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
			pd3d9dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			pd3d9dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );
			float fU0, fV0, fU1, fV1;
			const uint32_t uiSrcW = ui32DstW;
			const uint32_t uiSrcH = ui32DstH;
			HalfTexelUv( uiSrcW, uiSrcH, fU0, fV0, fU1, fV1 );
			if LSN_UNLIKELY( !FillQuad( (*m_vbQuad), 0.0f, 0.0f, float( ui32DstW ), float( ui32DstH ),
				fU0, fV1, fU1, fV0 ) ) { return false; }
			//if LSN_UNLIKELY( !FillQuad( 0.0f, 0.0f, float( ui32DstW ), float( ui32DstH ), 0.0f, 1.0f, 1.0f, 0.0f ) ) { return false; }
			pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );

			// s0 = initial FP RT (POINT/CLAMP).
			pd3d9dDevice->SetTexture( 0, m_rtInitial->Texture()->Get() );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			// Set constants: fC0 = [srcH, 1/srcH, 0.5, 0]
			const float fC0[4] = { float( m_ui32SrcH ), 1.0f / float( m_ui32SrcH ), 0.5f, 0.0f };
			pd3d9dDevice->SetPixelShader( m_psVerticalNN->Get() );
			pd3d9dDevice->SetPixelShaderConstantF( 0, fC0, 1 );
			pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		}

		// ----- Pass 3: Composite to backbuffer inside _rOutput, black elsewhere -----
		{
			IDirect3DSurface9 * pd3ds9Back = nullptr;
			if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3ds9Back ) ) ) {
				pd3d9dDevice->SetRenderTarget( 0, pd3ds9Back );
				pd3ds9Back->Release();
			}

			// Clear can be issued inside an active scene (caller began it).
			pd3d9dDevice->Clear( 0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

			// Full backbuffer viewport; draw pre-transformed quad spanning _rOutput.
			D3DSURFACE_DESC sdBb{};
			IDirect3DSurface9 * pBB = nullptr;
			if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB ) ) ) {
				pBB->GetDesc( &sdBb );
				pBB->Release();
			}
			D3DVIEWPORT9 vpViewport{};
			vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = sdBb.Width; vpViewport.Height = sdBb.Height; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
			pd3d9dDevice->SetViewport( &vpViewport );

			pd3d9dDevice->SetTexture( 0, m_rtScanlined->Texture()->Get() );
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
			//if LSN_UNLIKELY( !FillQuad( float( _rOutput.left ), float( _rOutput.top ), float( _rOutput.right ), float( _rOutput.bottom ), 0.0f, 0.0f, 1.0f, 1.0f ) ) { return false; }



			pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );
			pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		}

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
