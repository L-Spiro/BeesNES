/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../Event/LSNEvent.h"
#include "../Utilities/LSNAlignmentAllocator.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNFilterBase.h"

#if defined( __arm__ ) || defined( __aarch64__ )
#include <arm_neon.h>
#endif

#include <thread>

#pragma warning( push )
#pragma warning( disable : 4324 )	// warning C4324: 'lsn::CNtscLSpiroFilter': structure was padded due to alignment specifier

namespace lsn {

	/**
	 * Class CNtscLSpiroFilter
	 * \brief My own implementation of an NTSC filter.
	 *
	 * Description: My own implementation of an NTSC filter.
	 */
	class CNtscLSpiroFilter : public CFilterBase {
	public :
		CNtscLSpiroFilter();
		virtual ~CNtscLSpiroFilter();


		// == Types.
		/**
		 * A filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		typedef float (*									PfFilterFunc)( float _fT, float _fWidth );


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height );

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
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &_ui16BitDepth, uint32_t &_ui32Stride, uint64_t _ui64PpuFrame, uint64_t _ui64RenderStartCycle );

		/**
		 * Gets the PPU output format.
		 *
		 * \return Returns the output format from the PPU/input format for this filter.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const { return CDisplayClient::LSN_POF_9BIT_PALETTE; }

		/**
		 * If true, the PPU is requested to provide a frame that has been flipped vertically.
		 *
		 * \return Returns true to receive a vertically flipped image from the PPU, false to receive an unflipped image.
		 */
		virtual bool										FlipInput() const { return false; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() { return CurTarget(); }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const { return 32; }

		/**
		 * Sets the filter kernel size.
		 * 
		 * \param _ui32Size The new size of the filter.
		 * \return Returns true if the memory for the internal buffer(s) was allocated.
		 **/
		bool												SetKernelSize( uint32_t _ui32Size );

		/**
		 * Sets the width of the input.
		 * 
		 * \param _ui16Width The width to set.
		 * \return Returns true if the memory for the internal buffer(s) was allocated.
		 **/
		bool												SetWidth( uint16_t _ui16Width );

		/**
		 * Sets the width scale.
		 * 
		 * \param _ui16WidthScale The width scale to set.
		 * \return Returns true if the memory for the internal buffer(s) was allocated.
		 **/
		bool												SetWidthScale( uint16_t _ui16WidthScale );

		/**
		 * Sets the height of the input.
		 * 
		 * \param _ui16Height The height to set.
		 * \return Returns true if the memory for the internal buffer(s) was allocated.
		 **/
		bool												SetHeight( uint16_t _ui16Height );

		/**
		 * Sets the CRT gamma.
		 * 
		 * \param _fGamma The gamma to set.
		 **/
		void												SetGamma( float _fGamma );

		/**
		 * Sets the hue.
		 * 
		 * \param _fHue The hue to set.
		 **/
		void												SetHue( float _fHue );

		/**
		 * Sets the brightness.
		 * 
		 * \param _fBrightness The brightness to set.
		 **/
		void												SetBrightness( float _fBrightness );

		/**
		 * Sets the saturation.
		 * 
		 * \param _fSat The saturation to set.
		 **/
		void												SetSaturation( float _fSat );

		/**
		 * Sets the black level.
		 * 
		 * \param _fBlack The black level to set.
		 **/
		void												SetBlackLevel( float _fBlack );

		/**
		 * Sets the white level.
		 * 
		 * \param _fWhite The white level to set.
		 **/
		void												SetWhiteLevel( float _fWhite );

		/**
		 * Sets the filter function.
		 * 
		 * \param _pfFunc The filter function.
		 **/
		void												SetFilterFunc( PfFilterFunc _pfFunc ) {
			m_pfFilterFunc = _pfFunc;
			GenFilterKernel( m_ui32FilterKernelSize );
		}

		/**
		 * Sets the Y filter function.
		 * 
		 * \param _pfFunc The filter function.
		 **/
		void												SetFilterFuncY( PfFilterFunc _pfFunc ) {
			m_pfFilterFuncY = _pfFunc;
			GenFilterKernel( m_ui32FilterKernelSize );
		}


	protected :
		// == Enumerations.
		/** Metrics. */
		enum {
			LSN_MAX_FILTER_SIZE								= 128,									/**< The maximum size of the gather for generating YIQ values. */
		};


		// == Types.
		/** A 4-element float vector for mega-SIMD. */
#ifdef __SSE4_1__
		typedef __m128										simd_4;
#elif defined( __arm__ ) || defined( __aarch64__ )
		typedef float32x4_t									simd_4;
#endif	// #ifdef __SSE4_1__
		
		/** Thread data. */
		struct LSN_THREAD_DATA {
			uint64_t										ui64RenderStartCycle;					/**< The render cycle at the start of the frame. */
			CNtscLSpiroFilter *								pnlsfThis;								/**< Point to this object. */
			const uint8_t *									pui8Pixels;								/**< The input 9-bit pixel array. */
			uint16_t										ui16LinesDone;							/**< How many lines the thread has done. */
			uint16_t										ui16EndLine;							/**< Line on which to end work. */
			std::atomic<bool>								bEndThread;								/**< If true, the thread is ended. */
		};


		// == Members.
		uint16_t											m_ui16Width = 0;						/**< The last input width. */
		uint16_t											m_ui16Height = 0;						/**< The last input height. */
		uint32_t											m_ui32FinalStride = 0;					/**< The final stride. */

#ifdef __AVX__
		__m256												m_mStackedFilterTable[LSN_MAX_FILTER_SIZE];		/**< A stack of filter kernels such that each filter index aligns to 32 bytes. */
		__m256												m_mStackedFilterTableY[LSN_MAX_FILTER_SIZE];	/**< A stack of filter kernels such that each filter index aligns to 32 bytes. */
		__m256												m_mStackedCosTable[12];					/**< 8 elements of the cosine table stacked. */
		__m256												m_mStackedSinTable[12];					/**< 8 elements of the sine table stacked. */
#endif	// #ifdef __AVX__
#ifdef __AVX512F__
		__m512												m_mStackedFilterTable512[LSN_MAX_FILTER_SIZE];	/**< A stack of filter kernels such that each filter index aligns to 64 bytes. */
		__m512												m_mStackedFilterTable512Y[LSN_MAX_FILTER_SIZE];	/**< A stack of filter kernels such that each filter index aligns to 64 bytes. */
		__m512												m_mStackedCosTable512[12];				/**< 16 elements of the cosine table stacked. */
		__m512												m_mStackedSinTable512[12];				/**< 16 elements of the sine table stacked. */
#endif	// #ifdef __AVX__

		LSN_ALIGN( 32 )
		float												m_fFilter[LSN_MAX_FILTER_SIZE];			/**< The filter kernel. */
		float												m_fFilterY[LSN_MAX_FILTER_SIZE];		/**< The filter kernel. */
		
		float												m_fPhaseCosTable[12];					/**< The cosine phase table. */
		float												m_fPhaseSinTable[12];					/**< The sine phase table. */
		
		PfFilterFunc										m_pfFilterFunc = CUtilities::BoxFilterFunc;		/**< The filter function for chroma. */
		PfFilterFunc										m_pfFilterFuncY = CUtilities::BoxFilterFunc;	/**< The filter function for Y. */
		uint32_t											m_ui32FilterKernelSize = 12;			/**< The kernel size for the gather during YIQ creation. */
		std::vector<float, CAlignmentAllocator<float>>		m_vSignalBuffer;						/**< The intermediate signal buffer for a single scanline. */
		std::vector<float *>								m_vSignalStart;							/**< Points into m_vSignalBuffer.data() at the first location that is both >= to (LSN_MAX_FILTER_SIZE/2) floats and aligned to a 64-byte address. */
		std::vector<simd_4>									m_vY;									/**< The YIQ Y buffer. */
		std::vector<simd_4>									m_vI;									/**< The YIQ I buffer. */
		std::vector<simd_4>									m_vQ;									/**< The YIQ Q buffer. */
		std::vector<float, CAlignmentAllocator<float>>		m_vBlendBuffer;							/**< The blend buffer. */
		std::vector<uint8_t>								m_vRgbBuffer;							/**< The output created by calling FilterFrame(). */
		uint16_t											m_ui16ScaledWidth = 0;					/**< Output width. */
		uint16_t											m_ui16WidthScale = 8;					/**< Scale factor between input and output width. */

		CEvent												m_eGo;									/**< Signal to tell the thread to go. */
		CEvent												m_eDone;								/**< Thread to tell the main thread that the 2nd thread is done. */
		std::unique_ptr<std::thread>						m_ptThread;								/**< The 2nd thread. */
		LSN_THREAD_DATA										m_tdThreadData;							/**< Thread data. */

		uint32_t											m_ui32Gamma[300];						/**< The gamma curve. */
		uint32_t											m_ui32GammaG[300];						/**< The gamma curve for green. */
		uint8_t												m_ui8Gamma[300];						/**< The gamma curve. */
		uint8_t												m_ui8GammaG[300];						/**< The gamma curve for green. */
		float												m_NormalizedLevels[16];					/**< Normalized levels. */

		// ** SETTINGS ** //
		float												m_fHueSetting = 0.0f;					/**< The hue. */
		float												m_fGammaSetting = 2.2f;					/**< The CRT gamma curve. */
		float												m_fBrightnessSetting = 1.0f;			/**< The brightness setting. */
		float												m_fSaturationSetting = 1.0f;			/**< The saturation setting. */
		float												m_fBlackSetting = 0.312f;				/**< Black level. */
		float												m_fWhiteSetting = 1.100f;				/**< White level. */
		float												m_fPhosphorDecayRate = 0.56f;			/**< Phosphor decay rate. */
		//float												m_fInitPhosphorDecay = 0.25f;			/**< Initial phosphor decay. */

		
		


		// == Functions.
		/**
		 * Converts a 9-bit PPU output index to an NTSC signal.
		 * 
		 * \param _ui16Pixel The PPU output index to convert.
		 * \param _ui16Phase The phase counter.
		 * \return Returns the signal produced by the PPU output index.
		 **/
		inline float										IndexToNtscSignal( uint16_t _ui16Pixel, uint16_t _ui16Phase );

		/**
		 * Converts a 9-bit PPU output palette value to 8 signals.
		 * 
		 * \param _pfDst The destination for the 8 signals.
		 * \param _ui16Pixel The PPU output index to convert.
		 * \param _ui16Cycle The cycle count for the pixel (modulo 12).
		 **/
		inline void											PixelToNtscSignals( float * _pfDst, uint16_t _ui16Pixel, uint16_t _ui16Cycle );

		/**
		 * Creates a single scanline from 9-bit PPU output to a float buffer of YIQ values.
		 * 
		 * \param _pfDstY The destination for where to begin storing the YIQ Y values.  Must be aligned to a 16-byte boundary.
		 * \param _pfDstI The destination for where to begin storing the YIQ I values.  Must be aligned to a 16-byte boundary.
		 * \param _pfDstQ The destination for where to begin storing the YIQ Q values.  Must be aligned to a 16-byte boundary.
		 * \param _pui16Pixels The start of the 9-bit PPU output for this scanline.
		 * \param _ui16Cycle The cycle count at the start of the scanline.
		 * \param _sRowIdx The scanline index.
		 **/
		void												ScanlineToYiq( float * _pfDstY, float * _pfDstI, float * _pfDstQ, const uint16_t * _pui16Pixels, uint16_t _ui16Cycle, size_t _sRowIdx );

		/**
		 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
		 * 
		 * \param _pui8Pixels The input array of 9-bit PPU outputs.
		 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
		 **/
		void												FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle );

