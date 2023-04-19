/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The 2A0X series of APU's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNPulse.h"
#include "../Bus/LSNBus.h"
#include "../System/LSNTickable.h"


#define LSN_APU_UPDATE					if constexpr ( !_bEven ) {				\
											m_pPulse1.Tick( true );				\
											m_pPulse2.Tick( true );				\
										}

namespace lsn {

	/** APU timings. */
	enum LSN_APU_TIMINGS {
		LSN_AT_NTSC_MODE_0_STEP_0						= uint32_t( 3728.5 * 2.0 ),			/**< 7457. */
		LSN_AT_NTSC_MODE_0_STEP_1						= uint32_t( 7456.5 * 2.0 ),			/**< 14913. */
		LSN_AT_NTSC_MODE_0_STEP_2						= uint32_t( 11185.5 * 2.0 ),		/**< 22371. */
		LSN_AT_NTSC_MODE_0_STEP_3_0						= uint32_t( 14914 * 2.0 ),			/**< 29828. */
		LSN_AT_NTSC_MODE_0_STEP_3_1						= uint32_t( 14914.5 * 2.0 ),		/**< 29829. */
		LSN_AT_NTSC_MODE_0_STEP_3_2						= uint32_t( 14915 * 2.0 ),			/**< 29830. */

		LSN_AT_PAL_MODE_0_STEP_0						= uint32_t( 4156.5 * 2.0 ),			/**< 8313. */
		LSN_AT_PAL_MODE_0_STEP_1						= uint32_t( 8313.5 * 2.0 ),			/**< 16627. */
		LSN_AT_PAL_MODE_0_STEP_2						= uint32_t( 12469.5 * 2.0 ),		/**< 24939. */
		LSN_AT_PAL_MODE_0_STEP_3_0						= uint32_t( 16626 * 2.0 ),			/**< 33252. */
		LSN_AT_PAL_MODE_0_STEP_3_1						= uint32_t( 16626.5 * 2.0 ),		/**< 33253. */
		LSN_AT_PAL_MODE_0_STEP_3_2						= uint32_t( 16627 * 2.0 ),			/**< 33254. */


		LSN_AT_NTSC_MODE_1_STEP_0						= uint32_t( 3728.5 * 2.0 ),			/**< 7457. */
		LSN_AT_NTSC_MODE_1_STEP_1						= uint32_t( 7456.5 * 2.0 ),			/**< 14913. */
		LSN_AT_NTSC_MODE_1_STEP_2						= uint32_t( 11185.5 * 2.0 ),		/**< 22371. */
		LSN_AT_NTSC_MODE_1_STEP_3						= uint32_t( 14914.5 * 2.0 ),		/**< 29829. */
		LSN_AT_NTSC_MODE_1_STEP_4_0						= uint32_t( 18640.5 * 2.0 ),		/**< 37281. */
		LSN_AT_NTSC_MODE_1_STEP_4_1						= uint32_t( 18641 * 2.0 ),			/**< 37282. */

		LSN_AT_PAL_MODE_1_STEP_0						= uint32_t( 4156.5 * 2.0 ),			/**< 8313. */
		LSN_AT_PAL_MODE_1_STEP_1						= uint32_t( 8313.5 * 2.0 ),			/**< 16627. */
		LSN_AT_PAL_MODE_1_STEP_2						= uint32_t( 12469.5 * 2.0 ),		/**< 24939. */
		LSN_AT_PAL_MODE_1_STEP_3						= uint32_t( 16626.5 * 2.0 ),		/**< 33253. */
		LSN_AT_PAL_MODE_1_STEP_4_0						= uint32_t( 20782.5 * 2.0 ),		/**< 41565. */
		LSN_AT_PAL_MODE_1_STEP_4_1						= uint32_t( 20783 * 2.0 ),			/**< 41566. */
	};

	/**
	 * Class CApu2A0X
	 * \brief The 2A0X series of APU's.
	 *
	 * Description: The 2A0X series of APU's.
	 */
	template <unsigned _tM0S0, unsigned _tM0S1, unsigned _tM0S2, unsigned _tM0S3_0, unsigned _tM0S3_1, unsigned _tM0S3_2,
		unsigned _tM1S0, unsigned _tM1S1, unsigned _tM1S2, unsigned _tM1S3, unsigned _tM1S4_0, unsigned _tM1S4_1>
	class CApu2A0X : public CTickable {
	public :
		CApu2A0X( CCpuBus * _pbBus ) :
			m_pbBus( _pbBus ),
			m_ui64Cycles( 0 ),
			m_ui64StepCycles( 0 ) {
		}
		~CApu2A0X() {
		}


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
			(this->*m_pftTick)();
			++m_ui64Cycles;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void											ResetAnalog() {
			m_ui64Cycles = 0;
			m_ui64StepCycles = 0;
			m_pftTick = &CApu2A0X::Tick_Mode0_Step0<true>;
		}

