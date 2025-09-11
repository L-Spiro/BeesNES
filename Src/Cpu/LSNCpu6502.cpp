/**
 * Copyright L. Spiro 2024
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
#include "LSNInstMetaData.inl"

#ifdef LSN_CPU_VERIFY
#include "EEExpEval.h"
#endif	// #ifdef LSN_CPU_VERIFY

namespace lsn {

	// == Members.
#include "LSNCycleFuncs.inl"

	CCpu6502::CCpu6502( CCpuBus * _pbBus ) :
		CCpuBase( _pbBus ) {

		m_pfTickFunc = m_pfTickFuncCopy = &CCpu6502::Tick_NextInstructionStd;
	}
	CCpu6502::~CCpu6502() {
		ResetToKnown();
	}

	// == Functions.
	/**
	 * Performs a single PHI1 update.
	 */
	void CCpu6502::Tick() {
		/*if ( m_ui64CycleCount >= 39873593 - 2 ) {
			volatile int uiuiu = 0;
		}*/
		m_bIrqStatusPhi1Flag = m_bIrqSeenLowPhi2;
		m_bIrqSeenLowPhi2 = false;

#ifndef LSN_CPU_VERIFY
		m_pmbMapper->Tick();
#endif	// #ifndef LSN_CPU_VERIFY
		(this->*m_pfTickFunc)();
	}

	/**
	 * Performs a single PHI2 update.
	 **/
	void CCpu6502::TickPhi2() {
		(this->*m_pfTickFunc)();

		m_bDetectedNmi |= (!m_bLastNmiStatusLine && m_bNmiStatusLine); m_bLastNmiStatusLine = m_bNmiStatusLine;
		//m_bIrqSeenLowPhi2 |= m_bIrqStatusLine & static_cast<int8_t>(!(m_fsState.rRegs.ui8Status & I()));
		m_bIrqSeenLowPhi2 |= (m_ui8IrqStatusLine != 0);

		++m_ui64CycleCount;
		/*if ( m_ui64CycleCount == (266551) ) {
			volatile int kjhkjh  =0;
		}*/
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
		for ( uint32_t I = 0x4000; I < 0x4015; ++I ) {
			m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
			m_pbBus->SetWriteFunc( uint16_t( I ), CCpuBus::NoWrite, this, uint16_t( I ) );
		}
		for ( uint32_t I = 0x4018; I < (LSN_MEM_FULL_SIZE); ++I ) {
			m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
			m_pbBus->SetWriteFunc( uint16_t( I ), CCpuBus::NoWrite, this, uint16_t( I ) );
		}

		// DMA transfer.
		m_pbBus->SetReadFunc( 0x4014, CCpuBus::NoRead, this, 0x4014 );
		m_pbBus->SetWriteFunc( 0x4014, CCpu6502::Write4014, this, 0x4014 );

		// Controller ports.
		m_pbBus->SetReadFunc( 0x4016, CCpu6502::Read4016, this, 0 );
		m_pbBus->SetWriteFunc( 0x4016, CCpu6502::Write4016, this, 0 );
		m_pbBus->SetReadFunc( 0x4017, CCpu6502::Read4017, this, 0 );
		m_pbBus->SetFloatMask( 0x4015, 0x00 );
	}

	/**
	 * Begins an OAM DMA transfer.
	 * 
	 * \param _ui8Val The value written to 0x4014.
	 */
	void CCpu6502::BeginOamDma( uint8_t _ui8Val ) {
		m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_IDLE, false>;
		m_ui16DmaAddress = uint16_t( _ui8Val ) << 8;
		m_bDmaGo = false;
		// Leave m_pfTickFuncCopy as-is to return to it after the transfer.
	}

	/**
	 * Begins a DMC DMA transfer.
	 */
	void CCpu6502::BeginDmcDma() {
	}

	/**
	 * Notifies the class that an NMI has occurred.
	 */
	void CCpu6502::Nmi() {
		m_bNmiStatusLine = true;
	}

	/**
	 * Clears the NMI flag.
	 */
	void CCpu6502::ClearNmi() {
		m_bNmiStatusLine = false;
	}

	/**
	 * Signals an IRQ to be handled before the next instruction.
	 *
	 * \param _ui8Source The source of the IRQ.
	 */
	void CCpu6502::Irq( uint8_t _ui8Source ) {
		m_ui8IrqStatusLine |= _ui8Source;

		/*if ( m_ui64CycleCount < 279340 + 42150 ) {
			char szBuffer[128];
			::sprintf_s( szBuffer, "IRQ Requested on Cycle: %u.\r\n", static_cast<uint32_t>(m_ui64CycleCount) );
			::OutputDebugStringA( szBuffer );
		}*/
	}

	/**
	 * Clears the IRQ flag.
	 *
	 * \param _ui8Source The source of the IRQ.
	 */
	void CCpu6502::ClearIrq( uint8_t _ui8Source ) {
		m_ui8IrqStatusLine &= ~_ui8Source;

		/*if ( m_ui64CycleCount < 279340 + 42150 ) {
			char szBuffer[128];
			::sprintf_s( szBuffer, "IRQ Cleared on Cycle: %u.\r\n", static_cast<uint32_t>(m_ui64CycleCount) );
			::OutputDebugStringA( szBuffer );
		}*/
	}

	/**
	 * Gets the status of the IRQ line.
	 *
	 * \param _ui8Source The source of the IRQ.
	 * \return Returns true if the IRQ status line is low.
	 **/
	bool CCpu6502::GetIrqStatus( uint8_t _ui8Source ) const {
		return (m_ui8IrqStatusLine & _ui8Source) != 0;
	}

