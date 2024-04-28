/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for all CPU's.
 */


#pragma once


#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CCpuBase
	 * \brief The base class for all CPU's.
	 *
	 * Description: The base class for all CPU's.
	 */
	class CCpuBase {
	public :
		CCpuBase( CCpuBus * _pbBus ) :
			m_ui64CycleCount( 0 ),
			m_pbBus( _pbBus ) {
		}


		// == Functions.
		/**
		 * Gets the current CPU cycle count.
		 *
		 * \return Returns the current CPU cycle count.
		 */
		inline uint64_t						GetCycleCount() const { return m_ui64CycleCount; }

		/**
		 * Signals an IRQ to be handled before the next instruction.
		 */
		virtual void						Irq() {}

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
			LSN_AM_INDIRECT,																/**< Absolute indirect addressing (JMP) = 4 extra cycles. */
		};

		/** Instructions. Numbers specified because these are used in look-up tables. */
		enum LSN_INSTRUCTIONS : uint8_t {
			LSN_I_ADC						= 0,											/**< Add with carry. */
			LSN_I_AND						= 1,											/**< AND with accumulator. */
			LSN_I_ASL						= 2,											/**< Arithmetic shift left. */
			LSN_I_BCC						= 3,											/**< Branch on carry clear. */
			LSN_I_BCS						= 4,											/**< Branch on carry set. */
			LSN_I_BEQ						= 5,											/**< Branch on equal (zero set). */
			LSN_I_BIT						= 6,											/**< Bit test. */
			LSN_I_BMI						= 7,											/**< Branch on minus (negative set). */
			LSN_I_BNE						= 8,											/**< Branch on not equal (zero clear). */
			LSN_I_BPL						= 9,											/**< Branch on plus (negative clear). */
			LSN_I_BRK						= 10,											/**< Break/interrupt. */
			LSN_I_BVC						= 11,											/**< Branch on overflow clear. */
			LSN_I_BVS						= 12,											/**< Branch on overflow set. */
			LSN_I_CLC						= 13,											/**< Clear carry. */
			LSN_I_CLD						= 14,											/**< Clear decimal. */
			LSN_I_CLI						= 15,											/**< Clear interrupt-disable. */
			LSN_I_CLV						= 16,											/**< Clear overflow. */
			LSN_I_CMP						= 17,											/**< Compare (with accumulator). */
			LSN_I_CPX						= 18,											/**< Compare with X. */
			LSN_I_CPY						= 19,											/**< Compare with Y. */
			LSN_I_DEC						= 20,											/**< Decrement. */
			LSN_I_DEX						= 21,											/**< Decrement X. */
			LSN_I_DEY						= 22,											/**< Decrement Y. */
			LSN_I_EOR						= 23,											/**< XOR with accumulator. */
			LSN_I_INC						= 24,											/**< Increment. */
			LSN_I_INX						= 25,											/**< Increment X. */
			LSN_I_INY						= 26,											/**< Increment Y. */
			LSN_I_JMP						= 27,											/**< Jump. */
			LSN_I_JSR						= 28,											/**< Jump subroutine. */
			LSN_I_LDA						= 29,											/**< Load accumulator. */
			LSN_I_LDX						= 30,											/**< Load X. */
			LSN_I_LDY						= 31,											/**< Load Y. */
			LSN_I_LSR						= 32,											/**< Logical shift right. */
			LSN_I_NOP						= 33,											/**< No operation. */
			LSN_I_ORA						= 34,											/**< OR with accumulator. */
			LSN_I_PHA						= 35,											/**< Push accumulator. */
			LSN_I_PHP						= 36,											/**< Push processor status (SR). */
			LSN_I_PLA						= 37,											/**< Pull accumulator. */
			LSN_I_PLP						= 38,											/**< Pull processor status (SR). */
			LSN_I_ROL						= 39,											/**< Rotate left. */
			LSN_I_ROR						= 40,											/**< Rotate right. */
			LSN_I_RTI						= 41,											/**< Return from interrupt. */
			LSN_I_RTS						= 42,											/**< Return from subroutine. */
			LSN_I_SBC						= 43,											/**< Subtract with carry. */
			LSN_I_SEC						= 44,											/**< Set carry. */
			LSN_I_SED						= 45,											/**< Set decimal. */
			LSN_I_SEI						= 46,											/**< Set interrupt-disable. */
			LSN_I_STA						= 47,											/**< Store accumulator. */
			LSN_I_STX						= 48,											/**< Store X. */
			LSN_I_STY						= 49,											/**< Store Y. */
			LSN_I_TAX						= 50,											/**< Transfer accumulator to X. */
			LSN_I_TAY						= 51,											/**< Transfer accumulator to Y. */
			LSN_I_TSX						= 52,											/**< Transfer stack pointer to X. */
			LSN_I_TXA						= 53,											/**< Transfer X to accumulator. */
			LSN_I_TXS						= 54,											/**< Transfer X to stack pointer. */
			LSN_I_TYA						= 55,											/**< Transfer Y to accumulator. */

			LSN_I_ALR						= 56,											/**< Illegal. Performs A & OP; LSR. */
			LSN_I_ASR						= 56,											/**< Same as LSN_I_ALR. */
			LSN_I_ANC						= 57,											/**< Illegal. Bitwise AND with carry. C flag is set using ASL rules. */
			LSN_I_ANC2						= 58,											/**< Illegal. Bitwise AND with carry. C flag is set using ROL rules. */
			LSN_I_ANE						= 59,											/**< Illegal and highly unstable. Performs A = (A | CONST) & X & OP, where CONST is effectively random. */
			LSN_I_ARR						= 60,											/**< Illegal. Performs A = A & OP; A = (A >> 1) | (C << 7). */
			LSN_I_DCP						= 61,											/**< Illegal. Performs OP--; CMP(A, OP). */
			LSN_I_ISC						= 62,											/**< Illegal. Performs INC; SBC. */
			LSN_I_ISB						= 62,											/**< Same as LSN_I_ISC. */
			LSN_I_LAS						= 63,											/**< Illegal. Performs A = X = S = (OP & S). */
			LSN_I_LAX						= 64,											/**< Illegal. Performs A = X = OP. */
			LSN_I_LXA						= 65,											/**< Illegal and highly unstable. Performs X = A = ((A | CONST) & OP), where CONST is effectively random. */
			LSN_I_RLA						= 66,											/**< Illegal. ROL OP, A &= OP. */
			LSN_I_RRA						= 67,											/**< Illegal. ROR OP, A += OP + C. */
			LSN_I_SAX						= 68,											/**< Illegal. Performs OP = A & X. */
			LSN_I_SBX						= 69,											/**< Illegal. Performs X = (A & X) - OP, setting flags as with CMP. */
			LSN_I_SHA						= 70,											/**< Illegal. Stores A & X & (high-byte of address + 1) at the address. */
			LSN_I_SHX						= 71,											/**< Illegal. Stores X & (high-byte of address + 1) at the address. */
			LSN_I_SHY						= 72,											/**< Illegal. Stores Y & (high-byte of address + 1) at the address. */
			LSN_I_SLO						= 73,											/**< Illegal. OP <<= 1, A &= OP. */
			LSN_I_SRE						= 74,											/**< Illegal. OP >>= 1, A ^= OP. */
			LSN_I_TAS						= 75,											/**< Illegal. Performs S = A & X; Stores A & X & (high-byte of address + 1) at the address. */
			LSN_I_SHS						= 75,											/**< Same as LSN_I_TAS. */
			LSN_I_USBC						= 76,											/**< Illegal. Performs SBC + NOP. */
			LSN_I_DOP						= 77,											/**< No operation. */
			LSN_I_TOP						= 78,											/**< No operation. */
			LSN_I_JAM						= 79,											/**< Illegal. Jams the machine repeatedly putting 0xFF on the bus. */

			LSN_I_NMI						= 80,											/**< Non-opcode: NMI. */
			LSN_I_IRQ						= 81,											/**< Non-opcode: IRQ. */

			LSN_I_TOTAL
		};

		/** Other parameters. */
		enum LSN_MISC {
			LSN_M_MAX_INSTR_CYCLE_COUNT		= 8,
		};


		// == Members.
		uint64_t							m_ui64CycleCount;								/**< The total CPU cycles that have ticked. */
		CCpuBus *							m_pbBus;										/**< Pointer to the bus. */
	};

}	// namespace lsn
