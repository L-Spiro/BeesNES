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


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Input/LSNInputPoller.h"
#include "../Mappers/LSNMapperBase.h"
#include "../System/LSNInterruptable.h"
#include "../System/LSNTickable.h"
#include "LSNCpuBase.h"

#include <vector>

#ifdef LSN_CPU_VERIFY
#include "LSONJson.h"
#endif	// #ifdef LSN_CPU_VERIFY

#define LSN_INSTR_START_PHI1( ISREAD )						/*m_bIsReadCycle = (ISREAD)*/
#define LSN_INSTR_END_PHI1
#define LSN_INSTR_START_PHI2_READ( ADDR, RESULT )			RESULT = m_pbBus->Read( uint16_t( ADDR ) ); /*m_bIsReadCycle = true;*/											\
															if ( m_bRdyLow ) { m_ui16DmaCpuAddress = uint16_t( ADDR );														\
																m_bDmaGo = true;																							\
																/*return*/; }
#define LSN_INSTR_START_PHI2_WRITE( ADDR, VAL )				/*m_bIsReadCycle = false;*/ m_pbBus->Write( uint16_t( ADDR ), uint8_t( VAL ) )
#define LSN_INSTR_END_PHI2

#define LSN_NEXT_FUNCTION_BY( AMT )							m_ui8FuncIndex += uint8_t( AMT )
#define LSN_NEXT_FUNCTION									LSN_NEXT_FUNCTION_BY( 1 )
#define LSN_FINISH_INST( CHECK_INTERRUPTS )					if constexpr ( CHECK_INTERRUPTS ) { LSN_CHECK_INTERRUPTS; } m_pfTickFunc = m_pfTickFuncCopy = &CCpu6502::Tick_NextInstructionStd; PrefetchOpcode()

#define LSN_CHECK_INTERRUPTS								/*if ( !(m_rRegs.ui8Status & I()) )*/ { m_bHandleIrq = m_bIrqStatusPhi1Flag; } m_bHandleNmi |= m_bDetectedNmi

#define LSN_PUSH( VAL )										LSN_INSTR_START_PHI2_WRITE( (0x100 | m_rRegs.ui8S), (VAL) ); m_ui8SModify = uint8_t( -1 )
#define LSN_POP( RESULT )									LSN_INSTR_START_PHI2_READ( (0x100 | uint8_t( m_rRegs.ui8S)), (RESULT) ); m_ui8SModify = 1

#define LSN_UPDATE_PC										if ( m_bAllowWritingToPc ) { m_rRegs.ui16Pc += m_ui16PcModify; } m_ui16PcModify = 0
#define LSN_UPDATE_S										m_rRegs.ui8S += m_ui8SModify; m_ui8SModify = 0


namespace lsn {

	/**
	 * Class CCpu6502
	 * \brief Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 *
	 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 */
	class CCpu6502 : public CTickable, public CInterruptable, public CCpuBase {
	public :
		// == Various constructors.
		CCpu6502( CCpuBus * _pbBus );
		~CCpu6502();


		// == Enumeration.
		/** Status flags. */
		static constexpr uint8_t							C() { return (1 << 0); }														/**< Carry         (0=No Carry, 1=Carry). */
		static constexpr uint8_t							Z() { return (1 << 1); }														/**< Zero          (0=Nonzero, 1=Zero). */
		static constexpr uint8_t							I() { return (1 << 2); }														/**< IRQ Disable   (0=IRQ Enable, 1=IRQ Disable). */
		static constexpr uint8_t							D() { return (1 << 3); }														/**< Decimal Mode  (0=Normal, 1=BCD Mode for ADC/SBC opcodes). */
		static constexpr uint8_t							X() { return (1 << 4); }														/**< Break Flag    (0=IRQ/NMI, 1=RESET or BRK/PHP opcode). */
		static constexpr uint8_t							M() { return (1 << 5); }														/**< Reserved. */
		static constexpr uint8_t							V() { return (1 << 6); }														/**< Overflow      (0=No Overflow, 1=Overflow). */
		static constexpr uint8_t							N() { return (1 << 7); }														/**< Negative/Sign (0=Positive, 1=Negative). */

		/** Special addresses. */
		enum LSN_VECTORS : uint16_t {
			LSN_V_NMI										= 0xFFFA,																		/**< The address of execution during an NMI interrupt. */
			LSN_V_RESET										= 0xFFFC,																		/**< The address of execution during a reset. */
			LSN_V_IRQ_BRK									= 0xFFFE,																		/**< The address of execution during an IRQ or BRK interrupt. */
		};

		/** DMA states. */
		enum LSN_DMA_STATES {
			LSN_DS_IDLE,																													/**< Waiting for a read cycle. */
			LSN_DS_READ_WRITE,																												/**< Reading and writing. */
			LSN_DS_END,																														/**< Emulates repeating of the last-attempted read cycle by issuing a dummy Phi1 and then reading the DMA CPU address during Phi2, then sets the tick pointers back to normal operation. */
		};


		// == Types.
		/** The processor registers. */
	struct LSN_REGISTERS {
			uint8_t											ui8A;																			/**< A     Accumulator. */
			uint8_t											ui8X;																			/**< X     Index Register X. */
			uint8_t											ui8Y;																			/**< Y     Index Register Y. */
			uint8_t											ui8S;																			/**< S     Stack Pointer. */
			union {
				uint8_t										ui8Pc[2];																		/**< PC    Program Counter. */
				uint16_t									ui16Pc;																			/**< PC    Program Counter. */
			};
			
