#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 vertex shader resource.
 */

#include "LSNDirectX9VertexShader.h"

namespace lsn {

	CDirectX9VertexShader::CDirectX9VertexShader( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ) {
	}
	CDirectX9VertexShader::~CDirectX9VertexShader() {
		Reset();
	}

	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9VertexShader::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pd3dvs9Shader ) { return; }
		CDirectX9::SafeRelease( m_pd3dvs9Shader );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9VertexShader::OnDeviceReset() {
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
	void CDirectX9VertexShader::Reset() {
		CDirectX9::SafeRelease( m_pd3dvs9Shader );
		m_vByteCode.clear();
		m_vAsm.clear();
		m_bResourceCanBeLost = false;
	}

	/**
	 * Creates the vertex shader from assembled bytecode.
	 */
	bool CDirectX9VertexShader::CreateFromByteCode( const DWORD * _pdwByteCode, size_t _stDwords ) {
		Reset();
		if ( !CreateInternalFromByteCode( _pdwByteCode, _stDwords ) ) { return false; }
		m_vByteCode.assign( _pdwByteCode, _pdwByteCode + _stDwords );
		m_bResourceCanBeLost = true;
		return true;
	}

	/**
	 * Creates the vertex shader from ASCII assembly.
	 */
	bool CDirectX9VertexShader::CreateFromAsm( const char * _pszAsm ) {
		Reset();
		if ( !AssembleAndCreate( _pszAsm ) ) { return false; }
		m_vAsm.assign( _pszAsm, _pszAsm + std::strlen( _pszAsm ) + 1 );
		m_bResourceCanBeLost = true;
		return true;
	}

	/**
	 * \brief Creates the vertex shader from assembled bytecode.
	 * 
	 * \param _pdwByteCode Pointer to DWORD stream of assembled shader.
	 * \param _stDwords Number of DWORDs in \p _pdwByteCode.
	 * \return Returns true if the shader was created.
	 */
	bool CDirectX9VertexShader::CreateInternalFromByteCode( const DWORD * _pdwByteCode, size_t /*_stDwords*/ ) {
		if ( !m_pdx9dDevice || !m_pdx9dDevice->GetDirectX9Device() ) { return false; }
		IDirect3DDevice9 * pd3dDev = m_pdx9dDevice->GetDirectX9Device();
		return SUCCEEDED( pd3dDev->CreateVertexShader( _pdwByteCode, &m_pd3dvs9Shader ) );
	}

	/**
	 * \brief Assembles ASCII shader text and creates the vertex shader.
	 * 
	 * \param _pszAsm Null-terminated ASCII assembly (e.g., vs_2_0).
	 * \return Returns true if assembly and creation both succeeded.
	 */
	bool CDirectX9VertexShader::AssembleAndCreate( const char * /*_pszAsm*/ ) {
		return false;
	}

	} // namespace lsn

#endif // LSN_DX9
