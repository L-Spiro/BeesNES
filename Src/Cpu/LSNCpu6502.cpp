/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
 * 
 * https://www.nesdev.org/6502_cpu.txt
 * http://www.oxyron.de/html/opcodes02.html
 * http://www.6502.org/tutorials/6502opcodes.html
 * http://users.telenet.be/kim1-6502/6502/proman.html
 * http://problemkaputt.de/everynes.htm#cpu65xxmicroprocessor
 * https://www.masswerk.at/6502/6502_instruction_set.html
 */


#include "LSNCpu6502.h"
#include "../Bus/LSNBus.h"

#define LSN_ADVANCE_CONTEXT_COUNTERS_BY( AMT )											/*m_ccCurContext.ui8Cycle += AMT;*/	\
																						m_ccCurContext.ui8FuncIdx += AMT;
																						/*++m_ccCurContext.ui8Cycle*/
#define LSN_ADVANCE_CONTEXT_COUNTERS													LSN_ADVANCE_CONTEXT_COUNTERS_BY( 1 )
#define LSN_PUSH( VAL )																	m_pbBus->Write( 0x100 + S--, (VAL) )
#define LSN_POP()																		m_pbBus->Read( 0x100 + ++S )
#define LSN_FINISH_INST																	m_pfTickFunc = m_pfTickFuncCopy = &CCpu6502::Tick_NextInstructionStd

#define LSN_INDIRECT_X_R( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::FUNC, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::ReadFromEffectiveAddress_IzX_IzY_ZpX_AbX_AbY_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 8, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_W( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::FUNC, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME

#define LSN_INDIRECT_Y_R( NAME, FUNC1, FUNC2 )											{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::FetchEffectiveAddressLow_IzY, &CCpu6502::FetchEffectiveAddressHigh_IzY, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::FetchEffectiveAddressLow_IzY, &CCpu6502::FetchEffectiveAddressHigh_IzY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX_AbY<false>, &CCpu6502::ReadFromEffectiveAddress_IzX_IzY_ZpX_AbX_AbY_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 8, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_Y_W( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::FetchEffectiveAddressLow_IzY, &CCpu6502::FetchEffectiveAddressHigh_IzY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX_AbY<false>, &CCpu6502::FUNC, }, 6, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::FUNC, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_RMW( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromEffectiveAddress_Zp, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 5, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::FUNC, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_R( NAME, FUNC )													{ &CCpu6502::FetchLowAddrByteAndIncPc, &CCpu6502::FetchHighAddrByteAndIncPc, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_RMW( NAME, FUNC )													{ &CCpu6502::FetchLowAddrByteAndIncPc, &CCpu6502::FetchHighAddrByteAndIncPc, &CCpu6502::ReadFromEffectiveAddress_IzX_IzY_ZpX_AbX_AbY_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 6, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_W( NAME, FUNC )													{ &CCpu6502::FetchLowAddrByteAndIncPc, &CCpu6502::FetchHighAddrByteAndIncPc, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME

#define LSN_IMMEDIATE( NAME, FUNC )														{ &CCpu6502::FUNC, }, 2, LSN_AM_IMMEDIATE, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_X_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpX, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpX, &CCpu6502::ReadFromEffectiveAddress_IzX_IzY_ZpX_AbX_AbY_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 6, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_X_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpX, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_Y_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpY, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_Y_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Zp, &CCpu6502::ReadFromAddressAndAddX_ZpY, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_Y_R( NAME, FUNC1, FUNC2 )											{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddY, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX_AbY<false>, &CCpu6502::ReadFromEffectiveAddress_IzX_IzY_ZpX_AbX_AbY_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_Y_W( NAME, FUNC )													{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddY, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX_AbY<false>, &CCpu6502::FUNC, }, 5, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME

#define LSN_ABSOLUTE_X_R( NAME, FUNC1, FUNC2 )											{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddX, &CCpu6502::FUNC1, &CCpu6502::FUNC2, }, 4, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddX, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX_AbY<false>, &CCpu6502::ReadFromEffectiveAddress_IzX_IzY_ZpX_AbX_AbY_Abs, &CCpu6502::FUNC, &CCpu6502::FinalWriteCycle, }, 7, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_X_W( NAME, FUNC )													{ &CCpu6502::FetchLowAddrByteAndIncPc_WriteImm, &CCpu6502::FetchHighAddrByteAndIncPcAndAddX, &CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX_AbY<false>, &CCpu6502::FUNC, }, 5, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME

#define LSN_BRANCH( NAME, COND, VAL )													{ &CCpu6502::Branch_Cycle2<uint8_t( LSN_STATUS_FLAGS::COND ), VAL>, &CCpu6502::Branch_Cycle3, &CCpu6502::Branch_Cycle4, }, 2, LSN_AM_RELATIVE, 2, LSN_I_ ## NAME,

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

namespace lsn {

