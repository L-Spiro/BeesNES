#ifdef LSN_DX9

#include "LSNDx9PaletteFilter.h"


namespace lsn {

	CDx9PaletteFilter::CDx9PaletteFilter() {
	}
	CDx9PaletteFilter::~CDx9PaletteFilter() {
	}

	// == Functions.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx9PaletteFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		return CParent::Init( _stBuffers, _ui16Width, _ui16Height );
	}

	/**
	 * Sets the palette.
	 * 
	 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
	 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.  False always indicates a memory failure if _pfRgba512 is not nullptr.
	 **/
	bool CDx9PaletteFilter::SetLut( const float * _pfRgba512 ) {
		if ( !_pfRgba512 ) { return false; }
		try {
			m_vLut.resize( 4 * 512 );
			std::memcpy( m_vLut.data(), _pfRgba512, sizeof( float ) * 4 * 512 );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx9PaletteFilter::Activate() {
		CParent::Activate();
		//if ( s_dgsState.CreateDx9( 
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9PaletteFilter::DeActivate() {
		CParent::DeActivate();
	}

	/**
	 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.  The final buffer, along with
	 *	its width, height, bit-depth, and stride, are returned.
	 *
	 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.  Its format will be that returned in InputFormat().
	 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
	 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
	 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
	 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
	 * \param _ui64PpuFrame The PPU frame associated with the input data.
	 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CDx9PaletteFilter::ApplyFilter( uint8_t * /*_pui8Input*/, uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/ ) {
		return nullptr;	// Only valid for GPU filters; we present directly ourselves.
	}

	/**
	 * \brief Updates the 512-entry float RGBA LUT.
	 *
	 * The LUT is a 512Å~1 A32B32G32R32F MANAGED texture. Each entry is RGBA in linear space.
	 *
	 * \return Returns true on success.
	 */
	bool CDx9PaletteFilter::UpdateLut() {
		if LSN_UNLIKELY( !m_pdx9dDevice || !m_ui32SrcW || !m_ui32SrcH ) { return false; }
		// Create once: 512x1 A32B32G32R32F MANAGED texture.
		if LSN_UNLIKELY( !m_tLut.get() ) {
			m_tLut = std::make_unique<CDirectX9Texture>( m_pdx9dDevice );
			if ( !m_tLut.get() ) { return false; }
		}
		if LSN_UNLIKELY( !m_tLut->Valid() ) {
			if ( !m_vLut.size() ) { return false; }	// If it is just being created, we need to have a palette to send it.
			if ( !m_tLut->Create2D( 512, 1, 1, /*usage*/0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED ) ) {
				return false;
			}
		}
		if LSN_UNLIKELY( m_vLut.size() ) {
			D3DLOCKED_RECT lrLocked{};
			if LSN_UNLIKELY( !m_tLut->LockRect( 0, lrLocked, nullptr, 0 ) ) { return false; }
			std::memcpy( lrLocked.pBits, m_vLut.data(), sizeof( float ) * 4 * 512 );
			m_tLut->UnlockRect( 0 );
			m_vLut.clear();
		}
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
