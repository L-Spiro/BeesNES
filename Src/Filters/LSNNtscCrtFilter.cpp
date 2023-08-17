/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: EMMIR (LMP88959)'s NTSC filter.
 */

#include "LSNNtscCrtFilter.h"
#include "../Utilities/LSNUtilities.h"
#include "NTSC-CRT-Fast/crt.h"
#include <Helpers/LSWHelpers.h>

#define m_nsSettings				(*reinterpret_cast<NES_NTSC_SETTINGS *>(m_vSettings.data()))
#define m_nnCrtNtsc					(*reinterpret_cast<CRT *>(m_vCrtNtsc.data()))

namespace lsn {

	CNtscCrtFilter::CNtscCrtFilter() :
#ifdef LSN_CRT_PERF
		m_pMonitor( "CNtscCrtFilter" ),
#endif	// #ifdef LSN_CRT_PERF
		m_ui32FinalStride( 0 ),
		m_ui32FinalWidth( CRT_HRES ),
		m_ui32FinalHeight( 0 ),
		m_bRunThreads( true ) {
		int iPhases[4] = { 0, 16, 0, -16 };
		std::memcpy( m_iPhaseRef, iPhases, sizeof( iPhases ) );

		m_vSettings.resize( sizeof( NES_NTSC_SETTINGS ) );
		m_vCrtNtsc.resize( sizeof( CRT ) );
	}
	CNtscCrtFilter::~CNtscCrtFilter() {
		StopPhospherDecayThread();
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CNtscCrtFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( m_ui32OutputWidth * sizeof( uint16_t ) );

		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( m_stStride * _ui16Height );
			for ( auto J = m_vBasicRenderTarget[I].size() / sizeof( uint16_t ); J--; ) {
				(*reinterpret_cast<uint16_t *>(&m_vBasicRenderTarget[I][J*sizeof( uint16_t )])) = 0x0F;
			}
		}

		constexpr uint32_t ui32Scale = 1;
		m_ui32FinalWidth = CRT_HRES * ui32Scale;
		m_ui32FinalHeight = _ui16Height * ui32Scale;
		m_ui32FinalStride = RowStride( m_ui32FinalWidth, OutputBits() );
		m_vFilteredOutput.resize( m_ui32FinalStride * m_ui32FinalHeight );

		::crt_init( &m_nnCrtNtsc, m_ui32FinalWidth, m_ui32FinalHeight, reinterpret_cast<int *>(m_vFilteredOutput.data()) );
		/*m_nnCrtNtsc.brightness = 7;
		m_nnCrtNtsc.contrast = 172;
		m_nnCrtNtsc.black_point = 20;
		m_nnCrtNtsc.white_point = 120;*/

		
		m_nsSettings.as_color = 1;
		constexpr int iPhaseOffset = 2;
		m_nsSettings.cc[0] = m_iPhaseRef[(iPhaseOffset + 0) & 3];
		m_nsSettings.cc[1] = m_iPhaseRef[(iPhaseOffset + 1) & 3];
		m_nsSettings.cc[2] = m_iPhaseRef[(iPhaseOffset + 2) & 3];
		m_nsSettings.cc[3] = m_iPhaseRef[(iPhaseOffset + 3) & 3];
		m_nsSettings.ccs = 16;

		StopPhospherDecayThread();
#if defined( LSN_USE_WINDOWS )
		m_ptPhospherThread = std::make_unique<std::thread>( PhospherDecayThread, this );
#endif	// #if defined( LSN_USE_WINDOWS )

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
	uint8_t * CNtscCrtFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle ) {
#ifdef LSN_CRT_PERF
		m_pMonitor.Begin();
#endif	// #ifdef LSN_CRT_PERF
		// Fade the phosphers.
		{
			if ( m_ptPhospherThread.get() ) {
				m_ePhospherGo.Signal();
			}
			else {
				CUtilities::DecayArgb( reinterpret_cast<uint32_t *>(m_vFilteredOutput.data()), m_ui32FinalWidth, m_ui32FinalHeight );
			}
		}

		m_nsSettings.data = reinterpret_cast<unsigned short *>(_pui8Input);
		m_nsSettings.w = int( m_ui32OutputWidth );
		m_nsSettings.h = int( m_ui32OutputHeight );
		m_nsSettings.dot_crawl_offset = _ui64RenderStartCycle % 3;

		::crt_nes2ntsc( &m_nnCrtNtsc, &m_nsSettings );

		{
			if ( m_ptPhospherThread.get() ) {
				m_ePhospherDone.WaitForSignal();
			}
		}

		::crt_draw( &m_nnCrtNtsc, 3 );
		_ui32Width = m_ui32FinalWidth;
		_ui32Height = m_ui32FinalHeight;
		_ui32Stride = m_ui32FinalStride;

#ifdef LSN_CRT_PERF
		m_pMonitor.Stop();
#endif	// #ifdef LSN_CRT_PERF
		return m_vFilteredOutput.data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CNtscCrtFilter::Activate() {
		std::memset( m_vFilteredOutput.data(), 0, m_vFilteredOutput.size() );
	}

	/**
	 * Stops the phospher-decay thread.
	 */
	void CNtscCrtFilter::StopPhospherDecayThread() {
		m_bRunThreads = false;
		if ( m_ptPhospherThread.get() ) {
			m_ePhospherGo.Signal();
			m_ePhospherDone.WaitForSignal();
			m_ptPhospherThread->join();
			m_ptPhospherThread.reset();
		}
		m_bRunThreads = true;
	}

	/**
	 * The phospher-decay thread.
	 *
	 * \param _pncfFilter Pointer to this object.
	 */
	void CNtscCrtFilter::PhospherDecayThread( CNtscCrtFilter * _pncfFilter ) {
		while ( _pncfFilter->m_bRunThreads ) {
			_pncfFilter->m_ePhospherGo.WaitForSignal();
			if ( _pncfFilter->m_bRunThreads ) {

				// Apply smear.
#if 1
				for ( auto Y = _pncfFilter->m_ui32FinalHeight; Y--; ) {
					uint32_t ui32Val = 0;
					uint32_t * pui32Src = reinterpret_cast<uint32_t *>(_pncfFilter->m_vFilteredOutput.data() + Y * _pncfFilter->m_ui32FinalStride);
					for ( uint32_t X = 0; X < _pncfFilter->m_ui32FinalWidth; ++X ) {
						ui32Val = /*((ui32Val >> 1) & 0x7F7F7F7F) +*/
							/*((ui32Val >> 2) & 0x3F3F3F3F) +*/
							((ui32Val >> 3) & 0x1F1F1F1F) +
							((ui32Val >> 6) & 0x03030303) +
							((ui32Val >> 7) & 0x01010101) +

							((ui32Val >> 4) & 0x0F0F0F0F) +
							((ui32Val >> 5) & 0x07070707) +
							((ui32Val >> 6) & 0x03030303) +
							((ui32Val >> 7) & 0x01010101);
						ui32Val = pui32Src[X] = CUtilities::AddArgb( pui32Src[X], ui32Val );
					}
				}
#endif


				CUtilities::DecayArgb( reinterpret_cast<uint32_t *>(_pncfFilter->m_vFilteredOutput.data()), _pncfFilter->m_ui32FinalWidth, _pncfFilter->m_ui32FinalHeight );
			}
			_pncfFilter->m_ePhospherDone.Signal();
		}
	}

}	// namespace lsn

#undef m_nnCrtNtsc
#undef m_nsSettings
