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

#define LSN_SRGB_POST_PERF
#ifdef LSN_SRGB_POST_PERF
#include "../Utilities/LSNPerformance.h"
#endif	// #ifdef LSN_SRGB_POST_PERF


namespace lsn {

	/**
	 * Class CSrgbPostProcess
	 * \brief The bilinear post-processing filter.
	 *
	 * Description: The bilinear post-processing filter.
	 */
	class CSrgbPostProcess : public CPostProcessBase {
	public :
		CSrgbPostProcess();
		~CSrgbPostProcess();


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
			uint32_t /*_ui32ScreenWidth*/, uint32_t /*_ui32ScreenHeight*/, bool &/*_bFlipped*/,
			uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/,
			uint64_t /*_ui64RenderStartCycle*/ );


	protected :
		// == Types.
		/** Thread parameters */
		struct LSN_THREAD {
			CSrgbPostProcess *								pblppThis;
			uint8_t *										pui8Table;
			uint8_t *										pui8Input;
			uint32_t										ui32ScreenWidth;
			uint32_t										ui32ScreenHeight;
			uint32_t										ui32Stride;
		};

		// == Members.
		/** The resizing thread. */
		std::unique_ptr<std::thread>						m_ptResizeThread[2];
		/** The signal for the phospher-decay thread to go. */
		CEvent												m_eResizeGo[2];
		/** The signal that the phospher-decay thread has finished. */
		CEvent												m_eResizeDone[2];
		/** Boolean to stop all threads. */
		std::atomic<bool>									m_bRunThreads;
		/** Thread data. */
		LSN_THREAD											m_tThreadData;
		/** The sRGB table. */
		uint8_t												m_ui8Table[256];

#ifdef LSN_SRGB_POST_PERF
		/** The performance monitor. */
		CPerformance										m_pMonitor;
#endif	// #ifdef LSN_SRGB_POST_PERF


		// == Functions.
		/**
		 * Stops the threads.
		 */
		void												StopThreads();

		/**
		 * The R thread.
		 *
		 * \param _pblppFilter Pointer to this object.
		 */
		template<unsigned _uIdx>
		static void											Thread( LSN_THREAD * _ptThread ) {
			while ( _ptThread->pblppThis->m_bRunThreads ) {
				_ptThread->pblppThis->m_eResizeGo[_uIdx].WaitForSignal();
				if ( _ptThread->pblppThis->m_bRunThreads ) {
					uint32_t ui32Stride = _ptThread->ui32Stride;
					uint32_t ui32Height = _ptThread->ui32ScreenHeight;
					uint32_t ui32Width = _ptThread->ui32ScreenWidth;
					uint8_t * pui8Data = _ptThread->pblppThis->m_vFinalBuffer.data();
					uint8_t * pui8Input = _ptThread->pui8Input;
					uint8_t * pui8Table = _ptThread->pui8Table;
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						uint8_t * pui8Src = pui8Input + Y * ui32Stride;
						uint8_t * pui8Dst = pui8Data + Y * ui32Stride;
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							pui8Dst[_uIdx] = pui8Table[pui8Src[_uIdx]];

							pui8Src += 4;
							pui8Dst += 4;
						}
					}
				}
				_ptThread->pblppThis->m_eResizeDone[_uIdx].Signal();
			}
		}
	};

}	// namespace lsn
