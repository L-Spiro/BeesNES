/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The bilinear post-processing filter.
 */

#include "LSNBiLinearPostProcess.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNFilterBase.h"
/*#include "SinCos/EESinCos.h"
#include <cmath>*/


namespace lsn {

	CBiLinearPostProcess::CBiLinearPostProcess() :
#ifdef LSN_BILINEAR_POST_PERF
		m_pMonitor( "CBiLinearPostProcess" ),
#endif	// #ifdef LSN_BILINEAR_POST_PERF
		m_ui32SourceFactorX( 0 ),
		m_ui32SourceFactorY( 0 ),
		m_bRunThreads( true ) {

#if defined( LSN_WINDOWS )
		m_tThreadData.pblppThis = this;
		m_ptResizeThread = std::make_unique<std::thread>( ResizeThread, &m_tThreadData );
#endif	// #if defined( LSN_WINDOWS )
	}
	CBiLinearPostProcess::~CBiLinearPostProcess() {
		StopResizeThread();
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
	uint8_t * CBiLinearPostProcess::ApplyFilter( uint8_t * _pui8Input,
		uint32_t _ui32ScreenWidth, uint32_t _ui32ScreenHeight, bool &/*_bFlipped*/,
		uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/,
		uint64_t /*_ui64RenderStartCycle*/ ) {
		/*double dRatio = double( _ui32ScreenWidth ) / _ui32ScreenHeight;
		_ui32ScreenWidth = std::min<uint32_t>( _ui32ScreenWidth, uint32_t( std::round( 256 * 3 * dRatio ) ) );
		_ui32ScreenHeight = std::min<uint32_t>( _ui32ScreenHeight, 240 * 3 );*/

		uint32_t ui32Stride = CFilterBase::RowStride( _ui32ScreenWidth, 32 );

		if LSN_UNLIKELY( _ui32ScreenWidth == _ui32Width && _ui32ScreenHeight == _ui32Height && ui32Stride == _ui32Stride ) {
			// Pass-through.
			return _pui8Input;
		}

		uint32_t ui32Size = ui32Stride * _ui32ScreenHeight;
		if LSN_UNLIKELY( m_vFinalBuffer.size() != ui32Size ) {
			m_vFinalBuffer = std::vector<uint8_t>();
			m_vFinalBuffer.resize( ui32Size );
		}
		// A blank bottom row isn't needed because CUtilities::LinearInterpCombineRows_Int() checks for (m_vFactorsY[Y] & 0xFF) being 0 and if so it just copies the
		//	first input row, making no accesses into the second input row.
		if LSN_UNLIKELY( _ui32ScreenWidth != m_vFactorsX.size() || m_ui32SourceFactorX != _ui32Width ) {
			m_vFactorsX = std::vector<uint32_t>();
			m_vFactorsX.resize( _ui32ScreenWidth );
			for ( uint32_t X = _ui32ScreenWidth; X--; ) {
				m_vFactorsX[X] = CUtilities::SamplingFactor_BiLinear( _ui32Width, _ui32ScreenWidth, X );
			}
			m_ui32SourceFactorX = _ui32Width;
		}

		if LSN_UNLIKELY( _ui32ScreenHeight != m_vFactorsY.size() || m_ui32SourceFactorY != _ui32Height ) {
			m_vFactorsY = std::vector<uint32_t>();
			m_vFactorsY.resize( _ui32ScreenHeight );
			for ( uint32_t Y = _ui32ScreenHeight; Y--; ) {
				m_vFactorsY[Y] = CUtilities::SamplingFactor_Scanline_Sharp( _ui32Height, _ui32ScreenHeight, Y );
			}
			m_ui32SourceFactorY = _ui32Height;
		}

		if LSN_UNLIKELY( ui32Stride * (_ui32Height + 1) != m_vRowTmp.size() ) {
			m_vRowTmp = std::vector<uint8_t>();
			m_vRowTmp.resize( ui32Stride * (_ui32Height + 1) );
		}

#ifdef LSN_BILINEAR_POST_PERF
		m_pMonitor.Begin();
#endif	// #ifdef LSN_BILINEAR_POST_PERF


		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			uint32_t * pui32SrcRow = reinterpret_cast<uint32_t *>(_pui8Input + (Y * _ui32Stride));
			uint32_t * pui32DstRow = reinterpret_cast<uint32_t *>(m_vRowTmp.data() + (Y * ui32Stride));
			//LSN_PREFETCH_LINE( m_vRowTmp.data() + ((Y + 1) * ui32Stride) );
			CUtilities::LinearInterpolateRow_Int( pui32SrcRow, pui32DstRow, m_vFactorsX.data(), _ui32Width, _ui32ScreenWidth );
		}

		m_tThreadData.ui32ScreenHeight = _ui32ScreenHeight;
		m_tThreadData.ui32ScreenWidth = _ui32ScreenWidth;
		m_tThreadData.ui32Stride = ui32Stride;
		if ( m_ptResizeThread.get() ) {
			m_eResizeGo.Signal();
		}

		uint32_t ui32FinalH = _ui32ScreenHeight >> 1;
		for ( uint32_t Y = 0; Y < ui32FinalH; ++Y ) {
			uint32_t ui32SrcRow = m_vFactorsY[Y] >> 8;
			uint32_t * pui32SrcRow = reinterpret_cast<uint32_t *>(m_vRowTmp.data() + ((ui32SrcRow) * ui32Stride));
			uint32_t * pui32SrcNextRow = reinterpret_cast<uint32_t *>(m_vRowTmp.data() + ((ui32SrcRow + 1) * ui32Stride));
			//LSN_PREFETCH_LINE( m_vRowTmp.data() + ((ui32SrcRow + 2) * ui32Stride) );
			uint32_t * pui32DstRow = reinterpret_cast<uint32_t *>(m_vFinalBuffer.data() + (Y * ui32Stride));
			CUtilities::LinearInterpCombineRows_Int( pui32SrcRow, pui32SrcNextRow, pui32DstRow, _ui32ScreenWidth, m_vFactorsY[Y] & 0xFF );
		}

		if ( m_ptResizeThread.get() ) {
			m_eResizeDone.WaitForSignal();
		}

#ifdef LSN_BILINEAR_POST_PERF
		m_pMonitor.Stop();
#endif	// #ifdef LSN_BILINEAR_POST_PERF
		_ui32Width = _ui32ScreenWidth;
		_ui32Height = _ui32ScreenHeight;
		_ui32Stride = ui32Stride;
		return m_vFinalBuffer.data();
	}

