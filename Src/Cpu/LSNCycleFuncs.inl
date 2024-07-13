#define LSN_INDIRECT_X_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr_8bit<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr_8bit<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 8, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME

#define LSN_INDIRECT_Y_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::IndirectYAdd_PtrOrAddr_To_AddrOrPtr<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_BoundarySkip_Phi2<LSN_FROM_P, true>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P, true>, &CCpu6502::FUNC, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME,
#define LSN_INDIRECT_Y_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::IndirectYAdd_PtrOrAddr_To_AddrOrPtr<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H<LSN_FROM_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_P>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_P, true>, &CCpu6502::BeginInst, }, 8, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME,

#define LSN_ZERO_PAGE_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_X_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit<LSN_TO_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME,
#define LSN_ZERO_PAGE_X_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>, &CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit<LSN_TO_A>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 6, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME,

#define LSN_ZERO_PAGE_RMW( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 5, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_IMMEDIATE( NAME, FUNC )														{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2<true>, &CCpu6502::FUNC, }, 2, LSN_AM_IMMEDIATE, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_RMW( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 6, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME

#define LSN_BRANCH( NAME, COND, VALUE )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Branch_Cycle1<COND, VALUE>, &CCpu6502::Branch_Cycle1_Phi2, &CCpu6502::Branch_Cycle2, &CCpu6502::Branch_Cycle2_Phi2, &CCpu6502::Branch_Cycle3, &CCpu6502::Branch_Cycle3_Phi2, &CCpu6502::Branch_Cycle4, }, 2, LSN_AM_RELATIVE, 2, LSN_I_ ## NAME,
 
CCpu6502::LSN_INSTR CCpu6502::m_iInstructionSet[256] = {								/**< The instruction set. */
	/** 00-07 */
	{	// 00
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_IncPc_Phi2,
			&CCpu6502::Null<LSN_W, true>,												&CCpu6502::PushPc_H_Phi2<0>,
			&CCpu6502::Null<LSN_W>,														&CCpu6502::PushPc_L_Phi2<-1>,
			&CCpu6502::SelectBrkVectors,												&CCpu6502::PushS_Phi2<-2>,
			&CCpu6502::Null<LSN_R, false, true>,										&CCpu6502::CopyVectorToPc_L_Phi2,
			&CCpu6502::SetBrkFlags,														&CCpu6502::CopyVectorToPc_H_Phi2,
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
			&CCpu6502::Php,																&CCpu6502::PushOperand_Phi2<0, true>,
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
			///* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			//&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_P>,
			//&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_P>,
			//&CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit<LSN_TO_A>,					&CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>,
			//&CCpu6502::Null<LSN_W>,														&CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>,
			//&CCpu6502::Asl,																&CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>,
			//&CCpu6502::BeginInst
	},
	{	// 17
		LSN_ZERO_PAGE_X_RMW( SLO, Slo )
	},


	/** 18-1F */
	{	// 18
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Fetch_Operand_Discard_Phi2<true>,
			&CCpu6502::Clc_BeginInst,

			/*&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Clc, &CCpu6502::PrefetchNextOp,*/ },
		2, LSN_AM_IMPLIED, 1, LSN_I_CLC,
	},
};
