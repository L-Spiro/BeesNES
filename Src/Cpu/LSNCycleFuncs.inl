#define LSN_INDIRECT_X_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_RMW( NAME, FUNC )												{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 8, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_RMW( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 5, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_IMMEDIATE( NAME, FUNC )														{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2<true>, &CCpu6502::FUNC, }, 2, LSN_AM_IMMEDIATE, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A, true>, &CCpu6502::FUNC, }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_RMW( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2<LSN_TO_A>, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>,  &CCpu6502::Null<LSN_W>, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A>, &CCpu6502::FUNC, &CCpu6502::Write_Operand_To_AddrOrPtr_Phi2<LSN_TO_A, true>, &CCpu6502::BeginInst, }, 6, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
 
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
		//LSN_BRANCH( BPL, N(), 0 )														// Branch if N == 0.
		{
			/* BeginInst() */															&CCpu6502::Fetch_Opcode_IncPc_Phi2,
			&CCpu6502::Branch_Cycle1<N(), 0>,											&CCpu6502::Branch_Cycle1_Phi2,
			&CCpu6502::Null<LSN_R, true>,												&CCpu6502::Branch_Cycle2_Phi2,
			&CCpu6502::Branch_Cycle3,													&CCpu6502::Branch_Cycle3_Phi2,
			&CCpu6502::Branch_Cycle4,
			/*&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::Branch_Cycle2<N(), 0>, &CCpu6502::Branch_Cycle2_Phi2,
			&CCpu6502::Branch_Cycle3, &CCpu6502::Branch_Cycle3_Phi2,
			&CCpu6502::Branch_Cycle4, &CCpu6502::PrefetchNextOp*/
		},
		2, LSN_AM_RELATIVE, 2, LSN_I_BPL,
	},

};
