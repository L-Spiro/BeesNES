/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The 2C0X series of PPU's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../System/LSNTickable.h"

namespace lsn {

	/**
	 * Class CPpu2C0X
	 * \brief The 2C0X series of PPU's.
	 *
	 * Description: The 2C0X series of PPU's.
	 */
	template <unsigned _tDotWidth, unsigned _tDotHeight,
		unsigned _tPreRender, unsigned _tRender, unsigned _tPostRender,
		unsigned _tVBlank, unsigned _tPostVBlank,
		bool _bOddFrameShenanigans>
	class CPpu2C0X : public CTickable {
	public :
		CPpu2C0X( CCpuBus * _pbBus ) :
			m_pbBus( _pbBus ),
			m_ui64Frame( 0 ),
			m_ui64Cycle( 0 ),
			m_ui16Scanline( 0 ),
			m_ui16RowDot( 0 ) {
		}
		~CPpu2C0X() {
		}


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
			++m_ui64Cycle;

			// Temporary hack to get things working.
			uint16_t ui16LineWidth = _tDotWidth;
			if constexpr ( _bOddFrameShenanigans ) {
				if ( m_ui64Frame & 0x1 && m_ui16RowDot == (_tDotWidth - 1) && m_ui16Scanline == _tDotHeight ) {
					ui16LineWidth = _tDotWidth - 1;
				}
			}
			if ( ++m_ui16RowDot == ui16LineWidth ) {
				m_ui16RowDot = 0;
				++m_ui16Scanline;
				if ( m_ui16Scanline == _tDotHeight ) {
					m_ui16Scanline = 0;
					++m_ui64Frame;
				}
			}
			else if ( m_ui16RowDot == 1 && m_ui16Scanline == (_tPreRender + _tRender + _tPostRender) ) {
				// [1, 241] on NTSC.
				// [1, 241] on PAL.
				// [1, 241] on Dendy.

			}
			
		}

		/**
		 * Resets the CPU to a known state.
		 */
		void											ResetToKnown() {
			ResetAnalog();
			m_ui64Frame = 0;
			m_ui64Cycle = 0;
			m_ui16Scanline = 0;
			m_ui16RowDot = 0;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void											ResetAnalog() {
		}

		/**
		 * Gets the full X resolution, including off-screen areas.
		 *
		 * \return Returns the full X resolution, including off-screen areas.
		 */
		inline constexpr uint16_t						DotWidth() const { return _tDotWidth; }

		/**
		 * Gets the full Y resolution, including off-screen areas.
		 *
		 * \return Returns the full Y resolution, including off-screen areas.
		 */
		inline constexpr uint16_t						DotHeight() const { return _tDotHeight; }

		/**
		 * Gets the frame count.
		 *
		 * \return Returns the frame count.
		 */
		inline uint64_t									GetFrameCount() const { return m_ui64Frame; }

		/**
		 * Gets the cycle count.
		 *
		 * \return Returns the cycle count.
		 */
		inline uint64_t									GetCycleCount() const { return m_ui64Cycle; }

		/**
		 * Gets the current row position.
		 *
		 * \return Returns the current row position.
		 */
		inline uint16_t									GetCurrentRowPos() const { return m_ui16RowDot; }

		/**
		 * Gets the current scanline.
		 *
		 * \return Returns the current scanline.
		 */
		inline uint16_t									GetCurrentScanline() const { return m_ui16Scanline; }


	protected :
		// == Members.
		uint64_t										m_ui64Frame;									/**< The frame counter. */
		uint64_t										m_ui64Cycle;									/**< The cycle counter. */
		CCpuBus *										m_pbBus;										/**< Pointer to the bus. */
		CPpuBus											m_bBus;											/**< The PPU's internal RAM. */
		uint16_t										m_ui16Scanline;									/**< The scanline counter. */
		uint16_t										m_ui16RowDot;									/**< The horizontal counter. */
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
#define LSN_PPU_TYPE( REGION )							LSN_PM_ ## REGION ## _DOTS_H, LSN_PM_ ## REGION ## _SCANLINES, LSN_PM_ ## REGION ## _PRERENDER, LSN_PM_ ## REGION ## _RENDER_LINES, LSN_PM_ ## REGION ## _POSTRENDER_LINES, LSN_PM_ ## REGION ## _VBLANK_LINES, LSN_PM_ ## REGION ## _POSTBLANK_LINES
	/**
	 * An NTSC PPU.
	 */
	typedef CPpu2C0X<LSN_PPU_TYPE( NTSC ), true>														CNtscPpu;

	/**
	 * A PAL PPU.
	 */
	typedef CPpu2C0X<LSN_PPU_TYPE( PAL ), false>														CPalPpu;

	/**
	 * A Dendy PPU.
	 */
	typedef CPpu2C0X<LSN_PPU_TYPE( DENDY ), false>														CDendyPpu;

#undef LSN_PPU_TYPE


}	// namespace lsn