		/**
		 * Renders a range of scanlines.
		 * 
		 * \param _pui8Pixels The input array of 9-bit PPU outputs.
		 * \param _ui16Start Index of the first scanline to render.
		 * \param _ui16End INdex of the end scanline.
		 * \param _ui64RenderStartCycle The PPU cycle at the start of the frame being rendered.
		 **/
		void												RenderScanlineRange( const uint8_t * _pui8Pixels, uint16_t _ui16Start, uint16_t _ui16End, uint64_t _ui64RenderStartCycle );

		/**
		 * Generates the phase sin/cos tables.
		 * 
		 * \param _fHue The hue offset.
		 **/
		void												GenPhaseTables( float _fHue );

		/**
		 * Fills the __m128 registers with the black level and (white-black) level.
		 **/
		void												GenNormalizedSignals();

		/**
		 * Generates the filter kernel.
		 * 
		 * \param _ui32Width The width of the kernel.
		 **/
		void												GenFilterKernel( uint32_t _ui32Width );

		/**
		 * Allocates the YIQ buffers for a given width and height.
		 * 
		 * \param _ui16W The width of the buffers.
		 * \param _ui16H The height of the buffers.
		 * \param _ui16Scale The width scale factor.
		 * \return Returns true if the allocations succeeded.
		 **/
		bool												AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H, uint16_t _ui16Scale );

#ifdef __AVX512F__
		/**
		 * Performs convolution on 16 values at a time.
		 * 
		 * \param _pfSignals The source signals to convolve.
		 * \param _sFilterIdx The filter table index.
		 * \param _sCosSinIdx The cosine/sine table index.
		 * \param _mCos The summed result of cosine convolution.
		 * \param _mSin The summed result of sine convolution.
		 * \param _mSignal The summed result of signal convolution.
		 **/
		inline void											Convolution16( float * _pfSignals, size_t _sFilterIdx, size_t _sCosSinIdx, __m512 &_mCos, __m512 &_mSin, __m512 &_mSignal );
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		/**
		 * Performs convolution on 8 values at a time.
		 * 
		 * \param _pfSignals The source signals to convolve.
		 * \param _sFilterIdx The filter table index.
		 * \param _sCosSinIdx The cosine/sine table index.
		 * \param _mCos The summed result of cosine convolution.
		 * \param _mSin The summed result of sine convolution.
		 * \param _mSignal The summed result of signal convolution.
		 **/
		inline void											Convolution8( float * _pfSignals, size_t _sFilterIdx, size_t _sCosSinIdx, __m256 &_mCos, __m256 &_mSin, __m256 &_mSignal );
#endif	// #ifdef __AVX__
		
#ifdef __SSE4_1__
		/**
		 * Performs convolution on 4 values at a time.
		 * 
		 * \param _pfSignals The source signals to convolve.
		 * \param _sFilterIdx The filter table index.
		 * \param _sCosSinIdx The cosine/sine table index.
		 * \param _mCos The summed result of cosine convolution.
		 * \param _mSin The summed result of sine convolution.
		 * \param _mSignal The summed result of signal convolution.
		 **/
		inline void											Convolution4( float * _pfSignals, size_t _sFilterIdx, size_t _sCosSinIdx, __m128 &_mCos, __m128 &_mSin, __m128 &_mSignal );
#endif	// #ifdef __SSE4_1__

