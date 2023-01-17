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
		m_ui32SourceFactorX( 0 ),
		m_ui32SourceFactorY( 0 ) {
	}
	CBiLinearPostProcess::~CBiLinearPostProcess() {
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
		uint32_t ui32Stride = CFilterBase::RowStride( _ui32ScreenWidth, 32 );
		
		if ( _ui32ScreenWidth == _ui32Width && _ui32ScreenHeight == _ui32Height && ui32Stride == _ui32Stride ) {
			// Pass-through.
			return _pui8Input;
		}

		uint32_t ui32Size = ui32Stride * _ui32ScreenHeight;
		if ( m_vFinalBuffer.size() != ui32Size ) {
			m_vFinalBuffer = std::vector<uint8_t>();
			m_vFinalBuffer.resize( ui32Size );
		}
		// A blank bottom row isn't needed because CUtilities::LinearInterpCombineRows_Int() checks for (m_vFactorsY[Y] & 0xFF) being 0 and if so it just copies the
		//	first input row, making no accesses into the second input row.
		/*if ( ui32Stride != m_vEndRow.size() ) {
			m_vEndRow.resize( ui32Stride );
		}*/
		if ( _ui32ScreenWidth != m_vFactorsX.size() || m_ui32SourceFactorX != _ui32Width ) {
			m_vFactorsX = std::vector<uint32_t>();
			m_vFactorsX.resize( _ui32ScreenWidth );
			for ( uint32_t X = _ui32ScreenWidth; X--; ) {
				
				m_vFactorsX[X] = (((_ui32Width - 1) * X) << 8) / (_ui32ScreenWidth - 1);

				/*double dSin, dCos;
				uint32_t ui32Idx = m_vFactorsX[X] >> 8;
				::SinCos( (m_vFactorsX[X] & 0xFF) / 256.0 * 1.57079632679489661923, &dSin, &dCos );
				uint32_t ui32Frac = uint32_t( std::round( dSin / (dSin + dCos) * 256.0 ) );
				m_vFactorsX[X] = (ui32Idx << 8) | ui32Frac;*/

				/*uint32_t ui32Idx = m_vFactorsX[X] >> 8;
				uint32_t ui32Frac = uint32_t( std::max( 0, int32_t( ((m_vFactorsX[X] & 0xFF) << 1) - 0xFF ) ) );
				m_vFactorsX[X] = (ui32Idx << 8) | ui32Frac;*/
			}
			m_ui32SourceFactorX = _ui32Width;
		}

		if ( _ui32ScreenHeight != m_vFactorsY.size() || m_ui32SourceFactorY != _ui32Height ) {
			m_vFactorsY = std::vector<uint32_t>();
			m_vFactorsY.resize( _ui32ScreenHeight );
			for ( uint32_t Y = _ui32ScreenHeight; Y--; ) {
				m_vFactorsY[Y] = (((_ui32Height - 1) * Y) << 8) / (_ui32ScreenHeight - 1);
				uint32_t ui32Idx = m_vFactorsY[Y] >> 8;
				uint32_t ui32Frac = uint32_t( std::max( 0, int32_t( ((m_vFactorsY[Y] & 0xFF) << 1) - 0xFF ) ) );
				m_vFactorsY[Y] = (ui32Idx << 8) | ui32Frac;
			}
			m_ui32SourceFactorY = _ui32Height;
		}

		if ( ui32Stride * _ui32Height != m_vRowTmp.size() ) {
			m_vRowTmp = std::vector<uint8_t>();
			m_vRowTmp.resize( ui32Stride * _ui32Height );
		}


#if 1
		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			uint32_t * pui32SrcRow = reinterpret_cast<uint32_t *>(_pui8Input + (Y * _ui32Stride));
			uint32_t * pui32DstRow = reinterpret_cast<uint32_t *>(m_vRowTmp.data() + (Y * ui32Stride));
			CUtilities::LinearInterpolateRow_Int( pui32SrcRow, pui32DstRow, m_vFactorsX.data(), _ui32Width, _ui32ScreenWidth );
		}
		for ( uint32_t Y = 0; Y < _ui32ScreenHeight; ++Y ) {
			uint32_t ui32SrcRow = m_vFactorsY[Y] >> 8;
			uint32_t * pui32SrcRow = reinterpret_cast<uint32_t *>(m_vRowTmp.data() + ((ui32SrcRow) * ui32Stride));
			uint32_t * pui32SrcNextRow = reinterpret_cast<uint32_t *>(m_vRowTmp.data() + ((ui32SrcRow + 1) * ui32Stride));
			/*uint32_t * pui32SrcNextRow = (ui32SrcRow == _ui32Height - 1) ?
				reinterpret_cast<uint32_t *>(m_vEndRow.data()) :
				reinterpret_cast<uint32_t *>(m_vRowTmp.data() + ((ui32SrcRow + 1) * ui32Stride));*/
			uint32_t * pui32DstRow = reinterpret_cast<uint32_t *>(m_vFinalBuffer.data() + (Y * ui32Stride));
			CUtilities::LinearInterpCombineRows_Int( pui32SrcRow, pui32SrcNextRow, pui32DstRow, _ui32ScreenWidth, m_vFactorsY[Y] & 0xFF );
		}
#else
		for ( uint32_t Y = 0; Y < _ui32ScreenHeight; ++Y ) {
			uint32_t ui32FactorY = ((_ui32Height * Y) << 8) / _ui32ScreenHeight;
			uint32_t ui32SrcY = ui32FactorY >> 8;
			//ui32FactorY &= 0xFF;
			ui32FactorY = 0;

			uint32_t * pui32SrcRow = reinterpret_cast<uint32_t *>(_pui8Input + (ui32SrcY * _ui32Stride));
			/*uint32_t * pui32SrcNextRow = (ui32SrcY == _ui32Height - 1) ?
				reinterpret_cast<uint32_t *>(m_vEndRow.data()) :
				reinterpret_cast<uint32_t *>(_pui8Input + ((ui32SrcY + 1) * _ui32Stride));*/

			uint32_t * pui32DstRow = reinterpret_cast<uint32_t *>(m_vFinalBuffer.data() + (Y * ui32Stride));
			CUtilities::LinearInterpolateRow_Int( pui32SrcRow, pui32DstRow, m_vFactorsX.data(), _ui32Width, _ui32ScreenWidth );
			/*for ( uint32_t X = _ui32ScreenWidth; X--; ) {
				uint32_t ui32FactorX = ((_ui32Width * X) << 8) / _ui32ScreenWidth;
				uint32_t ui32SrcX = ui32FactorX >> 8;
				ui32FactorX &= 0xFF;

				uint32_t ui32A = pui32SrcRow[ui32SrcX];
				uint32_t ui32B = (ui32SrcX == _ui32Width - 1) ? 0x00000000 : pui32SrcRow[ui32SrcX+1];

				uint32_t ui32C = pui32SrcNextRow[ui32SrcX];
				uint32_t ui32D = (ui32SrcX == _ui32Width - 1) ? 0x00000000 : pui32SrcNextRow[ui32SrcX+1];

				pui32DstRow[X] = CUtilities::BiLinearSample_Int( ui32A, ui32B, ui32C, ui32D, ui32FactorX, ui32FactorY );
			}*/
		}
#endif
		_ui32Width = _ui32ScreenWidth;
		_ui32Height = _ui32ScreenHeight;
		_ui32Stride = ui32Stride;
		return m_vFinalBuffer.data();
	}

}	// namespace lsn
