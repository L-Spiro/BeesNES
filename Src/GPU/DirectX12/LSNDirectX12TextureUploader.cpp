#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for uploading CPU texel arrays to a Direct3D 12 texture.
 */

#include "LSNDirectX12TextureUploader.h"

namespace lsn {

	CDirectX12TextureUploader::CDirectX12TextureUploader() {
	}
	CDirectX12TextureUploader::~CDirectX12TextureUploader() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the texture and internal states.
	 **/
	void CDirectX12TextureUploader::Reset() {
		if LSN_LIKELY( m_ptTexture.get() && m_ptTexture->Get() ) { m_ptTexture->Reset(); }
		if LSN_LIKELY( m_prUpload.get() && m_prUpload->Get() ) { m_prUpload->Reset(); }
		m_ptTexture.reset();
		m_prUpload.reset();
		m_ui32Width = 0;
		m_ui32Height = 0;
		m_fFormat = DXGI_FORMAT_UNKNOWN;
	}

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
	bool CDirectX12TextureUploader::UploadTexels( CDirectX12Device * _pd12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, const uint8_t * _pui8Data, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Stride, DXGI_FORMAT _fFormat ) {
		if LSN_UNLIKELY( !_pd12dDevice || !_pd12dDevice->GetDevice() || !_pgclCommandList || !_pgclCommandList->Get() || !_pui8Data || !_ui32Width || !_ui32Height || !_ui32Stride ) { return false; }

		ID3D12Device * pd12Device = _pd12dDevice->GetDevice();

		if LSN_UNLIKELY( !m_ptTexture.get() || !m_ptTexture->Get() || !m_prUpload.get() || !m_prUpload->Get() || m_ui32Width != _ui32Width || m_ui32Height != _ui32Height || m_fFormat != _fFormat ) {
			Reset();
			m_ptTexture = std::make_unique<CDirectX12Texture>();
			m_prUpload = std::make_unique<CDirectX12Resource>();

			D3D12_HEAP_PROPERTIES hpDefault = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			D3D12_HEAP_PROPERTIES hpUpload = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
			
			D3D12_RESOURCE_DESC rdTex = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, _ui32Width, _ui32Height, 1, 1, _fFormat, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE };
			if ( !m_ptTexture->CreateCommittedTexture( pd12Device, &hpDefault, D3D12_HEAP_FLAG_NONE, &rdTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE ) ) { return false; }
			
			UINT64 ui64UploadSize = 0;
			pd12Device->GetCopyableFootprints( &rdTex, 0, 1, 0, nullptr, nullptr, nullptr, &ui64UploadSize );
			D3D12_RESOURCE_DESC rdUpload = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, ui64UploadSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
			if ( !m_prUpload->CreateCommittedResource( pd12Device, &hpUpload, D3D12_HEAP_FLAG_NONE, &rdUpload, D3D12_RESOURCE_STATE_GENERIC_READ ) ) { return false; }

			m_ui32Width = _ui32Width;
			m_ui32Height = _ui32Height;
			m_fFormat = _fFormat;
		}

		D3D12_RANGE rReadRange = { 0, 0 };
		void * pvData = nullptr;
		if LSN_UNLIKELY( FAILED( m_prUpload->Get()->Map( 0, &rReadRange, &pvData ) ) ) { return false; }

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT psfFootprint;
		D3D12_RESOURCE_DESC rdDesc = m_ptTexture->Get()->GetDesc();
		pd12Device->GetCopyableFootprints( &rdDesc, 0, 1, 0, &psfFootprint, nullptr, nullptr, nullptr );
		const UINT uiPitch = psfFootprint.Footprint.RowPitch;

		if LSN_LIKELY( uiPitch == _ui32Stride ) {
			std::memcpy( pvData, _pui8Data, _ui32Height * _ui32Stride );
		}
		else {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				const uint8_t * pui8SrcRow = _pui8Data + Y * _ui32Stride;
				uint8_t * pui8DstRow = reinterpret_cast<uint8_t *>(pvData) + Y * uiPitch;
				std::memcpy( pui8DstRow, pui8SrcRow, _ui32Width * (_ui32Stride / _ui32Width) );
			}
		}
		m_prUpload->Get()->Unmap( 0, nullptr );

		D3D12_RESOURCE_BARRIER rbBarriers[1];
		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_ptTexture->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST } };
		_pgclCommandList->Get()->ResourceBarrier( 1, rbBarriers );

		D3D12_TEXTURE_COPY_LOCATION tclDest = { m_ptTexture->Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, { 0 } };
		D3D12_TEXTURE_COPY_LOCATION tclSrc = { m_prUpload->Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, { psfFootprint } };
		_pgclCommandList->Get()->CopyTextureRegion( &tclDest, 0, 0, 0, &tclSrc, nullptr );

		rbBarriers[0] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { m_ptTexture->Get(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
		_pgclCommandList->Get()->ResourceBarrier( 1, rbBarriers );

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
