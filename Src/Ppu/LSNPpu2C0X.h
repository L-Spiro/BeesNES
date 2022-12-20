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
#include "../Display/LSNDisplayClient.h"
#include "../Display/LSNDisplayHost.h"
#include "../Palette/LSNPalette.h"
#include "../System/LSNNmiable.h"
#include "../System/LSNTickable.h"

#define LSN_CTRL_NAMETABLE_X( OBJ )						(OBJ.s.ui8Nametable & 0x01)
#define LSN_CTRL_NAMETABLE_Y( OBJ )						((OBJ.s.ui8Nametable >> 1) & 0x01)

//#define LSN_USE_PPU_DIAGRAM_CYCLE_SKIP

#define LSN_END_CONTROL_CYCLE							++m_ui16RowDot;

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
		unsigned _tRenderW, unsigned _tBorderW,
		bool _bOddFrameShenanigans, double _dPerferredRatio>
	class CPpu2C0X : public CTickable, public CDisplayClient {
	public :
		CPpu2C0X( CCpuBus * _pbBus, CNmiable * _pnNmiTarget ) :
			m_pbBus( _pbBus ),
			m_pnNmiTarget( _pnNmiTarget ),
			m_ui64Frame( 0 ),
			m_ui64Cycle( 0 ),
			m_ui16Scanline( 0 ),
			m_ui16RowDot( 0 ),
			m_ui16ShiftPatternLo( 0 ),
			m_ui16ShiftPatternHi( 0 ),
			m_ui16ShiftAttribLo( 0 ),
			m_ui16ShiftAttribHi( 0 ),
			m_ui8NtAtBuffer( 0 ),
			m_ui8NextTileId( 0 ),
			m_ui8NextTileAttribute( 0 ),
			m_ui8NextTileLsb( 0 ),
			m_ui8NextTileMsb( 0 ),
			m_ui8IoBusFloater( 0 ),
			m_ui8OamAddr( 0 ),
			m_ui8OamLatch( 0 ),
			m_ui8Oam2ClearIdx( 0 ),
			m_bAddresLatch( false ) {

			for ( auto Y = _tDotHeight; Y--; ) {
				for ( auto X = _tDotWidth; X--; ) {
					{
						LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+X];
						cThis.pfFunc = &CPpu2C0X::Pixel_Idle_Control;
					}
					{
						LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+X];
						cThis.pfFunc = &CPpu2C0X::Pixel_Idle_Work;
					}
					{
						LSN_CYCLE & cThis = m_cSpriteCycles[Y*_tDotWidth+X];
						cThis.pfFunc = &CPpu2C0X::Pixel_Idle_Sprite;
					}
				}
			}
			// Add pixel gather/render functions.
			{
				// The main rendering area.

				for ( auto Y = 0; Y < (_tPreRender + _tRender); ++Y ) {
					ApplyStdRenderFunctionPointers( uint16_t( Y ) );
					ApplySpriteFunctionPointers( uint16_t( Y ) );
				}
				// The "-1" scanline.
				{
					constexpr size_t Y = _tDotHeight - 1;
					ApplyStdRenderFunctionPointers( uint16_t( Y ) );
					ApplySpriteFunctionPointers( uint16_t( Y ) );
				}

			}


			// Add row ends.
			for ( auto Y = _tDotHeight; Y--; ) {
				LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+(_tDotWidth-1)];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_EndRow_Control;
			}
			// Add frame end.
			{
#ifdef LSN_USE_PPU_DIAGRAM_CYCLE_SKIP
				LSN_CYCLE & cThis = m_cControlCycles[(_tDotHeight-1)*_tDotWidth+(_tDotWidth-1)];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_EndFrame_Control;
#else
				{
					LSN_CYCLE & cThis = m_cControlCycles[(_tDotHeight-1)*_tDotWidth+(_tDotWidth-2)];
					cThis.pfFunc = &CPpu2C0X::Pixel_Idle_EndFrame_Control;
				}
				{
					LSN_CYCLE & cThis = m_cControlCycles[(_tDotHeight-1)*_tDotWidth+(_tDotWidth-1)];
					cThis.pfFunc = &CPpu2C0X::Pixel_Idle_EndFrame_Even_Control;
				}
#endif
				
			}
			// Add v-blank.
			{
				LSN_CYCLE & cThis = m_cControlCycles[(_tPreRender+_tRender+_tPostRender)*_tDotWidth+1];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_VBlank_Control;
			}
			// Clear v-blank and friends.
			{
				LSN_CYCLE & cThis = m_cControlCycles[(_tDotHeight-1)*_tDotWidth+1];
				cThis.pfFunc = &CPpu2C0X::Pixel_Idle_StartFrame_Control;
			}
			// Swap the display buffer.
			{
				LSN_CYCLE & cThis = m_cControlCycles[(_tPreRender+_tRender)*_tDotWidth+(1+_tRenderW+2)];
				cThis.pfFunc = &CPpu2C0X::Pixel_Swap_Control;
			}
		}
		~CPpu2C0X() {
		}


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
			size_t stIdx = m_ui16Scanline * _tDotWidth + m_ui16RowDot;
			(this->*m_cSpriteCycles[stIdx].pfFunc)();
			(this->*m_cWorkCycles[stIdx].pfFunc)();
			(this->*m_cControlCycles[stIdx].pfFunc)();
			++m_ui64Cycle;
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
			m_ui8IoBusFloater = 0;
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

			m_ui8OamAddr = 0;
			m_ui8OamLatch = 0;
			m_ui8Oam2ClearIdx = 0;
		}

		/**
		 * Applies the PPU's memory mapping t the bus.
		 */
		void											ApplyMemoryMap() {
			// == Pattern Tables
			for ( uint32_t I = LSN_PPU_PATTERN_TABLES; I < LSN_PPU_NAMETABLES; ++I ) {
				m_bBus.SetReadFunc( uint16_t( I ), CPpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
				// Default to ROM.  Allow cartridges to udpate the write pointers to make it RAM.
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::NoWrite, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
				//m_bBus.SetWriteFunc( uint16_t( I ), CPpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
			}

			// == Nametables
			ApplyVerticalMirroring();

			// == Palettes
			for ( uint32_t I = LSN_PPU_PALETTE_MEMORY; I < LSN_PPU_MEM_FULL_SIZE; ++I ) {
				m_bBus.SetReadFunc( uint16_t( I ), CPpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % LSN_PPU_PALETTE_MEMORY_SIZE) + LSN_PPU_PALETTE_MEMORY ) );
				m_bBus.SetWriteFunc( uint16_t( I ), CPpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % LSN_PPU_PALETTE_MEMORY_SIZE) + LSN_PPU_PALETTE_MEMORY ) );
			}
			// 4th color of each entry mirrors the background color at LSN_PPU_PALETTE_MEMORY.
			for ( uint32_t I = LSN_PPU_PALETTE_MEMORY + 4; I < LSN_PPU_MEM_FULL_SIZE; I += 4 ) {
				/*m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % (LSN_PPU_PALETTE_MEMORY_SIZE / 2)) + LSN_PPU_PALETTE_MEMORY ) );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % (LSN_PPU_PALETTE_MEMORY_SIZE / 2)) + LSN_PPU_PALETTE_MEMORY ) );*/
				m_bBus.SetReadFunc( uint16_t( I ), CPpuBus::StdRead, this, uint16_t( LSN_PPU_PALETTE_MEMORY ) );
				m_bBus.SetWriteFunc( uint16_t( I ), WritePaletteIdx4, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % (LSN_PPU_PALETTE_MEMORY_SIZE / 2)) + LSN_PPU_PALETTE_MEMORY ) );
			}

			for ( uint32_t I = LSN_PPU_START; I < LSN_APU_START; I += LSN_PPU ) {
				// 0x2000: PPUCTRL.
				m_pbBus->SetReadFunc( uint16_t( I + 0x00 ), PpuNoRead, this, LSN_PPU_START );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x00 ), Write2000, this, 0 );

				// 0x2001: PPUMASK.
				m_pbBus->SetReadFunc( uint16_t( I + 0x01 ), PpuNoRead, this, LSN_PPU_START + 0x01 );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x01 ), Write2001, this, 0 );

				// 0x2002: PPUSTATUS.
				m_pbBus->SetReadFunc( uint16_t( I + 0x02 ), Read2002, this, 0 );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x02 ), Write2002, this, 0 );

				// 0x2003: OAMADDR.
				m_pbBus->SetReadFunc( uint16_t( I + 0x03 ), PpuNoRead, this, LSN_PPU_START + 0x03 );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x03 ), Write2003, this, 0 );

				// 0x2004: OAMDATA.
				m_pbBus->SetReadFunc( uint16_t( I + 0x04 ), Read2004, this, 0 );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x04 ), Write2004, this, 0 );

				// 0x2005: PPUSCROLL.
				m_pbBus->SetReadFunc( uint16_t( I + 0x05 ), PpuNoRead, this, LSN_PPU_START + 0x05 );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x05 ), Write2005, this, 0 );

				// 0x2006: PPUADDR.
				m_pbBus->SetReadFunc( uint16_t( I + 0x06 ), PpuNoRead, this, LSN_PPU_START + 0x06 );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x06 ), Write2006, this, 0 );

				// 0x2007: PPUDATA.
				m_pbBus->SetReadFunc( uint16_t( I + 0x07 ), Read2007, this, 0 );
				m_pbBus->SetWriteFunc( uint16_t( I + 0x07 ), Write2007, this, 0 );
			}
		}

		/**
		 * Applies vertical mirroring to the nametable addresses ([LSN_PPU_NAMETABLES..LSN_PPU_PALETTE_MEMORY]).
		 */
		void											ApplyVerticalMirroring() {
			// == Nametables
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				uint16_t ui16Root = ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
				ui16Root %= LSN_PPU_NAMETABLES_SCREEN * 2;									// Map $2800 to $2000 and $2C00 to $2400.
				ui16Root += LSN_PPU_NAMETABLES;
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, ui16Root );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, ui16Root );
			}
		}

		/**
		 * Applies horizontal mirroring to the nametable addresses ([LSN_PPU_NAMETABLES..LSN_PPU_PALETTE_MEMORY]).
		 */
		void											ApplyHorizontalMirroring() {
			// == Nametables
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				uint16_t ui16Root = ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
				// Mirror $2400 to $2000 and $2C00 to $2800.
				ui16Root = (ui16Root % LSN_PPU_NAMETABLES_SCREEN) + ((ui16Root / (LSN_PPU_NAMETABLES_SCREEN * 2)) * (LSN_PPU_NAMETABLES_SCREEN * 2)) + LSN_PPU_NAMETABLES;
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, ui16Root );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, ui16Root );
			}
		}

		/**
		 * Applies 4-screens mirroring to the nametable addresses ([LSN_PPU_NAMETABLES..LSN_PPU_PALETTE_MEMORY]).
		 */
		void											ApplyFourScreensMirroring() {
			// == Nametables
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				uint16_t ui16Root = ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
				ui16Root += LSN_PPU_NAMETABLES;
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, ui16Root );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, ui16Root );
			}
		}

		/**
		 * Applies 1-screen mirroring to the nametable addresses ([LSN_PPU_NAMETABLES..LSN_PPU_PALETTE_MEMORY]).
		 */
		void											ApplyOneScreenMirroring() {
			// == Nametables
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				uint16_t ui16Root = ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
				ui16Root = (ui16Root % LSN_PPU_NAMETABLES_SCREEN) + LSN_PPU_NAMETABLES;
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, ui16Root );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, ui16Root );
			}
		}

		/**
		 * Applies 1-screen mirroring to the nametable addresses ([LSN_PPU_NAMETABLES..LSN_PPU_PALETTE_MEMORY]).
		 */
		void											ApplyOneScreenMirroring_B() {
			// == Nametables
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				uint16_t ui16Root = ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
				ui16Root = (ui16Root % LSN_PPU_NAMETABLES_SCREEN) + LSN_PPU_NAMETABLES + LSN_PPU_NAMETABLES_SCREEN;
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, ui16Root );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, ui16Root );
			}
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
		 * Gets the display width in pixels.  Used to create render targets.
		 *
		 * \return Returns the pixel width of the display area.
		 */
		virtual uint32_t								DisplayWidth() const { return _tRenderW; }

		/**
		 * Gets the display height in pixels.  Used to create render targets.
		 *
		 * \return Returns the pixel height of the display area.
		 */
		virtual uint32_t								DisplayHeight() const { return _tRender + _tPreRender; }

		/**
		 * Gets the display ratio in pixels.
		 *
		 * \return Returns the ratio of the display area.
		 */
		virtual double									DisplayRatio() const { return _dPerferredRatio; }

		/**
		 * If true, extra room is added to the side of the view to display some debug information.
		 *
		 * \return Returns true in order to add an extra 128 pixels horizontally for debug display, otherwise false.  Defaults to false.
		 */
		virtual bool									DebugSideDisplay() const { return false; }

		/**
		 * Gets a reference to the PPU bus.
		 *
		 * \return Returns a reference to the PPU bus.
		 */
		CPpuBus &										GetBus() { return m_bBus; }

		/**
		 * Gets the palette.
		 *
		 * \return Returns a reference to the palette.
		 */
		LSN_PALETTE &									Palette() { return m_pPalette; }

		/**
		 * Gets the palette as read-only.
		 *
		 * \return Returns a constant reference to the palette.
		 */
		const LSN_PALETTE &								Palette() const { return m_pPalette; }

		/**
		 * An "idle" pixel handler.
		 */
		void LSN_FASTCALL								Pixel_Idle_Control() {
			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * An "idle" work pixel handler.  Does nothing.
		 */
		void LSN_FASTCALL								Pixel_Idle_Work() {
		}

		/**
		 * An "idle" sprite pixel handler.  Does nothing.
		 */
		void LSN_FASTCALL								Pixel_Idle_Sprite() {
		}

		/**
		 * An "idle" pixel handler for the end of rows.
		 */
		void LSN_FASTCALL								Pixel_Idle_EndRow_Control() {
			m_ui16RowDot = 0;
			++m_ui16Scanline;
		}

		/**
		 * Starting a frame at [1,_tDotHeight-1] clears some flags.
		 */
		void LSN_FASTCALL								Pixel_Idle_StartFrame_Control() {
			m_psPpuStatus.s.ui8VBlank = m_psPpuStatus.s.ui8Sprite0Hit = 0;
			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * An "idle" pixel handler for the end of the frame.
		 */
		void LSN_FASTCALL								Pixel_Idle_EndFrame_Control() {
#ifdef LSN_USE_PPU_DIAGRAM_CYCLE_SKIP
			m_ui16Scanline = 0;
			
			if ( _bOddFrameShenanigans && (m_ui64Frame & 0x1) ) {
				m_ui16RowDot = 1;
			}
			else {
				m_ui16RowDot = 0;
			}
			++m_ui64Frame;
#else
			if constexpr ( _bOddFrameShenanigans ) {
				if ( (m_ui64Frame & 0x1) && Rendering() ) {
					// Skips Pixel_Idle_EndFrame_Even_Control().
					m_ui16Scanline = 0;
					m_ui16RowDot = 0;
					++m_ui64Frame;
				}
				else {
					// Goes to Pixel_Idle_EndFrame_Even_Control().
					LSN_END_CONTROL_CYCLE;
				}
			}
			else {
				// Goes to Pixel_Idle_EndFrame_Even_Control().
				LSN_END_CONTROL_CYCLE;
			}
#endif	// #ifdef LSN_USE_PPU_DIAGRAM_CYCLE_SKIP
		}

		/**
		 * An "idle" pixel handler for the end of even frames.  Skipped on odd frames if rendering is off.
		 */
		void LSN_FASTCALL								Pixel_Idle_EndFrame_Even_Control() {
			m_ui16Scanline = 0;
			m_ui16RowDot = 0;
			++m_ui64Frame;
		}

		/**
		 * Handling v-blank.
		 */
		void LSN_FASTCALL								Pixel_Idle_VBlank_Control() {
			// [1, 241] on NTSC.
			// [1, 241] on PAL.
			// [1, 241] on Dendy.
			m_psPpuStatus.s.ui8VBlank = 1;
			if ( m_pcPpuCtrl.s.ui8Nmi ) {
				m_pnNmiTarget->Nmi();
			}
			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * Informing the display host that a render is done.
		 */
		void LSN_FASTCALL								Pixel_Swap_Control() {
			if ( m_pdhHost ) {
				if ( m_pui8RenderTarget ) {
					
					if ( DebugSideDisplay() ) {
						for ( uint16_t I = 0; I < 2; ++I ) {
							for ( uint16_t ui16TileY = 0; ui16TileY < 16; ++ui16TileY ) {
								for ( uint16_t ui16TileX = 0; ui16TileX < 16; ++ui16TileX ) {
									uint16_t ui16Offset = ui16TileY * 256 + ui16TileX * 16;
									for ( uint16_t ui16Row = 0; ui16Row < 8; ++ui16Row ) {
										uint8_t ui8TileLsb = m_bBus.Read( 0x1000 * I + ui16Offset + ui16Row + 0 );
										uint8_t ui8TileMsb = m_bBus.Read( 0x1000 * I + ui16Offset + ui16Row + 8 );
										for ( uint16_t ui16Col = 0; ui16Col < 8; ++ui16Col ) {
											uint8_t ui8Pixel = (ui8TileLsb & 0x01) + (ui8TileMsb & 0x01);
											ui8TileLsb >>= 1;
											ui8TileMsb >>= 1;
											uint16_t ui16X = ui16TileX * 8 + (7 - ui16Col);
											ui16X += _tRenderW;

											uint16_t ui16Y = I * (16 * 8) + ui16TileY * 8 + ui16Row;
											ui16Y = (_tRender - 1) - ui16Y;
											if ( ui16Y < _tRender ) {

												uint8_t * pui8This = &m_pui8RenderTarget[ui16Y*m_stRenderTargetStride+ui16X*3];
												uint8_t ui8Val = ui8Pixel * (256 / 4);
												pui8This[0] = pui8This[1] = pui8This[2] = ui8Val;
											}
										}
									}
								}
							}
						}
					}
#if 0
					// For fun.
					for ( auto Y = DisplayHeight(); Y--; ) {
						for ( auto X = DisplayWidth(); X--; ) {
							uint8_t * pui8This = &m_pui8RenderTarget[Y*m_stRenderTargetStride+X*3];
							uint16_t ui16Addr = uint16_t( (Y + m_ui64Frame) * 256 + X * 1 );
							
							uint8_t ui8Val = m_pbBus->DBG_Inspect( ui16Addr + 0 );
							pui8This[0] = pui8This[1] = pui8This[2] = ui8Val;
							/*pui8This[2] = m_pbBus->DBG_Inspect( ui16Addr + 1 );
							pui8This[0] = m_pbBus->DBG_Inspect( ui16Addr + 0 );*/
						}
					}
#endif
				}
				m_pdhHost->Swap();
			}

			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * The first of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadNtNoShift_0_Work() {
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF) );
		}

		/**
		 * The second of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadNtNoShift_1_Work() {
			m_ui8NextTileId = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadNt_0_Work() {
			ShiftBackgroundRegisters();
			LoadLatchedBackgroundIntoShiftRegisters();
			RenderPixel();
			// LSN_PPU_NAMETABLES = 0x2000.
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF) );
		}

		/**
		 * The second of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadNt_1_Work() {
			ShiftBackgroundRegisters();
			RenderPixel();
			m_ui8NextTileId = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 AT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadAt_0_Work() {
			ShiftBackgroundRegisters();
			RenderPixel();
			// LSN_PPU_NAMETABLES = 0x2000.
			// LSN_PPU_ATTRIBUTE_TABLE_OFFSET = 0x03C0.
			m_ui8NtAtBuffer = m_bBus.Read( (LSN_PPU_NAMETABLES + LSN_PPU_ATTRIBUTE_TABLE_OFFSET) | (m_paPpuAddrV.s.ui16NametableY << 11) |
				(m_paPpuAddrV.s.ui16NametableX << 10) |
				((m_paPpuAddrV.s.ui16CourseY >> 2) << 3) |
				(m_paPpuAddrV.s.ui16CourseX >> 2) );
			if ( m_paPpuAddrV.s.ui16CourseY & 0x2 ) { m_ui8NtAtBuffer >>= 4; }
			if ( m_paPpuAddrV.s.ui16CourseX & 0x2 ) { m_ui8NtAtBuffer >>= 2; }
			m_ui8NtAtBuffer &= 0x3;
		}

		/**
		 * The second of the 2 AT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadAt_1_Work() {
			ShiftBackgroundRegisters();
			RenderPixel();
			m_ui8NextTileAttribute = m_ui8NtAtBuffer;
			/*if ( m_paPpuAddrV.s.ui16CourseY & 0x2 ) { m_ui8NextTileAttribute >>= 4; }
			if ( m_paPpuAddrV.s.ui16CourseX & 0x2 ) { m_ui8NextTileAttribute >>= 2; }
			m_ui8NextTileAttribute &= 0x3;*/
		}

		/**
		 * The first of the 2 LSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadLsb_0_Work() {
			ShiftBackgroundRegisters();
			RenderPixel();
			// LSN_PPU_PATTERN_TABLES = 0x0000.
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
				(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
				(m_paPpuAddrV.s.ui16FineY) +
				0) );
		}

		/**
		 * The second of the 2 LSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadLsb_1_Work() {
			ShiftBackgroundRegisters();
			RenderPixel();
			m_ui8NextTileLsb = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 MSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadMsb_0_Work() {
			ShiftBackgroundRegisters();
			RenderPixel();
			// LSN_PPU_PATTERN_TABLES = 0x0000.
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
				(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
				(m_paPpuAddrV.s.ui16FineY) +
				8) );
		}

		/**
		 * The second of the 2 MSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadMsb_1_Work() {
			ShiftBackgroundRegisters();
			RenderPixel();
			m_ui8NextTileMsb = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadNt_0_Work() {
			m_ui8OamAddr = 0;
			// LSN_PPU_NAMETABLES = 0x2000.
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF) );
		}

		/**
		 * The second of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadNt_1_Work() {
			m_ui8OamAddr = 0;
			m_ui8NextTileId = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 AT read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadAt_0_Work() {
			m_ui8OamAddr = 0;
			// LSN_PPU_NAMETABLES = 0x2000.
			// LSN_PPU_ATTRIBUTE_TABLE_OFFSET = 0x03C0.
			m_ui8NtAtBuffer = m_bBus.Read( (LSN_PPU_NAMETABLES + LSN_PPU_ATTRIBUTE_TABLE_OFFSET) | (m_paPpuAddrV.s.ui16NametableY << 11) |
				(m_paPpuAddrV.s.ui16NametableX << 10) |
				((m_paPpuAddrV.s.ui16CourseY >> 2) << 3) |
				(m_paPpuAddrV.s.ui16CourseX >> 2) );
		}

		/**
		 * The second of the 2 AT read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadAt_1_Work() {
			m_ui8OamAddr = 0;
			m_ui8NextTileAttribute = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 LSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadLsb_0_Work() {
			m_ui8OamAddr = 0;
			// LSN_PPU_PATTERN_TABLES = 0x0000.
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
				(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
				(m_paPpuAddrV.s.ui16FineY) +
				0) );
		}

		/**
		 * The second of the 2 LSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadLsb_1_Work() {
			m_ui8OamAddr = 0;
			m_ui8NextTileLsb = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 MSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadMsb_0_Work() {
			m_ui8OamAddr = 0;
			// LSN_PPU_PATTERN_TABLES = 0x0000.
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
				(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
				(m_paPpuAddrV.s.ui16FineY) +
				8) );
		}

		/**
		 * The second of the 2 MSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_GarbageLoadMsb_1_Work() {
			m_ui8OamAddr = 0;
			m_ui8NextTileMsb = m_ui8NtAtBuffer;
		}

		/**
		 * The first cycle in clearing secondary OAM.  Reads from the main OAM (which returns 0xFF during this period) and latches the value.
		 */
		void LSN_FASTCALL								Pixel_ClearOam2_0_Sprite() {
			m_ui8OamLatch = m_pbBus->Read( LSN_PR_OAMDATA );
		}

		/**
		 * The second cycle in clearing secondary OAM.  Copies the latched value read from OAM (0x2004) to the current index in the secondary OAM buffer.
		 */
		void LSN_FASTCALL								Pixel_ClearOam2_1_Sprite() {
			m_soSecondaryOam.ui8Bytes[m_ui8Oam2ClearIdx++] = m_ui8OamLatch;
			m_ui8Oam2ClearIdx %= sizeof( m_soSecondaryOam.ui8Bytes );
		}

		/**
		 * Handles populating the secondary OAM buffer during cycles 65-256.
		 */
		template <bool _bIsFirst>
		void LSN_FASTCALL								Pixel_Evaluation_Sprite() {
		}

		/**
		 * Incrementing the X scroll register.
		 */
		void LSN_FASTCALL								Pixel_IncScrollX_Control() {
			if ( Rendering() ) {
				// m_paPpuAddrV.s.ui16CourseX is only 5 bits so wrapping is automatic with the increment.  Check if the nametable needs to be swapped before the increment.
				if ( m_paPpuAddrV.s.ui16CourseX == 31 ) {
					// m_paPpuAddrV.s.ui16NametableX is 1 bit, so just toggle.
					m_paPpuAddrV.s.ui16NametableX = !m_paPpuAddrV.s.ui16NametableX;
				}
				++m_paPpuAddrV.s.ui16CourseX;
			}
			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * Incrementing the Y scroll register.  Also increments the X.
		 */
		void LSN_FASTCALL								Pixel_IncScrollY_Control() {
			if ( Rendering() ) {
				if ( m_paPpuAddrV.s.ui16FineY < 7 ) {
					++m_paPpuAddrV.s.ui16FineY;
				}
				else {
					m_paPpuAddrV.s.ui16FineY = 0;
					// Wrap and increment the course.
					// Do we need to swap vertical nametable targets?
					switch ( m_paPpuAddrV.s.ui16CourseY ) {
						case 29 : {
							// Wrap the course offset and flip the nametable bit.
							m_paPpuAddrV.s.ui16CourseY = 0;
							m_paPpuAddrV.s.ui16NametableY = ~m_paPpuAddrV.s.ui16NametableY;
							break;
						}
						case 31 : {
							// We are in attribute memory.  Reset but without flipping the nametable.
							m_paPpuAddrV.s.ui16CourseY = 0;
							break;
						}
						default : {
							// Somewhere between.  Just increment.
							++m_paPpuAddrV.s.ui16CourseY;
						}
					}
				}
			}
			Pixel_IncScrollX_Control();
		}

		/**
		 * Transfer the X values from the T pointer to the V pointer.
		 */
		void LSN_FASTCALL								Pixel_TransferX_Control() {
			if ( Rendering() ) {
				m_paPpuAddrV.s.ui16NametableX = m_paPpuAddrT.s.ui16NametableX;
				m_paPpuAddrV.s.ui16CourseX = m_paPpuAddrT.s.ui16CourseX;
			}
			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * Transfer the Y values from the T pointer to the V pointer.
		 */
		void LSN_FASTCALL								Pixel_TransferY_Control() {
			if ( Rendering() ) {
				m_paPpuAddrV.s.ui16FineY = m_paPpuAddrT.s.ui16FineY;
				m_paPpuAddrV.s.ui16NametableY = m_paPpuAddrT.s.ui16NametableY;
				m_paPpuAddrV.s.ui16CourseY = m_paPpuAddrT.s.ui16CourseY;
			}
			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * Determines if any rendering is taking place.
		 *
		 * \return Returns true if either the background or sprites are enabled, false otherwise.
		 */
		bool											Rendering() const { return m_pmPpuMask.s.ui8ShowBackground || m_pmPpuMask.s.ui8ShowSprites; }

		/**
		 * Writing to 0x2000 (PPUCTRL).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			ppPpu->m_ui8IoBusFloater = ppPpu->m_pcPpuCtrl.ui8Reg = _ui8Val;
			ppPpu->m_paPpuAddrT.s.ui16NametableX = LSN_CTRL_NAMETABLE_X( ppPpu->m_pcPpuCtrl );
			ppPpu->m_paPpuAddrT.s.ui16NametableY = LSN_CTRL_NAMETABLE_Y( ppPpu->m_pcPpuCtrl );
		}

		/**
		 * Writing to 0x2001 (PPUMASK).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2001( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			ppPpu->m_ui8IoBusFloater = ppPpu->m_pmPpuMask.ui8Reg = _ui8Val;
		}

		/**
		 * Reading from 0x2002 (PPUMASK) (resets the v-blank flag).
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
		 * Writing to 0x2002 (PPUSTATUS).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2002( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Only the top 3 bits can be modified on the floating bus.
			ppPpu->m_ui8IoBusFloater = (ppPpu->m_ui8IoBusFloater & 0x1F) | (_ui8Val & 0xE0);
		}

		/**
		 * Writing to 0x2003 (OAMADDR).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2003( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			ppPpu->m_ui8IoBusFloater = ppPpu->m_ui8OamAddr = _ui8Val;
		}

		/**
		 * Reading from 0x2004 (OAMDATA).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read2004( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			if ( (ppPpu->m_ui16Scanline < (_tPreRender + _tRender)) || ppPpu->m_ui16Scanline == (_tDotHeight - 1) ) {
				if ( ppPpu->m_ui16RowDot >= 1 && ppPpu->m_ui16RowDot <= 64 ) {
					_ui8Ret = 0xFF;
					return;
				} 
			}
			_ui8Ret = ppPpu->m_ui8IoBusFloater;
			ppPpu->m_ui8IoBusFloater = ppPpu->m_oOam.ui8Bytes[ppPpu->m_ui8OamAddr];
			//_ui8Ret = ppPpu->m_oOam.ui8Bytes[ppPpu->m_ui8OamAddr];
		}

		/**
		 * Writing to 0x2004 (OAMDATA).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2004( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			ppPpu->m_ui8IoBusFloater = ppPpu->m_oOam.ui8Bytes[ppPpu->m_ui8OamAddr++] = _ui8Val;
		}

		/**
		 * Writing to 0x2005 (PPUSCROLL).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2005( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			ppPpu->m_ui8IoBusFloater = _ui8Val;
			if ( !ppPpu->m_bAddresLatch ) {
				ppPpu->m_ui8FineScrollX = _ui8Val & 0x7;
				ppPpu->m_paPpuAddrT.s.ui16CourseX = _ui8Val >> 3;
			}
			else {
				ppPpu->m_paPpuAddrT.s.ui16FineY = _ui8Val & 0x7;
				ppPpu->m_paPpuAddrT.s.ui16CourseY = _ui8Val >> 3;
			}
			ppPpu->m_bAddresLatch ^= 1;
		}

		/**
		 * Writing to 0x2006 (PPUADDR).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2006( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Write top 8 bits first.  Easily acheived by flipping the latch before writing.
			ppPpu->m_ui8IoBusFloater = _ui8Val;
			ppPpu->m_bAddresLatch ^= 1;
			ppPpu->m_paPpuAddrT.ui8Bytes[ppPpu->m_bAddresLatch] = _ui8Val;
			ppPpu->m_paPpuAddrT.ui16Addr &= (ppPpu->m_bBus.Size() - 1);
			if ( !ppPpu->m_bAddresLatch ) {
				// ppPpu->m_bAddresLatch was 1 when we came here, flipped at the start.  This is the 2nd write.
				ppPpu->m_paPpuAddrV.ui16Addr = ppPpu->m_paPpuAddrT.ui16Addr;
			}
		}

		/**
		 * Reading from 0x2007 (PPUDATA) (PPU bus memory/PPUDATA).
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
				_ui8Ret = ppPpu->m_ui8IoBusFloater;
				ppPpu->m_ui8IoBusFloater = ppPpu->m_bBus.Read( ui16Addr );
				/*_ui8Ret = ppPpu->m_bBus.GetFloat();
				ppPpu->m_bBus.Read( ui16Addr );*/
			}
			ppPpu->m_paPpuAddrV.ui16Addr = (ui16Addr + (ppPpu->m_pcPpuCtrl.s.ui8IncrementMode ? 32 : 1)) & (LSN_PPU_MEM_FULL_SIZE - 1);
		}

		/**
		 * Writing to 0x2007 (PPUDATA) (PPU bus memory/PPUDATA).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			uint16_t ui16Addr = ppPpu->m_paPpuAddrV.ui16Addr;
			ppPpu->m_bBus.Write( ui16Addr, _ui8Val );
			ppPpu->m_ui8IoBusFloater = _ui8Val;
			ppPpu->m_paPpuAddrV.ui16Addr = (ui16Addr + (ppPpu->m_pcPpuCtrl.s.ui8IncrementMode ? 32 : 1)) & (LSN_PPU_MEM_FULL_SIZE - 1);
		}

		/**
		 * Writing to the background index of the palettes ($3F04/$3F08/$3F0C/$3F10/$3F14/$3F18/$3F1C).  These address can contain the written data but also update $3F00.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						WritePaletteIdx4( void * /*_pvParm0*/, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			_pui8Data[_ui16Parm1] /*= _pui8Data[LSN_PPU_PALETTE_MEMORY]*/ = _ui8Val;
		}

		/**
		 * Reading from a write-only address (returns the IO bus contents and fills the bus with the contents at the address).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PpuNoRead( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			_ui8Ret = ppPpu->m_ui8IoBusFloater;

			//ppPpu->m_ui8IoBusFloater = _pui8Data[_ui16Parm1];
		}

		/**
		 * A function usable for addresses that can't be written on the PPU.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
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
					uint16_t							ui16CourseX				: 5;					/**< Course X scroll position. */
					uint16_t							ui16CourseY				: 5;					/**< Course Y scroll position. */
					uint16_t							ui16NametableX			: 1;					/**< Nametable X. */
					uint16_t							ui16NametableY			: 1;					/**< Nametable Y. */
					uint16_t							ui16FineY				: 3;					/**< Fine Y. */
				}										s;
			};
		};

		/** Cycle handlers (function pointers doing per-cycle/per-pixel work). */
		struct LSN_CYCLE {
			PfCycles									pfFunc;											/**< The function pointer doing the work on this pixel. */
		};

		/** OAM memory. */
		struct LSN_OAM {
			union {
				struct {
					uint8_t								ui8Y;											/**< Sprite Y position. */
					uint8_t								ui8Id;											/**< Pattern-memory tile ID. */
					uint8_t								ui8Flags;										/**< Flags indicating how to render the sprite. */
					uint8_t								ui8X;											/**< Sprite X position. */
				}										s[64];
				uint32_t								ui32Obj[64];
				uint8_t									ui8Bytes[256];
			};
		};

		/** Secondary OAM memory. */
		struct LSN_SECONDARY_OAM {
			union {
				struct {
					uint8_t								ui8Y;											/**< Sprite Y position. */
					uint8_t								ui8Id;											/**< Pattern-memory tile ID. */
					uint8_t								ui8Flags;										/**< Flags indicating how to render the sprite. */
					uint8_t								ui8X;											/**< Sprite X position. */
				}										s[8];
				uint32_t								ui32Obj[8];
				uint8_t									ui8Bytes[32];
			};
		};


		// == Members.
		LSN_PALETTE										m_pPalette;
		uint64_t										m_ui64Frame;									/**< The frame counter. */
		uint64_t										m_ui64Cycle;									/**< The cycle counter. */
		CCpuBus *										m_pbBus;										/**< Pointer to the bus. */
		CNmiable *										m_pnNmiTarget;									/**< The target object of NMI notifications. */
		LSN_CYCLE										m_cControlCycles[_tDotWidth*_tDotHeight];		/**< The per-pixel array of function pointers to do per-cycle control work.  Control work relates to setting flags and maintaining the register state, etc. */
		LSN_CYCLE										m_cWorkCycles[_tDotWidth*_tDotHeight];			/**< The per-pixel array of function pointers to do per-cycle rendering work.  Standard work cycles are used to fetch data and render the results. */
		LSN_CYCLE										m_cSpriteCycles[_tDotWidth*_tDotHeight];		/**< The per-pixel array of function pointers to do per-cycle sprite-handing work.  Sprite work cycles are used to fetch sprite data for rendering. */
		CPpuBus											m_bBus;											/**< The PPU's internal RAM. */
		LSN_OAM											m_oOam;											/**< OAM memory. */
		LSN_SECONDARY_OAM								m_soSecondaryOam;								/**< Secondary OAM used for rendering during a scanline. */
		LSN_PPUADDR										m_paPpuAddrT;									/**< The "t" PPUADDR register. */
		LSN_PPUADDR										m_paPpuAddrV;									/**< The "v" PPUADDR register. */
		LSN_PPUCTRL										m_pcPpuCtrl;									/**< The PPUCTRL register. */
		LSN_PPUMASK										m_pmPpuMask;									/**< The PPUMASK register. */
		LSN_PPUSTATUS									m_psPpuStatus;									/**< The PPUSTATUS register. */
		uint16_t										m_ui16Scanline;									/**< The scanline counter. */
		uint16_t										m_ui16RowDot;									/**< The horizontal counter. */
		uint16_t										m_ui16ShiftPatternLo;							/**< The 16-bit shifter for the pattern low bits. */
		uint16_t										m_ui16ShiftPatternHi;							/**< The 16-bit shifter for the pattern high bits. */
		uint16_t										m_ui16ShiftAttribLo;							/**< The 16-bit shifter for the attribute low bits. */
		uint16_t										m_ui16ShiftAttribHi;							/**< The 16-bit shifter for the attribute high bits. */
		uint8_t											m_ui8IoBusFloater;								/**< The I/O bus floater. */
		uint8_t											m_ui8FineScrollX;								/**< The fine X scroll position. */
		uint8_t											m_ui8NtAtBuffer;								/**< I guess the 2 cycles of the NT/AT load first store the value into a temprary and then into the latch (to later be masked out every 8th cycle)? */
		uint8_t											m_ui8OamAddr;									/**< OAM address. */
		uint8_t											m_ui8OamLatch;									/**< Holds temporary OAM data. */
		uint8_t											m_ui8Oam2ClearIdx;								/**< The index of the byte being cleared during the secondary OAM clear. */

		uint8_t											m_ui8NextTileId;								/**< The queued background tile ID during rendering. */
		uint8_t											m_ui8NextTileAttribute;							/**< The queued background tile attribute during rendering. */
		uint8_t											m_ui8NextTileLsb;								/**< The queued background tile LSB. */
		uint8_t											m_ui8NextTileMsb;								/**< The queued background tile MSB. */

		bool											m_bAddresLatch;									/**< The address latch. */


		// == Functions.
		/**
		 * Assigns the gather/render functions at a given X Y pixel location.
		 *
		 * \param _stX The X pixel location from which to begin assiging the gather/render functions.
		 * \param _stY The Y pixel location from which to begin assiging the gather/render functions.
		 * \param _bIncludeControls If true, control functions such as incrementing the horizontal and vertical addresses are added.
		 * \param _bDummy If true, functions to use the latched values are not added.
		 */
		void											AssignGatherRenderFuncs( size_t _stX, size_t _stY, bool _bIncludeControls, bool /*_bDummy*/ ) {
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_1_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadAt_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadAt_1_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadLsb_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadLsb_1_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadMsb_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_LoadMsb_1_Work;
			}
			if ( _bIncludeControls ) {
				_stX -= 8;
				{
					LSN_CYCLE & cThis = m_cControlCycles[_stY*_tDotWidth+(_stX+7)];
					cThis.pfFunc = &CPpu2C0X::Pixel_IncScrollX_Control;
				}
			}
		}

		/**
		 * Assigns the garbage gather/render functions at a given X Y pixel location.
		 *
		 * \param _stX The X pixel location from which to begin assiging the gather/render functions.
		 * \param _stY The Y pixel location from which to begin assiging the gather/render functions.
		 */
		void											AssignGarbageGatherRenderFuncs( size_t _stX, size_t _stY ) {
			//AssignGatherRenderFuncs( _stX, _stY, false, false );
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadNt_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadNt_1_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadAt_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadAt_1_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadLsb_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadLsb_1_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadMsb_0_Work;
			}
			{
				LSN_CYCLE & cThis = m_cWorkCycles[_stY*_tDotWidth+_stX++];
				cThis.pfFunc = &CPpu2C0X::Pixel_GarbageLoadMsb_1_Work;
			}
		}

		/**
		 * Applies the function pointers for a given render scanline.
		 *
		 * \param _ui16Scanline The scanline whose function pointers for standard rendering are to be set.
		 */
		void											ApplyStdRenderFunctionPointers( uint16_t _ui16Scanline ) {
#define LSN_LEFT				1
#define LSN_RIGHT				(_tRenderW + LSN_LEFT)
#define LSN_SPR_RIGHT			(LSN_RIGHT + 8 * 8)
			const auto Y = _ui16Scanline;
			for ( auto X = LSN_LEFT; X < LSN_RIGHT; X += 8 ) {
				AssignGatherRenderFuncs( X, Y, true, false );
			}
			// Garbage reads.
			for ( auto X = LSN_RIGHT; X < LSN_SPR_RIGHT; X += 8 ) {
				AssignGarbageGatherRenderFuncs( X, Y );
			}
			// Dummy reads.
			for ( auto X = LSN_SPR_RIGHT; X < (_tDotWidth - 4); X += 8 ) {
				AssignGatherRenderFuncs( X, Y, false, true );
			}
			{	// Unused NT fetches.
				{
					LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-4)];
					cThis.pfFunc = &CPpu2C0X::Pixel_LoadNtNoShift_0_Work;
				}
				{
					LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-3)];
					cThis.pfFunc = &CPpu2C0X::Pixel_LoadNtNoShift_1_Work;
				}
				{
					LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-2)];
					cThis.pfFunc = &CPpu2C0X::Pixel_LoadNtNoShift_0_Work;
				}
				{
					LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-1)];
					cThis.pfFunc = &CPpu2C0X::Pixel_LoadNtNoShift_1_Work;
				}
			}
			{	// Increase vertical.
				LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+256];
				cThis.pfFunc = &CPpu2C0X::Pixel_IncScrollY_Control;
			}
			{	// Transfer horizontal.
				LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+257];
				cThis.pfFunc = &CPpu2C0X::Pixel_TransferX_Control;
			}
			AssignGatherRenderFuncs( LSN_SPR_RIGHT, Y, true, false );
			AssignGatherRenderFuncs( LSN_SPR_RIGHT + 8, Y, true, false );

			if ( _tDotHeight - 1 == Y ) {
				for ( auto X = 280; X <= 304; ++X ) {
					{	// Transfer vertical.
						LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+X];
						cThis.pfFunc = &CPpu2C0X::Pixel_TransferY_Control;
					}
				}
			}

