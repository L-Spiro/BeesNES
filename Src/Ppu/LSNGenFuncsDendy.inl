void LSN_FASTCALL								Cycle_2__0x0_338x0_340x0_0x1_338x1_340x1_0x2_338x2_340x2_0x3_X() {

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__340x311() {

	++m_stCurCycle;

	if constexpr (_bOddFrameShenanigans) {
		if (m_ui64Frame & 0x1) {
			m_stCurCycle = 1;
		}
		else {
			m_stCurCycle = 0;
		}
	}
	else {
		m_stCurCycle = 0;
	}
	++m_ui64Frame;
}


void LSN_FASTCALL								Cycle_2__337x0_339x0_337x1_339x1_337x2_339x2_337x3_339x3_337x4_339x4_X() {

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__1x291() {

	// [1, 241] on NTSC.
	// [1, 241] on PAL.
	// [1, 241] on Dendy.
	m_psPpuStatus.s.ui8VBlank = 1;
	if (m_pcPpuCtrl.s.ui8Nmi) {
		m_pnNmiTarget->Nmi();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__68x0_76x0_84x0_92x0_100x0_108x0_116x0_124x0_132x0_140x0_X() {

	Pixel_Evaluation_Sprite<false, false>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileAttribute = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__66x0_74x0_82x0_90x0_98x0_106x0_114x0_122x0_130x0_138x0_X() {

	Pixel_Evaluation_Sprite<false, false>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileId = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__70x0_78x0_86x0_94x0_102x0_110x0_118x0_126x0_134x0_142x0_X() {

	Pixel_Evaluation_Sprite<false, false>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileLsb = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__72x0_80x0_88x0_96x0_104x0_112x0_120x0_128x0_136x0_144x0_X() {

	Pixel_Evaluation_Sprite<false, false>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (m_bRendering) {
		IncHorizontal();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__256x0_256x1_256x2_256x3_256x4_256x5_256x6_256x7_256x8_256x9_X() {

	Pixel_Evaluation_Sprite<false, false>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (m_bRendering) {
		IncHorizontal();
	}

	// Increase v.V.
	if (m_bRendering) {
		IncVertical();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__73x0_81x0_89x0_97x0_105x0_113x0_121x0_129x0_137x0_145x0_X() {

	Pixel_Evaluation_Sprite<false, true>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;

		m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
		m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

		m_ui16ShiftAttribLo = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
		m_ui16ShiftAttribHi = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
	}

	RenderPixel();

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__67x0_75x0_83x0_91x0_99x0_107x0_115x0_123x0_131x0_139x0_X() {

	Pixel_Evaluation_Sprite<false, true>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	// LSN_PPU_NAMETABLES = 0x2000.
	// LSN_PPU_ATTRIBUTE_TABLE_OFFSET = 0x03C0.
	m_ui8NtAtBuffer = m_bBus.Read((LSN_PPU_NAMETABLES + LSN_PPU_ATTRIBUTE_TABLE_OFFSET) | (m_paPpuAddrV.s.ui16NametableY << 11) |
		(m_paPpuAddrV.s.ui16NametableX << 10) |
		((m_paPpuAddrV.s.ui16CourseY >> 2) << 3) |
		(m_paPpuAddrV.s.ui16CourseX >> 2));
	if (m_paPpuAddrV.s.ui16CourseY & 0x2) { m_ui8NtAtBuffer >>= 4; }
	if (m_paPpuAddrV.s.ui16CourseX & 0x2) { m_ui8NtAtBuffer >>= 2; }
	m_ui8NtAtBuffer &= 0x3;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__69x0_77x0_85x0_93x0_101x0_109x0_117x0_125x0_133x0_141x0_X() {

	Pixel_Evaluation_Sprite<false, true>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	// LSN_PPU_PATTERN_TABLES = 0x0000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
		(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
		(m_paPpuAddrV.s.ui16FineY) +
		0));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__71x0_79x0_87x0_95x0_103x0_111x0_119x0_127x0_135x0_143x0_X() {

	Pixel_Evaluation_Sprite<false, true>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	// LSN_PPU_PATTERN_TABLES = 0x0000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
		(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
		(m_paPpuAddrV.s.ui16FineY) +
		8));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__65x0_65x1_65x2_65x3_65x4_65x5_65x6_65x7_65x8_65x9_X() {

	Pixel_Evaluation_Sprite<true, true>();

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;

		m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
		m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

		m_ui16ShiftAttribLo = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
		m_ui16ShiftAttribHi = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
	}

	RenderPixel();

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__257x0_257x1_257x2_257x3_257x4_257x5_257x6_257x7_257x8_257x9_X() {

	Pixel_Fetch_Sprite<0, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	if (m_bRendering) {
		m_paPpuAddrV.s.ui16NametableX = m_paPpuAddrT.s.ui16NametableX;
		m_paPpuAddrV.s.ui16CourseX = m_paPpuAddrT.s.ui16CourseX;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__258x0_258x1_258x2_258x3_258x4_258x5_258x6_258x7_258x8_258x9_X() {

	Pixel_Fetch_Sprite<0, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__259x0_259x1_259x2_259x3_259x4_259x5_259x6_259x7_259x8_259x9_X() {

	Pixel_Fetch_Sprite<0, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__260x0_260x1_260x2_260x3_260x4_260x5_260x6_260x7_260x8_260x9_X() {

	Pixel_Fetch_Sprite<0, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__261x0_261x1_261x2_261x3_261x4_261x5_261x6_261x7_261x8_261x9_X() {

	Pixel_Fetch_Sprite<0, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__262x0_262x1_262x2_262x3_262x4_262x5_262x6_262x7_262x8_262x9_X() {

	Pixel_Fetch_Sprite<0, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__263x0_263x1_263x2_263x3_263x4_263x5_263x6_263x7_263x8_263x9_X() {

	Pixel_Fetch_Sprite<0, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__264x0_264x1_264x2_264x3_264x4_264x5_264x6_264x7_264x8_264x9_X() {

	Pixel_Fetch_Sprite<0, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__265x0_265x1_265x2_265x3_265x4_265x5_265x6_265x7_265x8_265x9_X() {

	Pixel_Fetch_Sprite<1, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__266x0_266x1_266x2_266x3_266x4_266x5_266x6_266x7_266x8_266x9_X() {

	Pixel_Fetch_Sprite<1, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__267x0_267x1_267x2_267x3_267x4_267x5_267x6_267x7_267x8_267x9_X() {

	Pixel_Fetch_Sprite<1, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__268x0_268x1_268x2_268x3_268x4_268x5_268x6_268x7_268x8_268x9_X() {

	Pixel_Fetch_Sprite<1, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__269x0_269x1_269x2_269x3_269x4_269x5_269x6_269x7_269x8_269x9_X() {

	Pixel_Fetch_Sprite<1, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__270x0_270x1_270x2_270x3_270x4_270x5_270x6_270x7_270x8_270x9_X() {

	Pixel_Fetch_Sprite<1, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__271x0_271x1_271x2_271x3_271x4_271x5_271x6_271x7_271x8_271x9_X() {

	Pixel_Fetch_Sprite<1, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__272x0_272x1_272x2_272x3_272x4_272x5_272x6_272x7_272x8_272x9_X() {

	Pixel_Fetch_Sprite<1, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__273x0_273x1_273x2_273x3_273x4_273x5_273x6_273x7_273x8_273x9_X() {

	Pixel_Fetch_Sprite<2, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__274x0_274x1_274x2_274x3_274x4_274x5_274x6_274x7_274x8_274x9_X() {

	Pixel_Fetch_Sprite<2, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__275x0_275x1_275x2_275x3_275x4_275x5_275x6_275x7_275x8_275x9_X() {

	Pixel_Fetch_Sprite<2, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__276x0_276x1_276x2_276x3_276x4_276x5_276x6_276x7_276x8_276x9_X() {

	Pixel_Fetch_Sprite<2, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__277x0_277x1_277x2_277x3_277x4_277x5_277x6_277x7_277x8_277x9_X() {

	Pixel_Fetch_Sprite<2, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__278x0_278x1_278x2_278x3_278x4_278x5_278x6_278x7_278x8_278x9_X() {

	Pixel_Fetch_Sprite<2, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__279x0_279x1_279x2_279x3_279x4_279x5_279x6_279x7_279x8_279x9_X() {

	Pixel_Fetch_Sprite<2, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__280x0_280x1_280x2_280x3_280x4_280x5_280x6_280x7_280x8_280x9_X() {

	Pixel_Fetch_Sprite<2, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__281x0_281x1_281x2_281x3_281x4_281x5_281x6_281x7_281x8_281x9_X() {

	Pixel_Fetch_Sprite<3, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__282x0_282x1_282x2_282x3_282x4_282x5_282x6_282x7_282x8_282x9_X() {

	Pixel_Fetch_Sprite<3, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__283x0_283x1_283x2_283x3_283x4_283x5_283x6_283x7_283x8_283x9_X() {

	Pixel_Fetch_Sprite<3, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__284x0_284x1_284x2_284x3_284x4_284x5_284x6_284x7_284x8_284x9_X() {

	Pixel_Fetch_Sprite<3, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__285x0_285x1_285x2_285x3_285x4_285x5_285x6_285x7_285x8_285x9_X() {

	Pixel_Fetch_Sprite<3, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__286x0_286x1_286x2_286x3_286x4_286x5_286x6_286x7_286x8_286x9_X() {

	Pixel_Fetch_Sprite<3, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__287x0_287x1_287x2_287x3_287x4_287x5_287x6_287x7_287x8_287x9_X() {

	Pixel_Fetch_Sprite<3, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__288x0_288x1_288x2_288x3_288x4_288x5_288x6_288x7_288x8_288x9_X() {

	Pixel_Fetch_Sprite<3, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__289x0_289x1_289x2_289x3_289x4_289x5_289x6_289x7_289x8_289x9_X() {

	Pixel_Fetch_Sprite<4, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__290x0_290x1_290x2_290x3_290x4_290x5_290x6_290x7_290x8_290x9_X() {

	Pixel_Fetch_Sprite<4, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__291x0_291x1_291x2_291x3_291x4_291x5_291x6_291x7_291x8_291x9_X() {

	Pixel_Fetch_Sprite<4, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__292x0_292x1_292x2_292x3_292x4_292x5_292x6_292x7_292x8_292x9_X() {

	Pixel_Fetch_Sprite<4, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__293x0_293x1_293x2_293x3_293x4_293x5_293x6_293x7_293x8_293x9_X() {

	Pixel_Fetch_Sprite<4, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__294x0_294x1_294x2_294x3_294x4_294x5_294x6_294x7_294x8_294x9_X() {

	Pixel_Fetch_Sprite<4, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__295x0_295x1_295x2_295x3_295x4_295x5_295x6_295x7_295x8_295x9_X() {

	Pixel_Fetch_Sprite<4, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__296x0_296x1_296x2_296x3_296x4_296x5_296x6_296x7_296x8_296x9_X() {

	Pixel_Fetch_Sprite<4, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__297x0_297x1_297x2_297x3_297x4_297x5_297x6_297x7_297x8_297x9_X() {

	Pixel_Fetch_Sprite<5, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__298x0_298x1_298x2_298x3_298x4_298x5_298x6_298x7_298x8_298x9_X() {

	Pixel_Fetch_Sprite<5, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__299x0_299x1_299x2_299x3_299x4_299x5_299x6_299x7_299x8_299x9_X() {

	Pixel_Fetch_Sprite<5, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__300x0_300x1_300x2_300x3_300x4_300x5_300x6_300x7_300x8_300x9_X() {

	Pixel_Fetch_Sprite<5, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__301x0_301x1_301x2_301x3_301x4_301x5_301x6_301x7_301x8_301x9_X() {

	Pixel_Fetch_Sprite<5, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__302x0_302x1_302x2_302x3_302x4_302x5_302x6_302x7_302x8_302x9_X() {

	Pixel_Fetch_Sprite<5, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__303x0_303x1_303x2_303x3_303x4_303x5_303x6_303x7_303x8_303x9_X() {

	Pixel_Fetch_Sprite<5, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__304x0_304x1_304x2_304x3_304x4_304x5_304x6_304x7_304x8_304x9_X() {

	Pixel_Fetch_Sprite<5, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__305x0_305x1_305x2_305x3_305x4_305x5_305x6_305x7_305x8_305x9_X() {

	Pixel_Fetch_Sprite<6, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__306x0_306x1_306x2_306x3_306x4_306x5_306x6_306x7_306x8_306x9_X() {

	Pixel_Fetch_Sprite<6, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__307x0_307x1_307x2_307x3_307x4_307x5_307x6_307x7_307x8_307x9_X() {

	Pixel_Fetch_Sprite<6, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__308x0_308x1_308x2_308x3_308x4_308x5_308x6_308x7_308x8_308x9_X() {

	Pixel_Fetch_Sprite<6, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__309x0_309x1_309x2_309x3_309x4_309x5_309x6_309x7_309x8_309x9_X() {

	Pixel_Fetch_Sprite<6, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__310x0_310x1_310x2_310x3_310x4_310x5_310x6_310x7_310x8_310x9_X() {

	Pixel_Fetch_Sprite<6, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__311x0_311x1_311x2_311x3_311x4_311x5_311x6_311x7_311x8_311x9_X() {

	Pixel_Fetch_Sprite<6, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__312x0_312x1_312x2_312x3_312x4_312x5_312x6_312x7_312x8_312x9_X() {

	Pixel_Fetch_Sprite<6, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__313x0_313x1_313x2_313x3_313x4_313x5_313x6_313x7_313x8_313x9_X() {

	Pixel_Fetch_Sprite<7, 0>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__314x0_314x1_314x2_314x3_314x4_314x5_314x6_314x7_314x8_314x9_X() {

	Pixel_Fetch_Sprite<7, 1>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__315x0_315x1_315x2_315x3_315x4_315x5_315x6_315x7_315x8_315x9_X() {

	Pixel_Fetch_Sprite<7, 2>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__316x0_316x1_316x2_316x3_316x4_316x5_316x6_316x7_316x8_316x9_X() {

	Pixel_Fetch_Sprite<7, 3>();	// Sprite fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__317x0_317x1_317x2_317x3_317x4_317x5_317x6_317x7_317x8_317x9_X() {

	Pixel_Fetch_Sprite<7, 4>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__318x0_318x1_318x2_318x3_318x4_318x5_318x6_318x7_318x8_318x9_X() {

	Pixel_Fetch_Sprite<7, 5>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__319x0_319x1_319x2_319x3_319x4_319x5_319x6_319x7_319x8_319x9_X() {

	Pixel_Fetch_Sprite<7, 6>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__320x0_320x1_320x2_320x3_320x4_320x5_320x6_320x7_320x8_320x9_X() {

	Pixel_Fetch_Sprite<7, 7>();	// Sprite fetches (257-320).

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__282x311_284x311_290x311_292x311_298x311_300x311() {

	if (m_bRendering) {
		m_paPpuAddrV.s.ui16FineY = m_paPpuAddrT.s.ui16FineY;
		m_paPpuAddrV.s.ui16NametableY = m_paPpuAddrT.s.ui16NametableY;
		m_paPpuAddrV.s.ui16CourseY = m_paPpuAddrT.s.ui16CourseY;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__321x0_329x0_321x1_329x1_321x2_329x2_321x3_329x3_321x4_329x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;

		m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
		m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

		m_ui16ShiftAttribLo = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
		m_ui16ShiftAttribHi = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
	}

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__1x311() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;

		m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
		m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

		m_ui16ShiftAttribLo = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
		m_ui16ShiftAttribHi = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
	}

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	m_psPpuStatus.s.ui8VBlank = 0;
	m_psPpuStatus.s.ui8SpriteOverflow = 0;
	m_psPpuStatus.s.ui8Sprite0Hit = 0;
	m_pnNmiTarget->ClearNmi();

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__323x0_331x0_323x1_331x1_323x2_331x2_323x3_331x3_323x4_331x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	// LSN_PPU_NAMETABLES = 0x2000.
	// LSN_PPU_ATTRIBUTE_TABLE_OFFSET = 0x03C0.
	m_ui8NtAtBuffer = m_bBus.Read((LSN_PPU_NAMETABLES + LSN_PPU_ATTRIBUTE_TABLE_OFFSET) | (m_paPpuAddrV.s.ui16NametableY << 11) |
		(m_paPpuAddrV.s.ui16NametableX << 10) |
		((m_paPpuAddrV.s.ui16CourseY >> 2) << 3) |
		(m_paPpuAddrV.s.ui16CourseX >> 2));
	if (m_paPpuAddrV.s.ui16CourseY & 0x2) { m_ui8NtAtBuffer >>= 4; }
	if (m_paPpuAddrV.s.ui16CourseX & 0x2) { m_ui8NtAtBuffer >>= 2; }
	m_ui8NtAtBuffer &= 0x3;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__325x0_333x0_325x1_333x1_325x2_333x2_325x3_333x3_325x4_333x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	// LSN_PPU_PATTERN_TABLES = 0x0000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
		(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
		(m_paPpuAddrV.s.ui16FineY) +
		0));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__327x0_335x0_327x1_335x1_327x2_335x2_327x3_335x3_327x4_335x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	// LSN_PPU_PATTERN_TABLES = 0x0000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
		(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
		(m_paPpuAddrV.s.ui16FineY) +
		8));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__324x0_332x0_324x1_332x1_324x2_332x2_324x3_332x3_324x4_332x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileAttribute = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__322x0_330x0_322x1_330x1_322x2_330x2_322x3_330x3_322x4_330x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileId = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__326x0_334x0_326x1_334x1_326x2_334x2_326x3_334x3_326x4_334x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileLsb = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__328x0_336x0_328x1_336x1_328x2_336x2_328x3_336x3_328x4_336x4_X() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (m_bRendering) {
		IncHorizontal();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__256x311() {

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (m_bRendering) {
		IncHorizontal();
	}

	// Increase v.V.
	if (m_bRendering) {
		IncVertical();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__257x240() {

	if (m_pdhHost) {
		if (m_pui8RenderTarget) {
			if (DebugSideDisplay()) {
				for (uint16_t I = 0; I < 2; ++I) {
					for (uint16_t ui16TileY = 0; ui16TileY < 16; ++ui16TileY) {
						for (uint16_t ui16TileX = 0; ui16TileX < 16; ++ui16TileX) {
							uint16_t ui16Offset = ui16TileY * 256 + ui16TileX * 16;
							for (uint16_t ui16Row = 0; ui16Row < 8; ++ui16Row) {
								uint8_t ui8TileLsb = m_bBus.Read(0x1000 * I + ui16Offset + ui16Row + 0);
								uint8_t ui8TileMsb = m_bBus.Read(0x1000 * I + ui16Offset + ui16Row + 8);
								for (uint16_t ui16Col = 0; ui16Col < 8; ++ui16Col) {
									uint8_t ui8Pixel = (ui8TileLsb & 0x01) + (ui8TileMsb & 0x01);
									ui8TileLsb >>= 1;
									ui8TileMsb >>= 1;
									uint16_t ui16X = ui16TileX * 8 + (7 - ui16Col);
									ui16X += _tRenderW;

									uint16_t ui16Y = I * (16 * 8) + ui16TileY * 8 + ui16Row;
									ui16Y = (_tRender - 1) - ui16Y;
									if (ui16Y < _tRender) {

										uint8_t* pui8This = &m_pui8RenderTarget[ui16Y * m_stRenderTargetStride + ui16X * 3];
										uint8_t ui8Val = ui8Pixel * (255 / 4);
										pui8This[0] = pui8This[1] = pui8This[2] = ui8Val;
									}
								}
							}
						}
					}
				}
			}
		}
		m_pdhHost->Swap();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__4x0_12x0_20x0_28x0_36x0_44x0_52x0_60x0_4x1_12x1_X() {

	m_soSecondaryOam.ui8Bytes[m_ui8Oam2ClearIdx++] = m_ui8OamLatch; m_ui8Oam2ClearIdx %= sizeof(m_soSecondaryOam.ui8Bytes);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileAttribute = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__2x0_10x0_18x0_26x0_34x0_42x0_50x0_58x0_2x1_10x1_X() {

	m_soSecondaryOam.ui8Bytes[m_ui8Oam2ClearIdx++] = m_ui8OamLatch; m_ui8Oam2ClearIdx %= sizeof(m_soSecondaryOam.ui8Bytes);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileId = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__6x0_14x0_22x0_30x0_38x0_46x0_54x0_62x0_6x1_14x1_X() {

	m_soSecondaryOam.ui8Bytes[m_ui8Oam2ClearIdx++] = m_ui8OamLatch; m_ui8Oam2ClearIdx %= sizeof(m_soSecondaryOam.ui8Bytes);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileLsb = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__8x0_16x0_24x0_32x0_40x0_48x0_56x0_64x0_8x1_16x1_X() {

	m_soSecondaryOam.ui8Bytes[m_ui8Oam2ClearIdx++] = m_ui8OamLatch; m_ui8Oam2ClearIdx %= sizeof(m_soSecondaryOam.ui8Bytes);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (m_bRendering) {
		IncHorizontal();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__261x311_262x311_263x311_264x311_269x311_270x311_271x311_272x311_277x311_278x311_X() {

	m_ui8OamAddr = 0;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__280x311_285x311_286x311_287x311_288x311_293x311_294x311_295x311_296x311_301x311_X() {

	m_ui8OamAddr = 0;

	if (m_bRendering) {
		m_paPpuAddrV.s.ui16FineY = m_paPpuAddrT.s.ui16FineY;
		m_paPpuAddrV.s.ui16NametableY = m_paPpuAddrT.s.ui16NametableY;
		m_paPpuAddrV.s.ui16CourseY = m_paPpuAddrT.s.ui16CourseY;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__259x311_265x311_267x311_273x311_275x311_305x311_307x311_313x311_315x311() {

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__281x311_283x311_289x311_291x311_297x311_299x311() {

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	if (m_bRendering) {
		m_paPpuAddrV.s.ui16FineY = m_paPpuAddrT.s.ui16FineY;
		m_paPpuAddrV.s.ui16NametableY = m_paPpuAddrT.s.ui16NametableY;
		m_paPpuAddrV.s.ui16CourseY = m_paPpuAddrT.s.ui16CourseY;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__257x311() {

	m_ui8OamAddr = 0;
	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));	// Garbage fetches (257-320).

	if (m_bRendering) {
		m_paPpuAddrV.s.ui16NametableX = m_paPpuAddrT.s.ui16NametableX;
		m_paPpuAddrV.s.ui16CourseX = m_paPpuAddrT.s.ui16CourseX;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__9x0_17x0_25x0_33x0_41x0_49x0_57x0_9x1_17x1_25x1_X() {

	m_ui8OamLatch = m_pbBus->Read(LSN_PR_OAMDATA);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;

		m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
		m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

		m_ui16ShiftAttribLo = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
		m_ui16ShiftAttribHi = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
	}

	RenderPixel();

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__3x0_11x0_19x0_27x0_35x0_43x0_51x0_59x0_3x1_11x1_X() {

	m_ui8OamLatch = m_pbBus->Read(LSN_PR_OAMDATA);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	// LSN_PPU_NAMETABLES = 0x2000.
	// LSN_PPU_ATTRIBUTE_TABLE_OFFSET = 0x03C0.
	m_ui8NtAtBuffer = m_bBus.Read((LSN_PPU_NAMETABLES + LSN_PPU_ATTRIBUTE_TABLE_OFFSET) | (m_paPpuAddrV.s.ui16NametableY << 11) |
		(m_paPpuAddrV.s.ui16NametableX << 10) |
		((m_paPpuAddrV.s.ui16CourseY >> 2) << 3) |
		(m_paPpuAddrV.s.ui16CourseX >> 2));
	if (m_paPpuAddrV.s.ui16CourseY & 0x2) { m_ui8NtAtBuffer >>= 4; }
	if (m_paPpuAddrV.s.ui16CourseX & 0x2) { m_ui8NtAtBuffer >>= 2; }
	m_ui8NtAtBuffer &= 0x3;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__5x0_13x0_21x0_29x0_37x0_45x0_53x0_61x0_5x1_13x1_X() {

	m_ui8OamLatch = m_pbBus->Read(LSN_PR_OAMDATA);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	// LSN_PPU_PATTERN_TABLES = 0x0000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
		(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
		(m_paPpuAddrV.s.ui16FineY) +
		0));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__7x0_15x0_23x0_31x0_39x0_47x0_55x0_63x0_7x1_15x1_X() {

	m_ui8OamLatch = m_pbBus->Read(LSN_PR_OAMDATA);

	if (m_bRendering) {
		for (uint8_t I = m_ui8ThisLineSpriteCount; I--; ) {
			if (m_asActiveSprites.ui8X[I]) {
				--m_asActiveSprites.ui8X[I];
			}
			else {
				m_asActiveSprites.ui8ShiftLo[I] <<= 1;
				m_asActiveSprites.ui8ShiftHi[I] <<= 1;
			}
		}
	}

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	// LSN_PPU_PATTERN_TABLES = 0x0000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_PATTERN_TABLES | ((m_pcPpuCtrl.s.ui8BackgroundTileSelect << 12) +
		(static_cast<uint16_t>(m_ui8NextTileId) << 4) +
		(m_paPpuAddrV.s.ui16FineY) +
		8));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__1x1_1x2_1x3_1x4_1x5_1x6_1x7_1x8_1x9_1x10_X() {

	m_ui8OamLatch = m_pbBus->Read(LSN_PR_OAMDATA);

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;

		m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
		m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

		m_ui16ShiftAttribLo = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
		m_ui16ShiftAttribHi = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
	}

	RenderPixel();

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__1x0() {

	m_ui8OamLatch = m_pbBus->Read(LSN_PR_OAMDATA);

	if (m_bRendering) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;

		m_ui16ShiftPatternLo = (m_ui16ShiftPatternLo & 0xFF00) | m_ui8NextTileLsb;
		m_ui16ShiftPatternHi = (m_ui16ShiftPatternHi & 0xFF00) | m_ui8NextTileMsb;

		m_ui16ShiftAttribLo = (m_ui16ShiftAttribLo & 0xFF00) | ((m_ui8NextTileAttribute & 0b01) ? 0xFF : 0x00);
		m_ui16ShiftAttribHi = (m_ui16ShiftAttribHi & 0xFF00) | ((m_ui8NextTileAttribute & 0b10) ? 0xFF : 0x00);
	}

	RenderPixel();

	// LSN_PPU_NAMETABLES = 0x2000.
	m_ui8NtAtBuffer = m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	m_ui64RenderStartCycle = m_ui64Cycle;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2__0x240() {

	m_ui8ThisLineSpriteCount = 0;

	++m_stCurCycle;
}