		/**
		 * Resets the APU to a known state.
		 */
		void											ResetToKnown() {
			ResetAnalog();
		}

		/**
		 * Applies the APU's memory mapping t the bus.
		 */
		void											ApplyMemoryMap() {
			// Apply the APU memory map to the bus.

			// None of the registers are readable except 0x4015.
			for ( uint32_t I = LSN_APU_START; I < LSN_APU_IO_START; ++I ) {
				if ( I != 0x4014 /*&& I != 0x4016*/	// Not registers.
					&& I != 0x4015 ) {			// Readable.
					m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
				}
			}
			// I/O registers normally disabled except in debug mode.
			for ( uint32_t I = LSN_APU_IO_START; I < (LSN_APU_IO_START + LSN_APU_IO); ++I ) {
				m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
			}
			// Unallocated space from 0x4020 to 0x40FF.
			for ( uint32_t I = (LSN_APU_IO_START + LSN_APU_IO); I <= (LSN_APU_START + 0xFF); ++I ) {
				m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
			}
		}




	protected :
		// == Types.
		/** A function pointer for the tick handlers. */
		typedef void (CApu2A0X:: *						PfTicks)();


		// == Members.
		/** The cycle counter. */
		uint64_t										m_ui64Cycles;
		/** The step cycle counter. */
		uint64_t										m_ui64StepCycles;
		/** The main bus. */
		CCpuBus *										m_pbBus;
		/** The current cycle function. */
		PfTicks											m_pftTick;
		/** Pulse 1. */
		CPulse											m_pPulse1;
		/** Pulse 2. */
		CPulse											m_pPulse2;


		// == Functions.
		/** Mode-0 step-0 tick function. */
		template <bool _bEven>
		void											Tick_Mode0_Step0() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S0 ) {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step1<!_bEven>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step0<!_bEven>;
			}
		}

		/** Mode-0 step-1 tick function. */
		template <bool _bEven>
		void											Tick_Mode0_Step1() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S1 ) {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step2<!_bEven>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step1<!_bEven>;
			}
		}

		/** Mode-0 step-2 tick function. */
		template <bool _bEven>
		void											Tick_Mode0_Step2() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S2 ) {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step3<!_bEven>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step2<!_bEven>;
			}
		}

		/** Mode-0 step-3 tick function. */
		template <bool _bEven>
		void											Tick_Mode0_Step3() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S3_2 ) {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step0<!_bEven>;
				m_ui64StepCycles = 0;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step3<!_bEven>;
			}
		}


		/** Mode-1 step-0 tick function. */
		template <bool _bEven>
		void											Tick_Mode1_Step0() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S0 ) {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step1<!_bEven>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step0<!_bEven>;
			}
		}

		/** Mode-1 step-1 tick function. */
		template <bool _bEven>
		void											Tick_Mode1_Step1() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S1 ) {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step2<!_bEven>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step1<!_bEven>;
			}
		}

		/** Mode-1 step-2 tick function. */
		template <bool _bEven>
		void											Tick_Mode1_Step2() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S2 ) {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step3<!_bEven>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step2<!_bEven>;
			}
		}

		/** Mode-1 step-3 tick function. */
		template <bool _bEven>
		void											Tick_Mode1_Step3() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S3 ) {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step4<!_bEven>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step3<!_bEven>;
			}
		}

		/** Mode-1 step-4 tick function. */
		template <bool _bEven>
		void											Tick_Mode1_Step4() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S4_1 ) {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step0<!_bEven>;
				m_ui64StepCycles = 0;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step4<!_bEven>;
			}
		}

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
#define LSN_APU_TYPE( REGION )							LSN_AT_ ## REGION ## _MODE_0_STEP_0, LSN_AT_ ## REGION ## _MODE_0_STEP_1, LSN_AT_ ## REGION ## _MODE_0_STEP_2,																				\
														LSN_AT_ ## REGION ## _MODE_0_STEP_3_0, LSN_AT_ ## REGION ## _MODE_0_STEP_3_1, LSN_AT_ ## REGION ## _MODE_0_STEP_3_2,																		\
														LSN_AT_ ## REGION ## _MODE_1_STEP_0, LSN_AT_ ## REGION ## _MODE_1_STEP_1, LSN_AT_ ## REGION ## _MODE_1_STEP_2, LSN_AT_ ## REGION ## _MODE_1_STEP_3,											\
														LSN_AT_ ## REGION ## _MODE_1_STEP_4_0, LSN_AT_ ## REGION ## _MODE_1_STEP_4_1

	/**
	 * An NTSC PPU.
	 */
	typedef CApu2A0X<LSN_APU_TYPE( NTSC )>													CNtscApu;

	/**
	 * A PAL PPU.
	 */
	typedef CApu2A0X<LSN_APU_TYPE( PAL )>													CPalApu;

#undef LSN_APU_TYPE

}	// namespace lsn
