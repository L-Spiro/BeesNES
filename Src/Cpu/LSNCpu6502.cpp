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

		if ( "10 91 3e" == cvoVerifyMe.sName ) {
			volatile int ghg = 0;
		}
		for ( auto I = cvoVerifyMe.vCycles.size(); I--; ) {
			Tick();
			TickPhi2();
		}
		Tick();
		
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
	

}	// namespace lsn
