#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for uploading CPU texel arrays to a Direct3D 9 texture.
 */

#include "LSNDx9TextureUploader.h"

namespace lsn {

	CDx9TextureUploader::CDx9TextureUploader() {
	}
	CDx9TextureUploader::~CDx9TextureUploader() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the texture and internal states.
	 **/
	void CDx9TextureUploader::Reset() {
		if LSN_LIKELY( m_ptTexture.get() && m_ptTexture->Get() ) { m_ptTexture->Reset(); }
		m_ptTexture.reset();
		m_ui32Width = 0;
		m_ui32Height = 0;
		m_fFormat = D3DFMT_UNKNOWN;
	}

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
	bool CDx9TextureUploader::UploadTexels( CDirectX9Device * _pdx9dDevice, const uint8_t * _pui8Data, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Stride, D3DFORMAT _fFormat ) {
		if LSN_UNLIKELY( !_pdx9dDevice || !_pdx9dDevice->GetDirectX9Device() || !_pui8Data || !_ui32Width || !_ui32Height || !_ui32Stride ) { return false; }

		if LSN_UNLIKELY( !m_ptTexture.get() || !m_ptTexture->Valid() || m_ui32Width != _ui32Width || m_ui32Height != _ui32Height || m_fFormat != _fFormat ) {
			m_ptTexture = std::make_unique<CDirectX9Texture>( _pdx9dDevice );
			if ( !m_ptTexture->Create2D( _ui32Width, _ui32Height, 1, D3DUSAGE_DYNAMIC, _fFormat, D3DPOOL_DEFAULT ) ) { return false; }
			m_ui32Width = _ui32Width;
			m_ui32Height = _ui32Height;
			m_fFormat = _fFormat;
		}

		D3DLOCKED_RECT lrRect{};
		if LSN_UNLIKELY( !m_ptTexture->LockRect( 0, lrRect, nullptr, D3DLOCK_DISCARD ) || nullptr == lrRect.pBits ) { return false; }

		if LSN_LIKELY( lrRect.Pitch == INT( _ui32Stride ) ) {
			std::memcpy( lrRect.pBits, _pui8Data, _ui32Height * _ui32Stride );
		}
		else {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				const uint8_t * pui8SrcRow = _pui8Data + Y * _ui32Stride;
				uint8_t * pui8DstRow = reinterpret_cast<uint8_t *>(lrRect.pBits) + Y * lrRect.Pitch;
				std::memcpy( pui8DstRow, pui8SrcRow, _ui32Width * (_ui32Stride / _ui32Width) );
			}
		}

		m_ptTexture->UnlockRect( 0 );
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
