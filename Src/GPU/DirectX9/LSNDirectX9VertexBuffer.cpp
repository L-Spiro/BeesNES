/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D vertex buffer.
 */


#ifdef LSN_DX9

#include "LSNDirectX9VertexBuffer.h"

namespace lsn {

	CDirectX9VertexBuffer::CDirectX9VertexBuffer( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ),
		m_pd3dvb9Buffer( nullptr ) {
	}
	CDirectX9VertexBuffer::~CDirectX9VertexBuffer() {
		Reset();
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9VertexBuffer::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pd3dvb9Buffer ) { return; }

		CDirectX9::SafeRelease( m_pd3dvb9Buffer );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9VertexBuffer::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		//return CreateApiTexture( NULL, 0 );
		return true;
	}

	/**
	 * Resets everything to scratch.
	 */
	void CDirectX9VertexBuffer::Reset() {
		CDirectX9::SafeRelease( m_pd3dvb9Buffer );

		m_bResourceCanBeLost = false;
		Parent::Reset();
	}

	/**
	 * Creates a vertex buffer.
	 * 
	 * \param _uiLength Size of the vertex buffer, in bytes. For FVF vertex buffers, Length must be large enough to contain at least one vertex, but it need not be a multiple of the vertex size. Length is not validated for non-FVF buffers.
	 * \param _dwUsage Usage can be 0, which indicates no usage value. However, if usage is desired, use a combination of one or more D3DUSAGE constants. It is good practice to match the usage parameter in CreateVertexBuffer with the behavior flags in IDirect3D9::CreateDevice.
	 * \param _dwFVF Combination of D3DFVF, a usage specifier that describes the vertex format of the vertices in this buffer. If this parameter is set to a valid FVF code, the created vertex buffer is an FVF vertex buffer (see Remarks). Otherwise, if this parameter is set to zero, the vertex buffer is a non-FVF vertex buffer.
	 * \param _pPool Member of the D3DPOOL enumerated type, describing a valid memory class into which to place the resource. Do not set to D3DPOOL_SCRATCH.
	 * \return Returns true if the vertex buffer was created.
	 **/
	bool CDirectX9VertexBuffer::CreateVertexBuffer(
		UINT _uiLength,
		DWORD _dwUsage,
		DWORD _dwFVF,
		D3DPOOL _pPool ) {
		Reset();
		if ( SUCCEEDED( m_pdx9dDevice->GetDirectX9Device()->CreateVertexBuffer( _uiLength, _dwUsage, _dwFVF, _pPool,
			&m_pd3dvb9Buffer, nullptr ) ) ) {
			if ( Alloc( _uiLength, uint32_t( _pPool ), uint32_t( _dwUsage ), uint32_t( _dwFVF ) ) ) {
				return true;
			}
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