			uint8_t											ui8Status = 0;																	/**< The processor status register. */
		};

		typedef void (CCpu6502:: *							PfCycle)();																		/**< A function pointer for the functions that handle each cycle. */
		typedef void (CCpu6502:: *							PfTicks)();																		/**< A function pointer for the tick handlers. */

		/** An instruction. The micro-functions (pfHandler) that make up each cycle of each instruction are programmed to know what to do and can correctly pass the cycles without
		 *	using ui8TotalCycles or amAddrMode. This means pcName, ui8TotalCycles, and amAddrMode are only used for debugging, verification, printing things, etc.
		 * Since we are adding work by increasing the number of functions calls per instruction, we get that time back by not checking for addressing modes or referencing any other
		 *	tables or data.  For the sake of performance, each micro-function just knows what to do and does so in the most efficient manner possible, free from any unnecessary
		 *	branching etc.
		 * pfHandler points to an array of functions that can handle all possible cycles for a given instruction, and we use a subtractive process for eliminating optional cycles
		 *	rather than using the additive approach most commonly found in emulators.
		 */
		struct LSN_INSTR {
			PfCycle											pfHandler[LSN_M_MAX_INSTR_CYCLE_COUNT*2];										/**< Indexed by LSN_CPU_CONTEXT::ui8FuncIdx, these functions handle each cycle of the instruction. */
			uint8_t											ui8TotalCycles;																	/**< Total non-optional non-overlapping cycles in the instruction. Used only for debugging, disassembling, etc. */
			LSN_ADDRESSING_MODES							amAddrMode;																		/**< Addressing mode. Used only for debugging, disassembling, etc. */
			uint8_t											ui8Size;																		/**< Size in bytes of the instruction. Used only for debugging, disassembling, etc. */
			LSN_INSTRUCTIONS								iInstruction;																	/**< The instruction. */
		};


