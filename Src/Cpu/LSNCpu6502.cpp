/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
 * 
 * https://www.nesdev.org/6502_cpu.txt
 * http://www.oxyron.de/html/opcodes02.html
 * http://www.6502.org/tutorials/6502opcodes.html#CLC
 * http://users.telenet.be/kim1-6502/6502/proman.html
 * http://problemkaputt.de/everynes.htm#cpu65xxmicroprocessor
 */


#include "LSNCpu6502.h"
#include "../Bus/LSNBus.h"

#define LSN_ADVANCE_CONTEXT_COUNTERS_BY( AMT )											/*m_pccCurContext->ui8Cycle += AMT;*/	\
																						m_pccCurContext->ui8FuncIdx += AMT
#define LSN_ADVANCE_CONTEXT_COUNTERS													LSN_ADVANCE_CONTEXT_COUNTERS_BY( 1 )
#define LSN_PUSH( VAL )																	m_pbBus->CpuWrite( 0x100 + S--, (VAL) )
#define LSN_POP()																		m_pbBus->CpuRead( 0x100 + ++S )
#define LSN_FINISH_INST																	m_vContextStack.pop_back();																		\
																						m_pccCurContext = m_vContextStack.size() ? &m_vContextStack[m_vContextStack.size()-1] : nullptr

#define LSN_INDIRECT_X_R( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::FUNC, }, 6, LSN_AM_INDIRECT_X, 2
#define LSN_INDIRECT_X_RMW( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 8, LSN_AM_INDIRECT_X, 2

#define LSN_INDIRECT_Y_R( NAME, FUNC1, FUNC2, DESC )									NAME, DESC, { &CCpu6502::FetchPointerAndIncPc, &CCpu6502::FetchEffectiveAddressLow_IzY, &CCpu6502::FetchEffectiveAddressHigh_IzY, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 5, LSN_AM_INDIRECT_Y, 2
#define LSN_INDIRECT_Y_RMW( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchPointerAndIncPc, &CCpu6502::FetchEffectiveAddressLow_IzY, &CCpu6502::FetchEffectiveAddressHigh_IzY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 8, LSN_AM_INDIRECT_Y, 2

#define LSN_ZERO_PAGE_R( NAME, FUNC, DESC )												NAME, DESC, { &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::FUNC, }, 3, LSN_AM_ZERO_PAGE, 2
#define LSN_ZERO_PAGE_RMW( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromEffectiveAddress_Zp, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 5, LSN_AM_ZERO_PAGE, 2

#define LSN_ABSOLUTE_R( NAME, FUNC, DESC )												NAME, DESC, { &CCpu6502::FetchLowAddrByteAndIncPc, &CCpu6502::FetchHighAddrByteAndIncPc, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE, 3
#define LSN_ABSOLUTE_RMW( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchLowAddrByteAndIncPc, &CCpu6502::FetchHighAddrByteAndIncPc, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 6, LSN_AM_ABSOLUTE, 3

#define LSN_IMMEDIATE( NAME, FUNC, DESC )												NAME, DESC, { &CCpu6502::FUNC, }, 2, LSN_AM_IMMEDIATE, 2

#define LSN_ZERO_PAGE_X_R( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpX, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_X, 2
#define LSN_ZERO_PAGE_X_RMW( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 6, LSN_AM_ZERO_PAGE_X, 2

#define LSN_ABSOLUTE_Y_R( NAME, FUNC1, FUNC2, DESC )									NAME, DESC, { &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddY, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 4, LSN_AM_ABSOLUTE_Y, 3
#define LSN_ABSOLUTE_Y_RMW( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 7, LSN_AM_ABSOLUTE_Y, 3

#define LSN_ABSOLUTE_X_R( NAME, FUNC1, FUNC2, DESC )									NAME, DESC, { &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddX, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 4, LSN_AM_ABSOLUTE_X, 3
#define LSN_ABSOLUTE_X_RMW( NAME, FUNC, DESC )											NAME, DESC, { &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddX, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 7, LSN_AM_ABSOLUTE_X, 3

/*
 * imm = #$00						Immediate addressing
 * zp = $00							Zero page addressing
 * zpx = $00,X						Zero page indexed addressing
 * zpy = $00,Y						Zero page indexed addressing
 * izx = ($00,X)					Indexed indirect addressing
 * izy = ($00),Y					Indirect indexed addressing
 * abs = $0000						Absolute addressing
 * abx = $0000,X					Absolute indexed addressing
 * aby = $0000,Y					Absolute indexed addressing
 * ind = ($0000)					Absolute indirect addressing (JMP)
 * rel = $0000 (PC-relative)		Relative addressing
 */
// Considered handled:
//	LSN_AM_IMPLIED
//	LSN_AM_INDIRECT_X
//	LSN_AM_ZERO_PAGE
//	LSN_AM_IMMEDIATE
//	LSN_AM_ABSOLUTE
//	LSN_AM_RELATIVE (Testing needed.)
//	LSN_AM_INDIRECT_Y
//	LSN_AM_ZERO_PAGE_X
//	LSN_AM_ABSOLUTE_Y
//	LSN_AM_ABSOLUTE_X

namespace lsn {