	// == Members.
	CCpu6502::LSN_INSTR CCpu6502::m_iInstructionSet[256+2] = {							/**< The instruction set. */
		/** 00-07 */
		{	// 00
			{
				&CCpu6502::ReadNextInstByteAndDiscardAndIncPc,
				&CCpu6502::PushPch,
				&CCpu6502::PushPcl,
				&CCpu6502::PushStatusAndBAndSetAddressByIrq,
				&CCpu6502::CopyVectorPcl,
				&CCpu6502::CopyVectorPch, },											// Fetches from m_ccCurContext.a.ui16Address and writes to the high byte of PC.
				7, LSN_AM_IMPLIED, 1, LSN_I_BRK,
		},
		{	// 01
			LSN_INDIRECT_X_R( ORA, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 02
			{	// Jams the machine.
				&CCpu6502::JAM, },
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
			LSN_BRANCH( BPL, LSN_SF_NEGATIVE, 0 )										// Branch if N == 0.
		},
		{	// 11
			LSN_INDIRECT_Y_R( ORA, ORA_IzY_AbX_AbY_1, ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 12
			{	// Jams the machine.
				&CCpu6502::JAM, },
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
				&CCpu6502::NOP_Imp, },
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
				&CCpu6502::JAM, },
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
			LSN_BRANCH( BMI, LSN_SF_NEGATIVE, 1 )										// Branch if N == 1.
		},
		{	// 31
			LSN_INDIRECT_Y_R( AND, AND_IzY_AbX_AbY_1, AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 32
			{	// Jams the machine.
				&CCpu6502::JAM, },
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
				&CCpu6502::NOP_Imp, },
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
				&CCpu6502::JAM, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 43
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
			LSN_BRANCH( BVC, LSN_SF_OVERFLOW, 0 )										// Branch if V == 0.
		},
		{	// 51
			LSN_INDIRECT_Y_R( EOR, EOR_IzY_AbX_AbY_1, EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 52
			{	// Jams the machine.
				&CCpu6502::JAM, },
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
				&CCpu6502::NOP_Imp, },
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
				&CCpu6502::JAM, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 63
			LSN_INDIRECT_X_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 64
			LSN_ZERO_PAGE_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 65
			LSN_ZERO_PAGE_R( ADC, ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 66
			LSN_ZERO_PAGE_RMW( ROR, ROR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 67
			LSN_ZERO_PAGE_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 68-6F */
		{	// 68
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::PLA_PLP_RTI_RTS_Cycle3,
				&CCpu6502::PLA, },
				4, LSN_AM_IMPLIED, 1, LSN_I_PLA,
		},
		{	// 69
			LSN_IMMEDIATE( ADC, ADC_Imm )
		},
		{	// 6A
			{
				&CCpu6502::ROR_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_ROR,
		},
		{	// 6B
			LSN_IMMEDIATE( ARR, ARR_Imm )
		},
		{	// 6C
			{
				&CCpu6502::FetchLowAddrByteAndIncPc,
				&CCpu6502::FetchHighAddrByteAndIncPc,
				&CCpu6502::Jmp_Ind_Cycle4,
				&CCpu6502::JMP_Ind, },
				5, LSN_AM_INDIRECT, 3, LSN_I_JMP,
		},
		{	// 6D
			LSN_ABSOLUTE_R( ADC, ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 6E
			LSN_ABSOLUTE_RMW( ROR, ROR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 6F
			LSN_ABSOLUTE_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 70-77 */
		{	// 70
			LSN_BRANCH( BVS, LSN_SF_OVERFLOW, 1 )										// Branch if V == 1.
		},
		{	// 71
			LSN_INDIRECT_Y_R( ADC, ADC_IzY_AbX_AbY_1, ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 72
			{	// Jams the machine.
				&CCpu6502::JAM, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 73
			LSN_INDIRECT_Y_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 74
			LSN_ZERO_PAGE_X_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 75
			LSN_ZERO_PAGE_X_R( ADC, ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 76
			LSN_ZERO_PAGE_X_RMW( ROR, ROR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 77
			LSN_ZERO_PAGE_X_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 78-7F */
		{	// 78
			{
				&CCpu6502::SEI, },
				2, LSN_AM_IMPLIED, 1, LSN_I_SEI,
		},
		{	// 79
			LSN_ABSOLUTE_Y_R( ADC, ADC_IzY_AbX_AbY_1, ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 7A
			{	// Undocumented command.
				&CCpu6502::NOP_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
		},
		{	// 7B
			LSN_ABSOLUTE_Y_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 7C
			LSN_ABSOLUTE_X_R( NOP, NOP_IzY_AbX_AbY_1, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 7D
			LSN_ABSOLUTE_X_R( ADC, ADC_IzY_AbX_AbY_1, ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 7E
			LSN_ABSOLUTE_X_RMW( ROR, ROR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 7F
			LSN_ABSOLUTE_X_RMW( RRA, RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 80-87 */
		{	// 80
			LSN_IMMEDIATE( NOP, NOP_Imm )
		},
		{	// 81
			LSN_INDIRECT_X_W( STA, STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 82
			LSN_IMMEDIATE( NOP, NOP_Imm )
		},
		{	// 83
			LSN_INDIRECT_X_W( SAX, SAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 84
			LSN_ZERO_PAGE_W( STY, STY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 85
			LSN_ZERO_PAGE_W( STA, STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 86
			LSN_ZERO_PAGE_W( STX, STX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 87
			LSN_ZERO_PAGE_W( SAX, SAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 88-8F */
		{	// 88
			{
				&CCpu6502::DEY, },
				2, LSN_AM_IMPLIED, 1, LSN_I_DEY,
		},
		{	// 89
			LSN_IMMEDIATE( NOP, NOP_Imm )
		},
		{	// 8A
			{
				&CCpu6502::TXA, },
				2, LSN_AM_IMPLIED, 1, LSN_I_TXA,
		},
		{	// 8B
			LSN_IMMEDIATE( ANE, ANE )
		},
		{	// 8C
			LSN_ABSOLUTE_W( STY, STY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 8D
			LSN_ABSOLUTE_W( STA, STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 8E
			LSN_ABSOLUTE_W( STX, STX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 8F
			LSN_ABSOLUTE_W( SAX, SAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 90-97 */
		{	// 90
			LSN_BRANCH( BCC, LSN_SF_CARRY, 0 )										// Branch if C == 0.
		},
		{	// 91
			LSN_INDIRECT_Y_W( STA, STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 92
			{	// Jams the machine.
				&CCpu6502::JAM, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// 93
			LSN_INDIRECT_Y_W( SHA, SHA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 94
			LSN_ZERO_PAGE_X_W( STY, STY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 95
			LSN_ZERO_PAGE_X_W( STA, STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 96
			LSN_ZERO_PAGE_Y_W( STX, STX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 97
			LSN_ZERO_PAGE_Y_W( SAX, SAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** 98-9F */
		{	// 98
			{
				&CCpu6502::TYA, },
				2, LSN_AM_IMPLIED, 1, LSN_I_TYA,
		},
		{	// 99
			LSN_ABSOLUTE_Y_W( STA, STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 9A
			{
				&CCpu6502::TXS, },
				2, LSN_AM_IMPLIED, 1, LSN_I_TXS,
		},
		{	// 9B
			LSN_ABSOLUTE_Y_W( SHS, SHS )
		},
		{	// 9C
			LSN_ABSOLUTE_X_W( SHY, SHY )
		},
		{	// 9D
			LSN_ABSOLUTE_X_W( STA, STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// 9E
			LSN_ABSOLUTE_Y_W( SHX, SHX )
		},
		{	// 9F
			LSN_ABSOLUTE_Y_W( SHA, SHA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** A0-A7 */
		{	// A0
			LSN_IMMEDIATE( LDY, LDY_Imm )
		},
		{	// A1
			LSN_INDIRECT_X_R( LDA, LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// A2
			LSN_IMMEDIATE( LDX, LDX_Imm )
		},
		{	// A3
			LSN_INDIRECT_X_R( LAX, LAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// A4
			LSN_ZERO_PAGE_R( LDY, LDY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// A5
			LSN_ZERO_PAGE_R( LDA, LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// A6
			LSN_ZERO_PAGE_R( LDX, LDX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// A7
			LSN_ZERO_PAGE_R( LAX, LAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** A8-AF */
		{	// A8
			{
				&CCpu6502::TAY, },
				2, LSN_AM_IMPLIED, 1, LSN_I_TAY,
		},
		{	// A9
			LSN_IMMEDIATE( LDA, LDA_Imm )
		},
		{	// AA
			{
				&CCpu6502::TAX, },
				2, LSN_AM_IMPLIED, 1, LSN_I_TAX,
		},
		{	// AB
			LSN_IMMEDIATE( LXA, LXA )
		},
		{	// AC
			LSN_ABSOLUTE_R( LDY, LDY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// AD
			LSN_ABSOLUTE_R( LDA, LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// AE
			LSN_ABSOLUTE_R( LDX, LDX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// AF
			LSN_ABSOLUTE_R( LAX, LAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** B0-B7 */
		{	// B0
			LSN_BRANCH( BCS, LSN_SF_CARRY, 1 )										// Branch if C == 1.
		},
		{	// B1
			LSN_INDIRECT_Y_R( LDA, LDA_IzY_AbX_AbY_1, LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// B2
			{	// Jams the machine.
				&CCpu6502::JAM, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// B3
			LSN_INDIRECT_Y_R( LAX, LAX_IzY_AbX_AbY_1, LAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// B4
			LSN_ZERO_PAGE_X_R( LDY, LDY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// B5
			LSN_ZERO_PAGE_X_R( LDA, LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// B6
			LSN_ZERO_PAGE_Y_R( LDX, LDX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// B7
			LSN_ZERO_PAGE_Y_R( LAX, LAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** B8-BF */
		{	// B8
			{
				&CCpu6502::CLV, },
				2, LSN_AM_IMPLIED, 1, LSN_I_CLV,
		},
		{	// B9
			LSN_ABSOLUTE_Y_R( LDA, LDA_IzY_AbX_AbY_1, LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// BA
			{
				&CCpu6502::TSX, },
				2, LSN_AM_IMPLIED, 1, LSN_I_TSX,
		},
		{	// BB
			LSN_ABSOLUTE_Y_R( LAS, LAS_IzY_AbX_AbY_1, LAS_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// BC
			LSN_ABSOLUTE_X_R( LDY, LDY_IzY_AbX_AbY_1, LDY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// BD
			LSN_ABSOLUTE_X_R( LDA, LDA_IzY_AbX_AbY_1, LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// BE
			LSN_ABSOLUTE_Y_R( LDX, LDX_IzY_AbX_AbY_1, LDX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// BF
			LSN_ABSOLUTE_Y_R( LAX, LAX_IzY_AbX_AbY_1, LAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** C0-C7 */
		{	// C0
			LSN_IMMEDIATE( CPY, CPY_Imm )
		},
		{	// C1
			LSN_INDIRECT_X_R( CMP, CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// C2
			LSN_IMMEDIATE( NOP, NOP_Imm )
		},
		{	// C3
			LSN_INDIRECT_X_RMW( DCP, DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// C4
			LSN_ZERO_PAGE_R( CPY, CPY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// C5
			LSN_ZERO_PAGE_R( CMP, CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// C6
			LSN_ZERO_PAGE_RMW( DEC, DEC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// C7
			LSN_ZERO_PAGE_RMW( DCP, DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** C8-CF */
		{	// C8
			{
				&CCpu6502::INY, },
				2, LSN_AM_IMPLIED, 1, LSN_I_INY,
		},
		{	// C9
			LSN_IMMEDIATE( CMP, CMP_Imm )
		},
		{	// CA
			{
				&CCpu6502::DEX, },
				2, LSN_AM_IMPLIED, 1, LSN_I_DEX,
		},
		{	// CB
			LSN_IMMEDIATE( SBX, SBX )
		},
		{	// CC
			LSN_ABSOLUTE_R( CPY, CPY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// CD
			LSN_ABSOLUTE_R( CMP, CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// CE
			LSN_ABSOLUTE_RMW( DEC, DEC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// CF
			LSN_ABSOLUTE_RMW( DCP, DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		
		/** D0-D7 */
		{	// D0
			LSN_BRANCH( BNE, LSN_SF_ZERO, 0 )										// Branch if Z == 0.
		},
		{	// D1
			LSN_INDIRECT_Y_R( CMP, CMP_IzY_AbX_AbY_1, CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// D2
			{	// Jams the machine.
				&CCpu6502::JAM, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// D3
			LSN_INDIRECT_Y_RMW( DCP, DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// D4
			LSN_ZERO_PAGE_X_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// D5
			LSN_ZERO_PAGE_X_R( CMP, CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// D6
			LSN_ZERO_PAGE_X_RMW( DEC, DEC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// D7
			LSN_ZERO_PAGE_X_RMW( DCP, DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** D8-DF */
		{	// D8
			{
				&CCpu6502::CLD, },
				2, LSN_AM_IMPLIED, 1, LSN_I_CLD,
		},
		{	// D9
			LSN_ABSOLUTE_Y_R( CMP, CMP_IzY_AbX_AbY_1, CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// DA
			{
				&CCpu6502::NOP_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
		},
		{	// DB
			LSN_ABSOLUTE_Y_RMW( DCP, DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// DC
			LSN_ABSOLUTE_X_R( NOP, NOP_IzY_AbX_AbY_1, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// DD
			LSN_ABSOLUTE_X_R( CMP, CMP_IzY_AbX_AbY_1, CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// DE
			LSN_ABSOLUTE_X_RMW( DEC, DEC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// DF
			LSN_ABSOLUTE_X_RMW( DCP, DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** E0-E7 */
		{	// E0
			LSN_IMMEDIATE( CPX, CPX_Imm )
		},
		{	// E1
			LSN_INDIRECT_X_R( SBC, SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// E2
			LSN_IMMEDIATE( NOP, NOP_Imm )
		},
		{	// E3
			LSN_INDIRECT_X_RMW( ISB, ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// E4
			LSN_ZERO_PAGE_R( CPX, CPX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// E5
			LSN_ZERO_PAGE_R( SBC, SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// E6
			LSN_ZERO_PAGE_RMW( INC, INC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// E7
			LSN_ZERO_PAGE_RMW( ISB, ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** E8-EF */
		{	// E8
			{
				&CCpu6502::INX, },
				2, LSN_AM_IMPLIED, 1, LSN_I_INX,
		},
		{	// E9
			LSN_IMMEDIATE( SBC, SBC_Imm )
		},
		{	// EA
			{
				&CCpu6502::NOP_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
		},
		{	// EB
			LSN_IMMEDIATE( SBC, SBC_Imm )
		},
		{	// EC
			LSN_ABSOLUTE_R( CPX, CPX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// ED
			LSN_ABSOLUTE_R( SBC, SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// EE
			LSN_ABSOLUTE_RMW( INC, INC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// EF
			LSN_ABSOLUTE_RMW( ISB, ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** F0-F7 */
		{	// F0
			LSN_BRANCH( BEQ, LSN_SF_ZERO, 1 )										// Branch if Z == 0.
		},
		{	// F1
			LSN_INDIRECT_Y_R( SBC, SBC_IzY_AbX_AbY_1, SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// F2
			{	// Jams the machine.
				&CCpu6502::JAM, },
				2, LSN_AM_IMPLIED, 1, LSN_I_JAM,
		},
		{	// F3
			LSN_INDIRECT_Y_RMW( ISB, ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// F4
			LSN_ZERO_PAGE_X_R( NOP, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// F5
			LSN_ZERO_PAGE_X_R( SBC, SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// F6
			LSN_ZERO_PAGE_X_RMW( INC, INC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// F7
			LSN_ZERO_PAGE_X_RMW( ISB, ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		/** F8-FF */
		{	// F8
			{
				&CCpu6502::SED, },
				2, LSN_AM_IMPLIED, 1, LSN_I_SED,
		},
		{	// F9
			LSN_ABSOLUTE_Y_R( SBC, SBC_IzY_AbX_AbY_1, SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// FA
			{
				&CCpu6502::NOP_Imp, },
				2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
		},
		{	// FB
			LSN_ABSOLUTE_Y_RMW( ISB, ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// FC
			LSN_ABSOLUTE_X_R( NOP, NOP_IzY_AbX_AbY_1, NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// FD
			LSN_ABSOLUTE_X_R( SBC, SBC_IzY_AbX_AbY_1, SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// FE
			LSN_ABSOLUTE_X_RMW( INC, INC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},
		{	// FF
			LSN_ABSOLUTE_X_RMW( ISB, ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs )
		},

		{	// 100
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::PushPch,
				&CCpu6502::PushPcl,
				&CCpu6502::PushStatusAndNoBAndSetAddressByIrq,
				&CCpu6502::CopyVectorPcl,
				&CCpu6502::CopyVectorPch, },											// Fetches from m_ccCurContext.a.ui16Address and writes to the high byte of PC.
				7, LSN_AM_IMPLIED, 1, LSN_I_NMI,
		},
		{	// 101
			{
				&CCpu6502::ReadNextInstByteAndDiscard,
				&CCpu6502::PushPch,
				&CCpu6502::PushPcl,
				&CCpu6502::PushStatusAndNoBAndSetAddressByIrq,
				&CCpu6502::CopyVectorPcl,
				&CCpu6502::CopyVectorPch, },											// Fetches from m_ccCurContext.a.ui16Address and writes to the high byte of PC.
				7, LSN_AM_IMPLIED, 1, LSN_I_IRQ,
		},
	};

#include "LSNInstMetaData.inl"					/**< Metadata for the instructions (for assembly and disassembly etc.) */

	// == Various constructors.
	CCpu6502::CCpu6502( CCpuBus * _pbBus ) :
		CCpuBase( _pbBus ),
		m_pmbMapper( nullptr ),
		m_pfTickFunc( &CCpu6502::Tick_NextInstructionStd ),
		m_pipPoller( nullptr ),
		A( 0 ),
		S( 0xFD ),
		X( 0 ),
		Y( 0 ),
		m_ui16DmaAddress( 0 ),
		m_ui8DmaPos( 0 ),
		m_ui8DmaValue( 0 ),
		m_bNmiStatusLine( false ),
		m_bHandleNmi( false ),
		m_bIrqStatusLine( false ),
		m_bHandleIrq( false ) {
		pc.PC = 0xC000;
		m_ui8Status = 0x04;
		std::memset( &m_ccCurContext, 0, sizeof( m_ccCurContext ) );
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
		m_ui64CycleCount = 0;
		A = 0;
		S = 0xFD;
		X = Y = 0;
		m_ui8Status = 0x04;
		std::memset( m_ui8Inputs, 0, sizeof( m_ui8Inputs ) );
		std::memset( m_ui8InputsState, 0, sizeof( m_ui8InputsState ) );
		std::memset( m_ui8InputsPoll, 0, sizeof( m_ui8InputsPoll ) );
	}

	/**
	 * Performs an "analog" reset, allowing previous data to remain.
	 */
	void CCpu6502::ResetAnalog() {
		//m_ccCurContext.bActive = false;
		m_pfTickFunc = m_pfTickFuncCopy = &CCpu6502::Tick_NextInstructionStd;
		S -= 3;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ ), true>( m_ui8Status );

		pc.PC = m_pbBus->Read( 0xFFFC ) | (m_pbBus->Read( 0xFFFD ) << 8);

		m_bHandleNmi = m_bNmiStatusLine = false;
		m_bHandleIrq = m_bIrqStatusLine = false;
	}

	/**
	 * Performs a single cycle update.
	 */
	void CCpu6502::Tick() {
		m_pmbMapper->Tick();
		(this->*m_pfTickFunc)();

		m_bHandleNmi |= m_bNmiStatusLine;
		m_bHandleIrq |= m_bIrqStatusLine;

		++m_ui64CycleCount;
	}

	/**
	 * Applies the CPU's memory mapping t the bus.
	 */
	void CCpu6502::ApplyMemoryMap() {
		// Apply the CPU memory map to the bus.
		for ( uint32_t I = LSN_CPU_START; I < (LSN_CPU_START + LSN_CPU_FULL_SIZE); ++I ) {
			m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::StdRead, this, uint16_t( ((I - LSN_CPU_START) % LSN_INTERNAL_RAM) + LSN_CPU_START ) );
			m_pbBus->SetWriteFunc( uint16_t( I ), CCpuBus::StdWrite, this, uint16_t( ((I - LSN_CPU_START) % LSN_INTERNAL_RAM) + LSN_CPU_START ) );
		}

		// DMA transfer.
		m_pbBus->SetReadFunc( 0x4014, CCpuBus::NoRead, this, 0x4014 );
		m_pbBus->SetWriteFunc( 0x4014, CCpu6502::Write4014, this, 0x4014 );

		// Controller ports.
		m_pbBus->SetReadFunc( 0x4016, CCpu6502::Read4016, this, 0 );
		m_pbBus->SetWriteFunc( 0x4016, CCpu6502::Write4016, this, 0 );
		m_pbBus->SetReadFunc( 0x4017, CCpu6502::Read4017, this, 0 );
		m_pbBus->SetWriteFunc( 0x4017, CCpu6502::Write4017, this, 0 );
	}

	/**
	 * Begins a DMA transfer.
	 * 
	 * \param _ui8Val The value written to 0x4014.
	 */
	void CCpu6502::BeginDma( uint8_t _ui8Val ) {
		m_pfTickFunc = &CCpu6502::Tick_DmaStart;
		m_ui16DmaAddress = uint16_t( _ui8Val ) << 8;
		// Leave m_pfTickFuncCopy as-is to return to it after the transfer.
	}

	/**
	 * Notifies the class that an NMI has occurred.
	 */
	void CCpu6502::Nmi() {
		m_bNmiStatusLine = true;
	}

	/**
	 * Signals an IRQ to be handled before the next instruction.
	 */
	void CCpu6502::Irq() {
		m_bIrqStatusLine = true;
	}

#ifdef _DEBUG
//#define LSN_PRINT_CYCLES
#endif	// #ifdef _DEBUG
	/** Fetches the next opcode and begins the next instruction. */
	void CCpu6502::Tick_NextInstructionStd() {
#ifdef LSN_PRINT_CYCLES
		if ( pc.PC == 0xC0AF ) {
		//if ( pc.PC == 0xE127 ) {
			volatile int gjhg = 0;
		}
		// TMP.
		static uint64_t ui64CyclesAtStart = 0;
		static uint64_t ui64LastCycles = 0;
		static uint16_t ui16LastInstr = 0;
		static uint16_t ui16LastPc = 0;

		ui16LastPc = pc.PC;
#endif	// #ifdef LSN_PRINT_CYCLES

		if ( m_bHandleNmi ) {
			m_bNmiStatusLine = false;
			BeginInst( LSN_SO_NMI );
		}
		else if ( m_bHandleIrq ) {
			m_bIrqStatusLine = false;
			BeginInst( LSN_SO_IRQ );
		}
		else {
			FetchOpcodeAndIncPc();
		}

#ifdef LSN_PRINT_CYCLES
		ui64LastCycles = m_ui64CycleCount - ui64CyclesAtStart;
		ui64CyclesAtStart = m_ui64CycleCount;
		if ( pc.PC ) {
			char szBuffer[256];
			::sprintf_s( szBuffer, "Op: %.2X (%s); Cycles: %llu; PC: %.4X", ui16LastInstr, m_smdInstMetaData[m_iInstructionSet[ui16LastInstr].iInstruction].pcName, ui64LastCycles, ui16LastPc );
			::OutputDebugStringA( szBuffer );
			::sprintf_s( szBuffer, "\tA: %.2X; X: %.2X; Y: %.2X; S: %.2X; P: %.2X; Cycle: %llu\r\n", A, X, Y, S, m_ui8Status, m_ui64CycleCount );
			::OutputDebugStringA( szBuffer );
		}
		
#endif	// #ifdef LSN_PRINT_CYCLES

#ifdef LSN_PRINT_CYCLES
		ui16LastInstr = m_ccCurContext.ui16OpCode;
#endif	// #ifdef LSN_PRINT_CYCLES
	}

	/** Performs a cycle inside an instruction. */
	void CCpu6502::Tick_InstructionCycleStd() {
		(this->*m_iInstructionSet[m_ccCurContext.ui16OpCode].pfHandler[m_ccCurContext.ui8FuncIdx])();
	}

	/** DMA start. Moves on to the DMA read/write cycle when the current CPU cycle is even (IE odd cycles take 1 extra cycle). */
	void CCpu6502::Tick_DmaStart() {
		if ( (m_ui64CycleCount & 0x1) == 0 ) {
			m_ui16DmaCounter = 256;
			m_ui8DmaPos = 0;
			m_pfTickFunc = &CCpu6502::Tick_DmaRead;
		}
	}

	/** DMA read cycle. */
	void CCpu6502::Tick_DmaRead() {
		m_ui8DmaValue = m_pbBus->Read( m_ui16DmaAddress + m_ui8DmaPos );
		m_pfTickFunc = &CCpu6502::Tick_DmaWrite;
	}

	/** DMA write cycle. */
	void CCpu6502::Tick_DmaWrite() {
		m_pbBus->Write( LSN_PR_OAMDATA, m_ui8DmaValue );
		if ( --m_ui16DmaCounter == 0 ) {
			m_pfTickFunc = m_pfTickFuncCopy;
		}
		else {
			m_pfTickFunc = &CCpu6502::Tick_DmaRead;
			++m_ui8DmaPos;
		}
	}

	/**
	 * Reads the next instruction byte and throws it away.
	 */
	void CCpu6502::ReadNextInstByteAndDiscard() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		m_pbBus->Read( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
	}

	/** Reads the next instruction byte and throws it away. */
	void CCpu6502::ReadNextInstByteAndDiscardAndIncPc() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -----------------------------------------------
		//  2    PC     R  read next instruction byte (and throw it away),
		//                 increment PC

		m_pbBus->Read( pc.PC++ );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
	}

	/** Reads the next instruction byte and throws it away, increasing PC. */
	void CCpu6502::NopAndIncPcAndFinish() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC++ );
	}

	/**
	 * Fetches a value using immediate addressing.
	 */
	void CCpu6502::FetchValueAndIncPc_Imm() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		m_ccCurContext.ui8Operand = m_pbBus->Read( pc.PC++ );
	}

	/** Fetches a pointer and increments PC .*/
	void CCpu6502::FetchPointerAndIncPc() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  2      PC       R  fetch pointer address, increment PC
		
		// Fetch pointer address, increment PC.
		m_ccCurContext.ui8Pointer = m_pbBus->Read( pc.PC++ );
	}

	/** Fetches an 8-bit address for Zero-Page dereferencing and increments PC. Stores the address in LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchAddressAndIncPc_Zp() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch address, increment PC
		
		// Fetches the value from an 8-bit address on the zero page.
		m_ccCurContext.a.ui16Address = m_pbBus->Read( pc.PC++ );
	}

	/** Fetches the low address and writes the value to the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchLowAddrByteAndIncPc_WriteImm() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  2     PC      R  fetch low byte of address, increment PC
		
		// val = PEEK(arg + Y)
		// Fetches the value from a 16-bit address anywhere in memory.
		m_ccCurContext.a.ui8Bytes[0] = m_pbBus->Read( pc.PC++ );
	}

	/** Fetches the low address value for absolute addressing but does not write the value to LSN_CPU_CONTEXT::a.ui16Address yet.  Pair with FetchHighAddrByteAndIncPc(). */
	void CCpu6502::FetchLowAddrByteAndIncPc() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  2    PC     R  fetch low address byte, increment PC
		
		// Fetches the value from a 16-bit address anywhere in memory.
		m_ccCurContext.j.ui8Bytes[0] = m_pbBus->Read( pc.PC++ );
	}

	/** Fetches the high address value for absolute/indirect addressing. */
	void CCpu6502::FetchHighAddrByteAndIncPc() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3    PC     R  copy low address byte to PCL, fetch high address
		//                 byte to PCH

		// Fetches the value from a 16-bit address anywhere in memory.
		m_ccCurContext.a.ui8Bytes[0] = m_ccCurContext.j.ui8Bytes[0];
		m_ccCurContext.a.ui8Bytes[1] = m_pbBus->Read( pc.PC++ );
	}

	/** Fetches the high address value for absolute/indirect addressing.  Adds Y to the low byte of the resulting address. */
	void CCpu6502::FetchHighAddrByteAndIncPcAndAddY() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  3     PC      R  fetch high byte of address,
		//                   add index register to low address byte,
		//                   increment PC

		// val = PEEK(arg + Y)
		m_ccCurContext.a.ui8Bytes[1] = m_pbBus->Read( pc.PC++ );
		m_ccCurContext.j.ui16JmpTarget = static_cast<uint16_t>(m_ccCurContext.a.ui16Address + Y);
		m_ccCurContext.a.ui8Bytes[0] = m_ccCurContext.j.ui8Bytes[0];
	}

	/** Fetches the high address value for absolute/indirect addressing.  Adds X to the low byte of the resulting address. */
	void CCpu6502::FetchHighAddrByteAndIncPcAndAddX() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  3    PC       R  fetch high byte of address,
		//                   add index register X to low address byte,
		//                   increment PC

		// val = PEEK(arg + X)
		m_ccCurContext.a.ui8Bytes[1] = m_pbBus->Read( pc.PC++ );
		m_ccCurContext.j.ui16JmpTarget = static_cast<uint16_t>(m_ccCurContext.a.ui16Address + X);
		m_ccCurContext.a.ui8Bytes[0] = m_ccCurContext.j.ui8Bytes[0];
	}

	/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
	void CCpu6502::ReadFromEffectiveAddress_IzX_IzY_ZpX_AbX_AbY_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  4  address  R  read from effective address
		m_ccCurContext.ui8Operand = m_pbBus->Read( m_ccCurContext.a.ui16Address );
	}

	/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
	void CCpu6502::ReadFromEffectiveAddress_Zp() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3  address  R  read from effective address
		
		// Fetches the value from an 8-bit address on the zero page.
		m_ccCurContext.ui8Operand = m_pbBus->Read( m_ccCurContext.a.ui16Address );
	}

	/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores the result into LSN_CPU_CONTEXT::ui8Pointer.  Preceded by FetchPointerAndIncPc(). */
	void CCpu6502::ReadAtPointerAddress() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		m_ccCurContext.ui8Pointer = m_pbBus->Read( m_ccCurContext.ui8Pointer );
	}

	/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores (LSN_CPU_CONTEXT::ui8Pointer+X)&0xFF into LSN_CPU_CONTEXT::a.ui16Address.  Preceded by FetchPointerAndIncPc(). */
	void CCpu6502::ReadFromAddressAndAddX_ZpX() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  3   address   R  read from address, add index register to it

		// val = PEEK((arg + X) % 256)
		// This is the:
		//	(arg + X) % 256
		//	part.
		m_pbBus->Read( m_ccCurContext.a.ui16Address );	// This read cycle reads from the address prior to it being adjusted by X.
		m_ccCurContext.a.ui16Address = uint8_t( m_ccCurContext.a.ui16Address + X );
	}

	/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores (LSN_CPU_CONTEXT::ui8Pointer+Y)&0xFF into LSN_CPU_CONTEXT::a.ui16Address.  Preceded by FetchPointerAndIncPc(). */
	void CCpu6502::ReadFromAddressAndAddX_ZpY() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  3   address   R  read from address, add index register to it

		// val = PEEK((arg + Y) % 256)
		// This is the:
		//	(arg + Y) % 256
		//	part.
		m_pbBus->Read( m_ccCurContext.a.ui16Address );	// This read cycle reads from the address prior to it being adjusted by X.
		m_ccCurContext.a.ui16Address = uint8_t( m_ccCurContext.a.ui16Address + Y );
	}

	/** Reads from LSN_CPU_CONTEXT::a.ui16Address+Y, stores the result into LSN_CPU_CONTEXT::ui8Operand. */
	void CCpu6502::AddYAndReadAddress_IndY() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		// This is a read cycle and will read whatever the effective address is, but the effective address is updated across 2 cycles if a page boundary is crossed,
		//	meaning the first read was using an invalid address.
		// The real hardware uses the extra cycle to fix up the address and read again, so we emulate both the invalid read.
		m_ccCurContext.ui8Operand = m_pbBus->Read( (m_ccCurContext.a.ui16Address & 0xFF00) | ((m_ccCurContext.a.ui16Address + Y) & 0xFF) );
	}

	/** 4th cycle of JMP (indorect). */
	void CCpu6502::Jmp_Ind_Cycle4() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  4   pointer   R  fetch low address to latch
		m_ccCurContext.j.ui8Bytes[0] = m_pbBus->Read( m_ccCurContext.a.ui16Address );
	}

	/** 3rd cycle of JSR. */
	void CCpu6502::Jsr_Cycle3() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3  $0100,S  R  internal operation (predecrement S?)
		m_pbBus->Read( 0x100 + S/*(S - 1)*/ );
	}

	/** 4th cycle of JSR. */
	void CCpu6502::Jsr_Cycle4() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  4  $0100,S  W  push PCH on stack, decrement S
		LSN_PUSH( pc.ui8Bytes[1] );
	}

	/** 5th cycle of JSR. */
	void CCpu6502::Jsr_Cycle5() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  $0100,S  W  push PCL on stack, decrement S
		LSN_PUSH( pc.ui8Bytes[0] );
	}

	/** 3rd cycle of PLA/PLP/RTI/RTS. */
	void CCpu6502::PLA_PLP_RTI_RTS_Cycle3() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -----------------------------------------------
		//  3  $0100,S  R  increment S

		// S is incremented in LSN_POP().
		// I think the read happens before the increment.
		m_pbBus->Read( 0x100 + S/*(S + 1)*/ );
	}

	/** Pushes PCH. */
	void CCpu6502::PushPch() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		LSN_PUSH( pc.ui8Bytes[1] );
	}

	/** Pushes PCL, decrements S. */
	void CCpu6502::PushPcl() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  4  $0100,S  W  push PCL on stack, decrement S
		LSN_PUSH( pc.ui8Bytes[0] );
	}

	/** Pushes status with B. */
	void CCpu6502::PushStatusAndBAndSetAddressByIrq() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		// NMI overrides IRQ and BRK.
		LSN_PUSH( m_ui8Status | uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) );
		// It is also at this point that the branch vector is determined.  Store it in LSN_CPU_CONTEXT::a.ui16Address.
		m_ccCurContext.a.ui16Address = m_bHandleNmi ? uint16_t( LSN_VECTORS::LSN_V_NMI ) : uint16_t( LSN_VECTORS::LSN_V_IRQ_BRK );
		if ( m_bHandleNmi ) { m_bHandleNmi = false; }
	}

	/** Pushes status without B. */
	void CCpu6502::PushStatusAndNoBAndSetAddressByIrq() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		// NMI overrides IRQ and BRK.
		uint8_t ui8Status = m_ui8Status;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ), false>( ui8Status );
		LSN_PUSH( ui8Status );
		// It is also at this point that the branch vector is determined.  Store it in LSN_CPU_CONTEXT::a.ui16Address.
		m_ccCurContext.a.ui16Address = m_bHandleNmi ? uint16_t( LSN_VECTORS::LSN_V_NMI ) : uint16_t( LSN_VECTORS::LSN_V_IRQ_BRK );
		if ( m_bHandleNmi ) { m_bHandleNmi = false; }
	}

	/** Pushes status an decrements S. */
	void CCpu6502::PushStatus() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  $0100,S  W  push P on stack, decrement S
		LSN_PUSH( m_ui8Status );
	}

	/** Pulls status, ignoring B. */
	void CCpu6502::PullStatusWithoutB() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		const uint8_t ui8Mask = (uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) | uint8_t( LSN_STATUS_FLAGS::LSN_SF_RESERVED ));
		m_ui8Status = (LSN_POP() & ~ui8Mask) | (m_ui8Status & ui8Mask);
	}

	/** Pulls status. */
	void CCpu6502::PullStatus() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		m_ui8Status = LSN_POP();
	}

	/** Pulls PCL. */
	void CCpu6502::PullPcl() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		pc.ui8Bytes[0] = LSN_POP();
	}

	/** Pulls PCH. */
	void CCpu6502::PullPch() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		pc.ui8Bytes[1] = LSN_POP();
	}

	/** Reads from LSN_CPU_CONTEXT::ui8Pointer, adds X to it, stores the result in LSN_CPU_CONTEXT::ui8Pointer. */
	void CCpu6502::ReadAddressAddX_IzX() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  3    pointer    R  read from the address, add X to it

		// val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
		// This is the "(arg + X)" step, but since every cycle is a read or write there is also a superfluous read of "arg" here.
		m_pbBus->Read( m_ccCurContext.ui8Pointer );
		m_ccCurContext.ui8Pointer = static_cast<uint8_t>(m_ccCurContext.ui8Pointer + X);
	}

	/** Reads the low byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+X, store in the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchEffectiveAddressLow_IzX() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  4   pointer+X   R  fetch effective address low
		
		// val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
		// This is the:
		//	PEEK((arg + X) % 256)
		//	part.
		m_ccCurContext.a.ui8Bytes[0] = m_pbBus->Read( m_ccCurContext.ui8Pointer );
	}

	/** Reads the high byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+X+1, store in the high byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchEffectiveAddressHigh_IzX() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5  pointer+X+1  R  fetch effective address high

		// val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
		// This is the:
		//	PEEK((arg + X + 1) % 256) * 256
		//	part.
		m_ccCurContext.a.ui8Bytes[1] = m_pbBus->Read( static_cast<uint8_t>(m_ccCurContext.ui8Pointer + 1) );
	}

	/** Reads the low byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer, store in the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::FetchEffectiveAddressLow_IzY() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  3    pointer    R  fetch effective address low
		
		// val = PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)
		// Fetch effective address low.
		// This is the:
		//	PEEK(arg)
		//	part.
		m_ccCurContext.a.ui8Bytes[0] = m_pbBus->Read( m_ccCurContext.ui8Pointer );
	}

	/** Reads the high byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+1, store in the high byte of LSN_CPU_CONTEXT::a.ui16Address, adds Y. */
	void CCpu6502::FetchEffectiveAddressHigh_IzY() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  4   pointer+1   R  fetch effective address high,
		//                     add Y to low byte of effective address
		
		// val = PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)
		// Fetch effective address high.
		// This is the:
		//	PEEK((arg + 1) % 256) * 256
		//	part.
		m_ccCurContext.a.ui8Bytes[1] = m_pbBus->Read( static_cast<uint8_t>(m_ccCurContext.ui8Pointer + 1) );
		// And this is the:
		//	+ Y
		//	part.
		// Check here if we are going to need to skip a cycle (if a page boundary is NOT crossed).
		m_ccCurContext.j.ui16JmpTarget = static_cast<uint16_t>(m_ccCurContext.a.ui16Address + Y);
		m_ccCurContext.a.ui8Bytes[0] = m_ccCurContext.j.ui8Bytes[0];
	}

	/** Reads from the effective address (LSN_CPU_CONTEXT::a.ui16Address), which may be wrong if a page boundary was crossed.  If so, fixes the high byte of LSN_CPU_CONTEXT::a.ui16Address. */
	template <bool _bHandleCrossing>
	void CCpu6502::ReadEffectiveAddressFixHighByte_IzY_AbX_AbY() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		if constexpr ( _bHandleCrossing ) {
			bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
			if ( bCrossed ) {
				LSN_ADVANCE_CONTEXT_COUNTERS;
				// Read from effective address,
				// fix high byte of effective address.
				m_ccCurContext.ui8Operand = m_pbBus->Read( m_ccCurContext.a.ui16Address );

				// Crossed a page boundary.
				m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
			}
			else {
				// Skip a cycle.  m_ccCurContext.ui8Operand already holds the correct value.
				LSN_ADVANCE_CONTEXT_COUNTERS_BY( 2 );

				// Read from effective address,
				// fix high byte of effective address.
				m_ccCurContext.ui8Operand = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			}
		}
		else {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_ccCurContext.ui8Operand = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
	}

	/** Fetches the low byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the low byte of PC and sets the I flag. */
	void CCpu6502::CopyVectorPcl() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		pc.ui8Bytes[0] = m_pbBus->Read( m_ccCurContext.a.ui16Address );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ ), true>( m_ui8Status );
	}

	/** Fetches the high byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the high byte of PC. */
	void CCpu6502::CopyVectorPch() {
		LSN_FINISH_INST;
		pc.ui8Bytes[1] = m_pbBus->Read( m_ccCurContext.a.ui16Address + 1 );
	}

	/** Fetches the low byte of PC from $FFFE. */
	void CCpu6502::FetchPclFromFFFE() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  6   $FFFE   R  fetch PCL
		pc.ui8Bytes[0] = m_pbBus->Read( 0xFFFE );
	}

	/** Fetches the high byte of PC from $FFFF. */
	void CCpu6502::FetchPclFromFFFF() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		pc.ui8Bytes[1] = m_pbBus->Read( 0xFFFF );
	}

	/** Writes the operand value back to the effective address stored in LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::WriteValueBackToEffectiveAddress() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
	}

	/** Writes the operand value back to the effective address stored in LSN_CPU_CONTEXT::a.ui16Address&0xFF. */
	void CCpu6502::WriteValueBackToEffectiveAddress_Zp() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		m_pbBus->Write( m_ccCurContext.a.ui16Address & 0xFF, m_ccCurContext.ui8Operand );
	}

	/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	template <unsigned _uBit, unsigned _uVal>
	void CCpu6502::Branch_Cycle2() {
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
			LSN_FINISH_INST;
			// Fetch next opcode.
			m_ccCurContext.ui8Operand = m_pbBus->Read( pc.PC++ );
		}
		else {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Fetch next opcode.
			m_ccCurContext.ui8Operand = m_pbBus->Read( pc.PC++ );
			// Branch taken.
			m_ccCurContext.j.ui16JmpTarget = static_cast<int16_t>(static_cast<int8_t>(m_ccCurContext.ui8Operand)) + pc.PC;
			// Set PCL.
			pc.ui8Bytes[0] = m_ccCurContext.j.ui8Bytes[0];
		}
	}

	/** 3rd cycle of branch instructions. Branch was taken and crossed a page boundary, but PC is already up-to-date so read/discard/exit. */
	void CCpu6502::Branch_Cycle3() {
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != pc.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Fetch next opcode.
			m_pbBus->Read( pc.PC );	// Read and discard.  Affects emulation of the floating bus.
			// Crossed a page boundary.
			pc.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Fetch next opcode.
			m_pbBus->Read( pc.PC );	// Read and discard.  Affects emulation of the floating bus.
			// Did not cross a page boundary.
		}
	}

	/** 4th cycle of branch instructions. Page boundary was crossed. */
	void CCpu6502::Branch_Cycle4() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
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
		// Subtract the last cycle?  Opcode fetch is just for illustration?
		// I am assuming this means it should behave as expected by putting PC in place and then letting the
		//	system eat the next opcode on the next cycle, but they could be pre-fetching the opcode as an
		//	optimization.
		m_pbBus->Read( pc.PC );
	}

	/** Performs m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand ); and LSN_FINISH_INST;, which finishes Read-Modify-Write instructions. */
	void CCpu6502::FinalWriteCycle() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  6  address  W  write the new value to effective address
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		
	}

	/** Performs A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::ADC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		Adc( A, m_pbBus->Read( m_ccCurContext.a.ui16Address ) );
	}

	/** Performs A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::ADC_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;

			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			Adc( A, ui8Tmp );
		}
	}

	/** Performs A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::ADC_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		Adc( A, m_pbBus->Read( pc.PC++ ) );
	}

	/** Fetches from PC and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::ANC_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
		A &= ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::AND_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
		A &= ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::AND_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			A &= ui8Tmp;
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
		}
	}

	/** Fetches from PC and performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::AND_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		A &= ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Fetches from PC and performs A = (A | CONST) & X & OP.  Sets flags N and Z. */
	void CCpu6502::ANE() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		A = (A | 0xFF) & X & ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Fetches from PC and performs A = A & OP; A = (A >> 1) | (C << 7).  Sets flags C, V, N and Z. */
	void CCpu6502::ARR_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		A &= ui8Tmp;
		uint8_t ui8HiBit = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )) << 7;
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (A & 0x01) != 0 );
		A = (A >> 1) | ui8HiBit;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW )>( m_ui8Status,
			((m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )) & ((A >> 5) & 0x1)) != 0 );
	}

	/** Performs OP <<= 1.  Sets flags C, N, and Z. */
	void CCpu6502::ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
		m_ccCurContext.ui8Operand <<= 1;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, m_ccCurContext.ui8Operand == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
	}

	/** Performs A <<= 1.  Sets flags C, N, and Z. */
	void CCpu6502::ASL_Imp() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// We have a discarded read here.
		m_pbBus->Read( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		

		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (A & 0x80) != 0 );
		A <<= 1;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs A &= OP; A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::ASR_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		A &= ui8Tmp;
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (A & 0x01) != 0 );
		A >>= 1;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), false>( m_ui8Status );
	}

	/** Sets flags N, V and Z according to a bit test. */
	void CCpu6502::BIT_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		const uint8_t ui8Op = m_pbBus->Read( m_ccCurContext.a.ui16Address );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, (ui8Op & A) == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (ui8Op & (1 << 7)) != 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW )>( m_ui8Status, (ui8Op & (1 << 6)) != 0x00 );
	}

	/** Pops the high byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the high byte of PC. */
	void CCpu6502::BRK() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  7   $FFFF   R  fetch PCH
		pc.ui8Bytes[1] = m_pbBus->Read( 0xFFFF );
	}

	/** Clears the carry flag. */
	void CCpu6502::CLC() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), false>( m_ui8Status );
	}

	/** Clears the decimal flag. */
	void CCpu6502::CLD() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_DECIMAL ), false>( m_ui8Status );
	}

	/** Clears the IRQ flag. */
	void CCpu6502::CLI() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ ), false>( m_ui8Status );
	}

	/** Clears the overflow flag. */
	void CCpu6502::CLV() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW ), false>( m_ui8Status );
	}

	/** Compares A with OP. */
	void CCpu6502::CMP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		Cmp( A, m_pbBus->Read( m_ccCurContext.a.ui16Address ) );
	}

	/** Compares A with OP. */
	void CCpu6502::CMP_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			Cmp( A, ui8Tmp );
		}
	}

	/** Compares A with OP. */
	void CCpu6502::CMP_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		Cmp( A, m_pbBus->Read( pc.PC++ ) );
	}

	/** Compares X with OP. */
	void CCpu6502::CPX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		Cmp( X, m_pbBus->Read( m_ccCurContext.a.ui16Address ) );
	}

	/** Compares X with OP. */
	void CCpu6502::CPX_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		Cmp( X, m_pbBus->Read( pc.PC++ ) );
	}

	/** Compares Y with OP. */
	void CCpu6502::CPY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		Cmp( Y, m_pbBus->Read( m_ccCurContext.a.ui16Address ) );
	}

	/** Compares Y with OP. */
	void CCpu6502::CPY_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		Cmp( Y, m_pbBus->Read( pc.PC++ ) );
	}

	/** Performs [ADDR]--; CMP(A).  Sets flags C, N, and Z. */
	void CCpu6502::DCP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		// It carries if the last bit gets shifted off.
		--m_ccCurContext.ui8Operand;
		Cmp( A, m_ccCurContext.ui8Operand );
	}

	/** Performs [ADDR]--.  Sets flags N and Z. */
	void CCpu6502::DEC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		--m_ccCurContext.ui8Operand;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, m_ccCurContext.ui8Operand == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
	}

	/** Performs X--.  Sets flags N and Z. */
	void CCpu6502::DEX() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		--X;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
	}

	/** Performs Y--.  Sets flags N and Z. */
	void CCpu6502::DEY() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		--Y;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, Y == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (Y & 0x80) != 0 );
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::EOR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
		A ^= ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::EOR_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			A ^= ui8Tmp;
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
		}
	}

	/** Fetches from PC and performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::EOR_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		A ^= ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs [ADDR]++.  Sets flags N and Z. */
	void CCpu6502::INC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		++m_ccCurContext.ui8Operand;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, m_ccCurContext.ui8Operand == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
	}

	/** Performs X++.  Sets flags N and Z. */
	void CCpu6502::INX() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		++X;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
	}

	/** Performs Y++.  Sets flags N and Z. */
	void CCpu6502::INY() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		++Y;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, Y == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (Y & 0x80) != 0 );
	}

	/** Performs M++; SBC.  Sets flags C, N, V, and Z. */
	void CCpu6502::ISB_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		// It carries if the last bit gets shifted off.
		++m_ccCurContext.ui8Operand;
		Sbc( A, m_ccCurContext.ui8Operand );
		//Cmp( A, m_ccCurContext.ui8Operand & 0xFF );
	}

	/** Jams the machine, putting 0xFF on the bus repeatedly. */
	void CCpu6502::JAM() {
		// Jam by having neither LSN_ADVANCE_CONTEXT_COUNTERS nor LSN_FINISH_INST.
		m_pbBus->SetFloat( 0xFF );
	}

	/** Follows FetchLowAddrByteAndIncPc() and copies the read value into the low byte of PC after fetching the high byte. */
	void CCpu6502::JMP_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  3    PC     R  copy low address byte to PCL, fetch high address
		//                 byte to PCH
		m_ccCurContext.j.ui8Bytes[1] = m_pbBus->Read( pc.PC );
		pc.PC = m_ccCurContext.j.ui16JmpTarget;
	}

	/** Copies the read value into the low byte of PC after fetching the high byte. */
	void CCpu6502::JMP_Ind() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  5  pointer+1* R  fetch PCH, copy latch to PCL

		// Note: * The PCH will always be fetched from the same page
		//	as PCL, i.e. page boundary crossing is not handled.
		m_ccCurContext.j.ui8Bytes[1] = m_pbBus->Read( (m_ccCurContext.a.ui16Address & 0xFF00) | uint8_t( m_ccCurContext.a.ui8Bytes[0] + 1 ) );
		pc.PC = m_ccCurContext.j.ui16JmpTarget;
	}

	/** JSR (Jump to Sub-Routine). */
	void CCpu6502::JSR() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  6    PC     R  copy low address byte to PCL, fetch high address
		//                 byte to PCH
		m_ccCurContext.j.ui8Bytes[1] = m_pbBus->Read( pc.PC );
		pc.PC = m_ccCurContext.j.ui16JmpTarget;
	}

	/** Performs A = X = S = (OP & S).  Sets flags N and Z. */
	void CCpu6502::LAS_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		A = X = S = (m_pbBus->Read( m_ccCurContext.a.ui16Address ) & S);

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs A = X = S = (OP & S).  Sets flags N and Z. */
	void CCpu6502::LAS_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			A = X = S = (ui8Tmp & S);
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
		}
	}

	/** Performs A = X = OP.  Sets flags N and Z. */
	void CCpu6502::LAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		A = X = m_pbBus->Read( m_ccCurContext.a.ui16Address );

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs A = X = OP.  Sets flags N and Z. */
	void CCpu6502::LAX_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			A = X = ui8Tmp;
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
		}
	}

	/** Performs A = OP.  Sets flags N and Z. */
	void CCpu6502::LDA_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC
		A = m_pbBus->Read( pc.PC++ );

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs A = OP.  Sets flags N and Z. */
	void CCpu6502::LDA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		A = m_pbBus->Read( m_ccCurContext.a.ui16Address );

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs A = OP.  Sets flags N and Z. */
	void CCpu6502::LDA_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			A = ui8Tmp;
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
		}
	}

	/** Performs X = OP.  Sets flags N and Z. */
	void CCpu6502::LDX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		X = m_pbBus->Read( m_ccCurContext.a.ui16Address );

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
	}

	/** Performs X = OP.  Sets flags N and Z. */
	void CCpu6502::LDX_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			X = ui8Tmp;
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
		}
	}

	/** Performs X = OP.  Sets flags N and Z. */
	void CCpu6502::LDX_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC
		X = m_pbBus->Read( pc.PC++ );

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
	}

	/** Performs Y = OP.  Sets flags N and Z. */
	void CCpu6502::LDY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		Y = m_pbBus->Read( m_ccCurContext.a.ui16Address );

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, Y == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (Y & 0x80) != 0 );
	}

	/** Performs Y = OP.  Sets flags N and Z. */
	void CCpu6502::LDY_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			Y = ui8Tmp;
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, Y == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (Y & 0x80) != 0 );
		}
	}

	/** Performs Y = OP.  Sets flags N and Z. */
	void CCpu6502::LDY_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC
		Y = m_pbBus->Read( pc.PC++ );

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, Y == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (Y & 0x80) != 0 );
	}

	/** Performs OP >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::LSR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x01) != 0 );
		m_ccCurContext.ui8Operand >>= 1;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, m_ccCurContext.ui8Operand == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), false>( m_ui8Status );
	}

	/** Performs A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::LSR_Imp() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// We have a discarded read here.
		m_pbBus->Read( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		

		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (A & 0x01) != 0 );
		A >>= 1;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE ), false>( m_ui8Status );
	}

	/** Fetches from PC and performs A = X = (A | CONST) & OP.  Sets flags N and Z. */
	void CCpu6502::LXA() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		// Weird like ANE().
		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		A = X = (A | 0xFF) & ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Reads the next instruction byte and throws it away. */
	void CCpu6502::NOP_Imp() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
	}

	/** Reads the next instruction byte and throws it away, increments PC. */
	void CCpu6502::NOP_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC++ );
	}

	/** Reads LSN_CPU_CONTEXT::a.ui16Address and throws it away. */
	void CCpu6502::NOP_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( m_ccCurContext.a.ui16Address );
	}

	/** No operation. */
	void CCpu6502::NOP_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
		}
	}

	/** Fetches from PC and performs A = A | OP.  Sets flags N and Z. */
	void CCpu6502::ORA_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		A |= ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
	void CCpu6502::ORA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		const uint8_t ui8Op = m_pbBus->Read( m_ccCurContext.a.ui16Address );
		A |= ui8Op;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
	void CCpu6502::ORA_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Op = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			A |= ui8Op;
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
			SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
		}
	}

	/** Pushes the accumulator. */
	void CCpu6502::PHA() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		LSN_PUSH( A );
	}

	/** Pushes the status byte. */
	void CCpu6502::PHP() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		/* http://users.telenet.be/kim1-6502/6502/proman.html#811
		8.11 PHP - PUSH PROCESSOR STATUS ON STACK

          This instruction transfers the contents of the processor status reg-
     ister unchanged to the stack, as governed by the stack pointer.
          Symbolic notation for this is P v.
          The PHP instruction affects no registers or flags in the micropro-
     cessor.
          PHP is a single-byte instruction and the addressing mode is Implied.
		*/
		uint8_t ui8Copy = m_ui8Status;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) | uint8_t( LSN_STATUS_FLAGS::LSN_SF_RESERVED ), true>( ui8Copy );
		// Can be set by PLP().
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_RESERVED ), false>( m_ui8Status );
		LSN_PUSH( ui8Copy );
	}

	/** Pulls the accumulator. */
	void CCpu6502::PLA() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		A = LSN_POP();

		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Pulls the status byte. */
	void CCpu6502::PLP() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		const uint8_t ui8Mask = (uint8_t( LSN_STATUS_FLAGS::LSN_SF_BREAK ) | uint8_t( LSN_STATUS_FLAGS::LSN_SF_RESERVED ));
		m_ui8Status = (LSN_POP() & ~ui8Mask);// | (m_ui8Status & ui8Mask);
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_RESERVED ), true>( m_ui8Status );
	}

	/** Performs OP = (OP << 1) | (C); A = A & (OP).  Sets flags C, N and Z. */
	void CCpu6502::RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		uint8_t ui8LowBit = m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY );
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
		m_ccCurContext.ui8Operand = (m_ccCurContext.ui8Operand << 1) | ui8LowBit;
		A &= m_ccCurContext.ui8Operand;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs A = (A << 1) | (C).  Sets flags C, N, and Z. */
	void CCpu6502::ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		uint8_t ui8LowBit = m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY );
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
		m_ccCurContext.ui8Operand = (m_ccCurContext.ui8Operand << 1) | ui8LowBit;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, m_ccCurContext.ui8Operand == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
	}

	/** Performs A = (A << 1) | (C).  Sets flags C, N, and Z. */
	void CCpu6502::ROL_Imp() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// We have a discarded read here.
		m_pbBus->Read( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		
		uint8_t ui8LowBit = m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY );
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (A & 0x80) != 0 );
		A = (A << 1) | ui8LowBit;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
	void CCpu6502::ROR_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #   address  R/W description
		// --- --------- --- ------------------------------------------
		//  6  address+X  W  write the value back to effective address,
		//                   and do the operation on it
		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		uint8_t ui8HiBit = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )) << 7;
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x01) != 0 );
		m_ccCurContext.ui8Operand = (m_ccCurContext.ui8Operand >> 1) | ui8HiBit;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, m_ccCurContext.ui8Operand == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
	}

	/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
	void CCpu6502::ROR_Imp() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// We have a discarded read here.
		m_pbBus->Read( pc.PC );	// Affects what floats on the bus for the more-accurate emulation of a floating bus.
		
		uint8_t ui8HiBit = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )) << 7;
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (A & 0x01) != 0 );
		A = (A >> 1) | ui8HiBit;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs OP = (OP >> 1) | (C << 7); A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::RRA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;

		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		uint8_t ui8HiBit = (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )) << 7;
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x01) != 0 );		// This affects whether the carry bit gets added below.
		m_ccCurContext.ui8Operand = (m_ccCurContext.ui8Operand >> 1) | ui8HiBit;

		//const uint8_t ui8Tmp = m_ccCurContext.ui8Operand;
		Adc( A, m_ccCurContext.ui8Operand );
		/*uint16_t ui16Result = uint16_t( A ) + uint16_t( ui8Tmp ) + (m_ui8Status & uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ));
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_OVERFLOW )>( m_ui8Status, (~(A ^ ui8Tmp) & (A ^ ui16Result) & 0x80) == 0 );
		A = uint8_t( ui16Result );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, ui16Result > 0xFF );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, ui16Result == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );*/
	}

	/** Pops into PCH. */
	void CCpu6502::RTI() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- -----------------------------------------------
		//  6  $0100,S  R  pull PCH from stack
		pc.ui8Bytes[1] = LSN_POP();
	}

	/** Reads PC and increments it. */
	void CCpu6502::RTS() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC++ );
	}

	/** Writes (A & X) to LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::SAX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		m_pbBus->Write( m_ccCurContext.a.ui16Address, A & X );
	}

	/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::SBC_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		Sbc( A, m_pbBus->Read( m_ccCurContext.a.ui16Address ) );
	}

	/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::SBC_IzY_AbX_AbY_1() {
		//  #    address   R/W description
		// --- ----------- --- ------------------------------------------
		//  5   address+Y*  R  read from effective address,
		//                     fix high byte of effective address
		
		// We may have read from the wrong address if the high byte of the effective address isn't correct.
		// If it is correct, we can skip to the work routine, otherwise continue to the next cycle.
		bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != m_ccCurContext.a.ui8Bytes[1];
		if ( bCrossed ) {
			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Read from effective address,
			// fix high byte of effective address.
			m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// Crossed a page boundary.
			m_ccCurContext.a.ui8Bytes[1] = m_ccCurContext.j.ui8Bytes[1];
		}
		else {
			// Last cycle in the instruction.
			LSN_FINISH_INST;
			// Read from effective address,
			// fix high byte of effective address.
			const uint8_t ui8Tmp = m_pbBus->Read( m_ccCurContext.a.ui16Address );
			// We are done.
			Sbc( A, ui8Tmp );
		}
	}

	/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::SBC_Imm() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		// Uses the 8-bit operand itself as the value for the operation, rather than fetching a value from a memory address.
		Sbc( A, m_pbBus->Read( pc.PC++ ) );
	}

	/** Fetches from PC and performs X = (A & X) - OP.  Sets flags C, N and Z. */
	void CCpu6502::SBX() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		//  #  address R/W description
		// --- ------- --- ------------------------------------------
		//  2    PC     R  fetch value, increment PC

		const uint8_t ui8Tmp = m_pbBus->Read( pc.PC++ );
		const uint8_t ui8AnX = (A & X);
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, ui8AnX >= ui8Tmp );
		X = ui8AnX - ui8Tmp;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
	}

	/** Sets the carry flag. */
	void CCpu6502::SEC() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY ), true>( m_ui8Status );
	}

	/** Sets the decimal flag. */
	void CCpu6502::SED() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_DECIMAL ), true>( m_ui8Status );
	}

	/** Sets the IRQ flag. */
	void CCpu6502::SEI() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		// #  address R/W description
		// --- ------- --- -----------------------------------------------
		// 2    PC     R  read next instruction byte (and throw it away)
		m_pbBus->Read( pc.PC );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_IRQ ), true>( m_ui8Status );
	}

	/** Illegal. Stores A & X & (high-byte of address + 1) at the address. */
	void CCpu6502::SHA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		// #    address   R/W description
		// --- ----------- --- ------------------------------------------
		// 6   address+Y   W  write to effective address

		/* Stores A AND X AND (high-byte of addr. + 1) at addr.
		
		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		A AND X AND (H+1) -> M
		*/
		m_pbBus->Write( m_ccCurContext.a.ui16Address, uint8_t( m_ccCurContext.a.ui8Bytes[1] + 1 ) & A & X );
	}

	/** Illegal. Puts A & X into SP; stores A & X & (high-byte of address + 1) at the address. */
	void CCpu6502::SHS() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		// #    address   R/W description
		// --- ----------- --- ------------------------------------------
		// 6   address+Y   W  write to effective address

		/* Puts A AND X in SP and stores A AND X AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		A AND X -> SP, A AND X AND (H+1) -> M
		*/
		S = X & A;
		m_pbBus->Write( m_ccCurContext.a.ui16Address, uint8_t( m_ccCurContext.a.ui8Bytes[1] + 1 ) & S );
	}

	/** Illegal. Stores X & (high-byte of address + 1) at the address. */
	void CCpu6502::SHX() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		// #    address   R/W description
		// --- ----------- --- ------------------------------------------
		// 6   address+Y   W  write to effective address

		/* Stores Y AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		Y AND (H+1) -> M
		*/
		m_pbBus->Write( m_ccCurContext.a.ui16Address, uint8_t( m_ccCurContext.a.ui8Bytes[1] + 1 ) & X );
	}

	/** Illegal. Stores Y & (high-byte of address + 1) at the address. */
	void CCpu6502::SHY() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		// #    address   R/W description
		// --- ----------- --- ------------------------------------------
		// 6   address+Y   W  write to effective address

		/* Stores Y AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		Y AND (H+1) -> M
		*/
		m_pbBus->Write( m_ccCurContext.a.ui16Address, uint8_t( m_ccCurContext.a.ui8Bytes[1] + 1 ) & Y );
	}

	/** Performs OP = (OP << 1); A = A | (OP).  Sets flags C, N and Z. */
	void CCpu6502::SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x80) != 0 );
		m_ccCurContext.ui8Operand <<= 1;
		A |= m_ccCurContext.ui8Operand;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Performs OP = (OP >> 1); A = A ^ (OP).  Sets flags C, N and Z. */
	void CCpu6502::SRE_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		LSN_ADVANCE_CONTEXT_COUNTERS;
		//  #  address R/W description
		// --- ------- --- -------------------------------------------------
		//  5  address  W  write the value back to effective address,
		//                 and do the operation on it

		m_pbBus->Write( m_ccCurContext.a.ui16Address, m_ccCurContext.ui8Operand );
		// It carries if the last bit gets shifted off.
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_CARRY )>( m_ui8Status, (m_ccCurContext.ui8Operand & 0x01) != 0 );
		m_ccCurContext.ui8Operand = (m_ccCurContext.ui8Operand >> 1);
		A ^= m_ccCurContext.ui8Operand;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Writes A to LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::STA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		m_pbBus->Write( m_ccCurContext.a.ui16Address, A );
	}

	/** Writes X to LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::STX_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		m_pbBus->Write( m_ccCurContext.a.ui16Address, X );
	}

	/** Writes Y to LSN_CPU_CONTEXT::a.ui16Address. */
	void CCpu6502::STY_IzX_IzY_ZpX_AbX_AbY_Zp_Abs() {
		// Last cycle in the instruction.  Do this before the write because the write operation might change the next Tick() function pointer.
		LSN_FINISH_INST;

		m_pbBus->Write( m_ccCurContext.a.ui16Address, Y );
	}

	/** Copies A into X.  Sets flags N, and Z. */
	void CCpu6502::TAX() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC );
		X = A;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
	}

	/** Copies A into Y.  Sets flags N, and Z. */
	void CCpu6502::TAY() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC );
		Y = A;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, Y == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (Y & 0x80) != 0 );
	}

	/** Copies S into X. */
	void CCpu6502::TSX() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC );
		X = S;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, X == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (X & 0x80) != 0 );
	}

	/** Copies X into A.  Sets flags N, and Z. */
	void CCpu6502::TXA() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC );
		A = X;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

	/** Copies X into S. */
	void CCpu6502::TXS() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC );
		S = X;
	}

	/** Copies Y into A.  Sets flags N, and Z. */
	void CCpu6502::TYA() {
		// Last cycle in the instruction.
		LSN_FINISH_INST;
		m_pbBus->Read( pc.PC );
		A = Y;
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_ZERO )>( m_ui8Status, A == 0x00 );
		SetBit<uint8_t( LSN_STATUS_FLAGS::LSN_SF_NEGATIVE )>( m_ui8Status, (A & 0x80) != 0 );
	}

}	// namespace lsn

