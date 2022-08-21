/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
 *
 * Notes:
 *	The LSN_SF_BREAK flag is not stored in the CPU registers, it is only used when pushing to
 *	the stack inside PushStatusAndBAndSetAddressByIrq().
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <vector>

namespace lsn {

	/**
	 * Class CCpu6502
	 * \brief Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 *
	 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
	 */
	class CCpu6502 {
	public :
		// == Various constructors.
		CCpu6502( class CBus * _pbBus );
		~CCpu6502();


		// == Enumerations.
		/** Status flags. */
		enum class LSN_STATUS_FLAGS : uint8_t {
			LSN_SF_CARRY					= 1 << 0,										/**< Carry         (0=No Carry, 1=Carry) */
			LSN_SF_ZERO						= 1 << 1,										/**< Zero          (0=Nonzero, 1=Zero) */
			LSN_SF_IRQ						= 1 << 2,										/**< IRQ Disable   (0=IRQ Enable, 1=IRQ Disable) */
			LSN_SF_DECIMAL					= 1 << 3,										/**< Decimal Mode  (0=Normal, 1=BCD Mode for ADC/SBC opcodes) */
			LSN_SF_BREAK					= 1 << 4,										/**< Break Flag    (0=IRQ/NMI, 1=RESET or BRK/PHP opcode) */
			LSN_SF_OVERFLOW					= 1 << 6,										/**< Overflow      (0=No Overflow, 1=Overflow) */
			LSN_SF_NEGATIVE					= 1 << 7,										/**< Negative/Sign (0=Positive, 1=Negative) */
		};

		/** Special addresses. */
		enum class LSN_VECTORS : uint16_t {
			LSN_V_NMI						= 0xFFFA,										/**< The address of execution during an NMI interrupt. */
			LSN_V_RESET						= 0xFFFC,										/**< The address of execution during a reset. */
			LSN_V_IRQ_BRK					= 0xFFFE,										/**< The address of execution during an IRQ or BRK interrupt. */
		};


		// == Functions.
		/**
		 * Resets the CPU to a known state.
		 */
		void								ResetToKnown();

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void								ResetAnalog();

		/**
		 * DESC
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return RETURN
		 */
		void								Irq();

		/**
		 * DESC
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return RETURN
		 */
		void								Nmi();


	protected :
		// == Enumerations.
		/** Addressing modes. */
		enum LSN_ADDRESSING_MODES : uint8_t {
			LSN_AM_IMPLIED,																	/**< Implied addressing = 1 extra cycle. */
			LSN_AM_ACCUMULATOR,																/**< Accumulator = 1 extra cycle. */
			LSN_AM_IMMEDIATE,																/**< Immediate = 1 extra cycle. */
			LSN_AM_ZERO_PAGE,																/**< Zero-page = 2 extra cycles. */
			LSN_AM_ZERO_PAGE_X,																/**< Zero-page-X = 3 extra cycles. */
			LSN_AM_ZERO_PAGE_Y,																/**< Zero-page-Y = 3 extra cycles. */
			LSN_AM_RELATIVE,																/**< Relative jumping = 2 extra cycles. */
			LSN_AM_ABSOLUTE,																/**< Absolute addressing = 3 extra cycles. */
			LSN_AM_ABSOLUTE_X,																/**< Absolute X addressing = 3 extra cycles + optional cycle. */
			LSN_AM_ABSOLUTE_Y,																/**< Absolute Y addressing = 3 extra cycles + optional cycle. */
			LSN_AM_INDIRECT_X,																/**< Indirect X addressing = 5 extra cycles. */
			LSN_AM_INDIRECT_Y,																/**< Indirect Y addressing = 4 extra cycles + optional cycle. */
		};

		/** Other parameters. */
		enum LSN_MISC {
			LSN_M_MAX_INSTR_CYCLE_COUNT		= 8,
		};

		/** Contexts. */
		enum LSN_CONTEXTS : uint8_t {
			LSN_C_NORMAL																	/**< Normal processing.  Execute the current cycle of the current instruction or move on to the next instruction. */

		};

		
		// == Types.
		/** Data for the current context of execution (most often just regular instruction execution). */
		struct LSN_CPU_CONTEXT {
			union {
				uint16_t					ui16Address;									/**< For various access types. */
				uint8_t						ui8Bytes[2];
			}								a;
			union {
				uint16_t					ui16JmpTarget;
				uint8_t						ui8Bytes[2];
			}								j;
			LSN_CONTEXTS					cContext;										/**< The context for whatever is currently being processed. */
			uint8_t							ui8Cycle;										/**< The per-instruction cycle count. */
			uint8_t							ui8FuncIdx;										/**< The index of the next function to call in the instruction. */
			uint8_t							ui8OpCode;										/**< The current opcode. */
			uint8_t							ui8Operand;										/**< The operand and low byte of addresses. */
			uint8_t							ui8Pointer;										/**< For indirect indexed access. */
		};

