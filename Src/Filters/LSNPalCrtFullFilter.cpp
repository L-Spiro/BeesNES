/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: EMMIR (LMP88959)'s NTSC filter.
 */

#include "LSNPalCrtFullFilter.h"
#include "../Utilities/LSNUtilities.h"
#include "PAL-CRT-Full/pal_core.h"

#define m_nsSettings				(*reinterpret_cast<PAL_SETTINGS *>(m_vSettings.data()))
#define m_nnCrtPal					(*reinterpret_cast<PAL_CRT *>(m_vCrtNtsc.data()))

namespace lsn {

	CPalCrtFullFilter::CPalCrtFullFilter() :
		m_ui32FinalStride( 0 ),
		m_ui32FinalWidth( PAL_HRES ),
		m_ui32FinalHeight( 0 ),
		m_bRunThreads( true ) {

		m_vSettings.resize( sizeof( PAL_SETTINGS ) );
		m_vCrtNtsc.resize( sizeof( PAL_CRT ) );

#ifdef LSN_CRT_PERF
		m_ui32Calls = 0;
		m_ui64AccumTime = 0;
#endif	// #ifdef LSN_CRT_PERF
	}
	CPalCrtFullFilter::~CPalCrtFullFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CPalCrtFullFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
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
		m_ui32FinalWidth = PAL_HRES * ui32Scale;
		m_ui32FinalHeight = _ui16Height * ui32Scale;
		m_ui32FinalStride = RowStride( m_ui32FinalWidth, OutputBits() );
		m_vFilteredOutput.resize( m_ui32FinalStride * m_ui32FinalHeight );

		::pal_init( &m_nnCrtPal, m_ui32FinalWidth, m_ui32FinalHeight, PAL_PIX_FORMAT_BGRA, m_vFilteredOutput.data() );
		//PAL_CRT psSetts = (*reinterpret_cast<PAL_CRT *>(m_vCrtNtsc.data()));
		/*m_nnCrtPal.brightness = -14;
		m_nnCrtPal.contrast = 125;
		m_nnCrtPal.saturation = 9;
		m_nnCrtPal.blend = 1;
		//m_nnCrtPal.scanlines = 1;
		m_nnCrtPal.white_point = 80;*/
		//m_nnCrtPal.saturation = 12;
		//m_nnCrtPal.white_point = 80;
		/*m_nnCrtPal.brightness = 0;
		m_nnCrtPal.contrast = 150;
		m_nnCrtPal.saturation = 12;
		m_nnCrtPal.white_point = 80;*/
		//m_nnCrtPal.blend = 1;

		/*m_nnCrtPal.brightness = 0;
		m_nnCrtPal.contrast = 165;
		m_nnCrtPal.saturation = 16;
		m_nnCrtPal.black_point = 2;
		m_nnCrtPal.white_point = 85;
		m_nnCrtPal.blend = 1;

		m_nnCrtPal.chroma_correction = 1;
		m_nsSettings.yoffset = 7;*/
		
		//m_nnCrtPal.hue = 23;
		m_nnCrtPal.brightness = 4;
		m_nnCrtPal.contrast = 180;
		m_nnCrtPal.saturation = 17;
		m_nnCrtPal.black_point = 4;
		m_nnCrtPal.white_point = 75;
		m_nnCrtPal.blend = 1;

		m_nnCrtPal.chroma_correction = 1;
		m_nsSettings.yoffset = 7;
		

		/** 
		* 		hue	0	int
				brightness	0	int
				contrast	180	int
				saturation	10	int
				black_point	0	int
				white_point	100	int
				scanlines	0	int
				blend	1	int
		*/

		StopPhospherDecayThread();
#if defined( LSN_WINDOWS )
		m_ptPhospherThread = std::make_unique<std::thread>( PhospherDecayThread, this );
#endif	// #if defined( LSN_WINDOWS )

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
	uint8_t * CPalCrtFullFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/ ) {
#ifdef LSN_CRT_PERF
		uint64_t ui64TimeNow = m_cPerfClock.GetRealTick();
#endif	// #ifdef LSN_CRT_PERF
		// Fade the phosphers.
		if ( m_nnCrtPal.blend ) {
			if ( m_ptPhospherThread.get() ) {
				m_ePhospherGo.Signal();
			}
		}

		m_nsSettings.data = reinterpret_cast<unsigned short *>(_pui8Input);
		m_nsSettings.w = int( m_ui32OutputWidth );
		m_nsSettings.h = int( m_ui32OutputHeight );
		

		::pal_modulate( &m_nnCrtPal, &m_nsSettings );

		if ( m_nnCrtPal.blend ) {
			if ( m_ptPhospherThread.get() ) {
				m_ePhospherDone.WaitForSignal();
			}
		}

		::pal_demodulate( &m_nnCrtPal, 4 );
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
	void CPalCrtFullFilter::Activate() {
		std::memset( m_vFilteredOutput.data(), 0, m_vFilteredOutput.size() );
	}

	/**
	 * Stops the phospher-decay thread.
	 */
	void CPalCrtFullFilter::StopPhospherDecayThread() {
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
	void CPalCrtFullFilter::PhospherDecayThread( CPalCrtFullFilter * _pncfFilter ) {
		while ( _pncfFilter->m_bRunThreads ) {
			_pncfFilter->m_ePhospherGo.WaitForSignal();
			if ( _pncfFilter->m_bRunThreads ) {
				// Apply smear.
#if 0
				for ( auto Y = _pncfFilter->m_ui32FinalHeight; Y--; ) {
					uint32_t ui32Val = 0;
					uint32_t * pui32Src = reinterpret_cast<uint32_t *>(_pncfFilter->m_vFilteredOutput.data() + Y * _pncfFilter->m_ui32FinalStride);
					for ( uint32_t X = 0; X < _pncfFilter->m_ui32FinalWidth; ++X ) {
						ui32Val = /*((ui32Val >> 1) & 0x7F7F7F7F) +*/
							((ui32Val >> 2) & 0x3F3F3F3F) +
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
#if 0
				CUtilities::DecayArgb( reinterpret_cast<uint32_t *>(_pncfFilter->m_vFilteredOutput.data()), _pncfFilter->m_ui32FinalWidth, _pncfFilter->m_ui32FinalHeight );
#endif
			}
			_pncfFilter->m_ePhospherDone.Signal();
		}
	}

}	// namespace lsn

#undef m_nnCrtPal
#undef m_nsSettings
