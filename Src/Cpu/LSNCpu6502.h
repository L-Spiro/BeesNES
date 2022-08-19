/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Enough emulation of a Ricoh 6502 CPU to run a Nintendo Entertainment System.
 *
 * Notes:
 *	The LSN_SF_BREAK flag is not stored in the CPU registers, it is only used when pushing to
 *	the stack inside PushStatusAndB().
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
			PfCycle							pfHandler[LSN_M_MAX_INSTR_CYCLE_COUNT];			/**< Indexed by LSN_CPU_CONTEXT::ui8FuncIdx, these functions handle each cycle of the instruction. */
			uint8_t							ui8TotalCycles;									/**< Total non-optional non-overlapping cycles in the instruction. Used only for debugging, disassembling, etc. */
			LSN_ADDRESSING_MODES			amAddrMode;										/**< Addressing mode. Used only for debugging, disassembling, etc. */
		};


		// == Members.
		class CBus *						m_pbBus;										/**< Pointer to the bus. */
		std::vector<LSN_CPU_CONTEXT>		m_vContextStack;								/**< Stack of contexts. */
		uint16_t							PC;												/**< Program counter. */
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
		/** Fetches the low address value for absolute addressing. */
		void								FetchLowAddrByteAndIncPc();						// Cycle 2.
		/** Fetches the high address value for absolute/indirect addressing. */
		void								FetchHighAddrByteAndIncPc();					// Cycle 3.
		/** Fetches the high address value for absolute/indirect addressing.  Adds Y to the low byte of the resulting address. */
		void								FetchHighAddrByteAndIncPcAndAddY();				// Cycle 3.
		/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
		void								ReadFromEffectiveAddress_Abs();					// Cycle 4.
		/** Reads from the effective address.  The address is in LSN_CPU_CONTEXT::a.ui16Address.  The result is stored in LSN_CPU_CONTEXT::ui8Operand. */
		void								ReadFromEffectiveAddress_Zp();					// Cycles 2, 4.
		/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores the result into LSN_CPU_CONTEXT::ui8Pointer.  Preceded by FetchPointerAndIncPc(). */
		void								ReadAtPointerAddress();							// Cycle 3.
		/** Reads from LSN_CPU_CONTEXT::ui8Pointer, stores (LSN_CPU_CONTEXT::ui8Pointer+X)&0xFF into LSN_CPU_CONTEXT::a.ui16Address.  Preceded by FetchPointerAndIncPc(). */
		void								ReadFromAddressAndAddX_Zpx();
		/** Reads from LSN_CPU_CONTEXT::a.ui16Address+Y, stores the result into LSN_CPU_CONTEXT::ui8Operand. */
		void								AddYAndReadAddress_IndY();						// Cycle 5.
		/** Pushes PCH. */
		void								PushPch();										// Cycle 3.
		/** Pushes PCH, sets the B flag, and decrements S. */
		void								PushPchWithBFlagAndDecS();						// Cycle 3.
		/** Pushes PCL. */
		void								PushPcl();										// Cycle 4.
		/** Pushes PCL, decrements S. */
		void								PushPclAndDecS();								// Cycle 4.
		/** Pushes status with B. */
		void								PushStatusAndB();								// Cycle 5.
		/** Pushes status an decrements S. */
		void								PushStatusAndDecS();							// Cycle 5.
		/** Pushes status without B. */
		void								PushStatus();									// Cycle 5.
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
		void								ReadEffectiveAddressFixHighByte_IzY();			// Cycle 5.
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

		/** Copies the read value into the low byte of PC after fetching the high byte.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchLowAddrByteAndIncPc()
		 */
		void								JMP_Abs();										// Cycle 3.
		/** Pops the high byte of the NMI/IRQ/BRK/reset vector (stored in LSN_CPU_CONTEXT::a.ui16Address) into the high byte of PC.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	ReadNextInstByteAndDiscardAndIncPc
		 *	PushPchWithBFlagAndDecS
		 *	PushPclAndDecS
		 *	PushStatusAndDecS
		 *	FetchPclFromFFFE
		 */
		void								BRK();											// Cycle 7.
		/** Clears the carry flag. */
		void								CLC();
		/** Reads the next instruction byte and throws it away. */
		void								NOP();
		/** Reads the next instruction byte and throws it away, increasing PC. */
		void								NopAndIncPc();
		/** Reads LSN_CPU_CONTEXT::a.ui16Address and throws it away. */
		void								NOP_Abs();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchPointerAndIncPc
		 *	ReadAddressAddX_IzX
		 *	FetchEffectiveAddressLow_IzX
		 *	FetchEffectiveAddressHigh_IzX
		 */
		void								ORA_IzX();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchPointerAndIncPc
		 *	FetchEffectiveAddressLow_IzY
		 *	FetchEffectiveAddressHigh_IzY
		 */
		void								ORA_IzY_1();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchPointerAndIncPc
		 *	FetchEffectiveAddressLow_IzY
		 *	FetchEffectiveAddressHigh_IzY
		 *	ORA_IzY_1
		 */
		void								ORA_IzY_2();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchLowAddrByteAndIncPc
		 *	FetchHighAddrByteAndIncPcAndAddY
		 *	FetchEffectiveAddressHigh_IzY
		 */
		void								ORA_AbY_1();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchLowAddrByteAndIncPc
		 *	FetchHighAddrByteAndIncPcAndAddY
		 *	FetchEffectiveAddressHigh_IzY
		 *	ORA_AbY_1
		 */
		void								ORA_AbY_2();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address+Y and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchEffectiveAddressLow_IzY()
		 *	FetchEffectiveAddressHigh_IzY()
		 *	AddYAndReadAddress_IndY() (On Page Boundary)
		 */
		void								ORA_IndY();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 */
		void								ORA_Zp();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchPointerAndIncPc()
		 *	ReadFromAddressAndAddX_Zpx()
		 */
		void								ORA_Zpx();
		/** Fetches from PC and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 */
		void								ORA_Imm();
		/** Fetches from PC and performs A = A | OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchLowAddrByteAndIncPc()
		 *	FetchHighAddrByteAndIncPc()
		 */
		void								ORA_Abs();
		/** Fetches from PC and performs A = A & OP.  Sets flags N and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 */
		void								ANC_Imm();
		/** A zero-page NOP.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 */
		void								NOP_Zp();
		/** A zero-page ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 *	ReadFromEffectiveAddress_Zp
		 */
		void								ASL_Zp_1();
		/** A zero-page ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 *	ReadFromEffectiveAddress_Zp
		 *	ASL_Zp_1
		 */
		void								ASL_Zp_2();
		/** A zero-page ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 *	ReadFromAddressAndAddX_Zpx
		 *	ReadFromEffectiveAddress_Abs
		 */
		void								ASL_Zpx_1();
		/** A zero-page ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 *	ReadFromAddressAndAddX_Zpx
		 *	ReadFromEffectiveAddress_Abs
		 *	ASL_Zpx_1
		 */
		void								ASL_Zpx_2();
		/** An ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 */
		void								ASL_Imp();
		/** An ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchLowAddrByteAndIncPc
		 *	FetchHighAddrByteAndIncPc
		 *	ReadFromEffectiveAddress_Abs
		 */
		void								ASL_Abs_1();
		/** An ASL (Arithmetic Shift Left).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchLowAddrByteAndIncPc()
		 *	FetchHighAddrByteAndIncPc()
		 *	ReadFromEffectiveAddress_Abs()
		 *	WriteValueBackToEffectiveAddress
		 *	ASL_Abs_1
		 */
		void								ASL_Abs_2();
		/** Pushes the status byte.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	ReadNextInstByteAndDiscard()
		 */
		void								PHP_Imp();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs OP = (OP << 1); A = A | (OP).  Sets flags N and Z.  This is the first cycle of the function, which performs only the "OP = (OP << 1)" part.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchPointerAndIncPc
		 *	ReadAddressAddX_IzX
		 *	FetchEffectiveAddressLow_IzX
		 *	FetchEffectiveAddressHigh_IzX
		 *	ReadFromEffectiveAddress_Abs
		 */
		void								SLO_IndX_1();
		/** Fetches from LSN_CPU_CONTEXT::a.ui16Address and performs OP = (OP << 1); A = A | (OP).  Sets flags N and Z.  This is the second cycle of the function, which performs only the "A = A | (OP)" part.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchPointerAndIncPc
		 *	ReadAddressAddX_IzX
		 *	FetchEffectiveAddressLow_IzX
		 *	FetchEffectiveAddressHigh_IzX
		 *	ReadFromEffectiveAddress_Abs
		 *	SLO_IndX_1
		 */
		void								SLO_IndX_2();
		/** A zero-page SLO (Undocumented).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 *	ReadFromEffectiveAddress_Zp
		 */
		void								SLO_Zp_1();
		/** A zero-page SLO (Undocumented).  Sets flags C, N, and Z.
		 * Chain:
		 *	FetchOpcodeAndIncPc (implicit.)
		 *	FetchAddressAndIncPc_Zp
		 *	ReadFromEffectiveAddress_Zp
		 *	SLO_Zp_1
		 */
		void								SLO_Zp_2();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

}	// namespace lsn
