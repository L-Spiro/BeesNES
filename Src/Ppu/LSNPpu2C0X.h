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
#include "../System/LSNNmiable.h"
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
		CPpu2C0X( CCpuBus * _pbBus, CNmiable * _pnNmiTarget ) :
			m_pbBus( _pbBus ),
			m_pnNmiTarget( _pnNmiTarget ),
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
				m_psPpuStatus.s.ui8VBlank = 1;
				if ( m_pcPpuCtrl.s.ui8Nmi ) {
					m_pnNmiTarget->Nmi();
				}
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
			m_pcPpuCtrl.ui8Reg = 0;

			m_psPpuStatus.s.ui8SpriteOverflow = 0;
			m_psPpuStatus.s.ui8Sprite0Hit = 0;
			m_psPpuStatus.s.ui8VBlank = 0;
		}

		/**
		 * Applies the PPU's memory mapping t the bus.
		 */
		void											ApplyMemoryMap() {
			// 0x2000: PPUCTRL.
			m_pbBus->SetReadFunc( 0x2000, PpuNoRead, this, 0x2000 );
			m_pbBus->SetWriteFunc( 0x2000, Write2000, this, 0x2000 );

			// 0x2001: PPUMASK.
			m_pbBus->SetReadFunc( 0x2001, PpuNoRead, this, 0x2001 );

			// 0x2002: PPUSTATUS.
			m_pbBus->SetReadFunc( 0x2002, Read2002, this, 0 );
			m_pbBus->SetWriteFunc( 0x2002, Write2002, this, 0 );

			// 0x2003: OAMADDR.

			// 0x2004: OAMDATA.

			// 0x2005: PPUSCROLL.
			m_pbBus->SetReadFunc( 0x2005, PpuNoRead, this, 0x2005 );

			// 0x2006: PPUADDR.
			m_pbBus->SetReadFunc( 0x2006, PpuNoRead, this, 0x2006 );
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

		/**
		 * Writing to 0x2000.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Only the top 3 bits can be modified on the floating bus.
			ppPpu->m_ui8IoBusFloater = ppPpu->m_pcPpuCtrl.ui8Reg = _ui8Val;
		}

		/**
		 * Reading from 0x2002 (resets the v-blank flag).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read2002( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Most registers return the I/O float and then update the I/O float with the red value.
			// 0x2002 immediately updates the floating bus.
			ppPpu->m_ui8IoBusFloater = (ppPpu->m_ui8IoBusFloater & 0x1F) | (ppPpu->m_psPpuStatus.ui8Reg & 0xE0);
			_ui8Ret = ppPpu->m_ui8IoBusFloater;
			// Reads cause the v-blank flag to reset.
			ppPpu->m_psPpuStatus.s.ui8VBlank = 0;
		}

		/**
		 * Writing to 0x2002.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2002( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Only the top 3 bits can be modified on the floating bus.
			ppPpu->m_ui8IoBusFloater = (ppPpu->m_ui8IoBusFloater & 0x1F) | (_ui8Val & 0xE0);
		}

		/**
		 * Reading from 0x2002 (resets the v-blank flag).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PpuNoRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			_ui8Ret = ppPpu->m_ui8IoBusFloater;

			ppPpu->m_ui8IoBusFloater = _pui8Data[_ui16Parm1];
		}

		/**
		 * A function usable for addresses that can't be written on the PPU.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						PpuNoWrite( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			ppPpu->m_ui8IoBusFloater = _ui8Val;
		}


	protected :
		// == Types.
		/** The PPUCTRL register. */
		struct LSN_PPUCTRL {
			union {
				struct {
					uint8_t								ui8Nametable			: 2;					/**< Base nametable address. */
					uint8_t								ui8IncrementMode		: 1;					/**< VRAM address-increment per CPU read/write of PPUDATA. */
					uint8_t								ui8SpriteTileSelect		: 1;					/**< Sprite pattern table address for 8-by-8 tiles. */
					uint8_t								ui8BackgroundTileSelect	: 1;					/**< Background pattern table. */
					uint8_t								ui8SpriteSize			: 1;					/**< Sprite size (0 = 8-by-8, 1 = 8-by-16). */
					uint8_t								ui8Slave				: 1;					/**< Master/slave setting. */
					uint8_t								ui8Nmi					: 1;					/**< Generate an NMI at the start of v-blank (0 = no, 1 = yes). */
				}										s;

				uint8_t									ui8Reg;											/**< Register as a uint8_t. */
			};
		};

		/** The PPUSTATUS register. */
		struct LSN_PPUSTATUS {
			union {
				struct {
					uint8_t								ui8OpenBus				: 5;					/**< Unused bits redirect to the open bus. */
					uint8_t								ui8SpriteOverflow		: 1;					/**< Sprite-overflow flag. */
					uint8_t								ui8Sprite0Hit			: 1;					/**< Sprite-0 hit flag. */
					uint8_t								ui8VBlank				: 1;					/**< V-blank flag. */
				}										s;

				uint8_t									ui8Reg;											/**< Register as a uint8_t. */
			};
		};


		// == Members.
		uint64_t										m_ui64Frame;									/**< The frame counter. */
		uint64_t										m_ui64Cycle;									/**< The cycle counter. */
		CCpuBus *										m_pbBus;										/**< Pointer to the bus. */
		CNmiable *										m_pnNmiTarget;									/**< The target object of NMI notifications. */
		CPpuBus											m_bBus;											/**< The PPU's internal RAM. */
		uint16_t										m_ui16Scanline;									/**< The scanline counter. */
		uint16_t										m_ui16RowDot;									/**< The horizontal counter. */
		uint8_t											m_ui8IoBusFloater;								/**< The I/O bus floater. */
		LSN_PPUCTRL										m_pcPpuCtrl;									/**< The PPUCTRL register. */
		LSN_PPUSTATUS									m_psPpuStatus;									/**< The PPUSTATUS register. */
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
