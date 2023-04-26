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
#include "../Utilities/LSNDelayedValue.h"


#define LSN_APU_UPDATE					if constexpr ( _bEven ) {				\
											if ( m_bModeSwitch ) {				\
												m_bModeSwitch = false;			\
											}									\
											else {								\
												m_pPulse1.Tick( true );			\
												m_pPulse2.Tick( true );			\
											}									\
										}

#define LSN_4017_DELAY					3										/**< 1 higher than expected because the delayed register is ticked BEFORE the standard tick function is called. */

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
			m_ui64StepCycles( 0 ),
			m_dvRegisters3_4017( Set4017, this ) {

		}
		~CApu2A0X() {
		}


		// == Types.
		typedef CDelayedValue<uint8_t, LSN_4017_DELAY>	DelayedVal;


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
			m_dvRegisters3_4017.Tick();
			(this->*m_pftTick)();

			
			++m_ui64Cycles;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void											ResetAnalog() {
			m_ui64Cycles = 0;
			m_ui64StepCycles = 0;
			m_pftTick = &CApu2A0X::Tick_Mode0_Step0<false>;
			m_bModeSwitch = false;
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


			m_pbBus->SetWriteFunc( 0x4017, Write4017, this, 0 );
		}

		/**
		 * Returns true if the current APU cycle is even.
		 * 
		 * \return Returns true if the current APU cycle is even.
		 **/
		bool											IsEvenCycle() const { return (m_ui64Cycles & 1) == 1; }




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
		/** Delayed writes. */
		DelayedVal										m_dvRegisters3_4017;
		/** Set to true upon a write to $4017. */
		bool											m_bModeSwitch;


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

		/**
		 * The callback function for when $4017 gets set.
		 * 
		 * \param _pvParm A pointer to this APU object.
		 * \param _tNewVal The new value being set.
		 * \param _tOldVal The old value.
		 **/
		static void										Set4017( void * _pvParm, DelayedVal::Type /*_tNewVal*/, DelayedVal::Type /*_tOldVal*/ ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm);
			paApu->m_bModeSwitch = true;
		}

		/**
		 * Writing to 0x4017 (Frame counter: 5-frame sequence, disable frame interrupt).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4017( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			// * If the write occurs during an APU cycle, the effects occur 3 CPU cycles after the $4017 write cycle, and if the write occurs between APU cycles, the effects occurs 4 CPU cycles after the write cycle.
			// "During" an APU cycle means every even CPU cycle.  "Between" APU cycles means every odd CPU cycle.
			paApu->m_dvRegisters3_4017.WriteWithDelay( _ui8Val, paApu->IsEvenCycle() ? LSN_4017_DELAY : (LSN_4017_DELAY - 1) );
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
	typedef CApu2A0X<LSN_APU_TYPE( NTSC )>				CNtscApu;

	/**
	 * A PAL PPU.
	 */
	typedef CApu2A0X<LSN_APU_TYPE( PAL )>				CPalApu;

#undef LSN_APU_TYPE

}	// namespace lsn
