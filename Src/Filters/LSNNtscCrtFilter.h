/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: EMMIR (LMP88959)’s NTSC filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../Event/LSNEvent.h"
#include "LSNFilterBase.h"
#include <thread>
#include <vector>

//#define LSN_CRT_PERF
#ifdef LSN_CRT_PERF
#include "../Utilities/LSNPerformance.h"
#endif	// #ifdef LSN_CRT_PERF

namespace lsn {

	/**
	 * Class CNtscCrtFilter
	 * \brief EMMIR (LMP88959)’s NTSC filter.
	 *
	 * Description: EMMIR (LMP88959)’s NTSC filter.
	 */
	class CNtscCrtFilter : public CFilterBase {
	public :
		CNtscCrtFilter();
		virtual ~CNtscCrtFilter();


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
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate();


	protected :
		// == Members.
		/** The EMMIR (LMP88959) NTSC emulation. */
		//NES_NTSC_SETTINGS									m_nsSettings;
		std::vector<uint8_t>								m_vSettings;
		/** The CRT structure. */
		//CRT													m_nnCrtNtsc;
		std::vector<uint8_t>								m_vCrtNtsc;
		/** The filtered output buffer. */
		std::vector<uint8_t>								m_vFilteredOutput;
		/** The phospher-decay thread. */
		std::unique_ptr<std::thread>						m_ptPhospherThread;
		/** The signal for the phospher-decay thread to go. */
		CEvent												m_ePhospherGo;
		/** The signal that the phospher-decay thread has finished. */
		CEvent												m_ePhospherDone;
		/** The final stride. */
		uint32_t											m_ui32FinalStride;
		/** The phase table. */
		int													m_iPhaseRef[4];
		/** The final width. NTSC is CRT_HRES piels wide. */
		uint32_t											m_ui32FinalWidth;
		/** The final height. */
		uint32_t											m_ui32FinalHeight;
		/** Boolean to stop all threads. */
		std::atomic<bool>									m_bRunThreads;

#ifdef LSN_CRT_PERF
		/** The performance monitor. */
		CPerformance										m_pMonitor;
#endif	// #ifdef LSN_CRT_PERF


		// == Functions.
		/**
		 * Stops the phospher-decay thread.
		 */
		void												StopPhospherDecayThread();

		/**
		 * The phospher-decay thread.
		 *
		 * \param _pncfFilter Pointer to this object.
		 */
		static void											PhospherDecayThread( CNtscCrtFilter * _pncfFilter );
	};

}	// namespace lsn
