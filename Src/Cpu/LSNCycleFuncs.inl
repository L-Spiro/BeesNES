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
			&CCpu6502::Null<LSN_W, true>,												&CCpu6502::Push_Pc_H_Phi2<0>,
			&CCpu6502::Null<LSN_W>,														&CCpu6502::Push_Pc_L_Phi2<-1>,
			&CCpu6502::SelectBrkVectors,												&CCpu6502::Push_S_Phi2<-2>,
			&CCpu6502::Null<LSN_R, false, true>,										&CCpu6502::CopyVectorToPc_L_Phi2,
			&CCpu6502::SetBrkFlags,														&CCpu6502::CopyVectorToPc_H_Phi2<true>,
			&CCpu6502::Brk_BeginInst },
		7, LSN_AM_IMPLIED, 2, LSN_I_BRK,
	},
	{	// 01
		LSN_INDIRECT_X_R( ORA, Ora_BeginInst )
	},
	{	// 02
		{
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
		/*{
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::AndAndIncPc, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMMEDIATE, 2, LSN_I_ORA,*/
	},
	{	// 2A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::RolOnA_BeginInst,
			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::RolOnA, &CCpu6502::PrefetchNextOp*/ },
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
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
		/*{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Sec, &CCpu6502::PrefetchNextOp, },*/
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
			&CCpu6502::Null<LSN_R>,														&CCpu6502::Read_Stack_To_Target_H_Phi2<3, true>,
			&CCpu6502::Rti_BeginInst, },
			6, LSN_AM_IMPLIED, 1, LSN_I_RTI,
	},
	{	// 41
		LSN_INDIRECT_X_R( EOR, Eor_BeginInst )
	},
	{	// 42
		{
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
			&CCpu6502::BeginInst<false, true>,

			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::PushA_Phi2,
			&CCpu6502::NullAdjustS_Write, &CCpu6502::PrefetchNextOp*/ },
		3, LSN_AM_IMPLIED, 1, LSN_I_PHA,
	},
	{	// 49
		LSN_IMMEDIATE( EOR, Eor_BeginInst<true> )
	},
	{	// 4A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::LsrOnA_BeginInst,
			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::LsrOnA, &CCpu6502::PrefetchNextOp,*/ },
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
			&CCpu6502::Jmp_BeginInst
			/*&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::Jmp, &CCpu6502::PrefetchNextOp,*/ },
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
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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

			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Cli, &CCpu6502::PrefetchNextOp, },*/
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
			&CCpu6502::CopyTargetToPc<true>,											&CCpu6502::Fetch_Operand_IncPc_Phi2<true>,
			&CCpu6502::Rts_BeginInst,

			/*&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::PullStackToOperand_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::PullStackToPcl_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::ReadStackToPch_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::PrefetchNextOp*/ },
			6, LSN_AM_IMPLIED, 1, LSN_I_RTS,
	},
	{	// 61
		LSN_INDIRECT_X_R( ADC, Adc_BeginInst )
	},
	{	// 62
		{
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
			&CCpu6502::Pla_BeginInst,

			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PullStackToOperand_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::ReadStackToOperand_Phi2,
			&CCpu6502::Pla, &CCpu6502::PrefetchNextOp*/ },
			4, LSN_AM_IMPLIED, 1, LSN_I_PLP,
	},
	{	// 69
		LSN_IMMEDIATE( ADC, Adc_BeginInst<true> )
	},
	{	// 6A
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::RorOnA_BeginInst, },

			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::RorOnA, &CCpu6502::PrefetchNextOp },*/
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
			&CCpu6502::Jmp_BeginInst,

			/*&CCpu6502::FetchPointerAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchPointerHighAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_SamePage_Phi2,
			&CCpu6502::Jmp, &CCpu6502::PrefetchNextOp*/ },
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
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
			&CCpu6502::Sei_BeginInst,
			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Sei, &CCpu6502::PrefetchNextOp,*/ },
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
			// TODO: Use this for LSN_CPU_VERIFY, write real routine otherwise.
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Jam,																&CCpu6502::Jam_Phi2 },
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
};
