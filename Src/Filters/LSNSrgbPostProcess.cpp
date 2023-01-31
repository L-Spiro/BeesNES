/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The linear -> sRGB post-processing filter.
 */

#include "LSNSrgbPostProcess.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNFilterBase.h"


namespace lsn {

	CSrgbPostProcess::CSrgbPostProcess() :
#ifdef LSN_SRGB_POST_PERF
		m_pMonitor( "CSrgbPostProcess" ),
#endif	// #ifdef LSN_SRGB_POST_PERF
		m_bRunThreads( true ) {


		auto LinearTosRGB = [&]( double _dVal ) {
			return _dVal <= 0.0031308 ?
				_dVal * 12.92 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		};
		for ( auto I = LSN_ELEMENTS( m_ui8Table ); I--; ) {
			double dFract = LinearTosRGB( double( I ) / (LSN_ELEMENTS( m_ui8Table ) - 1) );
			m_ui8Table[I] = uint8_t( std::round( dFract * 255.0 ) );
		}

#if defined( LSN_USE_WINDOWS )
		m_tThreadData.pblppThis = this;
		m_tThreadData.pui8Table = m_ui8Table;
		m_ptResizeThread[0] = std::make_unique<std::thread>( Thread<0>, &m_tThreadData );
		m_ptResizeThread[1] = std::make_unique<std::thread>( Thread<1>, &m_tThreadData );
#endif	// #if defined( LSN_USE_WINDOWS )
	}
	CSrgbPostProcess::~CSrgbPostProcess() {
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
	uint8_t * CSrgbPostProcess::ApplyFilter( uint8_t * _pui8Input,
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


		

#if 1
		m_tThreadData.ui32ScreenHeight = _ui32Height;
		m_tThreadData.ui32ScreenWidth = _ui32Width;
		m_tThreadData.ui32Stride = ui32Stride;
		m_tThreadData.pui8Input = _pui8Input;
		for ( size_t I = 0; I < 2; ++I ) {
			if ( m_ptResizeThread[I].get() ) {
				m_eResizeGo[I].Signal();
			}
		}

		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			uint8_t * pui8Src = _pui8Input + Y * ui32Stride;
			uint8_t * pui8Dst = m_vFinalBuffer.data() + Y * ui32Stride;
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				pui8Dst[3] = pui8Src[3];
				/*pui8Dst[0] = m_ui8Table[pui8Src[0]];
				pui8Dst[1] = m_ui8Table[pui8Src[1]];*/
				pui8Dst[2] = m_ui8Table[pui8Src[2]];

				pui8Src += 4;
				pui8Dst += 4;
			}
		}


		for ( size_t I = 0; I < 2; ++I ) {
			if ( m_ptResizeThread[I].get() ) {
				m_eResizeDone[I].WaitForSignal();
			}
		}
#else
		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			uint8_t * pui8Src = _pui8Input + Y * ui32Stride;
			uint8_t * pui8Dst = m_vFinalBuffer.data() + Y * ui32Stride;
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				pui8Dst[3] = pui8Src[3];
				pui8Dst[0] = m_ui8Table[pui8Src[0]];
				pui8Dst[1] = m_ui8Table[pui8Src[1]];
				pui8Dst[2] = m_ui8Table[pui8Src[2]];

				pui8Src += 4;
				pui8Dst += 4;
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
	void CSrgbPostProcess::StopThreads() {
		m_bRunThreads = false;
		for ( size_t I = 0; I < 2; ++I ) {
			if ( m_ptResizeThread[I].get() ) {
				m_eResizeGo[I].Signal();
				m_eResizeDone[I].WaitForSignal();
				m_ptResizeThread[I]->join();
				m_ptResizeThread[I].reset();
			}
		}
		m_bRunThreads = true;
	}


}	// namespace lsn
