#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying a gamma curve to a texture.
 */

#include "LSNDirectX9TextureGamma.h"
#include "LSNDirectX9DiskInclude.h"

#include <string>

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fRhw;
		float fU, fV;
	};
#pragma pack( pop )
	static constexpr DWORD LSN_FVF_XYZRHWTEX1 = D3DFVF_XYZRHW | D3DFVF_TEX1;


	CDirectX9TextureGamma::CDirectX9TextureGamma() {
	}
	CDirectX9TextureGamma::~CDirectX9TextureGamma() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CDirectX9TextureGamma::Reset() {
		if LSN_LIKELY( m_vbQuad.get() && m_vbQuad->Get() ) { m_vbQuad->Reset(); }
		m_vbQuad.reset();
		m_psShader.reset();
		m_gShaderGamma = CNesPalette::LSN_G_NONE;
	}

	/**
	 * Renders the input texture to the target, applying the selected gamma curve.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ptSrc The source texture to draw.
	 * \param _ui32SrcW The width of the source texture.
	 * \param _ui32SrcH The height of the source texture.
	 * \param _prtDst The destination render target.
	 * \param _gGamma The gamma curve to apply.
	 * \return Returns true on success.
	 **/
	bool CDirectX9TextureGamma::Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, CDirectX9RenderTarget * _prtDst, CNesPalette::LSN_GAMMA _gGamma ) {
		if LSN_UNLIKELY( !_pdx9dDevice || !_ptSrc || !_prtDst || !_ui32SrcW || !_ui32SrcH ) { return false; }

		if LSN_UNLIKELY( !EnsureResources( _pdx9dDevice ) ) { return false; }
		if LSN_UNLIKELY( !EnsureShader( _pdx9dDevice, _gGamma ) ) { return false; }

		IDirect3DDevice9 * pd3d9dDevice = _pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		IDirect3DSurface9 * pd3ds9Surf = _prtDst->GetSurface();
		if LSN_UNLIKELY( !pd3ds9Surf ) { return false; }
		pd3d9dDevice->SetRenderTarget( 0, pd3ds9Surf );
		pd3ds9Surf->Release();

		D3DVIEWPORT9 vpViewport{};
		vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = _ui32SrcW; vpViewport.Height = _ui32SrcH; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
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

		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_vbQuad->Lock( 0, 0, reinterpret_cast<void **>(&pvP), D3DLOCK_DISCARD ) ) ) {
			constexpr float fOff = 0.5f;
			float fL = 0.0f - fOff;
			float fT = 0.0f - fOff;
			float fR = static_cast<float>(_ui32SrcW) - fOff;
			float fB = static_cast<float>(_ui32SrcH) - fOff;

			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, 0.0f };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, 0.0f };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, 1.0f };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, 1.0f };
			m_vbQuad->Unlock();
		}

		pd3d9dDevice->SetStreamSource( 0, m_vbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );
		pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		return true;
	}

	/**
	 * Ensures the vertex buffer is created.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX9TextureGamma::EnsureResources( CDirectX9Device * _pdx9dDevice ) {
		if LSN_UNLIKELY( !m_vbQuad.get() || !m_vbQuad->Valid() ) {
			m_vbQuad = std::make_unique<CDirectX9VertexBuffer>( _pdx9dDevice );
			if ( !m_vbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }
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
	bool CDirectX9TextureGamma::EnsureShader( CDirectX9Device * _pdx9dDevice, CNesPalette::LSN_GAMMA _gGamma ) {
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
			default : { break; }
		}

		std::string sShaderSrc =
			"#include \"LSNGamma.hlsl\"\n"
			"sampler2D sSrc : register( s0 );\n"
			"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
			"    float4 c = tex2D( sSrc, uv );\n"
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
	bool CDirectX9TextureGamma::CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode, ID3DXInclude * _piInclude ) {
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
