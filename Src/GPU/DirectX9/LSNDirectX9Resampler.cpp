#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 hardware wrapper for 2-pass GPU resampling.
 */

#include "LSNDirectX9Resampler.h"
#include <algorithm>
#include <string>

namespace lsn {

#pragma pack( push, 1 )
	struct LSN_XYZRHWTEX1 {
		float fX, fY, fZ, fRhw;
		float fU, fV;
	};
#pragma pack( pop )
	static constexpr DWORD LSN_FVF_XYZRHWTEX1 = D3DFVF_XYZRHW | D3DFVF_TEX1;


	CDirectX9Resampler::CDirectX9Resampler() {
	}
	CDirectX9Resampler::~CDirectX9Resampler() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CDirectX9Resampler::Reset() {
		if LSN_LIKELY( m_rtIntermediate.get() && m_rtIntermediate->Get() ) { m_rtIntermediate->Reset(); }
		if LSN_LIKELY( m_ptLutX.get() && m_ptLutX->Get() ) { m_ptLutX->Reset(); }
		if LSN_LIKELY( m_ptLutY.get() && m_ptLutY->Get() ) { m_ptLutY->Reset(); }
		if LSN_LIKELY( m_pvbQuad.get() && m_pvbQuad->Get() ) { m_pvbQuad->Reset(); }
		
		m_rtIntermediate.reset();
		m_ptLutX.reset();
		m_ptLutY.reset();
		m_pvbQuad.reset();
		m_psResampleX.reset();
		m_psResampleY.reset();
		
		m_ui32LastSrcW = 0;
		m_ui32LastSrcH = 0;
		m_ui32LastDstW = 0;
		m_ui32LastDstH = 0;
		m_ui32MaxTapsX = 0;
		m_ui32MaxTapsY = 0;
	}

	/**
	 * Renders the input texture to the target surface utilizing a 2-pass resampling algorithm.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ptSrc The source texture to draw.
	 * \param _ui32SrcW The width of the source texture.
	 * \param _ui32SrcH The height of the source texture.
	 * \param _prtDst The destination render target.
	 * \param _ui32DstW The target width.
	 * \param _ui32DstH The target height.
	 * \return Returns true on success.
	 **/
	bool CDirectX9Resampler::Render( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32SrcW, uint32_t _ui32SrcH, CDirectX9RenderTarget * _prtDst, uint32_t _ui32DstW, uint32_t _ui32DstH ) {
		if LSN_UNLIKELY( !_pdx9dDevice || !_ptSrc || !_prtDst || !_ui32SrcW || !_ui32SrcH || !_ui32DstW || !_ui32DstH ) { return false; }
		if LSN_UNLIKELY( !EnsureResources( _pdx9dDevice, _ui32SrcW, _ui32SrcH, _ui32DstW, _ui32DstH ) ) { return false; }

		IDirect3DDevice9 * pd3d9dDevice = _pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }

		// Standard States.
		pd3d9dDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3d9dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3d9dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		
		// All lookups inside the shaders require precise point sampling.
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3d9dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3d9dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		// ---- Pass 1: Horizontal Resampling ----
		IDirect3DSurface9 * pd3ds9SurfIntermediate = m_rtIntermediate->GetSurface();
		if LSN_UNLIKELY( !pd3ds9SurfIntermediate ) { return false; }
		pd3d9dDevice->SetRenderTarget( 0, pd3ds9SurfIntermediate );
		pd3ds9SurfIntermediate->Release();

		D3DVIEWPORT9 vpViewport{};
		vpViewport.X = 0; vpViewport.Y = 0; vpViewport.Width = _ui32DstW; vpViewport.Height = _ui32SrcH; vpViewport.MinZ = 0.0f; vpViewport.MaxZ = 1.0f;
		pd3d9dDevice->SetViewport( &vpViewport );

		pd3d9dDevice->SetTexture( 0, _ptSrc );
		pd3d9dDevice->SetTexture( 1, m_ptLutX->Get() );

		pd3d9dDevice->SetPixelShader( m_psResampleX->Get() );
		pd3d9dDevice->SetFVF( LSN_FVF_XYZRHWTEX1 );

		// Constants: c0 = [MaxTaps, 1 / MaxTaps, 0, 0]
		const float fC0X[4] = { static_cast<float>(m_ui32MaxTapsX), 1.0f / static_cast<float>(m_ui32MaxTapsX), 0.0f, 0.0f };
		pd3d9dDevice->SetPixelShaderConstantF( 0, fC0X, 1 );

		if LSN_UNLIKELY( !DrawQuad( pd3d9dDevice, _ui32DstW, _ui32SrcH ) ) { return false; }

		// ---- Pass 2: Vertical Resampling ----
		IDirect3DSurface9 * pd3ds9SurfOut = _prtDst->GetSurface();
		if LSN_UNLIKELY( !pd3ds9SurfOut ) { return false; }
		pd3d9dDevice->SetRenderTarget( 0, pd3ds9SurfOut );
		pd3ds9SurfOut->Release();

