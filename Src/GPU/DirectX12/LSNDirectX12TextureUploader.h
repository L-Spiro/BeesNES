#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for uploading CPU texel arrays to a Direct3D 12 texture.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX12Device.h"
#include "LSNDirectX12Texture.h"
#include "LSNDirectX12Resource.h"
#include "LSNDirectX12GraphicsCommandList.h"

#include <memory>

namespace lsn {

	/**
	 * Class CDirectX12TextureUploader
	 * \brief A generic helper class for uploading CPU texel arrays to a Direct3D 12 texture.
	 *
	 * Description: Owns a texture in the DEFAULT heap and handles the upload heap mapping, memory copying, and GPU command recording to transfer CPU byte arrays to the GPU.
	 */
	class CDirectX12TextureUploader {
	public :
		CDirectX12TextureUploader();
		~CDirectX12TextureUploader();


		// == Functions.
		/**
		 * Resets the texture and internal states.
		 **/
		void													Reset();

		/**
		 * Uploads a texel array to the managed texture, creating or recreating the texture if dimensions or formats change.
		 * 
		 * \param _pd12dDevice The Direct3D 12 device.
		 * \param _pgclCommandList The command list used to record the copy commands and transition barriers.
		 * \param _pui8Data The raw RGBA byte array from the CPU.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Stride The pitch of the input byte array.
		 * \param _fFormat The Direct3D 12 format of the texture.
		 * \return Returns true on success.
		 **/
		bool													UploadTexels( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, const uint8_t * _pui8Data, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Stride, DXGI_FORMAT _fFormat = DXGI_FORMAT_R8G8B8A8_UNORM );

		/**
		 * Gets the underlying Direct3D 12 texture wrapper.
		 * 
		 * \return Returns a pointer to the managed texture object.
		 **/
		inline CDirectX12Texture *								GetTexture() { return m_ptTexture.get(); }


	protected :
		// == Members.
		/** The managed texture in the default heap. */
		std::unique_ptr<CDirectX12Texture>						m_ptTexture;
		/** The upload buffer used to transfer data. */
		std::unique_ptr<CDirectX12Resource>						m_prUpload;
		/** The current width of the texture. */
		uint32_t												m_ui32Width = 0;
		/** The current height of the texture. */
		uint32_t												m_ui32Height = 0;
		/** The current format of the texture. */
		DXGI_FORMAT												m_fFormat = DXGI_FORMAT_UNKNOWN;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