		/**
		 * Converts a single scanline of YIQ values in m_vY/m_vI/m_vQ to BGRA values in the same scanline of m_vRgbBuffer.
		 * 
		 * \param _sScanline The scanline to convert
		 **/
		void												ConvertYiqToBgra( size_t _sScanline );

		/**
		 * Stops the worker thread.
		 **/
		void												StopThread();

		/**
		 * The worker thread.
		 * 
		 * \param _ptdData Parameters passed to the thread.
		 **/
		static void											WorkThread( LSN_THREAD_DATA * _ptdData );
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Converts a 9-bit PPU output index to an NTSC signal.
	 * 
	 * \param _ui16Pixel The PPU output index to convert.
	 * \param _ui16Phase The phase counter.
	 * \return Returns the signal produced by the PPU output index.
	 **/
	inline float CNtscLSpiroFilter::IndexToNtscSignal( uint16_t _ui16Pixel, uint16_t _ui16Phase ) {
		// Decode the NES color.
		uint16_t ui16Color = (_ui16Pixel & 0x0F);								// 0..15 "cccc".
		uint16_t ui16Level = (ui16Color >= 0xE) ? 1 : (_ui16Pixel >> 4) & 3;	// 0..3  "ll".  For colors 14..15, level 1 is forced.
		uint16_t ui16Emphasis = (_ui16Pixel >> 6);								// 0..7  "eee".

#define LSN_INCOLORPHASE( COLOR )					(((COLOR) + _ui16Phase) % 12 < 6)
		// When de-emphasis bits are set, some parts of the signal are attenuated:
		// Colors [14..15] are not affected by de-emphasis.
		uint16_t ui16Atten = ((ui16Color < 0xE) &&
			(((ui16Emphasis & 1) && LSN_INCOLORPHASE( 0xC )) ||
			((ui16Emphasis & 2) && LSN_INCOLORPHASE( 0x4 )) ||
			((ui16Emphasis & 4) && LSN_INCOLORPHASE( 0x8 )))) ? 8 : 0;

		// The square wave for this color alternates between these two voltages:
		float fLow  = m_NormalizedLevels[ui16Level+ui16Atten];
		float fHigh = (&m_NormalizedLevels[4])[ui16Level+ui16Atten];
		if LSN_UNLIKELY( ui16Color == 0 ) { return fHigh; }						// For color 0, only high level is emitted.
		if LSN_UNLIKELY( ui16Color > 12 ) { return fLow; }						// For colors 13..15, only low level is emitted.

		return LSN_INCOLORPHASE( ui16Color ) ? fHigh : fLow;
#undef LSN_INCOLORPHASE
	}

