/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 cube texture resource.
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
	 * Class CDirectX9CubeTexture
	 * \brief A Direct3D 9 cube texture resource.
	 *
	 * Description: A Direct3D 9 cube texture resource.
	 */
	class CDirectX9CubeTexture : public CDirectX9LosableResource, public CTextureBase {
	public :
		// == Various constructors.
		CDirectX9CubeTexture( CDirectX9Device * _pdx9dDevice );
		~CDirectX9CubeTexture();


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
		 * Creates a cube texture.
		 * 
		 * \param _uiEdgeLength Size of the edges of all the top-level faces of the cube texture. The pixel dimensions of subsequent levels of each face will be the truncated value of half of the previous level's pixel dimension (independently). Each dimension clamps at a size of 1 pixel. Thus, if the division by 2 results in 0 (zero), 1 will be taken instead.
		 * \param _uiLevels Number of levels in each face of the cube texture. If this is zero, Direct3D will generate all cube texture sublevels down to 1x1 pixels for each face for hardware that supports mipmapped cube textures. Call IDirect3DBaseTexture9::GetLevelCount to see the number of levels generated.
		 * \param _dwUsage Usage can be 0, which indicates no usage value. However, if usage is desired, use a combination of one or more D3DUSAGE constants. It is good practice to match the usage parameter in CreateCubeTexture with the behavior flags in IDirect3D9::CreateDevice.
		 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of all levels in all faces of the cube texture.
		 * \param _pPool Member of the D3DPOOL enumerated type, describing the memory class into which the cube texture should be placed.
		 * \return Returns true if the texture was created.
		 **/
		bool													CreateCubeTexture(
			UINT _uiEdgeLength,
			UINT _uiLevels,
			DWORD _dwUsage,
			D3DFORMAT _fFormat,
			D3DPOOL _pPool );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline IDirect3DCubeTexture9 *							Get() { return m_pd3dctTexture; }


	protected :
		// == Members.
		/** The actual DirectX 9 texture. */
		IDirect3DCubeTexture9 *									m_pd3dctTexture;
		/** A pointer to the Direct3D 9 device. */
		CDirectX9Device *										m_pdx9dDevice;

	private :
		typedef CTextureBase									Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
