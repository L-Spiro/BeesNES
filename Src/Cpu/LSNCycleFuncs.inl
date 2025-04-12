#define LSN_INDIRECT_X_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr_8bit<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME,
#define LSN_INDIRECT_X_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr_8bit<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 8, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME,
#define LSN_INDIRECT_X_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr_8bit<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_W>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME,

#define LSN_INDIRECT_Y_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::IndirectYAdd_PtrOrAddr_To_AddrOrPtr<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_BoundarySkip_Phi2<LSN_FROM_P, true>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P, true>, &CCpu6502::FUNC, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME,
#define LSN_INDIRECT_Y_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::IndirectYAdd_PtrOrAddr_To_AddrOrPtr<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_P>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_P, true>, &CCpu6502::BeginInst, }, 8, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME,
#define LSN_INDIRECT_Y_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::IndirectYAdd_PtrOrAddr_To_AddrOrPtr<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_A>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 6, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME,

#define LSN_ZERO_PAGE_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME,
#define LSN_ZERO_PAGE_RMW( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 5, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME,
#define LSN_ZERO_PAGE_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME,

#define LSN_ZERO_PAGE_X_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit<LSN_TO_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME,
#define LSN_ZERO_PAGE_X_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit<LSN_TO_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 6, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME,
#define LSN_ZERO_PAGE_X_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME,

#define LSN_ZERO_PAGE_Y_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC }, 4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME,
#define LSN_ZERO_PAGE_Y_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME,

#define LSN_IMMEDIATE( NAME, FUNC )														{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2<true>, &CCpu6502::FUNC, }, 2, LSN_AM_IMMEDIATE, 2, LSN_I_ ## NAME,

#define LSN_ABSOLUTE_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME,
#define LSN_ABSOLUTE_RMW( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 6, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME,
#define LSN_ABSOLUTE_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_W, true>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME,

#define LSN_ABSOLUTE_X_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_BoundarySkip_Phi2<LSN_FROM_A, true>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME,
#define LSN_ABSOLUTE_X_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME,
#define LSN_ABSOLUTE_X_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 5, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME,

#define LSN_ABSOLUTE_Y_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_BoundarySkip_Phi2<LSN_FROM_A, true>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME,
#define LSN_ABSOLUTE_Y_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME,
#define LSN_ABSOLUTE_Y_W( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>, &CCpu6502::FUNC, &CCpu6502::BeginInst, }, 5, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME,

#define LSN_BRANCH( NAME, COND, VALUE )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Branch_Cycle1<COND, VALUE>, &CCpu6502::Branch_Cycle1_Phi2, &CCpu6502::Branch_Cycle2, &CCpu6502::Branch_Cycle2_Phi2, &CCpu6502::Branch_Cycle3, &CCpu6502::Branch_Cycle3_Phi2, &CCpu6502::Branch_Cycle4, }, 2, LSN_AM_RELATIVE, 2, LSN_I_ ## NAME,
 