		vpViewport.Width = _ui32DstW; 
		vpViewport.Height = _ui32DstH;
		pd3d9dDevice->SetViewport( &vpViewport );

		pd3d9dDevice->SetTexture( 0, m_rtIntermediate->Texture()->Get() );
		pd3d9dDevice->SetTexture( 1, m_ptLutY->Get() );

		pd3d9dDevice->SetPixelShader( m_psResampleY->Get() );

		const float fC0Y[4] = { static_cast<float>(m_ui32MaxTapsY), 1.0f / static_cast<float>(m_ui32MaxTapsY), 0.0f, 0.0f };
		pd3d9dDevice->SetPixelShaderConstantF( 0, fC0Y, 1 );

		if LSN_UNLIKELY( !DrawQuad( pd3d9dDevice, _ui32DstW, _ui32DstH ) ) { return false; }

		return true;
	}

	/**
	 * Ensures the vertex buffer, render targets, LUTs, and shaders are correctly built.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ui32SrcW The source width.
	 * \param _ui32SrcH The source height.
	 * \param _ui32DstW The target width.
	 * \param _ui32DstH The target height.
	 * \return Returns true if resources are ready.
	 **/
	bool CDirectX9Resampler::EnsureResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32DstW, uint32_t _ui32DstH ) {
		if LSN_UNLIKELY( !m_pvbQuad.get() || !m_pvbQuad->Valid() ) {
			m_pvbQuad = std::make_unique<CDirectX9VertexBuffer>( _pdx9dDevice );
			if ( !m_pvbQuad->CreateVertexBuffer( sizeof( LSN_XYZRHWTEX1 ) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LSN_FVF_XYZRHWTEX1, D3DPOOL_DEFAULT ) ) { return false; }
		}

		// Ensure intermediate float32 texture fits Pass 1 output (DstW x SrcH).
		if LSN_UNLIKELY( !m_rtIntermediate.get() || !m_rtIntermediate->Valid() || m_ui32LastDstW != _ui32DstW || m_ui32LastSrcH != _ui32SrcH ) {
			if LSN_LIKELY( m_rtIntermediate.get() && m_rtIntermediate->Get() ) { m_rtIntermediate->Reset(); }
			m_rtIntermediate = std::make_unique<CDirectX9RenderTarget>( _pdx9dDevice );
			if ( !m_rtIntermediate->CreateColorTarget( _ui32DstW, _ui32SrcH, D3DFMT_A32B32G32R32F ) ) { return false; }
		}

		// Check if Horizontal LUT needs to be rebuilt.
		if LSN_UNLIKELY( !m_ptLutX.get() || !m_ptLutX->Valid() || m_ui32LastSrcW != _ui32SrcW || m_ui32LastDstW != _ui32DstW ) {
			if ( !BuildLUT( _pdx9dDevice, _ui32SrcW, _ui32DstW, m_ptLutX, m_ui32MaxTapsX ) ) { return false; }
			m_ui32LastSrcW = _ui32SrcW;
			m_ui32LastDstW = _ui32DstW;
		}

		// Check if Vertical LUT needs to be rebuilt.
		if LSN_UNLIKELY( !m_ptLutY.get() || !m_ptLutY->Valid() || m_ui32LastSrcH != _ui32SrcH || m_ui32LastDstH != _ui32DstH ) {
			if ( !BuildLUT( _pdx9dDevice, _ui32SrcH, _ui32DstH, m_ptLutY, m_ui32MaxTapsY ) ) { return false; }
			m_ui32LastSrcH = _ui32SrcH;
			m_ui32LastDstH = _ui32DstH;
		}

		// Compile shaders if missing. Use Shader Model 3.0 (ps_3_0) to support dynamic texture loops.
		if LSN_UNLIKELY( !m_psResampleX.get() || !m_psResampleX->Valid() ) {
			m_psResampleX = std::make_unique<CDirectX9PixelShader>( _pdx9dDevice );
			
			static const char * kPsResampleXHlsl =
				"sampler2D sSrc : register( s0 );\n"
				"sampler2D sLut : register( s1 );\n"
				"float4 c0 : register( c0 );\n"
				"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
				"    float4 cFinal = float4( 0.0, 0.0, 0.0, 0.0 );\n"
				"    int iMaxTaps = (int)c0.x;\n"
				"    for ( int i = 0; i < 256; ++i ) {\n"
				"        if ( i >= iMaxTaps ) { break; }\n"
				"        float fLutY = (float( i ) + 0.5) * c0.y;\n"
				"        float4 vLut = tex2Dlod( sLut, float4( uv.x, fLutY, 0.0, 0.0 ) );\n"
				"        cFinal += tex2Dlod( sSrc, float4( vLut.g, uv.y, 0.0, 0.0 ) ) * vLut.r;\n"
				"    }\n"
				"    return cFinal;\n"
				"}\n";

			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsResampleXHlsl, "main", "ps_3_0", vBc ) ) { return false; }
			if ( !m_psResampleX->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}

		if LSN_UNLIKELY( !m_psResampleY.get() || !m_psResampleY->Valid() ) {
			m_psResampleY = std::make_unique<CDirectX9PixelShader>( _pdx9dDevice );
			
			static const char * kPsResampleYHlsl =
				"sampler2D sSrc : register( s0 );\n"
				"sampler2D sLut : register( s1 );\n"
				"float4 c0 : register( c0 );\n"
				"float4 main( float2 uv : TEXCOORD0 ) : COLOR {\n"
				"    float4 cFinal = float4( 0.0, 0.0, 0.0, 0.0 );\n"
				"    int iMaxTaps = (int)c0.x;\n"
				"    for ( int i = 0; i < 256; ++i ) {\n"
				"        if ( i >= iMaxTaps ) { break; }\n"
				"        float fLutY = (float( i ) + 0.5) * c0.y;\n"
				"        float4 vLut = tex2Dlod( sLut, float4( uv.y, fLutY, 0.0, 0.0 ) );\n"
				"        cFinal += tex2Dlod( sSrc, float4( uv.x, vLut.g, 0.0, 0.0 ) ) * vLut.r;\n"
				"    }\n"
				"    return cFinal;\n"
				"}\n";

			std::vector<DWORD> vBc;
			if ( !CompileHlslPs( kPsResampleYHlsl, "main", "ps_3_0", vBc ) ) { return false; }
			if ( !m_psResampleY->CreateFromByteCode( vBc.data(), vBc.size() ) ) { return false; }
		}

		return true;
	}

	/**
	 * Builds a 1-D Look-Up Texture containing weights and source indices.
	 * 
	 * \param _pdx9dDevice The Direct3D 9 device.
	 * \param _ui32SrcSize The size of the source dimension.
	 * \param _ui32DstSize The size of the destination dimension.
	 * \param _ptLut The unique pointer holding the texture to be created.
	 * \param _ui32OutMaxTaps Reference to store the maximum kernel size determined during generation.
	 * \return Returns true if the LUT was successfully built.
	 **/
	bool CDirectX9Resampler::BuildLUT( CDirectX9Device * _pdx9dDevice, uint32_t _ui32SrcSize, uint32_t _ui32DstSize, std::unique_ptr<CDirectX9Texture> &_ptLut, uint32_t &_ui32OutMaxTaps ) {
		if LSN_UNLIKELY( !CreateContribList( _ui32SrcSize, _ui32DstSize, LSN_TA_CLAMP, CResamplerBase::m_fFilter[m_ffFilter].pfFunc, CResamplerBase::m_fFilter[m_ffFilter].fSupport, 1.0f ) ) { return false; }

		uint32_t ui32MaxTaps = 0;
		for ( size_t I = 0; I < m_cContribs.size(); ++I ) {
			ui32MaxTaps = std::max( ui32MaxTaps, static_cast<uint32_t>(m_cContribs[I].fContributions.size()) );
		}

		if LSN_LIKELY( _ptLut.get() && _ptLut->Get() ) { _ptLut->Reset(); }
		_ptLut = std::make_unique<CDirectX9Texture>( _pdx9dDevice );
		
		if ( !_ptLut->Create2D( _ui32DstSize, ui32MaxTaps, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT ) ) { return false; }

		D3DLOCKED_RECT lrRect{};
		if ( !_ptLut->LockRect( 0, lrRect, nullptr, D3DLOCK_DISCARD ) || !lrRect.pBits ) { return false; }

		for ( uint32_t Y = 0; Y < ui32MaxTaps; ++Y ) {
			float * pfRow = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(lrRect.pBits) + Y * lrRect.Pitch);
			for ( uint32_t X = 0; X < _ui32DstSize; ++X ) {
				if ( Y < m_cContribs[X].fContributions.size() ) {
					pfRow[X*4+0] = m_cContribs[X].fContributions[Y]; // R: Weight
					pfRow[X*4+1] = (static_cast<float>(m_cContribs[X].i32Indices[Y]) + 0.5f) / static_cast<float>(_ui32SrcSize); // G: Source UV Coordinate
				}
				else {
					pfRow[X*4+0] = 0.0f;
					pfRow[X*4+1] = 0.0f;
				}
				pfRow[X*4+2] = 0.0f; // B
				pfRow[X*4+3] = 0.0f; // A
			}
		}

		_ptLut->UnlockRect( 0 );

		m_cContribs.clear();
		m_cContribs.shrink_to_fit();

		_ui32OutMaxTaps = ui32MaxTaps;
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
	bool CDirectX9Resampler::DrawQuad( IDirect3DDevice9 * _pd3d9dDevice, uint32_t _ui32W, uint32_t _ui32H ) {
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
	bool CDirectX9Resampler::CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode ) {
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
