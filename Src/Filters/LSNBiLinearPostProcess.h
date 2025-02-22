/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The bilinear post-processing filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Event/LSNEvent.h"
#include "LSNPostProcessBase.h"
#include <thread>
#include <vector>

//#define LSN_BILINEAR_POST_PERF
#ifdef LSN_BILINEAR_POST_PERF
#include "../Utilities/LSNPerformance.h"
#endif	// #ifdef LSN_BILINEAR_POST_PERF


namespace lsn {

	/**
	 * Class CBiLinearPostProcess
	 * \brief The bilinear post-processing filter.
	 *
	 * Description: The bilinear post-processing filter.
	 */
	class CBiLinearPostProcess : public CPostProcessBase {
	public :
		CBiLinearPostProcess();
		~CBiLinearPostProcess();


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
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input,
			uint32_t _ui32ScreenWidth, uint32_t _ui32ScreenHeight, bool &/*_bFlipped*/,
			uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/,
			uint64_t /*_ui64RenderStartCycle*/ );


	protected :
		// == Types.
		/** Thread parameters */
		struct LSN_THREAD {
			CBiLinearPostProcess *							pblppThis;
			uint32_t										ui32ScreenWidth;
			uint32_t										ui32ScreenHeight;
			uint32_t										ui32Stride;
		};

		// == Members.
		/** Black row. */
		//std::vector<uint8_t>								m_vEndRow;
		/** The X factors. */
		std::vector<uint32_t>								m_vFactorsX;
		/** The Y factors. */
		std::vector<uint32_t>								m_vFactorsY;
		/** The intermediate row buffer. */
		std::vector<uint8_t>								m_vRowTmp;
		/** The X factors' source width. */
		uint32_t											m_ui32SourceFactorX;
		/** The Y factors' source height. */
		uint32_t											m_ui32SourceFactorY;

		/** The resizing thread. */
		std::unique_ptr<std::thread>						m_ptResizeThread;
		/** The signal for the phospher-decay thread to go. */
		CEvent												m_eResizeGo;
		/** The signal that the phospher-decay thread has finished. */
		CEvent												m_eResizeDone;
		/** Boolean to stop all threads. */
		std::atomic<bool>									m_bRunThreads;
		/** Thread data. */
		LSN_THREAD											m_tThreadData;

#ifdef LSN_BILINEAR_POST_PERF
		/** The performance monitor. */
		CPerformance										m_pMonitor;
#endif	// #ifdef LSN_BILINEAR_POST_PERF


		// == Functions.
		/**
		 * Stops the resizing thread.
		 */
		void												StopResizeThread();

		/**
		 * The resizing thread.
		 *
		 * \param _pblppFilter Pointer to this object.
		 */
		static void											ResizeThread( LSN_THREAD * _ptThread );
	};

}	// namespace lsn
