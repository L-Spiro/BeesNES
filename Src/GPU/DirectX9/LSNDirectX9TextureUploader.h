#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for uploading CPU texel arrays to a Direct3D 9 texture.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9Texture.h"

#include <memory>

namespace lsn {

	/**
	 * Class CDirectX9TextureUploader
	 * \brief A generic helper class for uploading CPU texel arrays to a Direct3D 9 texture.
	 *
	 * Description: Owns a dynamic texture and handles the locking/copying of CPU byte arrays to the GPU.
	 */
	class CDirectX9TextureUploader {
	public :
		CDirectX9TextureUploader();
		~CDirectX9TextureUploader();


		// == Functions.
		/**
		 * Resets the texture and internal states.
		 **/
		void													Reset();

		/**
		 * Uploads a texel array to the managed texture, creating or recreating the texture if dimensions or formats change.
		 * 
		 * \param _pdx9dDevice The Direct3D 9 device.
		 * \param _pui8Data The raw RGBA byte array from the CPU.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Stride The pitch of the input byte array.
		 * \param _fFormat The Direct3D 9 format of the texture.
		 * \return Returns true on success.
		 **/
		bool													UploadTexels( CDirectX9Device * _pdx9dDevice, const uint8_t * _pui8Data, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Stride, D3DFORMAT _fFormat = D3DFMT_A8R8G8B8 );

		/**
		 * Gets the underlying Direct3D 9 texture wrapper.
		 * 
		 * \return Returns a pointer to the managed texture object.
		 **/
		inline CDirectX9Texture *								GetTexture() { return m_ptTexture.get(); }


	protected :
		// == Members.
		/** The managed texture. */
		std::unique_ptr<CDirectX9Texture>						m_ptTexture;
		/** The current width of the texture. */
		uint32_t												m_ui32Width = 0;
		/** The current height of the texture. */
		uint32_t												m_ui32Height = 0;
		/** The current format of the texture. */
		D3DFORMAT												m_fFormat = D3DFMT_UNKNOWN;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
