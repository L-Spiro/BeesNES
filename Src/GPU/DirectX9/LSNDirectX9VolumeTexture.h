/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D volume texture resource.
 */


#ifdef LSN_DX9

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../LSNTextureBase.h"
#include "LSNDirectX9.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9LosableResource.h"

namespace lsn {

	/**
	 * Class CDirectX9VolumeTexture
	 * \brief A Direct3D volume texture resource.
	 *
	 * Description: A Direct3D volume texture resource.
	 */
	class CDirectX9VolumeTexture : public CDirectX9LosableResource, public CTextureBase {
	public :
		// == Various constructors.
		CDirectX9VolumeTexture( CDirectX9Device * _pdx9dDevice );
		~CDirectX9VolumeTexture();


		// == Functions.
		/**
		 * Must perform some action when the device is lost.
		 */
		virtual void											OnDeviceLost();

		/**
		 * Must renew resources when the device is reset.
		 *
		 * \return Return true if the renewal is successful, false otherwise.
		 */
		virtual bool											OnDeviceReset();

		/**
		 * Resets everything to scratch.
		 */
		void													Reset();

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
		bool													CreateVolumeTexture(
			UINT _uiWidth,
			UINT _uiHeight,
			UINT _uiDepth,
			UINT _uiLevels,
			DWORD _dwUsage,
			D3DFORMAT _fFormat,
			D3DPOOL _pPool );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline IDirect3DVolumeTexture9 *						Get() { return m_pd3dvtTexture; }


	protected :
		// == Members.
		/** The actual DirectX 9 texture. */
		IDirect3DVolumeTexture9 *								m_pd3dvtTexture;
		/** A pointer to the Direct3D 9 device. */
		CDirectX9Device *										m_pdx9dDevice;

	private :
		typedef CTextureBase									Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
