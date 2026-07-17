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


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Input/LSNInputPoller.h"
#include "../Mappers/LSNMapperBase.h"
#include "../System/LSNSystemBase.h"
#include "../System/LSNTickable.h"
#include "LSNCpuBase.h"

#include <vector>

#ifdef LSN_CPU_VERIFY
#include "LSONJson.h"
#endif	// #ifdef LSN_CPU_VERIFY


#define LSN_INSTR_START_PHI1( ISREAD )						if constexpr ( ISREAD ) { if LSN_UNLIKELY( _pcCpu->m_bRdyLow || _pcCpu->m_bDmcDma ) { _pcCpu->BackupState(); } } if LSN_LIKELY( !_pcCpu->m_bRdyLow ) { ++_pcCpu->m_ui8RdyOffCnt; }
#define LSN_INSTR_END_PHI1									
#define LSN_INSTR_START_PHI2_READ( ADDR, RESULT )			RESULT = _pcCpu->m_pbBus->Read( uint16_t( ADDR ) );																						\
															if LSN_UNLIKELY( _pcCpu->m_fsStateBackup.bCopiedState/*_pcCpu->m_bRdyLow*/ ) { _pcCpu->m_ui16DmaCpuAddress = uint16_t( ADDR );			\
																_pcCpu->m_bDmaGo = _pcCpu->m_bRdyLow;																								\
																_pcCpu->m_bDmcGo = _pcCpu->m_bDmcDma;																								\
																_pcCpu->RestoreState();																														\
																/*static int64_t Cnt = 0; if ( ++Cnt >= 1000 ) { __debugbreak(); }*/																\
																return; }
#define LSN_INSTR_START_PHI2_WRITE( ADDR, VAL )				_pcCpu->m_pbBus->Write( uint16_t( ADDR ), uint8_t( VAL ) )
#define LSN_INSTR_END_PHI2									//if LSN_LIKELY( !_pcCpu->m_bRdyLow ) { ++_pcCpu->m_ui8RdyOffCnt; }

#define LSN_NEXT_FUNCTION_BY( AMT )							_pcCpu->m_fsState.ui8FuncIndex += uint8_t( AMT )
#define LSN_NEXT_FUNCTION									LSN_NEXT_FUNCTION_BY( 1 )
#define LSN_FINISH_INST( CHECK_INTERRUPTS )					if constexpr ( CHECK_INTERRUPTS ) { LSN_CHECK_INTERRUPTS; } LSN_NEXT_FUNCTION

//#define LSN_CHECK_INTERRUPTS								if ( !(_pcCpu->m_fsState.rRegs.ui8Status & I()) ) { _pcCpu->m_bHandleIrq = _pcCpu->m_bIrqStatusPhi1Flag; } _pcCpu->m_bHandleNmi |= _pcCpu->m_bNmiStatusPhi1Flag
#define LSN_CHECK_INTERRUPTS								if ( !(_pcCpu->m_fsState.rRegs.ui8Status & I()) ) { _pcCpu->m_bHandleIrq = _pcCpu->m_bIrqStatusPhi1Flag; } _pcCpu->m_bHandleNmi |= _pcCpu->m_bDetectedNmi

#define LSN_PUSH( VAL )										LSN_INSTR_START_PHI2_WRITE( (0x100 | uint8_t( _pcCpu->m_fsState.rRegs.ui8S + _i8SOff )), (VAL) ); _pcCpu->m_fsState.ui8SModify = uint8_t( -1L + _i8SOff )
#define LSN_POP( RESULT )									LSN_INSTR_START_PHI2_READ( (0x100 | uint8_t( _pcCpu->m_fsState.rRegs.ui8S + _i8SOff )), (RESULT) ); _pcCpu->m_fsState.ui8SModify = uint8_t( 1 + _i8SOff )

#define LSN_UPDATE_PC										if LSN_LIKELY( _pcCpu->m_fsState.bAllowWritingToPc ) { _pcCpu->m_fsState.rRegs.ui16Pc += _pcCpu->m_fsState.ui16PcModify; } _pcCpu->m_fsState.ui16PcModify = 0
#define LSN_UPDATE_S										_pcCpu->m_fsState.rRegs.ui8S += _pcCpu->m_fsState.ui8SModify; _pcCpu->m_fsState.ui8SModify = 0

#define LSN_R												true
#define LSN_W												false

#define LSN_TO_A											true
#define LSN_TO_P											false
#define LSN_FROM_A											true
#define LSN_FROM_P											false

#ifdef LSN_CPU_VERIFY
//#define LSN_CYCLES_DOC										1
#endif	// #ifdef LSN_CPU_VERIFY
#ifdef LSN_CYCLES_DOC
#define LSN_PRINT_STACK																																			\
	if ( int8_t( _pcCpu->m_fsState.ui8SModify ) < 0 ) { lsn::DebugA( ("Dec. S by " + std::to_string( -int8_t( _pcCpu->m_fsState.ui8SModify ) ) + ". ").c_str() ); }				\
	else if ( int8_t( _pcCpu->m_fsState.ui8SModify ) > 0 ) { lsn::DebugA( ("Inc. S by " + std::to_string( int8_t( _pcCpu->m_fsState.ui8SModify ) ) + ". ").c_str() ); }
#define LSN_PRINT_PC																																			\
	if ( int16_t( _pcCpu->m_fsState.ui16PcModify ) < 0 ) { lsn::DebugA( "Dec. PC. " ); }																				\
	else if ( int16_t( _pcCpu->m_fsState.ui16PcModify ) > 0 ) { lsn::DebugA( "Inc. PC. " ); }
#else
#define LSN_PRINT_STACK
#define LSN_PRINT_PC
#endif	// #ifdef LSN_CYCLES_DOC


namespace lsn {

#pragma warning( push )
#pragma warning( disable : 4324 )	// warning C4324: 'lsn::CCpu6502::LSN_FULL_STATE': structure was padded due to alignment specifier

	/**
	 * Class CCpu6502
	 * \brief Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 *
	 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 */
	class CCpu6502 : public CTickable, public CCpuBase {
	public :
		// == Various constructors.
		CCpu6502( CCpuBus * _pbBus, CSystemBase * _psbSystem );
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
			LSN_DS_DUMMY,																													/**< DMC dummy cycle. */
			LSN_DS_READ_WRITE,																												/**< Reading and writing. */
			//LSN_DS_END,																														/**< Emulates repeating of the last-attempted read cycle by issuing a dummy Phi1 and then reading the DMA CPU address during Phi2, then sets the tick pointers back to normal operation. */
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

		typedef void (*										PfCycle)( CCpu6502 * );															/**< A function pointer for the functions that handle each cycle. */
		typedef void (*										PfTicks)( CCpu6502 * );															/**< A function pointer for the tick handlers. */

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
			const char *									pcTypeString;																	/**< The type string of the instruction. */
		};

		/** Instruction data for assembly/disassembly. */
		struct LSN_INSTR_META_DATA {
			const char *									pcName;																			/**< The name of the instruction. */
			const char8_t *									putf8Desc;																		/**< The instruction description. */
		};