	/**
	 * Converts a 9-bit PPU output palette value to 8 signals.
	 * 
	 * \param _pfDst The destination for the 8 signals.
	 * \param _ui16Pixel The PPU output index to convert.
	 * \param _ui16Cycle The cycle count for the pixel (modulo 12).
	 **/
	inline void CNtscLSpiroFilter::PixelToNtscSignals( float * _pfDst, uint16_t _ui16Pixel, uint16_t _ui16Cycle ) {
		for ( size_t I = 0; I < 8; ++I ) {
			(*_pfDst++) = IndexToNtscSignal( _ui16Pixel, uint16_t( _ui16Cycle + I ) );
		}
	}

#ifdef __AVX512F__
	/**
	 * Performs convolution on 16 values at a time.
	 * 
	 * \param _pfSignals The source signals to convolve.
	 * \param _sFilterIdx The filter table index.
	 * \param _sCosSinIdx The cosine/sine table index.
	 * \param _mCos The summed result of cosine convolution.
	 * \param _mSin The summed result of sine convolution.
	 * \param _mSignal The summed result of signal convolution.
	 **/
	inline void CNtscLSpiroFilter::Convolution16( float * _pfSignals, size_t _sFilterIdx, size_t _sCosSinIdx, __m512 &_mCos, __m512 &_mSin, __m512 &_mSignal ) {
		// Load the signals.
		__m512 mSignals = _mm512_loadu_ps( _pfSignals );
		// Load the filter weights.
		__m512 mFilter = _mm512_load_ps( reinterpret_cast<float *>(&m_mStackedFilterTable512[_sFilterIdx] ) );
		__m512 mFilterY = _mm512_load_ps( reinterpret_cast<float *>(&m_mStackedFilterTable512Y[_sFilterIdx] ) );
		// Load the cosine values.
		__m512 mCos = _mm512_load_ps( reinterpret_cast<float *>(&m_mStackedCosTable512[_sCosSinIdx] ) );

		// Multiply Signals and weights.
		__m512 mLevels = _mm512_mul_ps( mSignals, mFilter );
		__m512 mLevelsY = _mm512_mul_ps( mSignals, mFilterY );
		// Load the sine values.
		__m512 mSin = _mm512_load_ps( reinterpret_cast<float *>(&m_mStackedSinTable512[_sCosSinIdx] ) );

		// Multiply levels and cosines.
		//_mCos = _mm512_add_ps( _mCos, _mm512_mul_ps( mLevels, mCos ) );
		_mCos = _mm512_fmadd_ps( mLevels, mCos, _mCos );
		// Multiply levels and sines.
		//_mSin = _mm512_add_ps( _mSin, _mm512_mul_ps( mLevels, mSin ) );
		_mSin = _mm512_fmadd_ps( mLevels, mSin, _mSin );
		// Accumulate the signals.
		_mSignal = _mm512_add_ps( _mSignal, mLevelsY );
	}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
	/**
	 * Performs convolution on 8 values at a time.
	 * 
	 * \param _pfSignals The source signals to convolve.
	 * \param _sFilterIdx The filter table index.
	 * \param _sCosSinIdx The cosine/sine table index.
	 * \param _mCos The summed result of cosine convolution.
	 * \param _mSin The summed result of sine convolution.
	 * \param _mSignal The summed result of signal convolution.
	 **/
	inline void CNtscLSpiroFilter::Convolution8( float * _pfSignals, size_t _sFilterIdx, size_t _sCosSinIdx, __m256 &_mCos, __m256 &_mSin, __m256 &_mSignal ) {
		// Load the signals.
		__m256 mSignals = _mm256_loadu_ps( _pfSignals );
		// Load the filter weights.
		__m256 mFilter = _mm256_load_ps( reinterpret_cast<float *>(&m_mStackedFilterTable[_sFilterIdx] ) );
		__m256 mFilterY = _mm256_load_ps( reinterpret_cast<float *>(&m_mStackedFilterTableY[_sFilterIdx] ) );
		// Load the cosine values.
		__m256 mCos = _mm256_load_ps( reinterpret_cast<float *>(&m_mStackedCosTable[_sCosSinIdx] ) );

		// Multiply Signals and weights.
		__m256 mLevels = _mm256_mul_ps( mSignals, mFilter );
		__m256 mLevelsY = _mm256_mul_ps( mSignals, mFilterY );
		// Load the sine values.
		__m256 mSin = _mm256_load_ps( reinterpret_cast<float *>(&m_mStackedSinTable[_sCosSinIdx] ) );

		// Multiply levels and cosines.
		//_mCos = _mm256_add_ps( _mCos, _mm256_mul_ps( mLevels, mCos ) );
		_mCos = _mm256_fmadd_ps( mLevels, mCos, _mCos );
		// Multiply levels and sines.
		//_mSin = _mm256_add_ps( _mSin, _mm256_mul_ps( mLevels, mSin ) );
		_mSin = _mm256_fmadd_ps( mLevels, mSin, _mSin );
		// Accumulate the signals.
		_mSignal = _mm256_add_ps( _mSignal, mLevelsY );
	}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
	/**
	 * Performs convolution on 4 values at a time.
	 * 
	 * \param _pfSignals The source signals to convolve.
	 * \param _sFilterIdx The filter table index.
	 * \param _sCosSinIdx The cosine/sine table index.
	 * \param _fCos The summed result of cosine convolution.
	 * \param _fSin The summed result of sine convolution.
	 * \return Returns the sum of the signal convolution.
	 **/
	inline void CNtscLSpiroFilter::Convolution4( float * _pfSignals, size_t _sFilterIdx, size_t _sCosSinIdx, __m128 &_mCos, __m128 &_mSin, __m128 &_mSignal ) {
		// Load the signals.
		__m128 mSignals = _mm_loadu_ps( _pfSignals );
		// Load the filter weights.
		__m128 mFilter = _mm_load_ps( reinterpret_cast<float *>(&m_mStackedFilterTable[_sFilterIdx] ) );
		__m128 mFilterY = _mm_load_ps( reinterpret_cast<float *>(&m_mStackedFilterTableY[_sFilterIdx] ) );
		// Load the cosine values.
		__m128 mCos = _mm_load_ps( reinterpret_cast<float *>(&m_mStackedCosTable[_sCosSinIdx] ) );

		// Multiply Signals and weights.
		__m128 mLevels = _mm_mul_ps( mSignals, mFilter );
		__m128 mLevelsY = _mm_mul_ps( mSignals, mFilterY );
		// Load the sine values.
		__m128 mSin = _mm_load_ps( reinterpret_cast<float *>(&m_mStackedSinTable[_sCosSinIdx] ) );

		// Multiply levels and cosines.
		_mCos = _mm_add_ps( _mCos, _mm_mul_ps( mLevels, mCos ) );
		// Multiply levels and sines.
		_mSin = _mm_add_ps( _mSin, _mm_mul_ps( mLevels, mSin ) );
		// Accumulate the signals.
		_mSignal = _mm_add_ps( _mSignal, mLevelsY );
	}
#endif	// #ifdef __SSE4_1__

}	// namespace lsn

#pragma warning( pop )
