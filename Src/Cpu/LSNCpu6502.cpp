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
		m_bIrqSeenLowPhi2 |= m_bIrqStatusLine & static_cast<int8_t>(!(m_rRegs.ui8Status & I()));

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
	 * Begins a DMA transfer.
	 * 
	 * \param _ui8Val The value written to 0x4014.
	 */
	void CCpu6502::BeginDma( uint8_t _ui8Val ) {
		m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_IDLE, false>;
		m_ui16DmaAddress = uint16_t( _ui8Val ) << 8;
		m_bDmaGo = false;
		// Leave m_pfTickFuncCopy as-is to return to it after the transfer.
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
	 */
	void CCpu6502::Irq() {
		m_bIrqStatusLine = true;

		/*if ( m_ui64CycleCount < 279340 + 42150 ) {
			char szBuffer[128];
			::sprintf_s( szBuffer, "IRQ Requested on Cycle: %u.\r\n", static_cast<uint32_t>(m_ui64CycleCount) );
			::OutputDebugStringA( szBuffer );
		}*/
	}

	/**
	 * Clears the IRQ flag.
	 */
	void CCpu6502::ClearIrq() {
		m_bIrqStatusLine = false;

		/*if ( m_ui64CycleCount < 279340 + 42150 ) {
			char szBuffer[128];
			::sprintf_s( szBuffer, "IRQ Cleared on Cycle: %u.\r\n", static_cast<uint32_t>(m_ui64CycleCount) );
			::OutputDebugStringA( szBuffer );
		}*/
	}

	/**
	 * Gets the status of the IRQ line.
	 * 
	 * \return Returns true if the IRQ status line is low.
	 **/
	bool CCpu6502::GetIrqStatus() const {
		return m_bIrqStatusLine;
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
		m_rRegs.ui8A = cvoVerifyMe.cvsStart.cvrRegisters.ui8A;
		m_rRegs.ui8S = cvoVerifyMe.cvsStart.cvrRegisters.ui8S;
		m_rRegs.ui8X = cvoVerifyMe.cvsStart.cvrRegisters.ui8X;
		m_rRegs.ui8Y = cvoVerifyMe.cvsStart.cvrRegisters.ui8Y;
		m_rRegs.ui8Status = cvoVerifyMe.cvsStart.cvrRegisters.ui8Status;
		m_rRegs.ui16Pc = cvoVerifyMe.cvsStart.cvrRegisters.ui16Pc;

		for ( auto I = cvoVerifyMe.cvsStart.vRam.size(); I--; ) {
			m_pbBus->Write( cvoVerifyMe.cvsStart.vRam[I].ui16Addr, cvoVerifyMe.cvsStart.vRam[I].ui8Value );
		}
		m_pbBus->ApplyMap();				// Set default read/write functions.

		/*if ( "93 6" == cvoVerifyMe.sName ) {
			volatile int ghg = 0;
		}*/
		// Tick once for each cycle.
		m_ui8Operand = m_pbBus->Read( m_rRegs.ui16Pc );
		m_ui16PcModify = 1;
		for ( auto I = cvoVerifyMe.vCycles.size(); I--; ) {
			Tick();
			TickPhi2();
		}
		m_pbBus->ReadWriteLog().pop_back();
		
		// Verify.
#define LSN_VURIFFY( REG )																																												\
	if ( m_rRegs. ## REG != cvoVerifyMe.cvsEnd.cvrRegisters. ## REG ) {																																	\
		::OutputDebugStringA( cvoVerifyMe.sName.c_str() );																																				\
		::OutputDebugStringA( "\r\nCPU Failure: " # REG "\r\n" );																																		\
		::OutputDebugStringA( (std::string( "Expected: ") + std::to_string( cvoVerifyMe.cvsEnd.cvrRegisters. ## REG ) +																					\
			std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( cvoVerifyMe.cvsEnd.cvrRegisters. ## REG ) ) + std::string( ") Got: " ) + std::to_string( m_rRegs. ## REG ) +							\
			std::string( " (" ) + ee::CExpEval::ToHex( uint64_t( m_rRegs. ## REG ) ) + std::string( ")" )).c_str() );																					\
		::OutputDebugStringA( "\r\n\r\n" );																																								\
	}

		LSN_VURIFFY( ui8A );
		LSN_VURIFFY( ui8X );
		LSN_VURIFFY( ui8Y );
		LSN_VURIFFY( ui8S );
		LSN_VURIFFY( ui8Status );
		LSN_VURIFFY( ui16Pc );
#undef LSN_VURIFFY


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
	void CCpu6502::Adc() {
		LSN_INSTR_START_PHI1( true );

		Adc( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs an add-with-carry with an operand, setting flags C, N, V, and Z. */
	void CCpu6502::AdcAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		Adc( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds X to m_ui8Address[0]. */
	void CCpu6502::AddAddressAndX() {
		LSN_INSTR_START_PHI1( true );

		m_ui16Pointer = m_ui16Address + m_rRegs.ui8X;
		m_ui8Address[0] += m_rRegs.ui8X;

		m_bBoundaryCrossed = m_ui8Pointer[1] != m_ui8Address[1];

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds X to m_ui8Address[0]. */
	void CCpu6502::AddAddressAndXIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_ui16Pointer = m_ui16Address + m_rRegs.ui8X;
		m_ui8Address[0] += m_rRegs.ui8X;

		m_bBoundaryCrossed = m_ui8Pointer[1] != m_ui8Address[1];

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds address to Y. */
	void CCpu6502::AddAddressAndY() {
		LSN_INSTR_START_PHI1( true );

		m_ui16Pointer = m_ui16Address + m_rRegs.ui8Y;
		m_ui8Address[0] += m_rRegs.ui8Y;

		m_bBoundaryCrossed = m_ui8Pointer[1] != m_ui8Address[1];

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds address to Y. */
	void CCpu6502::AddAddressAndYAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_ui16Pointer = m_ui16Address + m_rRegs.ui8Y;
		m_ui8Address[0] += m_rRegs.ui8Y;

		m_bBoundaryCrossed = m_ui8Pointer[1] != m_ui8Address[1];

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Fixes high byte of address after adding address to Y. */
	void CCpu6502::AddAddressAndXorYHigh() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_ui8Address[1] = m_ui8Pointer[1];

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adds X and m_ui8Operand, stores to m_ui16Address, reads from m_ui8Operand. */
	void CCpu6502::AddXAndOperandToPointer() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_ui16Pointer = uint8_t( m_ui8Operand + m_rRegs.ui8X );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = A & OP.  Sets flags C, N, and Z, increases PC. */
	void CCpu6502::AncAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A &= m_ui8Operand;

		SetBit<C()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::And() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A &= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = A & OP.  Sets flags N and Z. */
	void CCpu6502::AndAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A &= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = (A | CONST) & X & OP.  Sets flags N and Z. */
	void CCpu6502::AneAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A = (m_rRegs.ui8A | 0xEE) & m_rRegs.ui8X & m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = A & OP; A = (A >> 1) | (C << 7).  Sets flags C, V, N and Z. */
	void CCpu6502::ArrAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A &= m_ui8Operand;
		uint8_t ui8HiBit = (m_rRegs.ui8Status & C()) << 7;
		// It carries if the last bit gets shifted in.
		SetBit<C()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		m_rRegs.ui8A = (m_rRegs.ui8A >> 1) | ui8HiBit;
		SetBit<Z()>( m_rRegs.ui8Status, m_rRegs.ui8A == 0x00 );
		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<V()>( m_rRegs.ui8Status,
			((m_rRegs.ui8Status & C()) ^ ((m_rRegs.ui8A >> 5) & 0x1)) != 0 );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs M <<= 1.  Sets C, N, and V. */
	void CCpu6502::Asl() {
		LSN_INSTR_START_PHI1( true );

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );

		m_ui8Operand <<= 1;

		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs M <<= 1.  Sets C, N, and V, increases PC. */
	void CCpu6502::AslAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );

		m_ui8Operand <<= 1;

		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A <<= 1.  Sets C, N, and V. */
	void CCpu6502::AslOnA() {
		LSN_INSTR_START_PHI1( true );

		SetBit<C()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );

		m_rRegs.ui8A <<= 1;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A &= OP; A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::AsrAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A &= m_ui8Operand;
		SetBit<C()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x01) != 0 );

		m_rRegs.ui8A >>= 1;

		SetBit<N(), false>( m_rRegs.ui8Status );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Sets flags N, V and Z according to a bit test. */
	void CCpu6502::Bit() {
		LSN_INSTR_START_PHI1( true );

		SetBit<V()>( m_rRegs.ui8Status, (m_ui8Operand & (1 << 6)) != 0x00 );
		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & (1 << 7)) != 0x00 );
		SetBit<Z()>( m_rRegs.ui8Status, !(m_ui8Operand & m_rRegs.ui8A) );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	template <unsigned _uBit, unsigned _uVal>
	void CCpu6502::Branch_Cycle2() {
		LSN_INSTR_START_PHI1( true );

		m_bTakeJump = (m_rRegs.ui8Status & _uBit) == (_uVal * _uBit);
		LSN_UPDATE_PC;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
	void CCpu6502::Branch_Cycle2_Phi2() {
		if ( !m_bTakeJump ) {
			LSN_FINISH_INST( true );
			/*LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, m_ui8Operand );
			m_ui16PcModify = 1;*/
		}
		else {
			uint8_t ui8Tmp;
			LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, ui8Tmp );

			LSN_NEXT_FUNCTION;

			m_ui16Address = static_cast<int16_t>(static_cast<int8_t>(m_ui8Operand)) + m_rRegs.ui16Pc;

			m_bBoundaryCrossed = m_ui8Address[1] != m_rRegs.ui8Pc[1];
			if ( !m_bBoundaryCrossed ) {
				LSN_CHECK_INTERRUPTS;
			}
		}

		LSN_INSTR_END_PHI2;
	}

	/** 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary. */
	void CCpu6502::Branch_Cycle3() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8Pc[0] = m_ui8Address[0];

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;

		/*
		* bool bCrossed = m_ccCurContext.j.ui8Bytes[1] != pc.ui8Bytes[1];
		if ( bCrossed ) {
			// Fetch next opcode.
			LSN_INSTR_READ_DISCARD_PHI1( pc.PC, Phi2_Read<LSN_BRANCH_NMI> );

			LSN_ADVANCE_CONTEXT_COUNTERS;
			// Crossed a page boundary.
			// Set PCL.
			pc.ui8Bytes[0] = m_ccCurContext.j.ui8Bytes[0];
			m_bBoundaryCrossed = true;

			// NMI not polled here.
			LSN_INSTR_END_PHI1( LSN_BRANCH_NMI );
		}
		else {
			// Fetch next opcode.
			LSN_INSTR_READ_DISCARD_PHI1( pc.PC, Phi2_Read<true> );
			// Did not cross a page boundary.

			// Last cycle in the instruction.
			LSN_FINISH_INST( false );

			// Set PCL.
			pc.ui8Bytes[0] = m_ccCurContext.j.ui8Bytes[0];

			LSN_INSTR_END_PHI1( true );
		}*/
	}

	/** 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary. */
	void CCpu6502::Branch_Cycle3_Phi2() {
		if ( m_bBoundaryCrossed ) {
			uint8_t ui8Tmp;
			LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, ui8Tmp );
			LSN_NEXT_FUNCTION;
		}
		else {
			LSN_FINISH_INST( false );
			/*LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, m_ui8Operand );
			m_ui16PcModify = 1;*/
		}

		LSN_INSTR_END_PHI2;
	}

	/** 4th cycle of branch instructions. Page boundary was crossed. */
	void CCpu6502::Branch_Cycle4() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8Pc[1] = m_ui8Address[1];

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies the vector address into PC. */
	void CCpu6502::Brk() {
		LSN_INSTR_START_PHI1( true );
		m_bAllowWritingToPc = true;
		m_rRegs.ui16Pc = m_ui16Address;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Clears the carry bit. */
	void CCpu6502::Clc() {
		LSN_INSTR_START_PHI1( true );
		
		SetBit<C(), false>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Clears the decimal flag. */
	void CCpu6502::Cld() {
		LSN_INSTR_START_PHI1( true );

		SetBit<D(), false>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Clears the IRQ flag. */
	void CCpu6502::Cli() {
		LSN_INSTR_START_PHI1( true );

		SetBit<I(), false>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Clears the overflow flag. */
	void CCpu6502::Clv() {
		LSN_INSTR_START_PHI1( true );

		SetBit<V(), false>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Compares A with OP. */
	void CCpu6502::Cmp() {
		LSN_INSTR_START_PHI1( true );

		Cmp( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Compares A with OP. */
	void CCpu6502::CmpAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		Cmp( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Compares X with OP. */
	void CCpu6502::Cpx() {
		LSN_INSTR_START_PHI1( true );

		Cmp( m_rRegs.ui8X, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Compares X with OP. */
	void CCpu6502::CpxAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		Cmp( m_rRegs.ui8X, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Compares Y with OP. */
	void CCpu6502::Cpy() {
		LSN_INSTR_START_PHI1( true );

		Cmp( m_rRegs.ui8Y, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}
		
	/** Compares Y with OP. */
	void CCpu6502::CpyAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		Cmp( m_rRegs.ui8Y, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies Operand to Status without the B bit. */
	void CCpu6502::CopyOperandToStatusWithoutB_AdjustS() {
		LSN_INSTR_START_PHI1( true );
		
		LSN_UPDATE_S;

		constexpr uint8_t ui8Mask = M() | X();
		m_rRegs.ui8Status = (m_ui8Operand & ~ui8Mask) | (m_rRegs.ui8Status & ui8Mask);

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Fetches the low byte of the NMI/IRQ/BRK/reset vector into the low byte of PC. */
	void CCpu6502::CopyVectorPcl_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_vBrkVector, m_ui8Address[0] );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the high byte of the NMI/IRQ/BRK/reset vector into the high byte of PC and sets the I flag. */
	void CCpu6502::CopyVectorPch() {
		LSN_INSTR_START_PHI1( true );

		SetBit<I(), true>( m_rRegs.ui8Status );
		SetBit<X(), false>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Fetches the high byte of the NMI/IRQ/BRK/reset vector into the high byte of PC and sets the I flag. */
	void CCpu6502::CopyVectorPch_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_vBrkVector + 1, m_ui8Address[1] );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::Eor() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A ^= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs [ADDR]--; CMP(A).  Sets flags C, N, and Z. */
	void CCpu6502::Dcp() {
		LSN_INSTR_START_PHI1( true );

		--m_ui8Operand;
		Cmp( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs [ADDR]--.  Sets flags N and Z. */
	void CCpu6502::Dec() {
		LSN_INSTR_START_PHI1( true );

		--m_ui8Operand;
		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs X--.  Sets flags N and Z. */
	void CCpu6502::Dex() {
		LSN_INSTR_START_PHI1( true );

		--m_rRegs.ui8X;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8X );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs Y--.  Sets flags N and Z. */
	void CCpu6502::Dey() {
		LSN_INSTR_START_PHI1( true );

		--m_rRegs.ui8Y;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8Y );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = A ^ OP.  Sets flags N and Z. */
	void CCpu6502::EorAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A ^= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Fetches the address and increments PC. */
	void CCpu6502::FetchAddressAndIncPc_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, m_ui16Address );
		m_ui16PcModify = 1;
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Fetches into m_ui8Address[1] and increments PC .*/
	void CCpu6502::FetchAddressHighAndIncPc_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, m_ui8Address[1] );
		m_ui16PcModify = 1;
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the operand and increments PC. */
	void CCpu6502::FetchOpcodeAndIncPc() {
		LSN_INSTR_START_PHI1( true );
		m_ui16OpCode = m_ui8Operand;
		
		/*if ( m_bHandleNmi || m_bHandleIrq || m_bIsReset ) {
			m_ui16OpCode = 0;
			m_ui16PcModify = 0;
			m_bAllowWritingToPc = false;
		}*/

		m_pfCurInstruction = m_iInstructionSet[m_ui16OpCode].pfHandler;
		
		LSN_UPDATE_PC;

		LSN_NEXT_FUNCTION;

		BeginInst();
		LSN_INSTR_END_PHI1;
	}

	/** Fetches the operand. */
	void CCpu6502::FetchOperandAndDiscard_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, ui8Op );
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the next opcode and does not the program counter. */
	void CCpu6502::FetchOperandDecPc_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, ui8Op );
		m_ui16PcModify = uint16_t( -1 );
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Fetches the operand and increments PC. */
	void CCpu6502::FetchOperandAndIncPc_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, ui8Op );
		m_ui8Operand = ui8Op;
		m_ui16PcModify = 1;
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Fetches a pointer and increments PC .*/
	void CCpu6502::FetchPointerAndIncPc_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, ui8Op );
		m_ui16Pointer = ui8Op;
		m_ui16PcModify = 1;
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Fetches into m_ui8Pointer[1] and increments PC .*/
	void CCpu6502::FetchPointerHighAndIncPc_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, ui8Op );
		m_ui8Pointer[1] = ui8Op;
		m_ui16PcModify = 1;
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP++.  Sets flags N and Z. */
	void CCpu6502::Inc() {
		LSN_INSTR_START_PHI1( true );

		++m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs X++.  Sets flags N and Z. */
	void CCpu6502::Inx() {
		LSN_INSTR_START_PHI1( true );

		++m_rRegs.ui8X;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8X );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs Y++.  Sets flags N and Z. */
	void CCpu6502::Iny() {
		LSN_INSTR_START_PHI1( true );

		++m_rRegs.ui8Y;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8Y );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs M++; SBC.  Sets flags C, N, V, and Z. */
	void CCpu6502::Isb() {
		LSN_INSTR_START_PHI1( true );

		++m_ui8Operand;
		Sbc( m_rRegs.ui8A, m_ui8Operand );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Jams the machine. */
	void CCpu6502::Jam() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Jams the machine. */
	void CCpu6502::Jam_Phi2() {
		uint8_t ui8Op;
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc + 1, ui8Op );
		
		LSN_NEXT_FUNCTION;

		LSN_NEXT_FUNCTION_BY( -2 );

		LSN_INSTR_END_PHI2;
	}

	/** Copies m_ui16Address PC. */
	void CCpu6502::Jmp() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui16Pc = m_ui16Address;
		m_ui16PcModify = 0;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies m_ui16Address into PC. */
	void CCpu6502::Jsr() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui16Pc = m_ui16Address;
		m_ui16PcModify = 0;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = X = S = (OP & S).  Sets flags N and Z. */
	void CCpu6502::Las() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A = m_rRegs.ui8X = m_rRegs.ui8S = (m_ui8Operand & m_rRegs.ui8S);

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = X = OP.  Sets flags N and Z. */
	void CCpu6502::Lax() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A = m_rRegs.ui8X = m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = OP.  Sets flags N and Z. */
	void CCpu6502::Lda() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A = m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = OP.  Sets flags N and Z. */
	void CCpu6502::LdaAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A = m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs X = OP.  Sets flags N and Z. */
	void CCpu6502::Ldx() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8X = m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8X );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs X = OP.  Sets flags N and Z. */
	void CCpu6502::LdxAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8X = m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8X );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs Y = OP.  Sets flags N and Z. */
	void CCpu6502::Ldy() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8Y = m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8Y );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs Y = OP.  Sets flags N and Z. */
	void CCpu6502::LdyAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8Y = m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8Y );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::Lsr() {
		LSN_INSTR_START_PHI1( true );

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x01) != 0 );
		m_ui8Operand >>= 1;

		SetBit<N(), false>( m_rRegs.ui8Status );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A >>= 1.  Sets flags C, N, and Z. */
	void CCpu6502::LsrOnA() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		SetBit<C()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x01) != 0 );
		m_rRegs.ui8A >>= 1;

		SetBit<N(), false>( m_rRegs.ui8Status );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = X = (A | CONST) & OP.  Sets flags N and Z. */
	void CCpu6502::LxaAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;
#ifdef LSN_CPU_VERIFY
		m_rRegs.ui8A = m_rRegs.ui8X = (m_rRegs.ui8A | 0xEE) & m_ui8Operand;
#else
		m_rRegs.ui8A = m_rRegs.ui8X = (m_rRegs.ui8A | 0xFF) & m_ui8Operand;
#endif	// #ifdef LSN_CPU_VERIFY

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adjusts S after stack operation. */
	void CCpu6502::NullAdjustS_Read() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_S;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Adjusts S after stack operation. */
	void CCpu6502::NullAdjustS_Write() {
		LSN_INSTR_START_PHI1( false );

		LSN_UPDATE_S;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Increments PC. */
	void CCpu6502::NullIncPc_Read() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Increments PC. */
	void CCpu6502::NullIncPc_Write() {
		LSN_INSTR_START_PHI1( false );

		LSN_UPDATE_PC;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Does nothing. */
	void CCpu6502::Null_Read() {
		LSN_INSTR_START_PHI1( true );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Does nothing. */
	void CCpu6502::Null_Write() {
		LSN_INSTR_START_PHI1( false );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A |= Operand with m_ui8Operand.  Sets flags N and Z. */
	void CCpu6502::Ora() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A |= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A |= Operand with m_ui8Operand.  Sets flags N and Z, increases PC. */
	void CCpu6502::OraAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		m_rRegs.ui8A |= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );
		

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Pulls the accumulator. */
	void CCpu6502::Pla() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A = m_ui8Operand;
		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Sets m_ui8Operand to the status byte with Break and Reserved set. */
	void CCpu6502::Php() {
		LSN_INSTR_START_PHI1( true );

		m_ui8Operand = m_rRegs.ui8Status;
		SetBit<X() | M(), true>( m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Pulls the status byte, unsets X, sets M. */
	void CCpu6502::Plp() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8Status = (m_ui8Operand & ~X()) | M();

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies the vector address into PC. */
	void CCpu6502::PrefetchNextOp() {
		LSN_FINISH_INST( true );

		/*LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, m_ui8Operand );
		m_ui16PcModify = 1;*/

		LSN_INSTR_END_PHI2;
	}

	/** Pulls from the stack, stores in m_ui8Operand. */
	void CCpu6502::PullStackToOperand_Phi2() {
		LSN_POP( m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pulls from the stack, stores in PC.l. */
	void CCpu6502::PullStackToPcl_Phi2() {
		LSN_POP( m_rRegs.ui8Pc[0] );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes A. */
	void CCpu6502::PushA_Phi2() {
		LSN_PUSH( m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes m_ui8Operand. */
	void CCpu6502::PushOperand_Phi2() {
		LSN_PUSH( m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes PCH. */
	void CCpu6502::PushPch_Phi2() {
		LSN_PUSH( m_rRegs.ui8Pc[1] );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes PCL. */
	void CCpu6502::PushPcl_Phi2() {
		LSN_PUSH( m_rRegs.ui8Pc[0] );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes the status byte with B conditionally set. */
	void CCpu6502::PushStatus() {
		LSN_INSTR_START_PHI1( false );

		LSN_UPDATE_S;

		// Select vector to use.
		if ( m_bIsReset ) {
			m_vBrkVector = LSN_V_RESET;
			m_bPushB = false;
			m_bIsReset = false;
		}
		else if ( m_bDetectedNmi ) {
			m_vBrkVector = LSN_V_NMI;
			m_bPushB = false;
		}
		else if ( m_bHandleIrq ) {
			m_vBrkVector = LSN_V_IRQ_BRK;
			m_bPushB = false;
		}
		else {
			m_vBrkVector = LSN_V_IRQ_BRK;
			m_bPushB = true;
		}

		if ( m_bDetectedNmi ) {
			m_bHandleNmi = m_bDetectedNmi = false;
			m_bNmiStatusLine = false;
		}
		m_bHandleIrq = false;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Pushes the status byte with B conditionally set. */
	void CCpu6502::PushStatus_Phi2() {
		if ( m_bPushB ) {
			LSN_PUSH( m_rRegs.ui8Status | X() );
		}
		else {
			LSN_PUSH( m_rRegs.ui8Status );
		}

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes X. */
	void CCpu6502::PushX_Phi2() {
		LSN_PUSH( m_rRegs.ui8X );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Pushes Y. */
	void CCpu6502::PushY_Phi2() {
		LSN_PUSH( m_rRegs.ui8Y );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_ui16Address and stores the result in m_ui8Operand. */
	void CCpu6502::ReadAddressToOperand_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_ui16Address, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		m_bBoundaryCrossed = m_ui8Pointer[1] != m_ui8Address[1];

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_ui16Address to m_ui8Operand, skips next cycle if m_ui8Pointer[1] == m_ui8Address[1]. */
	void CCpu6502::ReadAddressToOperand_BoundarySkip_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_ui16Address, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		if ( m_ui8Pointer[1] == m_ui8Address[1] ) {
			LSN_NEXT_FUNCTION_BY( 2 );
		}

		LSN_INSTR_END_PHI2;
	}

	/** Adds X and m_ui8Operand, stores to m_ui16Address, reads from m_ui8Operand. */
	void CCpu6502::ReadOperandAndDiscard_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( m_ui8Operand, ui8Tmp );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_ui8Operand and stores in m_ui16Address. */
	void CCpu6502::ReadOperandToAddress_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_ui8Operand, m_ui16Address );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_ui8Operand and stores in m_ui8Operand. */
	void CCpu6502::ReadOperandToOperand_Phi2() {
		uint8_t ui8Tmp;
		LSN_INSTR_START_PHI2_READ( m_ui8Operand, ui8Tmp );
		m_ui8Operand = ui8Tmp;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_ui16Pointer and stores the low byte in m_ui8Address[1]. */
	void CCpu6502::ReadPointerToAddressHigh_Phi2() {
		LSN_INSTR_START_PHI2_READ( uint8_t( m_ui16Pointer + 1 ), m_ui8Address[1] );
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_ui16Pointer and stores the low byte in m_ui8Address[1]. */
	void CCpu6502::ReadPointerToAddressHigh_SamePage_Phi2() {
		LSN_INSTR_START_PHI2_READ( (m_ui8Pointer[1] << 8) | uint8_t( m_ui8Pointer[0] + 1 ), m_ui8Address[1] );
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from m_ui16Pointer and stores the low byte in m_ui8Address[0]. */
	void CCpu6502::ReadPointerToAddressLow_Phi2() {
		LSN_INSTR_START_PHI2_READ( m_ui16Pointer, m_ui16Address );
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads the stack, stores in m_ui8Operand. */
	void CCpu6502::ReadStackToOperand_Phi2() {
		LSN_INSTR_START_PHI2_READ( 0x100 | m_rRegs.ui8S, m_ui8Operand );
		
		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Reads from the stack, stores in PC.h. */
	void CCpu6502::ReadStackToPch_Phi2() {
		//LSN_POP( m_rRegs.ui8Pc[1] );
		LSN_INSTR_START_PHI2_READ( (0x100 | uint8_t( m_rRegs.ui8S)), m_rRegs.ui8Pc[1] );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Performs OP = (OP << 1) | (C); A = A & (OP).  Sets flags C, N and Z. */
	void CCpu6502::Rla() {
		LSN_INSTR_START_PHI1( true );

		uint8_t ui8LowBit = m_rRegs.ui8Status & C();

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		m_ui8Operand = (m_ui8Operand << 1) | ui8LowBit;
		m_rRegs.ui8A &= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP = (OP << 1) | (C).  Sets flags C, N, and Z. */
	void CCpu6502::Rol() {
		LSN_INSTR_START_PHI1( true );

		uint8_t ui8LowBit = m_rRegs.ui8Status & C();

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		m_ui8Operand = (m_ui8Operand << 1) | ui8LowBit;

		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = (A << 1) | (C).  Sets flags C, N, and Z. */
	void CCpu6502::RolOnA() {
		LSN_INSTR_START_PHI1( true );

		uint8_t ui8LowBit = m_rRegs.ui8Status & C();

		SetBit<C()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		m_rRegs.ui8A = (m_rRegs.ui8A << 1) | ui8LowBit;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
	void CCpu6502::Ror() {
		LSN_INSTR_START_PHI1( true );

		uint8_t ui8HiBit = (m_rRegs.ui8Status & C()) << 7;

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x01) != 0 );
		m_ui8Operand = (m_ui8Operand >> 1) | ui8HiBit;

		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
	void CCpu6502::RorAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		uint8_t ui8HiBit = (m_rRegs.ui8Status & C()) << 7;

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x01) != 0 );
		m_ui8Operand = (m_ui8Operand >> 1) | ui8HiBit;

		SetBit<N()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
	void CCpu6502::RorOnA() {
		LSN_INSTR_START_PHI1( true );

		uint8_t ui8HiBit = (m_rRegs.ui8Status & C()) << 7;

		SetBit<C()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x01) != 0 );
		m_rRegs.ui8A = (m_rRegs.ui8A >> 1) | ui8HiBit;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP = (OP >> 1) | (C << 7); A += OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::Rra() {
		LSN_INSTR_START_PHI1( true );

		uint8_t ui8HiBit = (m_rRegs.ui8Status & C()) << 7;

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x01) != 0 );
		m_ui8Operand = (m_ui8Operand >> 1) | ui8HiBit;

		Adc( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Writes (A & X) to m_ui16Address. */
	void CCpu6502::Sax_Phi2() {
		LSN_INSTR_START_PHI2_WRITE( m_ui16Address, m_rRegs.ui8A & m_rRegs.ui8X );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::Sbc() {
		LSN_INSTR_START_PHI1( true );

		Sbc( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
	void CCpu6502::SbcAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		Sbc( m_rRegs.ui8A, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs X = (A & X) - OP.  Sets flags C, N and Z. */
	void CCpu6502::SbxAndIncPc() {
		LSN_INSTR_START_PHI1( true );

		LSN_UPDATE_PC;

		const uint8_t ui8AnX = (m_rRegs.ui8A & m_rRegs.ui8X);
		SetBit<C()>( m_rRegs.ui8Status, ui8AnX >= m_ui8Operand );
		m_rRegs.ui8X = ui8AnX - m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8X );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Sets the carry flag. */
	void CCpu6502::Sec() {
		LSN_INSTR_START_PHI1( true );

		SetBit<C(), true>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Sets the decimal flag. */
	void CCpu6502::Sed() {
		LSN_INSTR_START_PHI1( true );

		SetBit<D(), true>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Sets the IRQ flag. */
	void CCpu6502::Sei() {
		LSN_INSTR_START_PHI1( true );

		SetBit<I(), true>( m_rRegs.ui8Status );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Illegal. Stores A & X & (high-byte of address + 1) at the address. */
	void CCpu6502::Sha_Phi2() {
		if ( m_bBoundaryCrossed ) {
			uint16_t ui16Val = m_ui8Address[1] & m_rRegs.ui8A & m_rRegs.ui8X;
			LSN_INSTR_START_PHI2_WRITE( m_ui8Address[0] | ui16Val << 8, ui16Val );
		}
		else {
			uint16_t ui16Val = (m_ui8Address[1] + 1) & m_rRegs.ui8A & m_rRegs.ui8X;
			LSN_INSTR_START_PHI2_WRITE( m_ui16Address, ui16Val );
		}

		/* Stores A AND X AND (high-byte of addr. + 1) at addr.
		
		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		A AND X AND (H+1) -> M
		*/

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Illegal. Puts A & X into SP; stores A & X & (high-byte of address + 1) at the address. */
	void CCpu6502::Shs_Phi2() {
		m_rRegs.ui8S = m_rRegs.ui8A & m_rRegs.ui8X;
		if ( m_bBoundaryCrossed ) {
			uint16_t ui16Val = m_ui8Address[1] & m_rRegs.ui8A & m_rRegs.ui8X;
			LSN_INSTR_START_PHI2_WRITE( m_ui8Address[0] | ui16Val << 8, ui16Val );
		}
		else {
			uint16_t ui16Val = (m_ui8Address[1] + 1) & m_rRegs.ui8A & m_rRegs.ui8X;
			LSN_INSTR_START_PHI2_WRITE( m_ui16Address, ui16Val );
		}

		/* Puts A AND X in SP and stores A AND X AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		A AND X -> SP, A AND X AND (H+1) -> M
		*/

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Illegal. Stores X & (high-byte of address + 1) at the address. */
	void CCpu6502::Shx_Phi2() {
		if ( m_bBoundaryCrossed ) {
			uint16_t ui16Val = m_ui8Address[1] & m_rRegs.ui8X;
			LSN_INSTR_START_PHI2_WRITE( m_ui8Address[0] | ui16Val << 8, ui16Val );
		}
		else {
			uint16_t ui16Val = (m_ui8Address[1] + 1) & m_rRegs.ui8X;
			LSN_INSTR_START_PHI2_WRITE( m_ui16Address, ui16Val );
		}

		/* Stores X AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		X AND (H+1) -> M
		*/

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Illegal. Stores Y & (high-byte of address + 1) at the address. */
	void CCpu6502::Shy_Phi2() {
		if ( m_bBoundaryCrossed ) {
			uint16_t ui16Val = m_ui8Address[1] & m_rRegs.ui8Y;
			LSN_INSTR_START_PHI2_WRITE( m_ui8Address[0] | ui16Val << 8, ui16Val );
		}
		else {
			uint16_t ui16Val = (m_ui8Address[1] + 1) & m_rRegs.ui8Y;
			LSN_INSTR_START_PHI2_WRITE( m_ui16Address, ui16Val );
		}

		/* Stores Y AND (high-byte of addr. + 1) at addr.

		unstable: sometimes 'AND (H+1)' is dropped, page boundary crossings may not work (with the high-byte of the value used as the high-byte of the address)

		Y AND (H+1) -> M
		*/

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Performs OP = (OP << 1); A = A | (OP).  Sets flags C, N and Z. */
	void CCpu6502::Slo() {
		LSN_INSTR_START_PHI1( true );

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x80) != 0 );
		m_ui8Operand <<= 1;
		m_rRegs.ui8A |= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Performs OP = (OP >> 1); A = A ^ (OP).  Sets flags C, N and Z. */
	void CCpu6502::Sre() {
		LSN_INSTR_START_PHI1( true );

		SetBit<C()>( m_rRegs.ui8Status, (m_ui8Operand & 0x01) != 0 );
		m_ui8Operand >>= 1;
		m_rRegs.ui8A ^= m_ui8Operand;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies A into X.  Sets flags N, and Z. */
	void CCpu6502::Tax() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8X = m_rRegs.ui8A;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8X );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies A into Y.  Sets flags N, and Z. */
	void CCpu6502::Tay() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8Y = m_rRegs.ui8A;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8Y & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8Y );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies S into X. */
	void CCpu6502::Tsx() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8X = m_rRegs.ui8S;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8X & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8X );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies X into A.  Sets flags N, and Z. */
	void CCpu6502::Txa() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A = m_rRegs.ui8X;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies X into S. */
	void CCpu6502::Txs() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8S = m_rRegs.ui8X;

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Copies Y into A.  Sets flags N, and Z. */
	void CCpu6502::Tya() {
		LSN_INSTR_START_PHI1( true );

		m_rRegs.ui8A = m_rRegs.ui8Y;

		SetBit<N()>( m_rRegs.ui8Status, (m_rRegs.ui8A & 0x80) != 0 );
		SetBit<Z()>( m_rRegs.ui8Status, !m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI1;
	}

	/** Writes A to m_ui16Address. */
	void CCpu6502::WriteAToAddress_Phi2() {
		LSN_INSTR_START_PHI2_WRITE( m_ui16Address, m_rRegs.ui8A );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Writes m_ui8Operand to m_ui16Address. */
	void CCpu6502::WriteOperandToAddress_Phi2() {
		LSN_INSTR_START_PHI2_WRITE( m_ui16Address, m_ui8Operand );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Writes X to m_ui16Address. */
	void CCpu6502::WriteXToAddress_Phi2() {
		LSN_INSTR_START_PHI2_WRITE( m_ui16Address, m_rRegs.ui8X );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

	/** Writes Y to m_ui16Address. */
	void CCpu6502::WriteYToAddress_Phi2() {
		LSN_INSTR_START_PHI2_WRITE( m_ui16Address, m_rRegs.ui8Y );

		LSN_NEXT_FUNCTION;

		LSN_INSTR_END_PHI2;
	}

}	// namespace lsn