	// == Members.
	CCpu6502::LSN_INSTR CCpu6502::m_iInstructionSet[256] = {							/**< The instruction set. */
		/** 00-07 */
		{	// 00
			"BRK",
			u8"BRK causes a non-maskable interrupt and increments the program counter by one. Therefore an RTI will go to the address of the BRK +2 so that BRK may be used to replace a two-byte instruction for debugging and the subsequent RTI will be correct.",
			{
				&CCpu6502::ReadNextInstByteAndDiscardAndIncPc,
				&CCpu6502::PushPchWithBFlagAndDecS,
				&CCpu6502::PushPclAndDecS,
				&CCpu6502::PushStatusAndDecS,
				&CCpu6502::FetchPclFromFFFE,
				&CCpu6502::BRK, },									// Fetches from 0xFFFF and writes to the high byte of PC.
				7, LSN_AM_IMPLIED, 1
		},
		{	// 01
			LSN_INDIRECT_X_R( "ORA", ORA_IzX, u8"Bitwise OR with accumulator." )
		},
		{	// 02
			"X02",
			u8"Illegal opcode.",
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1
		},
		{	// 03
			LSN_INDIRECT_X_RMW( "SLO", SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. OP <<= 1, A &= OP." )
		},
		{	// 04
			LSN_ZERO_PAGE_R( "NOP", NOP_Abs, u8"No operation." )
		},
		{	// 05
			LSN_ZERO_PAGE_R( "ORA", ORA_Zp, u8"Bitwise OR with accumulator." )
		},
		{	// 06
			LSN_ZERO_PAGE_RMW( "ASL", ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Arithmetic shift left (shifts in a zero bit on the right)." )
		},
		{	// 07
			LSN_ZERO_PAGE_RMW( "SLO", SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. OP <<= 1, A &= OP." )
		},

		/** 08-0F */
		{	// 08
			"PHP",
			u8"Push processor status register (with break flag set).",
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::PHP_Imp, },
				3, LSN_AM_IMPLIED, 1
		},
		{	// 09
			LSN_IMMEDIATE( "ORA", ORA_Imm, u8"Bitwise OR with accumulator." )
		},
		{	// 0A
			"ASL",
			u8"Arithmetic shift left (shifts in a zero bit on the right).",
			{
				&CCpu6502::ASL_Imp, },
				2, LSN_AM_IMPLIED, 1
		},
		{	// 0B
			LSN_IMMEDIATE( "ANC", ANC_Imm, u8"Unusual operation. AND with carry." )
		},
		{	// 0C
			LSN_ABSOLUTE_R( "NOP", NOP_Abs, u8"No operation." )
		},
		{	// 0D
			LSN_ABSOLUTE_R( "ORA", ORA_Abs, u8"Bitwise OR with accumulator." )
		},
		{	// 0E
			LSN_ABSOLUTE_RMW( "ASL", ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Arithmetic shift left (shifts in a zero bit on the right)." )
		},
		{	// 0F
			LSN_ABSOLUTE_RMW( "SLO", SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. OP <<= 1, A &= OP." )
		},

		/** 10-17 */
		{	// 10
			"BPL",
			u8"Branch on plus (negative clear).",
			{
				&CCpu6502::Branch_Cycle2,
				&CCpu6502::Branch_Cycle3<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), 0>,		// Branch if N == 0.
				&CCpu6502::Branch_Cycle4,														// Optional (if branch is taken).
				&CCpu6502::Branch_Cycle5, },													// Optional (if branch crosses page boundary).
				2, LSN_AM_RELATIVE, 2
		},
		{	// 11
			LSN_INDIRECT_Y_R( "ORA", ORA_IzY_1, ORA_IzY_2, u8"Bitwise OR with accumulator." )
		},
		{	// 12
			"X12",
			u8"Illegal opcode.",
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1
		},
		{	// 13
			LSN_INDIRECT_Y_RMW( "SLO", SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. OP <<= 1, A &= OP." )
		},
		{	// 14
			LSN_ZERO_PAGE_X_R( "NOP", NOP_Abs, u8"No operation." )
		},
		{	// 15
			LSN_ZERO_PAGE_X_R( "ORA", ORA_ZpX, u8"Bitwise OR with accumulator." )
		},
		{	// 16
			LSN_ZERO_PAGE_X_RMW( "ASL", ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Arithmetic shift left (shifts in a zero bit on the right)." )
		},
		{	// 17
			LSN_ZERO_PAGE_X_RMW( "SLO", SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. OP <<= 1, A &= OP." )
		},

		/** 18-1F */
		{	// 18
			"CLC",
			u8"Clear carry.",
			{
				&CCpu6502::CLC, },
				2, LSN_AM_IMPLIED, 1
		},
		{	// 19
			LSN_ABSOLUTE_Y_R( "ORA", ORA_AbY_1, ORA_AbY_2, u8"Bitwise OR with accumulator." )
		},
		{	// 1A
			"NOP",
			u8"No operation.",
			{	// Undocumented command.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1
		},
		{	// 1B
			LSN_ABSOLUTE_Y_RMW( "SLO", SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. OP <<= 1, A &= OP." )
		},
		{	// 1C
			LSN_ABSOLUTE_X_R( "NOP", ReadEffectiveAddressFixHighByte_IzY_AbX, NOP_Abs, u8"No operation." )
		},
		{	// 1D
			LSN_ABSOLUTE_X_R( "ORA", ORA_AbY_1, ORA_AbY_2, u8"Bitwise OR with accumulator." )
		},
		{	// 1E
			LSN_ABSOLUTE_X_RMW( "ASL", ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Arithmetic shift left (shifts in a zero bit on the right)." )
		},
		{	// 1F
			LSN_ABSOLUTE_X_RMW( "SLO", SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. OP <<= 1, A &= OP." )
		},

		/** 20-27 */
		{	// 20
			"JSR",
			u8"Jump to subroutine.",
			{
				&CCpu6502::FetchLowAddrByteAndIncPc,
				&CCpu6502::Jsr_Cycle3,
				&CCpu6502::Jsr_Cycle4,
				&CCpu6502::Jsr_Cycle5,
				&CCpu6502::JSR, },
				6, LSN_AM_ABSOLUTE, 3
		},
		{	// 21
			LSN_INDIRECT_X_R( "AND", AND_IzX_Zp_Abs, u8"Bitwise AND with accumulator." )
		},
		{	// 22
			"X22",
			u8"Illegal opcode.",
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1
		},
		{	// 23
			LSN_INDIRECT_X_RMW( "RLA", RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. ROL OP, A &= OP." )
		},
		{	// 24
			LSN_ZERO_PAGE_R( "BIT", BIT_Zp_Abs, u8"Bit test." )
		},
		{	// 25
			LSN_ZERO_PAGE_R( "AND", AND_IzX_Zp_Abs, u8"Bitwise AND with accumulator." )
		},
		{	// 26
			LSN_ZERO_PAGE_RMW( "ROL", ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Rotate left (shifts in carry bit on the right)." )
		},
		{	// 27
			LSN_ZERO_PAGE_RMW( "RLA", RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. ROL OP, A &= OP." )
		},

		/** 28-2F */
		{	// 28
			"PLP",
			u8"Pull processor status from stack.",
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::Plp_Cycle3,
				&CCpu6502::PLP_Imp, },
				4, LSN_AM_IMPLIED, 1
		},
		{	// 29
			LSN_IMMEDIATE( "AND", AND_Imm, u8"Bitwise AND with accumulator." )
		},
		{	// 2A
			"ROL",
			u8"Rotate left (shifts in carry bit on the right).",
			{
				&CCpu6502::ROL_Imp, },
				2, LSN_AM_IMPLIED, 1
		},
		{	// 2B
			LSN_IMMEDIATE( "ANC", ANC_Imm, u8"Unusual operation. AND with carry." )
		},
		{	// 2C
			LSN_ABSOLUTE_R( "BIT", BIT_Zp_Abs, u8"Bit test." )
		},
		{	// 2D
			LSN_ABSOLUTE_R( "AND", AND_IzX_Zp_Abs, u8"Bitwise AND with accumulator." )
		},
		{	// 2E
			LSN_ABSOLUTE_RMW( "ROL", ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Rotate left (shifts in carry bit on the right)." )
		},
		{	// 2F
			LSN_ABSOLUTE_RMW( "RLA", RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs, u8"Illegal. ROL OP, A &= OP." )
		},
	};

	// == Various constructors.
	CCpu6502::CCpu6502( class CBus * _pbBus ) :
		m_pbBus( _pbBus ),
		m_pccCurContext( nullptr ),
		A( 0 ),
		S( 0xFD ),
		X( 0 ),
		Y( 0 ) {
		pc.PC = 0xC000;
		m_ui8Status = 0x34;
	}
	CCpu6502::~CCpu6502() {
		ResetToKnown();
	}

	// == Functions.
	/**
	 * Resets the CPU to a known state.
	 */
	void CCpu6502::ResetToKnown() {
		ResetAnalog();
		A = 0;
		S = 0xFD;
		X = Y = 0;
		pc.PC = 0xC000;
		m_ui8Status = 0x34;
	}

	/**
	 * Performs an "analog" reset, allowing previous data to remain.
	 */
	void CCpu6502::ResetAnalog() {
		m_vContextStack.clear();
		S -= 3;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ ), true );
	}

	/**
	 * Fetches the next opcode and increments the program counter.
	 */
	void CCpu6502::FetchOpcodeAndIncPc() {
		// Enter normal instruction context.
		LSN_CPU_CONTEXT ccContext;
		ccContext.cContext = LSN_C_NORMAL;
		ccContext.ui8Cycle = 1;		// Values as they should be after this function exits (this function representing the first cycle in all instructions).
		ccContext.ui8FuncIdx = 0;	// This function is implicit, so index 0 is the function handler for the 2nd cycle following this one.
		m_vContextStack.push_back( ccContext );
		// Store the pointer to the last item on the stack so it doesn't have to be recalculated over and over.
		m_pccCurContext = &m_vContextStack[m_vContextStack.size()-1];

		// Perform the actual work.
		m_pccCurContext->ui8OpCode = m_pbBus->CpuRead( pc.PC++ );
	}

	/**
	 * Reads the next instruction byte and throws it away.
	 */
	void CCpu6502::ReadNextInstByteAndDiscard() {
		m_pbBus->CpuRead( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads the next instruction byte and throws it away. */
	void CCpu6502::ReadNextInstByteAndDiscardAndIncPc() {
		//  #  address R/W description
		// --- ------- --- -----------------------------------------------
		//  2    PC     R  read next instruction byte (and throw it away),
		//                 increment PC

		m_pbBus->CpuRead( pc.PC++ );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/**
	 * Fetches a value using immediate addressing.
	 */
	void CCpu6502::FetchValueAndIncPc_Imm() {
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( pc.PC++ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches a pointer and increments PC .*/
	void CCpu6502::FetchPointerAndIncPc() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  2      PC       R  fetch pointer address, increment PC
		
		// Fetch pointer address, increment PC.
		m_pccCurContext->ui8Pointer = m_pbBus->CpuRead( pc.PC++ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches an 8-bit address for Zero-Page dereferencing and increments PC. Stores the address in LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchAddressAndIncPc_Zp() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch address, increment PC
		
		// Fetches the value from an 8-bit address on the zero page.
		m_pccCurContext->a.ui16Address = m_pbBus->CpuRead( pc.PC++ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches the low address and writes the value to the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchLowAddrByteAndIncPc_WriteImm() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  2     PC      R  fetch low byte of address, increment PC
		
		// val = PEEK(arg + Y)
		// Fetches the value from a 16-bit address anywhere in memory.
		m_pccCurContext->a.ui8Bytes[0] = m_pbBus->CpuRead( pc.PC++ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches the low address value for absolute addressing but does not write the value to LSN_CPU_CONTEXT::a.ui16Address yet.  Pair with FetchHighAddrByteAndIncPc(). */
	void CCpu6502::FetchLowAddrByteAndIncPc() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  2    PC     R  fetch low address byte, increment PC
		
		// Fetches the value from a 16-bit address anywhere in memory.
		m_pccCurContext->j.ui8Bytes[0] = m_pbBus->CpuRead( pc.PC++ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches the high address value for absolute/indirect addressing. */
	void CCpu6502::FetchHighAddrByteAndIncPc() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3    PC     R  copy low address byte to PCL, fetch high address
		//                 byte to PCH

		// Fetches the value from a 16-bit address anywhere in memory.
		m_pccCurContext->a.ui8Bytes[0] = m_pccCurContext->j.ui8Bytes[0];
		m_pccCurContext->a.ui8Bytes[1] = m_pbBus->CpuRead( pc.PC++ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches the high address value for absolute/indirect addressing.  Adds Y to the low byte of the resulting address. */
	void CCpu6502::FetchHighAddrByteAndIncPcAndAddY() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  3     PC      R  fetch high byte of address,
		//                   add index register to low address byte,
		//                   increment PC

		// val = PEEK(arg + Y)
		m_pccCurContext->a.ui8Bytes[1] = m_pbBus->CpuRead( pc.PC++ );
		m_pccCurContext->j.ui16JmpTarget = static_cast<uint16_t>(m_pccCurContext->a.ui16Address + Y);
		m_pccCurContext->a.ui8Bytes[0] = m_pccCurContext->j.ui8Bytes[0];
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches the high address value for absolute/indirect addressing.  Adds X to the low byte of the resulting address. */
	void CCpu6502::FetchHighAddrByteAndIncPcAndAddX() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  3    PC       R  fetch high byte of address,
		//                   add index register X to low address byte,
		//                   increment PC

		// val = PEEK(arg + X)
		m_pccCurContext->a.ui8Bytes[1] = m_pbBus->CpuRead( pc.PC++ );
		m_pccCurContext->j.ui16JmpTarget = static_cast<uint16_t>(m_pccCurContext->a.ui16Address + X);
		m_pccCurContext->a.ui8Bytes[0] = m_pccCurContext->j.ui8Bytes[0];
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
	void CCpu6502::ReadFromEffectiveAddress_Abs() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  4  address  R  read from effective address
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
	void CCpu6502::ReadFromEffectiveAddress_Zp() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3  address  R  read from effective address
		
		// Fetches the value from an 8-bit address on the zero page.
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores the result into LSN_CPU_CONTEXT::ui8Pointer.  Preceded by FetchPointerAndIncPc(). */
	void CCpu6502::ReadAtPointerAddress() {
		m_pccCurContext->ui8Pointer = m_pbBus->CpuRead( m_pccCurContext->ui8Pointer );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores (LSN_CPU_CONTEXT::ui8Pointer+X)&0xFF into LSN_CPU_CONTEXT::a.ui16Address.  Preceded by FetchPointerAndIncPc(). */
	void CCpu6502::ReadFromAddressAndAddX_ZpX() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  3   address   R  read from address, add index register to it

		// val = PEEK((arg + X) % 256)
		// This is the:
		//	(arg + X) % 256
		//	part.
		m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );	// This read cycle reads from the address prior to it being adjusted by X.
		m_pccCurContext->a.ui16Address = uint8_t( m_pccCurContext->a.ui16Address + X );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads from LSN_CPU_CONTEXT::a.ui16Address+Y, stores the result into LSN_CPU_CONTEXT::ui8Operand. */
	void CCpu6502::AddYAndReadAddress_IndY() {
		// This is a read cycle and will read whatever the effective address is, but the effective address is updated across 2 cycles if a page boundary is crossed,
		//	meaning the first read was using an invalid address.
		// The real hardware uses the extra cycle to fix up the address and read again, so we emulate both the invalid read.
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( (m_pccCurContext->a.ui16Address & 0xFF00) | ((m_pccCurContext->a.ui16Address + Y) & 0xFF) );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** 3rd cycle of JSR. */
	void CCpu6502::Jsr_Cycle3() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3  $0100,S  R  internal operation (predecrement S?)
		m_pbBus->CpuRead( 0x100 + S/*(S - 1)*/ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** 4th cycle of JSR. */
	void CCpu6502::Jsr_Cycle4() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  4  $0100,S  W  push PCH on stack, decrement S
		LSN_PUSH( pc.ui8Bytes[1] );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** 5th cycle of JSR. */
	void CCpu6502::Jsr_Cycle5() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  $0100,S  W  push PCL on stack, decrement S
		LSN_PUSH( pc.ui8Bytes[0] );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** 3rd cycle of PLA/PLP. */
	void CCpu6502::Plp_Cycle3() {
		//  #  address R/W description
		// --- ------- --- -----------------------------------------------
		//  3  $0100,S  R  increment S

		// S is incremented in LSN_POP().
		// I think the read happens before the increment.
		m_pbBus->CpuRead( 0x100 + S/*(S + 1)*/ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes PCH. */
	void CCpu6502::PushPch() {
		LSN_PUSH( pc.ui8Bytes[1] );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes PCH, sets the B flag, and decrements S. */
	void CCpu6502::PushPchWithBFlagAndDecS() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3  $0100,S  W  push PCH on stack (with B flag set), decrement S
		LSN_PUSH( pc.ui8Bytes[1] );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ), true );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes PCL, decrements S. */
	void CCpu6502::PushPclAndDecS() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  4  $0100,S  W  push PCL on stack, decrement S
		LSN_PUSH( pc.ui8Bytes[0] );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes status with B. */
	void CCpu6502::PushStatusAndB() {
		LSN_PUSH( m_ui8Status | uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) );
		// It is also at this point that the branch vector is determined.  Store it in LSN_CPU_CONTEXT::a.ui16Address.
		m_pccCurContext->a.ui16Address = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ )) ? uint16_t( LSN_VECTORS::LSN_V_NMI ) : uint16_t( LSN_VECTORS::LSN_V_IRQ_BRK );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes status an decrements S. */
	void CCpu6502::PushStatusAndDecS() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  $0100,S  W  push P on stack, decrement S
		LSN_PUSH( m_ui8Status );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes status without B. */
	void CCpu6502::PushStatus() {
		LSN_PUSH( m_ui8Status );
		// It is also at this point that the branch vector is determined.  Store it in LSN_CPU_CONTEXT::a.ui16Address.
		m_pccCurContext->a.ui16Address = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ )) ? uint16_t( LSN_VECTORS::LSN_V_NMI ) : uint16_t( LSN_VECTORS::LSN_V_IRQ_BRK );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads from LSN_CPU_CONTEXT::ui8Pointer, adds X to it, stores the result in LSN_CPU_CONTEXT::ui8Pointer. */
	void CCpu6502::ReadAddressAddX_IzX() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  3    pointer    R  read from the address, add X to it

		// val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
		// This is the "(arg + X)" step, but since every cycle is a read or write there is also a superfluous read of "arg" here.
		m_pbBus->CpuRead( m_pccCurContext->ui8Pointer );
		m_pccCurContext->ui8Pointer = static_cast<uint8_t>(m_pccCurContext->ui8Pointer + X);
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads the low byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+X, store in the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchEffectiveAddressLow_IzX() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  4   pointer+X   R  fetch effective address low
		
		// val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
		// This is the:
		//	PEEK((arg + X) % 256)
		//	part.
		m_pccCurContext->a.ui8Bytes[0] = m_pbBus->CpuRead( m_pccCurContext->ui8Pointer );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads the high byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+X+1, store in the high byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchEffectiveAddressHigh_IzX() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5  pointer+X+1  R  fetch effective address high

		// val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
		// This is the:
		//	PEEK((arg + X + 1) % 256) * 256
		//	part.
		m_pccCurContext->a.ui8Bytes[1] = m_pbBus->CpuRead( static_cast<uint8_t>(m_pccCurContext->ui8Pointer + 1) );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads the low byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer, store in the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchEffectiveAddressLow_IzY() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  3    pointer    R  fetch effective address low
		
		// val = PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)
		// Fetch effective address low.
		// This is the:
		//	PEEK(arg)
		//	part.
		m_pccCurContext->a.ui8Bytes[0] = m_pbBus->CpuRead( m_pccCurContext->ui8Pointer );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Reads the high byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+1, store in the high byte of LSN_CPU_CONTEXT::a.ui16Address, adds Y. */
	void CCpu6502::FetchEffectiveAddressHigh_IzY() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  4   pointer+1   R  fetch effective address high,
		//                     add Y to low byte of effective address
		
		// val = PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)
		// Fetch effective address high.
		// This is the:
		//	PEEK((arg + 1) % 256) * 256
		//	part.
		m_pccCurContext->a.ui8Bytes[1] = m_pbBus->CpuRead( static_cast<uint8_t>(m_pccCurContext->ui8Pointer + 1) );
		// And this is the:
		//	+ Y
		//	part.
		// Check here if we are going to need to skip a cycle (if a page boundary is NOT crossed).
		m_pccCurContext->j.ui16JmpTarget = static_cast<uint16_t>(m_pccCurContext->a.ui16Address + Y);
		m_pccCurContext->a.ui8Bytes[0] = m_pccCurContext->j.ui8Bytes[0];
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//const uint16_t uiAdvanceBy = ((m_pccCurContext->ui16JmpTarget & 0xFF00) == (m_pccCurContext->a.ui16Address & 0xFF00)) + 1;
		//LSN_ADVANCE_CONTEXT_COUNTERS_BY( uiAdvanceBy );
	}

	/** Reads from the effective address (LSN_CPU_CONTEXT::a.ui16Address), which may be wrong if a page boundary was crossed.  If so, fixes the high byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// Read from effective address,
		// fix high byte of effective address.
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		;
		bool bCrossed = m_pccCurContext->j.ui8Bytes[1] != m_pccCurContext->a.ui8Bytes[1];
		if ( bCrossed ) {
			// Crossed a page boundary.
			m_pccCurContext->a.ui8Bytes[1] = m_pccCurContext->j.ui8Bytes[1];
			LSN_ADVANCE_CONTEXT_COUNTERS;
		}
		else {
			// Skip a cycle.  m_pccCurContext->ui8Operand already holds the correct value.
			LSN_ADVANCE_CONTEXT_COUNTERS_BY( 2 );
		}
	}

	/** Pops the low byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the low byte of PC. */
	void CCpu6502::CopyVectorPcl() {
		pc.ui8Bytes[0] = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches the low byte of PC from $FFFE. */
	void CCpu6502::FetchPclFromFFFE() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  6   $FFFE   R  fetch PCL
		pc.ui8Bytes[0] = m_pbBus->CpuRead( 0xFFFE );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches the high byte of PC from $FFFF. */
	void CCpu6502::FetchPclFromFFFF() {
		pc.ui8Bytes[1] = m_pbBus->CpuRead( 0xFFFF );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Writes the operand value back to the effective address stored in LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::WriteValueBackToEffectiveAddress() {
		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Writes the operand value back to the effective address stored in LSN_CPU_CONTEXT::a.ui16Address&0xFF. */
	void CCpu6502::WriteValueBackToEffectiveAddress_Zp() {
		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address & 0xFF, m_pccCurContext->ui8Operand );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** 2nd cycle of branch instructions. Reads the operand (JMP offset). */
	void CCpu6502::Branch_Cycle2() {
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( pc.PC++ );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** 3rd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	template <unsigned _uBit, unsigned _uVal>
	void CCpu6502::Branch_Cycle3() {
		// Fetch next opcode.
		m_pbBus->CpuRead( pc.PC );	// Read and discard.  Affects emulation of the floating bus.
		// If the branch is not taken, we are done here.
		// For example:
		// -> _uBit == LSN_STATUS_FLAGS::LSN_SF_NEGATIVE
		// -> _uVal == 0
		// -> if ( (m_ui8Status & LSN_STATUS_FLAGS::LSN_SF_NEGATIVE) != (0 * LSN_STATUS_FLAGS::LSN_SF_NEGATIVE) ) {
		// Or:
		// -> _uBit == LSN_STATUS_FLAGS::LSN_SF_NEGATIVE
		// -> _uVal == 1
		// -> if ( (m_ui8Status & LSN_STATUS_FLAGS::LSN_SF_NEGATIVE) != (1 * LSN_STATUS_FLAGS::LSN_SF_NEGATIVE) ) {
		if ( (m_ui8Status & _uBit) != (_uVal * _uBit) ) {
			// Branch not taken.
			++pc.PC;
			LSN_FINISH_INST;
		}
		else {
			// Branch taken.
			m_pccCurContext->j.ui16JmpTarget = static_cast<int16_t>(static_cast<int8_t>(m_pccCurContext->ui8Operand)) + pc.PC;
			// Set PCL.
			pc.ui8Bytes[0] = m_pccCurContext->j.ui8Bytes[0];
			LSN_ADVANCE_CONTEXT_COUNTERS;
		}
	}

	/** 2nd cycle of branch instructions. Performs the check to decide which cycle comes next (or to end the instruction). */
	/*template <unsigned _uBit, unsigned _uVal>
	void CCpu6502::Branch_Cycle2() {
		// This is a read instruction.
		m_pbBus->CpuRead( PC );	// Read and discard.  Affects emulation of the floating bus.
		
		// If the branch is not taken, we are done here.
		// _uVal is 1 or 0, so _uVal * _uBit is _uBit or 0.
		if ( (m_ui8Status & _uBit) != (_uVal * _uBit) ) {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
		else {
			// Branch is taken.
			// Calculate offset and write to the low byte of PC.
			uint16_t ui16NewAddr = static_cast<int16_t>(static_cast<int8_t>(m_pccCurContext->ui8Operand)) + PC;
			uint16_t ui16PcH = PC & 0xFF00;
			// Store the high byte to write later.
			m_pccCurContext->ui8Pointer = static_cast<uint8_t>(ui16NewAddr >> 8);
			PC = ui16PcH | (ui16NewAddr & 0xFF);	// Write low byte.
			// m_pccCurContext->ui8Pointer == (ui16PcH >> 8)
			// (m_pccCurContext->ui8Pointer << 8) == ui16PcH

			LSN_ADVANCE_CONTEXT_COUNTERS;
		}
	}*/

	/** 3rd cycle of branch instructions. Branch was taken, so copy part of the address and check for a page boundary. */
	/*void CCpu6502::Branch_Cycle3() {
		// This is a read instruction.
		m_pbBus->CpuRead( PC );	// Read and discard.  Affects emulation of the floating bus.
		
		
		// Check for page crossing. High byte of PC was not modified yet (next instruction).
		// Remember from cycle 2:
		// m_pccCurContext->ui8Pointer == (ui16PcH >> 8)
		// (m_pccCurContext->ui8Pointer << 8) == ui16PcH
		uint16_t ui16PcH = m_pccCurContext->ui8Pointer << 8;
		bool bCrossed = ui16PcH != (PC & 0xFF00);
		PC = (PC & 0xFF) | ui16PcH;
		if ( ui16PcH != (PC & 0xFF00) ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}*/

	/** 4th cycle of branch instructions. Branch was taken and crossed a page boundary, but PC is already up-to-date so read/discard/exit. */
	void CCpu6502::Branch_Cycle4() {
		// Fetch next opcode.
		m_pbBus->CpuRead( pc.PC );	// Read and discard.  Affects emulation of the floating bus.

		bool bCrossed = m_pccCurContext->j.ui8Bytes[1] != pc.ui8Bytes[1];
		if ( bCrossed ) {
			// Crossed a page boundary.
			pc.ui8Bytes[1] = m_pccCurContext->j.ui8Bytes[1];
			LSN_ADVANCE_CONTEXT_COUNTERS;
		}
		else {
			// Did not cross a page boundary.
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** 5th cycle of branch instructions. Page boundary was crossed. */
	void CCpu6502::Branch_Cycle5() {
		/**
		Relative addressing (BCC, BCS, BNE, BEQ, BPL, BMI, BVC, BVS)

        #   address  R/W description
       --- --------- --- ---------------------------------------------
        1     PC      R  fetch opcode, increment PC
        2     PC      R  fetch operand, increment PC
        3     PC      R  Fetch opcode of next instruction,
                         If branch is taken, add operand to PCL.
                         Otherwise increment PC.
        4+    PC*     R  Fetch opcode of next instruction.
                         Fix PCH. If it did not change, increment PC.
        5!    PC      R  Fetch opcode of next instruction,
                         increment PC.

       Notes: The opcode fetch of the next instruction is included to
              this diagram for illustration purposes. When determining
              real execution times, remember to subtract the last
              cycle.

              * The high byte of Program Counter (PCH) may be invalid
                at this time, i.e. it may be smaller or bigger by $100.

              + If branch is taken, this cycle will be executed.

              ! If branch occurs to different page, this cycle will be
                executed.
		**/
		// Subtract the last cycle?  opcode fetch is just for illustration?
		// I am assuming this means it should behave as expected by putting PC in place and then letting the
		//	system eat the next opcode on the next cycle, but they could be pre-fetching the opcode as an
		//	optimization.
		m_pbBus->CpuRead( pc.PC );
		LSN_FINISH_INST;
	}

	/** Performs m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand ); and LSN_FINISH_INST;, which finishes Read-Modify-Write instructions. */
	void CCpu6502::FinalWriteCycle() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  6  address  W  write the new value to effective address
		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Follows FetchLowAddrByteAndIncPc() and copies the read value into the low byte of PC after fetching the high byte. */
	void CCpu6502::JMP_Abs() {
		m_pccCurContext->a.ui8Bytes[1] = m_pbBus->CpuRead( pc.PC );
		pc.PC = m_pccCurContext->a.ui16Address;

		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Pops the high byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the high byte of PC. 
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	ReadNextInstByteAndDiscardAndIncPc
	 *	PushPchWithBFlagAndDecS
	 *	PushPclAndDecS
	 *	PushStatusAndDecS
	 *	FetchPclFromFFFE
	 */
	void CCpu6502::BRK() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  7   $FFFF   R  fetch PCH
		pc.ui8Bytes[1] = m_pbBus->CpuRead( 0xFFFF );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Clears the carry flag. */
	void CCpu6502::CLC() {
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), false );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Reads the next instruction byte and throws it away. */
	void CCpu6502::NOP() {
		m_pbBus->CpuRead( pc.PC );

		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Reads the next instruction byte and throws it away, increasing PC. */
	void CCpu6502::NopAndIncPc() {
		m_pbBus->CpuRead( pc.PC++ );

		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Reads LSN_CPU_CONTEXT::a.ui16Address and throws it away. */
	void CCpu6502::NOP_Abs() {
		m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );

		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchPointerAndIncPc
	 *	ReadAddressAddX_IzX
	 *	FetchEffectiveAddressLow_IzX
	 *	FetchEffectiveAddressHigh_IzX
	 */
	void CCpu6502::ORA_IzX() {
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A |= m_pccCurContext->ui8Operand;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchPointerAndIncPc
	 *	FetchEffectiveAddressLow_IzY
	 *	FetchEffectiveAddressHigh_IzY
	 */
	void CCpu6502::ORA_IzY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// Read from effective address,
		// fix high byte of effective address.
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_pccCurContext->j.ui8Bytes[1] != m_pccCurContext->a.ui8Bytes[1];
		if ( bCrossed ) {
			// Crossed a page boundary.
			m_pccCurContext->a.ui8Bytes[1] = m_pccCurContext->j.ui8Bytes[1];
			LSN_ADVANCE_CONTEXT_COUNTERS;
		}
		else {
			// We are done.
			A |= m_pccCurContext->ui8Operand;
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchPointerAndIncPc
	 *	FetchEffectiveAddressLow_IzY
	 *	FetchEffectiveAddressHigh_IzY
	 *	ORA_IzY_1
	 */
	void CCpu6502::ORA_IzY_2() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  6+  address+Y   R  read from effective address

		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A |= m_pccCurContext->ui8Operand;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchLowAddrByteAndIncPc
	 *	FetchHighAddrByteAndIncPcAndAddY
	 *	FetchEffectiveAddressHigh_IzY
	 */
	void CCpu6502::ORA_AbY_1() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  4  address+I* R  read from effective address,
		//                   fix the high byte of effective address
		
		// Copy of ORA_IzY_1.
		// Read from effective address,
		// fix high byte of effective address.
		m_pccCurContext->ui8Operand = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_pccCurContext->j.ui8Bytes[1] != m_pccCurContext->a.ui8Bytes[1];
		if ( bCrossed ) {
			// Crossed a page boundary.
			m_pccCurContext->a.ui8Bytes[1] = m_pccCurContext->j.ui8Bytes[1];
			LSN_ADVANCE_CONTEXT_COUNTERS;
		}
		else {
			// We are done.
			A |= m_pccCurContext->ui8Operand;
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchLowAddrByteAndIncPc
	 *	FetchHighAddrByteAndIncPcAndAddY
	 *	FetchEffectiveAddressHigh_IzY
	 *	ORA_AbY_1
	 */
	void CCpu6502::ORA_AbY_2() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  5+ address+I  R  re-read from effective address
		
		// Copy of ORA_IzY_2.
		const uint8_t ui8Op = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A |= ui8Op;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchAddressAndIncPc_Zp
	 */
	void CCpu6502::ORA_Zp() {
		const uint8_t ui8Op = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A |= ui8Op;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchPointerAndIncPc()
	 *	ReadFromAddressAndAddX_ZpX()
	 */
	void CCpu6502::ORA_ZpX() {
		const uint8_t ui8Op = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A |= ui8Op;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from PC and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 */
	void CCpu6502::ORA_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		uint8_t ui8Tmp = m_pbBus->CpuRead( pc.PC++ );
		A |= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from PC and performs A = A | OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchLowAddrByteAndIncPc()
	 *	FetchHighAddrByteAndIncPc()
	 */
	void CCpu6502::ORA_Abs() {
		uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A |= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Sets flags N, V and Z according to a bit test.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchAddressAndIncPc_Zp
	 */
	void CCpu6502::BIT_Zp_Abs() {
		const uint8_t ui8Op = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), (ui8Op & A) == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (ui8Op & (1 << 7)) != 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), (ui8Op & (1 << 6)) != 0x00 );
		LSN_FINISH_INST;
	}

	/** Fetches from PC and performs A = A & OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 */
	void CCpu6502::ANC_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A &= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchAddressAndIncPc_Zp
	 */
	void CCpu6502::AND_IzX_Zp_Abs() {
		uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A &= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from PC and performs A = A & OP.  Sets flags N and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 */
	void CCpu6502::AND_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		uint8_t ui8Tmp = m_pbBus->CpuRead( pc.PC++ );
		A &= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** A zero-page ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchAddressAndIncPc_Zp
	 *	ReadFromAddressAndAddX_ZpX
	 *	ReadFromEffectiveAddress_Abs
	 */
	void CCpu6502::ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		m_pccCurContext->ui8Operand <<= 1;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), m_pccCurContext->ui8Operand == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** An ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 */
	void CCpu6502::ASL_Imp() {
		// We have a discarded read here.
		m_pbBus->CpuRead( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		

		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (A & 0x80) != 0 );
		A <<= 1;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Pushes the status byte.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	ReadNextInstByteAndDiscard()
	 */
	void CCpu6502::PHP_Imp() {
		/* http://users.telenet.be/kim1-6502/6502/proman.html#811
		8.11 PHP - PUSH PROCESSOR STATUS ON STACK

          This instruction transfers the contents of the processor status reg-
     ister unchanged to the stack, as governed by the stack pointer.
          Symbolic notation for this is P v.
          The PHP instruction affects no registers or flags in the micropro-
     cessor.
          PHP is a single-byte instruction and the addressing mode is Implied.
		*/
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ), true );
		LSN_PUSH( m_ui8Status );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Pulls the status byte.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	ReadNextInstByteAndDiscard()
	 */
	void CCpu6502::PLP_Imp() {
		m_ui8Status = LSN_POP();
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Performs A = (A << 1) | (A >> 7).  Sets flags C, N, and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 */
	void CCpu6502::ROL_Imp() {
		// We have a discarded read here.
		m_pbBus->CpuRead( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		

		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (A & 0x80) != 0 );
		A = (A << 1) | (A >> 7);
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Performs OP = (OP << 1) | (OP >> 7).  Sets flags C, N, and Z.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchLowAddrByteAndIncPc
	 *	FetchHighAddrByteAndIncPc
	 */
	void CCpu6502::ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		m_pccCurContext->ui8Operand = (m_pccCurContext->ui8Operand << 1) | (m_pccCurContext->ui8Operand >> 7);
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), m_pccCurContext->ui8Operand == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs OP = (OP << 1); A = A | (OP).  Sets flags N and Z.  This is the first cycle of the function, which performs only the "OP = (OP << 1)" part.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchPointerAndIncPc
	 *	ReadAddressAddX_IzX
	 *	FetchEffectiveAddressLow_IzX
	 *	FetchEffectiveAddressHigh_IzX
	 *	ReadFromEffectiveAddress_Abs
	 */
	void CCpu6502::SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		m_pccCurContext->ui8Operand <<= 1;
		A |= m_pccCurContext->ui8Operand;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), m_pccCurContext->ui8Operand == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs OP = (OP << 1) | (OP >> 7); A = A | (OP).  Sets flags N and Z.  This is the first cycle of the function, which performs only the "OP = (OP << 1)" part.
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchPointerAndIncPc
	 *	ReadAddressAddX_IzX
	 *	FetchEffectiveAddressLow_IzX
	 *	FetchEffectiveAddressHigh_IzX
	 *	ReadFromEffectiveAddress_Abs
	 */
	void CCpu6502::RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		m_pccCurContext->ui8Operand = (m_pccCurContext->ui8Operand << 1) | (m_pccCurContext->ui8Operand >> 7);
		A |= m_pccCurContext->ui8Operand;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), m_pccCurContext->ui8Operand == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** JSR (Jump to Sub-Routine).
	 * Chain:
	 *	FetchOpcodeAndIncPc (implicit.)
	 *	FetchLowAddrByteAndIncPc
	 *	Jsr_Cycle3
	 *	Jsr_Cycle4
	 *	Jsr_Cycle5
	 */
	void CCpu6502::JSR() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  6    PC     R  copy low address byte to PCL, fetch high address
		//                 byte to PCH
		m_pccCurContext->j.ui8Bytes[1] = m_pbBus->CpuRead( pc.PC );
		pc.PC = m_pccCurContext->j.ui16JmpTarget;
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

}	// namespace lsn

#undef LSN_POP
#undef LSN_PUSH
#undef LSN_ADVANCE_CONTEXT_COUNTERS