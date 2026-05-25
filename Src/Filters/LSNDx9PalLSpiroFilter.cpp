#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of a PAL filter.
 */

#include "LSNDx9PalLSpiroFilter.h"
#include "../GPU/DirectX9/LSNDirectX9DiskInclude.h"
#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

namespace lsn {

	// == Members.
	CDx9PalLSpiroFilter::CDx9PalLSpiroFilter() {
		SetMonitorGammaApply( false );
	}
	CDx9PalLSpiroFilter::~CDx9PalLSpiroFilter() {
		StopThreads();
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx9PalLSpiroFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		StopThreads();
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_ui32FinalStride = RowStride( m_ui32OutputWidth, OutputBits() );
		AllocYiqBuffers( _ui16Width, _ui16Height, m_ui16WidthScale );

		m_tuUploader.Reset();
		m_trRenderer.Reset();
		ReleaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );
		m_stStride = size_t( m_ui32OutputWidth * sizeof( uint16_t ) );

		StartThreads();
		return pofOut;
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
	 * \param _i32DispLeft The display area left.
	 * \param _i32DispTop The display area top.
	 * \param _ui32DispWidth The display area width.
	 * \param _ui32DispHeight The display area height
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CDx9PalLSpiroFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		if LSN_UNLIKELY( !m_pdx9dDevice ) { return m_vBasicRenderTarget[0].data(); }
		if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
			m_ui32SrcW = _ui32Width;
			m_ui32SrcH = _ui32Height;
			EnsureSizeAndResources();
			AllocYiqBuffers( uint16_t( m_ui32SrcW ), uint16_t( m_ui32SrcH ), m_ui16WidthScale );
		}
		
		lsw::LSW_RECT rRect;
		rRect.left = LONG( _i32DispLeft );
		rRect.top = LONG( _i32DispTop );
		rRect.right = rRect.left + LONG( _ui32DispWidth );
		rRect.bottom = rRect.top + LONG( _ui32DispHeight );

		const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
		m_vRgbBuffer.resize( m_ui16ScaledWidth * m_ui32SrcH * 4 * sizeof( float ) );

		FilterFrame( _pui8Input, _ui64RenderStartCycle + 2 );

		m_tuUploader.UploadTexels( m_pdx9dDevice, m_vRgbBuffer.data(), m_ui16ScaledWidth, m_ui32SrcH, ui32Pitch, D3DFMT_A32B32G32R32F );

		Render( rRect );

		_ui32Width = uint32_t( s_dgsState.rScreenRect.Width() );
		_ui32Height = uint32_t( s_dgsState.rScreenRect.Height() );
		_ui32Stride = _ui32Width * sizeof( uint32_t );
		return m_vBasicRenderTarget[0].data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx9PalLSpiroFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
		AllocYiqBuffers( uint16_t( m_ui32SrcW ), uint16_t( m_ui32SrcH ), m_ui16WidthScale );
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx9PalLSpiroFilter::DeActivate() {
		CParent::DeActivate();

		m_tuUploader.Reset();
		m_tpsScaler.Reset();
		m_trRenderer.Reset();

		if ( m_pdx9dDevice ) {
			s_dgsState.DestroyDx9();
			m_pdx9dDevice = nullptr;
		}
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CDx9PalLSpiroFilter::FrameResize() {
		s_dgsState.OnSizeDx9();
		EnsureSizeAndResources();
	}

	/**
	 * Sets the number of worker threads used by the filter.
	 *
	 * \param _stThreads Number of worker threads to use.  0 disables worker threads.
	 */
	void CDx9PalLSpiroFilter::SetWorkerThreadCount( size_t _stThreads ) {
		if ( _stThreads == m_stWorkerThreadCount ) { return; }

		const bool bRestart = m_bThreadsStarted;
		if ( bRestart ) { StopThreads(); }
		m_stWorkerThreadCount = _stThreads;
		if ( bRestart ) { StartThreads(); }
	}

	/**
	 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
	 **/
	void CDx9PalLSpiroFilter::FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle ) {
		const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
		if LSN_UNLIKELY( !m_vThreads.size() ) {
			RenderScanlineRange<false>( _pui8Pixels, 0, m_ui16Height, _ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );
			return;
		}

		const size_t stThreads = m_vThreads.size() + 1;
		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_jJob.pui8Pixels = _pui8Pixels;
			m_jJob.ui64RenderStartCycle = _ui64RenderStartCycle;
			m_jJob.stThreads = stThreads;
			++m_ui64JobId;
			m_ui32WorkersRemaining.store( uint32_t( m_vThreads.size() ) );
		}
		m_cvGo.notify_all();

		const uint16_t ui16Lines = m_ui16Height;
		const uint16_t ui16Start = 0;
		const uint16_t ui16End = uint16_t( (uint32_t( ui16Lines ) * 1U) / uint32_t( stThreads ) );
		RenderScanlineRange<false>( _pui8Pixels, ui16Start, ui16End, _ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );

		std::unique_lock<std::mutex> ulLock( m_mThreadMutex );
		m_cvDone.wait( ulLock, [&]() { return m_ui32WorkersRemaining.load() == 0; } );
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * 
	 * \return Returns true on success.
	 */
	bool CDx9PalLSpiroFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pdx9dDevice ) {
			if ( !s_dgsState.CreateDx9() ) { return false; }
			m_pdx9dDevice = &s_dgsState.dx9Device;
			m_tuUploader.Reset();
			m_tpsScaler.Reset();
			m_trRenderer.Reset();
		}

		if ( m_ui32RsrcW == m_ui32SrcW && m_ui32RsrcH == m_ui32SrcH ) {
			m_bValidState = true; 
			return true;
		}

		ReleaseSizeDependents();
		m_bValidState = true;
		m_ui32RsrcW = m_ui32SrcW;
		m_ui32RsrcH = m_ui32SrcH;
		return true;
	}

	/**
	 * \brief Releases size-dependent resources (index texture, FP RTs, quad VB).
	 */
	void CDx9PalLSpiroFilter::ReleaseSizeDependents() {
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * Renders the final output to the backbuffer.
	 *
	 * \param _rOutput The destination rectangle.
	 * \return Returns true if rendering succeeded.
	 */
	bool CDx9PalLSpiroFilter::Render( const lsw::LSW_RECT &_rOutput ) {
		if LSN_UNLIKELY( !m_bValidState || !m_tuUploader.GetTexture() || !m_tuUploader.GetTexture()->Valid() ) { return false; }
		IDirect3DDevice9 * pd3d9dDevice = m_pdx9dDevice->GetDirectX9Device();
		if LSN_UNLIKELY( !pd3d9dDevice ) { return false; }


		if ( !m_tpsScaler.Render( m_pdx9dDevice, m_tuUploader.GetTexture()->Get(), m_ui16ScaledWidth, m_ui32SrcH, GetActualHorSharpness(), GetActualVertSharpness(), CNesPalette::LSN_G_CRT1, m_bUse16BitInitialTarget ) ) {
			return false;
		}


		IDirect3DSurface9 * psBackBuffer = nullptr;
		if LSN_LIKELY( SUCCEEDED( pd3d9dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &psBackBuffer ) ) ) {
			m_trRenderer.Render( m_pdx9dDevice, m_tpsScaler.GetTexture()->Get(), psBackBuffer, _rOutput, 1.0f, true, true );
			psBackBuffer->Release();
		}

		return true;
	}

	/**
	 * \brief Starts the worker threads.
	 *
	 * Creates m_vThreads based on m_stWorkerThreadCount and resets the thread-control state.
	 * Safe to call multiple times; if threads are already started, this function does nothing.
	 */
	void CDx9PalLSpiroFilter::StartThreads() {
		if ( m_bThreadsStarted ) { return; }

		const size_t stWorkers = m_stWorkerThreadCount;
		m_bStopThreads = false;
		m_ui64JobId = 0;
		m_ui32WorkersRemaining.store( 0 );
		m_vThreads.clear();

		if ( stWorkers ) {
			m_vThreads.reserve( stWorkers );
			for ( size_t I = 0; I < stWorkers; ++I ) {
				m_vThreads.emplace_back( &CDx9PalLSpiroFilter::WorkerThread, this, I + 1 );
			}
		}

		m_bThreadsStarted = true;
	}

	/**
	 * \brief Stops the worker threads.
	 *
	 * Signals all worker threads to exit, wakes them, joins them, clears m_vThreads, and
	 * resets thread-control state.  Safe to call multiple times; if threads are not started,
	 * this function does nothing.
	 */
	void CDx9PalLSpiroFilter::StopThreads() {
		if ( !m_bThreadsStarted ) { return; }

		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_bStopThreads = true;
		}
		m_cvGo.notify_all();

		for ( auto & T : m_vThreads ) {
			if ( T.joinable() ) {
				T.join();
			}
		}
		m_vThreads.clear();

		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_bStopThreads = false;
		}
		m_ui32WorkersRemaining.store( 0 );
		m_bThreadsStarted = false;
	}

	/**
	 * \brief The worker thread entry point.
	 *
	 * Waits for jobs signaled via m_cvGo, renders the scanline range assigned to this worker,
	 * then decrements m_ui32WorkersRemaining and notifies m_cvDone when the final worker
	 * finishes the job.
	 *
	 * \param _stThreadIdx The worker thread index in the range [1, stThreads - 1].
	 *	Index 0 is reserved for the calling thread.
	 */
	void CDx9PalLSpiroFilter::WorkerThread( size_t _stThreadIdx ) {
		::SetThreadHighPriority();
		lsn::CScopedNoSubnormals snsNoSubnormals;

		uint64_t ui64LastJobId = 0;

		for ( ;; ) {
			LSN_JOB jJob;
			{
				std::unique_lock<std::mutex> ulLock( m_mThreadMutex );
				m_cvGo.wait( ulLock, [&]() { return m_bStopThreads || m_ui64JobId != ui64LastJobId; } );
				if ( m_bStopThreads ) { break; }

				ui64LastJobId = m_ui64JobId;
				jJob = m_jJob;
			}

			const uint16_t ui16Lines = m_ui16Height;
			const uint16_t ui16Start = uint16_t( (uint32_t( ui16Lines ) * uint32_t( _stThreadIdx )) / uint32_t( jJob.stThreads ) );
			const uint16_t ui16End = uint16_t( (uint32_t( ui16Lines ) * uint32_t( _stThreadIdx + 1 )) / uint32_t( jJob.stThreads ) );
			
			if ( ui16End > ui16Start ) {
				const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
				RenderScanlineRange<false>( jJob.pui8Pixels, ui16Start, ui16End, jJob.ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );
			}

			if ( m_ui32WorkersRemaining.fetch_sub( 1 ) == 1 ) {
				std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
				m_cvDone.notify_one();
			}
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
