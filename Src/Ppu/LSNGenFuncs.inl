void LSN_FASTCALL								Cycle_0x0_258x0_259x0_260x0_261x0_262x0_263x0_264x0_265x0_266x0_X() {

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_338x0_340x0_338x1_340x1_338x2_340x2_338x3_340x3_338x4_340x4_X() {

	/*m_ui8NextTileId = m_ui8NtAtBuffer;*/

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_340x261() {

	/*m_ui8NextTileId = m_ui8NtAtBuffer;*/

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


void LSN_FASTCALL								Cycle_337x0_339x0_337x1_339x1_337x2_339x2_337x3_339x3_337x4_339x4_X() {

	// LSN_PPU_NAMETABLES = 0x2000.
	/*m_ui8NtAtBuffer = */m_bBus.Read(LSN_PPU_NAMETABLES | (m_paPpuAddrV.ui16Addr & 0x0FFF));

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_1x241() {

	// [1, 241] on NTSC.
	// [1, 241] on PAL.
	// [1, 241] on Dendy.
	m_psPpuStatus.s.ui8VBlank = 1;
	if (m_pcPpuCtrl.s.ui8Nmi) {
		m_pnNmiTarget->Nmi();
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_257x240() {

	Pixel_Swap_Control();

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_280x261_281x261_282x261_283x261_284x261_285x261_286x261_287x261_288x261_289x261_X() {

	if (Rendering()) {
		m_paPpuAddrV.s.ui16FineY = m_paPpuAddrT.s.ui16FineY;
		m_paPpuAddrV.s.ui16NametableY = m_paPpuAddrT.s.ui16NametableY;
		m_paPpuAddrV.s.ui16CourseY = m_paPpuAddrT.s.ui16CourseY;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_257x0_257x1_257x2_257x3_257x4_257x5_257x6_257x7_257x8_257x9_X() {

	if (Rendering()) {
		m_paPpuAddrV.s.ui16NametableX = m_paPpuAddrT.s.ui16NametableX;
		m_paPpuAddrV.s.ui16CourseX = m_paPpuAddrT.s.ui16CourseX;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_321x0_329x0_321x1_329x1_321x2_329x2_321x3_329x3_321x4_329x4_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_1x261() {

	if (Rendering()) {
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

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_1x0_9x0_17x0_25x0_33x0_41x0_49x0_57x0_65x0_73x0_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_323x0_331x0_323x1_331x1_323x2_331x2_323x3_331x3_323x4_331x4_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_325x0_333x0_325x1_333x1_325x2_333x2_325x3_333x3_325x4_333x4_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_327x0_335x0_327x1_335x1_327x2_335x2_327x3_335x3_327x4_335x4_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_3x0_11x0_19x0_27x0_35x0_43x0_51x0_59x0_67x0_75x0_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_5x0_13x0_21x0_29x0_37x0_45x0_53x0_61x0_69x0_77x0_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_7x0_15x0_23x0_31x0_39x0_47x0_55x0_63x0_71x0_79x0_X() {

	if (Rendering()) {
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


void LSN_FASTCALL								Cycle_4x0_12x0_20x0_28x0_36x0_44x0_52x0_60x0_68x0_76x0_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileAttribute = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_2x0_10x0_18x0_26x0_34x0_42x0_50x0_58x0_66x0_74x0_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileId = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_6x0_14x0_22x0_30x0_38x0_46x0_54x0_62x0_70x0_78x0_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileLsb = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_8x0_16x0_24x0_32x0_40x0_48x0_56x0_64x0_72x0_80x0_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (Rendering()) {
		// m_paPpuAddrV.s.ui16CourseX is only 5 bits so wrapping is automatic with the increment.  Check if the nametable needs to be swapped before the increment.
		if (m_paPpuAddrV.s.ui16CourseX == 31) {
			// m_paPpuAddrV.s.ui16NametableX is 1 bit, so just toggle.
			m_paPpuAddrV.s.ui16NametableX = !m_paPpuAddrV.s.ui16NametableX;
		}
		++m_paPpuAddrV.s.ui16CourseX;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_256x0_256x1_256x2_256x3_256x4_256x5_256x6_256x7_256x8_256x9_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	RenderPixel();

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (Rendering()) {
		// m_paPpuAddrV.s.ui16CourseX is only 5 bits so wrapping is automatic with the increment.  Check if the nametable needs to be swapped before the increment.
		if (m_paPpuAddrV.s.ui16CourseX == 31) {
			// m_paPpuAddrV.s.ui16NametableX is 1 bit, so just toggle.
			m_paPpuAddrV.s.ui16NametableX = !m_paPpuAddrV.s.ui16NametableX;
		}
		++m_paPpuAddrV.s.ui16CourseX;
	}

	// Increase v.V.
	if (Rendering()) {
		if (m_paPpuAddrV.s.ui16FineY < 7) {
			++m_paPpuAddrV.s.ui16FineY;
		}
		else {
			m_paPpuAddrV.s.ui16FineY = 0;
			// Wrap and increment the course.
			// Do we need to swap vertical nametable targets?
			switch (m_paPpuAddrV.s.ui16CourseY) {
			case 29: {
				// Wrap the course offset and flip the nametable bit.
				m_paPpuAddrV.s.ui16CourseY = 0;
				m_paPpuAddrV.s.ui16NametableY = ~m_paPpuAddrV.s.ui16NametableY;
				break;
			}
			case 31: {
				// We are in attribute memory.  Reset but without flipping the nametable.
				m_paPpuAddrV.s.ui16CourseY = 0;
				break;
			}
			default: {
				// Somewhere between.  Just increment.
				++m_paPpuAddrV.s.ui16CourseY;
			}
			}
		}
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_324x0_332x0_324x1_332x1_324x2_332x2_324x3_332x3_324x4_332x4_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileAttribute = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_322x0_330x0_322x1_330x1_322x2_330x2_322x3_330x3_322x4_330x4_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileId = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_326x0_334x0_326x1_334x1_326x2_334x2_326x3_334x3_326x4_334x4_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileLsb = m_ui8NtAtBuffer;

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_328x0_336x0_328x1_336x1_328x2_336x2_328x3_336x3_328x4_336x4_X() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (Rendering()) {
		// m_paPpuAddrV.s.ui16CourseX is only 5 bits so wrapping is automatic with the increment.  Check if the nametable needs to be swapped before the increment.
		if (m_paPpuAddrV.s.ui16CourseX == 31) {
			// m_paPpuAddrV.s.ui16NametableX is 1 bit, so just toggle.
			m_paPpuAddrV.s.ui16NametableX = !m_paPpuAddrV.s.ui16NametableX;
		}
		++m_paPpuAddrV.s.ui16CourseX;
	}

	++m_stCurCycle;
}


void LSN_FASTCALL								Cycle_256x261() {

	if (Rendering()) {
		m_ui16ShiftPatternLo <<= 1;
		m_ui16ShiftPatternHi <<= 1;
		m_ui16ShiftAttribLo <<= 1;
		m_ui16ShiftAttribHi <<= 1;
	}

	m_ui8NextTileMsb = m_ui8NtAtBuffer;

	// Increase v.H.
	if (Rendering()) {
		// m_paPpuAddrV.s.ui16CourseX is only 5 bits so wrapping is automatic with the increment.  Check if the nametable needs to be swapped before the increment.
		if (m_paPpuAddrV.s.ui16CourseX == 31) {
			// m_paPpuAddrV.s.ui16NametableX is 1 bit, so just toggle.
			m_paPpuAddrV.s.ui16NametableX = !m_paPpuAddrV.s.ui16NametableX;
		}
		++m_paPpuAddrV.s.ui16CourseX;
	}

	// Increase v.V.
	if (Rendering()) {
		if (m_paPpuAddrV.s.ui16FineY < 7) {
			++m_paPpuAddrV.s.ui16FineY;
		}
		else {
			m_paPpuAddrV.s.ui16FineY = 0;
			// Wrap and increment the course.
			// Do we need to swap vertical nametable targets?
			switch (m_paPpuAddrV.s.ui16CourseY) {
			case 29: {
				// Wrap the course offset and flip the nametable bit.
				m_paPpuAddrV.s.ui16CourseY = 0;
				m_paPpuAddrV.s.ui16NametableY = ~m_paPpuAddrV.s.ui16NametableY;
				break;
			}
			case 31: {
				// We are in attribute memory.  Reset but without flipping the nametable.
				m_paPpuAddrV.s.ui16CourseY = 0;
				break;
			}
			default: {
				// Somewhere between.  Just increment.
				++m_paPpuAddrV.s.ui16CourseY;
			}
			}
		}
	}

	++m_stCurCycle;
}


