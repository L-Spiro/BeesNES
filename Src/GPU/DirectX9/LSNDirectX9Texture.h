/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 texture resource.
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
	 * \brief A Direct3D 9 texture resource.
	 *
	 * Description: A Direct3D 9 texture resource.
	 */
	class CDirectX9Texture : public CDirectX9LosableResource, public CTextureBase {
	public :
		// == Various constructors.
		CDirectX9Texture( CDirectX9Device * _pdx9dDevice );
		virtual ~CDirectX9Texture();


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

		/**
		 * \brief Determines whether this texture currently wraps a valid D3D9 texture.
		 * 
		 * \return Returns true if the texture exists.
		 */
		inline bool												Valid() const { return m_pd3dtTexture != nullptr; }

		/**
		 * \brief Locks a 2-D texture level for CPU access.
		 *
		 * \param _uiLevel The mip level to lock (0 = base level).
		 * \param _lrLocked On success receives the pitch and pointer to the texel data.
		 * \param _prRect Optional sub-rectangle to lock; nullptr locks the whole level.
		 * \param _dwFlags Combination of LSN_D3DLOCK flags (e.g., LSN_D3DLOCK_DISCARD).
		 * \return Returns true on success.
		 */
		bool													LockRect( UINT _uiLevel,
			D3DLOCKED_RECT &_lrLocked,
			const RECT * _prRect,
			DWORD _dwFlags );

		/**
		 * \brief Unlocks a previously locked 2-D texture level.
		 *
		 * \param _uiLevel The mip level to unlock (0 = base level).
		 */
		void													UnlockRect( UINT _uiLevel );

		/**
		 * \brief Retrieves a surface for a given level (caller releases).
		 *
		 * \param _uiLevel The mip level to fetch.
		 * \return Returns AddRef()'d IDirect3DSurface9* on success, or nullptr.
		 */
		inline IDirect3DSurface9 *								GetSurfaceLevel( UINT _uiLevel ) {
			if LSN_UNLIKELY( !m_pd3dtTexture ) { return nullptr; }
			IDirect3DSurface9 * pd3ds9Surf = nullptr;
			if LSN_LIKELY( SUCCEEDED( m_pd3dtTexture->GetSurfaceLevel( _uiLevel, &pd3ds9Surf ) ) ) { return pd3ds9Surf; }
			return nullptr;
		}

		/**
		 * \brief Convenience alias matching other backends: create a 2-D texture.
		 *
		 * \param _uiWidth Top-level width.
		 * \param _uiHeight Top-level height.
		 * \param _uiLevels Mipmap count (0 = full chain).
		 * \param _dwUsage Usage flags.
		 * \param _fFormat Texture format.
		 * \param _pPool Memory pool.
		 * \return Returns true if the texture was created.
		 */
		inline bool												Create2D( UINT _uiWidth, UINT _uiHeight,
			UINT _uiLevels, DWORD _dwUsage,
			D3DFORMAT _fFormat, D3DPOOL _pPool ) {
			return CreateTexture( _uiWidth, _uiHeight, _uiLevels, _dwUsage, _fFormat, _pPool );
		}



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
