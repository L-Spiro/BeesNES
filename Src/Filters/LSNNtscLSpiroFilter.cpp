/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#include "LSNNtscLSpiroFilter.h"

#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

namespace lsn {

	// == Members.
	CNtscLSpiroFilter::CNtscLSpiroFilter() {
	}
	CNtscLSpiroFilter::~CNtscLSpiroFilter() {
		StopThread();
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CNtscLSpiroFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

		m_ui32OutputWidth = m_ui16ScaledWidth;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( m_stStride * _ui16Height );
			for ( auto J = m_vBasicRenderTarget[I].size() / sizeof( uint16_t ); J--; ) {
				(*reinterpret_cast<uint16_t *>(&m_vBasicRenderTarget[I][J*sizeof( uint16_t )])) = 0x0F;
			}
		}
		m_ui32FinalStride = RowStride( m_ui32OutputWidth, OutputBits() );


		StopThread();
		m_tdThreadData.ui16LinesDone = 0;
		m_tdThreadData.ui16EndLine = 0;
		m_tdThreadData.bEndThread = false;
		m_tdThreadData.pnlsfThis = this;
		m_ptThread = std::make_unique<std::thread>( WorkThread, &m_tdThreadData );
		return InputFormat();
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
	uint8_t * CNtscLSpiroFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &_ui16BitDepth, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle ) {
		FilterFrame( _pui8Input, _ui64RenderStartCycle + 2 );

		_ui16BitDepth = uint16_t( OutputBits() );
		_ui32Width = m_ui16ScaledWidth;
		_ui32Height = m_ui16Height;
		_ui32Stride = m_ui32FinalStride;
		return m_vRgbBuffer.data();
	}

	/**
	 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
	 **/
	void CNtscLSpiroFilter::FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle ) {
		m_tdThreadData.ui16LinesDone = m_ui16Height / 2;
		m_tdThreadData.ui16EndLine = m_ui16Height;
		m_tdThreadData.ui64RenderStartCycle = _ui64RenderStartCycle;
		m_tdThreadData.pui8Pixels = _pui8Pixels;
		m_eGo.Signal();
		RenderScanlineRange( _pui8Pixels, 0, m_ui16Height / 2, _ui64RenderStartCycle, m_vRgbBuffer.data(), m_ui16ScaledWidth * 4 );

		//m_eDone.WaitForSignal();
	}

	/**
	 * Allocates the YIQ buffers for a given width and height.
	 * 
	 * \param _ui16W The width of the buffers.
	 * \param _ui16H The height of the buffers.
	 * \param _ui16Scale The width scale factor.
	 * \return Returns true if the allocations succeeded.
	 **/
	bool CNtscLSpiroFilter::AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H, uint16_t _ui16Scale ) {
		if ( !CLSpiroFilterBase::AllocYiqBuffers( _ui16W, _ui16H, _ui16Scale ) ) { return false; }

		size_t sSize = _ui16W * _ui16Scale * _ui16H;
		if ( !sSize ) { return true; }
		try {
			m_vRgbBuffer.resize( sSize * 4 );
		}
		catch ( ... ) { return false; }
		return true;
	}

	/**
	 * Stops the worker thread.
	 **/
	void CNtscLSpiroFilter::StopThread() {
		m_tdThreadData.bEndThread = true;
		if ( m_ptThread.get() ) {
			m_eGo.Signal();
			m_eDone.WaitForSignal();
			m_ptThread->join();
			m_ptThread.reset();
		}
		m_tdThreadData.bEndThread = false;
	}

	/**
	 * The worker thread.
	 * 
	 * \param _ptdData Parameters passed to the thread.
	 **/
	void CNtscLSpiroFilter::WorkThread( LSN_THREAD_DATA * _ptdData ) {
		::SetThreadHighPriority();
		lsn::CScopedNoSubnormals snsNoSubnormals;
		auto pnlfThis = _ptdData->pnlsfThis;
		while ( !_ptdData->bEndThread ) {
			pnlfThis->m_eDone.Signal();
			pnlfThis->m_eGo.WaitForSignal();
			if ( _ptdData->bEndThread ) { break; }

			pnlfThis->RenderScanlineRange( _ptdData->pui8Pixels, _ptdData->ui16LinesDone, _ptdData->ui16EndLine, _ptdData->ui64RenderStartCycle, pnlfThis->m_vRgbBuffer.data(), pnlfThis->m_ui16ScaledWidth * 4 );
		}

		pnlfThis->m_eDone.Signal();
	}

}	// namespace lsn
