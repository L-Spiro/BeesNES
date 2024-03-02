/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNFilterBase.h"

#include <smmintrin.h>


namespace lsn {

	/**
	 * Class CNtscBisqwitFilter
	 * \brief My own implementation of an NTSC filter.
	 *
	 * Description: My own implementation of an NTSC filter.
	 */
	class CNtscLSpiroFilter : public CFilterBase {
	public :
		CNtscLSpiroFilter();
		virtual ~CNtscLSpiroFilter();


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
		 **/
		void												SetKernelSize( uint32_t _ui32Size );

		/**
		 * Sets the width of the input.
		 * 
		 * \param _ui16Width The width to set.
		 * \return Returns true if the memory for the internal buffer(s) was allocated.
		 **/
		bool												SetWidth( uint16_t _ui16Width );

		/**
		 * Sets the height of the input.
		 * 
		 * \param _ui16Height The height to set.
		 * \return Returns true if the memory for the internal buffer(s) was allocated.
		 **/
		bool												SetHeight( uint16_t _ui16Height );


	protected :
		// == Enumerations.
		/** Metrics. */
		enum {
			LSN_MAX_FILTER_SIZE								= 24,									/**< The maximum size of the gather for generating YIQ values. */
		};


		// == Members.
		float												m_fBlack = 0.312f;						/**< Black level. */
		float												m_fWhite = 1.100f;						/**< White level. */
		float												m_fPhaseCosTable[12];					/**< The cosine phase table. */
		float												m_fPhaseSinTable[12];					/**< The sine phase table. */
		__declspec(align(32))
		float												m_fFilter[LSN_MAX_FILTER_SIZE];			/**< The filter kernel. */
		__m128												m_mBlack;								/**< Prepared black level register. */
		__m128												m_mWhiteMinusBlack;						/**< Prepared (white-black) level register. */
		__m128												m_mCosSinTable[12];						/**< The cos/sin table expressed such that each vector is [1.0f, COS, SIN, 0.0f]. */
		__m128												m_mSin33;								/**< sin( -33 degrees ). */
		__m128												m_mCos33;								/**< cos( -33 degrees ). */
		__m128												m_mNegSin33;							/**< sin( 33 degrees ). */
		__m128												m_1_14;									/**< 1.14. */
		__m128												m_2_03;									/**< 2.03. */
		__m128												m_n0_394242;							/**< -0.394242. */
		__m128												m_n0_580681;							/**< -0.580681. */
		__m128												m_1;									/**< 1.0f. */
		__m128												m_0;									/**< 0.0f. */
		__m128												m_255;									/**< 255.0f. */
		__m128												m_255_5;								/**< 255.5f. */
		__m128i												m_255i;									/** 255. */
		float												m_fHue = 0.0f;							/**< The hue. */
		uint32_t											m_ui32FilterKernelSize = 6;				/**< The kernel size for the gather during YIQ creation. */
		std::vector<float>									m_vSignalBuffer;						/**< The intermediate signal buffer for a single scanline. */
		float *												m_pfSignalStart = nullptr;				/**< Points into m_vSignalBuffer.data() at the first location that is both >= to (LSN_MAX_FILTER_SIZE/2) floats and aligned to a 32-byte address. */
		std::vector<__m128>									m_vY;									/**< The YIQ Y buffer. */
		std::vector<__m128>									m_vI;									/**< The YIQ I buffer. */
		std::vector<__m128>									m_vQ;									/**< The YIQ Q buffer. */
		uint16_t											m_ui16Width = 0;						/**< The last input width. */
		uint16_t											m_ui16Height = 0;						/**< The last input height. */
		std::vector<uint8_t>								m_vRgbBuffer;							/**< The output created by calling FilterFrame(). */
		uint32_t											m_ui32FinalStride = 0;					/**< The final stride. */

		__declspec(align(32))
		static const float									m_fLevels[16];							/**< Output levels. */
		static uint8_t										m_ui8Gamma[256];						/**< The gamma curve. */


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
		 **/
		void												ScanlineToYiq( float * _pfDstY, float * _pfDstI, float * _pfDstQ, const uint16_t * _pui16Pixels, uint16_t _ui16Cycle );

		/**
		 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
		 * 
		 * \param _pui8Pixels The input array of 9-bit PPU outputs.
		 * \param _ui16Width Width of the input in pixels.
		 * \param _ui16Height Height of the input in pixels.
		 * \param _ui32Stride Bytes between pixel rows.
		 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
		 * \return Returns true if all internal buffers could be allocated.
		 **/
		bool												FilterFrame( const uint8_t * _pui8Pixels, uint16_t _ui16Width, uint16_t _ui16Height, uint32_t &_ui32Stride, uint64_t _ui64RenderStartCycle );

		/**
		 * Generates the phase sin/cos tables.
		 * 
		 * \param _fHue The hue offset.
		 **/
		void												GenPhaseTables( float _fHue );

		/**
		 * Fills the __m128 registers with the black level and (white-black) level.
		 **/
		void												GenSseNormalizers();

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
		 * \return Returns true if the allocations succeeded.
		 **/
		bool												AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H );

		/**
		 * Converts a single scanline of YIQ values in m_vY/m_vI/m_vQ to BGRA values in the same scanline of m_vRgbBuffer.
		 * 
		 * \param _sScanline The scanline to convert
		 **/
		void												ConvertYiqToBgra( size_t _sScanline );
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
			((ui16Emphasis & 1) && LSN_INCOLORPHASE( 0xC )) ||
			((ui16Emphasis & 2) && LSN_INCOLORPHASE( 0x4 )) ||
			((ui16Emphasis & 4) && LSN_INCOLORPHASE( 0x8 ))) ? 8 : 0;

		// The square wave for this color alternates between these two voltages:
		float fLow  = m_fLevels[ui16Level+ui16Atten];
		float fHigh = (&m_fLevels[4])[ui16Level+ui16Atten];
		if ( ui16Color == 0 ) { return fHigh; }			// For color 0, only high level is emitted.
		if ( ui16Color > 12 ) { return fLow; }			// For colors 13..15, only low level is emitted.

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
		__m128 * pmDst = reinterpret_cast<__m128 *>(_pfDst);		// _pfDst will always be properly aligned.
		for ( size_t I = 0; I < 8; ++I ) {
			(*_pfDst++) = IndexToNtscSignal( _ui16Pixel, uint16_t( _ui16Cycle + I ) );
		}
		__m128 mNumerator = _mm_sub_ps( (*pmDst), m_mBlack );														// signal - black.
		_mm_store_ps( reinterpret_cast<float *>(pmDst++), _mm_div_ps( mNumerator, m_mWhiteMinusBlack ) );			// (signal - black) / (white - black).

		mNumerator = _mm_sub_ps( (*pmDst), m_mBlack );																// signal - black.
		_mm_store_ps( reinterpret_cast<float *>(pmDst), _mm_div_ps( mNumerator, m_mWhiteMinusBlack ) );				// (signal - black) / (white - black).
	}

}	// namespace lsn
