/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D texture resource.
 */


#ifdef LSN_DX9

#include "LSNDirectX9Texture.h"

namespace lsn {

	CDirectX9Texture::CDirectX9Texture( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ),
		m_pd3dtTexture( nullptr ) {
	}
	CDirectX9Texture::~CDirectX9Texture() {
		Reset();
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9Texture::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pd3dtTexture ) { return; }

		CDirectX9::SafeRelease( m_pd3dtTexture );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9Texture::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		//return CreateApiTexture( NULL, 0 );
		return true;
	}

	/**
	 * Resets everything to scratch.
	 */
	void CDirectX9Texture::Reset() {
		CDirectX9::SafeRelease( m_pd3dtTexture );

		m_bResourceCanBeLost = false;
		Parent::Reset();
	}

	/**
	 * Creates a 2-D texture.
	 * 
	 * \param _uiWidth Width of the top-level of the texture, in pixels. The pixel dimensions of subsequent levels will be the truncated value of half of the previous level's pixel dimension (independently). Each dimension clamps at a size of 1 pixel. Thus, if the division by 2 results in 0, 1 will be taken instead.
	 * \param _uiHeight Height of the top-level of the texture, in pixels. The pixel dimensions of subsequent levels will be the truncated value of half of the previous level's pixel dimension (independently). Each dimension clamps at a size of 1 pixel. Thus, if the division by 2 results in 0, 1 will be taken instead.
	 * \param _uiLevels Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels down to 1 by 1 pixels for hardware that supports mipmapped textures. Call IDirect3DBaseTexture9::GetLevelCount to see the number of levels generated.
	 * \param _dwUsage Usage can be 0, which indicates no usage value. However, if usage is desired, use a combination of one or more D3DUSAGE constants. It is good practice to match the usage parameter with the behavior flags in IDirect3D9::CreateDevice.
	 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of all levels in the texture.
	 * \param _pPool Member of the D3DPOOL enumerated type, describing the memory class into which the texture should be placed.
	 * \return Returns true if the texture was created.
	 **/
	bool CDirectX9Texture::CreateTexture(
		UINT _uiWidth,
		UINT _uiHeight,
		UINT _uiLevels,
		DWORD _dwUsage,
		D3DFORMAT _fFormat,
		D3DPOOL _pPool ) {
		Reset();
		if ( SUCCEEDED( m_pdx9dDevice->GetDirectX9Device()->CreateTexture( _uiWidth, _uiHeight, _uiLevels, _dwUsage, _fFormat, _pPool,
			&m_pd3dtTexture, nullptr ) ) ) {
			if ( Alloc( _uiWidth, _uiHeight, 1, _uiLevels, 1, 1, uint32_t( _pPool ), uint32_t( _dwUsage ), uint32_t( _fFormat ) ) ) {
				return true;
			}
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