#undef LSN_BRANCH

#undef LSN_ABSOLUTE_X_W
#undef LSN_ABSOLUTE_X_RMW
#undef LSN_ABSOLUTE_X_R

#undef LSN_ABSOLUTE_Y_W
#undef LSN_ABSOLUTE_Y_RMW
#undef LSN_ABSOLUTE_Y_R

#undef LSN_ZERO_PAGE_Y_W
#undef LSN_ZERO_PAGE_Y_R

#undef LSN_ZERO_PAGE_X_W
#undef LSN_ZERO_PAGE_X_RMW
#undef LSN_ZERO_PAGE_X_R

#undef LSN_IMMEDIATE

#undef LSN_ABSOLUTE_W
#undef LSN_ABSOLUTE_RMW
#undef LSN_ABSOLUTE_R

#undef LSN_ZERO_PAGE_W
#undef LSN_ZERO_PAGE_RMW
#undef LSN_ZERO_PAGE_R

#undef LSN_INDIRECT_Y_W
#undef LSN_INDIRECT_Y_RMW
#undef LSN_INDIRECT_Y_R

#undef LSN_INDIRECT_X_W
#undef LSN_INDIRECT_X_RMW
#undef LSN_INDIRECT_X_R

#undef LSN_FINISH_INST
#undef LSN_POP
#undef LSN_PUSH
#undef LSN_ADVANCE_CONTEXT_COUNTERS
#undef LSN_ADVANCE_CONTEXT_COUNTERS_BY