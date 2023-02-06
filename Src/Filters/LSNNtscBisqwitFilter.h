/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Bisqwit’s NTSC filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../Event/LSNEvent.h"
#include "LSNFilterBase.h"
#include <thread>
#include <vector>


//#define LSN_BISQWIT_PERF
#ifdef LSN_BISQWIT_PERF
#include "../Utilities/LSNPerformance.h"
#endif	// #ifdef LSN_BISQWIT_PERF

namespace lsn {

	/**
	 * Class CNtscBisqwitFilter
	 * \brief Bisqwit’s NTSC filter.
	 *
	 * Description: Bisqwit’s NTSC filter.
	 */
	class CNtscBisqwitFilter : public CFilterBase {
	public :
		CNtscBisqwitFilter();
		virtual ~CNtscBisqwitFilter();


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


	protected :
		// == Types.
		/** Thread parameters */
		struct LSN_THREAD {
			uint64_t										ui64RenderCycle;
			CNtscBisqwitFilter *							pblppThis;
			uint8_t *										pui8Input;
			uint32_t										ui32ScreenWidth;
			uint32_t										ui32ScreenHeight;
			uint32_t										ui32Stride;
		};

		// == Members.
		/** Thread data. */
		LSN_THREAD											m_tThreadData;
		/** The filtered output buffer. */
		std::vector<uint8_t>								m_vFilteredOutput;
		/** The thread. */
		std::unique_ptr<std::thread>						m_ptThread;
		/** The signal for the thread to go. */
		CEvent												m_eGo;
		/** The signal that the thread has finished. */
		CEvent												m_eDone;
		/** The final stride. */
		uint32_t											m_ui32FinalStride;
		/** The final width. NTSC is CRT_HRES piels wide. */
		uint32_t											m_ui32FinalWidth;
		/** The final height. */
		uint32_t											m_ui32FinalHeight;
		/** Boolean to stop all threads. */
		std::atomic<bool>									m_bRunThreads;
		/** Brightness. */
		float												m_fBrightness;
		/** Contrast. */
		float												m_fContrast;
		/** Saturation. */
		float												m_fSaturation;
		/** Hue. */
		float												m_fHue;

		/** The bit-mask look-up table. */
		const uint16_t										m_ui16BitMaskTable[12] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800 };
		/** Signals-per-pixel. */
		static constexpr int32_t							m_i32SignalsPerPixel = 8;
		/** Signal width. */
		static constexpr int32_t							m_i32SignalWidth = 258;

		/* Ywidth, Iwidth and Qwidth are the filter widths for Y,I,Q respectively.
		 *	All widths at 12 produce the best signal quality.
		 *	12,24,24 would be the closest values matching the NTSC spec.
		 *	But off-spec values 12,22,26 are used here, to bring forth mild
		 *	"chroma dots", an artifacting common with badly tuned TVs.
		 *	Larger values = more horizontal blurring.
		 */
		int32_t												m_i32WidthY, m_i32WidthI, m_i32WidthQ;
		int32_t												m_i32Y;
		int32_t												m_i32Ir, m_i32Ig, m_i32Ib;
		int32_t												m_i32Qr, m_i32Qg, m_i32Qb;

		int8_t												m_i8SinTable[27];		// sin( X * 2pi / 12 ) * 8
		int8_t												m_i8SigLow[0x40];
		int8_t												m_i8SigHi[0x40];

		/* PAL:
		 *	const  float   phasex  = (float) 5/18*2;
		 *	const  float   phasey  = (float) 1/ 6*2;
		 *	const  float   pi      = 3.14f;
		 *
		 *	float alpha = (x*phasex + y*phasey)*pi;          // 2*pi*freq*t
		 *	if (y%2 == 0) alpha = -alpha;                    // phase alternating line!
		 *	moire[x+y*18] = Y + U*sin(alpha) + V*cos(alpha); // modulated composite signal */


#ifdef LSN_BISQWIT_PERF
		/** The performance monitor. */
		CPerformance										m_pMonitor;
#endif	// #ifdef LSN_BISQWIT_PERF



		// == Functions.
		/**
		 * Converts one row of the 16-bit PPU output to an NTSC signal.
		 *
		 * \param _pi8Signal The output signal generated.
		 * \param _i32Phase The phase offset of the signal.
		 * \param _i32Row The row to generate.
		 * \param _pui16Input The input palette values.
		 */
		void												GenerateNtscSignal( int8_t * _pi8Signal, int32_t &_i32Phase, int32_t _i32Row, const uint16_t * _pui16Input );

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
		void												NtscDecodeLine( int32_t _i32Width, const int8_t * _pi8Signal, uint32_t * _pui32Output, int32_t _i32Phase0 );

		void												DoFrame( uint64_t _ui64RenderCycle, uint8_t * _pui8Input, uint32_t _ui32From, uint32_t _ui32To, uint32_t _ui32Width );

		/**
		 * Stops the thread.
		 */
		void												StopThread();

		/**
		 * The thread.
		 *
		 * \param _pblppFilter Pointer to this object.
		 */
		static void											Thread( LSN_THREAD * _ptThread );
	};

}	// namespace lsn
