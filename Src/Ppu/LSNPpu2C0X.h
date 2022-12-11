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
#include "../System/LSNDmaSource.h"
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
		unsigned _tRenderW,
		bool _bOddFrameShenanigans>
	class CPpu2C0X : public CTickable {
	public :
		CPpu2C0X( CCpuBus * _pbBus, CNmiable * _pnNmiTarget, CDmaSource * _pdsDmaSrc ) :
			m_pbBus( _pbBus ),
			m_pnNmiTarget( _pnNmiTarget ),
			m_pdsDmaSrc( _pdsDmaSrc ),
			m_ui64Frame( 0 ),
			m_ui64Cycle( 0 ),
			m_ui16Scanline( 0 ),
			m_ui16RowDot( 0 ),
			m_ui8NtAtBuffer( 0 ),
			m_ui8NextTileId( 0 ),
			m_ui8NextTileAttribute( 0 ),
			m_ui8NextTileLsb( 0 ),
			m_ui8NextTileMsb( 0 ),
			m_ui8IoBusFloater( 0 ),
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
				}
			}
			// Add pixel gather/render functions.
			{
				// The main rendering area.
				for ( auto Y = _tPreRender; Y < (_tRender + _tPreRender); ++Y ) {
					for ( auto X = 1; X < (_tRenderW + 1); X += 8 ) {
						AssignGatherRenderFuncs( X, Y, true, false );
					}
					// Dummy reads.
					for ( auto X = (_tRenderW + 1); X < (_tDotWidth - 8); X += 8 ) {
						AssignGatherRenderFuncs( X, Y, false, true );
					}
					{	// Unused NT fetches.
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-4)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_0_Work;
						}
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-3)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_1_Work;
						}
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-2)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_0_Work;
						}
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-1)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_1_Work;
						}
					}
					{	// Transfer horizontal.
						LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+257];
						cThis.pfFunc = &CPpu2C0X::Pixel_TransferX_Control;
					}
					{	// Increase vertical.
						LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+256];
						cThis.pfFunc = &CPpu2C0X::Pixel_IncScrollY_Control;
					}
					AssignGatherRenderFuncs( 321, Y, true, false );
					AssignGatherRenderFuncs( 321 + 8, Y, true, false );
				}
				// The "-1" scanline.
				{
					constexpr size_t Y = _tDotHeight - 1;
					for ( auto X = 1; X < (_tRenderW + 1); X += 8 ) {
						AssignGatherRenderFuncs( X, Y, true, false );
					}
					// Dummy reads.
					for ( auto X = (_tRenderW + 1); X < (_tDotWidth - 8); X += 8 ) {
						AssignGatherRenderFuncs( X, Y, false, true );
					}
					{	// Unused NT fetches.
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-4)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_0_Work;
						}
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-3)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_1_Work;
						}
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-2)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_0_Work;
						}
						{
							LSN_CYCLE & cThis = m_cWorkCycles[Y*_tDotWidth+(_tDotWidth-1)];
							cThis.pfFunc = &CPpu2C0X::Pixel_LoadNt_1_Work;
						}
					}
					{	// Transfer horizontal.
						LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+257];
						cThis.pfFunc = &CPpu2C0X::Pixel_TransferX_Control;
					}
					{	// Increase vertical.
						LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+256];
						cThis.pfFunc = &CPpu2C0X::Pixel_IncScrollY_Control;
					}
					AssignGatherRenderFuncs( 321, Y, true, false );
					AssignGatherRenderFuncs( 321 + 8, Y, true, false );
					for ( auto X = 280; X <= 304; ++X ) {
						{	// Transfer vertical.
							LSN_CYCLE & cThis = m_cControlCycles[Y*_tDotWidth+257];
							cThis.pfFunc = &CPpu2C0X::Pixel_TransferY_Control;
						}
					}
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
		}
		~CPpu2C0X() {
		}


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
			size_t stIdx = m_ui16Scanline * _tDotWidth + m_ui16RowDot;
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
		}

		/**
		 * Applies the PPU's memory mapping t the bus.
		 */
		void											ApplyMemoryMap() {
			if ( m_pdsDmaSrc ) {
				m_pdsDmaSrc->SetDmaTarget( m_oOam.ui8Bytes );
			}

			// == Pattern Tables
			for ( uint32_t I = LSN_PPU_PATTERN_TABLES; I < LSN_PPU_NAMETABLES; ++I ) {
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
				// Default to ROM.  Allow cartridges to udpate the write pointers to make it RAM.
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::NoWrite, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
			}

			// == Nametables
			ApplyVerticalMirroring();

			// == Palettes
			for ( uint32_t I = LSN_PPU_PALETTE_MEMORY; I < LSN_PPU_MEM_FULL_SIZE; ++I ) {
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % LSN_PPU_PALETTE_MEMORY_SIZE) + LSN_PPU_PALETTE_MEMORY ) );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % LSN_PPU_PALETTE_MEMORY_SIZE) + LSN_PPU_PALETTE_MEMORY ) );
			}
			// 4th color of each entry mirrors the background color at LSN_PPU_PALETTE_MEMORY.
			for ( uint32_t I = LSN_PPU_PALETTE_MEMORY + 4; I < LSN_PPU_MEM_FULL_SIZE; I += 4 ) {
				m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % (LSN_PPU_PALETTE_MEMORY_SIZE / 2)) + LSN_PPU_PALETTE_MEMORY ) );
				m_bBus.SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % (LSN_PPU_PALETTE_MEMORY_SIZE / 2)) + LSN_PPU_PALETTE_MEMORY ) );
				/*m_bBus.SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( LSN_PPU_PALETTE_MEMORY ) );
				m_bBus.SetWriteFunc( uint16_t( I ), WritePaletteIdx4, this, uint16_t( ((I - LSN_PPU_PALETTE_MEMORY) % (LSN_PPU_PALETTE_MEMORY_SIZE / 2)) + LSN_PPU_PALETTE_MEMORY ) );*/
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
			m_pbBus->SetWriteFunc( 0x2005, Write2005, this, 0 );

			// 0x2006: PPUADDR.
			m_pbBus->SetReadFunc( 0x2006, PpuNoRead, this, 0x2006 );
			m_pbBus->SetWriteFunc( 0x2006, Write2006, this, 0 );

			// 0x2007: PPUDATA.
			m_pbBus->SetReadFunc( 0x2007, Read2007, this, 0 );
			m_pbBus->SetWriteFunc( 0x2007, Write2007, this, 0 );
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
		void LSN_FASTCALL								Pixel_Idle_Control() {
			LSN_END_CONTROL_CYCLE;
		}

		/**
		 * An "idle" work pixel handler.  Does nothing.
		 */
		void LSN_FASTCALL								Pixel_Idle_Work() {
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
		 * The first of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadNt_0_Work() {
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF) );
		}

		/**
		 * The second of the 2 NT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadNt_1_Work() {
			m_ui8NextTileId = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 AT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadAt_0_Work() {
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_ATTRIBUTE_TABLE_OFFSET | (m_paPpuAddrV.s.ui16NametableY << 11) |
				(m_paPpuAddrV.s.ui16NametableX << 10) |
				((m_paPpuAddrV.s.ui16CourseY >> 2) << 3) |
				(m_paPpuAddrV.s.ui16CourseX >> 2) );
		}

		/**
		 * The second of the 2 AT read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadAt_1_Work() {
			m_ui8NextTileAttribute = m_ui8NtAtBuffer;
			if ( m_paPpuAddrV.s.ui16CourseY & 0x2 ) { m_ui8NextTileAttribute >>= 4; }
			if ( m_paPpuAddrV.s.ui16CourseX & 0x2 ) { m_ui8NextTileAttribute >>= 2; }
			m_ui8NextTileAttribute &= 0x3;
		}

		/**
		 * The first of the 2 LSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadLsb_0_Work() {
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
				(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
				(m_paPpuAddrV.s.ui16FineY) +
				0) );
		}

		/**
		 * The second of the 2 LSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadLsb_1_Work() {
			m_ui8NextTileLsb = m_ui8NtAtBuffer;
		}

		/**
		 * The first of the 2 MSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadMsb_0_Work() {
			m_ui8NtAtBuffer = m_bBus.Read( LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
				(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
				(m_paPpuAddrV.s.ui16FineY) +
				8) );
		}

		/**
		 * The second of the 2 MSB read cycles.
		 */
		void LSN_FASTCALL								Pixel_LoadMsb_1_Work() {
			m_ui8NextTileMsb = m_ui8NtAtBuffer;
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
		 * Writing to 0x2000.
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
		 * Writing to 0x2001.
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
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write2002( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CPpu2C0X * ppPpu = reinterpret_cast<CPpu2C0X *>(_pvParm0);
			// Only the top 3 bits can be modified on the floating bus.
			ppPpu->m_ui8IoBusFloater = (ppPpu->m_ui8IoBusFloater & 0x1F) | (_ui8Val & 0xE0);
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
			ppPpu->m_bAddresLatch ^= 1;
			ppPpu->m_paPpuAddrT.ui8Bytes[ppPpu->m_bAddresLatch] = _ui8Val;
			ppPpu->m_paPpuAddrT.ui16Addr &= (ppPpu->m_bBus.Size() - 1);
			if ( !ppPpu->m_bAddresLatch ) {
				// ppPpu->m_bAddresLatch was 1 when we came here, flipped at the start.  This is the 2nd write.
				ppPpu->m_paPpuAddrV.ui16Addr = ppPpu->m_paPpuAddrT.ui16Addr;
			}
		}

		/**
		 * Reading from 0x2007 (PPU bus memory/PPUDATA).
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
		 * Writing to 0x2007 (PPU bus memory/PPUDATA).
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
			_pui8Data[_ui16Parm1] = /*_pui8Data[LSN_PPU_PALETTE_MEMORY] = */_ui8Val;
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
				uint32_t								ui32Obj[64];
				uint8_t									ui8Bytes[256];
			};
		};


		// == Members.
		uint64_t										m_ui64Frame;									/**< The frame counter. */
		uint64_t										m_ui64Cycle;									/**< The cycle counter. */
		CCpuBus *										m_pbBus;										/**< Pointer to the bus. */
		CDmaSource *									m_pdsDmaSrc;									/**< The DMA source object.  Gets told where to send DMA transfers. */
		CNmiable *										m_pnNmiTarget;									/**< The target object of NMI notifications. */
		LSN_CYCLE										m_cControlCycles[_tDotWidth*_tDotHeight];		/**< The per-pixel array of function pointers to do per-cycle control work.  Control work relates to setting flags and maintaining the register state, etc. */
		LSN_CYCLE										m_cWorkCycles[_tDotWidth*_tDotHeight];			/** The per-pixel array of function pointers to do per-cycle rendering work.  Standard work cycles are used to fetch data and render the results. */
		CPpuBus											m_bBus;											/**< The PPU's internal RAM. */
		LSN_OAM											m_oOam;											/**< OAM memory. */
		uint16_t										m_ui16Scanline;									/**< The scanline counter. */
		uint16_t										m_ui16RowDot;									/**< The horizontal counter. */
		LSN_PPUADDR										m_paPpuAddrT;									/**< The "t" PPUADDR register. */
		LSN_PPUADDR										m_paPpuAddrV;									/**< The "v" PPUADDR register. */
		LSN_PPUCTRL										m_pcPpuCtrl;									/**< The PPUCTRL register. */
		LSN_PPUMASK										m_pmPpuMask;									/**< The PPUMASK register. */
		LSN_PPUSTATUS									m_psPpuStatus;									/**< The PPUSTATUS register. */		
		uint8_t											m_ui8IoBusFloater;								/**< The I/O bus floater. */
		uint8_t											m_ui8FineScrollX;								/**< The fine X scroll position. */
		uint8_t											m_ui8NtAtBuffer;								/**< I guess the 2 cycles of the NT/AT load first store the value into a temprary and then into the latch (to later be masked out every 8th cycle)? */

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
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
#define LSN_PPU_TYPE( REGION )							LSN_PM_ ## REGION ## _DOTS_X, LSN_PM_ ## REGION ## _SCANLINES, LSN_PM_ ## REGION ## _PRERENDER, LSN_PM_ ## REGION ## _RENDER_LINES, LSN_PM_ ## REGION ## _POSTRENDER_LINES, LSN_PM_ ## REGION ## _VBLANK_LINES, LSN_PM_ ## REGION ## _POSTBLANK_LINES, LSN_PM_ ## REGION ## _RENDER_WIDTH
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