		// == Functions.
		/**
		 * Resets the CPU to a known state.
		 */
		void												ResetToKnown() {
			ResetAnalog();
			std::memset( &m_fsState.rRegs, 0, sizeof( m_fsState.rRegs ) );
			m_fsState.rRegs.ui8S = 0x0;
			m_ui64CycleCount = 0ULL;
			m_fsState.ui8Operand = 0;

			m_ui16DmaCounter = 0;
			m_ui16DmaAddress = 0;

			m_ui8DmaPos = m_ui8DmaValue = 0;
			m_bNmiStatusLine = false;
			m_bLastNmiStatusLine = false;
			m_bDetectedNmi = false;
			//m_bNmiStatusPhi1Flag = false;
			m_bHandleNmi = false;
			m_ui8IrqStatusLine = 0;
			m_bIrqSeenLowPhi2 = false;
			m_bIrqStatusPhi1Flag = false;
			m_bHandleIrq = false;
			m_bRdyLow = false;
			m_ui8RdyOffCnt = 0;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void												ResetAnalog() {
			m_pfTickFunc = m_pfTickFuncCopy = &CCpu6502::Tick_NextInstructionStd;
			m_fsState.bBoundaryCrossed = false;
			m_fsState.ui16PcModify = 0;
			m_fsState.ui8SModify = 0;
			m_fsState.ui16OpCode = 0;
			m_fsStateBackup.bCopiedState = false;
			
#ifdef LSN_CPU_VERIFY
			m_fsState.bAllowWritingToPc = true;
			m_bIsReset = m_bBrkIsReset = false;
#else
			m_fsState.bAllowWritingToPc = false;
			m_bIsReset = m_bBrkIsReset = true;
#endif	// #ifdef LSN_CPU_VERIFY

			m_fsState.pfCurInstruction = m_iInstructionSet[m_fsState.ui16OpCode].pfHandler;

			if ( m_pmbMapper ) { m_pmbMapper->Reset(); }
		}

		/**
		 * Sets m_bIsReset to true.
		 **/
		void												Reset() {
			m_bIsReset = m_bBrkIsReset = true;
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
		 * Begins an OAM DMA transfer.
		 * 
		 * \param _ui8Val The value written to 0x4014.
		 */
		void												BeginOamDma( uint8_t _ui8Val );

		/**
		 * Begins a DMC DMA transfer.
		 * 
		 * \param _bIsRestart true if this is fetching the next byte of an ongoing sample, false if it's the first byte of a new sample transfer.
		 */
		virtual void										BeginDmcDma( bool _bIsRestart );

		/**
		 * Begins a DMC DMA transfer.
		 */
		//void												BeginDmcDma();

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
		 *
		 * \param _ui8Source The source of the IRQ.
		 */
		virtual void										Irq( uint8_t _ui8Source );

		/**
		 * Clears the IRQ flag.
		 *
		 * \param _ui8Source The source of the IRQ.
		 */
		virtual void										ClearIrq( uint8_t _ui8Source );

		/**
		 * Gets the status of the IRQ line.
		 *
		 * \param _ui8Source The source of the IRQ.
		 * \return Returns true if the IRQ status line is low.
		 **/
		virtual bool										GetIrqStatus( uint8_t _ui8Source ) const;


#ifdef LSN_CPU_VERIFY
		/**
		 * Runs a test given a JSON's value representing the test to run.
		 *
		 * \param _jJson The JSON file.
		 * \param _jvTest The test to run.
		 * \return Returns -1 on error, the number of cycles otherwise.
		 */
		int32_t												RunJsonTest( lson::CJson &_jJson, const lson::CJsonContainer::LSON_JSON_VALUE &_jvTest );

		/**
		 * Gets the instruction table.
		 * 
		 * \return Returns a constant pointer to the instruction table (256 entries).
		 **/
		static inline const LSN_INSTR *						InstrTable() { return m_iInstructionSet; }
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


		// == Members.
		static const LSN_INSTR_META_DATA					m_smdInstMetaData[LSN_I_TOTAL];														/**< Metadata for the instructions (for assembly and disassembly etc.) */


	protected :
		// == Types.
		/** The full state structure for instructions. */
		LSN_ALIGN( 64 )
		struct LSN_FULL_STATE {
			const PfCycle *									pfCurInstruction = nullptr;															/**< The current instruction being executed. */
			LSN_REGISTERS									rRegs;
			LSN_VECTORS										vBrkVector = LSN_V_IRQ_BRK;															/**< The vector to use inside BRK and whether to push B with status. */

			union {
				uint8_t										ui8Address[2];																		/**< An address loaded into memory before transfer to a register such as PC. */
				uint16_t									ui16Address;																		/**< An address loaded into memory before transfer to a register such as PC. */
			};
			union {
				uint8_t										ui8Pointer[2];																		/**< An address loaded into memory for indirect access. */
				uint16_t									ui16Pointer;																		/**< An address loaded into memory for indirect access. */
			};
			union {
				uint8_t										ui8Target[2];																		/**< When an address needs to be updated with L and H separately. */
				uint16_t									ui16Target;																			/**< When an address needs to be updated with L and H separately. */
			};
			uint16_t										ui16OpCode = 0;																		/**< The current opcode. */
			uint16_t										ui16PcModify = 0;																	/**< The amount by which to modify PC during the next Phi1. */
			uint8_t											ui8Operand;																			/**< The operand. */
			uint8_t											ui8SModify = 0;																		/**< The amount by which to modify S during the next Phi1. */
			uint8_t											ui8FuncIndex = 0;																	/**< The function index. */
			bool											bAllowWritingToPc = true;															/**< Allow writing to PC? */
			bool											bTakeJump = true;																	/**< Determines if a branch is taken. */
			bool											bBoundaryCrossed = false;															/**< Did we cross a page boundary? */
			bool											bPushB = false;																		/**< Push the B flag with the status byte? */
			bool											bCopiedState = false;																/**< If m_bRdyLow triggers a state copy, this is set in PHI1 after the copy and used in PHI2 to know that a copy was made and to abord PHI2 as soon as the read address has been finalized. */
		};


		// == Members.
		PfTicks												m_pfTickFunc = nullptr;																/**< The current tick function (called by Tick()). */
		PfTicks												m_pfTickFuncCopy = nullptr;															/**< A copy of the current tick, used to restore the intended original tick when control flow is changed by DMA transfers. */
		PfTicks												m_pfOamDmaFuncs[2]{};																/**< OAM DMA function backups when DMC DMA is detected. */
		PfTicks												m_pfDmcDmaFuncs[2]{};																/**< DMC DMA function backups. */
		CInputPoller *										m_pipPoller = nullptr;																/**< The input poller. */
		CMapperBase *										m_pmbMapper = nullptr;																/**< The mapper, which gets ticked on each CPU cycle. */
		CSystemBase *										m_psbSystem = nullptr;																/**< Pointer to the system.  Allows access to the APU

		//const PfCycle *										m_pfCurInstruction;
		//uint8_t												m_fsState.ui8FuncIndex = 0;																	/**< The function index. */
		LSN_FULL_STATE										m_fsState;																			/**< Everything a standard instruction-cycle function can modify.  Backed up at the start of the first DMA read cycle and restored at the end after the read address for that cycle has been calculated. */
		LSN_FULL_STATE										m_fsStateBackup;																	/**< The backup of the state for the cycle that first gets interrupted by DMA and is then executed at the end of DMA. */

		uint16_t											m_ui16DmaCounter = 0;																/**< DMA counter. */
		uint16_t											m_ui16DmaAddress = 0;																/**< The DMA address from which to start copying. */
		uint16_t											m_ui16DmaCpuAddress = 0;															/**< The last CPU read address when DMA starts. */

		uint8_t												m_ui8DmaPos = 0;																	/**< The DMA transfer offset.*/
		uint8_t												m_ui8DmaValue = 0;																	/**< The DMA transfer value.*/

		uint8_t												m_ui8RdyOffCnt = 0;																	/**< Keeps track of on which cycle the RDY flag goes low, relative to the start of an instruction. */
		
		bool												m_bNmiStatusLine = false;															/**< The status line for NMI. */
		bool												m_bLastNmiStatusLine = false;														/**< THe last status line for NMI. */
		bool												m_bDetectedNmi = false;																/**< The edge detector for the PHI2 part of the cycle. */
		//bool												m_bNmiStatusPhi1Flag = false;														/**< The delayed latch for NMI, */
		bool												m_bHandleNmi = false;																/**< Once an NMI edge is detected, this is set to indicate that it needs to be handled on the PHI1 of the next cycle. */
		uint8_t												m_ui8IrqStatusLine = 0;																/**< The status line for IRQ. */
		bool												m_bIrqSeenLowPhi2 = false;															/**< Set if m_bIrqStatusLine is low on PHI2. */
		bool												m_bIrqStatusPhi1Flag = false;														/**< Set on Phi1 if m_bIrqSeenLowPhi2 was set. */
		bool												m_bHandleIrq = false;																/**< Once the IRQ status line is detected as having triggered, this tells us to handle an IRQ on the next instruction. */
		bool												m_bIsReset = true;																	/**< Are we resetting? */
		bool												m_bBrkIsReset = true;																/**< Shadows m_bIsReset, but m_bIsReset gets unset in the middle of BRK, while this lasts the whole BRK. */
		
		bool												m_bRdyLow = false;																	/**< When RDY is pulled low, reads inside opcodes abort the CPU cycle. */
		bool												m_bDmcDma = false;																	/**< Halts the CPU for DMC DMA. */
		bool												m_bDmaGo = false;																	/**< Signals DMA to begin.  Set on the next read cycle after RDY goes low. */
		bool												m_bDmcGo = false;																	/**> Signals DMC DMA to begin.  Set on the next read cycle after m_bDmcDma is set to true. */
		bool												m_bDmaRead = false;																	/**< Is DMA on a read cycle? */
		bool												m_bDmcRead = false;																	/**< Is DMC on a read cycle? */
		bool												m_bDmcBusAccess = false;															/**< Did the DMC access the bus on this cycle? */
		
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
		/**
		 * Fetches the next opcode and begins the next instruction.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static inline void									Tick_NextInstructionStd( CCpu6502 * _pcCpu );

		/**
		 * Performs a cycle inside an instruction.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static inline void									Tick_InstructionCycleStd( CCpu6502 * _pcCpu );

		/**
		 * Converts an OAM DMA function pointer to an index.
		 * 
		 * \param _pfFunc The function pointer to convert.
		 * \return Returns an index representing the OAM DMA function pointer.
		 **/
		inline uint8_t										OamDmaFuncToIdx( PfCycle _pfFunc );

		/**
		 * Converts a DMC DMA function pointer to an index.
		 * 
		 * \param _pfFunc The function pointer to convert.
		 * \return Returns an index representing the DMC DMA function pointer.
		 **/
		inline uint8_t										DmcDmaFuncToIdx( PfCycle _pfFunc );

		/**
		 * Converts an index to an OAM DMA function pointer.
		 * 
		 * \param _u8Idx The index to convert.
		 * \return Returns the associated function pointer or nullptr.
		 **/
		inline PfCycle										IdxToOamFunc( uint8_t _u8Idx );

		/**
		 * Converts an index to a DMC DMA function pointer.
		 * 
		 * \param _u8Idx The index to convert.
		 * \return Returns the associated function pointer or nullptr.
		 **/
		inline PfCycle										IdxToDmcFunc( uint8_t _u8Idx );

		/** 
		 * The OAM DMA cycles.
		 * 
		 * \param _pcCpu The pointer to the CPU object.
		 */
		template <unsigned _uState, bool _bPhi2, bool _bCalledFromDmc = false>
		static void											Tick_OamDma( CCpu6502 * _pcCpu ) {
#define LSN_GET												1
#define LSN_PUT												(LSN_GET ^ 1)
#define LSN_SET_PTRS( STATE )															\
	_pcCpu->m_pfTickFunc = &CCpu6502::Tick_OamDma<STATE, !_bPhi2, _bCalledFromDmc>;				\
	_pcCpu->m_pfOamDmaFuncs[false] = &CCpu6502::Tick_OamDma<STATE, false, _bCalledFromDmc>;		\
	_pcCpu->m_pfOamDmaFuncs[true] = &CCpu6502::Tick_OamDma<STATE, true, _bCalledFromDmc>
			// _uState == LSN_DMA_STATES
			//
			// Triggered by Phi2 read, so can never happen between Phi1 nd Phi2.  Always begins on a Phi1.
			// Idle function doesn't need to track Phi1 or Phi2. m_bDmaGo is only set on Phi2, so the Phi can be determined then.
			//
			// (m_ui64CycleCount & 0x1) == LSN_GET is a "get" cycle.
			// (m_ui64CycleCount & 0x1) == LSN_PUT is a "put" cycle.
			//	When neither a get nor a put can happen, dummy read the address that allowed halting of the CPU for DMA.
			if constexpr ( !_bCalledFromDmc ) {
				if LSN_UNLIKELY( _pcCpu->m_pfDmcDmaFuncs[0] ) {
					_pcCpu->m_pfOamDmaFuncs[0] = &CCpu6502::Tick_OamDma<_uState, false, true>;
					_pcCpu->m_pfOamDmaFuncs[1] = &CCpu6502::Tick_OamDma<_uState, true, true>;

					// Call the DMC DMA function.
					(_pcCpu->m_pfDmcDmaFuncs[_bPhi2])( _pcCpu );
					return;
				}
			}

			if constexpr ( _uState == LSN_DS_IDLE ) {
				(_pcCpu->m_pfTickFuncCopy)( _pcCpu );

				if ( _pcCpu->m_bDmaGo ) {
					// _bPhi2 will always be true here since the CPU only performs reads on Phi2.
					// Although we move to the LSN_DS_READ_WRITE with a hard-coded Phi1, proper operations can be ensured via debugging.
					// The CPU is now stalled.  Begin the transfer.
					_pcCpu->m_ui16DmaCounter = 256;
					_pcCpu->m_ui8DmaPos = 0;
					_pcCpu->m_bDmaRead = true;

					LSN_SET_PTRS( LSN_DS_READ_WRITE );

					// This is the halt cycle.
				}
				else {
					LSN_SET_PTRS( LSN_DS_IDLE );
				}
			}
			if constexpr ( _uState == LSN_DS_READ_WRITE ) {
				// If m_bDmcBusAccess, that means this is being called from the DMC DMA routine, and the DMC DMA routine performed a memory access,
				//	consuming the single available read/write for this cycle.  We have to skip our read/write, re-align, and try again.
				bool bAccessBus;
				if constexpr ( !_bPhi2 ) {
					bAccessBus = false;
				}
				else {
					bAccessBus = !_pcCpu->m_bDmcBusAccess;
				}
				if LSN_LIKELY( bAccessBus ) {
					if ( _pcCpu->m_bDmaRead ) {
						if ( (_pcCpu->m_ui64CycleCount & 0x1) == LSN_GET ) {
							// Read (get).
							_pcCpu->m_ui8DmaValue = _pcCpu->m_pbBus->Read( uint16_t( _pcCpu->m_ui16DmaAddress + _pcCpu->m_ui8DmaPos ) );
							_pcCpu->m_bDmaRead = false;
						}
						else {
							// Have to wait for alignment.  Perform the dummy read.
							_pcCpu->m_pbBus->Read( uint16_t( _pcCpu->m_ui16DmaCpuAddress ) );
						}
						LSN_SET_PTRS( LSN_DS_READ_WRITE );
					}
					else {
						if ( (_pcCpu->m_ui64CycleCount & 0x1) == LSN_PUT ) {
							// Write (put).
							_pcCpu->m_pbBus->Write( LSN_PR_OAMDATA, _pcCpu->m_ui8DmaValue );
							if ( --_pcCpu->m_ui16DmaCounter == 0 ) {
								// Done with the copy.  Move to the end state, which will "virtually" replay the halting cycle.
								_pcCpu->m_bRdyLow = false;
								_pcCpu->m_bDmaGo = false;
								if ( _pcCpu->m_pfDmcDmaFuncs[0] ) {		// Still DMC DMA going?
									_pcCpu->m_pfTickFunc = _pcCpu->m_pfDmcDmaFuncs[!_bPhi2];
								}
								else {
									_pcCpu->m_pfTickFunc = _pcCpu->m_pfTickFuncCopy;
								}

								_pcCpu->m_pfOamDmaFuncs[0] = nullptr;
								_pcCpu->m_pfOamDmaFuncs[1] = nullptr;
							}
							else {
								++_pcCpu->m_ui8DmaPos;
								_pcCpu->m_bDmaRead = true;
								LSN_SET_PTRS( LSN_DS_READ_WRITE );
							}
						}
						else {
							// Have to wait for alignment.  Perform the dummy read.
							_pcCpu->m_pbBus->Read( uint16_t( _pcCpu->m_ui16DmaCpuAddress ) );
							LSN_SET_PTRS( LSN_DS_READ_WRITE );
						}
					}
				}
				else {
					LSN_SET_PTRS( LSN_DS_READ_WRITE );
				}
			}

#undef LSN_SET_PTRS
#undef LSN_PUT
#undef LSN_GET
		}

		/**
		 * The DMC DMA cycles.
		 * 
		 * \param _pcCpu The pointer to the CPU object.
		 */
		template <unsigned _uState, bool _bPhi2, bool _bIsReload>
		static void											Tick_DmcDma( CCpu6502 * _pcCpu ) {
#define LSN_GET												1
#define LSN_PUT												(LSN_GET ^ 1)
#define LSN_SET_PTRS( STATE )															\
	_pcCpu->m_pfTickFunc = &CCpu6502::Tick_DmcDma<STATE, !_bPhi2, _bIsReload>;					\
	_pcCpu->m_pfDmcDmaFuncs[false] = &CCpu6502::Tick_DmcDma<STATE, false, _bIsReload>;			\
	_pcCpu->m_pfDmcDmaFuncs[true] = &CCpu6502::Tick_DmcDma<STATE, true, _bIsReload>
			
			_pcCpu->m_bDmcBusAccess = false;
			if constexpr ( _uState == LSN_DS_IDLE ) {
				if ( _pcCpu->m_pfOamDmaFuncs[0] ) {
					(_pcCpu->m_pfOamDmaFuncs[_bPhi2])( _pcCpu );
				}
				else {
					(_pcCpu->m_pfTickFuncCopy)( _pcCpu );
				}
				if ( _pcCpu->m_bDmcGo ) {
					if constexpr ( _bPhi2 ) {
						LSN_SET_PTRS( LSN_DS_DUMMY );
					}
					else {
						LSN_SET_PTRS( LSN_DS_IDLE );
					}
				}
				else {
					LSN_SET_PTRS( LSN_DS_IDLE );
				}
			}
			if constexpr ( _uState == LSN_DS_DUMMY ) {
				if constexpr ( _bPhi2 ) {
					_pcCpu->m_pbBus->Read( uint16_t( _pcCpu->m_ui16DmaCpuAddress ) );
					if ( _pcCpu->m_pfOamDmaFuncs[_bPhi2] ) {
						_pcCpu->m_bDmcBusAccess = true;
						(_pcCpu->m_pfOamDmaFuncs[_bPhi2])( _pcCpu );
						_pcCpu->m_bDmcBusAccess = false;
					}
				}
				else if ( _pcCpu->m_pfOamDmaFuncs[_bPhi2] ) {
					(_pcCpu->m_pfOamDmaFuncs[_bPhi2])( _pcCpu );
				}

				if constexpr ( _bPhi2 ) {
					// Finished the cycle.  Move to the copy state.
					LSN_SET_PTRS( LSN_DS_READ_WRITE );

					// We are trying to read.
					_pcCpu->m_bDmcRead = true;
				}
				else {
					LSN_SET_PTRS( LSN_DS_DUMMY );
				}
			}
			if constexpr ( _uState == LSN_DS_READ_WRITE ) {
				if constexpr ( _bPhi2 ) {
					if ( _pcCpu->m_bDmcRead && (_pcCpu->m_ui64CycleCount & 0x1) == LSN_GET ) {
						_pcCpu->m_bDmcBusAccess = true;
						uint16_t ui16DmcAddr = _pcCpu->m_psbSystem->DmcDmaAddress();
						auto ui8DmcValue = _pcCpu->m_pbBus->Read( ui16DmcAddr );
						
						_pcCpu->m_pfDmcDmaFuncs[0] = nullptr;
						_pcCpu->m_pfDmcDmaFuncs[1] = nullptr;

						_pcCpu->m_bDmcGo = false;
						_pcCpu->m_bDmcDma = false;
						_pcCpu->m_bDmcRead = false;

						if ( _pcCpu->m_pfOamDmaFuncs[_bPhi2] ) {
							(_pcCpu->m_pfOamDmaFuncs[_bPhi2])( _pcCpu );	// It will set the next function pointer.
						}
						else {
							_pcCpu->m_pfTickFunc = _pcCpu->m_pfTickFuncCopy;
						}

						_pcCpu->m_psbSystem->ReceiveDmcSample( ui8DmcValue );
						_pcCpu->m_bDmcBusAccess = false;
					}
					else {
						// Not a GET cycle.
						if ( _pcCpu->m_pfOamDmaFuncs[_bPhi2] ) {
							(_pcCpu->m_pfOamDmaFuncs[_bPhi2])( _pcCpu );	// It will set the next function pointer.
						}
						LSN_SET_PTRS( LSN_DS_READ_WRITE );
					}

					
				}
				else {
					// Nothing for us to do.
					if ( _pcCpu->m_pfOamDmaFuncs[_bPhi2] ) {
						(_pcCpu->m_pfOamDmaFuncs[_bPhi2])( _pcCpu );
					}
					LSN_SET_PTRS( LSN_DS_READ_WRITE );
				}
			}

#undef LSN_SET_PTRS
#undef LSN_PUT
#undef LSN_GET
		}

		/**
		 * Backs up the current state.
		 **/
		inline void											BackupState() {
			std::memcpy( &m_fsStateBackup, &m_fsState, sizeof( m_fsState ) );
			m_fsStateBackup.bCopiedState = true;
			//m_ui8RdyOffCnt = m_fsState.ui8FuncIndex;
		}

		/**
		 * Restores the state.
		 **/
		inline void											RestoreState() {
			std::memcpy( &m_fsState, &m_fsStateBackup, sizeof( m_fsState ) );
			m_fsStateBackup.bCopiedState = false;
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
			reinterpret_cast<CCpu6502 *>(_pvParm0)->BeginOamDma( _ui8Val );
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
			if LSN_LIKELY( pcThis->m_pipPoller ) {
				_ui8Ret = (_ui8Ret & 0b11100000) | (pcThis->m_pipPoller->Read( 0x4016 ) & 0b00011111);
			}
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
			if LSN_LIKELY( pcThis->m_pipPoller ) {
				pcThis->m_pipPoller->PollPort( _ui8Val );
			}
		}

		/**
		 * Reading from 0x4017 gets the MSB of the current controller-2 state.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL							Read4017( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CCpu6502 * pcThis = reinterpret_cast<CCpu6502 *>(_pvParm0);
			if LSN_LIKELY( pcThis->m_pipPoller ) {
				_ui8Ret = (_ui8Ret & 0b11100000) | (pcThis->m_pipPoller->Read( 0x4017 ) & 0b00011111);
			}
		}


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// CYCLES
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Performs an add-with-carry with an operand, setting flags C, N, V, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Adc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Adds X and m_fsState.ui8Operand, stores to either m_ui16Address or m_ui16Pointer.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr, bool _bRead = true, bool _bIncPc = false>
		static void											Add_XAndOperand_To_AddrOrPntr_8bit( CCpu6502 * _pcCpu );

		/**
		 * Adds X and m_ui16Pointer or m_ui16Address, stores to either m_ui16Address or m_ui16Pointer.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr, bool _bRead = true, bool _bIncPc = false>
		static void											Add_XAndPtrOrAddr_To_AddrOrPntr_8bit( CCpu6502 * _pcCpu );

		/**
		 * Adds X and either m_ui16Address.L or m_ui16Pointer.L, stores in either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr, bool _bRead = true, bool _bIncPc = false>
		static void											Add_XAndPtrOrAddr_To_AddrOrPtr( CCpu6502 * _pcCpu );

		/**
		 * Adds Y and either m_ui16Address.L or m_ui16Pointer.L, stores in either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr, bool _bRead = true, bool _bIncPc = false>
		static void											Add_YAndPtrOrAddr_To_AddrOrPtr( CCpu6502 * _pcCpu );

		/**
		 * Performs A = A & OP.  Sets flags C, N, and Z, increases PC.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Anc_IncPc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = A & OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											And_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = (A | CONST) & X & OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Ane_IncPc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = A & OP; A = (A >> 1) | (C << 7).  Sets flags C, V, N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Arr_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs M <<= 1.  Sets C, N, and V.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Asl( CCpu6502 * _pcCpu );

		/**
		 * Performs A <<= 1.  Sets C, N, and V.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											AslOnA_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A &= OP; A >>= 1.  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Asr_IncPc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Sets flags N, V and Z according to a bit test.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Bit_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * 1st cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction).
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <unsigned _uBit, unsigned _uVal>
		static void											Branch_Cycle1( CCpu6502 * _pcCpu );

		/**
		 * 1st cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction).
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Branch_Cycle1_Phi2( CCpu6502 * _pcCpu );

		/**
		 * 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction).
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Branch_Cycle2( CCpu6502 * _pcCpu );

		/**
		 * 2nd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction).
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Branch_Cycle2_Phi2( CCpu6502 * _pcCpu );

		/**
		 * 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Branch_Cycle3( CCpu6502 * _pcCpu );

		/**
		 * 3rd cycle of branch instructions. Branch was taken and might have crossed a page boundary.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Branch_Cycle3_Phi2( CCpu6502 * _pcCpu );

		/**
		 * 4th cycle of branch instructions. Page boundary was crossed.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Branch_Cycle4( CCpu6502 * _pcCpu );

		/**
		 * Final touches to BRK (copies m_ui16Address to m_fsState.rRegs.ui16Pc) and first cycle of the next instruction.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Brk_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Clears the carry bit.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Clc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Clears the decimal flag.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Cld_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Clears the IRQ flag.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Cli_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Clears the overflow flag.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Clv_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Compares A with OP.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Cmp_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Compares X with OP.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Cpx_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Compares Y with OP.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Cpy_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Copies m_fsState.ui8Operand to Status without the B bit.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											CopyOperandToStatusWithoutB( CCpu6502 * _pcCpu );

		/**
		 * Copies m_ui16Target to PC, optionally adjusts PC.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											CopyTargetToPc( CCpu6502 * _pcCpu );

		/**
		 * Copies from the vector to PC.h.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bEndInstr = false>
		static void											CopyVectorToPc_H_Phi2( CCpu6502 * _pcCpu );
			
		/**
		 * Copies from the vector to PC.l.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											CopyVectorToPc_L_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Performs [ADDR]--; CMP(A).  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Dcp( CCpu6502 * _pcCpu );

		/**
		 * Performs [ADDR]--.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Dec( CCpu6502 * _pcCpu );

		/**
		 * Performs X--.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Dex_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs Y--.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Dey_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = A ^ OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Eor_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Fetches the current opcode and increments PC.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Fetch_Opcode_IncPc_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Fetches the operand.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bEndInstr = false>
		static void											Fetch_Operand_Discard_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Fetches the operand and increments PC.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bEndInstr = false>
		static void											Fetch_Operand_IncPc_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Fetches the operand to either m_ui16Address.H or m_ui16Pointer.H and increments PC.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr, bool _bEndInstr = false>
		static void											Fetch_Operand_To_AddrOrPtr_H_IncPc_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Fetches the operand to either m_ui16Address or m_ui16Pointer and increments PC.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Fetch_Operand_To_AddrOrPtr_IncPc_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Uses m_ui16Target to fix the high byte of either m_ui16Address or m_ui16Pointer.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bFromAddr>
		static void											Fix_PtrOrAddr_To_AddrOrPtr_H( CCpu6502 * _pcCpu );

		/**
		 * Performs the Indirect Y add on the low byte.  m_ui16Address -> m_ui16Pointer or m_ui16Pointer -> m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bFromAddr>
		static void											IndirectYAdd_PtrOrAddr_To_AddrOrPtr( CCpu6502 * _pcCpu );

		/**
		 * Performs OP++.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Inc( CCpu6502 * _pcCpu );

		/**
		 * Performs X++.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Inx_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs Y++.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Iny_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs M++; SBC.  Sets flags C, N, V, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Isb( CCpu6502 * _pcCpu );

		/**
		 * Copies m_ui16Address into PC.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Jmp_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Copies m_ui16Address into PC, adjusts S.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Jsr_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = X = S = (OP & S).  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Las_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = X = OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Lax_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Lda_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs X = OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Ldx_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs Y = OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Ldy_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs OP >>= 1.  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Lsr( CCpu6502 * _pcCpu );

		/**
		 * Performs A >>= 1.  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											LsrOnA_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs A = X = (A | CONST) & OP.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Lxa_IncPc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Generic null operation.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bRead = true, bool _bIncPc = false, bool _bAdjS = false, bool _bBeginInstr = false>
		static void											Null( CCpu6502 * _pcCpu );

		/**
		 * Generic null operation for BRK that can be either a read or write, depending on RESET.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false, bool _bAdjS = false, bool _bBeginInstr = false>
		static void											Null_RorW( CCpu6502 * _pcCpu );

		/**
		 * Performs A |= Operand with m_fsState.ui8Operand.  Sets flags N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Ora_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Sets m_fsState.ui8Operand to the status byte with Break and Reserved set.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Php( CCpu6502 * _pcCpu );

		/**
		 * Pulls the accumulator: Copies m_fsState.ui8Operand to A.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Pla_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Pulls the status byte, unsets X, sets M.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Plp_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Pulls from the stack, stores in m_fsState.ui8Operand.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0>
		static void											Pull_To_Operand_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Pulls from the stack, stores in m_ui16Target.L.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0>
		static void											Pull_To_Target_L_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Pushes A.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0, bool _bEndInstr = false>
		static void											Push_A_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Pushes m_fsState.ui8Operand.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0, bool _bEndInstr = false>
		static void											Push_Operand_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Pushes PCh with the given S offset.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0>
		static void											Push_Pc_H_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Pushes PCl with the given S offset.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0>
		static void											Push_Pc_L_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Pushes Status with or without B/X to the given S offset.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0>
		static void											Push_S_Phi2( CCpu6502 * _pcCpu );

/**
		 * Reads a given hard-coded address, optionally moving to the next cycle or the previous cycle.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <uint16_t _ui16Addr, bool _bMoveBack>
		static void											ReadAddr_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads from m_fsState.ui8Operand, discards result.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Read_Operand_Discard_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads from either m_ui16Pointer or m_ui16Address and stores the low byte in either m_ui8Address[1] or m_ui8Pointer[1].
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bFromAddr, bool _bEndInstr = false>
		static void											Read_PtrOrAddr_To_AddrOrPtr_H_SamePage_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads either m_ui16Pointer or m_ui16Address and stores in either m_ui16Address.H or m_ui16Pointer.H.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bFromAddr>
		static void											Read_PtrOrAddr_To_AddrOrPtr_H_8Bit_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads either m_ui16Pointer or m_ui16Address and stores in either m_ui16Address.L or m_ui16Pointer.L.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bFromAddr>
		static void											Read_PtrOrAddr_To_AddrOrPtr_L_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads either m_ui16Pointer or m_ui16Address and stores in m_fsState.ui8Operand.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bFromAddr, bool _bEndInstr = false>
		static void											Read_PtrOrAddr_To_Operand_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads either m_ui16Pointer or m_ui16Address and stores in m_fsState.ui8Operand.  Skips a full cycle if m_fsState.bBoundaryCrossed is false (and only then is _bEndInstr checked).
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bFromAddr, bool _bEndInstr = false>
		static void											Read_PtrOrAddr_To_Operand_BoundarySkip_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads the stack, stores in m_fsState.ui8Operand.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bEndInstr = false>
		static void											Read_Stack_To_Operand_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Reads the stack, stores in m_ui16Target.H.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <int8_t _i8SOff = 0, bool _bEndInstr = false>
		static void											Read_Stack_To_Target_H_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Performs OP = (OP << 1) | (C); A = A & (OP).  Sets flags C, N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Rla( CCpu6502 * _pcCpu );

		/**
		 * Performs OP = (OP << 1) | (C).  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Rol( CCpu6502 * _pcCpu );

		/**
		 * Performs OP = (OP << 1) | (C).  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											RolOnA_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs OP = (OP >> 1) | (C << 7).  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Ror( CCpu6502 * _pcCpu );

		/**
		 * Performs A = (A >> 1) | (C << 7).  Sets flags C, N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											RorOnA_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs OP = (OP >> 1) | (C << 7); A += OP + C.  Sets flags C, V, N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Rra( CCpu6502 * _pcCpu );

		/**
		 * Copies m_ui16Target to PC, adjusts S.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Rti_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Adjusts PC and calls BeginInst().
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Rts_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Writes (A & X) to either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Sax_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Performs A = A - OP + C.  Sets flags C, V, N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false>
		static void											Sbc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs X = (A & X) - OP.  Sets flags C, N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Sbx_IncPc_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Sets the carry flag.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Sec_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Sets the decimal flag.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Sed_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Sets the IRQ flag.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Sei_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Selects the BRK vector etc.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bAdjS>
		static void											SelectBrkVectors( CCpu6502 * _pcCpu );

		/**
		 * Sets I and X.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											SetBrkFlags( CCpu6502 * _pcCpu );

		/**
		 * Illegal. Stores A & X & (high-byte of address + 1) at either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr, unsigned _uRdyCnt>
		static void											Sha_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Illegal. Puts A & X into SP; stores A & X & (high-byte of address + 1) at the address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Shs_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Illegal. Stores X & (high-byte of address + 1) at the address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Shx_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Illegal. Stores Y & (high-byte of address + 1) at the address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Shy_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Performs OP = (OP << 1); A = A | (OP).  Sets flags C, N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Slo( CCpu6502 * _pcCpu );

		/**
		 * Performs OP = (OP >> 1); A = A ^ (OP).  Sets flags C, N and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Sre( CCpu6502 * _pcCpu );

		/**
		 * Copies A into X.  Sets flags N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Tax_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Copies A into Y.  Sets flags N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Tay_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Copies S into X.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Tsx_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Copies X into A.  Sets flags N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Txa_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Copies Y into A.  Sets flags N, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Tya_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Copies X into S.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		static void											Txs_BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Writes A to either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Write_A_To_AddrOrPtr_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Writes m_fsState.ui8Operand to either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr, bool _bEndInstr = false>
		static void											Write_Operand_To_AddrOrPtr_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Writes X to either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Write_X_To_AddrOrPtr_Phi2( CCpu6502 * _pcCpu );

		/**
		 * Writes Y to either m_ui16Pointer or m_ui16Address.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bToAddr>
		static void											Write_Y_To_AddrOrPtr_Phi2( CCpu6502 * _pcCpu );
		

		/**
		 * Prepares to enter a new instruction.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 */
		template <bool _bIncPc = false, bool _bAdjS = false>
		static inline void									BeginInst( CCpu6502 * _pcCpu );

		/**
		 * Performs an add-with-carry with an operand, setting flags C, N, V, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 * \param _ui8RegVal The register value used in the comparison.
		 * \param _ui8OpVal The operand value used in the comparison.
		 */
		static inline void									Adc( CCpu6502 * _pcCpu, uint8_t &_ui8RegVal, uint8_t _ui8OpVal );

		/**
		 * Performs a compare against a register and an operand by setting flags.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 * \param _ui8RegVal The register value used in the comparison.
		 * \param _ui8OpVal The operand value used in the comparison.
		 */
		static inline void									Cmp( CCpu6502 * _pcCpu, uint8_t _ui8RegVal, uint8_t _ui8OpVal );

		/**
		 * Performs an subtract-with-carry with an operand, setting flags C, N, V, and Z.
		 *
		 * \param _pcCpu A pointer to the CCpu6502 instance.
		 * \param _ui8RegVal The register value used in the comparison.
		 * \param _ui8OpVal The operand value used in the comparison.
		 */
		static inline void									Sbc( CCpu6502 * _pcCpu, uint8_t &_ui8RegVal, uint8_t _ui8OpVal );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Fuctions.
	/**
	 * Fetches the next opcode and begins the next instruction.
	 *
	 * \param _pcCpu A pointer to the CCpu6502 instance.
	 */
	inline void CCpu6502::Tick_NextInstructionStd( CCpu6502 * _pcCpu ) {
		CCpu6502::BeginInst( _pcCpu );
	}

	/**
	 * Performs a cycle inside an instruction.
	 *
	 * \param _pcCpu A pointer to the CCpu6502 instance.
	 */
	inline void CCpu6502::Tick_InstructionCycleStd( CCpu6502 * _pcCpu ) {
		//(*CCpu6502::m_iInstructionSet[(*_pcCpu).m_fsState.ui16OpCode].pfHandler[(*_pcCpu).m_fsState.ui8FuncIndex])( _pcCpu );
		(*(*_pcCpu).m_fsState.pfCurInstruction[(*_pcCpu).m_fsState.ui8FuncIndex])( _pcCpu );
	}

	/**
	 * Converts an OAM DMA function pointer to an index.
	 * 
	 * \param _pfFunc The function pointer to convert.
	 * \return Returns an index representing the OAM DMA function pointer.
	 **/
	inline uint8_t CCpu6502::OamDmaFuncToIdx( PfCycle _pfFunc ) {
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_IDLE, false, false> ) { return 0; }
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_IDLE, false, true> ) { return 1; }
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_IDLE, true, false> ) { return 2; }
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_IDLE, true, true> ) { return 3; }
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, false, false> ) { return 4; }
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, false, true> ) { return 5; }
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, true, false> ) { return 6; }
		if ( _pfFunc == &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, true, true> ) { return 7; }
		return uint8_t( -1 );

		// Generated via:
		//OamStates = { "LSN_DS_IDLE", /*"LSN_DS_DUMMY", */"LSN_DS_READ_WRITE", };
		//FalseTrue = { "false", "true" };
		//PrintFunc = "\tinline uint8_t CCpu6502::OamDmaFuncToIdx( PfCycle _pfFunc ) {\r\n";
		//Idx = 0;
		//for ( S = 0; S < OamStates.size(); ++S ) {
		//	for ( Phi2 = 0; Phi2 < FalseTrue.size(); ++Phi2 ) {
		//		for ( FromDmc = 0; FromDmc < FalseTrue.size(); ++FromDmc ) {
		//			PrintFunc += "\t\tif ( _pfFunc == &CCpu6502::Tick_OamDma<{}, {}, {}> ) {{ return {}; }}\r\n".format(
		//				OamStates[S], FalseTrue[Phi2], FalseTrue[FromDmc], Idx++ );
		//		}
		//	}
		//}
		//PrintFunc += "\t\treturn uint8_t( -1 );\r\n";
		//PrintFunc += "\t}";
	}

	/**
	 * Converts a DMC DMA function pointer to an index.
	 * 
	 * \param _pfFunc The function pointer to convert.
	 * \return Returns an index representing the DMC DMA function pointer.
	 **/
	inline uint8_t CCpu6502::DmcDmaFuncToIdx( PfCycle _pfFunc ) {
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, false, false> ) { return 0; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, false, true> ) { return 1; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, true, false> ) { return 2; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, true, true> ) { return 3; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, false, false> ) { return 4; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, false, true> ) { return 5; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, true, false> ) { return 6; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, true, true> ) { return 7; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, false, false> ) { return 8; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, false, true> ) { return 9; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, true, false> ) { return 10; }
		if ( _pfFunc == &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, true, true> ) { return 11; }
		return uint8_t( -1 );

		// Generated via:
		//OamStates = { "LSN_DS_IDLE", "LSN_DS_DUMMY", "LSN_DS_READ_WRITE", };
		//FalseTrue = { "false", "true" };
		//PrintFunc = "\tinline uint8_t CCpu6502::DmcDmaFuncToIdx( PfCycle _pfFunc ) {\r\n";
		//Idx = 0;
		//for ( S = 0; S < OamStates.size(); ++S ) {
		//	for ( Phi2 = 0; Phi2 < FalseTrue.size(); ++Phi2 ) {
		//		for ( FromDmc = 0; FromDmc < FalseTrue.size(); ++FromDmc ) {
		//			PrintFunc += "\t\tif ( _pfFunc == &CCpu6502::Tick_DmcDma<{}, {}, {}> ) {{ return {}; }}\r\n".format(
		//				OamStates[S], FalseTrue[Phi2], FalseTrue[FromDmc], Idx++ );
		//		}
		//	}
		//}
		//PrintFunc += "\t\treturn uint8_t( -1 );\r\n";
		//PrintFunc += "\t}";
	}

	/**
	 * Converts an index to an OAM DMA function pointer.
	 * 
	 * \param _u8Idx The index to convert.
	 * \return Returns the associated function pointer or nullptr.
	 **/
	inline CCpu6502::PfCycle CCpu6502::IdxToOamFunc( uint8_t _u8Idx ) {
		if ( 0 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_IDLE, false, false>; }
		if ( 1 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_IDLE, false, true>; }
		if ( 2 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_IDLE, true, false>; }
		if ( 3 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_IDLE, true, true>; }
		if ( 4 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, false, false>; }
		if ( 5 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, false, true>; }
		if ( 6 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, true, false>; }
		if ( 7 == _u8Idx ) { return &CCpu6502::Tick_OamDma<LSN_DS_READ_WRITE, true, true>; }
		return nullptr;

		// Generated via:
		//OamStates = { "LSN_DS_IDLE", /*"LSN_DS_DUMMY", */"LSN_DS_READ_WRITE", };
		//FalseTrue = { "false", "true" };
		//PrintFunc = "\tinline CCpu6502::PfCycle CCpu6502::IdxToOamFunc( uint8_t _u8Idx ) {\r\n";
		//Idx = 0;
		//for ( S = 0; S < OamStates.size(); ++S ) {
		//	for ( Phi2 = 0; Phi2 < FalseTrue.size(); ++Phi2 ) {
		//		for ( FromDmc = 0; FromDmc < FalseTrue.size(); ++FromDmc ) {
		//			PrintFunc += "\t\tif ( {} == _u8Idx ) {{ return &CCpu6502::Tick_OamDma<{}, {}, {}>; }}\r\n".format(
		//				Idx++, OamStates[S], FalseTrue[Phi2], FalseTrue[FromDmc] );
		//		}
		//	}
		//}
		//PrintFunc += "\t\treturn nullptr;\r\n";
		//PrintFunc += "\t}";
	}

	/**
	 * Converts an index to a DMC DMA function pointer.
	 * 
	 * \param _u8Idx The index to convert.
	 * \return Returns the associated function pointer or nullptr.
	 **/
	inline CCpu6502::PfCycle CCpu6502::IdxToDmcFunc( uint8_t _u8Idx ) {
		if ( 0 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, false, false>; }
		if ( 1 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, false, true>; }
		if ( 2 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, true, false>; }
		if ( 3 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_IDLE, true, true>; }
		if ( 4 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, false, false>; }
		if ( 5 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, false, true>; }
		if ( 6 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, true, false>; }
		if ( 7 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_DUMMY, true, true>; }
		if ( 8 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, false, false>; }
		if ( 9 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, false, true>; }
		if ( 10 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, true, false>; }
		if ( 11 == _u8Idx ) { return &CCpu6502::Tick_DmcDma<LSN_DS_READ_WRITE, true, true>; }
		return nullptr;

		// Generated via:
		//OamStates = { "LSN_DS_IDLE", "LSN_DS_DUMMY", "LSN_DS_READ_WRITE", };
		//FalseTrue = { "false", "true" };
		//PrintFunc = "\tinline CCpu6502::PfCycle CCpu6502::IdxToDmcFunc( uint8_t _u8Idx ) {\r\n";
		//Idx = 0;
		//for ( S in OamStates ) {
		//	for ( Phi2 : FalseTrue ) {
		//		for ( FromDmc = 0; FromDmc < FalseTrue.size(); ++FromDmc ) {
		//			PrintFunc += "\t\tif ( {} == _u8Idx ) {{ return &CCpu6502::Tick_DmcDma<{}, {}, {}>; }}\r\n".format(
		//				Idx++, S, Phi2, FalseTrue[FromDmc] );
		//		}
		//	}
		//}
		//PrintFunc += "\t\treturn nullptr;\r\n";
		//PrintFunc += "\t}";
	}

	/**
	 * Prepares to enter a new instruction.
	 *
	 * \param _pcCpu A pointer to the CCpu6502 instance.
	 * \param _ui16Op The instruction to begin executing.
	 */
	template <bool _bIncPc, bool _bAdjS>
	inline void CCpu6502::BeginInst( CCpu6502 * _pcCpu ) {
		LSN_INSTR_START_PHI1( true );

		if constexpr ( _bIncPc ) {
			LSN_UPDATE_PC;
		}

		if constexpr ( _bAdjS ) {
			LSN_UPDATE_S;
		}
		// Enter normal instruction context.
		(*_pcCpu).m_fsState.ui8FuncIndex = 0;
		// TODO: Move this to Tick_NextInstructionStd().
		(*_pcCpu).m_pfTickFunc = (*_pcCpu).m_pfTickFuncCopy = &CCpu6502::Tick_InstructionCycleStd;
		(*_pcCpu).m_fsState.bBoundaryCrossed = false;
		(*_pcCpu).m_ui8RdyOffCnt = 0;
		LSN_INSTR_END_PHI1;
	}

	/**
	 * Performs an add-with-carry with an operand, setting flags C, N, V, and Z.
	 *
	 * \param _pcCpu A pointer to the CCpu6502 instance.
	 * \param _ui8RegVal The register value used in the comparison.
	 * \param _ui8OpVal The operand value used in the comparison.
	 */
	inline void CCpu6502::Adc( CCpu6502 * _pcCpu, uint8_t &_ui8RegVal, uint8_t _ui8OpVal ) {
		uint16_t ui16Result = uint16_t( _ui8RegVal ) + uint16_t( _ui8OpVal ) + ((*_pcCpu).m_fsState.rRegs.ui8Status & C());
		lsn::SetBit<V()>( (*_pcCpu).m_fsState.rRegs.ui8Status, (~(uint16_t( _ui8RegVal ) ^ uint16_t( _ui8OpVal )) & (uint16_t( _ui8RegVal ) ^ ui16Result) & 0x0080) != 0 );
		_ui8RegVal = uint8_t( ui16Result );
		lsn::SetBit<C()>( (*_pcCpu).m_fsState.rRegs.ui8Status, ui16Result > 0xFF );
		lsn::SetBit<Z()>( (*_pcCpu).m_fsState.rRegs.ui8Status, _ui8RegVal == 0x00 );
		lsn::SetBit<N()>( (*_pcCpu).m_fsState.rRegs.ui8Status, (_ui8RegVal & 0x80) != 0 );
	}

	/**
	 * Performs a compare against a register and an operand by setting flags.
	 *
	 * \param _pcCpu A pointer to the CCpu6502 instance.
	 * \param _ui8RegVal The register value used in the comparison.
	 * \param _ui8OpVal The operand value used in the comparison.
	 */
	inline void CCpu6502::Cmp( CCpu6502 * _pcCpu, uint8_t _ui8RegVal, uint8_t _ui8OpVal ) {
		lsn::SetBit<C()>( (*_pcCpu).m_fsState.rRegs.ui8Status, _ui8RegVal >= _ui8OpVal );
		lsn::SetBit<Z()>( (*_pcCpu).m_fsState.rRegs.ui8Status, _ui8RegVal == _ui8OpVal );
		lsn::SetBit<N()>( (*_pcCpu).m_fsState.rRegs.ui8Status, ((_ui8RegVal - _ui8OpVal) & 0x80) != 0 );
	}

	/**
	 * Performs an subtract-with-carry with an operand, setting flags C, N, V, and Z.
	 *
	 * \param _pcCpu A pointer to the CCpu6502 instance.
	 * \param _ui8RegVal The register value used in the comparison.
	 * \param _ui8OpVal The operand value used in the comparison.
	 */
	inline void CCpu6502::Sbc( CCpu6502 * _pcCpu, uint8_t &_ui8RegVal, uint8_t _ui8OpVal ) {
		uint16_t ui16Val = uint16_t( _ui8OpVal ) ^ 0x00FF;
		uint16_t ui16Result = uint16_t( _ui8RegVal ) + (ui16Val) + ((*_pcCpu).m_fsState.rRegs.ui8Status & C());
		lsn::SetBit<V()>( (*_pcCpu).m_fsState.rRegs.ui8Status, ((uint16_t( _ui8RegVal ) ^ ui16Result) & (ui16Val ^ ui16Result) & 0x0080) != 0 );
		_ui8RegVal = uint8_t( ui16Result );
		lsn::SetBit<C()>( (*_pcCpu).m_fsState.rRegs.ui8Status, ui16Result > 0xFF );
		lsn::SetBit<Z()>( (*_pcCpu).m_fsState.rRegs.ui8Status, _ui8RegVal == 0x00 );
		lsn::SetBit<N()>( (*_pcCpu).m_fsState.rRegs.ui8Status, (_ui8RegVal & 0x80) != 0 );
	}

#pragma warning( pop )

}	// namespace lsn