		/** A function pointer for the functions that handle each cycle. */
		typedef void (CCpu6502:: *			PfCycle)();

		/** An instruction. The micro-functions (pfHandler) that make up each cycle of each instruction are programmed to know what to do and can correctly pass the cycles without
		 *	using ui8TotalCycles or amAddrMode. This means pcName, ui8TotalCycles, and amAddrMode are only used for debugging, verification, printing things, etc.
		 * Since we are adding work by increasing the number of functions calls per instruction, we get that time back by not checking for addressing modes or referencing any other
		 *	tables or data.  For the sake of performance, each micro-function just knows what to do and does so in the most efficient manner possible, free from any unnecessary
		 *	branching etc.
		 * pfHandler points to an array of functions that can handle all possible cycles for a given instruction, and we use a subtractive process for eliminating optional cycles
		 *	rather than using the additive approach most commonly found in emulators.
		 */
		struct LSN_INSTR {
			const char *					pcName;											/**< Name of the instruction. */
			const char8_t *					putf8Desc;										/**< Description for the instruction. */
			PfCycle							pfHandler[LSN_M_MAX_INSTR_CYCLE_COUNT];			/**< Indexed by LSN_CPU_CONTEXT::ui8FuncIdx, these functions handle each cycle of the instruction. */
			uint8_t							ui8TotalCycles;									/**< Total non-optional non-overlapping cycles in the instruction. Used only for debugging, disassembling, etc. */
			LSN_ADDRESSING_MODES			amAddrMode;										/**< Addressing mode. Used only for debugging, disassembling, etc. */
			uint8_t							ui8Size;										/**< Size in bytes of the instruction. Used only for debugging, disassembling, etc. */
		};


		// == Members.
		class CBus *						m_pbBus;										/**< Pointer to the bus. */
		std::vector<LSN_CPU_CONTEXT>		m_vContextStack;								/**< Stack of contexts. */
		union {
			uint16_t						PC;												/**< Program counter. */
			uint8_t							ui8Bytes[2];
		}									pc;
		uint8_t								A;												/**< Accumulator. */
		uint8_t								X;												/**< Index register X. */
		uint8_t								Y;												/**< Index register Y. */
		uint8_t								S;												/**< Stack pointer (addresses 0x0100 + S). */
		uint8_t								m_ui8Status;									/**< The status flags. */
		LSN_CPU_CONTEXT *					m_pccCurContext;								/**< Always points to the top of the stack but it is set as sparsely as possible so as to avoid recalculatig it each cycle. */
		static LSN_INSTR					m_iInstructionSet[256];							/**< The instruction set. */
		
		
		// == Functions.


