#define LSN_INDIRECT_X_R( NAME, FUNC )													{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::AddXAndOperandToPointer, &CCpu6502::ReadOperandAndDiscard_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::AddXAndOperandToPointer, &CCpu6502::ReadOperandAndDiscard_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 8, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME

#define LSN_INDIRECT_Y_R( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchPointerAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 8, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_R( NAME, FUNC )													{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadOperandToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_RMW( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 5, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_X_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 6, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME

#define LSN_IMMEDIATE( NAME, FUNC )														{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp }, 2, LSN_AM_IMMEDIATE, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_RMW( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp }, 6, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME

#define LSN_ABSOLUTE_X_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 7, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME

#define LSN_ABSOLUTE_Y_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME

#define LSN_BRANCH( NAME, COND, VALUE )													{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::Branch_Cycle2<COND, VALUE>, &CCpu6502::Branch_Cycle2_Phi2, &CCpu6502::Branch_Cycle3, &CCpu6502::Branch_Cycle3_Phi2, &CCpu6502::Branch_Cycle4, &CCpu6502::PrefetchNextOp }, 2, LSN_AM_RELATIVE, 2, LSN_I_ ## NAME
 
CCpu6502::LSN_INSTR CCpu6502::m_iInstructionSet[256] = {								/**< The instruction set. */
	/** 00-07 */
	{	// 00
		{
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Write, &CCpu6502::PushPch_Phi2,
			&CCpu6502::NullAdjustS_Write, &CCpu6502::PushPcl_Phi2,
			&CCpu6502::PushStatus, &CCpu6502::PushStatus_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::CopyVectorPcl_Phi2,
			&CCpu6502::CopyVectorPch, &CCpu6502::CopyVectorPch_Phi2,
			&CCpu6502::Brk, &CCpu6502::PrefetchNextOp },
		7, LSN_AM_IMPLIED, 2, LSN_I_BRK,
	},
	{	// 01
		LSN_INDIRECT_X_R( ORA, Ora )
	},
	{	// 02
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		2, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 03
		LSN_INDIRECT_X_RMW( SLO, Slo )
	},
	{	// 04
		LSN_ZERO_PAGE_R( NOP, Null_Read )
	},
	{	// 05
		LSN_ZERO_PAGE_R( ORA, Ora )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Php, &CCpu6502::PushOperand_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::PrefetchNextOp },
			3, LSN_AM_IMPLIED, 1, LSN_I_PHP,
	},
	{	// 09
		LSN_IMMEDIATE( ORA, OraAndIncPc )
		/*{
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::OraAndIncPc, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMMEDIATE, 2, LSN_I_ORA,*/
	},
	{	// 0A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::AslOnA, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_ASL,
	},
	{	// 0B
		LSN_IMMEDIATE( ANC, AncAndIncPc )
	},
	{	// 0C
		LSN_ABSOLUTE_R( NOP, Null_Read )
	},
	{	// 0D
		LSN_ABSOLUTE_R( ORA, Ora )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Ora, &CCpu6502::PrefetchNextOp },
			4, LSN_AM_ABSOLUTE, 3, LSN_I_ORA,*/
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
		/*{
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::Branch_Cycle2<N(), 0>, &CCpu6502::Branch_Cycle2_Phi2,
			&CCpu6502::Branch_Cycle3, &CCpu6502::Branch_Cycle3_Phi2,
			&CCpu6502::Branch_Cycle4, &CCpu6502::PrefetchNextOp
		},
		2, LSN_AM_RELATIVE, 2, LSN_I_BPL,*/
	},
	{	// 11
		LSN_INDIRECT_Y_R( ORA, Ora )
		/*{
			&CCpu6502::FetchPointerAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2,
			&CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Ora, &CCpu6502::PrefetchNextOp,
		},
		5, LSN_AM_INDIRECT_Y, 2, LSN_I_ORA,*/
	},
	{	// 12
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		2, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 13
		LSN_INDIRECT_Y_RMW( SLO, Slo )
		/*{
			&CCpu6502::FetchPointerAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2,
			&CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Slo, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		},
		8, LSN_AM_INDIRECT_Y, 2, LSN_I_SLO,*/
	},
	{	// 14
		LSN_ZERO_PAGE_X_R( NOP, Null_Read )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		},
		4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_NOP,*/
	},
	{	// 15
		LSN_ZERO_PAGE_X_R( ORA, Ora )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Ora, &CCpu6502::PrefetchNextOp,
		},
		4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ORA,*/
	},
	{	// 16
		LSN_ZERO_PAGE_X_RMW( ASL, Asl )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Asl, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		},
		6, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ASL,*/
	},
	{	// 17
		LSN_ZERO_PAGE_X_RMW( SLO, Slo )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Slo, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		},
		6, LSN_AM_ZERO_PAGE_X, 2, LSN_SLO,*/
	},


	/** 18-1F */
	{	// 18
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Clc, &CCpu6502::PrefetchNextOp, },
		2, LSN_AM_IMPLIED, 1, LSN_I_CLC,
	},
	{	// 19
		LSN_ABSOLUTE_Y_R( ORA, Ora )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Ora, &CCpu6502::PrefetchNextOp,
		},
		4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ORA,*/
	},
	{	// 1A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp
		},
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 1B
		LSN_ABSOLUTE_Y_RMW( SLO, Slo )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Slo, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		},
		7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_SLO*/
	},
	{	// 1C
		LSN_ABSOLUTE_X_R( NOP, Null_Read )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		},
		4, LSN_AM_ABSOLUTE_X, 3, LSN_I_NOP*/
	},
	{	// 1D
		LSN_ABSOLUTE_X_R( ORA, Ora )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Ora, &CCpu6502::PrefetchNextOp,
		},
		4, LSN_AM_ABSOLUTE_X, 3, LSN_I_NOP*/
	},
	{	// 1E
		LSN_ABSOLUTE_X_RMW( ASL, Asl )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Asl, &CCpu6502::WriteOperandToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		},
		7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ASL*/
	},
	{	// 1F
		LSN_ABSOLUTE_X_RMW( SLO, Slo )
	},

	/** 20-27 */
	{	// 20
		{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadStackToOperand_Phi2,
			&CCpu6502::NullIncPc_Write, &CCpu6502::PushPch_Phi2,
			&CCpu6502::NullAdjustS_Write, &CCpu6502::PushPcl_Phi2,
			&CCpu6502::NullAdjustS_Write, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::Jsr, &CCpu6502::PrefetchNextOp,
		},
		6, LSN_AM_ABSOLUTE, 3, LSN_I_JSR,
	},
	{	// 21
		LSN_INDIRECT_X_R( AND, And )
	},
	{	// 22
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		2, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 23
		LSN_INDIRECT_X_RMW( RLA, Rla )
	},
	{	// 24
		LSN_ZERO_PAGE_R( BIT, Bit )
	},
	{	// 25
		LSN_ZERO_PAGE_R( AND, And )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PullStackToOperand_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::ReadStackToOperand_Phi2,
			&CCpu6502::Plp, &CCpu6502::PrefetchNextOp },
			4, LSN_AM_IMPLIED, 1, LSN_I_PLP,
	},
	{	// 29
		LSN_IMMEDIATE( AND, AndAndIncPc )
		/*{
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::AndAndIncPc, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMMEDIATE, 2, LSN_I_ORA,*/
	},
	{	// 2A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::RolOnA, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_ROL,
	},
	{	// 2B
		LSN_IMMEDIATE( ANC, AncAndIncPc )
	},
	{	// 2C
		LSN_ABSOLUTE_R( BIT, Bit )
	},
	{	// 2D
		LSN_ABSOLUTE_R( AND, And )
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
		LSN_INDIRECT_Y_R( AND, And )
	},
	{	// 32
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		2, LSN_AM_IMPLIED, 2, LSN_I_JAM
	},
	{	// 33
		LSN_INDIRECT_Y_RMW( RLA, Rla )
	},
	{	// 34
		LSN_ZERO_PAGE_X_R( NOP, Null_Read )
	},
	{	// 35
		LSN_ZERO_PAGE_X_R( AND, And )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Sec, &CCpu6502::PrefetchNextOp, },
		2, LSN_AM_IMPLIED, 1, LSN_I_SEC,
	},
	{	// 39
		LSN_ABSOLUTE_Y_R( AND, And )
	},
	{	// 3A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 3B
		LSN_ABSOLUTE_Y_RMW( RLA, Rla )
	},
	{	// 3C
		LSN_ABSOLUTE_X_R( NOP, Null_Read )
	},
	{	// 3D
		LSN_ABSOLUTE_X_R( AND, And )
	},
	{	// 3E
		LSN_ABSOLUTE_X_RMW( ROL, Rol )
	},
	{	// 3F
		LSN_ABSOLUTE_X_RMW( RLA, Rla )
	},

	/** 40-47 */
};

//LSN_INDIRECT_X_R( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc, &CCpu6502::ReadAddressAddX_IzX, &CCpu6502::FetchEffectiveAddressLow_IzX, &CCpu6502::FetchEffectiveAddressHigh_IzX, &CCpu6502::FUNC, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
