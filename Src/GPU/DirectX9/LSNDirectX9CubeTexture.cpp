/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D cube texture resource.
 */


#ifdef LSN_DX9

#include "LSNDirectX9CubeTexture.h"

namespace lsn {

	CDirectX9CubeTexture::CDirectX9CubeTexture( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ),
		m_pd3dctTexture( nullptr ) {
	}
	CDirectX9CubeTexture::~CDirectX9CubeTexture() {
		Reset();
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9CubeTexture::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pd3dctTexture ) { return; }

		CDirectX9::SafeRelease( m_pd3dctTexture );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9CubeTexture::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		//return CreateApiTexture( NULL, 0 );
		return true;
	}

	/**
	 * Resets everything to scratch.
	 */
	void CDirectX9CubeTexture::Reset() {
		CDirectX9::SafeRelease( m_pd3dctTexture );

		m_bResourceCanBeLost = false;
		Parent::Reset();
	}

	/**
	 * Creates a cube texture.
	 * 
	 * \param _uiEdgeLength Size of the edges of all the top-level faces of the cube texture. The pixel dimensions of subsequent levels of each face will be the truncated value of half of the previous level's pixel dimension (independently). Each dimension clamps at a size of 1 pixel. Thus, if the division by 2 results in 0 (zero), 1 will be taken instead.
	 * \param _uiLevels Number of levels in each face of the cube texture. If this is zero, Direct3D will generate all cube texture sublevels down to 1x1 pixels for each face for hardware that supports mipmapped cube textures. Call IDirect3DBaseTexture9::GetLevelCount to see the number of levels generated.
	 * \param _dwUsage Usage can be 0, which indicates no usage value. However, if usage is desired, use a combination of one or more D3DUSAGE constants. It is good practice to match the usage parameter in CreateCubeTexture with the behavior flags in IDirect3D9::CreateDevice.
	 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of all levels in all faces of the cube texture.
	 * \param _pPool Member of the D3DPOOL enumerated type, describing the memory class into which the cube texture should be placed.
	 * \return Returns true if the texture was created.
	 **/
	bool CDirectX9CubeTexture::CreateCubeTexture(
		UINT _uiEdgeLength,
		UINT _uiLevels,
		DWORD _dwUsage,
		D3DFORMAT _fFormat,
		D3DPOOL _pPool ) {
		Reset();
		if ( SUCCEEDED( m_pdx9dDevice->GetDirectX9Device()->CreateCubeTexture( _uiEdgeLength, _uiLevels, _dwUsage, _fFormat, _pPool,
			&m_pd3dctTexture, nullptr ) ) ) {
			if ( Alloc( _uiEdgeLength, _uiEdgeLength, 1, _uiLevels, 6, 1, uint32_t( _pPool ), uint32_t( _dwUsage ), uint32_t( _fFormat ) ) ) {
				return true;
			}
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
