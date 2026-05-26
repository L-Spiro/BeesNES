#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for rendering a texture to a surface with bilinear sampling and gamma correction.
 */

#include "LSNDirectX9TextureRenderer.h"

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fRhw;
		float fU, fV;
	};
#pragma pack(pop)
	static constexpr DWORD LSN_FVF_XYZRHWTEX1 = D3DFVF_XYZRHW | D3DFVF_TEX1;


	CDirectX9TextureRenderer::CDirectX9TextureRenderer() {
	}
	CDirectX9TextureRenderer::~CDirectX9TextureRenderer() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the vertex buffer, shaders, and internal states.
	 **/
	void CDirectX9TextureRenderer::Reset() {
		if LSN_LIKELY( m_pvbQuad.get() && m_pvbQuad->Get() ) { m_pvbQuad->Reset(); }
		m_pvbQuad.reset();
		m_ppsCopy.reset();
	}

	/**
	 * Renders the input texture to the target surface.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ptSrc The source texture to draw.
	 * \param _psDst The destination surface (e.g., the swap chain backbuffer).
	 * \param _rOutput The destination rectangle in client pixels.
	 * \param _fGamma The PC monitor's gamma parameter.
	 * \param _bClear If true, clears the destination surface to black before rendering.
	 * \param _bSrgb Enable or disable hardware sRGB conversion.
	 * \return Returns true on success.
	 **/
	bool CDirectX9TextureRenderer::Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, IDirect3DSurface9 * _psDst, const lsw::LSW_RECT &_rOutput, float _fGamma, bool _bClear, bool _bSrgb ) {
		if LSN_UNLIKELY( !_pdx9dDevice || !_ptSrc || !_psDst ) { return false; }
		if LSN_UNLIKELY( !EnsureResources( _pdx9dDevice ) ) { return false; }

		IDirect3DDevice9 * pd3d9dDevice = _pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		pd3d9dDevice->SetRenderTarget( 0, _psDst );

		if ( _bClear ) {
			pd3d9dDevice->Clear( 0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );
		}

		D3DSURFACE_DESC sdBb{};
		_psDst->GetDesc( &sdBb );

		D3DVIEWPORT9 vpViewport{};
		vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = sdBb.Width; vpViewport.Height = sdBb.Height; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
		pd3d9dDevice->SetViewport( &vpViewport );

		pd3d9dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3d9dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		
		pd3d9dDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, _bSrgb ? TRUE : FALSE );

		pd3d9dDevice->SetTexture( 0, _ptSrc );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		pd3d9dDevice->SetPixelShader( m_ppsCopy->Get() );
		pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );

		const float fC0[4] = { _fGamma, 0.0f, 0.0f, 0.0f };
		pd3d9dDevice->SetPixelShaderConstantF( 0, fC0, 1 );

		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_pvbQuad->Lock( 0, 0, reinterpret_cast<void **>(&pvP), D3DLOCK_DISCARD ) ) ) {
			constexpr float fOff = 0.5f;
			float fL = static_cast<float>(_rOutput.left) - fOff;
			float fT = static_cast<float>(_rOutput.top) - fOff;
			float fR = static_cast<float>(_rOutput.right) - fOff;
			float fB = static_cast<float>(_rOutput.bottom) - fOff;

			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, 0.0f };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, 0.0f };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, 1.0f };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, 1.0f };
			m_pvbQuad->Unlock();
		}

		pd3d9dDevice->SetStreamSource( 0, m_pvbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );
		pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		return true;
	}

	/**
	 * Ensures the vertex buffer and pixel shaders are created.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX9TextureRenderer::EnsureResources( CDirectX9Device * _pdx9dDevice ) {
		if LSN_UNLIKELY( !m_pvbQuad.get() || !m_pvbQuad->Valid() ) {
			m_pvbQuad = std::make_unique<CDirectX9VertexBuffer>( _pdx9dDevice );
			if ( !m_pvbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_ppsCopy.get() ) {
			m_ppsCopy = std::make_unique<CDirectX9PixelShader>( _pdx9dDevice );
		}

		if LSN_UNLIKELY( !m_ppsCopy->Valid() ) {
			static const char * kPsCopyHlsl =
				"sampler2D sSrc : register( s0 );\n"
				"float4 c0 : register( c0 );\n" // c0.x = gamma parameter (for future curves)
				"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
				"  float4 c = tex2D( sSrc, uv );\n"
				"  return saturate( c );\n"
				"}\n";

			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsCopyHlsl, "main", "ps_2_0", vBc ) ) { return false; }
			if ( !m_ppsCopy->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}

		return true;
	}

	/**
	 * Compiles an HLSL pixel shader using dynamically loaded D3DX.
	 * 
	 * \param _pcszSource Null-terminated HLSL source code.
	 * \param _pcszEntry Null-terminated entry-point function name.
	 * \param _pcszProfile Null-terminated profile.
	 * \param _vOutByteCode Output vector to receive the compiled bytecode.
	 * \return Returns true if compilation succeeded.
	 **/
	bool CDirectX9TextureRenderer::CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode ) {
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
		HRESULT hRes = pfnCompile( _pcszSource, static_cast<UINT>(std::strlen( _pcszSource )), nullptr, nullptr, _pcszEntry, _pcszProfile, 0, &pbCode, &pbErrs, nullptr );
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
