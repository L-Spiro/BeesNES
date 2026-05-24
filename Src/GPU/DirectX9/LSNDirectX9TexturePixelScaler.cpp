#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying integer nearest-neighbor scaling and gamma to a texture.
 */

#include "LSNDirectX9TexturePixelScaler.h"
#include "LSNDirectX9DiskInclude.h"

#include <string>

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fRhw;
		float fU, fV;
	};
#pragma pack(pop)
	static constexpr DWORD LSN_FVF_XYZRHWTEX1 = D3DFVF_XYZRHW | D3DFVF_TEX1;


	CDirectX9TexturePixelScaler::CDirectX9TexturePixelScaler() {
	}
	CDirectX9TexturePixelScaler::~CDirectX9TexturePixelScaler() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CDirectX9TexturePixelScaler::Reset() {
		if LSN_LIKELY( m_rtTarget.get() && m_rtTarget->Get() ) { m_rtTarget->Reset(); }
		if LSN_LIKELY( m_vbQuad.get() && m_vbQuad->Get() ) { m_vbQuad->Reset(); }
		m_rtTarget.reset();
		m_vbQuad.reset();
		m_psShader.reset();
		m_gShaderGamma = CNesPalette::LSN_G_NONE;
		m_ui32TargetW = 0;
		m_ui32TargetH = 0;
		m_fFormat = D3DFMT_UNKNOWN;
	}

	/**
	 * Renders the input texture to the internal scaled target.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ptSrc The source texture to draw.
	 * \param _ui32SrcW The width of the source texture.
	 * \param _ui32SrcH The height of the source texture.
	 * \param _ui32ScaleX The horizontal scaling factor.
	 * \param _ui32ScaleY The vertical scaling factor.
	 * \param _gGamma The gamma curve to apply.
	 * \param _bUse16BitTarget If true, a 16-bit target is used, otherwise a 32-bit target is used.
	 * \param _bFlipY If true, the rendered image is flipped vertically.
	 * \return Returns true on success.
	 **/
	bool CDirectX9TexturePixelScaler::Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32ScaleX, uint32_t _ui32ScaleY,
		CNesPalette::LSN_GAMMA _gGamma, bool _bUse16BitTarget, bool _bFlipY ) {
		if LSN_UNLIKELY( !_pdx9dDevice || !_ptSrc || !_ui32SrcW || !_ui32SrcH || !_ui32ScaleX || !_ui32ScaleY ) { return false; }

		const uint32_t ui32DstW = _ui32SrcW * _ui32ScaleX;
		const uint32_t ui32DstH = _ui32SrcH * _ui32ScaleY;
		const D3DFORMAT fFormat = _bUse16BitTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F;

		if LSN_UNLIKELY( !EnsureResources( _pdx9dDevice, ui32DstW, ui32DstH, fFormat ) ) { return false; }
		if LSN_UNLIKELY( !EnsureShader( _pdx9dDevice, _gGamma ) ) { return false; }

		IDirect3DDevice9 * pd3d9dDevice = _pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		IDirect3DSurface9 * pd3ds9Surf = m_rtTarget->GetSurface();
		if LSN_UNLIKELY( !pd3ds9Surf ) { return false; }
		pd3d9dDevice->SetRenderTarget( 0, pd3ds9Surf );
		pd3ds9Surf->Release();

		D3DVIEWPORT9 vpViewport{};
		vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = ui32DstW; vpViewport.Height = ui32DstH; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
		pd3d9dDevice->SetViewport( &vpViewport );

		pd3d9dDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3d9dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		pd3d9dDevice->SetTexture( 0, _ptSrc );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		pd3d9dDevice->SetPixelShader( m_psShader->Get() );
		pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );

		// Constants: c0 = [srcW, srcH, 1/srcW, 1/srcH], c1 = [0.5, 0, 0, 0]
		const float fC0[4] = { float( _ui32SrcW ), float( _ui32SrcH ), 1.0f / float( _ui32SrcW ), 1.0f / float( _ui32SrcH ) };
		pd3d9dDevice->SetPixelShaderConstantF( 0, fC0, 1 );
		const float fC1[4] = { 0.5f, 0.0f, 0.0f, 0.0f };
		pd3d9dDevice->SetPixelShaderConstantF( 1, fC1, 1 );

		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_vbQuad->Lock( 0, 0, reinterpret_cast<void **>(&pvP), D3DLOCK_DISCARD ) ) ) {
			constexpr float fOff = 0.5f;
			float fL = 0.0f - fOff;
			float fT = 0.0f - fOff;
			float fR = static_cast<float>(ui32DstW) - fOff;
			float fB = static_cast<float>(ui32DstH) - fOff;

			float fVTop = _bFlipY ? 1.0f : 0.0f;
			float fVBot = _bFlipY ? 0.0f : 1.0f;

			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, fVTop };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, fVTop };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, fVBot };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, fVBot };
			m_vbQuad->Unlock();
		}

		pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );
		pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		return true;
	}

	/**
	 * Ensures the vertex buffer and render target are created and properly sized.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ui32DstW The target width.
	 * \param _ui32DstH The target height.
	 * \param _fFormat The target format.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX9TexturePixelScaler::EnsureResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32DstW, uint32_t _ui32DstH, D3DFORMAT _fFormat ) {
		if LSN_UNLIKELY( !m_vbQuad.get() || !m_vbQuad->Valid() ) {
			m_vbQuad = std::make_unique<CDirectX9VertexBuffer>( _pdx9dDevice );
			if ( !m_vbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_rtTarget.get() || !m_rtTarget->Valid() || m_ui32TargetW != _ui32DstW || m_ui32TargetH != _ui32DstH || m_fFormat != _fFormat ) {
			if LSN_LIKELY( m_rtTarget.get() && m_rtTarget->Get() ) { m_rtTarget->Reset(); }
			m_rtTarget = std::make_unique<CDirectX9RenderTarget>( _pdx9dDevice );
			if ( !m_rtTarget->CreateColorTarget( _ui32DstW, _ui32DstH, _fFormat ) ) { return false; }
			m_ui32TargetW = _ui32DstW;
			m_ui32TargetH = _ui32DstH;
			m_fFormat = _fFormat;
		}

		return true;
	}

	/**
	 * Ensures the pixel shader is compiled with the correct gamma function.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _gGamma The gamma curve to apply.
	 * \return Returns true if the shader is ready.
	 **/
	bool CDirectX9TexturePixelScaler::EnsureShader( CDirectX9Device * _pdx9dDevice, CNesPalette::LSN_GAMMA _gGamma ) {
		if LSN_LIKELY( m_psShader.get() && m_psShader->Valid() && m_gShaderGamma == _gGamma ) { return true; }

		m_psShader = std::make_unique<CDirectX9PixelShader>( _pdx9dDevice );
		m_gShaderGamma = _gGamma;

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

		std::string sShaderSrc =
			"#include \"LSNGamma.hlsl\"\n"
			"sampler2D sSrc : register( s0 );\n"
			"float4 c0 : register( c0 );\n" // x=srcW, y=srcH, z=1/srcW, w=1/srcH
			"float4 c1 : register( c1 );\n" // x=0.5
			"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
			"    float2 uvy = (floor( uv * c0.xy ) + c1.xx ) * c0.zw;\n"
			"    float4 c = tex2D( sSrc, uvy );\n"
			"    c.rgb = " + sGammaCall + ";\n"
			"    return c;\n"
			"}\n";

		CDirectX9DiskInclude diInclude( CDirectX9DiskInclude::GetExeShadersDir() );
		std::vector<DWORD> vBc;
		if ( !CompileHlslPs( sShaderSrc.c_str(), "main", "ps_2_0", vBc, &diInclude ) ) { return false; }
		if ( !m_psShader->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }

		return true;
	}

	/**
	 * Compiles an HLSL pixel shader using dynamically loaded D3DX.
	 * 
	 * \param _pcszSource Null-terminated HLSL source code.
	 * \param _pcszEntry Null-terminated entry-point function name.
	 * \param _pcszProfile Null-terminated profile.
	 * \param _vOutByteCode Output vector to receive the compiled bytecode.
	 * \param _piInclude Optional #include handler.
	 * \return Returns true if compilation succeeded.
	 **/
	bool CDirectX9TexturePixelScaler::CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode, ID3DXInclude * _piInclude ) {
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
		HRESULT hRes = pfnCompile( _pcszSource, static_cast<UINT>(std::strlen( _pcszSource )), nullptr, _piInclude, _pcszEntry, _pcszProfile, 0, &pbCode, &pbErrs, nullptr );
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

}	// namespace lsn

#endif	// #ifdef LSN_DX9
