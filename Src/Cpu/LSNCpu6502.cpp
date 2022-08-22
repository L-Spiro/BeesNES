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

#define LSN_INDIRECT_X_R( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::FUNC, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 8, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME

#define LSN_INDIRECT_Y_R( NAME, FUNC1, FUNC2 )											{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::FetchEffectiveAddressLow_IzY, &CCpu6502::FetchEffectiveAddressHigh_IzY, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::FetchEffectiveAddressLow_IzY, &CCpu6502::FetchEffectiveAddressHigh_IzY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 8, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::FUNC, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_RMW( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromEffectiveAddress_Zp, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 5, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_R( NAME, FUNC )													{ &CCpu6502::FetchLowAddrByteAndIncPc, &CCpu6502::FetchHighAddrByteAndIncPc, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_RMW( NAME, FUNC )													{ &CCpu6502::FetchLowAddrByteAndIncPc, &CCpu6502::FetchHighAddrByteAndIncPc, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 6, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME

#define LSN_IMMEDIATE( NAME, FUNC )														{ &CCpu6502::FUNC, }, 2, LSN_AM_IMMEDIATE, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_X_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpX, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 6, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_Y_R( NAME, FUNC1, FUNC2 )											{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddY, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME

#define LSN_ABSOLUTE_X_R( NAME, FUNC1, FUNC2 )											{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddX, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 4, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddX, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX, &CCpu6502::ReadFromEffectiveAddress_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 7, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME

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
			{
				&CCpu6502::ReadNextInstByteAndDiscardAndIncPc,
				&CCpu6502::PushPchWithBFlag,
				&CCpu6502::PushPcl,
				&CCpu6502::PushStatus,
				&CCpu6502::FetchPclFromFFFE,
				&CCpu6502::BRK, },									// Fetches from 0xFFFF and writes to the high byte of PC.
				7, LSN_AM_IMPLIED, 1, LSN_I_BRK,
		},
		{	// 01
			LSN_INDIRECT_X_R( ORA, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 02
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 03
			LSN_INDIRECT_X_RMW( SLO, SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 04
			LSN_ZERO_PAGE_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 05
			LSN_ZERO_PAGE_R( ORA, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 06
			LSN_ZERO_PAGE_RMW( ASL, ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 07
			LSN_ZERO_PAGE_RMW( SLO, SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 08-0F */
		{	// 08
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::PHP, },
				3, LSN_AM_IMPLIED, 1, LSN_I_PHP,
		},
		{	// 09
			LSN_IMMEDIATE( ORA, ORA_Imm )
		},
		{	// 0A
			{
				&CCpu6502::ASL_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_ASL,
		},
		{	// 0B
			LSN_IMMEDIATE( ANC, ANC_Imm )
		},
		{	// 0C
			LSN_ABSOLUTE_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 0D
			LSN_ABSOLUTE_R( ORA, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 0E
			LSN_ABSOLUTE_RMW( ASL, ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 0F
			LSN_ABSOLUTE_RMW( SLO, SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 10-17 */
		{	// 10
			{
				&CCpu6502::Branch_Cycle2,
				&CCpu6502::Branch_Cycle3<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), 0>,		// Branch if N == 0.
				&CCpu6502::Branch_Cycle4,														// Optional (if branch is taken).
				&CCpu6502::Branch_Cycle5, },													// Optional (if branch crosses page boundary).
				2, LSN_AM_RELATIVE, 2, LSN_I_BPL,
		},
		{	// 11
			LSN_INDIRECT_Y_R( ORA, ORA_IzY_AbX_AbY_1, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 12
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 13
			LSN_INDIRECT_Y_RMW( SLO, SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 14
			LSN_ZERO_PAGE_X_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 15
			LSN_ZERO_PAGE_X_R( ORA, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 16
			LSN_ZERO_PAGE_X_RMW( ASL, ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 17
			LSN_ZERO_PAGE_X_RMW( SLO, SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 18-1F */
		{	// 18
			{
				&CCpu6502::CLC, },
				2, LSN_AM_IMPLIED, 1, LSN_I_CLC,
		},
		{	// 19
			LSN_ABSOLUTE_Y_R( ORA, ORA_IzY_AbX_AbY_1, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 1A
			{	// Undocumented command.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
		},
		{	// 1B
			LSN_ABSOLUTE_Y_RMW( SLO, SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 1C
			LSN_ABSOLUTE_X_R( NOP, NOP_IzY_AbX_AbY_1, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 1D
			LSN_ABSOLUTE_X_R( ORA, ORA_IzY_AbX_AbY_1, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 1E
			LSN_ABSOLUTE_X_RMW( ASL, ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 1F
			LSN_ABSOLUTE_X_RMW( SLO, SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 20-27 */
		{	// 20
			{
				&CCpu6502::FetchLowAddrByteAndIncPc,
				&CCpu6502::Jsr_Cycle3,
				&CCpu6502::Jsr_Cycle4,
				&CCpu6502::Jsr_Cycle5,
				&CCpu6502::JSR, },
				6, LSN_AM_ABSOLUTE, 3, LSN_I_JSR,
		},
		{	// 21
			LSN_INDIRECT_X_R( AND, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 22
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 23
			LSN_INDIRECT_X_RMW( RLA, RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 24
			LSN_ZERO_PAGE_R( BIT, BIT_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 25
			LSN_ZERO_PAGE_R( AND, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 26
			LSN_ZERO_PAGE_RMW( ROL, ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 27
			LSN_ZERO_PAGE_RMW( RLA, RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 28-2F */
		{	// 28
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::PLA_PLP_RTI_RTS_Cycle3,
				&CCpu6502::PLP, },
				4, LSN_AM_IMPLIED, 1, LSN_I_PLP,
		},
		{	// 29
			LSN_IMMEDIATE( AND, AND_Imm )
		},
		{	// 2A
			{
				&CCpu6502::ROL_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_ROL,
		},
		{	// 2B
			LSN_IMMEDIATE( ANC, ANC_Imm )
		},
		{	// 2C
			LSN_ABSOLUTE_R( BIT, BIT_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 2D
			LSN_ABSOLUTE_R( AND, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 2E
			LSN_ABSOLUTE_RMW( ROL, ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 2F
			LSN_ABSOLUTE_RMW( RLA, RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 30-37 */
		{	// 30
			{
				&CCpu6502::Branch_Cycle2,
				&CCpu6502::Branch_Cycle3<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), 1>,		// Branch if N == 0.
				&CCpu6502::Branch_Cycle4,														// Optional (if branch is taken).
				&CCpu6502::Branch_Cycle5, },													// Optional (if branch crosses page boundary).
				2, LSN_AM_RELATIVE, 2, LSN_I_BMI,
		},
		{	// 31
			LSN_INDIRECT_Y_R( AND, AND_IzY_AbX_AbY_1, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 32
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 33
			LSN_INDIRECT_Y_RMW( RLA, RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 34
			LSN_ZERO_PAGE_X_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 35
			LSN_ZERO_PAGE_X_R( AND, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 36
			LSN_ZERO_PAGE_X_RMW( ROL, ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 37
			LSN_ZERO_PAGE_X_RMW( RLA, RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 38-3F */
		{	// 38
			{
				&CCpu6502::SEC, },
				2, LSN_AM_IMPLIED, 1, LSN_I_SEC,
		},
		{	// 39
			LSN_ABSOLUTE_Y_R( AND, AND_IzY_AbX_AbY_1, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 3A
			{	// Undocumented command.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
		},
		{	// 3B
			LSN_ABSOLUTE_Y_RMW( RLA, RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 3C
			LSN_ABSOLUTE_X_R( NOP, NOP_IzY_AbX_AbY_1, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 3D
			LSN_ABSOLUTE_X_R( AND, AND_IzY_AbX_AbY_1, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 3E
			LSN_ABSOLUTE_X_RMW( ROL, ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 3F
			LSN_ABSOLUTE_X_RMW( RLA, RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 40-47 */
		{	// 40
			{
				&CCpu6502::ReadNextInstByteAndDiscardAndIncPc,
				&CCpu6502::PLA_PLP_RTI_RTS_Cycle3,
				&CCpu6502::PullStatusWithoutB,
				&CCpu6502::PullPcl,
				&CCpu6502::RTI, },									// Pops PCH.
				6, LSN_AM_IMPLIED, 1, LSN_I_RTI,
		},
		{	// 41
			LSN_INDIRECT_X_R( EOR, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 42
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},{	// 43
			LSN_INDIRECT_X_RMW( SRE, SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 44
			LSN_ZERO_PAGE_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 45
			LSN_ZERO_PAGE_R( EOR, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 46
			LSN_ZERO_PAGE_RMW( LSR, LSR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 47
			LSN_ZERO_PAGE_RMW( SRE, SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 48-4F */
		{	// 48
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::PHA, },
				3, LSN_AM_IMPLIED, 1, LSN_I_PHA,
		},
		{	// 49
			LSN_IMMEDIATE( EOR, EOR_Imm )
		},
		{	// 4A
			{
				&CCpu6502::LSR_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_LSR,
		},
		{	// 4B
			LSN_IMMEDIATE( ASR, ASR_Imm )
		},
		{	// 4C
			{
				&CCpu6502::FetchLowAddrByteAndIncPc,
				&CCpu6502::JMP_Abs, },
				3, LSN_AM_ABSOLUTE, 3, LSN_I_JMP,
		},
		{	// 4D
			LSN_ABSOLUTE_R( EOR, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 4E
			LSN_ABSOLUTE_RMW( LSR, LSR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 4F
			LSN_ABSOLUTE_RMW( SRE, SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 50-57 */
		{	// 50
			{
				&CCpu6502::Branch_Cycle2,
				&CCpu6502::Branch_Cycle3<uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), 0>,		// Branch if V == 0.
				&CCpu6502::Branch_Cycle4,														// Optional (if branch is taken).
				&CCpu6502::Branch_Cycle5, },													// Optional (if branch crosses page boundary).
				2, LSN_AM_RELATIVE, 2, LSN_I_BVC,
		},
		{	// 51
			LSN_INDIRECT_Y_R( EOR, EOR_IzY_AbX_AbY_1, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 52
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 53
			LSN_INDIRECT_Y_RMW( SRE, SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 54
			LSN_ZERO_PAGE_X_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 55
			LSN_ZERO_PAGE_X_R( EOR, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 56
			LSN_ZERO_PAGE_X_RMW( LSR, LSR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 57
			LSN_ZERO_PAGE_X_RMW( SRE, SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 58-5F */
		{	// 58
			{
				&CCpu6502::CLI, },
				2, LSN_AM_IMPLIED, 1, LSN_I_CLI,
		},
		{	// 59
			LSN_ABSOLUTE_Y_R( EOR, EOR_IzY_AbX_AbY_1, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 5A
			{	// Undocumented command.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
		},
		{	// 5B
			LSN_ABSOLUTE_Y_RMW( SRE, SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 5C
			LSN_ABSOLUTE_X_R( NOP, NOP_IzY_AbX_AbY_1, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 5D
			LSN_ABSOLUTE_X_R( EOR, EOR_IzY_AbX_AbY_1, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 5E
			LSN_ABSOLUTE_X_RMW( LSR, LSR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 5F
			LSN_ABSOLUTE_X_RMW( SRE, SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 60-67 */
		{	// 60
			{
				&CCpu6502::ReadNextInstByteAndDiscardAndIncPc,
				&CCpu6502::PLA_PLP_RTI_RTS_Cycle3,
				&CCpu6502::PullPcl,
				&CCpu6502::PullPch,
				&CCpu6502::RTS, },
				6, LSN_AM_IMPLIED, 1, LSN_I_RTS,
		},
		{	// 61
			LSN_INDIRECT_X_R( ADC, ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 62
			{	// Jams the machine.
				&CCpu6502::NOP, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 63
			LSN_INDIRECT_X_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
	};

#include "LSNInstMetaData.inl"					/**< Metadata for the instructions (for assembly and disassembly etc.) */

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

	/** Reads the next instruction byte and throws it away, increasing PC. */
	void CCpu6502::NopAndIncPcAndFinish() {
		m_pbBus->CpuRead( pc.PC++ );

		// Last cycle in the instruction.
		LSN_FINISH_INST;
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

	/** 3rd cycle of PLA/PLP/RTI/RTS. */
	void CCpu6502::PLA_PLP_RTI_RTS_Cycle3() {
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
	void CCpu6502::PushPchWithBFlag() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3  $0100,S  W  push PCH on stack (with B flag set), decrement S
		LSN_PUSH( pc.ui8Bytes[1] );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ), true );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes PCL, decrements S. */
	void CCpu6502::PushPcl() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  4  $0100,S  W  push PCL on stack, decrement S
		LSN_PUSH( pc.ui8Bytes[0] );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes status with B. */
	void CCpu6502::PushStatusAndBAndSetAddressByIrq() {
		LSN_PUSH( m_ui8Status | uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) );
		// It is also at this point that the branch vector is determined.  Store it in LSN_CPU_CONTEXT::a.ui16Address.
		m_pccCurContext->a.ui16Address = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ )) ? uint16_t( LSN_VECTORS::LSN_V_NMI ) : uint16_t( LSN_VECTORS::LSN_V_IRQ_BRK );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes status an decrements S. */
	void CCpu6502::PushStatus() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  $0100,S  W  push P on stack, decrement S
		LSN_PUSH( m_ui8Status );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pushes status without B. */
	void CCpu6502::PushStatusAndSetAddressByIrq() {
		LSN_PUSH( m_ui8Status );
		// It is also at this point that the branch vector is determined.  Store it in LSN_CPU_CONTEXT::a.ui16Address.
		m_pccCurContext->a.ui16Address = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ )) ? uint16_t( LSN_VECTORS::LSN_V_NMI ) : uint16_t( LSN_VECTORS::LSN_V_IRQ_BRK );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pulls status, ignoring B. */
	void CCpu6502::PullStatusWithoutB() {
		const uint8_t ui8Mask = (uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) | (1 << 5));
		m_ui8Status = (LSN_POP() & ~ui8Mask) | (m_ui8Status & ui8Mask);
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pulls status. */
	void CCpu6502::PullStatus() {
		m_ui8Status = LSN_POP();
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pulls PCL. */
	void CCpu6502::PullPcl() {
		pc.ui8Bytes[0] = LSN_POP();
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pulls PCH. */
	void CCpu6502::PullPch() {
		pc.ui8Bytes[1] = LSN_POP();
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

	/** Performs A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		const uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		uint16_t ui16Result = uint16_t( A ) + uint16_t( ui8Tmp ) + (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ));
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), (~(A ^ ui8Tmp) & (A ^ ui16Result) & 0x80) == 0 );
		A = uint8_t( ui16Result );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), ui16Result > 0xFF );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), ui16Result == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Performs A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::ADC_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// Read from effective address,
		// fix high byte of effective address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
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
			uint16_t ui16Result = uint16_t( A ) + uint16_t( ui8Tmp ) + (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ));
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), (~(A ^ ui8Tmp) & (A ^ ui16Result) & 0x80) == 0 );
			A = uint8_t( ui16Result );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), ui16Result > 0xFF );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), ui16Result == 0x00 );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** Performs A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::ADC_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( pc.PC++ );
		uint16_t ui16Result = uint16_t( A ) + uint16_t( ui8Tmp ) + (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ));
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), (~(A ^ ui8Tmp) & (A ^ ui16Result) & 0x80) == 0 );
		A = uint8_t( ui16Result );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), ui16Result > 0xFF );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), ui16Result == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from PC and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::ANC_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A &= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x80) != 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		const uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A &= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::AND_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// Read from effective address,
		// fix high byte of effective address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
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
			A &= ui8Tmp;
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** Fetches from PC and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::AND_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( pc.PC++ );
		A &= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Performs OP <<= 1.  Sets flags C, N, and Z. */
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

	/** Performs A <<= 1.  Sets flags C, N, and Z. */
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

	/** Performs A &= OP; A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::ASR_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( pc.PC++ );
		A &= ui8Tmp;
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (A & 0x01) != 0 );
		A >>= 1;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), false );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Sets flags N, V and Z according to a bit test. */
	void CCpu6502::BIT_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		const uint8_t ui8Op = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), (ui8Op & A) == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (ui8Op & (1 << 7)) != 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), (ui8Op & (1 << 6)) != 0x00 );
		LSN_FINISH_INST;
	}

	/** Pops the high byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the high byte of PC. */
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
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->CpuRead( pc.PC );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), false );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Clears the IRQ flag. */
	void CCpu6502::CLI() {
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->CpuRead( pc.PC );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ ), false );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		const uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A ^= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::EOR_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// Read from effective address,
		// fix high byte of effective address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
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
			A ^= ui8Tmp;
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** Fetches from PC and performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::EOR_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( pc.PC++ );
		A ^= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Follows FetchLowAddrByteAndIncPc() and copies the read value into the low byte of PC after fetching the high byte. */
	void CCpu6502::JMP_Abs() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3    PC     R  copy low address byte to PCL, fetch high address
		//                 byte to PCH
		m_pccCurContext->j.ui8Bytes[1] = m_pbBus->CpuRead( pc.PC );
		pc.PC = m_pccCurContext->j.ui16JmpTarget;

		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** JSR (Jump to Sub-Routine). */
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

	/** Performs OP >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::LSR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x01) != 0 );
		m_pccCurContext->ui8Operand >>= 1;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), m_pccCurContext->ui8Operand == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), false );
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Performs A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::LSR_Imp() {
		// We have a discarded read here.
		m_pbBus->CpuRead( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		

		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (A & 0x01) != 0 );
		A >>= 1;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), false );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Reads the next instruction byte and throws it away. */
	void CCpu6502::NOP() {
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->CpuRead( pc.PC );

		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Reads LSN_CPU_CONTEXT::a.ui16Address and throws it away. */
	void CCpu6502::NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );

		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** No operation. */
	void CCpu6502::NOP_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// Read from effective address,
		// fix high byte of effective address.
		m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
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
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** Fetches from PC and performs A = A | OP.  Sets flags N and Z. */
	void CCpu6502::ORA_Imm() {
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->CpuRead( pc.PC++ );
		A |= ui8Tmp;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
	void CCpu6502::ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		const uint8_t ui8Op = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
		A |= ui8Op;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
	void CCpu6502::ORA_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// Read from effective address,
		// fix high byte of effective address.
		const uint8_t ui8Op = m_pbBus->CpuRead( m_pccCurContext->a.ui16Address );
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
			A |= ui8Op;
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), A == 0x00 );
			SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
			// Last cycle in the instruction.
			LSN_FINISH_INST;
		}
	}

	/** Pushes the accumulator. */
	void CCpu6502::PHA() {
		LSN_PUSH( A );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Pushes the status byte. */
	void CCpu6502::PHP() {
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

	/** Pulls the status byte. */
	void CCpu6502::PLP() {
		const uint8_t ui8Mask = (uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) | (1 << 5));
		m_ui8Status = (LSN_POP() & ~ui8Mask) | (m_ui8Status & ui8Mask);
		//m_ui8Status = LSN_POP();
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Performs OP = (OP << 1) | (OP >> 7); A = A | (OP).  Sets flags C, N and Z. */
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

	/** Performs OP = (OP << 1) | (OP >> 7).  Sets flags C, N, and Z. */
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

	/** Performs A = (A << 1) | (A >> 7).  Sets flags C, N, and Z. */
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

	/** Performs OP = (OP >> 1) | (OP << 7); A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x01) != 0 );		// This affects whether the carry bit gets added below.
		m_pccCurContext->ui8Operand = (m_pccCurContext->ui8Operand >> 1) | (m_pccCurContext->ui8Operand << 7);

		const uint8_t ui8Tmp = m_pccCurContext->ui8Operand;
		uint16_t ui16Result = uint16_t( A ) + uint16_t( ui8Tmp ) + (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ));
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), (~(A ^ ui8Tmp) & (A ^ ui16Result) & 0x80) == 0 );
		A = uint8_t( ui16Result );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), ui16Result > 0xFF );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), ui16Result == 0x00 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), (A & 0x80) != 0 );
		
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

	/** Pops into PCH. */
	void CCpu6502::RTI() {
		//  #  address R/W description
		// --- ------- --- -----------------------------------------------
		//  6  $0100,S  R  pull PCH from stack
		pc.ui8Bytes[1] = LSN_POP();
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Reads PC and increments it. */
	void CCpu6502::RTS() {
		m_pbBus->CpuRead( pc.PC++ );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Sets the carry flag. */
	void CCpu6502::SEC() {
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->CpuRead( pc.PC );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), true );
		// Last cycle in the instruction.
		LSN_FINISH_INST;
	}

	/** Performs OP = (OP << 1); A = A | (OP).  Sets flags C, N and Z. */
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

	/** Performs OP = (OP >> 1); A = A ^ (OP).  Sets flags C, N and Z. */
	void CCpu6502::SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), (m_pccCurContext->ui8Operand & 0x01) != 0 );
		m_pccCurContext->ui8Operand = (m_pccCurContext->ui8Operand >> 1);
		A ^= m_pccCurContext->ui8Operand;
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO ), m_pccCurContext->ui8Operand == 0 );
		SetBit( m_ui8Status, uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), false );
		
		LSN_ADVANCE_CONTEXT_COUNTERS;
	}

}	// namespace lsn

#undef LSN_POP
#undef LSN_PUSH
#undef LSN_ADVANCE_CONTEXT_COUNTERS