		// == Cycle functions.
		/** Fetches the next opcode and increments the program counter. */
		void								FetchOpcodeAndIncPc();							// Cycle 1 (always).
		/** Reads the next instruction byte and throws it away. */
		void								ReadNextInstByteAndDiscard();					// Cycle 2.
		/** Reads the next instruction byte and throws it away. */
		void								ReadNextInstByteAndDiscardAndIncPc();			// Cycle 2.
		/** Fetches a value using immediate addressing. */
		void								FetchValueAndIncPc_Imm();						// Cycle 2.
		/** Fetches a pointer and increments PC .*/
		void								FetchPointerAndIncPc();							// Cycle 2.
		/** Fetches an 8-bit address for Zero-Page dereferencing and increments PC. Stores the address in LSN_CPU_CONTEXT::a.ui16Address. */
		void								FetchAddressAndIncPc_Zp();						// Cycle 2.
		/** Fetches the low address and writes the value to the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
		void								FetchLowAddrByteAndIncPc_WriteImm();			// Cycle 2.
		/** Fetches the low address value for absolute addressing but does not write the value to LSN_CPU_CONTEXT::a.ui16Address yet.  Pair with FetchHighAddrByteAndIncPc(). */
		void								FetchLowAddrByteAndIncPc();						// Cycle 2.
		/** Fetches the high address value for absolute/indirect addressing. */
		void								FetchHighAddrByteAndIncPc();					// Cycle 3.
		/** Fetches the high address value for absolute/indirect addressing.  Adds Y to the low byte of the resulting address. */
		void								FetchHighAddrByteAndIncPcAndAddY();				// Cycle 3.
		/** Fetches the high address value for absolute/indirect addressing.  Adds X to the low byte of the resulting address. */
		void								FetchHighAddrByteAndIncPcAndAddX();				// Cycle 3.
		/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
		void								ReadFromEffectiveAddress_Abs();					// Cycle 4.
		/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
		void								ReadFromEffectiveAddress_Zp();					// Cycles 2, 4.
		/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores the result into LSN_CPU_CONTEXT::ui8Pointer.  Preceded by FetchPointerAndIncPc(). */
		void								ReadAtPointerAddress();							// Cycle 3.
		/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores (LSN_CPU_CONTEXT::ui8Pointer+X)&0xFF into LSN_CPU_CONTEXT::a.ui16Address.  Preceded by FetchPointerAndIncPc(). */
		void								ReadFromAddressAndAddX_ZpX();
		/** Reads from LSN_CPU_CONTEXT::a.ui16Address+Y, stores the result into LSN_CPU_CONTEXT::ui8Operand. */
		void								AddYAndReadAddress_IndY();						// Cycle 5.
		/** 3rd cycle of JSR. */
		void								Jsr_Cycle3();									// Cycle 3.
		/** 4th cycle of JSR. */
		void								Jsr_Cycle4();									// Cycle 4.
		/** 5th cycle of JSR. */
		void								Jsr_Cycle5();									// Cycle 5.
		/** 3rd cycle of PLA/PLP/RTI/RTS. */
		void								PLA_PLP_RTI_RTS_Cycle3();						// Cycle 3.
		/** Pushes PCH. */
		void								PushPch();										// Cycle 3.
		/** Pushes PCH, sets the B flag, and decrements S. */
		void								PushPchWithBFlag();								// Cycle 3.
		/** Pushes PCL, decrements S. */
		void								PushPcl();										// Cycle 4.
		/** Pushes status with B. */
		void								PushStatusAndBAndSetAddressByIrq();				// Cycle 5.
		/** Pushes status an decrements S. */
		void								PushStatus();									// Cycle 5.
		/** Pushes status without B. */
		void								PushStatusAndSetAddressByIrq();					// Cycle 5.
		/** Pulls status, ignoring B. */
		void								PullStatusWithoutB();							// Cycle 4.
		/** Pulls status. */
		void								PullStatus();									// Cycle 4.
		/** Pulls PCL. */
		void								PullPcl();										// Cycle 5.
		/** Pulls PCH. */
		void								PullPch();										// Cycle 6.
		/** Reads from LSN_CPU_CONTEXT::ui8Pointer, adds X to it, stores the result in LSN_CPU_CONTEXT::ui8Pointer. */
		void								ReadAddressAddX_IzX();							// Cycle 3.
		/** Reads the low byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+X, store in the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
		void								FetchEffectiveAddressLow_IzX();				// Cycle 4.
		/** Reads the high byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+X+1, store in the high byte of LSN_CPU_CONTEXT::a.ui16Address. */
		void								FetchEffectiveAddressHigh_IzX();				// Cycle 5.
		/** Reads the low byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer, store in the low byte of LSN_CPU_CONTEXT::a.ui16Address. */
		void								FetchEffectiveAddressLow_IzY();				// Cycle 3.
		/** Reads the high byte of the effective address using LSN_CPU_CONTEXT::ui8Pointer+1, store in the high byte of LSN_CPU_CONTEXT::a.ui16Address, adds Y. */
		void								FetchEffectiveAddressHigh_IzY();				// Cycle 4.
		/** Reads from the effective address (LSN_CPU_CONTEXT::a.ui16Address), which may be wrong if a page boundary was crossed.  If so, fixes the high byte of LSN_CPU_CONTEXT::a.ui16Address. */
		void								ReadEffectiveAddressFixHighByte_IzY_AbX();			// Cycle 5.
		/** Pops the low byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the low byte of PC. */
		void								CopyVectorPcl();								// Cycle 6.
		/** Fetches the low byte of PC from $FFFE. */
		void								FetchPclFromFFFE();								// Cycle 6.
		/** Fetches the high byte of PC from $FFFF. */
		void								FetchPclFromFFFF();								// Cycle 7.
		/** Writes the operand value back to the effective address stored in LSN_CPU_CONTEXT::a.ui16Address. */
		void								WriteValueBackToEffectiveAddress();				// Cycle 5.
		/** Writes the operand value back to the effective address stored in LSN_CPU_CONTEXT::a.ui16Address&0xFF. */
		void								WriteValueBackToEffectiveAddress_Zp();			// Cycle 5.
		/** 2nd cycle of branch instructions. Reads the operand (JMP offset). */
		void								Branch_Cycle2();
		/** 3rd cycle of branch instructions. Fetches opcode of next instruction and performs the check to decide which cycle comes next (or to end the instruction). */
		template <unsigned _uBit, unsigned _uVal>
		void								Branch_Cycle3();
		/** 2nd cycle of branch instructions. Performs the check to decide which cycle comes next (or to end the instruction). */
		/*template <unsigned _uBit, unsigned _uVal>
		void								Branch_Cycle2();*/
		/** 3rd cycle of branch instructions. Branch was taken, so copy part of the address and check for a page boundary. */
		//void								Branch_Cycle3();
		/** 4th cycle of branch instructions. Branch was taken and crossed a page boundary, but PC is already up-to-date so read/discard/exit. */
		void								Branch_Cycle4();
		/** 5th cycle of branch instructions. Page boundary was crossed. */
		void								Branch_Cycle5();
		/** Performs m_pbBus->CpuWrite( m_pccCurContext->a.ui16Address, m_pccCurContext->ui8Operand ); and LSN_FINISH_INST;, which finishes Read-Modify-Write instructions. */
		void								FinalWriteCycle();

