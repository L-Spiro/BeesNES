#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Blargg’s implementation of an NTSC filter for Direct3D 12.
 */

#include "LSNDx12NtscBlarggFilter.h"
#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

namespace lsn {

	// == Members.
	CDx12NtscBlarggFilter::CDx12NtscBlarggFilter() :
		m_ui32FinalStride( 0 ) {
		nes_ntsc_setup_t nsTmp = nes_ntsc_composite;
		nsTmp.artifacts = 0.62;
		nsTmp.bleed = 0.0;
		nsTmp.fringing = 0.05;
		nsTmp.sharpness = 0.78;
		nsTmp.merge_fields = 0;
		nsTmp.saturation = -0.250;
		nsTmp.brightness = -0.084;
		nsTmp.gamma = 0.0;
		nsTmp.hue = 7.89 / 180.0;
		
		::nes_ntsc_init( &m_nnBlarggNtsc, &nsTmp );

		m_rsResampler.SetFilter( CResamplerBase::LSN_FF_ROBIDOUX );
		SetPhosphorDecayLevel( 0.15f );
		SetPhosphorDecayPeriod( 1.79113161563873291015625f / 7.0f );
	}
	CDx12NtscBlarggFilter::~CDx12NtscBlarggFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx12NtscBlarggFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_tuUploader.Reset();
		ReleaseBaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		m_ui32FinalStride = RowStride( NES_NTSC_OUT_WIDTH( _ui16Width ), OutputBits() );
		m_vRgbBuffer.resize( m_ui32FinalStride * _ui16Height );

		return pofOut;
	}

	/**
	 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.
	 *
	 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.
	 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
	 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
	 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
	 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
	 * \param _ui64PpuFrame The PPU frame associated with the input data.
	 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
	 * \param _i32DispLeft The display area left.
	 * \param _i32DispTop The display area top.
	 * \param _ui32DispWidth The display area width.
	 * \param _ui32DispHeight The display area height
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CDx12NtscBlarggFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		if LSN_UNLIKELY( !m_pdx12dDevice ) { return m_vBasicRenderTarget[0].data(); }
		if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
			m_ui32SrcW = _ui32Width;
			m_ui32SrcH = _ui32Height;
			EnsureSizeAndResources();
		}
		
		lsw::LSW_RECT rRect;
		rRect.left = LONG( _i32DispLeft );
		rRect.top = LONG( _i32DispTop );
		rRect.right = rRect.left + LONG( _ui32DispWidth );
		rRect.bottom = rRect.top + LONG( _ui32DispHeight );

		const uint32_t ui32Pitch = m_ui32FinalStride;
		m_vRgbBuffer.resize( ui32Pitch * m_ui32SrcH );

		::nes_ntsc_blit( &m_nnBlarggNtsc,
			reinterpret_cast<NES_NTSC_IN_T *>(_pui8Input), _ui32Width, _ui64RenderStartCycle % 3, 3,
			_ui32Width, _ui32Height,
			m_vRgbBuffer.data(), m_ui32FinalStride );
		
		_ui32Width = NES_NTSC_OUT_WIDTH( _ui32Width );
		_ui32Stride = m_ui32FinalStride;

		// Reset command list for upload and rendering execution
		m_caAllocator->Get()->Reset();
		m_gclCommandList->Get()->Reset( m_caAllocator->Get(), nullptr );

		m_tuUploader.UploadTexels( m_pdx12dDevice, m_gclCommandList.get(), m_vRgbBuffer.data(), _ui32Width, m_ui32SrcH, ui32Pitch, DXGI_FORMAT_B8G8R8A8_UNORM );

		Render( rRect );

		_ui32Width = uint32_t( s_dgsState.rScreenRect.Width() );
		_ui32Height = uint32_t( s_dgsState.rScreenRect.Height() );
		_ui32Stride = _ui32Width * sizeof( uint32_t );
		return m_vBasicRenderTarget[0].data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx12NtscBlarggFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx12NtscBlarggFilter::DeActivate() {
		m_tuUploader.Reset();
		m_caAllocator.reset();
		m_gclCommandList.reset();
		CParent::DeActivate();
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CDx12NtscBlarggFilter::FrameResize() {
		ReleaseBaseSizeDependents();
		OnSizeDx12();
		EnsureSizeAndResources();
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * \return Returns true on success.
	 */
	bool CDx12NtscBlarggFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx12dDevice ) {
			if ( !s_dgsState.CreateDx12() ) { return false; }
			m_pdx12dDevice = &Device();
			m_tuUploader.Reset();
		}
		if ( !m_pdx12dDevice ) { return false; }

		ID3D12Device * pd12Device = m_pdx12dDevice->GetDevice();
		if ( !pd12Device ) { return false; }

		if LSN_UNLIKELY( !m_caAllocator.get() ) {
			m_caAllocator = std::make_unique<CDirectX12CommandAllocator>();
			if ( !m_caAllocator->CreateCommandAllocator( pd12Device, D3D12_COMMAND_LIST_TYPE_DIRECT ) ) { return false; }
		}
		if LSN_UNLIKELY( !m_gclCommandList.get() ) {
			m_gclCommandList = std::make_unique<CDirectX12GraphicsCommandList>();
			if ( !m_gclCommandList->CreateCommandList( pd12Device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_caAllocator->Get(), nullptr ) ) { return false; }
			m_gclCommandList->Get()->Close();
		}

		m_bValidState = true;
		return true;
	}

	/**
	 * Renders the final output to the backbuffer.
	 *
	 * \param _rOutput The destination rectangle.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx12NtscBlarggFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_tuUploader.GetTexture() || !m_tuUploader.GetTexture()->Get() ) { return false; }
		
		uint32_t ui32NativeW = NES_NTSC_OUT_WIDTH( m_ui32SrcW );
		uint32_t ui32NativeH = m_ui32SrcH;
		
		bool bRet = RenderBase( m_pdx12dDevice, m_gclCommandList.get(), m_tuUploader.GetTexture(), ui32NativeW, ui32NativeH, _rOutput, false );

		m_gclCommandList->Get()->Close();

		ID3D12CommandList * ppCommandLists[] = { m_gclCommandList->Get() };
		m_pdx12dDevice->GetCommandQueue()->ExecuteCommandLists( 1, ppCommandLists );

		m_pdx12dDevice->FlushCommandQueue();

		return bRet;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
