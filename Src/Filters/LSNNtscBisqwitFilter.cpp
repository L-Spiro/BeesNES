/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Bisqwit’s NTSC filter.
 */

#include "LSNNtscBisqwitFilter.h"
#include "../Utilities/LSNUtilities.h"
#include <algorithm>
#include <cmath>


namespace lsn {

	CNtscBisqwitFilter::CNtscBisqwitFilter() :
#ifdef LSN_BISQWIT_PERF
		m_pMonitor( "CNtscBisqwitFilter" ),
#endif	// #ifdef LSN_BISQWIT_PERF
		m_ui32FinalStride( 0 ),
		m_ui32FinalWidth( 0 ),
		m_ui32FinalHeight( 0 ),
		m_i32WidthY( 12 ),
		m_i32WidthI( 22 ),
		m_i32WidthQ( 26 ),
		m_fBrightness( 0.0f ),
		m_fContrast( -0.125f ),
		m_fSaturation( -0.125f ),
		m_fHue( 0.0f ),
		m_bRunThreads( true ) {

		const int8_t i8SignalLumaLow[4] = { -29, -15, 22, 71 };
		const int8_t i8SignalLumaHigh[4] = { 32, 66, 105, 105 };

		for ( int32_t I = 0; I < 0x40; I++ ) {
			int32_t i32R = (I & 0x0F) >= 0x0E ? 0x1D : I;

			int32_t i32M = i8SignalLumaLow[i32R / 0x10];
			int32_t i32Q = i8SignalLumaHigh[i32R / 0x10];
			if ( (i32R & 0x0F) == 0x0D ) {
				i32Q = i32M;
			}
			else if ( (i32R & 0x0F) == 0x00 ) { 
				i32M = i32Q;
			}
			m_i8SigLow[I] = int8_t( i32M );
			m_i8SigHi[I] = int8_t( i32Q );
		}

		m_tThreadData.pblppThis = this;
		m_ptThread = std::make_unique<std::thread>( Thread, &m_tThreadData );
	}
	CNtscBisqwitFilter::~CNtscBisqwitFilter() {
		StopThread();
	}

	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CNtscBisqwitFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
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
		m_ui32FinalWidth = _ui16Width * 8;
		m_ui32FinalHeight = _ui16Height;
		m_ui32FinalStride = RowStride( m_ui32FinalWidth, OutputBits() );
		m_vFilteredOutput.resize( m_ui32FinalStride * m_ui32FinalHeight );


		int32_t i32Contrast = int32_t( (m_fContrast + 1.0) * (m_fContrast + 1.0) * 167941 );
		int32_t i32Saturation = int32_t( (m_fSaturation + 1.0) * (m_fSaturation + 1.0) * 144044 );
		for ( int32_t I = 0; I < 27; I++ ) {
			m_i8SinTable[I] = int8_t( std::round( 8 * std::sin( I * 2.0 * LSN_PI / 12.0 + m_fHue * LSN_PI ) ) );
		}

		m_i32WidthY = 12;
#if 0
		m_i32WidthI = 22;
		m_i32WidthQ = 26;
#elif 1
		m_i32WidthI = 24;
		m_i32WidthQ = 24;
#elif 1
		m_i32WidthI = 36;
		m_i32WidthQ = 36;
#endif

		m_i32Y = i32Contrast / m_i32WidthY;

		m_i32Ir = int32_t( i32Contrast * 1.994681e-6 * i32Saturation / m_i32WidthI );
		m_i32Qr = int32_t( i32Contrast * 9.915742e-7 * i32Saturation / m_i32WidthQ );

		m_i32Ig = int32_t( i32Contrast * 9.151351e-8 * i32Saturation / m_i32WidthI );
		m_i32Qg = int32_t( i32Contrast * -6.334805e-7 * i32Saturation / m_i32WidthQ );