CCpu6502::LSN_INSTR CCpu6502::m_iInstructionSet[256] = {								/**< The instruction set. */
	/** 00-07 */
	{	// 00
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Null_RorW<true>,													&CCpu6502::Push_Pc_H_Phi2<0>,
			&CCpu6502::Null_RorW,														&CCpu6502::Push_Pc_L_Phi2<-1>,
			&CCpu6502::SelectBrkVectors<false>,											&CCpu6502::Push_S_Phi2<-2>,
			&CCpu6502::Null<LSN_R, false, true>,										&CCpu6502::CopyVectorToPc_L_Phi2,
			/*&CCpu6502::Null<LSN_W>,														&CCpu6502::Push_S_Phi2<-2>,
			&CCpu6502::SelectBrkVectors<true>,											&CCpu6502::CopyVectorToPc_L_Phi2,*/
			&CCpu6502::SetBrkFlags,														&CCpu6502::CopyVectorToPc_H_Phi2<false>,
			&CCpu6502::Brk_BeginInst },
		7, LSN_AM_IMPLIED, 2, LSN_I_BRK,
	},
	{	// 01
		LSN_INDIRECT_X_R( ORA, Ora_BeginInst )
	},
	{	// 02
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 03
		LSN_INDIRECT_X_RMW( SLO, Slo )
	},
	{	// 04
		LSN_ZERO_PAGE_R( NOP, BeginInst )
	},
	{	// 05
		LSN_ZERO_PAGE_R( ORA, Ora_BeginInst )
	},
	{	// 06
		LSN_ZERO_PAGE_RMW( ASL, Asl )
	},
	{	// 07
		LSN_ZERO_PAGE_RMW( SLO, Slo )
	},


	/** 08-0F */
	{	// 08
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Php,																&CCpu6502::Push_Operand_Phi2<0, true>,
			&CCpu6502::Null<LSN_R, false, true, true> },
			3, LSN_AM_IMPLIED, 1, LSN_I_PHP,
	},
	{	// 09
		LSN_IMMEDIATE( ORA, Ora_BeginInst<true> )
	},
	{	// 0A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::AslOnA_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_ASL,
	},
	{	// 0B
		LSN_IMMEDIATE( ANC, Anc_IncPc_BeginInst )
	},
	{	// 0C
		LSN_ABSOLUTE_R( NOP, BeginInst )
	},
	{	// 0D
		LSN_ABSOLUTE_R( ORA, Ora_BeginInst )
	},
	{	// 0E
		LSN_ABSOLUTE_RMW( ASL, Asl )
	},
	{	// 0F
		LSN_ABSOLUTE_RMW( SLO, Slo )
	},


	/** 10-17 */
	{	// 10
		LSN_BRANCH( BPL, N(), 0 )														// Branch if N == 0.
	},
	{	// 11
		LSN_INDIRECT_Y_R( ORA, Ora_BeginInst )
	},
	{	// 12
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 13
		LSN_INDIRECT_Y_RMW( SLO, Slo )
	},
	{	// 14
		LSN_ZERO_PAGE_X_R( NOP, BeginInst )
	},
	{	// 15
		LSN_ZERO_PAGE_X_R( ORA, Ora_BeginInst )
	},
	{	// 16
		LSN_ZERO_PAGE_X_RMW( ASL, Asl )
	},
	{	// 17
		LSN_ZERO_PAGE_X_RMW( SLO, Slo )
	},


	/** 18-1F */
	{	// 18
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Clc_BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_CLC,
	},
	{	// 19
		LSN_ABSOLUTE_Y_R( ORA, Ora_BeginInst )
	},
	{	// 1A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 1B
		LSN_ABSOLUTE_Y_RMW( SLO, Slo )
	},
	{	// 1C
		LSN_ABSOLUTE_X_R( NOP, BeginInst )
	},
	{	// 1D
		LSN_ABSOLUTE_X_R( ORA, Ora_BeginInst )
	},
	{	// 1E
		LSN_ABSOLUTE_X_RMW( ASL, Asl )
	},
	{	// 1F
		LSN_ABSOLUTE_X_RMW( SLO, Slo )
	},

	/** 20-27 */
	{	// 20
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_Stack_To_Operand_Phi2,
			&CCpu6502::Null<LSN_W>,														&CCpu6502::Push_Pc_H_Phi2<0>,
			&CCpu6502::Null<LSN_W>,														&CCpu6502::Push_Pc_L_Phi2<-1>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A, true>,
			&CCpu6502::Jsr_BeginInst, },
		6, LSN_AM_ABSOLUTE, 3, LSN_I_JSR,
	},
	{	// 21
		LSN_INDIRECT_X_R( AND, And_BeginInst )
	},
	{	// 22
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 23
		LSN_INDIRECT_X_RMW( RLA, Rla )
	},
	{	// 24
		LSN_ZERO_PAGE_R( BIT, Bit_BeginInst )
	},
	{	// 25
		LSN_ZERO_PAGE_R( AND, And_BeginInst )
	},
	{	// 26
		LSN_ZERO_PAGE_RMW( ROL, Rol )
	},
	{	// 27
		LSN_ZERO_PAGE_RMW( RLA, Rla )
	},

	/** 28-2F */
	{	// 28
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::Pull_To_Operand_Phi2,
			&CCpu6502::Null<LSN_R, false, true>,										&CCpu6502::Read_Stack_To_Operand_Phi2<true>,
			&CCpu6502::Plp_BeginInst, },
			4, LSN_AM_IMPLIED, 1, LSN_I_PLP,
	},
	{	// 29
		LSN_IMMEDIATE( AND, And_BeginInst<true> )
	},
	{	// 2A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::RolOnA_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_ROL,
	},
	{	// 2B
		LSN_IMMEDIATE( ANC, Anc_IncPc_BeginInst )
	},
	{	// 2C
		LSN_ABSOLUTE_R( BIT, Bit_BeginInst )
	},
	{	// 2D
		LSN_ABSOLUTE_R( AND, And_BeginInst )
	},
	{	// 2E
		LSN_ABSOLUTE_RMW( ROL, Rol )
	},
	{	// 2F
		LSN_ABSOLUTE_RMW( RLA, Rla )
	},

	/** 30-37 */
	{	// 30
		LSN_BRANCH( BMI, N(), 1 )														// Branch if N == 1.
	},
	{	// 31
		LSN_INDIRECT_Y_R( AND, And_BeginInst )
	},
	{	// 32
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 33
		LSN_INDIRECT_Y_RMW( RLA, Rla )
	},
	{	// 34
		LSN_ZERO_PAGE_X_R( NOP, BeginInst )
	},
	{	// 35
		LSN_ZERO_PAGE_X_R( AND, And_BeginInst )
	},
	{	// 36
		LSN_ZERO_PAGE_X_RMW( ROL, Rol )
	},
	{	// 37
		LSN_ZERO_PAGE_X_RMW( RLA, Rla )
	},


	/** 38-3F */
	{	// 38
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Sec_BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_SEC,
	},
	{	// 39
		LSN_ABSOLUTE_Y_R( AND, And_BeginInst )
	},
	{	// 3A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 3B
		LSN_ABSOLUTE_Y_RMW( RLA, Rla )
	},
	{	// 3C
		LSN_ABSOLUTE_X_R( NOP, BeginInst )
	},
	{	// 3D
		LSN_ABSOLUTE_X_R( AND, And_BeginInst )
	},
	{	// 3E
		LSN_ABSOLUTE_X_RMW( ROL, Rol )
	},
	{	// 3F
		LSN_ABSOLUTE_X_RMW( RLA, Rla )
	},

	/** 40-47 */
	{	// 40
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Pull_To_Operand_Phi2<0>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::Pull_To_Operand_Phi2<1>,
			&CCpu6502::CopyOperandToStatusWithoutB,										&CCpu6502::Pull_To_Target_L_Phi2<2>,
			&CCpu6502::Null<LSN_R, false, true>,										&CCpu6502::Read_Stack_To_Target_H_Phi2<0, true>,
			&CCpu6502::Rti_BeginInst, },
			6, LSN_AM_IMPLIED, 1, LSN_I_RTI,
	},
	{	// 41
		LSN_INDIRECT_X_R( EOR, Eor_BeginInst )
	},
	{	// 42
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM
	},
	{	// 43
		LSN_INDIRECT_X_RMW( SRE, Sre )
	},
	{	// 44
		LSN_ZERO_PAGE_R( NOP, BeginInst )
	},
	{	// 45
		LSN_ZERO_PAGE_R( EOR, Eor_BeginInst )
	},
	{	// 46
		LSN_ZERO_PAGE_RMW( LSR, Lsr )
	},
	{	// 47
		LSN_ZERO_PAGE_RMW( SRE, Sre )
	},

	/** 48-4F */
	{	// 48
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_W>,														&CCpu6502::Push_A_Phi2<0, true>,
			&CCpu6502::BeginInst<false, true>, },
		3, LSN_AM_IMPLIED, 1, LSN_I_PHA,
	},
	{	// 49
		LSN_IMMEDIATE( EOR, Eor_BeginInst<true> )
	},
	{	// 4A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::LsrOnA_BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_LSR,
	},
	{	// 4B
		LSN_IMMEDIATE( ASR, Asr_IncPc_BeginInst )
	},
	{	// 4C
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A, true>,
			&CCpu6502::Jmp_BeginInst, },
		3, LSN_AM_ABSOLUTE, 3, LSN_I_JMP,
	},
	{	// 4D
		LSN_ABSOLUTE_R( EOR, Eor_BeginInst )
	},
	{	// 4E
		LSN_ABSOLUTE_RMW( LSR, Lsr )
	},
	{	// 4F
		LSN_ABSOLUTE_RMW( SRE, Sre )
	},

	/** 50-57 */
	{	// 50
		LSN_BRANCH( BVC, V(), 0 )														// Branch if V == 0.
	},
	{	// 51
		LSN_INDIRECT_Y_R( EOR, Eor_BeginInst )
	},
	{	// 52
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 53
		LSN_INDIRECT_Y_RMW( SRE, Sre )
	},
	{	// 54
		LSN_ZERO_PAGE_X_R( NOP, BeginInst )
	},
	{	// 55
		LSN_ZERO_PAGE_X_R( EOR, Eor_BeginInst )
	},
	{	// 56
		LSN_ZERO_PAGE_X_RMW( LSR, Lsr )
	},
	{	// 57
		LSN_ZERO_PAGE_X_RMW( SRE, Sre )
	},

	/** 58-5F */
	{	// 58
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Cli_BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_CLI
	},
	{	// 59
		LSN_ABSOLUTE_Y_R( EOR, Eor_BeginInst )
	},
	{	// 5A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 5B
		LSN_ABSOLUTE_Y_RMW( SRE, Sre )
	},
	{	// 5C
		LSN_ABSOLUTE_X_R( NOP, BeginInst )
	},
	{	// 5D
		LSN_ABSOLUTE_X_R( EOR, Eor_BeginInst )
	},
	{	// 5E
		LSN_ABSOLUTE_X_RMW( LSR, Lsr )
	},
	{	// 5F
		LSN_ABSOLUTE_X_RMW( SRE, Sre )
	},

	/** 60-67 */
	{	// 60
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Pull_To_Operand_Phi2<0>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::Pull_To_Target_L_Phi2<1>,
			&CCpu6502::Null<LSN_R, false, true>,										&CCpu6502::Read_Stack_To_Target_H_Phi2,
			&CCpu6502::CopyTargetToPc<false>,											&CCpu6502::Fetch_Operand_IncPc_Phi2<true>,
			&CCpu6502::Rts_BeginInst, },
			6, LSN_AM_IMPLIED, 1, LSN_I_RTS,
	},
	{	// 61
		LSN_INDIRECT_X_R( ADC, Adc_BeginInst )
	},
	{	// 62
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 63
		LSN_INDIRECT_X_RMW( RRA, Rra )
	},
	{	// 64
		LSN_ZERO_PAGE_R( NOP, BeginInst )
	},
	{	// 65
		LSN_ZERO_PAGE_R( ADC, Adc_BeginInst )
	},
	{	// 66
		LSN_ZERO_PAGE_RMW( ROR, Ror )
	},
	{	// 67
		LSN_ZERO_PAGE_RMW( RRA, Rra )
	},

	/** 68-6F */
	{	// 68
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::Pull_To_A_Phi2<0>,
			&CCpu6502::Null<LSN_R, false, true>,										&CCpu6502::Read_Stack_To_Operand_Phi2<true>,
			&CCpu6502::Pla_BeginInst, },
			4, LSN_AM_IMPLIED, 1, LSN_I_PLA,
	},
	{	// 69
		LSN_IMMEDIATE( ADC, Adc_BeginInst<true> )
	},
	{	// 6A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::RorOnA_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_ROR,
	},
	{	// 6B
		LSN_IMMEDIATE( ARR, Arr_BeginInst<true> )
	},
	{	// 6C
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_SamePage_Phi2<LSN_FROM_P, true>,
			&CCpu6502::Jmp_BeginInst, },
			5, LSN_AM_INDIRECT, 3, LSN_I_JMP,
	},
	{	// 6D
		LSN_ABSOLUTE_R( ADC, Adc_BeginInst )
	},
	{	// 6E
		LSN_ABSOLUTE_RMW( ROR, Ror )
	},
	{	// 6F
		LSN_ABSOLUTE_RMW( RRA, Rra )
	},

	/** 70-77 */
	{	// 70
		LSN_BRANCH( BVS, V(), 1 )														// Branch if V == 1.
	},
	{	// 71
		LSN_INDIRECT_Y_R( ADC, Adc_BeginInst )
	},
	{	// 72
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 73
		LSN_INDIRECT_Y_RMW( RRA, Rra )
	},
	{	// 74
		LSN_ZERO_PAGE_X_R( NOP, BeginInst )
	},
	{	// 75
		LSN_ZERO_PAGE_X_R( ADC, Adc_BeginInst )
	},
	{	// 76
		LSN_ZERO_PAGE_X_RMW( ROR, Ror )
	},
	{	// 77
		LSN_ZERO_PAGE_X_RMW( RRA, Rra )
	},

	/** 78-7F */
	{	// 78
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Sei_BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_SEI
	},
	{	// 79
		LSN_ABSOLUTE_Y_R( ADC, Adc_BeginInst )
	},
	{	// 7A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 7B
		LSN_ABSOLUTE_Y_RMW( RRA, Rra )
	},
	{	// 7C
		LSN_ABSOLUTE_X_R( NOP, BeginInst )
	},
	{	// 7D
		LSN_ABSOLUTE_X_R( ADC, Adc_BeginInst )
	},
	{	// 7E
		LSN_ABSOLUTE_X_RMW( ROR, Ror )
	},
	{	// 7F
		LSN_ABSOLUTE_X_RMW( RRA, Rra )
	},

	/** 80-87 */
	{	// 80
		LSN_IMMEDIATE( NOP, BeginInst<true> )
	},
	{	// 81
		LSN_INDIRECT_X_W( STA, Write_A_To_AddrOrPtr_Phi2<LSN_TO_A> )

		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_Operand_Discard_Phi2,
		//	&CCpu6502::Add_XAndOperand_To_AddrOrPntr_8bit<LSN_TO_P, LSN_R>,				&CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>,
		//	&CCpu6502::Null<LSN_R>,														&CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>,
		//	&CCpu6502::Null<LSN_W>,														&CCpu6502::Write_A_To_AddrOrPtr_Phi2<LSN_TO_A, true>,
		//	&CCpu6502::BeginInst,
		//}, 6, LSN_AM_INDIRECT_X, 2, LSN_I_STA,
	},
	{	// 82
		LSN_IMMEDIATE( NOP, BeginInst<true> )
	},
	{	// 83
		LSN_INDIRECT_X_W( SAX, Sax_Phi2<LSN_TO_A> )
	},
	{	// 84
		LSN_ZERO_PAGE_W( STY, Write_Y_To_AddrOrPtr_Phi2<LSN_TO_A> )
		
		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Write_Y_To_AddrOrPtr_Phi2<LSN_TO_A>,
		//	&CCpu6502::BeginInst, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_STY,
	},
	{	// 85
		LSN_ZERO_PAGE_W( STA, Write_A_To_AddrOrPtr_Phi2<LSN_TO_A> )
	},
	{	// 86
		LSN_ZERO_PAGE_W( STX, Write_X_To_AddrOrPtr_Phi2<LSN_TO_A> )
	},
	{	// 87
		LSN_ZERO_PAGE_W( SAX, Sax_Phi2<LSN_TO_A> )
	},

	/** 88-8F */
	{	// 88
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Dey_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_DEY,
	},
	{	// 89
		LSN_IMMEDIATE( NOP, BeginInst<true> )
	},
	{	// 8A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Txa_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_TXA,
	},
	{	// 8B
		LSN_IMMEDIATE( ANE, Ane_IncPc_BeginInst )
	},
	{	// 8C
		LSN_ABSOLUTE_W( STY, Write_Y_To_AddrOrPtr_Phi2<LSN_TO_A> )

		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>,
		//	&CCpu6502::Null<LSN_W, true>,												&CCpu6502::Write_Y_To_AddrOrPtr_Phi2<LSN_TO_A>,
		//	&CCpu6502::BeginInst, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME,

	},
	{	// 8D
		LSN_ABSOLUTE_W( STA, Write_A_To_AddrOrPtr_Phi2<LSN_TO_A> )
	},
	{	// 8E
		LSN_ABSOLUTE_W( STX, Write_X_To_AddrOrPtr_Phi2<LSN_TO_A> )
	},
	{	// 8F
		LSN_ABSOLUTE_W( SAX, Sax_Phi2<LSN_TO_A> )
	},

	/** 90-97 */
	{	// 90
		LSN_BRANCH( BCC, C(), 0 )														// Branch if C == 0.
	},
	{	// 91
		LSN_INDIRECT_Y_W( STA, Write_A_To_AddrOrPtr_Phi2<LSN_TO_P> )
		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>,
		//	&CCpu6502::Null<LSN_R>,														&CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>,
		//	&CCpu6502::IndirectYAdd_PtrOrAddr_To_AddrOrPtr<LSN_FROM_A>,					&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>,
		//	&CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_A>,						&CCpu6502::Write_A_To_AddrOrPtr_Phi2<LSN_TO_P>,
		//	&CCpu6502::BeginInst, }, 6, LSN_AM_INDIRECT_Y, 2, //LSN_I_ ## NAME,
	},
	{	// 92
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 93
		LSN_INDIRECT_Y_W( SHA, Sha_Phi2<LSN_TO_P> )
	},
	{	// 94
		LSN_ZERO_PAGE_X_W( STY, Write_Y_To_AddrOrPtr_Phi2<LSN_TO_A> )

		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>,
		//	&CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit<LSN_TO_A>,					&CCpu6502::Write_Y_To_AddrOrPtr_Phi2<LSN_TO_A>,
		//	&CCpu6502::BeginInst, }, 4, LSN_AM_ZERO_PAGE_X, 2, //LSN_I_ ## NAME,
	},
	{	// 95
		LSN_ZERO_PAGE_X_W( STA, Write_A_To_AddrOrPtr_Phi2<LSN_TO_A> )
	},
	{	// 96
		LSN_ZERO_PAGE_Y_W( STX, Write_X_To_AddrOrPtr_Phi2<LSN_TO_A> )

		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>,
		//	&CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A>,						&CCpu6502::Write_X_To_AddrOrPtr_Phi2<LSN_TO_A>,
		//	&CCpu6502::BeginInst, }, 4, LSN_AM_ZERO_PAGE_X, 2, //LSN_I_ ## NAME,
		
	},
	{	// 97
		LSN_ZERO_PAGE_Y_W( SAX, Sax_Phi2<LSN_TO_A> )
	},

	/** 98-9F */
	{	// 98
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Tya_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_TYA,
	},
	{	// 99
		LSN_ABSOLUTE_Y_W( STA, Write_A_To_AddrOrPtr_Phi2<LSN_TO_A> )

		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>,			&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>,
		//	&CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>,						&CCpu6502::Write_A_To_AddrOrPtr_Phi2<LSN_TO_A>,
		//	&CCpu6502::BeginInst, }, 5, LSN_AM_ABSOLUTE_Y, 3, //LSN_I_ ## NAME,
	},
	{	// 9A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Txs_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_TXS,
	},
	{	// 9B
		LSN_ABSOLUTE_Y_W( SHS, Shs_Phi2<LSN_TO_A> )
	},
	{	// 9C
		LSN_ABSOLUTE_X_W( SHY, Shy_Phi2<LSN_TO_A> )

		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A, true, true>,			&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>,
		//	&CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_P>,						&CCpu6502::Shy_Phi2<LSN_TO_A>,
		//	&CCpu6502::BeginInst, }, 5, LSN_AM_ABSOLUTE_Y, 3, //LSN_I_ ## NAME,
	},
	{	// 9D
		LSN_ABSOLUTE_X_W( STA, Write_A_To_AddrOrPtr_Phi2<LSN_TO_A> )
	},
	{	// 9E
		LSN_ABSOLUTE_Y_W( SHX, Shx_Phi2<LSN_TO_A> )
	},
	{	// 9F
		LSN_ABSOLUTE_Y_W( SHA, Sha_Phi2<LSN_TO_A> )
	},

	/** A0-A7 */
	{	// A0
		LSN_IMMEDIATE( LDY, Ldy_BeginInst<true> )
	},
	{	// A1
		LSN_INDIRECT_X_R( LDA, Lda_BeginInst )
	},
	{	// A2
		LSN_IMMEDIATE( LDX, Ldx_BeginInst<true> )
	},
	{	// A3
		LSN_INDIRECT_X_R( LAX, Lax_BeginInst )
	},
	{	// A4
		LSN_ZERO_PAGE_R( LDY, Ldy_BeginInst )
	},
	{	// A5
		LSN_ZERO_PAGE_R( LDA, Lda_BeginInst )
	},
	{	// A6
		LSN_ZERO_PAGE_R( LDX, Ldx_BeginInst )
	},
	{	// A7
		LSN_ZERO_PAGE_R( LAX, Lax_BeginInst )
	},

	/** A8-AF */
	{	// A8
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Tay_BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_TAY,
	},
	{	// A9
		LSN_IMMEDIATE( LDA, Lda_BeginInst<true> )
	},
	{	// AA
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Tax_BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_TAX,
	},
	{	// AB
		LSN_IMMEDIATE( LXA, Lxa_IncPc_BeginInst )
	},
	{	// AC
		LSN_ABSOLUTE_R( LDY, Ldy_BeginInst )
	},
	{	// AD
		LSN_ABSOLUTE_R( LDA, Lda_BeginInst )
	},
	{	// AE
		LSN_ABSOLUTE_R( LDX, Ldx_BeginInst )
	},
	{	// AF
		LSN_ABSOLUTE_R( LAX, Lax_BeginInst )
	},

	/** B0-B7 */
	{	// B0
		LSN_BRANCH( BCS, C(), 1 )														// Branch if C == 1.
	},
	{	// B1
		LSN_INDIRECT_Y_R( LDA, Lda_BeginInst )
	},
	{	// B2
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// B3
		LSN_INDIRECT_Y_R( LAX, Lax_BeginInst )
	},
	{	// B4
		LSN_ZERO_PAGE_X_R( LDY, Ldy_BeginInst )
	},
	{	// B5
		LSN_ZERO_PAGE_X_R( LDA, Lda_BeginInst )
	},
	{	// B6
		LSN_ZERO_PAGE_Y_R( LDX, Ldx_BeginInst )

		//{
		//	/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
		//	&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>,
		//	&CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr<LSN_TO_A>,						&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>,
		//	&CCpu6502::Ldx_BeginInst },
		//4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME,
	},
	{	// B7
		LSN_ZERO_PAGE_Y_R( LAX, Lax_BeginInst )
	},

	/** B8-BF */
	{	// B8
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Clv_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_CLV,
	},
	{	// B9
		LSN_ABSOLUTE_Y_R( LDA, Lda_BeginInst )
	},
	{	// BA
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Tsx_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_TSX,
	},
	{	// BB
		LSN_ABSOLUTE_Y_R( LAS, Las_BeginInst )
	},
	{	// BC
		LSN_ABSOLUTE_X_R( LDY, Ldy_BeginInst )
	},
	{	// BD
		LSN_ABSOLUTE_X_R( LDA, Lda_BeginInst )
	},
	{	// BE
		LSN_ABSOLUTE_Y_R( LDX, Ldx_BeginInst )
	},
	{	// BF
		LSN_ABSOLUTE_Y_R( LAX, Lax_BeginInst )
	},

	/** C0-C7 */
	{	// C0
		LSN_IMMEDIATE( CPY, Cpy_BeginInst<true> )
	},
	{	// C1
		LSN_INDIRECT_X_R( CMP, Cmp_BeginInst )
	},
	{	// C2
		LSN_IMMEDIATE( NOP, BeginInst<true> )
	},
	{	// C3
		LSN_INDIRECT_X_RMW( DCP, Dcp )
	},
	{	// C4
		LSN_ZERO_PAGE_R( CPY, Cpy_BeginInst )
	},
	{	// C5
		LSN_ZERO_PAGE_R( CMP, Cmp_BeginInst )
	},
	{	// C6
		LSN_ZERO_PAGE_RMW( DEC, Dec )
	},
	{	// C7
		LSN_ZERO_PAGE_RMW( DCP, Dcp )
	},

	/** C8-CF */
	{	// C8
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Iny_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_INY,
	},
	{	// C9
		LSN_IMMEDIATE( CMP, Cmp_BeginInst<true> )
	},
	{	// CA
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Dex_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_DEX,
	},
	{	// CB
		LSN_IMMEDIATE( SBX, Sbx_IncPc_BeginInst )
	},
	{	// CC
		LSN_ABSOLUTE_R( CPY, Cpy_BeginInst )
	},
	{	// CD
		LSN_ABSOLUTE_R( CMP, Cmp_BeginInst )
	},
	{	// CE
		LSN_ABSOLUTE_RMW( DEC, Dec )
	},
	{	// CF
		LSN_ABSOLUTE_RMW( DCP, Dcp )
	},
		
	/** D0-D7 */
	{	// D0
		LSN_BRANCH( BNE, Z(), 0 )														// Branch if Z == 0.
	},
	{	// D1
		LSN_INDIRECT_Y_R( CMP, Cmp_BeginInst )
	},
	{	// D2
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// D3
		LSN_INDIRECT_Y_RMW( DCP, Dcp )
	},
	{	// D4
		LSN_ZERO_PAGE_X_R( NOP, BeginInst )
	},
	{	// D5
		LSN_ZERO_PAGE_X_R( CMP, Cmp_BeginInst )
	},
	{	// D6
		LSN_ZERO_PAGE_X_RMW( DEC, Dec )
	},
	{	// D7
		LSN_ZERO_PAGE_X_RMW( DCP, Dcp )
	},

	/** D8-DF */
	{	// D8
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Cld_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_CLD,
	},
	{	// D9
		LSN_ABSOLUTE_Y_R( CMP, Cmp_BeginInst )
	},
	{	// DA
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// DB
		LSN_ABSOLUTE_Y_RMW( DCP, Dcp )
	},
	{	// DC
		LSN_ABSOLUTE_X_R( NOP, BeginInst )
	},
	{	// DD
		LSN_ABSOLUTE_X_R( CMP, Cmp_BeginInst )
	},
	{	// DE
		LSN_ABSOLUTE_X_RMW( DEC, Dec )
	},
	{	// DF
		LSN_ABSOLUTE_X_RMW( DCP, Dcp )
	},

	/** E0-E7 */
	{	// E0
		LSN_IMMEDIATE( CPX, Cpx_BeginInst<true> )
	},
	{	// E1
		LSN_INDIRECT_X_R( SBC, Sbc_BeginInst )
	},
	{	// E2
		LSN_IMMEDIATE( NOP, BeginInst<true> )
	},
	{	// E3
		LSN_INDIRECT_X_RMW( ISB, Isb )
	},
	{	// E4
		LSN_ZERO_PAGE_R( CPX, Cpx_BeginInst )
	},
	{	// E5
		LSN_ZERO_PAGE_R( SBC, Sbc_BeginInst )
	},
	{	// E6
		LSN_ZERO_PAGE_RMW( INC, Inc )
	},
	{	// E7
		LSN_ZERO_PAGE_RMW( ISB, Isb )
	},

	/** E8-EF */
	{	// E8
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Inx_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_INX,
	},
	{	// E9
		LSN_IMMEDIATE( SBC, Sbc_BeginInst<true> )
	},
	{	// EA
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// EB
		LSN_IMMEDIATE( SBC, Sbc_BeginInst<true> )
	},
	{	// EC
		LSN_ABSOLUTE_R( CPX, Cpx_BeginInst )
	},
	{	// ED
		LSN_ABSOLUTE_R( SBC, Sbc_BeginInst )
	},
	{	// EE
		LSN_ABSOLUTE_RMW( INC, Inc )
	},
	{	// EF
		LSN_ABSOLUTE_RMW( ISB, Isb )
	},

	/** F0-F7 */
	{	// F0
		LSN_BRANCH( BEQ, Z(), 1 )														// Branch if Z == 0.
	},
	{	// F1
		LSN_INDIRECT_Y_R( SBC, Sbc_BeginInst )
	},
	{	// F2
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFE, false>,
			&CCpu6502::Null<LSN_R>,														&CCpu6502::ReadAddr_Phi2<0xFFFF, true>, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// F3
		LSN_INDIRECT_Y_RMW( ISB, Isb )
	},
	{	// F4
		LSN_ZERO_PAGE_X_R( NOP, BeginInst )
	},
	{	// F5
		LSN_ZERO_PAGE_X_R( SBC, Sbc_BeginInst )
	},
	{	// F6
		LSN_ZERO_PAGE_X_RMW( INC, Inc )
	},
	{	// F7
		LSN_ZERO_PAGE_X_RMW( ISB, Isb )
	},

	/** F8-FF */
	{	// F8
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Sed_BeginInst, },
			2, LSN_AM_IMPLIED, 1, LSN_I_SED,
	},
	{	// F9
		LSN_ABSOLUTE_Y_R( SBC, Sbc_BeginInst )
	},
	{	// FA
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::BeginInst, },
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// FB
		LSN_ABSOLUTE_Y_RMW( ISB, Isb )
	},
	{	// FC
		LSN_ABSOLUTE_X_R( NOP, BeginInst )
	},
	{	// FD
		LSN_ABSOLUTE_X_R( SBC, Sbc_BeginInst )
	},
	{	// FE
		LSN_ABSOLUTE_X_RMW( INC, Inc )
	},
	{	// FF
		LSN_ABSOLUTE_X_RMW( ISB, Isb )
	},
};
