/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D texture resource.
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
	 * Class CDirectX9Texture
	 * \brief A Direct3D texture resource.
	 *
	 * Description: A Direct3D texture resource.
	 */
	class CDirectX9Texture : public CDirectX9LosableResource, public CTextureBase {
	public :
		// == Various constructors.
		CDirectX9Texture( CDirectX9Device * _pdx9dDevice );
		~CDirectX9Texture();


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
		virtual void											Reset();

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
		bool													CreateTexture(
			UINT _uiWidth,
			UINT _uiHeight,
			UINT _uiLevels,
			DWORD _dwUsage,
			D3DFORMAT _fFormat,
			D3DPOOL _pPool );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline IDirect3DTexture9 *								Get() { return m_pd3dtTexture; }


	protected :
		// == Members.
		/** The actual DirectX 9 texture. */
		IDirect3DTexture9 *										m_pd3dtTexture;
		/** A pointer to the Direct3D 9 device. */
		CDirectX9Device *										m_pdx9dDevice;

	private :
		typedef CTextureBase									Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