		m_i32Ib = int32_t( i32Contrast * -1.012984e-6 * i32Saturation / m_i32WidthI );
		m_i32Qb = int32_t( i32Contrast * 1.667217e-6 * i32Saturation / m_i32WidthQ );

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
	uint8_t * CNtscBisqwitFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle ) {
#ifdef LSN_BISQWIT_PERF
		m_pMonitor.Begin();
#endif	// #ifdef LSN_BISQWIT_PERF


		m_tThreadData.ui32Stride = _ui32Stride;
		m_tThreadData.ui32ScreenWidth = _ui32Width;
		m_tThreadData.ui32ScreenHeight = _ui32Height;
		m_tThreadData.ui64RenderCycle = _ui64RenderStartCycle + 341 * (_ui32Height >> 1);
		m_tThreadData.pui8Input = _pui8Input;
		if ( m_ptThread.get() ) {
			m_eGo.Signal();
		}

		DoFrame( _ui64RenderStartCycle, _pui8Input, 0, _ui32Height >> 1, _ui32Width );

		if ( m_ptThread.get() ) {
			m_eDone.WaitForSignal();
		}

		//DoFrame( _ui64RenderStartCycle + 341 * (_ui32Height >> 1), _pui8Input, _ui32Height >> 1, _ui32Height, _ui32Width );

		/*uint32_t constexpr ui32ResDiv = 1;
		int32_t i32PixelsPerCycle = 8 / ui32ResDiv;
		int32_t i32Phase = int32_t( _ui64RenderStartCycle * 8 % 12 );
		constexpr int32_t i32LineW = 256;
		int8_t i8RowSig[i32LineW*m_i32SignalsPerPixel];
		uint32_t ui32RowPixelGap = _ui32Width * i32PixelsPerCycle;
		uint32_t * pui32Out = reinterpret_cast<uint32_t *>(m_vFilteredOutput.data());
		for ( uint32_t Y = 0; Y < _ui32Height; Y++ ) {
			int32_t i32StartCycle = i32Phase % 12;
		
			GenerateNtscSignal( i8RowSig, i32Phase, Y, reinterpret_cast<const uint16_t *>(_pui8Input) );

			NtscDecodeLine( i32LineW * m_i32SignalsPerPixel, i8RowSig, pui32Out, (i32StartCycle + 7) % 12 );

			pui32Out += ui32RowPixelGap;
		}*/
		_ui32Width = m_ui32FinalWidth;
		_ui32Height = m_ui32FinalHeight;
		_ui32Stride = m_ui32FinalStride;

#ifdef LSN_BISQWIT_PERF
		m_pMonitor.Stop();
#endif	// #ifdef LSN_BISQWIT_PERF
		return m_vFilteredOutput.data();
	}

	/**
	 * Converts one row of the 16-bit PPU output to an NTSC signal.
	 *
	 * \param _pi8Signal The output signal generated.
	 * \param _i32Phase The phase offset of the signal.
	 * \param _i32Row The row to generate.
	 * \param _pui16Input The input palette values.
	 */
	void CNtscBisqwitFilter::GenerateNtscSignal( int8_t * _pi8Signal, int32_t &_i32Phase, int32_t _i32Row, const uint16_t * _pui16Input ) {
		for ( int32_t X = 0; X < 256; X++ ) {
			uint16_t ui16Color = _pui16Input[(_i32Row<<8)|X];

			int8_t i8Low = m_i8SigLow[ui16Color&0x3F];
			int8_t i8High = m_i8SigHi[ui16Color&0x3F];
			int8_t i8Emphasis = int8_t( ui16Color >> 6 );

			uint16_t ui16PhaseBitMask = m_ui16BitMaskTable[std::abs(_i32Phase-(ui16Color&0x0F))%12];

			uint8_t ui8Voltage;
			for ( int32_t J = 0; J < 8; J++ ) {
				ui16PhaseBitMask <<= 1;
				ui8Voltage = i8High;
				if ( ui16PhaseBitMask >= 0x40 ) {
					if ( ui16PhaseBitMask == 0x1000 ) {
						ui16PhaseBitMask = 1;
					}
					else {
						ui8Voltage = i8Low;
					}
				}

				if ( ui16PhaseBitMask & i8Emphasis ) {
					ui8Voltage -= ui8Voltage >> 2;
				}

				_pi8Signal[((X)<<3)|J] = ui8Voltage;
			}

			_i32Phase += m_i32SignalsPerPixel;
		}
		_i32Phase += (341 - 256) * m_i32SignalsPerPixel;
	}

	/**
	 * NTSC_DecodeLine(Width, Signal, Target, Phase0)
	 *
	 * Convert NES NTSC graphics signal into RGB using integer arithmetics only.
	 *
	 * Width: Number of NTSC signal samples.
	 *        For a 256 pixels wide screen, this would be 256*8. 283*8 if you include borders.
	 *
	 * Signal: An array of Width samples.
	 *         The following sample values are recognized:
	 *          -29 = Luma 0 low   32 = Luma 0 high (-38 and  6 when attenuated)
	 *          -15 = Luma 1 low   66 = Luma 1 high (-28 and 31 when attenuated)
	 *           22 = Luma 2 low  105 = Luma 2 high ( -1 and 58 when attenuated)
	 *           71 = Luma 3 low  105 = Luma 3 high ( 34 and 58 when attenuated)
	 *         In this scale, sync signal would be -59 and colorburst would be -40 and 19,
	 *         but these are not interpreted specially in this function.
	 *         The value is calculated from the relative voltage with:
	 *                   floor((voltage-0.518)*1000/12)-15
	 *
	 * Target: Pointer to a storage for Width RGB32 samples (00rrggbb).
	 *         Note that the function will produce a RGB32 value for _every_ half-clock-cycle.
	 *         This means 2264 RGB samples if you render 283 pixels per scanline (incl. borders).
	 *         The caller can pick and choose those columns they want from the signal
	 *         to render the picture at their desired resolution.
	 *
	 * Phase0: An integer in range 0-11 that describes the phase offset into colors on this scanline.
	 *         Would be generated from the PPU clock cycle counter at the start of the scanline.
	 *         In essence it conveys in one integer the same information that real NTSC signal
	 *         would convey in the colorburst period in the beginning of each scanline.
	 */
	void CNtscBisqwitFilter::NtscDecodeLine( int32_t _i32Width, const int8_t * _pi8Signal, uint32_t * _pui32Output, int32_t _i32Phase0 ) {
		uint32_t constexpr ui32ResDiv = 1;
		uint32_t constexpr ui32OverscanLeft = 0;
		uint32_t constexpr ui32OverscanRight = 0;
		auto Read = [=]( int32_t _i32Pos ) -> char { return _i32Pos >= 0 ? _pi8Signal[_i32Pos] : 0; };
		auto Cos = [=]( int32_t _i32Pos ) -> char { return m_i8SinTable[(_i32Pos+36)%12+_i32Phase0]; };
		auto Sin = [=]( int32_t _i32Pos ) -> char { return m_i8SinTable[(_i32Pos+36)%12+3+_i32Phase0]; };

		int32_t i32Brightness = int32_t( m_fBrightness * 750 );
		int32_t i32SumY = i32Brightness, i32SumI = 0, i32SumQ = 0;
		int32_t i32Offset = ui32ResDiv + 4;
		int32_t i32LeftOverscan = ui32OverscanLeft * 8 + i32Offset;
		int32_t i32RightOverscan = _i32Width - ui32OverscanRight * 8 + i32Offset;
#ifdef LSN_SRGB_BISQWIT
		auto LinearTosRGB = [&]( double _dVal ) {
			return _dVal <= 0.0031308 ?
				_dVal * 12.92 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		};

		int32_t i32LastR = 0, i32LastG = 0, i32LastB = 0;
		int32_t i32LastRs = 0, i32LastGs = 0, i32LastBs = 0;
#endif	// LSN_SRGB_BISQWIT
		for ( int32_t I = 0; I < i32RightOverscan; I++ ) {
			int32_t i32ReadI = Read( I );
			i32SumY += i32ReadI - Read( I - m_i32WidthY );
			i32SumI += i32ReadI * Cos( I ) - Read( I - m_i32WidthI ) * Cos( I - m_i32WidthI );
			i32SumQ += i32ReadI * Sin( I ) - Read( I - m_i32WidthQ ) * Sin( I - m_i32WidthQ );

			if ( !(I % ui32ResDiv) && I >= i32LeftOverscan ) {
				int32_t i32R = std::clamp( (i32SumY * m_i32Y + i32SumI * m_i32Ir + i32SumQ * m_i32Qr) / 65536, 0, 255 );
				int32_t i32G = std::clamp( (i32SumY * m_i32Y + i32SumI * m_i32Ig + i32SumQ * m_i32Qg) / 65536, 0, 255 );
				int32_t i32B = std::clamp( (i32SumY * m_i32Y + i32SumI * m_i32Ib + i32SumQ * m_i32Qb) / 65536, 0, 255 );
#ifdef LSN_SRGB_BISQWIT
				if ( i32R != i32LastR ) {
					i32LastR = i32R;
					double dFract = LinearTosRGB( i32R / 255.0 );
					i32R = i32LastRs = uint8_t( std::round( dFract * 255.0 ) );
				}
				else {
					i32R = i32LastRs;
				}

				if ( i32G != i32LastR ) {
					i32LastR = i32G;
					double dFract = LinearTosRGB( i32G / 255.0 );
					i32G = i32LastRs = uint8_t( std::round( dFract * 255.0 ) );
				}
				else {
					i32G = i32LastRs;
				}

				if ( i32B != i32LastR ) {
					i32LastR = i32B;
					double dFract = LinearTosRGB( i32B / 255.0 );
					i32B = i32LastRs = uint8_t( std::round( dFract * 255.0 ) );
				}
				else {
					i32B = i32LastRs;
				}
#endif	// LSN_SRGB_BISQWIT


				(*_pui32Output) = 0xFF000000 | (i32R << 16) | (i32G << 8) | i32B;
				_pui32Output++;
			}
		}
	}

	void CNtscBisqwitFilter::DoFrame( uint64_t _ui64RenderCycle, uint8_t * _pui8Input, uint32_t _ui32From, uint32_t _ui32To, uint32_t _ui32Width ) {
		uint32_t constexpr ui32ResDiv = 1;
		int32_t i32PixelsPerCycle = 8 / ui32ResDiv;
		int32_t i32Phase = int32_t( _ui64RenderCycle * m_i32SignalsPerPixel % 12 );
		constexpr int32_t i32LineW = 256;
		int8_t i8RowSig[i32LineW*m_i32SignalsPerPixel];
		uint32_t ui32RowPixelGap = _ui32Width * i32PixelsPerCycle;
		uint32_t * pui32Out = reinterpret_cast<uint32_t *>(m_vFilteredOutput.data() + _ui32From * m_ui32FinalStride);
		for ( uint32_t Y = _ui32From; Y < _ui32To; Y++ ) {
			int32_t i32StartCycle = i32Phase % 12;
		
			GenerateNtscSignal( i8RowSig, i32Phase, Y, reinterpret_cast<const uint16_t *>(_pui8Input) );
			NtscDecodeLine( i32LineW * m_i32SignalsPerPixel, i8RowSig, pui32Out, (i32StartCycle + 7) % 12 );

			pui32Out += ui32RowPixelGap;
		}
	}

	/**
	 * Stops the thread.
	 */
	void CNtscBisqwitFilter::StopThread() {
		m_bRunThreads = false;
		if ( m_ptThread.get() ) {
			m_eGo.Signal();
			m_eDone.WaitForSignal();
			m_ptThread->join();
			m_ptThread.reset();
		}
		m_bRunThreads = true;
	}

	/**
	 * The thread.
	 *
	 * \param _pblppFilter Pointer to this object.
	 */
	void CNtscBisqwitFilter::Thread( LSN_THREAD * _ptThread ) {
		while ( _ptThread->pblppThis->m_bRunThreads ) {
			_ptThread->pblppThis->m_eGo.WaitForSignal();
			if ( _ptThread->pblppThis->m_bRunThreads ) {
				_ptThread->pblppThis->DoFrame( _ptThread->ui64RenderCycle + 341 * (_ptThread->ui32ScreenHeight >> 1), _ptThread->pui8Input, _ptThread->ui32ScreenHeight >> 1, _ptThread->ui32ScreenHeight, _ptThread->ui32ScreenWidth );
			}
			_ptThread->pblppThis->m_eDone.Signal();
		}
	}

}	// namespace lsn
