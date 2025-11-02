#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 pixel shader resource.
 */

#include "LSNDirectX9PixelShader.h"

namespace lsn {

	CDirectX9PixelShader::CDirectX9PixelShader( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ) {
	}
	CDirectX9PixelShader::~CDirectX9PixelShader() {
		Reset();
	}

	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9PixelShader::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pd3dps9Shader ) { return; }
		CDirectX9::SafeRelease( m_pd3dps9Shader );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9PixelShader::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		if ( !m_vByteCode.empty() ) {
			return CreateInternalFromByteCode( m_vByteCode.data(), m_vByteCode.size() );
		}
		if ( !m_vAsm.empty() ) {
			return AssembleAndCreate( m_vAsm.data() );
		}
		return false;
	}

	/**
	 * Resets everything to scratch.
	 */
	void CDirectX9PixelShader::Reset() {
		CDirectX9::SafeRelease( m_pd3dps9Shader );
		m_vByteCode.clear();
		m_vAsm.clear();
		m_bResourceCanBeLost = false;
	}

	/**
	 * Creates the pixel shader from assembled bytecode.
	 */
	bool CDirectX9PixelShader::CreateFromByteCode( const DWORD * _pdwByteCode, size_t _stDwords ) {
		Reset();
		if ( !CreateInternalFromByteCode( _pdwByteCode, _stDwords ) ) { return false; }
		// Cache for reset.
		m_vByteCode.assign( _pdwByteCode, _pdwByteCode + _stDwords );
		m_bResourceCanBeLost = true;
		return true;
	}

	/**
	 * Creates the pixel shader from ASCII assembly (ps_2_0, etc.).
	 */
	bool CDirectX9PixelShader::CreateFromAsm( const char * _pszAsm ) {
		Reset();
		if ( !AssembleAndCreate( _pszAsm ) ) { return false; }
		// Cache the ASCII so we can re-assemble on reset.
		m_vAsm.assign( _pszAsm, _pszAsm + std::strlen( _pszAsm ) + 1 );
		m_bResourceCanBeLost = true;
		return true;
	}

	/**
	 * \brief Creates the pixel shader from assembled bytecode.
	 * 
	 * \param _pdwByteCode Pointer to DWORD stream of assembled shader.
	 * \param _stDwords Number of DWORDs in \p _pdwByteCode.
	 * \return Returns true if the shader was created.
	 */
	bool CDirectX9PixelShader::CreateInternalFromByteCode( const DWORD * _pdwByteCode, size_t /*_stDwords*/ ) {
		if ( !m_pdx9dDevice || !m_pdx9dDevice->GetDirectX9Device() ) { return false; }
		IDirect3DDevice9 * pd3dDev = m_pdx9dDevice->GetDirectX9Device();
		return SUCCEEDED( pd3dDev->CreatePixelShader( _pdwByteCode, &m_pd3dps9Shader ) );
	}

	/**
	 * \brief Assembles ASCII shader text and creates the pixel shader.
	 * 
	 * \param _pszAsm Null-terminated ASCII assembly (e.g., ps_2_0).
	 * \return Returns true if assembly and creation both succeeded.
	 */
	bool CDirectX9PixelShader::AssembleAndCreate( const char * /*_pszAsm*/ ) {
		// If we have a D3DX loader.
		// This stub expects pre-assembled bytecode or an external assembler.
		// For now, fail explicitly to avoid silent success.
		//(void)_pszAsm;
		return false;
	}

} // namespace lsn

#endif // LSN_DX9
