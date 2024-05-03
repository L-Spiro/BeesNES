#define LSN_INDIRECT_X_R( NAME, FUNC )													{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::AddXAndOperandToPointer, &CCpu6502::ReadOperandAndDiscard_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::AddXAndOperandToPointer, &CCpu6502::ReadOperandAndDiscard_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 8, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_X_W( NAME, FUNC )													{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::AddXAndOperandToPointer, &CCpu6502::ReadOperandAndDiscard_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::Null_Write, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME

#define LSN_INDIRECT_Y_R( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchPointerAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 8, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME
#define LSN_INDIRECT_Y_W( NAME, FUNC )													{ &CCpu6502::FetchPointerAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_RMW( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 5, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Write, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_X_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndX, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 6, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_X_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndX, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ZERO_PAGE_X, 2, LSN_I_ ## NAME

#define LSN_ZERO_PAGE_Y_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndY, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME
#define LSN_ZERO_PAGE_Y_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndY, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME

#define LSN_IMMEDIATE( NAME, FUNC )														{ &CCpu6502::FetchOperandAndIncPc_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp }, 2, LSN_AM_IMMEDIATE, 2, LSN_I_ ## NAME

#define LSN_ABSOLUTE_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_RMW( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp }, 6, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_ ## NAME

#define LSN_ABSOLUTE_X_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_X_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 7, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_X_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME

