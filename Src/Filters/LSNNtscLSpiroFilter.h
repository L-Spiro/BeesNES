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
#include "LSNLSpiroNtscFilterBase.h"

#include <mutex>

namespace lsn {

	/**
	 * Class CNtscLSpiroFilter
	 * \brief My own implementation of an NTSC filter.
	 *
	 * Description: My own implementation of an NTSC filter.
	 */
	class CNtscLSpiroFilter : public CLSpiroNtscFilterBase, public CFilterBase {
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
		 * Sets the number of worker threads used by the filter.
		 *
		 * \param _stThreads Number of worker threads to use.  0 disables worker threads.
		 */
		void												SetWorkerThreadCount( size_t _stThreads );

		/**
		 * Gets the number of worker threads used by the filter.
		 *
		 * \return Returns the total number of worker threads used by the filter.
		 */
		inline size_t										WorkerThreadCount() const { return m_stWorkerThreadCount; }


	protected :
		// == Types.
		/** A per-frame work package shared by all threads. */
		struct LSN_JOB {
			uint64_t										ui64RenderStartCycle = 0;							/**< The render cycle at the start of the frame. */
			const uint8_t *									pui8Pixels = nullptr;								/**< The input 9-bit pixel array. */
			size_t											stThreads = 1;										/**< Total number of threads for the job, including the calling thread. */
		};


		// == Members.
		std::vector<std::thread>							m_vThreads;											/**< Worker threads. */
		std::mutex											m_mThreadMutex;										/**< Mutex protecting thread state. */
		std::condition_variable								m_cvGo;												/**< Signal to tell worker threads to start a job. */
		std::condition_variable								m_cvDone;											/**< Signal to tell the main thread workers have finished. */
		std::atomic<uint32_t>								m_ui32WorkersRemaining = 0;							/**< Number of workers still running the current job. */
		uint64_t											m_ui64JobId = 0;									/**< Incremented to start a new job. */
		size_t												m_stWorkerThreadCount = 2;							/**< Total number of worker threads. */
		std::vector<uint8_t>								m_vRgbBuffer;										/**< The output created by calling FilterFrame(). */
		LSN_JOB												m_jJob;												/**< The current job. */
		bool												m_bThreadsStarted = false;							/**< True if the worker threads have been created. */
		bool												m_bStopThreads = false;								/**< True if worker threads should exit. */


		// == Functions.
		/**
		 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
		 * 
		 * \param _pui8Pixels The input array of 9-bit PPU outputs.
		 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
		 **/
		void												FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle );

		/**
		 * Allocates the YIQ buffers for a given width and height.
		 * 
		 * \param _ui16W The width of the buffers.
		 * \param _ui16H The height of the buffers.
		 * \param _ui16Scale The width scale factor.
		 * \return Returns true if the allocations succeeded.
		 **/
		virtual bool										AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H, uint16_t _ui16Scale ) override;

		/**
		 * \brief Starts the worker threads.
		 *
		 * Creates m_vThreads based on m_stWorkerThreadCount and resets the thread-control state.
		 * Safe to call multiple times; if threads are already started, this function does nothing.
		 */
		void												StartThreads();

		/**
		 * \brief Stops the worker threads.
		 *
		 * Signals all worker threads to exit, wakes them, joins them, clears m_vThreads, and
		 * resets thread-control state.  Safe to call multiple times; if threads are not started,
		 * this function does nothing.
		 */
		void												StopThreads();

		/**
		 * \brief The worker thread entry point.
		 *
		 * Waits for jobs signaled via m_cvGo, renders the scanline range assigned to this worker,
		 * then decrements m_ui32WorkersRemaining and notifies m_cvDone when the final worker
		 * finishes the job.
		 *
		 * \param _stThreadIdx The worker thread index in the range [1, stThreads - 1].
		 *	Index 0 is reserved for the calling thread.
		 */
		void												WorkerThread( size_t _stThreadIdx );

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace lsn
