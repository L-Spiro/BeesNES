/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: EMMIR (LMP88959)’s NTSC filter.
 */

#include "LSNNtscCrtFullFilter.h"
#include "../Utilities/LSNUtilities.h"
#include "NTSC-CRT-Full/crt_core.h"
#include <Helpers/LSWHelpers.h>

#define m_nsSettings				(*reinterpret_cast<NTSC_SETTINGS *>(m_vSettings.data()))
#define m_nnCrtNtsc					(*reinterpret_cast<CRT *>(m_vCrtNtsc.data()))

namespace lsn {

	CNtscCrtFullFilter::CNtscCrtFullFilter() :
		m_ui32FinalStride( 0 ),
		m_ui32FinalWidth( CRT_HRES ),
		m_ui32FinalHeight( 0 ),
		m_bRunThreads( true ) {
		int iPhases[4] = { 0, 16, 0, -16 };
		std::memcpy( m_iPhaseRef, iPhases, sizeof( iPhases ) );

		m_vSettings.resize( sizeof( NTSC_SETTINGS ) );
		m_vCrtNtsc.resize( sizeof( CRT ) );

#ifdef LSN_CRT_PERF
		m_ui32Calls = 0;
		m_ui64AccumTime = 0;
#endif	// #ifdef LSN_CRT_PERF
	}
	CNtscCrtFullFilter::~CNtscCrtFullFilter() {
		StopPhospherDecayThread();
#ifdef LSN_CRT_PERF
		char szBuffer[128];
		std::sprintf( szBuffer, "CRT Time: %.17f\r\n", m_ui64AccumTime / double( m_ui32Calls ) / m_cPerfClock.GetResolution() * 1000.0 );
		::OutputDebugStringA( szBuffer );
#endif	// #ifdef LSN_CRT_PERF
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CNtscCrtFullFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
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

		::crt_init_full( &m_nnCrtNtsc, m_ui32FinalWidth, m_ui32FinalHeight, reinterpret_cast<int *>(m_vFilteredOutput.data()) );
		/*m_nnCrtNtsc.brightness = -14;
		m_nnCrtNtsc.contrast = 172;*/
		m_nnCrtNtsc.blend = 1;
		m_nnCrtNtsc.scanlines = 1;

		m_nsSettings.border_color = 0x22;

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
	uint8_t * CNtscCrtFullFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle ) {
#ifdef LSN_CRT_PERF
		uint64_t ui64TimeNow = m_cPerfClock.GetRealTick();
#endif	// #ifdef LSN_CRT_PERF
		// Fade the phosphers.
		{
			if ( m_ptPhospherThread.get() ) {
				m_ePhospherGo.Signal();
			}
			else {
				uint32_t ui32Total = m_ui32FinalWidth * m_ui32FinalHeight;
				if ( (ui32Total & 1) == 0 ) {
					uint64_t * pui64Src = reinterpret_cast<uint64_t *>(m_vFilteredOutput.data());
					for ( uint32_t I = ui32Total >> 1; I--; ) {
						//pui32Src[I] = 0;
						uint64_t ui64Tmp = pui64Src[I] & 0x00FFFFFF00FFFFFFULL;
						pui64Src[I] = ((ui64Tmp >> 1) & 0x007F7F7F007F7F7FULL) +  
							((ui64Tmp >> 2) & 0x003F3F3F003F3F3FULL) + 
							((ui64Tmp >> 3) & 0x001F1F1F001F1F1FULL) + 
							((ui64Tmp >> 4) & 0x000F0F0F000F0F0FULL);
					}
				}
				else {
					uint32_t * pui32Src = reinterpret_cast<uint32_t *>(m_vFilteredOutput.data());
					for ( uint32_t I = ui32Total; I--; ) {
						//pui32Src[I] = 0;
						uint32_t ui32Tmp = pui32Src[I] & 0x00FFFFFF;
						pui32Src[I] = ((ui32Tmp >> 1) & 0x7F7F7F) +  
							((ui32Tmp >> 2) & 0x3F3F3F) + 
							((ui32Tmp >> 3) & 0x1F1F1F) + 
							((ui32Tmp >> 4) & 0x0F0F0F);
					}
				}
			}
		}

		m_nsSettings.data = reinterpret_cast<unsigned short *>(_pui8Input);
		m_nsSettings.w = int( m_ui32OutputWidth );
		m_nsSettings.h = int( m_ui32OutputHeight );
		m_nsSettings.dot_crawl_offset = _ui64RenderStartCycle % 3;

		::crt_modulate_full( &m_nnCrtNtsc, &m_nsSettings );

		{
			if ( m_ptPhospherThread.get() ) {
				m_ePhospherDone.WaitForSignal();
			}
		}

		::crt_demodulate_full( &m_nnCrtNtsc, 3 );
		_ui32Width = m_ui32FinalWidth;
		_ui32Height = m_ui32FinalHeight;
		_ui32Stride = m_ui32FinalStride;

#ifdef LSN_CRT_PERF
		m_ui32Calls++;
		m_ui64AccumTime += m_cPerfClock.GetRealTick() - ui64TimeNow;
#endif	// #ifdef LSN_CRT_PERF
		return m_vFilteredOutput.data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CNtscCrtFullFilter::Activate() {
		std::memset( m_vFilteredOutput.data(), 0, m_vFilteredOutput.size() );
	}

	/**
	 * Stops the phospher-decay thread.
	 */
	void CNtscCrtFullFilter::StopPhospherDecayThread() {
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
	void CNtscCrtFullFilter::PhospherDecayThread( CNtscCrtFullFilter * _pncfFilter ) {
		while ( _pncfFilter->m_bRunThreads ) {
			_pncfFilter->m_ePhospherGo.WaitForSignal();
			if ( _pncfFilter->m_bRunThreads ) {
				uint32_t ui32Total = _pncfFilter->m_ui32FinalWidth * _pncfFilter->m_ui32FinalHeight;
				if ( (ui32Total & 1) == 0 ) {
					uint64_t * pui64Src = reinterpret_cast<uint64_t *>(_pncfFilter->m_vFilteredOutput.data());
					for ( uint32_t I = ui32Total >> 1; I--; ) {
						//pui64Src[I] = 0;
#if 1
						uint64_t ui64Tmp = pui64Src[I] & 0x00FFFFFF00FFFFFFULL;
						pui64Src[I] = ((ui64Tmp >> 1) & 0x007F7F7F007F7F7FULL) +
							/*((ui64Tmp >> 2) & 0x003F3F3F003F3F3FULL) +*/
							/*((ui64Tmp >> 3) & 0x001F1F1F001F1F1FULL) +*/
							((ui64Tmp >> 4) & 0x000F0F0F000F0F0FULL);
#endif
					}
				}
				else {
					uint32_t * pui32Src = reinterpret_cast<uint32_t *>(_pncfFilter->m_vFilteredOutput.data());
					for ( uint32_t I = ui32Total; I--; ) {
						//pui32Src[I] = 0;
						uint32_t ui32Tmp = pui32Src[I] & 0x00FFFFFF;
						pui32Src[I] = ((ui32Tmp >> 1) & 0x7F7F7F) +
							/*((ui32Tmp >> 2) & 0x3F3F3F) +
							((ui32Tmp >> 3) & 0x1F1F1F) +*/
							((ui32Tmp >> 4) & 0x0F0F0F);
					}
				}

				// Apply smear.
#if 1
				for ( auto Y = _pncfFilter->m_ui32FinalHeight; Y--; ) {
					uint32_t ui32Val = 0;
					uint32_t * pui32Src = reinterpret_cast<uint32_t *>(_pncfFilter->m_vFilteredOutput.data() + Y * _pncfFilter->m_ui32FinalStride);
					for ( uint32_t X = 0; X < _pncfFilter->m_ui32FinalWidth; ++X ) {
						ui32Val = /*((ui32Val >> 1) & 0x7F7F7F7F) +
							((ui32Val >> 2) & 0x3F3F3F3F) +*/
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
			}
			_pncfFilter->m_ePhospherDone.Signal();
		}
	}

}	// namespace lsn

#undef m_nnCrtNtsc
#undef m_nsSettings