	/**
	 * Stops the resizing thread.
	 */
	void CBiLinearPostProcess::StopResizeThread() {
		m_bRunThreads = false;
		if ( m_ptResizeThread.get() ) {
			m_eResizeGo.Signal();
			m_eResizeDone.WaitForSignal();
			m_ptResizeThread->join();
			m_ptResizeThread.reset();
		}
		m_bRunThreads = true;
	}

	/**
	 * The resizing thread.
	 *
	 * \param _pblppFilter Pointer to this object.
	 */
	void CBiLinearPostProcess::ResizeThread( LSN_THREAD * _ptThread ) {
		while ( _ptThread->pblppThis->m_bRunThreads ) {
			_ptThread->pblppThis->m_eResizeGo.WaitForSignal();
			if ( _ptThread->pblppThis->m_bRunThreads ) {
				for ( uint32_t Y = _ptThread->ui32ScreenHeight >> 1; Y < _ptThread->ui32ScreenHeight; ++Y ) {
					uint32_t ui32SrcRow = _ptThread->pblppThis->m_vFactorsY[Y] >> 8;
					uint32_t * pui32SrcRow = reinterpret_cast<uint32_t *>(_ptThread->pblppThis->m_vRowTmp.data() + ((ui32SrcRow) * _ptThread->ui32Stride));
					uint32_t * pui32SrcNextRow = reinterpret_cast<uint32_t *>(_ptThread->pblppThis->m_vRowTmp.data() + ((ui32SrcRow + 1) * _ptThread->ui32Stride));
					//LSN_PREFETCH_LINE( _ptThread->pblppThis->m_vRowTmp.data() + ((ui32SrcRow + 2) * _ptThread->ui32Stride) );
					uint32_t * pui32DstRow = reinterpret_cast<uint32_t *>(_ptThread->pblppThis->m_vFinalBuffer.data() + (Y * _ptThread->ui32Stride));
					CUtilities::LinearInterpCombineRows_Int( pui32SrcRow, pui32SrcNextRow, pui32DstRow, _ptThread->ui32ScreenWidth, _ptThread->pblppThis->m_vFactorsY[Y] & 0xFF );
				}
			}
			_ptThread->pblppThis->m_eResizeDone.Signal();
		}
	}

}	// namespace lsn
