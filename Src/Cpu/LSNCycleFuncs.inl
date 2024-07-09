#define LSN_INDIRECT_X_R( NAME, FUNC )													{ /* BeginInst() */ &CCpu6502::Fetch_Opcode_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Fetch_Operand_IncPc_Phi2, &CCpu6502::Null<LSN_R, true>, &CCpu6502::Read_Operand_Discard_Phi2, &CCpu6502::Add_XAndOperand_To_AddrOrPntr<LSN_TO_P, LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2<LSN_FROM_P>, &CCpu6502::Null<LSN_R>, &CCpu6502::Read_PtrOrAddr_To_Operand_Phi2<LSN_FROM_A>, &CCpu6502::FUNC ## _BeginInst }, 6, LSN_AM_INDIRECT_X, 2, LSN_I_ ## NAME


 
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
		LSN_INDIRECT_X_R( ORA, Ora )
	},
};

//
