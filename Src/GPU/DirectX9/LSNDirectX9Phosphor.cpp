#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 hardware wrapper for a phosphor-decay post-processing effect.
 */

#include "LSNDirectX9Phosphor.h"
#include <string>

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fRhw;
		float fU, fV;
	};
#pragma pack( pop )
	static constexpr DWORD LSN_FVF_XYZRHWTEX1 = D3DFVF_XYZRHW | D3DFVF_TEX1;


	CDirectX9Phosphor::CDirectX9Phosphor() {
	}
	CDirectX9Phosphor::~CDirectX9Phosphor() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CDirectX9Phosphor::Reset() {
		if LSN_LIKELY( m_rtPhosphor[0].get() && m_rtPhosphor[0]->Get() ) { m_rtPhosphor[0]->Reset(); }
		if LSN_LIKELY( m_rtPhosphor[1].get() && m_rtPhosphor[1]->Get() ) { m_rtPhosphor[1]->Reset(); }
		if LSN_LIKELY( m_pvbQuad.get() && m_pvbQuad->Get() ) { m_pvbQuad->Reset(); }
		
		m_rtPhosphor[0].reset();
		m_rtPhosphor[1].reset();
		m_pvbQuad.reset();
		m_psPhosphorOutput.reset();
		m_psPhosphorUpdate.reset();
		
		m_ui32LastSrcW = 0;
		m_ui32LastSrcH = 0;
		m_stReadIndex = 0;
	}

	/**
	 * Renders the input texture to the target surface, updating the internal phosphor decay buffer.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ptSrc The source texture to draw (the current frame).
	 * \param _ui32SrcW The width of the source texture.
	 * \param _ui32SrcH The height of the source texture.
	 * \param _prtDst The destination render target for the visible output.
	 * \param _fInitDecay The initial decay factor applied to the current frame when writing to the buffer.
	 * \param _fDecayR The decay factor for the red channel.
	 * \param _fDecayG The decay factor for the green channel.
	 * \param _fDecayB The decay factor for the blue channel.
	 * \return Returns true on success.
	 **/
	bool CDirectX9Phosphor::Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, CDirectX9RenderTarget * _prtDst, float _fInitDecay, float _fDecayR, float _fDecayG, float _fDecayB ) {
		if LSN_UNLIKELY( !_pdx9dDevice || !_ptSrc || !_prtDst || !_ui32SrcW || !_ui32SrcH ) { return false; }
		if LSN_UNLIKELY( !EnsureResources( _pdx9dDevice, _ui32SrcW, _ui32SrcH ) ) { return false; }

		IDirect3DDevice9 * pd3d9dDevice = _pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		// Standard States.
		pd3d9dDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3d9dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		// Constants: c0 = [DecayR, DecayG, DecayB, InitDecay]
		const float fC0[4] = { _fDecayR, _fDecayG, _fDecayB, _fInitDecay };
		pd3d9dDevice->SetPixelShaderConstantF( 0, fC0, 1 );

		pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );

		// Source Image is s0. History is s1.
		pd3d9dDevice->SetTexture( 0, _ptSrc );
		pd3d9dDevice->SetTexture( 1, m_rtPhosphor[m_stReadIndex]->Texture()->Get() );

		// ---- Pass 1: Render Visible Composite to _prtDst ----
		IDirect3DSurface9 * pd3ds9SurfDst = _prtDst->GetSurface();
		if LSN_UNLIKELY( !pd3ds9SurfDst ) { return false; }
		pd3d9dDevice->SetRenderTarget( 0, pd3ds9SurfDst );
		pd3ds9SurfDst->Release();

		D3DVIEWPORT9 vpViewport{};
		vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = _ui32SrcW; vpViewport.Height = _ui32SrcH; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
		pd3d9dDevice->SetViewport( &vpViewport );

		pd3d9dDevice->SetPixelShader( m_psPhosphorOutput->Get() );
		if LSN_UNLIKELY( !DrawQuad( pd3d9dDevice, _ui32SrcW, _ui32SrcH ) ) { return false; }


		// ---- Pass 2: Update Phosphor History Buffer ----
		size_t stWriteIndex = m_stReadIndex ^ 1;
		IDirect3DSurface9 * pd3ds9SurfUpdate = m_rtPhosphor[stWriteIndex]->GetSurface();
		if LSN_UNLIKELY( !pd3ds9SurfUpdate ) { return false; }
		pd3d9dDevice->SetRenderTarget( 0, pd3ds9SurfUpdate );
		pd3ds9SurfUpdate->Release();

		pd3d9dDevice->SetPixelShader( m_psPhosphorUpdate->Get() );
		if LSN_UNLIKELY( !DrawQuad( pd3d9dDevice, _ui32SrcW, _ui32SrcH ) ) { return false; }

		// Swap read/write for the next frame.
		m_stReadIndex = stWriteIndex;

		return true;
	}

	/**
	 * Ensures the vertex buffer, render targets, and shaders are correctly built.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ui32SrcW The source width.
	 * \param _ui32SrcH The source height.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX9Phosphor::EnsureResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32SrcW, uint32_t _ui32SrcH ) {
		IDirect3DDevice9 * pd3d9dDevice = _pdx9dDevice->GetDirectX9Device();

		if LSN_UNLIKELY( !m_pvbQuad.get() || !m_pvbQuad->Valid() ) {
			m_pvbQuad = std::make_unique<CDirectX9VertexBuffer>( _pdx9dDevice );
			if ( !m_pvbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_rtPhosphor[0].get() || !m_rtPhosphor[0]->Valid() || !m_rtPhosphor[1].get() || !m_rtPhosphor[1]->Valid() || m_ui32LastSrcW != _ui32SrcW || m_ui32LastSrcH != _ui32SrcH ) {
			for ( size_t I = 0; I < 2; ++I ) {
				if LSN_LIKELY( m_rtPhosphor[I].get() && m_rtPhosphor[I]->Get() ) { m_rtPhosphor[I]->Reset(); }
				m_rtPhosphor[I] = std::make_unique<CDirectX9RenderTarget>( _pdx9dDevice );
				
				// D3DFMT_A16B16G16R16F prevents banding and snapping in the decay trails.
				if ( !m_rtPhosphor[I]->CreateColorTarget( _ui32SrcW, _ui32SrcH, D3DFMT_A16B16G16R16F ) ) { return false; }

				// Initial clear to black.
				IDirect3DSurface9 * pSurf = m_rtPhosphor[I]->GetSurface();
				if ( pSurf ) {
					IDirect3DSurface9 * pOldSurf = nullptr;
					pd3d9dDevice->GetRenderTarget( 0, &pOldSurf );
					pd3d9dDevice->SetRenderTarget( 0, pSurf );
					pd3d9dDevice->Clear( 0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 );
					if ( pOldSurf ) {
						pd3d9dDevice->SetRenderTarget( 0, pOldSurf );
						pOldSurf->Release();
					}
					pSurf->Release();
				}
			}

			m_ui32LastSrcW = _ui32SrcW;
			m_ui32LastSrcH = _ui32SrcH;
			m_stReadIndex = 0;
		}

		if LSN_UNLIKELY( !m_psPhosphorOutput.get() || !m_psPhosphorOutput->Valid() ) {
			m_psPhosphorOutput = std::make_unique<CDirectX9PixelShader>( _pdx9dDevice );
			
			// Output to screen: max(oldDecayed, current)
			static const char * kPsOutputHlsl =
				"sampler2D sSrc : register( s0 );\n"
				"sampler2D sHistory : register( s1 );\n"
				"float4 c0 : register( c0 );\n"
				"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
				"    float4 curColor = tex2D( sSrc, uv );\n"
				"    float4 oldColor = tex2D( sHistory, uv );\n"
				"    float3 oldDecayed = oldColor.rgb * c0.xyz;\n"
				"    return float4( max( oldDecayed, curColor.rgb ), curColor.a );\n"
				"}\n";

			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsOutputHlsl, "main", "ps_3_0", vBc ) ) { return false; }
			if ( !m_psPhosphorOutput->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_psPhosphorUpdate.get() || !m_psPhosphorUpdate->Valid() ) {
			m_psPhosphorUpdate = std::make_unique<CDirectX9PixelShader>( _pdx9dDevice );
			
			// Update buffer: max(scaledCurrent, oldDecayed)
			static const char * kPsUpdateHlsl =
				"sampler2D sSrc : register( s0 );\n"
				"sampler2D sHistory : register( s1 );\n"
				"float4 c0 : register( c0 );\n"
				"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
				"    float4 curColor = tex2D( sSrc, uv );\n"
				"    float4 oldColor = tex2D( sHistory, uv );\n"
				"    float3 oldDecayed = oldColor.rgb * c0.xyz;\n"
				"    float3 scaledCurrent = curColor.rgb * c0.w;\n"
				"    return float4( max( scaledCurrent, oldDecayed ), curColor.a );\n"
				"}\n";

			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsUpdateHlsl, "main", "ps_3_0", vBc ) ) { return false; }
			if ( !m_psPhosphorUpdate->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}

		return true;
	}

	/**
	 * Renders a full-size quad to the currently bound render target.
	 * 
	 * \param _pd3d9dDevice The raw IDirect3DDevice9 pointer.
	 * \param _ui32W The width of the viewport.
	 * \param _ui32H The height of the viewport.
	 * \return Returns true on success.
	 **/
	bool CDirectX9Phosphor::DrawQuad( IDirect3DDevice9 * _pd3d9dDevice, uint32_t _ui32W, uint32_t _ui32H ) {
		LSN_XYZRHWTEX1 * pvP = nullptr;
		if ( SUCCEEDED( m_pvbQuad->Lock( 0, 0, reinterpret_cast<void **>(&pvP), D3DLOCK_DISCARD ) ) ) {
			constexpr float fOff = 0.5f;
			float fL = 0.0f - fOff;
			float fT = 0.0f - fOff;
			float fR = static_cast<float>(_ui32W) - fOff;
			float fB = static_cast<float>(_ui32H) - fOff;

			pvP[0] = { fL, fT, 0.0f, 1.0f, 0.0f, 0.0f };
			pvP[1] = { fR, fT, 0.0f, 1.0f, 1.0f, 0.0f };
			pvP[2] = { fL, fB, 0.0f, 1.0f, 0.0f, 1.0f };
			pvP[3] = { fR, fB, 0.0f, 1.0f, 1.0f, 1.0f };
			m_pvbQuad->Unlock();
		}

		_pd3d9dDevice->SetStreamSource( 0, m_pvbQuad->Get(), 0, sizeof( LSN_XYZRHWTEX1 ) );
		_pd3d9dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
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
	bool CDirectX9Phosphor::CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode ) {
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