#ifdef LSN_CPU_VERIFY
	/**
	 * Runs a test given a JSON's value representing the test to run.
	 *
	 * \param _jJson The JSON file.
	 * \param _jvTest The test to run.
	 * \return Returns true if te test succeeds, false otherwise.
	 */
	bool CCpu6502::RunJsonTest( lson::CJson &_jJson, const lson::CJsonContainer::LSON_JSON_VALUE &_jvTest ) {
		LSN_CPU_VERIFY_OBJ cvoVerifyMe;
		if ( !GetTest( _jJson, _jvTest, cvoVerifyMe ) ) { return false; }

		// Create the initial state.
		ResetToKnown();
		m_pbBus->ApplyMap();				// Set default read/write functions.
		m_ui64CycleCount = 0;
		m_fsState.rRegs.ui8A = cvoVerifyMe.cvsStart.cvrRegisters.ui8A;
		m_fsState.rRegs.ui8S = cvoVerifyMe.cvsStart.cvrRegisters.ui8S;
		m_fsState.rRegs.ui8X = cvoVerifyMe.cvsStart.cvrRegisters.ui8X;
		m_fsState.rRegs.ui8Y = cvoVerifyMe.cvsStart.cvrRegisters.ui8Y;
		m_fsState.rRegs.ui8Status = cvoVerifyMe.cvsStart.cvrRegisters.ui8Status;
		m_fsState.rRegs.ui16Pc = cvoVerifyMe.cvsStart.cvrRegisters.ui16Pc;

		for ( auto I = cvoVerifyMe.cvsStart.vRam.size(); I--; ) {
			m_pbBus->Write( cvoVerifyMe.cvsStart.vRam[I].ui16Addr, cvoVerifyMe.cvsStart.vRam[I].ui8Value );
		}
		m_pbBus->ApplyMap();				// Set default read/write functions.

		/*if ( "10 91 3e" == cvoVerifyMe.sName ) {
			volatile int ghg = 0;
		}*/
		//m_bIsReset = true;
		for ( auto I = cvoVerifyMe.vCycles.size(); I--; ) {
			Tick();
			/*if ( m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_JAM ) {
				if ( m_bHandleNmi != I < 1 ) {
					::OutputDebugStringA( "\r\nDouble-check polling.\r\n" );
				}
			}*/
			m_bDetectedNmi = true;
			TickPhi2();
			if ( m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_JAM && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BRK &&
				m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BPL && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BNE && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BVC && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BVS &&
				m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BCC && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BCS && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BEQ && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_BMI ) {
				if ( m_bHandleNmi != (I <= 0) ) {
					::OutputDebugStringA( "\r\nDouble-check polling.\r\n" );
				}
			}
		}
		Tick();
		
		// Verify.
#define LSN_VURIFFY( REG )																																												\
	if ( m_fsState.rRegs. ## REG != cvoVerifyMe.cvsEnd.cvrRegisters. ## REG ) {																																	\
		::OutputDebugStringA( cvoVerifyMe.sName.c_str() );																																				\
		::OutputDebugStringA( "\r\nCPU Failure: " # REG "\r\n" );																																		\
		::OutputDebugStringA( (std::string( "Expected: ") + std::to_string( cvoVerifyMe.cvsEnd.cvrRegisters. ## REG ) +																					\
			std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( cvoVerifyMe.cvsEnd.cvrRegisters. ## REG ) ) + std::string( ") Got: " ) + std::to_string( m_fsState.rRegs. ## REG ) +							\
			std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( m_fsState.rRegs. ## REG ) ) + std::string( ")" )).c_str() );																					\
		::OutputDebugStringA( "\r\n\r\n" );																																								\
	}

		LSN_VURIFFY( ui8A );
		LSN_VURIFFY( ui8X );
		LSN_VURIFFY( ui8Y );
		LSN_VURIFFY( ui8S );
		LSN_VURIFFY( ui8Status );
		LSN_VURIFFY( ui16Pc );
#undef LSN_VURIFFY

		if ( m_fsState.ui8FuncIndex != 0 && m_iInstructionSet[m_fsState.ui16OpCode].iInstruction != LSN_I_JAM ) {
			::OutputDebugStringA( cvoVerifyMe.sName.c_str() );
			::OutputDebugStringA( "\r\nDidn't read the end of cycle functions.\r\n" );
			::OutputDebugStringA( "\r\n\r\n" );
		}

		if ( m_pbBus->ReadWriteLog().size() != cvoVerifyMe.vCycles.size() ) {
			::OutputDebugStringA( cvoVerifyMe.sName.c_str() );
			::OutputDebugStringA( "\r\nInternal Error\r\n" );
			::OutputDebugStringA( "\r\n\r\n" );
		}
		else {
			//if ( m_pbBus->ReadWriteLog().size() != m_iInstructionSet[ui16LastInstr].
			for ( size_t I = 0; I < m_pbBus->ReadWriteLog().size(); ++I ) {
				if ( m_pbBus->ReadWriteLog()[I].ui16Address != cvoVerifyMe.vCycles[I].ui16Addr ) {
					::OutputDebugStringA( cvoVerifyMe.sName.c_str() );
					::OutputDebugStringA( "\r\nCPU Failure: Cycle Address Wrong\r\n" );
					::OutputDebugStringA( (std::string( "Expected: ") + std::to_string( cvoVerifyMe.vCycles[I].ui16Addr ) +
						std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( cvoVerifyMe.vCycles[I].ui16Addr ) ) + std::string( ") Got: " ) + std::to_string( m_pbBus->ReadWriteLog()[I].ui16Address ) +
						std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( m_pbBus->ReadWriteLog()[I].ui16Address ) ) + std::string( ")" ) ).c_str() );
					::OutputDebugStringA( "\r\n\r\n" );
				}
				if ( m_pbBus->ReadWriteLog()[I].ui8Value != cvoVerifyMe.vCycles[I].ui8Value ) {
					::OutputDebugStringA( cvoVerifyMe.sName.c_str() );
					::OutputDebugStringA( "\r\nCPU Failure: Cycle Value Wrong\r\n" );
					::OutputDebugStringA( (std::string( "Expected: ") + std::to_string( cvoVerifyMe.vCycles[I].ui8Value ) +
						std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( cvoVerifyMe.vCycles[I].ui8Value ) ) + std::string( ") Got: " ) + std::to_string( m_pbBus->ReadWriteLog()[I].ui8Value ) +
						std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( m_pbBus->ReadWriteLog()[I].ui8Value ) ) + std::string( ")" ) ).c_str() );
					::OutputDebugStringA( "\r\n\r\n" );
				}
				if ( m_pbBus->ReadWriteLog()[I].bRead != cvoVerifyMe.vCycles[I].bRead ) {
					::OutputDebugStringA( cvoVerifyMe.sName.c_str() );
					::OutputDebugStringA( "\r\nCPU Failure: Cycle Read/Write Wrong\r\n" );
					::OutputDebugStringA( (std::string( "Expected: ") + std::to_string( cvoVerifyMe.vCycles[I].bRead ) + std::string( " Got: " ) + std::to_string( m_pbBus->ReadWriteLog()[I].bRead ) ).c_str() );
					::OutputDebugStringA( "\r\n\r\n" );
				}
			}
		}
		return true;
	}
#endif	// #ifdef LSN_CPU_VERIFY

#ifdef LSN_CPU_VERIFY
	/**
	 * Given a JSON object and the value for the test to run, this loads the test and fills a LSN_CPU_VERIFY structure.
	 *
	 * \param _jJson The JSON file.
	 * \param _jvTest The test to run.
	 * \param _cvoTest The test structure to fill out.
	 * \return Returns true if the JSON data was successfully extracted and the test created.
	 */
	bool CCpu6502::GetTest( lson::CJson &_jJson, const lson::CJsonContainer::LSON_JSON_VALUE &_jvTest, LSN_CPU_VERIFY_OBJ &_cvoTest ) {
		const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal;
		// The name.
		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvTest, "name" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_STRING ) {
			_cvoTest.sName = _jJson.GetContainer()->GetString( pjvVal->u.stString );
		}
		else { return false; }

		// The initial state.
		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvTest, "initial" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_OBJECT ) {
			if ( !LoadState( _jJson, (*pjvVal), _cvoTest.cvsStart ) ) { return false; }
		}
		else { return false; }

		// The final state.
		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvTest, "final" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_OBJECT ) {
			if ( !LoadState( _jJson, (*pjvVal), _cvoTest.cvsEnd ) ) { return false; }
		}
		else { return false; }

		// The cycles.
		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvTest, "cycles" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_ARRAY ) {
			for ( size_t I = 0; I < pjvVal->vArray.size(); ++I ) {
				const lson::CJsonContainer::LSON_JSON_VALUE & jvThis = _jJson.GetContainer()->GetValue( pjvVal->vArray[I] );
				if ( jvThis.vtType == lson::CJsonContainer::LSON_VT_ARRAY && jvThis.vArray.size() == 3 ) {
					LSN_CPU_VERIFY_CYCLE cvcCycle = {
						.ui16Addr = uint16_t( _jJson.GetContainer()->GetValue( jvThis.vArray[0] ).u.dDecimal ),
						.ui8Value = uint8_t( _jJson.GetContainer()->GetValue( jvThis.vArray[1] ).u.dDecimal ),
						.bRead = _jJson.GetContainer()->GetString( _jJson.GetContainer()->GetValue( jvThis.vArray[2] ).u.stString ) == "read",
					};
					_cvoTest.vCycles.push_back( cvcCycle );
				}
				else { return false; }
			}
		}
		else { return false; }
		return true;
	}

	/**
	 * Fills out a LSN_CPU_VERIFY_STATE structure given a JSON "initial" or "final" member.
	 *
	 * \param _jJson The JSON file.
	 * \param _jvState The bject member representing the state to load.
	 * \param _cvsState The state structure to fill.
	 * \return Returns true if the state was loaded.
	 */
	bool CCpu6502::LoadState( lson::CJson &_jJson, const lson::CJsonContainer::LSON_JSON_VALUE &_jvState, LSN_CPU_VERIFY_STATE &_cvsState ) {
		const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal;

		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvState, "pc" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
			_cvsState.cvrRegisters.ui16Pc = uint16_t( pjvVal->u.dDecimal );
		}
		else { return false; }

		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvState, "s" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
			_cvsState.cvrRegisters.ui8S = uint8_t( pjvVal->u.dDecimal );
		}
		else { return false; }

		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvState, "a" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
			_cvsState.cvrRegisters.ui8A = uint8_t( pjvVal->u.dDecimal );
		}
		else { return false; }

		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvState, "x" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
			_cvsState.cvrRegisters.ui8X = uint8_t( pjvVal->u.dDecimal );
		}
		else { return false; }

		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvState, "y" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
			_cvsState.cvrRegisters.ui8Y = uint8_t( pjvVal->u.dDecimal );
		}
		else { return false; }

		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvState, "p" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
			_cvsState.cvrRegisters.ui8Status = uint8_t( pjvVal->u.dDecimal );
		}
		else { return false; }


		pjvVal = _jJson.GetContainer()->GetMemberByName( _jvState, "ram" );
		if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_ARRAY ) {
			for ( size_t I = 0; I < pjvVal->vArray.size(); ++I ) {
				const lson::CJsonContainer::LSON_JSON_VALUE & jvThis = _jJson.GetContainer()->GetValue( pjvVal->vArray[I] );
				if ( jvThis.vtType == lson::CJsonContainer::LSON_VT_ARRAY && jvThis.vArray.size() == 2 ) {
					LSN_CPU_VERIFY_RAM cvrRam = {
						.ui16Addr = uint16_t( _jJson.GetContainer()->GetValue( jvThis.vArray[0] ).u.dDecimal ),
						.ui8Value = uint8_t( _jJson.GetContainer()->GetValue( jvThis.vArray[1] ).u.dDecimal ),
					};
					_cvsState.vRam.push_back( cvrRam );
				}
				else { return false; }
			}
		}
		else { return false; }

		return true;
	}