		// == Functions.
		/**
		 * Resets the CPU to a known state.
		 */
		void												ResetToKnown() {
			ResetAnalog();
			std::memset( &m_rRegs, 0, sizeof( m_rRegs ) );
			m_rRegs.ui8S = 0x0;
			m_ui64CycleCount = 0ULL;
			m_ui8Operand = 0;

			m_ui16DmaCounter = 0;
			m_ui16DmaAddress = 0;

			m_ui8DmaPos = m_ui8DmaValue = 0;
			m_bNmiStatusLine = false;
			m_bLastNmiStatusLine = false;
			m_bDetectedNmi = false;
			m_bHandleNmi = false;
			m_bIrqStatusLine = false;
			m_bIrqSeenLowPhi2 = false;
			m_bIrqStatusPhi1Flag = false;
			m_bHandleIrq = false;
			m_bRdyLow = false;

			std::memset( m_ui8Inputs, 0, sizeof( m_ui8Inputs ) );
			std::memset( m_ui8InputsState, 0, sizeof( m_ui8InputsState ) );
			std::memset( m_ui8InputsPoll, 0, sizeof( m_ui8InputsPoll ) );
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void												ResetAnalog() {
			m_pfTickFunc = m_pfTickFuncCopy = &CCpu6502::Tick_NextInstructionStd;
			m_bBoundaryCrossed = false;
			m_ui16PcModify = 0;
			m_ui8SModify = 0;
			m_ui16OpCode = 0;
			
#ifdef LSN_CPU_VERIFY
			m_bAllowWritingToPc = true;
			m_bIsReset = false;
#else
			m_bAllowWritingToPc = false;
			m_bIsReset = true;
#endif	// #ifdef LSN_CPU_VERIFY
		}

		/**
		 * Performs a single PHI1 update.
		 */
		virtual void										Tick();

		/**
		 * Performs a single PHI2 update.
		 **/
		virtual void										TickPhi2();

		/**
		 * Applies the CPU's memory mapping t the bus.
		 */
		void												ApplyMemoryMap();

		/**
		 * Begins a DMA transfer.
		 * 
		 * \param _ui8Val The value written to 0x4014.
		 */
		void												BeginDma( uint8_t _ui8Val );

		/**
		 * Notifies the class that an NMI has occurred.
		 */
		virtual void										Nmi();

		/**
		 * Clears the NMI flag.
		 */
		virtual void										ClearNmi();

		/**
		 * Signals an IRQ to be handled before the next instruction.
		 */
		virtual void										Irq();

		/**
		 * Clears the IRQ flag.
		 */
		virtual void										ClearIrq();

		/**
		 * Gets the status of the IRQ line.
		 * 
		 * \return Returns true if the IRQ status line is low.
		 **/
		virtual bool										GetIrqStatus() const;


#ifdef LSN_CPU_VERIFY
		/**
		 * Runs a test given a JSON's value representing the test to run.
		 *
		 * \param _jJson The JSON file.
		 * \param _jvTest The test to run.
		 * \return Returns true if te test succeeds, false otherwise.
		 */
		bool												RunJsonTest( lson::CJson &_jJson, const lson::CJsonContainer::LSON_JSON_VALUE &_jvTest );
#endif	// #ifdef LSN_CPU_VERIFY

		/**
		 * Sets the input poller.
		 *
		 * \param _pipPoller The input poller pointer.
		 */
		void												SetInputPoller( CInputPoller * _pipPoller ) {
			m_pipPoller = _pipPoller;
		}

		/**
		 * Sets the mapper.
		 *
		 * \param _pmbMapper A pointer to the mapper to be ticked with each CPU cycle.
		 */
		void												SetMapper( CMapperBase * _pmbMapper ) {
			m_pmbMapper = _pmbMapper;
		}


	protected :
		// == Members.
		PfTicks												m_pfTickFunc = nullptr;																/**< The current tick function (called by Tick()). */
		PfTicks												m_pfTickFuncCopy = nullptr;															/**< A copy of the current tick, used to restore the intended original tick when control flow is changed by DMA transfers. */
		CInputPoller *										m_pipPoller = nullptr;																/**< The input poller. */
		CMapperBase *										m_pmbMapper = nullptr;																/**< The mapper, which gets ticked on each CPU cycle. */
		LSN_REGISTERS										m_rRegs;																			/**< Registers. */

		LSN_VECTORS											m_vBrkVector = LSN_V_IRQ_BRK;														/**< The vector to use inside BRK and whether to push B with status. */
		uint8_t												m_ui8Operand;																		/**< The operand. */
		union {
			uint8_t											m_ui8Address[2];																	/**< An address loaded into memory before transfer to a register such as PC. */
			uint16_t										m_ui16Address;																		/**< An address loaded into memory before transfer to a register such as PC. */
		};
		union {
			uint8_t											m_ui8Pointer[2];																	/**< An address loaded into memory for indirect access. */
			uint16_t										m_ui16Pointer;																		/**< An address loaded into memory for indirect access. */
		};
		uint16_t											m_ui16OpCode = 0;																	/**< The current opcode. */
		uint16_t											m_ui16PcModify = 0;																	/**< The amount by which to modify PC during the next Phi1. */
		uint16_t											m_ui16DmaCounter = 0;																/**< DMA counter. */
		uint16_t											m_ui16DmaAddress = 0;																/**< The DMA address from which to start copying. */
		uint16_t											m_ui16DmaCpuAddress = 0;															/**< The last CPU read address when DMA starts. */
		uint8_t												m_ui8SModify = 0;																	/**< The amount by which to modify S during the next Phi1. */
		uint8_t												m_ui8FuncIndex = 0;																	/**< The function index. */
		uint8_t												m_ui8DmaPos = 0;																	/**< The DMA transfer offset.*/
		uint8_t												m_ui8DmaValue = 0;																	/**< The DMA transfer value.*/
		bool												m_bNmiStatusLine = false;															/**< The status line for NMI. */
		bool												m_bLastNmiStatusLine = false;														/**< THe last status line for NMI. */
		bool												m_bDetectedNmi = false;																/**< The edge detector for the PHI2 part of the cycle. */
		bool												m_bHandleNmi = false;																/**< Once an NMI edge is detected, this is set to indicate that it needs to be handled on the PHI1 of the next cycle. */
		bool												m_bIrqStatusLine = false;															/**< The status line for IRQ. */
		bool												m_bIrqSeenLowPhi2 = false;															/**< Set if m_bIrqStatusLine is low on PHI2. */
		bool												m_bIrqStatusPhi1Flag = false;														/**< Set on Phi1 if m_bIrqSeenLowPhi2 was set. */
		bool												m_bHandleIrq = false;																/**< Once the IRQ status line is detected as having triggered, this tells us to handle an IRQ on the next instruction. */
		bool												m_bIsReset = true;																	/**< Are we resetting? */
		
		//bool												m_bIsReadCycle = true;																/**< Is the current cycle a read? */
		bool												m_bBoundaryCrossed = false;															/**< Did we cross a page boundary? */
		bool												m_bPushB = false;																	/**< Push the B flag with the status byte? */
		bool												m_bAllowWritingToPc = true;															/**< Allow writing to PC? */
		bool												m_bTakeJump = true;																	/**< Determines if a branch is taken. */
		bool												m_bRdyLow = false;																	/**< When RDY is pulled low, reads inside opcodes abort the CPU cycle. */
		bool												m_bDmaGo = false;																	/**< Signals DMA to begin.  Set on the next read cycle after RDY goes low. */
		bool												m_bDmaRead = false;																	/**< Is DMA on a read cycle? */


		// Temporary input.
		uint8_t												m_ui8Inputs[8];
		uint8_t												m_ui8InputsState[8];
		uint8_t												m_ui8InputsPoll[8];
		
		static LSN_INSTR									m_iInstructionSet[256];																/**< The instruction set. */


#ifdef LSN_CPU_VERIFY
		// == Types.
		struct LSN_CPU_VERIFY_RAM {
			uint16_t										ui16Addr;
			uint8_t											ui8Value;
		};

		struct LSN_CPU_VERIFY_STATE {
			LSN_REGISTERS									cvrRegisters;
			std::vector<LSN_CPU_VERIFY_RAM>					vRam;
		};

		struct LSN_CPU_VERIFY_CYCLE {
			uint16_t										ui16Addr;
			uint8_t											ui8Value;
			bool											bRead;
		};

		struct LSN_CPU_VERIFY_OBJ {
			std::string										sName;
			LSN_CPU_VERIFY_STATE							cvsStart;
			LSN_CPU_VERIFY_STATE							cvsEnd;
			std::vector<LSN_CPU_VERIFY_CYCLE>				vCycles;
		};


		// == Functions.
		/**
		 * Given a JSON object and the value for the test to run, this loads the test and fills a LSN_CPU_VERIFY structure.
		 *
		 * \param _jJson The JSON file.
		 * \param _jvTest The test to run.
		 * \param _cvoTest The test structure to fill out.
		 * \return Returns true if the JSON data was successfully extracted and the test created.
		 */
		bool												GetTest( lson::CJson &_jJson, const lson::CJsonContainer::LSON_JSON_VALUE &_jvTest, LSN_CPU_VERIFY_OBJ &_cvoTest );

		/**
		 * Fills out a LSN_CPU_VERIFY_STATE structure given a JSON "initial" or "final" member.
		 *
		 * \param _jJson The JSON file.
		 * \param _jvState The bject member representing the state to load.
		 * \param _cvsState The state structure to fill.
		 * \return Returns true if the state was loaded.
		 */
		bool												LoadState( lson::CJson &_jJson, const lson::CJsonContainer::LSON_JSON_VALUE &_jvState, LSN_CPU_VERIFY_STATE &_cvsState );
#endif	// #ifdef LSN_CPU_VERIFY


		// == Functions.
		/** Fetches the next opcode and begins the next instruction. */
		inline void											Tick_NextInstructionStd();

		/** Performs a cycle inside an instruction. */
		inline void											Tick_InstructionCycleStd();

		/** The DMA cycles. */
		template <unsigned _uState, bool _bPhi2>
		void												Tick_Dma() {
#define LSN_GET									0
#define LSN_PUT									(LSN_GET ^ 1)
			// _uState == LSN_DMA_STATES
			// _bEven is true when the cycle index is odd (1, 3, 5, etc.)
			//
			// Triggered by Phi2 read, so can never happen between Phi1 nd Phi2.  Always begins on a Phi1.
			// Idle function doesn't need to track Phi1 or Phi2. m_bDmaGo is only set on Phi2, so the Phi can be determined then.
			//
			// (m_ui64CycleCount & 0x1) == LSN_GET is a "get" cycle.
			// (m_ui64CycleCount & 0x1) == LSN_PUT is a "put" cycle.
			//	When neither a get nor a put can happen, dummy read the address that allowed halting of the CPU for DMA.
			if constexpr ( _uState == LSN_DS_IDLE ) {
				m_bRdyLow = true;
				(this->*m_pfTickFuncCopy)();
				if ( m_bDmaGo ) {
					// _bPhi2 will always be true here since the CPU only performs reads on Phi2.
					// Although we move to the LSN_DS_READ_WRITE with a hard-coded Phi1, proper operations can be ensured via debugging.
					// The CPU is now stalled.  Begin the transfer.
					m_ui16DmaCounter = 256;
					m_ui8DmaPos = 0;
					m_bDmaRead = true;
					m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_READ_WRITE, false>;
				}
				else {
					m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_IDLE, !_bPhi2>;
				}
			}
			if constexpr ( _uState == LSN_DS_READ_WRITE ) {
				if constexpr ( _bPhi2 ) {
					if ( m_bDmaRead ) {
						if ( (m_ui64CycleCount & 0x1) == LSN_GET ) {
							// Read (get).
							m_ui8DmaValue = m_pbBus->Read( uint16_t( m_ui16DmaAddress + m_ui8DmaPos ) );
							m_bDmaRead = false;
						}
						else {
							// Have to wait for alignment.  Perform the dummy read.
							m_pbBus->Read( uint16_t( m_ui16DmaCpuAddress ) );
						}
						m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_READ_WRITE, !_bPhi2>;
					}
					else {
						if ( (m_ui64CycleCount & 0x1) == LSN_PUT ) {
							// Write (put).
							m_pbBus->Write( LSN_PR_OAMDATA, m_ui8DmaValue );
							if ( --m_ui16DmaCounter == 0 ) {
								// Done with the copy.  Move to the end state, which will "virtually" replay the halting cycle.
								m_bRdyLow = false;
								m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_END, !_bPhi2>;
							}
							else {
								++m_ui8DmaPos;
								m_bDmaRead = true;
								m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_READ_WRITE, !_bPhi2>;
							}
						}
						else {
							// Have to wait for alignment.  Perform the dummy read.
							m_pbBus->Read( uint16_t( m_ui16DmaCpuAddress ) );
							m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_READ_WRITE, !_bPhi2>;
						}
					}
				}
				else {
					m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_READ_WRITE, !_bPhi2>;
				}
			}
			if constexpr ( _uState == LSN_DS_END ) {
				// "Replay" the halting cycle by just performing the Phi2 read it made.
				// This is an adjustment from real hardware because actually going back and replaying the cycle again for
				//	real is extremely tricky and could incur a permanent performance cost to do properly.
				// Instead, that cycle already executed during the LSN_DS_IDLE, and rather than actually playing it back
				//	we simply have a do-nothing Phi1 and a repeat of that cycle's Phi2 read.
				if constexpr ( _bPhi2 ) {
					m_pbBus->Read( uint16_t( m_ui16DmaCpuAddress ) );
					m_pfTickFunc = m_pfTickFuncCopy;
				}
				else {
					m_pfTickFunc = &CCpu6502::Tick_Dma<LSN_DS_END, !_bPhi2>;
				}
			}
#undef LSN_PUT
#undef LSN_GET
		}

		/**
		 * Writing to 0x4014 initiates a DMA transfer.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL							Write4014( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			reinterpret_cast<CCpu6502 *>(_pvParm0)->BeginDma( _ui8Val );
		}

		/**
		 * Reading from 0x4016 gets the MSB of the current controller-1 state.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL							Read4016( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CCpu6502 * pcThis = reinterpret_cast<CCpu6502 *>(_pvParm0);
			_ui8Ret = (pcThis->m_ui8InputsState[0] & 0x80) != 0;
			pcThis->m_ui8InputsState[0] <<= 1;
		}

		/**
		 * Writing to 0x4016 puts bits on the controller-1 port.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL							Write4016( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CCpu6502 * pcThis = reinterpret_cast<CCpu6502 *>(_pvParm0);
			pcThis->m_ui8Inputs[0] = (pcThis->m_ui8Inputs[0] & 0b11111000) | (_ui8Val & 0b00000111);
			// Temp.
			if ( pcThis->m_pipPoller ) {
				pcThis->m_ui8InputsPoll[0] = pcThis->m_pipPoller->PollPort( 0 );
			}
			else {
				pcThis->m_ui8InputsPoll[0] = 0;
			}
			pcThis->m_ui8InputsState[0] = pcThis->m_ui8InputsPoll[0];

			pcThis->m_ui8Inputs[1] = (pcThis->m_ui8Inputs[1] & 0b11111000) | (_ui8Val & 0b00000111);
			pcThis->m_ui8InputsState[1] = pcThis->m_ui8InputsPoll[1];
		}

		/**
		 * Reading from 0x4016 gets the MSB of the current controller-2 state.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL							Read4017( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CCpu6502 * pcThis = reinterpret_cast<CCpu6502 *>(_pvParm0);
			_ui8Ret = (pcThis->m_ui8InputsState[1] & 0x80) != 0;
			pcThis->m_ui8InputsState[1] <<= 1;
		}

		/**
		 * Writing to 0x4017 puts bits on the controller-2 port.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL							Write4017( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CCpu6502 * pcThis = reinterpret_cast<CCpu6502 *>(_pvParm0);
			pcThis->m_ui8Inputs[1] = (pcThis->m_ui8Inputs[1] & 0b11111000) | (_ui8Val & 0b00000111);
			pcThis->m_ui8InputsState[1] = pcThis->m_ui8InputsPoll[1];
		}

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// CYCLES
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/** Performs an add-with-carry with an operand, setting flags C, N, V, and Z. */
		void												Adc();

		/** Performs an add-with-carry with an operand, setting flags C, N, V, and Z. */
		void												AdcAndIncPc();

		/** Adds X to m_ui8Address[0]. */
		void												AddAddressAndX();

		/** Adds X to m_ui8Address[0]. */
		void												AddAddressAndXIncPc();

		/** Adds address to Y. */
		void												AddAddressAndY();

		/** Adds address to Y. */
		void												AddAddressAndYAndIncPc();

		/** Fixes high byte of address after adding address to Y. */
		void												AddAddressAndXorYHigh();

		/** Adds X and m_ui8Operand, stores to m_ui16Pointer, reads from m_ui8Operand. */
		void												AddXAndOperandToPointer();

		/** Performs A = A & OP.  Sets flags C, N, and Z, increases PC. */
		void												AncAndIncPc();

		/** Performs A = A & OP.  Sets flags N and Z. */
		void												And();

		/** Performs A = A & OP.  Sets flags N and Z. */
		void												AndAndIncPc();

		/** Performs A = (A | CONST) & X & OP.  Sets flags N and Z. */
		void												AneAndIncPc();

		/** Performs A = A & OP; A = (A >> 1) | (C << 7).  Sets flags C, V, N and Z. */
		void												ArrAndIncPc();

		/** Performs M <<= 1.  Sets C, N, and V. */
		void												Asl();

		/** Performs M <<= 1.  Sets C, N, and V, increases PC. */
		void												AslAndIncPc();

		/** Performs A <<= 1.  Sets C, N, and V. */
		void												AslOnA();

		/** Performs A &= OP; A >>= 1.  Sets flags C, N, and Z. */
		void												AsrAndIncPc();

		/** Sets flags N, V and Z according to a bit test. */
		void												Bit();

		/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
		template <unsigned _uBit, unsigned _uVal>
		void												Branch_Cycle2();

		/** 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
		void												Branch_Cycle2_Phi2();

		/** 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary. */
		void												Branch_Cycle3();

		/** 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary. */
		void												Branch_Cycle3_Phi2();

		/** 4th cycle of branch instructions. Page boundary was crossed. */
		void												Branch_Cycle4();

		/** Copies the vector address into PC. */
		void												Brk();

		/** Clears the carry bit. */
		void												Clc();

		/** Clears the decimal flag. */
		void												Cld();

		/** Clears the IRQ flag. */
		void												Cli();

		/** Clears the overflow flag. */
		void												Clv();

		/** Compares A with OP. */
		void												Cmp();

		/** Compares A with OP. */
		void												CmpAndIncPc();

		/** Compares X with OP. */
		void												Cpx();

		/** Compares X with OP. */
		void												CpxAndIncPc();

		/** Compares Y with OP. */
		void												Cpy();
		
		/** Compares Y with OP. */
		void												CpyAndIncPc();

		/** Copies Operand to Status without the B bit. */
		void												CopyOperandToStatusWithoutB_AdjustS();

		/** Fetches the low byte of the NMI/IRQ/BRK/reset vector into the low byte of PC. */
		void												CopyVectorPcl_Phi2();

		/** Fetches the high byte of the NMI/IRQ/BRK/reset vector into the high byte of PC and sets the I flag. */
		void												CopyVectorPch();

		/** Fetches the high byte of the NMI/IRQ/BRK/reset vector into the high byte of PC and sets the I flag. */
		void												CopyVectorPch_Phi2();

		/** Performs [ADDR]--; CMP(A).  Sets flags C, N, and Z. */
		void												Dcp();

		/** Performs [ADDR]--.  Sets flags N and Z. */
		void												Dec();

		/** Performs X--.  Sets flags N and Z. */
		void												Dex();

		/** Performs Y--.  Sets flags N and Z. */
		void												Dey();

		/** Performs A = A ^ OP.  Sets flags N and Z. */
		void												Eor();

		/** Performs A = A ^ OP.  Sets flags N and Z. */
		void												EorAndIncPc();

		/** Fetches the address and increments PC. */
		void												FetchAddressAndIncPc_Phi2();

		/** Fetches into m_ui8Address[1] and increments PC .*/
		void												FetchAddressHighAndIncPc_Phi2();

		/** Fetches the operand and increments PC. */
		void												FetchOpcodeAndIncPc();

		/** Fetches the operand. */
		void												FetchOperandAndDiscard_Phi2();

		/** Fetches the next opcode and does not the program counter. */
		void												FetchOperandDecPc_Phi2();

		/** Fetches the operand and increments PC. */
		void												FetchOperandAndIncPc_Phi2();

		/** Fetches a pointer and increments PC .*/
		void												FetchPointerAndIncPc_Phi2();

		/** Fetches into m_ui8Pointer[1] and increments PC .*/
		void												FetchPointerHighAndIncPc_Phi2();

		/** Performs OP++.  Sets flags N and Z. */
		void												Inc();

		/** Performs X++.  Sets flags N and Z. */
		void												Inx();

		/** Performs Y++.  Sets flags N and Z. */
		void												Iny();

		/** Performs M++; SBC.  Sets flags C, N, V, and Z. */
		void												Isb();

		/** Jams the machine. */
		void												Jam();

		/** Jams the machine. */
		void												Jam_Phi2();

		/** Copies m_ui16Address PC. */
		void												Jmp();

		/** Copies m_ui16Address into PC. */
		void												Jsr();

		/** Performs A = X = S = (OP & S).  Sets flags N and Z. */
		void												Las();

		/** Performs A = X = OP.  Sets flags N and Z. */
		void												Lax();

		/** Performs A = OP.  Sets flags N and Z. */
		void												Lda();

		/** Performs A = OP.  Sets flags N and Z. */
		void												LdaAndIncPc();

		/** Performs X = OP.  Sets flags N and Z. */
		void												Ldx();

		/** Performs X = OP.  Sets flags N and Z. */
		void												LdxAndIncPc();

		/** Performs Y = OP.  Sets flags N and Z. */
		void												Ldy();

		/** Performs Y = OP.  Sets flags N and Z. */
		void												LdyAndIncPc();

		/** Performs OP >>= 1.  Sets flags C, N, and Z. */
		void												Lsr();

		/** Performs A >>= 1.  Sets flags C, N, and Z. */
		void												LsrOnA();

		/** Performs A = X = (A | CONST) & OP.  Sets flags N and Z. */
		void												LxaAndIncPc();

		/** Adjusts S after stack operation. */
		void												NullAdjustS_Read();

		/** Adjusts S after stack operation. */
		void												NullAdjustS_Write();

		/** Increments PC. */
		void												NullIncPc_Read();

		/** Increments PC. */
		void												NullIncPc_Write();

		/** Does nothing. */
		void												Null_Read();

		/** Does nothing. */
		void												Null_Write();

		/** Performs A |= Operand with m_ui8Operand.  Sets flags N and Z. */
		void												Ora();

		/** Performs A |= Operand with m_ui8Operand.  Sets flags N and Z, increases PC. */
		void												OraAndIncPc();

		/** Sets m_ui8Operand to the status byte with Break and Reserved set. */
		void												Php();

		/** Pulls the accumulator. */
		void												Pla();

		/** Pulls the status byte, unsets X, sets M. */
		void												Plp();

		/** Copies the vector address into PC. */
		void												PrefetchNextOp();

		/** Pulls from the stack, stores in m_ui8Operand. */
		void												PullStackToOperand_Phi2();

		/** Pulls from the stack, stores in PC.l. */
		void												PullStackToPcl_Phi2();

		/** Pushes A. */
		void												PushA_Phi2();

		/** Pushes m_ui8Operand. */
		void												PushOperand_Phi2();

		/** Pushes PCH. */
		void												PushPch_Phi2();

		/** Pushes PCL. */
		void												PushPcl_Phi2();

		/** Pushes the status byte with B conditionally set. */
		void												PushStatus();

		/** Pushes the status byte with B conditionally set. */
		void												PushStatus_Phi2();

		/** Pushes X. */
		void												PushX_Phi2();

		/** Pushes Y. */
		void												PushY_Phi2();

		/** Adds X and m_ui8Operand, stores to m_ui16Pointer, reads from m_ui8Operand. */
		void												ReadOperandAndDiscard_Phi2();

		/** Reads from m_ui16Address and stores the result in m_ui8Operand. */
		void												ReadAddressToOperand_Phi2();

		/** Reads from m_ui16Address to m_ui8Operand, skips next cycle if m_ui8Pointer[1] == m_ui8Address[1]. */
		void												ReadAddressToOperand_BoundarySkip_Phi2();

		/** Reads from m_ui8Operand and stores in m_ui16Address. */
		void												ReadOperandToAddress_Phi2();

		/** Reads from m_ui8Operand and stores in m_ui8Operand. */
		void												ReadOperandToOperand_Phi2();

		/** Reads from m_ui16Pointer and stores the low byte in m_ui8Address[1]. */
		void												ReadPointerToAddressHigh_Phi2();

		/** Reads from m_ui16Pointer and stores the low byte in m_ui8Address[1]. */
		void												ReadPointerToAddressHigh_SamePage_Phi2();

		/** Reads from m_ui16Pointer and stores the low byte in m_ui8Address[0]. */
		void												ReadPointerToAddressLow_Phi2();

		/** Reads the stack, stores in m_ui8Operand. */
		void												ReadStackToOperand_Phi2();

		/** Reads from the stack, stores in PC.h. */
		void												ReadStackToPch_Phi2();

		/** Performs OP = (OP << 1) | (C); A = A & (OP).  Sets flags C, N and Z. */
		void												Rla();

		/** Performs OP = (OP << 1) | (C).  Sets flags C, N, and Z. */
		void												Rol();

		/** Performs A = (A << 1) | (C).  Sets flags C, N, and Z. */
		void												RolOnA();

		/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
		void												Ror();

		/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
		void												RorAndIncPc();

		/** Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z. */
		void												RorOnA();

		/** Performs OP = (OP >> 1) | (C << 7); A += OP + C.  Sets flags C, V, N and Z. */
		void												Rra();

		/** Writes (A & X) to m_ui16Address. */
		void												Sax_Phi2();

		/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
		void												Sbc();

		/** Performs A = A - OP + C.  Sets flags C, V, N and Z. */
		void												SbcAndIncPc();

		/** Performs X = (A & X) - OP.  Sets flags C, N and Z. */
		void												SbxAndIncPc();

		/** Sets the carry flag. */
		void												Sec();

		/** Sets the decimal flag. */
		void												Sed();

		/** Sets the IRQ flag. */
		void												Sei();

		/** Illegal. Stores A & X & (high-byte of address + 1) at the address. */
		void												Sha_Phi2();

		/** Illegal. Puts A & X into SP; stores A & X & (high-byte of address + 1) at the address. */
		void												Shs_Phi2();

		/** Illegal. Stores X & (high-byte of address + 1) at the address. */
		void												Shx_Phi2();

		/** Illegal. Stores Y & (high-byte of address + 1) at the address. */
		void												Shy_Phi2();

		/** Performs OP = (OP << 1); A = A | (OP).  Sets flags C, N and Z. */
		void												Slo();

		/** Performs OP = (OP >> 1); A = A ^ (OP).  Sets flags C, N and Z. */
		void												Sre();

		/** Copies A into X.  Sets flags N, and Z. */
		void												Tax();

		/** Copies A into Y.  Sets flags N, and Z. */
		void												Tay();

		/** Copies S into X. */
		void												Tsx();

		/** Copies X into A.  Sets flags N, and Z. */
		void												Txa();

		/** Copies X into S. */
		void												Txs();

		/** Copies Y into A.  Sets flags N, and Z. */
		void												Tya();

		/** Writes A to m_ui16Address. */
		void												WriteAToAddress_Phi2();

		/** Writes m_ui8Operand to m_ui16Address. */
		void												WriteOperandToAddress_Phi2();

		/** Writes X to m_ui16Address. */
		void												WriteXToAddress_Phi2();

		/** Writes Y to m_ui16Address. */
		void												WriteYToAddress_Phi2();

		/**
		 * Prepares to enter a new instruction.
		 */
		inline void											BeginInst();

		/**
		 * Performs an add-with-carry with an operand, setting flags C, N, V, and Z.
		 *
		 * \param _ui8RegVal The register value used in the comparison.
		 * \param _ui8OpVal The operand value used in the comparison.
		 */
		inline void											Adc( uint8_t &_ui8RegVal, uint8_t _ui8OpVal );

		/**
		 * Performs a compare against a register and an operand by setting flags.
		 *
		 * \param _ui8RegVal The register value used in the comparison.
		 * \param _ui8OpVal The operand value used in the comparison.
		 */
		inline void											Cmp( uint8_t _ui8RegVal, uint8_t _ui8OpVal );

		/**
		 * Performs an subtract-with-carry with an operand, setting flags C, N, V, and Z.
		 *
		 * \param _ui8RegVal The register value used in the comparison.
		 * \param _ui8OpVal The operand value used in the comparison.
		 */
		inline void											Sbc( uint8_t &_ui8RegVal, uint8_t _ui8OpVal );

		/**
		 * Performs a read of the next opcode at PC, checks signals to decide on going into NMI/IRQ/RESET, etc.
		 */
		inline void											PrefetchOpcode();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Fuctions.
	/** Fetches the next opcode and begins the next instruction. */
	inline void CCpu6502::Tick_NextInstructionStd() {
		// Expects the opcode to be in m_ui8Operand.
		FetchOpcodeAndIncPc();
	}

	/** Performs a cycle inside an instruction. */
	inline void CCpu6502::Tick_InstructionCycleStd() {
		(this->*m_iInstructionSet[m_ui16OpCode].pfHandler[m_ui8FuncIndex])();
	}

	/**
	 * Prepares to enter a new instruction.
	 *
	 * \param _ui16Op The instruction to begin executing.
	 */
	inline void CCpu6502::BeginInst() {
		// Enter normal instruction context.
		m_ui8FuncIndex = 0;
		m_pfTickFunc = m_pfTickFuncCopy = &CCpu6502::Tick_InstructionCycleStd;
		m_bBoundaryCrossed = false;
	}

	/**
	 * Performs an add-with-carry with an operand, setting flags C, N, V, and Z.
	 *
	 * \param _ui8RegVal The register value used in the comparison.
	 * \param _ui8OpVal The operand value used in the comparison.
	 */
	inline void CCpu6502::Adc( uint8_t &_ui8RegVal, uint8_t _ui8OpVal ) {
		uint16_t ui16Result = uint16_t( _ui8RegVal ) + uint16_t( _ui8OpVal ) + (m_rRegs.ui8Status & C());
		SetBit<V()>( m_rRegs.ui8Status, (~(uint16_t( _ui8RegVal ) ^ uint16_t( _ui8OpVal )) & (uint16_t( _ui8RegVal ) ^ ui16Result) & 0x0080) != 0 );
		_ui8RegVal = uint8_t( ui16Result );
		SetBit<C()>( m_rRegs.ui8Status, ui16Result > 0xFF );
		SetBit<Z()>( m_rRegs.ui8Status, _ui8RegVal == 0x00 );
		SetBit<N()>( m_rRegs.ui8Status, (_ui8RegVal & 0x80) != 0 );
	}

	/**
	 * Performs a compare against a register and an operand by setting flags.
	 *
	 * \param _ui8RegVal The register value used in the comparison.
	 * \param _ui8OpVal The operand value used in the comparison.
	 */
	inline void CCpu6502::Cmp( uint8_t _ui8RegVal, uint8_t _ui8OpVal ) {
		// If the value in the register is equal or greater than the compared value, the Carry will be set.
		SetBit<C()>( m_rRegs.ui8Status, _ui8RegVal >= _ui8OpVal );
		// The equal (Z) and negative (N) flags will be set based on equality or lack thereof...
		SetBit<Z()>( m_rRegs.ui8Status, _ui8RegVal == _ui8OpVal );
		// ...and the sign (IE A>=$80) of the register.
		SetBit<N()>( m_rRegs.ui8Status, ((_ui8RegVal - _ui8OpVal) & 0x80) != 0 );
	}

	/**
	 * Performs an subtract-with-carry with an operand, setting flags C, N, V, and Z.
	 *
	 * \param _ui8RegVal The register value used in the comparison.
	 * \param _ui8OpVal The operand value used in the comparison.
	 */
	inline void CCpu6502::Sbc( uint8_t &_ui8RegVal, uint8_t _ui8OpVal ) {
		uint16_t ui16Val = uint16_t( _ui8OpVal ) ^ 0x00FF;
		uint16_t ui16Result = uint16_t( _ui8RegVal ) + (ui16Val) + (m_rRegs.ui8Status & C());
		SetBit<V()>( m_rRegs.ui8Status, ((uint16_t( _ui8RegVal ) ^ ui16Result) & (ui16Val ^ ui16Result) & 0x0080) != 0 );
		_ui8RegVal = uint8_t( ui16Result );
		SetBit<C()>( m_rRegs.ui8Status, ui16Result > 0xFF );
		SetBit<Z()>( m_rRegs.ui8Status, _ui8RegVal == 0x00 );
		SetBit<N()>( m_rRegs.ui8Status, (_ui8RegVal & 0x80) != 0 );
	}

	/**
	 * Performs a read of the next opcode at PC, checks signals to decide on going into NMI/IRQ/RESET, etc.
	 */
	inline void CCpu6502::PrefetchOpcode() {
		/*if ( m_ui64CycleCount >= 87666 - 6 ) {
			volatile int uiuiu = 0;
		}*/
		LSN_INSTR_START_PHI2_READ( m_rRegs.ui16Pc, m_ui8Operand );
		
		if ( m_bHandleNmi || m_bHandleIrq || m_bIsReset ) {
			m_ui8Operand = 0;
			m_ui16PcModify = 0;
			m_bAllowWritingToPc = false;
		}
		else {
			m_ui16PcModify = 1;
		}
	}

}	// namespace lsn