		/** Copies the read value into the low byte of PC after fetching the high byte.
		 */
		void								JMP_Abs();										// Cycle 3.
		/** Pops the high byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the high byte of PC. */
		void								BRK();
		/** Pops into PCH. */
		void								RTI();
		/** Clears the carry flag. */
		void								CLC();
		/** Sets the carry flag. */
		void								SEC();
		/** Reads the next instruction byte and throws it away. */
		void								NOP();
		/** No operation. */
		void								NOP_AbX_1();
		/** No operation. */
		void								NOP_AbX_2();
		/** Reads the next instruction byte and throws it away, increasing PC. */
		void								NopAndIncPc();
		/** Reads LSN_CPU_CONTEXT::a.ui16Address and throws it away. */
		void								NOP_IzX_ZpX_AbX_Zp_Abs();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
		void								ORA_IzX();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
		void								ORA_IzY_AbY_1();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
		void								ORA_IzY_AbY_2();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
		void								ORA_Zp();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z. */
		void								ORA_ZpX();
		/** Fetches from PC and performs A = A | OP.  Sets flags N and Z. */
		void								ORA_Imm();
		/** Fetches from PC and performs A = A | OP.  Sets flags N and Z. */
		void								ORA_Abs();
		/** Sets flags N, V and Z according to a bit test. */
		void								BIT_Zp_Abs();
		/** Fetches from PC and performs A = A & OP.  Sets flags N and Z. */
		void								ANC_Imm();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z. */
		void								AND_IzX_ZpX_Zp_Abs();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z. */
		void								AND_IzY_AbX_AbY_1();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A & OP.  Sets flags N and Z. */
		void								AND_IzY_AbX_AbY_2();
		/** Fetches from PC and performs A = A & OP.  Sets flags N and Z. */
		void								AND_Imm();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A ^ OP.  Sets flags N and Z. */
		void								EOR_IzX_ZpX_Zp_Abs();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A ^ OP.  Sets flags N and Z. */
		void								EOR_IzY_AbX_AbY_1();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A ^ OP.  Sets flags N and Z. */
		void								EOR_IzY_AbX_AbY_2();
		/** Fetches from PC and performs A = A ^ OP.  Sets flags N and Z. */
		void								EOR_Imm();
		/** A zero-page ASL (Arithmetic Shift Left).  Sets flags C, N, and Z. */
		void								ASL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs();
		/** An ASL (Arithmetic Shift Left).  Sets flags C, N, and Z. */
		void								ASL_Imp();
		/** Pushes the status byte. */
		void								PHP();
		/** Pulls the status byte. */
		void								PLP();
		/** Performs A = (A << 1) | (A >> 7).  Sets flags C, N, and Z. */
		void								ROL_Imp();
		/** Performs OP = (OP << 1) | (OP >> 7).  Sets flags C, N, and Z. */
		void								ROL_IzX_IzY_ZpX_AbX_AbY_Zp_Abs();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs OP = (OP << 1); A = A | (OP).  Sets flags N and Z.  This is the first cycle of the function, which performs only the "OP = (OP << 1)" part. */
		void								SLO_IzX_IzY_ZpX_AbX_AbY_Zp_Abs();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs OP = (OP << 1) | (OP >> 7); A = A | (OP).  Sets flags N and Z.  This is the first cycle of the function, which performs only the "OP = (OP << 1)" part. */
		void								RLA_IzX_IzY_ZpX_AbX_AbY_Zp_Abs();
		/** JSR (Jump to Sub-Routine). */
		void								JSR();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

}	// namespace lsn
