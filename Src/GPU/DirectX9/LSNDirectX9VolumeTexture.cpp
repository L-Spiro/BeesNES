/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 volume texture resource.
 */


#ifdef LSN_DX9

#include "LSNDirectX9VolumeTexture.h"

namespace lsn {

	CDirectX9VolumeTexture::CDirectX9VolumeTexture( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ),
		m_pd3dvtTexture( nullptr ) {
	}
	CDirectX9VolumeTexture::~CDirectX9VolumeTexture() {
		Reset();
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9VolumeTexture::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pd3dvtTexture ) { return; }

		CDirectX9::SafeRelease( m_pd3dvtTexture );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9VolumeTexture::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		//return CreateApiTexture( NULL, 0 );
		return true;
	}

	/**
	 * Resets everything to scratch.
	 */
	void CDirectX9VolumeTexture::Reset() {
		CDirectX9::SafeRelease( m_pd3dvtTexture );

		m_bResourceCanBeLost = false;
		Parent::Reset();
	}

	/**
	 * Creates a 3-D texture.
	 * 
	 * \param _uiWidth Width of the top-level of the volume texture, in pixels. This value must be a power of two if the D3DPTEXTURECAPS_VOLUMEMAP_POW2 member of D3DCAPS9 is set. The pixel dimensions of subsequent levels will be the truncated value of half of the previous level's pixel dimension (independently). Each dimension clamps at a size of 1 pixel. Thus, if the division by two results in 0 (zero), 1 will be taken instead. The maximum dimension that a driver supports (for width, height, and depth) can be found in MaxVolumeExtent in D3DCAPS9.
	 * \param _uiHeight Height of the top-level of the volume texture, in pixels. This value must be a power of two if the D3DPTEXTURECAPS_VOLUMEMAP_POW2 member of D3DCAPS9 is set. The pixel dimensions of subsequent levels will be the truncated value of half of the previous level's pixel dimension (independently). Each dimension clamps at a size of 1 pixel. Thus, if the division by 2 results in 0 (zero), 1 will be taken instead. The maximum dimension that a driver supports (for width, height, and depth) can be found in MaxVolumeExtent in D3DCAPS9.
	 * \param _uiDepth Depth of the top-level of the volume texture, in pixels. This value must be a power of two if the D3DPTEXTURECAPS_VOLUMEMAP_POW2 member of D3DCAPS9 is set. The pixel dimensions of subsequent levels will be the truncated value of half of the previous level's pixel dimension (independently). Each dimension clamps at a size of 1 pixel. Thus, if the division by 2 results in 0 (zero), 1 will be taken instead. The maximum dimension that a driver supports (for width, height, and depth) can be found in MaxVolumeExtent in D3DCAPS9.
	 * \param _uiLevels Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels down to 1x1 pixels for hardware that supports mipmapped volume textures. Call IDirect3DBaseTexture9::GetLevelCount to see the number of levels generated.
	 * \param _dwUsage Usage can be 0, which indicates no usage value. If usage is desired, use D3DUSAGE_DYNAMIC or D3DUSAGE_SOFTWAREPROCESSING. For more information, see D3DUSAGE.
	 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of all levels in the volume texture.
	 * \param _pPool Member of the D3DPOOL enumerated type, describing the memory class into which the volume texture should be placed.
	 * \return Returns true if the texture was created.
	 **/
	bool CDirectX9VolumeTexture::CreateVolumeTexture(
		UINT _uiWidth,
		UINT _uiHeight,
		UINT _uiDepth,
		UINT _uiLevels,
		DWORD _dwUsage,
		D3DFORMAT _fFormat,
		D3DPOOL _pPool ) {
		Reset();
		if ( SUCCEEDED( m_pdx9dDevice->GetDirectX9Device()->CreateVolumeTexture( _uiWidth, _uiHeight, _uiDepth, _uiLevels, _dwUsage, _fFormat, _pPool,
			&m_pd3dvtTexture, nullptr ) ) ) {
			if ( Alloc( _uiWidth, _uiHeight, _uiDepth, _uiLevels, 1, 1, uint32_t( _pPool ), uint32_t( _dwUsage ), uint32_t( _fFormat ) ) ) {
				return true;
			}
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
