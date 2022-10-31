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

#define LSN_CTRL_NAMETABLE_X( OBJ )						(OBJ.s.ui8Nametable & 0x01)
#define LSN_CTRL_NAMETABLE_Y( OBJ )						((OBJ.s.ui8Nametable >> 1) & 0x01)

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
			m_ui16RowDot( 0 ),
			m_bAddresLatch( false ) {

			for ( auto Y = _tDotHeight; Y--; ) {
				for ( auto X = _tDotWidth; X--; ) {
					LSN_CYCLE & cThis = m_cCycles[Y*_tDotWidth+X];
					cThis.pfFunc = &CPpu2C0X::Pixel_Idle;
				}
			}
			// Add row ends.
			for ( auto Y = _tDotHeight; Y--; ) {
				LSN_CYCLE & cThis = m_cCycles[Y*_tDotWidth+(_tDotWidth-1)];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_EndRow;
			}
			// Add frame end.
			{
				LSN_CYCLE & cThis = m_cCycles[(_tDotHeight-1)*_tDotWidth+(_tDotWidth-1)];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_EndFrame;
			}
			// Add v-blank.
			{
				LSN_CYCLE & cThis = m_cCycles[(_tPreRender+_tRender+_tPostRender)*_tDotWidth+1];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_VBlank;
			}
			// Clear v-blank and friends.
			{
				LSN_CYCLE & cThis = m_cCycles[(_tDotHeight-1)*_tDotWidth+1];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_StartFrame;
			}
		}
		~CPpu2C0X() {
		}


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
			++m_ui64Cycle;
			(this->*m_cCycles[m_ui16Scanline*_tDotWidth+m_ui16RowDot].pfFunc)();

			// Temporary hack to get things working.
			/*uint16_t ui16LineWidth = _tDotWidth;
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
			else if ( m_ui16RowDot == 1 ) {
				if ( m_ui16Scanline == (_tPreRender + _tRender + _tPostRender) ) {
					// [1, 241] on NTSC.
					// [1, 241] on PAL.
					// [1, 241] on Dendy.
					m_psPpuStatus.s.ui8VBlank = 1;
					if ( m_pcPpuCtrl.s.ui8Nmi ) {
						m_pnNmiTarget->Nmi();
					}
				}
				else if ( m_ui16Scanline == (_tDotHeight - 1) ) {
					m_psPpuStatus.s.ui8VBlank = m_psPpuStatus.s.ui8Sprite0Hit = 0;
				}
			}*/
			
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
			m_paPpuAddrT.ui16Addr = 0;
			m_paPpuAddrV.ui16Addr = 0;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void											ResetAnalog() {
			m_pcPpuCtrl.ui8Reg = 0;

			m_pmPpuMask.ui8Reg = 0;

			m_psPpuStatus.s.ui8SpriteOverflow = 0;
			m_psPpuStatus.s.ui8Sprite0Hit = 0;
			m_psPpuStatus.s.ui8VBlank = 0;

			m_bAddresLatch = false;
			m_ui8FineScrollX = 0;
		}

		/**
		 * Applies the PPU's memory mapping t the bus.
		 */
		void											ApplyMemoryMap() {
			// Pattern tables.
			for ( uint32_t I = LSN_PPU_PATTERN_TABLES; I < LSN_PPU_NAMETABLES; ++I ) {
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
			}

			// Nametables.
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE) + LSN_PPU_NAMETABLES ) );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE) + LSN_PPU_NAMETABLES ) );
			}

			// Palettes.
			for ( uint32_t I = LSN_PPU_PALETTE_MEMORY; I < LSN_PPU_MEM_FULL_SIZE; ++I ) {
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % LSN_PPU_PALETTE_MEMORY_SIZE) + LSN_PPU_PALETTE_MEMORY ) );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % LSN_PPU_PALETTE_MEMORY_SIZE) + LSN_PPU_PALETTE_MEMORY ) );
			}

			// 0x2000: PPUCTRL.
			m_pbBus->SetReadFunc( 0x2000, PpuNoRead, this, 0x2000 );
			m_pbBus->SetWriteFunc( 0x2000, Write2000, this, 0 );

			// 0x2001: PPUMASK.
			m_pbBus->SetReadFunc( 0x2001, PpuNoRead, this, 0x2001 );
			m_pbBus->SetWriteFunc( 0x2001, Write2001, this, 0 );

			// 0x2002: PPUSTATUS.
			m_pbBus->SetReadFunc( 0x2002, Read2002, this, 0 );
			m_pbBus->SetWriteFunc( 0x2002, Write2002, this, 0 );

			// 0x2003: OAMADDR.

			// 0x2004: OAMDATA.

			// 0x2005: PPUSCROLL.
			m_pbBus->SetReadFunc( 0x2005, PpuNoRead, this, 0x2005 );

			// 0x2006: PPUADDR.
			m_pbBus->SetReadFunc( 0x2006, PpuNoRead, this, 0x2006 );
			m_pbBus->SetWriteFunc( 0x2006, Write2006, this, 0 );

			// 0x2007: PPUDATA.
			m_pbBus->SetReadFunc( 0x2007, Read2007, this, 0 );
			m_pbBus->SetWriteFunc( 0x2007, Write2007, this, 0 );
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
		 * Gets the PPU bus.
		 *
		 * \return Returns the PPU bus.
		 */
		inline CPpuBus &								GetPpuBus() { return m_bBus; }

		/**
		 * An "idle" pixel handler.
		 */
		void LSN_FASTCALL								Pixel_Idle() {
			++m_ui16RowDot;
		}

		/**
		 * An "idle" pixel handler for the end of rows.
		 */
		void LSN_FASTCALL								Pixel_Idle_EndRow() {
			m_ui16RowDot = 0;
			++m_ui16Scanline;
		}

		/**
		 * Starting a frame at [1,_tDotHeight-1] clears some flags.
		 */
		void LSN_FASTCALL								Pixel_Idle_StartFrame() {
			m_psPpuStatus.s.ui8VBlank = m_psPpuStatus.s.ui8Sprite0Hit = 0;
			++m_ui16RowDot;
		}

		/**
		 * An "idle" pixel handler for the end of the frame.
		 */
		void LSN_FASTCALL								Pixel_Idle_EndFrame() {
			m_ui16Scanline = 0;
			
			if ( _bOddFrameShenanigans && m_ui64Frame & 0x1 ) {
				m_ui16RowDot = 1;
			}
			else {
				m_ui16RowDot = 0;
			}
			++m_ui64Frame;
		}

		/**
		 * Handling v-blank.
		 */
		void LSN_FASTCALL								Pixel_Idle_VBlank() {
			// [1, 241] on NTSC.
			// [1, 241] on PAL.
			// [1, 241] on Dendy.
			m_psPpuStatus.s.ui8VBlank = 1;
			if ( m_pcPpuCtrl.s.ui8Nmi ) {
				m_pnNmiTarget->Nmi();
			}
			++m_ui16RowDot;
		}

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
			ppPpu->m_ui8IoBusFloater = ppPpu->m_pcPpuCtrl.ui8Reg = _ui8Val;
			ppPpu->m_paPpuAddrT.s.ui16NametableX = LSN_CTRL_NAMETABLE_X( ppPpu->m_pcPpuCtrl );
			ppPpu->m_paPpuAddrT.s.ui16NametableY = LSN_CTRL_NAMETABLE_Y( ppPpu->m_pcPpuCtrl );
		}

		/**
		 * Writing to 0x2001.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2001( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			ppPpu->m_ui8IoBusFloater = ppPpu->m_pmPpuMask.ui8Reg = _ui8Val;
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
			// The address latch also gets reset.
			ppPpu->m_bAddresLatch = false;
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
		 * Writing to 0x2005.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2005( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Write top 8 bits first.  Easily acheived by flipping the latch before writing.
			if ( !ppPpu->m_bAddresLatch ) {
				ppPpu->m_ui8FineScrollX = _ui8Val & 0x7;
				ppPpu->m_paPpuAddrT.ui16CourseX = _ui8Val >> 3;
			}
			else {
				ppPpu->m_paPpuAddrT.ui16FineY = _ui8Val & 0x7;
				ppPpu->m_paPpuAddrT.ui16CourseY = _ui8Val >> 3;
			}
			ppPpu->m_bAddresLatch ^= 1;
		}

		/**
		 * Writing to 0x2006.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2006( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Write top 8 bits first.  Easily acheived by flipping the latch before writing.
			ppPpu->m_bAddresLatch ^= 1;
			ppPpu->m_paPpuAddrT.ui8Bytes[ppPpu->m_bAddresLatch] = _ui8Val;
			ppPpu->m_paPpuAddrT.ui16Addr &= (ppPpu->m_bBus.Size() - 1);
			if ( !ppPpu->m_bAddresLatch ) {
				// ppPpu->m_bAddresLatch was 1 when we came here, flipped at the start.  This is the 2nd write.
				ppPpu->m_paPpuAddrV.ui16Addr = ppPpu->m_paPpuAddrT.ui16Addr;
			}
		}

		/**
		 * Reading from 0x2007 (PPU bus memory).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read2007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// m_paPpuAddr.ui16Addr is expected to be sanitized so no initial checks for that.
			uint16_t ui16Addr = ppPpu->m_paPpuAddrV.ui16Addr;
			if ( ui16Addr >= LSN_PPU_PALETTE_MEMORY ) {
				// Palette memory is placed on the bus and returned immediately.
				_ui8Ret = ppPpu->m_bBus.Read( ui16Addr );
			}
			else {
				// For every other address the floating-bus contents are returned and the floater is updated with the requested value
				//	to be fetched on the next read.
				_ui8Ret = ppPpu->m_bBus.GetFloat();
				ppPpu->m_bBus.Read( ui16Addr );
			}
			ppPpu->m_paPpuAddrV.ui16Addr = (ui16Addr + (ppPpu->m_pcPpuCtrl.s.ui8IncrementMode ? 32 : 1)) & (LSN_PPU_MEM_FULL_SIZE - 1);
		}

		/**
		 * Writing to 0x2007 ((PPU bus memory).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			uint16_t ui16Addr = ppPpu->m_paPpuAddrV.ui16Addr;
			ppPpu->m_bBus.Write( ui16Addr, _ui8Val );
			ppPpu->m_paPpuAddrV.ui16Addr = (ui16Addr + (ppPpu->m_pcPpuCtrl.s.ui8IncrementMode ? 32 : 1)) & (LSN_PPU_MEM_FULL_SIZE - 1);
		}

		/**
		 * Reading from a write-only address (returns the IO bus contents and fills the bus with the contents at the address).
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
		/** Function pointer for per-cycle work. */
		typedef void (LSN_FASTCALL CPpu2C0X:: *			PfCycles)();

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

		/** The PPUMASK register. */
		struct LSN_PPUMASK {
			union {
				struct {
					uint8_t								ui8Greyscale			: 1;					/**< 0 = color, 1 = greyscale. */
					uint8_t								ui8LeftBackground		: 1;					/**< Show the background in the left-most 8 pixels. */
					uint8_t								ui8LeftSprites			: 1;					/**< Show sprites in the left-most 8 pixels. */
					uint8_t								ui8ShowBackground		: 1;					/**< Show the background. */
					uint8_t								ui8ShowSprites			: 1;					/**< Show sprites. */
					uint8_t								ui8RedEmph				: 1;					/**< Emphasize red (green on PAL/Dendy). */
					uint8_t								ui8GreenEmph			: 1;					/**< Emphasize green (red on PAL/Dendy). */
					uint8_t								ui8BlueEmph				: 1;					/**< Emphasize blue. */
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

		/** The PPUADDR register (loopy). */
		struct LSN_PPUADDR {
			union {
				uint16_t								ui16Addr;										/**< The full 16-bit address. */
				uint8_t									ui8Bytes[2];									/**< Per-byte access to the address. */
				struct {
					uint16_t							ui16CourseX : 5;								/**< Course X scroll position. */
					uint16_t							ui16CourseY : 5;								/**< Course Y scroll position. */
					uint16_t							ui16NametableX : 1;								/**< Nametable X. */
					uint16_t							ui16NametableY : 1;								/**< Nametable Y. */
					uint16_t							ui16FineY : 3;									/**< Fine Y. */
				}										s;
			};
		};

		/** Cycle handlers (function pointers doing per-cycle/per-pixel work). */
		struct LSN_CYCLE {
			PfCycles									pfFunc;											/**< The function pointer doing the work on this pixel. */
		};


		// == Members.
		uint64_t										m_ui64Frame;									/**< The frame counter. */
		uint64_t										m_ui64Cycle;									/**< The cycle counter. */
		CCpuBus *										m_pbBus;										/**< Pointer to the bus. */
		CNmiable *										m_pnNmiTarget;									/**< The target object of NMI notifications. */
		LSN_CYCLE										m_cCycles[_tDotWidth*_tDotHeight];				/**< The per-pixel array of function pointers to do per-cycle work. */
		CPpuBus											m_bBus;											/**< The PPU's internal RAM. */
		uint16_t										m_ui16Scanline;									/**< The scanline counter. */
		uint16_t										m_ui16RowDot;									/**< The horizontal counter. */
		LSN_PPUADDR										m_paPpuAddrT;									/**< The "t" PPUADDR register. */
		LSN_PPUADDR										m_paPpuAddrV;									/**< The "v" PPUADDR register. */
		LSN_PPUCTRL										m_pcPpuCtrl;									/**< The PPUCTRL register. */
		LSN_PPUMASK										m_pmPpuMask;									/**< The PPUMASK register. */
		LSN_PPUSTATUS									m_psPpuStatus;									/**< The PPUSTATUS register. */		
		uint8_t											m_ui8IoBusFloater;								/**< The I/O bus floater. */
		uint8_t											m_ui8FineScrollX;								/**< The fine X scroll position. */
		bool											m_bAddresLatch;									/**< The address latch. */


		// == Functions.
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

#undef LSN_CTRL_NAMETABLE_X
#undef LSN_CTRL_NAMETABLE_Y