#define LSN_ABSOLUTE_Y_R( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_Y_RMW( NAME, FUNC )												{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::Null_Write, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::FUNC, &CCpu6502::WriteOperandToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 7, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME
#define LSN_ABSOLUTE_Y_W( NAME, FUNC )													{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::FUNC, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME

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
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
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
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
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
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
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
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM
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
	{	// 40
		{
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::PullStackToOperand_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::PullStackToOperand_Phi2,
			&CCpu6502::CopyOperandToStatusWithoutB_AdjustS, &CCpu6502::PullStackToPcl_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::ReadStackToPch_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::PrefetchNextOp },
			6, LSN_AM_IMPLIED, 1, LSN_I_RTI,
	},
	{	// 41
		LSN_INDIRECT_X_R( EOR, Eor )
	},
	{	// 42
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM
	},
	{	// 43
		LSN_INDIRECT_X_RMW( SRE, Sre )
	},
	{	// 44
		LSN_ZERO_PAGE_R( NOP, Null_Read )
	},
	{	// 45
		LSN_ZERO_PAGE_R( EOR, Eor )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::PushA_Phi2,
			&CCpu6502::NullAdjustS_Write, &CCpu6502::PrefetchNextOp, },
		3, LSN_AM_IMPLIED, 1, LSN_I_PHA,
	},
	{	// 49
		LSN_IMMEDIATE( EOR, EorAndIncPc )
	},
	{	// 4A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::LsrOnA, &CCpu6502::PrefetchNextOp, },
		2, LSN_AM_IMPLIED, 1, LSN_I_LSR,
	},
	{	// 4B
		LSN_IMMEDIATE( ASR, AsrAndIncPc )
	},
	{	// 4C
		{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::Jmp, &CCpu6502::PrefetchNextOp, },
		3, LSN_AM_ABSOLUTE, 3, LSN_I_JMP,
	},
	{	// 4D
		LSN_ABSOLUTE_R( EOR, Eor )
	},
	{	// 4E
		LSN_ABSOLUTE_RMW( LSR, Lsr )
	},
	{	// 4F
		LSN_ABSOLUTE_RMW( SRE, Sre )
	},

	/** 50-57 */
	{	// 50
		LSN_BRANCH( BVC, V(), 0 )													// Branch if V == 0.
	},
	{	// 51
		LSN_INDIRECT_Y_R( EOR, Eor )
	},
	{	// 52
		{
		&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 53
		LSN_INDIRECT_Y_RMW( SRE, Sre )
	},
	{	// 54
		LSN_ZERO_PAGE_X_R( NOP, Null_Read )
	},
	{	// 55
		LSN_ZERO_PAGE_X_R( EOR, Eor )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Cli, &CCpu6502::PrefetchNextOp, },
		2, LSN_AM_IMPLIED, 1, LSN_I_CLI
	},
	{	// 59
		LSN_ABSOLUTE_Y_R( EOR, Eor )
	},
	{	// 5A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp
		},
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 5B
		LSN_ABSOLUTE_Y_RMW( SRE, Sre )
	},
	{	// 5C
		LSN_ABSOLUTE_X_R( NOP, Null_Read )
	},
	{	// 5D
		LSN_ABSOLUTE_X_R( EOR, Eor )
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
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::PullStackToOperand_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::PullStackToPcl_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::ReadStackToPch_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::PrefetchNextOp },
			6, LSN_AM_IMPLIED, 1, LSN_I_RTS,
	},
	{	// 61
		LSN_INDIRECT_X_R( ADC, Adc )
	},
	{	// 62
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 63
		LSN_INDIRECT_X_RMW( RRA, Rra )
	},
	{	// 64
		LSN_ZERO_PAGE_R( NOP, Null_Read )
	},
	{	// 65
		LSN_ZERO_PAGE_R( ADC, Adc )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PullStackToOperand_Phi2,
			&CCpu6502::NullAdjustS_Read, &CCpu6502::ReadStackToOperand_Phi2,
			&CCpu6502::Pla, &CCpu6502::PrefetchNextOp },
			4, LSN_AM_IMPLIED, 1, LSN_I_PLP,
	},
	{	// 69
		LSN_IMMEDIATE( ADC, AdcAndIncPc )
	},
	{	// 6A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::RorOnA, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_ROR,
	},
	{	// 6B
		LSN_IMMEDIATE( ARR, ArrAndIncPc )
	},
	{	// 6C
		{
			&CCpu6502::FetchPointerAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchPointerHighAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_SamePage_Phi2,
			&CCpu6502::Jmp, &CCpu6502::PrefetchNextOp },
			5, LSN_AM_INDIRECT, 3, LSN_I_JMP,
	},
	{	// 6D
		LSN_ABSOLUTE_R( ADC, Adc )
	},
	{	// 6E
		LSN_ABSOLUTE_RMW( ROR, Ror )
	},
	{	// 6F
		LSN_ABSOLUTE_RMW( RRA, Rra )
	},

	/** 70-77 */
	{	// 70
		LSN_BRANCH( BVS, V(), 1 )													// Branch if V == 1.
	},
	{	// 71
		LSN_INDIRECT_Y_R( ADC, Adc )
	},
	{	// 72
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 73
		LSN_INDIRECT_Y_RMW( RRA, Rra )
	},
	{	// 74
		LSN_ZERO_PAGE_X_R( NOP, Null_Read )
	},
	{	// 75
		LSN_ZERO_PAGE_X_R( ADC, Adc )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Sei, &CCpu6502::PrefetchNextOp, },
		2, LSN_AM_IMPLIED, 1, LSN_I_SEI
	},
	{	// 79
		LSN_ABSOLUTE_Y_R( ADC, Adc )
	},
	{	// 7A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp
		},
		2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// 7B
		LSN_ABSOLUTE_Y_RMW( RRA, Rra )
	},
	{	// 7C
		LSN_ABSOLUTE_X_R( NOP, Null_Read )
	},
	{	// 7D
		LSN_ABSOLUTE_X_R( ADC, Adc )
	},
	{	// 7E
		LSN_ABSOLUTE_X_RMW( ROR, Ror )
	},
	{	// 7F
		LSN_ABSOLUTE_X_RMW( RRA, Rra )
	},

	/** 80-87 */
	{	// 80
		LSN_IMMEDIATE( NOP, NullIncPc_Read )
	},
	{	// 81
		LSN_INDIRECT_X_W( STA, WriteAToAddress_Phi2 )
		/*{
			&CCpu6502::FetchOperandAndIncPc_Phi2,
			&CCpu6502::AddXAndOperandToPointer, &CCpu6502::ReadOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressLow_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2,
			&CCpu6502::Null_Write, &CCpu6502::WriteAToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		}, 6, LSN_AM_INDIRECT_X, 2, LSN_I_STA*/
	},
	{	// 82
		LSN_IMMEDIATE( NOP, NullIncPc_Read )
	},
	{	// 83
		LSN_INDIRECT_X_W( SAX, Sax_Phi2 )
	},
	{	// 84
		LSN_ZERO_PAGE_W( STY, WriteYToAddress_Phi2 )
		//{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Write, &CCpu6502::WriteYToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp }, 3, LSN_AM_ZERO_PAGE, 2, LSN_I_STA
	},
	{	// 85
		LSN_ZERO_PAGE_W( STA, WriteAToAddress_Phi2 )
	},
	{	// 86
		LSN_ZERO_PAGE_W( STX, WriteXToAddress_Phi2 )
	},
	{	// 87
		LSN_ZERO_PAGE_W( SAX, Sax_Phi2 )
	},

	/** 88-8F */
	{	// 88
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Dey, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_ASL,
	},
	{	// 89
		LSN_IMMEDIATE( NOP, NullIncPc_Read )
	},
	{	// 8A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Txa, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_TXA,
	},
	{	// 8B
		LSN_IMMEDIATE( ANE, AneAndIncPc )
	},
	{	// 8C
		LSN_ABSOLUTE_W( STY, WriteYToAddress_Phi2 )
		//{ &CCpu6502::FetchAddressAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::WriteYToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp }, 4, LSN_AM_ABSOLUTE, 3, LSN_I_STY
	},
	{	// 8D
		LSN_ABSOLUTE_W( STA, WriteAToAddress_Phi2 )
	},
	{	// 8E
		LSN_ABSOLUTE_W( STX, WriteXToAddress_Phi2 )
	},
	{	// 8F
		LSN_ABSOLUTE_W( SAX, Sax_Phi2 )
	},

	/** 90-97 */
	{	// 90
		LSN_BRANCH( BCC, C(), 0 )													// Branch if C == 0.
	},
	{	// 91
		LSN_INDIRECT_Y_W( STA, WriteAToAddress_Phi2 )
		//{ &CCpu6502::FetchPointerAndIncPc_Phi2, &CCpu6502::NullIncPc_Read, &CCpu6502::ReadPointerToAddressLow_Phi2, &CCpu6502::Null_Read, &CCpu6502::ReadPointerToAddressHigh_Phi2, &CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2, &CCpu6502::AddAddressAndXorYHigh, &CCpu6502::WriteAToAddress_Phi2, &CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 5, LSN_AM_INDIRECT_Y, 2, LSN_I_STA
	},
	{	// 92
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// 93
		LSN_INDIRECT_Y_W( SHA, Sha_Phi2 )
	},
	{	// 94
		LSN_ZERO_PAGE_X_W( STY, WriteYToAddress_Phi2 )
	},
	{	// 95
		LSN_ZERO_PAGE_X_W( STA, WriteAToAddress_Phi2 )
	},
	{	// 96
		LSN_ZERO_PAGE_Y_W( STX, WriteXToAddress_Phi2 )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndY, &CCpu6502::WriteXToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp,
		}, 4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME*/
	},
	{	// 97
		LSN_ZERO_PAGE_Y_W( SAX, Sax_Phi2 )
	},

	/** 98-9F */
	{	// 98
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Tya, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_TYA,
	},
	{	// 99
		LSN_ABSOLUTE_Y_W( STA, WriteAToAddress_Phi2 )
	},
	{	// 9A
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Txs, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_TXS,
	},
	{	// 9B
		LSN_ABSOLUTE_Y_W( SHS, Shs_Phi2 )
	},
	{	// 9C
		LSN_ABSOLUTE_X_W( SHY, Shy_Phi2 )
	},
	{	// 9D
		LSN_ABSOLUTE_X_W( STA, WriteAToAddress_Phi2 )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::AddAddressAndXIncPc, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::WriteAToAddress_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_X, 3, LSN_I_ ## NAME*/
	},
	{	// 9E
		LSN_ABSOLUTE_Y_W( SHX, Shx_Phi2 )
	},
	{	// 9F
		LSN_ABSOLUTE_Y_W( SHA, Sha_Phi2 )
		/*{	
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::FetchAddressHighAndIncPc_Phi2,
			&CCpu6502::AddAddressAndYAndIncPc, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndXorYHigh, &CCpu6502::Sha_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ABSOLUTE_Y, 3, LSN_I_ ## NAME*/
	},

	/** A0-A7 */
	{	// A0
		LSN_IMMEDIATE( LDY, LdyAndIncPc )
	},
	{	// A1
		LSN_INDIRECT_X_R( LDA, Lda )
	},
	{	// A2
		LSN_IMMEDIATE( LDX, LdxAndIncPc )
	},
	{	// A3
		LSN_INDIRECT_X_R( LAX, Lax )
	},
	{	// A4
		LSN_ZERO_PAGE_R( LDY, Ldy )
	},
	{	// A5
		LSN_ZERO_PAGE_R( LDA, Lda )
	},
	{	// A6
		LSN_ZERO_PAGE_R( LDX, Ldx )
	},
	{	// A7
		LSN_ZERO_PAGE_R( LAX, Lax )
	},

	/** A8-AF */
	{	// A8
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Tay, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_TAY,
	},
	{	// A9
		LSN_IMMEDIATE( LDA, LdaAndIncPc )
	},
	{	// AA
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Tax, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_TAX,
	},
	{	// AB
		LSN_IMMEDIATE( LXA, LxaAndIncPc )
	},
	{	// AC
		LSN_ABSOLUTE_R( LDY, Ldy )
	},
	{	// AD
		LSN_ABSOLUTE_R( LDA, Lda )
	},
	{	// AE
		LSN_ABSOLUTE_R( LDX, Ldx )
	},
	{	// AF
		LSN_ABSOLUTE_R( LAX, Lax )
	},

	/** B0-B7 */
	{	// B0
		LSN_BRANCH( BCS, C(), 1 )											// Branch if C == 1.
	},
	{	// B1
		LSN_INDIRECT_Y_R( LDA, Lda )
	},
	{	// B2
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// B3
		LSN_INDIRECT_Y_R( LAX, Lax )
	},
	{	// B4
		LSN_ZERO_PAGE_X_R( LDY, Ldy )
	},
	{	// B5
		LSN_ZERO_PAGE_X_R( LDA, Lda )
	},
	{	// B6
		LSN_ZERO_PAGE_Y_R( LDX, Ldx )
		/*{
			&CCpu6502::FetchAddressAndIncPc_Phi2,
			&CCpu6502::NullIncPc_Read, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::AddAddressAndY, &CCpu6502::ReadAddressToOperand_Phi2,
			&CCpu6502::FUNC, &CCpu6502::PrefetchNextOp, }, 4, LSN_AM_ZERO_PAGE_Y, 2, LSN_I_ ## NAME*/
	},
	{	// B7
		LSN_ZERO_PAGE_Y_R( LAX, Lax )
	},

	/** B8-BF */
	{	// B8
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Clv, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_CLV,
	},
	{	// B9
		LSN_ABSOLUTE_Y_R( LDA, Lda )
	},
	{	// BA
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Tsx, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_TSX,
	},
	{	// BB
		LSN_ABSOLUTE_Y_R( LAS, Las )
	},
	{	// BC
		LSN_ABSOLUTE_X_R( LDY, Ldy )
	},
	{	// BD
		LSN_ABSOLUTE_X_R( LDA, Lda )
	},
	{	// BE
		LSN_ABSOLUTE_Y_R( LDX, Ldx )
	},
	{	// BF
		LSN_ABSOLUTE_Y_R( LAX, Lax )
	},

	/** C0-C7 */
	{	// C0
		LSN_IMMEDIATE( CPY, CpyAndIncPc )
	},
	{	// C1
		LSN_INDIRECT_X_R( CMP, Cmp )
	},
	{	// C2
		LSN_IMMEDIATE( NOP, NullIncPc_Read )
	},
	{	// C3
		LSN_INDIRECT_X_RMW( DCP, Dcp )
	},
	{	// C4
		LSN_ZERO_PAGE_R( CPY, Cpy )
	},
	{	// C5
		LSN_ZERO_PAGE_R( CMP, Cmp )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Iny, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_INY,
	},
	{	// C9
		LSN_IMMEDIATE( CMP, CmpAndIncPc )
	},
	{	// CA
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Dex, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_DEX,
	},
	{	// CB
		LSN_IMMEDIATE( SBX, SbxAndIncPc )
	},
	{	// CC
		LSN_ABSOLUTE_R( CPY, Cpy )
	},
	{	// CD
		LSN_ABSOLUTE_R( CMP, Cmp )
	},
	{	// CE
		LSN_ABSOLUTE_RMW( DEC, Dec )
	},
	{	// CF
		LSN_ABSOLUTE_RMW( DCP, Dcp )
	},
		
	/** D0-D7 */
	{	// D0
		LSN_BRANCH( BNE, Z(), 0 )											// Branch if Z == 0.
	},
	{	// D1
		LSN_INDIRECT_Y_R( CMP, Cmp )
	},
	{	// D2
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// D3
		LSN_INDIRECT_Y_RMW( DCP, Dcp )
	},
	{	// D4
		LSN_ZERO_PAGE_X_R( NOP, Null_Read )
	},
	{	// D5
		LSN_ZERO_PAGE_X_R( CMP, Cmp )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Cld, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_CLD,
	},
	{	// D9
		LSN_ABSOLUTE_Y_R( CMP, Cmp )
	},
	{	// DA
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// DB
		LSN_ABSOLUTE_Y_RMW( DCP, Dcp )
	},
	{	// DC
		LSN_ABSOLUTE_X_R( NOP, Null_Read )
	},
	{	// DD
		LSN_ABSOLUTE_X_R( CMP, Cmp )
	},
	{	// DE
		LSN_ABSOLUTE_X_RMW( DEC, Dec )
	},
	{	// DF
		LSN_ABSOLUTE_X_RMW( DCP, Dcp )
	},

	/** E0-E7 */
	{	// E0
		LSN_IMMEDIATE( CPX, CpxAndIncPc )
	},
	{	// E1
		LSN_INDIRECT_X_R( SBC, Sbc )
	},
	{	// E2
		LSN_IMMEDIATE( NOP, NullIncPc_Read )
	},
	{	// E3
		LSN_INDIRECT_X_RMW( ISB, Isb )
	},
	{	// E4
		LSN_ZERO_PAGE_R( CPX, Cpx )
	},
	{	// E5
		LSN_ZERO_PAGE_R( SBC, Sbc )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Inx, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_INX,
	},
	{	// E9
		LSN_IMMEDIATE( SBC, SbcAndIncPc )
	},
	{	// EA
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// EB
		LSN_IMMEDIATE( SBC, SbcAndIncPc )
	},
	{	// EC
		LSN_ABSOLUTE_R( CPX, Cpx )
	},
	{	// ED
		LSN_ABSOLUTE_R( SBC, Sbc )
	},
	{	// EE
		LSN_ABSOLUTE_RMW( INC, Inc )
	},
	{	// EF
		LSN_ABSOLUTE_RMW( ISB, Isb )
	},

	/** F0-F7 */
	{	// F0
		LSN_BRANCH( BEQ, Z(), 1 )											// Branch if Z == 0.
	},
	{	// F1
		LSN_INDIRECT_Y_R( SBC, Sbc )
	},
	{	// F2
		{
			&CCpu6502::FetchOperandDecPc_Phi2,
			&CCpu6502::Jam, &CCpu6502::Jam_Phi2, },
		3, LSN_AM_IMPLIED, 2, LSN_I_JAM,
	},
	{	// F3
		LSN_INDIRECT_Y_RMW( ISB, Isb )
	},
	{	// F4
		LSN_ZERO_PAGE_X_R( NOP, Null_Read )
	},
	{	// F5
		LSN_ZERO_PAGE_X_R( SBC, Sbc )
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
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Sed, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_SED,
	},
	{	// F9
		LSN_ABSOLUTE_Y_R( SBC, Sbc )
	},
	{	// FA
		{
			&CCpu6502::FetchOperandAndDiscard_Phi2,
			&CCpu6502::Null_Read, &CCpu6502::PrefetchNextOp },
			2, LSN_AM_IMPLIED, 1, LSN_I_NOP,
	},
	{	// FB
		LSN_ABSOLUTE_Y_RMW( ISB, Isb )
	},
	{	// FC
		LSN_ABSOLUTE_X_R( NOP, Null_Read )
	},
	{	// FD
		LSN_ABSOLUTE_X_R( SBC, Sbc )
	},
	{	// FE
		LSN_ABSOLUTE_X_RMW( INC, Inc )
	},
	{	// FF
		LSN_ABSOLUTE_X_RMW( ISB, Isb )
	},
};

//