#endif	// #ifdef LSN_CPU_VERIFY



	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CYCLE FUNCTIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Performs an add-with-carry with an operand, setting flags C, N, V, and Z. */
	template <bool _bIncPc>
	void CCpu6502::Adc_BeginInst() {
		BeginInst<_bIncPc>();

		Adc( m_fsState.rRegs.ui8A, m_fsState.ui8Operand );
	}
	
	/** Adds X and m_fsState.ui8Operand, stores to either m_fsState.ui16Address or m_fsState.ui16Pointer. */
	template <bool _bToAddr, bool _bRead, bool _bIncPc>
	void CCpu6502::Add_XAndOperand_To_AddrOrPntr_8bit() {
		LSN_INSTR_START_PHI1( _bRead );

		if constexpr ( _bIncPc ) {
			LSN_UPDATE_PC;
		}

		if constexpr ( _bToAddr ) {
			m_fsState.ui16Address = uint8_t( m_fsState.ui8Operand + m_fsState.rRegs.ui8X );
		}
		else {
			m_fsState.ui16Pointer = uint8_t( m_fsState.ui8Operand + m_fsState.rRegs.ui8X );
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds X and m_fsState.ui16Pointer or m_fsState.ui16Address, stores to either m_fsState.ui16Address or m_fsState.ui16Pointer. */
	template <bool _bToAddr, bool _bRead, bool _bIncPc>
	void CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPntr_8bit() {
		LSN_INSTR_START_PHI1( _bRead );

		if constexpr ( _bIncPc ) {
			LSN_UPDATE_PC;
		}

		if constexpr ( _bToAddr ) {
			m_fsState.ui16Address = uint8_t( m_fsState.ui16Pointer + m_fsState.rRegs.ui8X );
		}
		else {
			m_fsState.ui16Pointer = uint8_t( m_fsState.ui16Address + m_fsState.rRegs.ui8X );
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds X and either m_fsState.ui16Address.L or m_fsState.ui16Pointer.L, stores in either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr, bool _bRead, bool _bIncPc>
	void CCpu6502::Add_XAndPtrOrAddr_To_AddrOrPtr() {
		LSN_INSTR_START_PHI1( _bRead );

		if constexpr ( _bIncPc ) {
			LSN_UPDATE_PC;
		}

		if constexpr ( _bToAddr ) {
			m_fsState.ui16Target = m_fsState.ui16Pointer + m_fsState.rRegs.ui8X;
			m_fsState.ui8Address[0] = m_fsState.ui8Target[0];
			m_fsState.ui8Address[1] = m_fsState.ui8Pointer[1];

			m_fsState.bBoundaryCrossed = m_fsState.ui8Pointer[1] != m_fsState.ui8Target[1];
		}
		else {
			m_fsState.ui16Target = m_fsState.ui16Address + m_fsState.rRegs.ui8X;
			m_fsState.ui8Pointer[0] = m_fsState.ui8Target[0];
			m_fsState.ui8Pointer[1] = m_fsState.ui8Address[1];

			m_fsState.bBoundaryCrossed = m_fsState.ui8Address[1] != m_fsState.ui8Target[1];
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds Y and either m_fsState.ui16Address.L or m_fsState.ui16Pointer.L, stores in either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr, bool _bRead, bool _bIncPc>
	void CCpu6502::Add_YAndPtrOrAddr_To_AddrOrPtr() {
		LSN_INSTR_START_PHI1( _bRead );

		if constexpr ( _bIncPc ) {
			LSN_UPDATE_PC;
		}

		if constexpr ( _bToAddr ) {
			m_fsState.ui16Target = m_fsState.ui16Pointer + m_fsState.rRegs.ui8Y;
			m_fsState.ui8Address[0] = m_fsState.ui8Target[0];
			m_fsState.ui8Address[1] = m_fsState.ui8Pointer[1];

			m_fsState.bBoundaryCrossed = m_fsState.ui8Pointer[1] != m_fsState.ui8Target[1];
		}
		else {
			m_fsState.ui16Target = m_fsState.ui16Address + m_fsState.rRegs.ui8Y;
			m_fsState.ui8Pointer[0] = m_fsState.ui8Target[0];
			m_fsState.ui8Pointer[1] = m_fsState.ui8Address[1];

			m_fsState.bBoundaryCrossed = m_fsState.ui8Address[1] != m_fsState.ui8Target[1];
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = A & OP.  Sets flags C, N, and Z, increases PC. */
	void CCpu6502::Anc_IncPc_BeginInst() {
		BeginInst<true>();

		m_fsState.rRegs.ui8A &= m_fsState.ui8Operand;

		SetBit<C() | N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs A = A & OP.  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::And_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8A &= m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs A = (A | CONST) & X & OP.  Sets flags N and Z. */
	void CCpu6502::Ane_IncPc_BeginInst() {
		BeginInst<true>();

		// "N and Z are set according to the value of the accumulator before the instruction executed" does not seem to be true.
		m_fsState.rRegs.ui8A = (m_fsState.rRegs.ui8A | 0xEE) & m_fsState.rRegs.ui8X & m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs A = A & OP; A = (A >> 1) | (C << 7).  Sets flags C, V, N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Arr_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8A &= m_fsState.ui8Operand;
		uint8_t ui8HiBit = (m_fsState.rRegs.ui8Status & C()) << 7;
		// It carries if the last bit gets shifted in.
		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		m_fsState.rRegs.ui8A = (m_fsState.rRegs.ui8A >> 1) | ui8HiBit;
		SetBit<Z()>( m_fsState.rRegs.ui8Status, m_fsState.rRegs.ui8A == 0x00 );
		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<V()>( m_fsState.rRegs.ui8Status,
			((m_fsState.rRegs.ui8Status & C()) ^ ((m_fsState.rRegs.ui8A >> 5) & 0x1)) != 0 );
	}

	/** Performs M <<= 1.  Sets C, N, and V. */
	void CCpu6502::Asl() {
		LSN_INSTR_START_PHI1( false );

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );

		m_fsState.ui8Operand <<= 1;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A <<= 1.  Sets C, N, and V. */
	void CCpu6502::AslOnA_BeginInst() {
		BeginInst();

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );

		m_fsState.rRegs.ui8A <<= 1;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs A &= OP; A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::Asr_IncPc_BeginInst() {
		BeginInst<true>();

		m_fsState.rRegs.ui8A &= m_fsState.ui8Operand;
		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x01) != 0 );

		m_fsState.rRegs.ui8A >>= 1;

		SetBit<N(), false>( m_fsState.rRegs.ui8Status );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );

		
	}

	/** Sets flags N, V and Z according to a bit test. */
	void CCpu6502::Bit_BeginInst() {
		BeginInst();

		SetBit<V()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & (1 << 6)) != 0x00 );
		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & (1 << 7)) != 0x00 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !(m_fsState.ui8Operand & m_fsState.rRegs.ui8A) );
	}

	/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	template <unsigned _uBit, unsigned _uVal>
	void CCpu6502::Branch_Cycle1() {
		LSN_INSTR_START_PHI1( true );

		m_fsState.bTakeJump = (m_fsState.rRegs.ui8Status & _uBit) == (_uVal * _uBit);
		LSN_UPDATE_PC;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** 1st cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	void CCpu6502::Branch_Cycle1_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Op );
		m_fsState.ui8Operand = ui8Op;

		m_fsState.ui16PcModify = 1;

		if ( !m_fsState.bTakeJump ) {
			LSN_FINISH_INST( true );
		}
		else {
			m_fsState.rRegs.ui16Pc += m_fsState.ui16PcModify;
			m_fsState.ui16PcModify = 0;
			m_fsState.ui16Address = static_cast<int16_t>(static_cast<int8_t>(m_fsState.ui8Operand)) + (m_fsState.rRegs.ui16Pc);

			m_fsState.bBoundaryCrossed = m_fsState.ui8Address[1] != m_fsState.rRegs.ui8Pc[1];
			if ( !m_fsState.bBoundaryCrossed ) {
				LSN_CHECK_INTERRUPTS;
			}

			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	void CCpu6502::Branch_Cycle2() {
		if ( !m_fsState.bTakeJump ) {
			BeginInst<true>();
		}
		else {
			LSN_INSTR_START_PHI1( true );

			LSN_UPDATE_PC;

			LSN_NEXT_FUNCTION;

			LSN_INSTR_END_PHI1;
		}
	}

	/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	void CCpu6502::Branch_Cycle2_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Tmp );

		if ( !m_fsState.bBoundaryCrossed ) {
			LSN_FINISH_INST( false );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary. */
	void CCpu6502::Branch_Cycle3() {
		if ( !m_fsState.bBoundaryCrossed ) {
			BeginInst();

			m_fsState.rRegs.ui8Pc[0] = m_fsState.ui8Address[0];
		}
		else {
			LSN_INSTR_START_PHI1( true );

			m_fsState.rRegs.ui8Pc[0] = m_fsState.ui8Address[0];
			
			LSN_NEXT_FUNCTION;

			LSN_INSTR_END_PHI1;
		}
	}

	/** 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary. */
	void CCpu6502::Branch_Cycle3_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Tmp );

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** 4th cycle of branch instructions. Page boundary was crossed. */
	void CCpu6502::Branch_Cycle4() {
		BeginInst();

		m_fsState.rRegs.ui8Pc[1] = m_fsState.ui8Address[1];
	}
		
	/** Final touches to BRK (copies m_fsState.ui16Address to m_fsState.rRegs.ui16Pc) and first cycle of the next instruction. */
	void CCpu6502::Brk_BeginInst() {
		BeginInst();
		
		m_bBrkIsReset = false;
		m_fsState.rRegs.ui16Pc = m_fsState.ui16Address;
	}

	/** Clears the carry bit. */
	void CCpu6502::Clc_BeginInst() {
		BeginInst();

		SetBit<C(), false>( m_fsState.rRegs.ui8Status );
	}

	/** Clears the decimal flag. */
	void CCpu6502::Cld_BeginInst() {
		BeginInst();

		SetBit<D(), false>( m_fsState.rRegs.ui8Status );
	}

	/** Clears the IRQ flag. */
	void CCpu6502::Cli_BeginInst() {
		BeginInst();

		SetBit<I(), false>( m_fsState.rRegs.ui8Status );
	}

	/** Clears the overflow flag. */
	void CCpu6502::Clv_BeginInst() {
		BeginInst();

		SetBit<V(), false>( m_fsState.rRegs.ui8Status );
	}

	/** Compares A with OP. */
	template <bool _bIncPc>
	void CCpu6502::Cmp_BeginInst() {
		BeginInst<_bIncPc>();

		Cmp( m_fsState.rRegs.ui8A, m_fsState.ui8Operand );
	}

	/** Compares X with OP. */
	template <bool _bIncPc>
	void CCpu6502::Cpx_BeginInst() {
		BeginInst<_bIncPc>();

		Cmp( m_fsState.rRegs.ui8X, m_fsState.ui8Operand );
	}

	/** Compares Y with OP. */
	template <bool _bIncPc>
	void CCpu6502::Cpy_BeginInst() {
		BeginInst<_bIncPc>();

		Cmp( m_fsState.rRegs.ui8Y, m_fsState.ui8Operand );
	}

	/** Copies m_fsState.ui8Operand to Status without the B bit. */
	void CCpu6502::CopyOperandToStatusWithoutB() {
		LSN_INSTR_START_PHI1( true );

		constexpr uint8_t ui8Mask = M() | X();
		m_fsState.rRegs.ui8Status = (m_fsState.ui8Operand & ~ui8Mask) | (m_fsState.rRegs.ui8Status & ui8Mask);

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies m_fsState.ui16Target to PC, optionally adjusts PC. */
	template <bool _bIncPc>
	void CCpu6502::CopyTargetToPc() {
		LSN_INSTR_START_PHI1( true );

		m_fsState.rRegs.ui16Pc = m_fsState.ui16Target;

		if constexpr ( _bIncPc ) {
			LSN_UPDATE_PC;
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies from the vector to PC.h. */
	template <bool _bEndInstr>
	void CCpu6502::CopyVectorToPc_H_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( m_fsState.vBrkVector + 1, ui8Tmp );
		m_fsState.ui8Address[1] = ui8Tmp;

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}
			
	/** Copies from the vector to PC.l. */
	void CCpu6502::CopyVectorToPc_L_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( m_fsState.vBrkVector, ui8Tmp );
		m_fsState.ui8Address[0] = ui8Tmp;


		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Performs [ADDR]--; CMP(A).  Sets flags C, N, and Z. */
	void CCpu6502::Dcp() {
		LSN_INSTR_START_PHI1( false );

		--m_fsState.ui8Operand;
		Cmp( m_fsState.rRegs.ui8A, m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs [ADDR]--.  Sets flags N and Z. */
	void CCpu6502::Dec() {
		LSN_INSTR_START_PHI1( false );

		--m_fsState.ui8Operand;
		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs X--.  Sets flags N and Z. */
	void CCpu6502::Dex_BeginInst() {
		BeginInst();

		--m_fsState.rRegs.ui8X;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8X );
	}

	/** Performs Y--.  Sets flags N and Z. */
	void CCpu6502::Dey_BeginInst() {
		BeginInst();

		--m_fsState.rRegs.ui8Y;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8Y );
	}

	/** Performs A = A ^ OP.  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Eor_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8A ^= m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Fetches the current opcode and increments PC. */
	void CCpu6502::Fetch_Opcode_IncPc_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Op );

#ifdef LSN_CPU_VERIFY
		m_fsState.ui16PcModify = 1;
#else
		if ( m_bHandleNmi || m_bHandleIrq || m_bIsReset ) {
			ui8Op = 0;
			m_fsState.bPushB = false;
			m_fsState.ui16PcModify = 0;
			m_fsState.bAllowWritingToPc = false;
		}
		else {
			m_fsState.bPushB = true;
			m_fsState.ui16PcModify = 1;
		}
#endif	// #ifdef LSN_CPU_VERIFY
		m_fsState.ui16OpCode = ui8Op;
		m_fsState.pfCurInstruction = m_iInstructionSet[m_fsState.ui16OpCode].pfHandler;

#if 0
		char szBUffer[256];
		std::sprintf( szBUffer, "%s\t%.2X %lu %.4X\r\n", m_smdInstMetaData[m_iInstructionSet[m_fsState.ui16OpCode].iInstruction].pcName, m_fsState.ui16OpCode, m_ui64CycleCount, m_fsState.rRegs.ui16Pc );
		::OutputDebugStringA( szBUffer );
		if ( 0xB6FE == m_fsState.rRegs.ui16Pc ) {
			volatile int gjhgh =0;
		}
#endif	// #if 1

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the operand. */
	template <bool _bEndInstr>
	void CCpu6502::Fetch_Operand_Discard_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Op );

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the operand and increments PC. */
	template <bool _bEndInstr>
	void CCpu6502::Fetch_Operand_IncPc_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Op );
		m_fsState.ui8Operand = ui8Op;

		m_fsState.ui16PcModify = 1;

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the operand to either m_fsState.ui16Address.H or m_fsState.ui16Pointer.H and increments PC. */
	template <bool _bToAddr, bool _bEndInstr>
	void CCpu6502::Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2() {
		uint8_t ui8Tmp;
		if constexpr ( _bToAddr ) {
			// PC -> Address.
			LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Tmp );
			m_fsState.ui8Address[1] = ui8Tmp;
		}
		else {
			// PC -> Pointer.
			LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Tmp );
			m_fsState.ui8Pointer[1] = ui8Tmp;
		}

		m_fsState.ui16PcModify = 1;
		
		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the operand to either m_fsState.ui16Address or m_fsState.ui16Pointer and increments PC. */
	template <bool _bToAddr>
	void CCpu6502::Fetch_Operand_To_AddrOrPtr_IncPc_Phi2() {
		uint8_t ui8Tmp;
		if constexpr ( _bToAddr ) {
			// PC -> Address.
			LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Tmp );
			m_fsState.ui16Address = ui8Tmp;
		}
		else {
			// PC -> Pointer.
			LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc, ui8Tmp );
			m_fsState.ui16Pointer = ui8Tmp;
		}

		m_fsState.ui16PcModify = 1;
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Uses m_fsState.ui16Target to fix the high byte of either m_fsState.ui16Address or m_fsState.ui16Pointer. */
	template <bool _bFromAddr>
	void CCpu6502::Fix_PtrOrAddr_To_AddrOrPtr_H() {
		LSN_INSTR_START_PHI1( true );

		if constexpr ( _bFromAddr ) {
			m_fsState.ui8Pointer[1] = m_fsState.ui8Target[1];
		}
		else {
			m_fsState.ui8Address[1] = m_fsState.ui8Target[1];
		}		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs the Indirect Y add on the low byte.  m_fsState.ui16Address -> m_fsState.ui16Pointer or m_fsState.ui16Pointer -> m_fsState.ui16Address. */
	template <bool _bFromAddr>
	void CCpu6502::IndirectYAdd_PtrOrAddr_To_AddrOrPtr() {
		LSN_INSTR_START_PHI1( true );

		if constexpr ( _bFromAddr ) {
			m_fsState.ui16Target = m_fsState.ui16Address + m_fsState.rRegs.ui8Y;
			m_fsState.ui8Pointer[0] = m_fsState.ui8Target[0];
			m_fsState.ui8Pointer[1] = m_fsState.ui8Address[1];
			m_fsState.bBoundaryCrossed = m_fsState.ui8Pointer[1] != m_fsState.ui8Target[1];
		}
		else {
			m_fsState.ui16Target = m_fsState.ui16Pointer + m_fsState.rRegs.ui8Y;
			m_fsState.ui8Address[0] = m_fsState.ui8Target[0];
			m_fsState.ui8Address[1] = m_fsState.ui8Pointer[1];
			m_fsState.bBoundaryCrossed = m_fsState.ui8Address[1] != m_fsState.ui8Target[1];
		}		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP++.  Sets flags N and Z. */
	void CCpu6502::Inc() {
		LSN_INSTR_START_PHI1( false );

		++m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.ui8Operand );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs X++.  Sets flags N and Z. */
	void CCpu6502::Inx_BeginInst() {
		BeginInst();

		++m_fsState.rRegs.ui8X;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8X );
	}

	/** Performs Y++.  Sets flags N and Z. */
	void CCpu6502::Iny_BeginInst() {
		BeginInst();

		++m_fsState.rRegs.ui8Y;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8Y );
	}

	/** Performs M++; SBC.  Sets flags C, N, V, and Z. */
	void CCpu6502::Isb() {
		LSN_INSTR_START_PHI1( false );

		++m_fsState.ui8Operand;
		Sbc( m_fsState.rRegs.ui8A, m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Jams the machine. */
	void CCpu6502::Jam() {
		LSN_INSTR_START_PHI1( true );

		if ( m_fsState.bAllowWritingToPc ) {
			m_fsState.rRegs.ui16Pc += uint16_t( -int16_t( m_fsState.ui16PcModify ) );
		}
		m_fsState.ui16PcModify = 0;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Jams the machine. */
	void CCpu6502::Jam_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_fsState.rRegs.ui16Pc + 1, ui8Op );

		LSN_NEXT_FUNCTION_BY( -1 );

		LSN_INSTR_END_PHI2;
	}

	/** Copies m_fsState.ui16Address into PC. */
	void CCpu6502::Jmp_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui16Pc = m_fsState.ui16Address;
		m_fsState.ui16PcModify = 0;
	}

	/** Copies m_fsState.ui16Address into PC, adjusts S. */
	void CCpu6502::Jsr_BeginInst() {
		BeginInst<false, true>();

		m_fsState.rRegs.ui16Pc = m_fsState.ui16Address;
		m_fsState.ui16PcModify = 0;
	}

	/** Performs A = X = S = (OP & S).  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Las_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8A = m_fsState.rRegs.ui8X = m_fsState.rRegs.ui8S = (m_fsState.ui8Operand & m_fsState.rRegs.ui8S);

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs A = X = OP.  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Lax_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8A = m_fsState.rRegs.ui8X = m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs A = OP.  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Lda_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8A = m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs X = OP.  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Ldx_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8X = m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8X );
	}

	/** Performs Y = OP.  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Ldy_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8Y = m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8Y );
	}

	/** Performs OP >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::Lsr() {
		LSN_INSTR_START_PHI1( false );

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x01) != 0 );
		m_fsState.ui8Operand >>= 1;

		SetBit<N(), false>( m_fsState.rRegs.ui8Status );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::LsrOnA_BeginInst() {
		BeginInst();

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x01) != 0 );
		m_fsState.rRegs.ui8A >>= 1;

		SetBit<N(), false>( m_fsState.rRegs.ui8Status );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs A = X = (A | CONST) & OP.  Sets flags N and Z. */
	void CCpu6502::Lxa_IncPc_BeginInst() {
		BeginInst<true>();

		LSN_UPDATE_PC;

#ifdef LSN_CPU_VERIFY
		m_fsState.rRegs.ui8A = m_fsState.rRegs.ui8X = (m_fsState.rRegs.ui8A | 0xEE) & m_fsState.ui8Operand;
#else
		m_fsState.rRegs.ui8A = m_fsState.rRegs.ui8X = (m_fsState.rRegs.ui8A | 0xFF) & m_fsState.ui8Operand;
#endif	// #ifdef LSN_CPU_VERIFY

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Generic null operation. */
	template <bool _bRead, bool _bIncPc, bool _bAdjS, bool _bBeginInstr>
	void CCpu6502::Null() {
		if constexpr ( _bBeginInstr ) {
			BeginInst<_bIncPc, _bAdjS>();
		}
		else {
			LSN_INSTR_START_PHI1( _bRead );

			if constexpr ( _bIncPc ) {
				LSN_UPDATE_PC;
			}
			if constexpr ( _bAdjS ) {
				LSN_UPDATE_S;
			}

			LSN_NEXT_FUNCTION;

			LSN_INSTR_END_PHI1;
		}
	}

	/** Generic null operation for BRK that can be either a read or write, depending on RESET. */
	template <bool _bIncPc, bool _bAdjS, bool _bBeginInstr>
	void CCpu6502::Null_RorW() {
		if constexpr ( _bBeginInstr ) {
			BeginInst<_bIncPc, _bAdjS>();
		}
		else {
			if LSN_UNLIKELY( m_bIsReset ) {
				LSN_INSTR_START_PHI1( true );
			}
			else {
				LSN_INSTR_START_PHI1( false );
			}

			if constexpr ( _bIncPc ) {
				LSN_UPDATE_PC;
			}
			if constexpr ( _bAdjS ) {
				LSN_UPDATE_S;
			}

			LSN_NEXT_FUNCTION;

			LSN_INSTR_END_PHI1;
		}
	}

	/** Performs A |= Operand with m_fsState.ui8Operand.  Sets flags N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Ora_BeginInst() {
		BeginInst<_bIncPc>();

		m_fsState.rRegs.ui8A |= m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Sets m_fsState.ui8Operand to the status byte with Break and Reserved set. */
	void CCpu6502::Php() {
		LSN_INSTR_START_PHI1( false );

		m_fsState.ui8Operand = m_fsState.rRegs.ui8Status;
		SetBit<X() | M(), true>( m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Pulls the accumulator: Copies m_fsState.ui8Operand to A. */
	void CCpu6502::Pla_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8A = m_fsState.ui8Operand;
		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Pulls the status byte, unsets X, sets M. */
	void CCpu6502::Plp_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8Status = (m_fsState.ui8Operand & ~X()) | M();
	}

	/** Pulls from the stack, stores in A. */
	template <int8_t _i8SOff>
	void CCpu6502::Pull_To_A_Phi2() {
		LSN_POP( m_fsState.rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pulls from the stack, stores in m_fsState.ui8Operand. */
	template <int8_t _i8SOff>
	void CCpu6502::Pull_To_Operand_Phi2() {
		LSN_POP( m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pulls from the stack, stores in m_fsState.ui16Target.H. */
	template <int8_t _i8SOff, bool _bEndInstr>
	void CCpu6502::Pull_To_Target_H_Phi2() {
		LSN_POP( m_fsState.ui8Target[0] );

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Pulls from the stack, stores in m_fsState.ui16Target.L. */
	template <int8_t _i8SOff>
	void CCpu6502::Pull_To_Target_L_Phi2() {
		LSN_POP( m_fsState.ui8Target[0] );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes A. */
	template <int8_t _i8SOff, bool _bEndInstr>
	void CCpu6502::Push_A_Phi2() {
		LSN_PUSH( m_fsState.rRegs.ui8A );

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Pushes m_fsState.ui8Operand. */
	template <int8_t _i8SOff, bool _bEndInstr>
	void CCpu6502::Push_Operand_Phi2() {
		LSN_PUSH( m_fsState.ui8Operand );

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Pushes PCh with the given S offset. */
	template <int8_t _i8SOff>
	void CCpu6502::Push_Pc_H_Phi2() {
		if LSN_UNLIKELY( m_bBrkIsReset ) {
			uint8_t ui8Tmp;
			LSN_INSTR_START_PHI2_READ( (0x100 | uint8_t( m_fsState.rRegs.ui8S + _i8SOff )), ui8Tmp );
			m_fsState.ui8SModify = uint8_t( -1L + _i8SOff );
		}
		else {
			LSN_PUSH( m_fsState.rRegs.ui8Pc[1] );
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes PCl with the given S offset. */
	template <int8_t _i8SOff>
	void CCpu6502::Push_Pc_L_Phi2() {
		if LSN_UNLIKELY( m_bBrkIsReset ) {
			uint8_t ui8Tmp;
			LSN_INSTR_START_PHI2_READ( (0x100 | uint8_t( m_fsState.rRegs.ui8S + _i8SOff )), ui8Tmp );
			m_fsState.ui8SModify = uint8_t( -1L + _i8SOff );
		}
		else {
			LSN_PUSH( m_fsState.rRegs.ui8Pc[0] );
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes Status with or without B/X to the given S offset. */
	template <int8_t _i8SOff>
	void CCpu6502::Push_S_Phi2() {
		if LSN_UNLIKELY( m_bBrkIsReset ) {
			uint8_t ui8Tmp;
			LSN_INSTR_START_PHI2_READ( (0x100 | uint8_t( m_fsState.rRegs.ui8S + _i8SOff )), ui8Tmp );
			m_fsState.ui8SModify = uint8_t( -1L + _i8SOff );
		}
		else {
			if ( m_fsState.bPushB ) {
				LSN_PUSH( m_fsState.rRegs.ui8Status | X() );
			}
			else {
				LSN_PUSH( m_fsState.rRegs.ui8Status );
			}
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads a given hard-coded address, optionally moving to the next cycle or the previous cycle. */
	template <uint16_t _ui16Addr, bool _bMoveBack>
	void CCpu6502::ReadAddr_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( _ui16Addr, ui8Tmp );

		if constexpr ( _bMoveBack ) {
			LSN_NEXT_FUNCTION_BY( -1 );
		}
		else {
			LSN_NEXT_FUNCTION;
		}
		

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_fsState.ui8Operand, discards result. */
	void CCpu6502::Read_Operand_Discard_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( m_fsState.ui8Operand, ui8Tmp );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from either m_fsState.ui16Pointer or m_fsState.ui16Address and stores the low byte in either m_fsState.ui8Address[1] or m_fsState.ui8Pointer[1]. */
	template <bool _bFromAddr, bool _bEndInstr>
	void CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_SamePage_Phi2() {
		uint8_t ui8Tmp;
		if constexpr ( !_bFromAddr ) {
			// Pointer -> Address.
			LSN_INSTR_START_PHI2_READ( (m_fsState.ui8Pointer[1] << 8) | uint8_t( m_fsState.ui8Pointer[0] + 1 ), ui8Tmp );
			m_fsState.ui8Address[1] = ui8Tmp;
		}
		else {
			// Address -> Pointer.
			LSN_INSTR_START_PHI2_READ( (m_fsState.ui8Address[1] << 8) | uint8_t( m_fsState.ui8Address[0] + 1 ), ui8Tmp );
			m_fsState.ui8Pointer[1] = ui8Tmp;
		}
		
		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Reads either m_fsState.ui16Pointer or m_fsState.ui16Address and stores in either m_fsState.ui16Address.H or m_fsState.ui16Pointer.H. */
	template <bool _bFromAddr>
	void CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2() {
		uint8_t ui8Tmp;
		if constexpr ( !_bFromAddr ) {
			// Pointer -> Address.
			LSN_INSTR_START_PHI2_READ( uint8_t( m_fsState.ui16Pointer + 1 ), ui8Tmp );
			m_fsState.ui8Address[1] = ui8Tmp;
		}
		else {
			// Address -> Pointer.
			LSN_INSTR_START_PHI2_READ( uint8_t( m_fsState.ui16Address + 1 ), ui8Tmp );
			m_fsState.ui8Pointer[1] = ui8Tmp;
		}
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads either m_fsState.ui16Pointer or m_fsState.ui16Address and stores in either m_fsState.ui16Address.L or m_fsState.ui16Pointer.L. */
	template <bool _bFromAddr>
	void CCpu6502::Read_PtrOrAddr_To_AddrOrPtr_L_Phi2() {
		uint8_t ui8Tmp;
		if constexpr ( !_bFromAddr ) {
			// Pointer -> Address.
			LSN_INSTR_START_PHI2_READ( m_fsState.ui16Pointer, ui8Tmp );
			m_fsState.ui16Address = ui8Tmp;
		}
		else {
			// Address -> Pointer.
			LSN_INSTR_START_PHI2_READ( m_fsState.ui16Address, ui8Tmp );
			m_fsState.ui16Pointer = ui8Tmp;
		}
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads either m_fsState.ui16Pointer or m_fsState.ui16Address and stores in m_fsState.ui8Operand. */
	template <bool _bFromAddr, bool _bEndInstr>
	void CCpu6502::Read_PtrOrAddr_To_Operand_Phi2() {
		uint8_t ui8Tmp;
		if constexpr ( _bFromAddr ) {
			LSN_INSTR_START_PHI2_READ( m_fsState.ui16Address, ui8Tmp );
		}
		else {
			LSN_INSTR_START_PHI2_READ( m_fsState.ui16Pointer, ui8Tmp );
		}
		m_fsState.ui8Operand = ui8Tmp;

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Reads either m_fsState.ui16Pointer or m_fsState.ui16Address and stores in m_fsState.ui8Operand.  Skips a full cycle if m_fsState.bBoundaryCrossed is false (and only then is _bEndInstr checked). */
	template <bool _bFromAddr, bool _bEndInstr>
	void CCpu6502::Read_PtrOrAddr_To_Operand_BoundarySkip_Phi2() {
		uint8_t ui8Tmp;
		if constexpr ( _bFromAddr ) {
			LSN_INSTR_START_PHI2_READ( m_fsState.ui16Address, ui8Tmp );
		}
		else {
			LSN_INSTR_START_PHI2_READ( m_fsState.ui16Pointer, ui8Tmp );
		}
		m_fsState.ui8Operand = ui8Tmp;
		
		if ( !m_fsState.bBoundaryCrossed ) {
			// No page boundaries were crossed.  We can optionally end, but must always skip 2 extra functions.
			if constexpr ( _bEndInstr ) {
				LSN_FINISH_INST( true );			// Gives us a LSN_NEXT_FUNCTION.
				LSN_NEXT_FUNCTION_BY( 2 );			// Add 2 more.
			}
			else {
				LSN_NEXT_FUNCTION_BY( 3 );
			}
		}
		else {
			// A page boundary was crossed, so we can't skip anything or end early.  Keep going normally.
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Reads the stack, stores in m_fsState.ui8Operand. */
	template <bool _bEndInstr>
	void CCpu6502::Read_Stack_To_Operand_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( 0x100 | m_fsState.rRegs.ui8S, ui8Tmp );
		m_fsState.ui8Operand = ui8Tmp;
		
		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Reads the stack, stores in m_fsState.ui16Target.H. */
	template <int8_t _i8SOff, bool _bEndInstr>
	void CCpu6502::Read_Stack_To_Target_H_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( 0x100 | uint8_t( m_fsState.rRegs.ui8S + _i8SOff ), ui8Tmp );
		m_fsState.ui8Target[1] = ui8Tmp;
		
		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Performs OP = (OP << 1) | (C); A = A & (OP).  Sets flags C, N and Z. */
	void CCpu6502::Rla() {
		LSN_INSTR_START_PHI1( false );

		uint8_t ui8LowBit = m_fsState.rRegs.ui8Status & C();

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		m_fsState.ui8Operand = (m_fsState.ui8Operand << 1) | ui8LowBit;
		m_fsState.rRegs.ui8A &= m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP = (OP << 1) | (C).  Sets flags C, N, and Z. */
	void CCpu6502::Rol() {
		LSN_INSTR_START_PHI1( false );

		uint8_t ui8LowBit = m_fsState.rRegs.ui8Status & C();

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		m_fsState.ui8Operand = (m_fsState.ui8Operand << 1) | ui8LowBit;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP = (OP << 1) | (C).  Sets flags C, N, and Z. */
	void CCpu6502::RolOnA_BeginInst() {
		BeginInst();

		uint8_t ui8LowBit = m_fsState.rRegs.ui8Status & C();

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		m_fsState.rRegs.ui8A = (m_fsState.rRegs.ui8A << 1) | ui8LowBit;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs OP = (OP >> 1) | (C << 7).  Sets flags C, N, and Z. */
	void CCpu6502::Ror() {
		LSN_INSTR_START_PHI1( false );

		uint8_t ui8HiBit = (m_fsState.rRegs.ui8Status & C()) << 7;

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x01) != 0 );
		m_fsState.ui8Operand = (m_fsState.ui8Operand >> 1) | ui8HiBit;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
	void CCpu6502::RorOnA_BeginInst() {
		BeginInst();

		uint8_t ui8HiBit = (m_fsState.rRegs.ui8Status & C()) << 7;

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x01) != 0 );
		m_fsState.rRegs.ui8A = (m_fsState.rRegs.ui8A >> 1) | ui8HiBit;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Performs OP = (OP >> 1) | (C << 7); A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::Rra() {
		LSN_INSTR_START_PHI1( false );

		uint8_t ui8HiBit = (m_fsState.rRegs.ui8Status & C()) << 7;

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x01) != 0 );
		m_fsState.ui8Operand = (m_fsState.ui8Operand >> 1) | ui8HiBit;

		Adc( m_fsState.rRegs.ui8A, m_fsState.ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies m_fsState.ui16Target to PC, adjusts S. */
	void CCpu6502::Rti_BeginInst() {
		BeginInst();

		//LSN_UPDATE_S;

		m_fsState.rRegs.ui16Pc = m_fsState.ui16Target;
	}

	/** Adjusts PC and calls BeginInst(). */
	void CCpu6502::Rts_BeginInst() {
		BeginInst<true>();
	}

	/** Writes (A & X) to either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr>
	void CCpu6502::Sax_Phi2() {
		if constexpr ( _bToAddr ) {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X );
		}
		else {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X );
		}

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
	template <bool _bIncPc>
	void CCpu6502::Sbc_BeginInst() {
		BeginInst<_bIncPc>();

		Sbc( m_fsState.rRegs.ui8A, m_fsState.ui8Operand );
	}

	/** Performs X = (A & X) - OP.  Sets flags C, N and Z. */
	void CCpu6502::Sbx_IncPc_BeginInst() {
		BeginInst<true>();

		const uint8_t ui8AnX = (m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X);
		SetBit<C()>( m_fsState.rRegs.ui8Status, ui8AnX >= m_fsState.ui8Operand );
		m_fsState.rRegs.ui8X = ui8AnX - m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8X );
	}

	/** Sets the carry flag. */
	void CCpu6502::Sec_BeginInst() {
		BeginInst();

		SetBit<C(), true>( m_fsState.rRegs.ui8Status );
	}

	/** Sets the decimal flag. */
	void CCpu6502::Sed_BeginInst() {
		BeginInst();

		SetBit<D(), true>( m_fsState.rRegs.ui8Status );
	}

	/** Sets the IRQ flag. */
	void CCpu6502::Sei_BeginInst() {
		BeginInst();

		SetBit<I(), true>( m_fsState.rRegs.ui8Status );
	}

	/** Selects the BRK vector etc. */
	template <bool _bAdjS>
	void CCpu6502::SelectBrkVectors() {
		

		if constexpr ( _bAdjS ) {
			LSN_INSTR_START_PHI1( true );
			LSN_UPDATE_S;
		}
		else {
			LSN_INSTR_START_PHI1( false );
		}

#ifdef LSN_CPU_VERIFY
		m_fsState.vBrkVector = LSN_V_IRQ_BRK;
		m_fsState.bPushB = true;
#else

		// Select vector to use.
		if ( m_bIsReset ) {
			m_fsState.vBrkVector = LSN_V_RESET;
			//m_fsState.bPushB = false;
			m_bIsReset = false;
		}
		else if ( m_bDetectedNmi ) {
			m_fsState.vBrkVector = LSN_V_NMI;
			//m_fsState.bPushB = false;
		}
		else if ( m_bHandleIrq ) {
			m_fsState.vBrkVector = LSN_V_IRQ_BRK;
			//m_fsState.bPushB = false;
		}
		else {
			m_fsState.vBrkVector = LSN_V_IRQ_BRK;
			//m_fsState.bPushB = true;
		}

		if LSN_LIKELY( !m_bRdyLow ) {
			if ( m_bDetectedNmi ) {
				m_bHandleNmi = m_bDetectedNmi = false;
				m_bNmiStatusLine = false;
			}
			m_bHandleIrq = false;
		}
#endif	// #ifdef LSN_CPU_VERIFY

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Sets I and X. */
	void CCpu6502::SetBrkFlags() {
		LSN_INSTR_START_PHI1( true );

		SetBit<I() | M(), true>( m_fsState.rRegs.ui8Status );
		SetBit<X(), false>( m_fsState.rRegs.ui8Status );
		m_fsState.bAllowWritingToPc = true;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Illegal. Stores A & X & (high-byte of address + 1) at either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr, unsigned _uRdyCnt>
	void CCpu6502::Sha_Phi2() {		
		if constexpr ( _bToAddr ) {
			uint16_t ui16High = m_fsState.ui8Address[1];
			if ( !m_fsState.bBoundaryCrossed ) {
				++ui16High;
			}
			if ( m_ui8RdyOffCnt == _uRdyCnt + 0 ) {
				ui16High = 0xFFFF;
			}
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = ui16High & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Address[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = ui16High & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, ui16Val );
			}
		}
		else {
			uint16_t ui16High = m_fsState.ui8Pointer[1];
			if ( !m_fsState.bBoundaryCrossed ) {
				++ui16High;
			}
			if ( m_ui8RdyOffCnt == _uRdyCnt + 0 ) {
				ui16High = 0xFFFF;
			}
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = ui16High & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Pointer[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = ui16High & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, ui16Val );
			}
		}

		/* Stores A AND X AND (high-byte of addr. + 1) at addr.
		
		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		A AND X AND (H+1) -> M
		*/

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** Illegal. Puts A & X into SP; stores A & X & (high-byte of address + 1) at the address. */
	template <bool _bToAddr>
	void CCpu6502::Shs_Phi2() {
		m_fsState.rRegs.ui8S = m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;

		if constexpr ( _bToAddr ) {
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = m_fsState.ui8Address[1] & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Address[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = (m_fsState.ui8Address[1] + 1) & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, ui16Val );
			}
		}
		else {
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = m_fsState.ui8Pointer[1] & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Pointer[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = (m_fsState.ui8Pointer[1] + 1) & m_fsState.rRegs.ui8A & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, ui16Val );
			}
		}

		/* Puts A AND X in SP and stores A AND X AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		A AND X -> SP, A AND X AND (H+1) -> M
		*/

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** Illegal. Stores X & (high-byte of address + 1) at the address. */
	template <bool _bToAddr>
	void CCpu6502::Shx_Phi2() {
		if constexpr ( _bToAddr ) {
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = m_fsState.ui8Address[1] & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Address[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = (m_fsState.ui8Address[1] + 1) & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, ui16Val );
			}
		}
		else {
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = m_fsState.ui8Pointer[1] & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Pointer[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = (m_fsState.ui8Pointer[1] + 1) & m_fsState.rRegs.ui8X;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, ui16Val );
			}
		}

		/* Stores X AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		X AND (H+1) -> M
		*/

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** Illegal. Stores Y & (high-byte of address + 1) at the address. */
	template <bool _bToAddr>
	void CCpu6502::Shy_Phi2() {
		if constexpr ( _bToAddr ) {
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = m_fsState.ui8Address[1] & m_fsState.rRegs.ui8Y;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Address[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = (m_fsState.ui8Address[1] + 1) & m_fsState.rRegs.ui8Y;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, ui16Val );
			}
		}
		else {
			if ( m_fsState.bBoundaryCrossed ) {
				uint16_t ui16Val = m_fsState.ui8Pointer[1] & m_fsState.rRegs.ui8Y;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui8Pointer[0] | ui16Val << 8, ui16Val );
			}
			else {
				uint16_t ui16Val = (m_fsState.ui8Pointer[1] + 1) & m_fsState.rRegs.ui8Y;
				LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, ui16Val );
			}
		}

		/* Stores Y AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		Y AND (H+1) -> M
		*/

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** Performs OP = (OP << 1); A = A | (OP).  Sets flags C, N and Z. */
	void CCpu6502::Slo() {
		LSN_INSTR_START_PHI1( false );

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x80) != 0 );
		m_fsState.ui8Operand <<= 1;
		m_fsState.rRegs.ui8A |= m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP = (OP >> 1); A = A ^ (OP).  Sets flags C, N and Z. */
	void CCpu6502::Sre() {
		LSN_INSTR_START_PHI1( false );

		SetBit<C()>( m_fsState.rRegs.ui8Status, (m_fsState.ui8Operand & 0x01) != 0 );
		m_fsState.ui8Operand >>= 1;
		m_fsState.rRegs.ui8A ^= m_fsState.ui8Operand;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies A into X.  Sets flags N, and Z. */
	void CCpu6502::Tax_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8X = m_fsState.rRegs.ui8A;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8X );
	}

	/** Copies A into Y.  Sets flags N, and Z. */
	void CCpu6502::Tay_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8Y = m_fsState.rRegs.ui8A;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8Y );
	}

	/** Copies S into X. */
	void CCpu6502::Tsx_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8X = m_fsState.rRegs.ui8S;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8X );
	}

	/** Copies X into A.  Sets flags N, and Z. */
	void CCpu6502::Txa_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8A = m_fsState.rRegs.ui8X;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Copies Y into A.  Sets flags N, and Z. */
	void CCpu6502::Tya_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8A = m_fsState.rRegs.ui8Y;

		SetBit<N()>( m_fsState.rRegs.ui8Status, (m_fsState.rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_fsState.rRegs.ui8Status, !m_fsState.rRegs.ui8A );
	}

	/** Copies X into S. */
	void CCpu6502::Txs_BeginInst() {
		BeginInst();

		m_fsState.rRegs.ui8S = m_fsState.rRegs.ui8X;
	}

	/** Writes A to either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr>
	void CCpu6502::Write_A_To_AddrOrPtr_Phi2() {
		if constexpr ( _bToAddr ) {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, m_fsState.rRegs.ui8A );
		}
		else {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, m_fsState.rRegs.ui8A );
		}

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** Writes m_fsState.ui8Operand to either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr, bool _bEndInstr>
	void CCpu6502::Write_Operand_To_AddrOrPtr_Phi2() {
		if constexpr ( _bToAddr ) {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, m_fsState.ui8Operand );
		}
		else {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, m_fsState.ui8Operand );
		}

		if constexpr ( _bEndInstr ) {
			LSN_FINISH_INST( true );
		}
		else {
			LSN_NEXT_FUNCTION;
		}

		LSN_INSTR_END_PHI2;
	}

	/** Writes X to either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr>
	void CCpu6502::Write_X_To_AddrOrPtr_Phi2() {
		if constexpr ( _bToAddr ) {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, m_fsState.rRegs.ui8X );
		}
		else {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, m_fsState.rRegs.ui8X );
		}

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

	/** Writes Y to either m_fsState.ui16Pointer or m_fsState.ui16Address. */
	template <bool _bToAddr>
	void CCpu6502::Write_Y_To_AddrOrPtr_Phi2() {
		if constexpr ( _bToAddr ) {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Address, m_fsState.rRegs.ui8Y );
		}
		else {
			LSN_INSTR_START_PHI2_WRITE( m_fsState.ui16Pointer, m_fsState.rRegs.ui8Y );
		}

		LSN_FINISH_INST( true );

		LSN_INSTR_END_PHI2;
	}

}	// namespace lsn
