/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The bleed post-processing filter.
 */

#include "LSNBleedPostProcess.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNFilterBase.h"


namespace lsn {

	CBleedPostProcess::CBleedPostProcess() :
#ifdef LSN_SRGB_POST_PERF
		m_pMonitor( "CBleedPostProcess" ),
#endif	// #ifdef LSN_SRGB_POST_PERF
		m_bRunThreads( true ) {

#if defined( LSN_USE_WINDOWS )
		m_tThreadData.pblppThis = this;
		m_ptResizeThread = std::make_unique<std::thread>( Thread, &m_tThreadData );
#endif	// #if defined( LSN_USE_WINDOWS )
	}
	CBleedPostProcess::~CBleedPostProcess() {
		StopThreads();
	}


	// == Functions.
	/**
	 * Applies the filter to the given input
	 *
	 * \param _ui32ScreenWidth The screen width.
	 * \param _ui32ScreenHeight The screen height.
	 * \param _bFlipped Indicates if the image is flipped on input and set to true to indicate that the image is in a flipped state on output.
	 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
	 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
	 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
	 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
	 * \param _ui64PpuFrame The PPU frame associated with the input data.
	 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CBleedPostProcess::ApplyFilter( uint8_t * _pui8Input,
		uint32_t /*_ui32ScreenWidth*/, uint32_t /*_ui32ScreenHeight*/, bool &/*_bFlipped*/,
		uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/,
		uint64_t /*_ui64RenderStartCycle*/ ) {
		uint32_t ui32Stride = CFilterBase::RowStride( _ui32Width, 32 );
		
		uint32_t ui32Size = ui32Stride * _ui32Height;
		if ( m_vFinalBuffer.size() != ui32Size ) {
			m_vFinalBuffer = std::vector<uint8_t>();
			m_vFinalBuffer.resize( ui32Size );
		}

#ifdef LSN_SRGB_POST_PERF
		m_pMonitor.Begin();
#endif	// #ifdef LSN_SRGB_POST_PERF


#define LSN_BLEED	ui32Val = /*((ui32Val >> 1) & 0x7F7F7F7F) +*/			\
						/*((ui32Val >> 2) & 0x3F3F3F3F) +*/					\
						((ui32Val >> 3) & 0x1F1F1F1F) +						\
						((ui32Val >> 6) & 0x03030303) +						\
						((ui32Val >> 6) & 0x03030303) +						\
						((ui32Val >> 7) & 0x01010101) +						\
																			\
						((ui32Val >> 4) & 0x0F0F0F0F) +						\
						((ui32Val >> 5) & 0x07070707) +						\
						((ui32Val >> 6) & 0x03030303) +						\
						((ui32Val >> 7) & 0x01010101)

#if 1
		m_tThreadData.ui32ScreenHeight = _ui32Height;
		m_tThreadData.ui32ScreenWidth = _ui32Width;
		m_tThreadData.ui32Stride = ui32Stride;
		m_tThreadData.pui8Input = _pui8Input;
		{
			if ( m_ptResizeThread.get() ) {
				m_eResizeGo.Signal();
			}
		}

		uint32_t ui32H = _ui32Height >> 1;
		for ( uint32_t Y = 0; Y < ui32H; ++Y ) {
			uint32_t ui32Val = 0;
			uint32_t * pui32Src = reinterpret_cast<uint32_t *>(_pui8Input + Y * ui32Stride);
			uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(m_vFinalBuffer.data() + Y * ui32Stride);
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				LSN_BLEED;
				ui32Val = pui32Dst[X] = CUtilities::AddArgb( pui32Src[X], ui32Val );
			}
		}


		{
			if ( m_ptResizeThread.get() ) {
				m_eResizeDone.WaitForSignal();
			}
		}
#else
		for ( auto Y = _ui32Height; Y--; ) {
			uint32_t ui32Val = 0;
			uint32_t * pui32Src = reinterpret_cast<uint32_t *>(_pui8Input + Y * ui32Stride);
			uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(m_vFinalBuffer.data() + Y * ui32Stride);
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				LSN_BLEED;
				ui32Val = pui32Dst[X] = CUtilities::AddArgb( pui32Src[X], ui32Val );
			}
		}
#endif

#ifdef LSN_SRGB_POST_PERF
		m_pMonitor.Stop();
#endif	// #ifdef LSN_SRGB_POST_PERF
		/*_ui32Width = _ui32ScreenWidth;
		_ui32Height = _ui32ScreenHeight;*/
		_ui32Stride = ui32Stride;
		return m_vFinalBuffer.data();
	}

	/**
	 * Stops the threads.
	 */
	void CBleedPostProcess::StopThreads() {
		m_bRunThreads = false;
		{
			if ( m_ptResizeThread.get() ) {
				m_eResizeGo.Signal();
				m_eResizeDone.WaitForSignal();
				m_ptResizeThread->join();
				m_ptResizeThread.reset();
			}
		}
		m_bRunThreads = true;
	}

	/**
	 * The work thread.
	 *
	 * \param _pblppFilter Pointer to this object.
	 */
	void CBleedPostProcess::Thread( LSN_THREAD * _ptThread ) {
		while ( _ptThread->pblppThis->m_bRunThreads ) {
			_ptThread->pblppThis->m_eResizeGo.WaitForSignal();
			if ( _ptThread->pblppThis->m_bRunThreads ) {
				uint32_t ui32H = _ptThread->ui32ScreenHeight;
				uint32_t ui32W = _ptThread->ui32ScreenWidth;
				uint32_t ui32Stride = _ptThread->ui32Stride;
				for ( uint32_t Y = ui32H >> 1; Y < ui32H; ++Y ) {
					uint32_t ui32Val = 0;
					uint32_t * pui32Src = reinterpret_cast<uint32_t *>(_ptThread->pui8Input + Y * ui32Stride);
					uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(_ptThread->pblppThis->m_vFinalBuffer.data() + Y * ui32Stride);
					for ( uint32_t X = 0; X < ui32W; ++X ) {
						LSN_BLEED;
						ui32Val = pui32Dst[X] = CUtilities::AddArgb( pui32Src[X], ui32Val );
					}
				}
			}
			_ptThread->pblppThis->m_eResizeDone.Signal();
		}
	}


}	// namespace lsn
