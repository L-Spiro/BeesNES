/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 index buffer.
 */


#ifdef LSN_DX9

#include "LSNDirectX9IndexBuffer.h"

namespace lsn {

	CDirectX9IndexBuffer::CDirectX9IndexBuffer( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ),
		m_pd3dib9Buffer( nullptr ) {
	}
	CDirectX9IndexBuffer::~CDirectX9IndexBuffer() {
		Reset();
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9IndexBuffer::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pd3dib9Buffer ) { return; }

		CDirectX9::SafeRelease( m_pd3dib9Buffer );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9IndexBuffer::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		//return CreateApiTexture( NULL, 0 );
		return true;
	}

	/**
	 * Resets everything to scratch.
	 */
	void CDirectX9IndexBuffer::Reset() {
		CDirectX9::SafeRelease( m_pd3dib9Buffer );

		m_bResourceCanBeLost = false;
		Parent::Reset();
	}

	/**
	 * Creates an index buffer.
	 * 
	 * \param _uiLength Size of the index buffer, in bytes.
	 * \param _dwUsage Usage can be 0, which indicates no usage value. However, if usage is desired, use a combination of one or more D3DUSAGE constants. It is good practice to match the usage parameter in CreateIndexBuffer with the behavior flags in IDirect3D9::CreateDevice.
	 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of the index buffer.
	 * \param _pPool Member of the D3DPOOL enumerated type, describing a valid memory class into which to place the resource.
	 * \return Returns true if the index buffer was created.
	 **/
	bool CDirectX9IndexBuffer::CreateIndexBuffer(
		UINT _uiLength,
		DWORD _dwUsage,
		D3DFORMAT _fFormat,
		D3DPOOL _pPool ) {
		Reset();
		if ( SUCCEEDED( m_pdx9dDevice->GetDirectX9Device()->CreateIndexBuffer( _uiLength, _dwUsage, _fFormat, _pPool,
			&m_pd3dib9Buffer, nullptr ) ) ) {
			UINT uiTotal = _uiLength /
				((D3DFMT_INDEX16 == _fFormat) ? sizeof( uint16_t ) : sizeof( uint32_t ));
			if ( Alloc( uiTotal, uint32_t( _pPool ), uint32_t( _dwUsage ), uint32_t( _fFormat ) ) ) {
				return true;
			}
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