#undef LSN_SPR_RIGHT
#undef LSN_RIGHT
#undef LSN_LEFT
		}

		/**
		 * Applies the sprite-shepherding function pointers for a given render scanline.
		 *
		 * \param _ui16Scanline The scanline whose function pointers for sprite-handling are to be set.
		 */
		void											ApplySpriteFunctionPointers( uint16_t _ui16Scanline ) {
#define LSN_LEFT				1
			const auto Y = _ui16Scanline;
			// Clear secondary OAM.
			for ( auto X = LSN_LEFT; X < (LSN_LEFT + 8 * 8); X += 2 ) {
				{	// Latch data.
					LSN_CYCLE & cThis = m_cSpriteCycles[Y*_tDotWidth+(X+0)];
					cThis.pfFunc = &CPpu2C0X::Pixel_ClearOam2_0_Sprite;
				}
				{	// Write data.
					LSN_CYCLE & cThis = m_cSpriteCycles[Y*_tDotWidth+(X+1)];
					cThis.pfFunc = &CPpu2C0X::Pixel_ClearOam2_1_Sprite;
				}
			}

#undef LSN_LEFT
		}

		/**
		 * Shifts the background registers left one.
		 */
		void											ShiftBackgroundRegisters() {
			if ( Rendering() ) {
				m_ui16ShiftPatternLo <<= 1;
				m_ui16ShiftPatternHi <<= 1;
				m_ui16ShiftAttribLo <<= 1;
				m_ui16ShiftAttribHi <<= 1;
			}
		}

		/**
		 * Loads the latched tile data into the shift registers.
		 */
		void											LoadLatchedBackgroundIntoShiftRegisters() {
			//if ( m_ui16RowDot >= 9 && m_ui16RowDot <= 257 ) {
				m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
				m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

				m_ui16ShiftAttribLo  = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
				m_ui16ShiftAttribHi  = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
			//}
		}

		/**
		 * Given an X and a Y scanline/row-cycle combination, determine if the pixel is represents can be output to the render target.
		 *
		 * \param _ui16X The X (row cycle).
		 * \param _ui16Y The Y (scanline).
		 * \param _ui16RtX The resulting render-target X if true is returned.
		 * \param _ui16RtY The resulting render-target Y if true is returned.
		 * \return Returns true if the given X and Y are contained within the render target
		 */
		inline bool										CycleToRenderTarget( uint16_t _ui16X, uint16_t _ui16Y, uint16_t &_ui16RtX, uint16_t &_ui16RtY ) {
			if ( _ui16X >= 1 && _ui16X < (_tRenderW + 1) ) {
				if ( _ui16Y < (_tPreRender + _tRender) ) {
					_ui16RtX = _ui16X - 1;
					_ui16RtY = (_tPreRender + _tRender - 1) - _ui16Y;
					return true;
				}
			}
			return false;
		}

		/**
		 * Renders a pixel based on the current state of the PPU.
		 */
		inline void										RenderPixel() {
			uint16_t ui16X, ui16Y;
			if ( CycleToRenderTarget( m_ui16RowDot, m_ui16Scanline, ui16X, ui16Y ) && m_pui8RenderTarget ) {
				uint8_t * pui8RenderPixel = &m_pui8RenderTarget[ui16Y*m_stRenderTargetStride+ui16X*3];
				uint8_t ui8Val = 0x0F;
				if ( ui16Y >= _tRender ) {}												// Black pre-render scanline on PAL.
				else if ( ui16X < _tBorderW || ui16X >= (_tRenderW - _tBorderW) ) {}		// Horizontal black border on PAL.
				else {
					uint8_t ui8BackgroundPixel = 0;
					uint8_t ui8BackgroundPalette = 0;
					if ( m_pmPpuMask.s.ui8ShowBackground && (m_pmPpuMask.s.ui8LeftBackground || m_ui16RowDot >= 9) ) {
				
						const uint16_t ui16Bit = 0x8000 >> m_ui8FineScrollX;
						ui8BackgroundPixel = (((m_ui16ShiftPatternHi & ui16Bit) > 0) << 1) |
							((m_ui16ShiftPatternLo & ui16Bit) > 0);
						ui8BackgroundPalette = (((m_ui16ShiftAttribHi & ui16Bit) > 0) << 1) |
							((m_ui16ShiftAttribLo & ui16Bit) > 0);
					}

					ui8Val = m_bBus.Read( 0x3F00 + (ui8BackgroundPalette << 2) | ui8BackgroundPixel ) & 0x3F;
//#define LSN_SHOW_PIXEL
#ifdef LSN_SHOW_PIXEL
					ui8Val = ui8BackgroundPixel * (255 / 4);// + ui8BackgroundPixel * 10;	// TMP
#endif	// #ifdef LSN_SHOW_PIXEL
				}
					
#ifdef LSN_SHOW_PIXEL
				pui8RenderPixel[0] = ui8Val;
				pui8RenderPixel[1] = ui8Val;
				pui8RenderPixel[2] = ui8Val;
#else
				pui8RenderPixel[0] = m_pPalette.uVals[ui8Val].ui8Rgb[0];
				pui8RenderPixel[1] = m_pPalette.uVals[ui8Val].ui8Rgb[1];
				pui8RenderPixel[2] = m_pPalette.uVals[ui8Val].ui8Rgb[2];
#endif	// #ifdef LSN_SHOW_PIXEL
				
				//ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F
			}
		}
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
#define LSN_PPU_TYPE( REGION )							LSN_PM_ ## REGION ## _DOTS_X, LSN_PM_ ## REGION ## _SCANLINES,														\
														LSN_PM_ ## REGION ## _PRERENDER, LSN_PM_ ## REGION ## _RENDER_LINES, LSN_PM_ ## REGION ## _POSTRENDER_LINES,		\
														LSN_PM_ ## REGION ## _VBLANK_LINES, LSN_PM_ ## REGION ## _POSTBLANK_LINES,											\
														LSN_PM_ ## REGION ## _RENDER_WIDTH, LSN_PM_ ## REGION ## _H_BORDER
	/**
	 * An NTSC PPU.
	 */
	typedef CPpu2C0X<LSN_PPU_TYPE( NTSC ), true, 1.143>													CNtscPpu;

	/**
	 * A PAL PPU.
	 */
	typedef CPpu2C0X<LSN_PPU_TYPE( PAL ), false, 1.386>													CPalPpu;

	/**
	 * A Dendy PPU.
	 */
	typedef CPpu2C0X<LSN_PPU_TYPE( DENDY ), false, 1.143>												CDendyPpu;

#undef LSN_PPU_TYPE


}	// namespace lsn

#undef LSN_CTRL_NAMETABLE_X
#undef LSN_CTRL_NAMETABLE_Y
