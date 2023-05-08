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


#define LSN_APU_UPDATE					if constexpr ( !_bEven ) {												\
											{																	\
												m_pPulse1.TickSequencer( m_ui8Registers[0x15] & 0b01 );			\
												m_pPulse2.TickSequencer( m_ui8Registers[0x15] & 0b10 );			\
											}																	\
										}

#define LSN_4017_DELAY					3

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
		unsigned _tM1S0, unsigned _tM1S1, unsigned _tM1S2, unsigned _tM1S3, unsigned _tM1S4_0, unsigned _tM1S4_1,
		unsigned _tMasterClock, unsigned _tMasterDiv, unsigned _tApuDiv>
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
			(this->*m_pftTick)();

			m_dvRegisters3_4017.Tick();
			
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
			m_pPulse1.SetSeq( GetDuty( 0 ) );
			m_pPulse2.SetSeq( GetDuty( 0 ) );
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

			m_pbBus->SetWriteFunc( 0x4000, Write4000, this, 0 );
			m_pbBus->SetWriteFunc( 0x4001, Write4001, this, 0 );
			m_pbBus->SetWriteFunc( 0x4002, Write4002, this, 0 );
			m_pbBus->SetWriteFunc( 0x4003, Write4003, this, 0 );

			m_pbBus->SetWriteFunc( 0x4004, Write4004, this, 0 );
			m_pbBus->SetWriteFunc( 0x4005, Write4005, this, 0 );
			m_pbBus->SetWriteFunc( 0x4006, Write4006, this, 0 );
			m_pbBus->SetWriteFunc( 0x4007, Write4007, this, 0 );

			m_pbBus->SetWriteFunc( 0x4015, Write4015, this, 0 );
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
		/** Non-delayed registers. */
		uint8_t											m_ui8Registers[0x15+1];
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
		 * Determines the next CPU cycle that corresponds to a new WAV sample and returns the CPU cycle index that corresponds to
		 *	that sample as well as the fraction between that sample and the next to interpolate when sampling.
		 * 
		 * \param _ui64ApuCycle The current APU cycle.
		 * \param _ui64Hz The audio Hz.
		 * \param _fInterp Holds the returned interpolation fraction.
		 * \return Returns the CPU cycle index that corresponds with the next WAV sample to output, as well as the interpolation
		 *	fraction between that WAV sample and the next.
		 **/
		uint64_t										ApuCycleOfNextSample( uint64_t _ui64ApuCycle, uint64_t _ui64Hz, float &_fInterp ) {
			/*
			 * This is the same as:
			constexpr double dApuHz = (double)_tMasterClock / _tMasterDiv / _tApuDiv;
			double dTime = m_ui64Cycles / dApuHz;
			double dSample = m_ui64Hz * dTime;
			double dFinalSample = (floor( dSample ) + 1) / m_ui64Hz * dApuHz;
			uint64_t ui64ApuTimeAtSample = (uint64_t)floor( dFinalSample );
			_fInterp = (float)mod( floor( dFinalSample ) / dApuHz * m_ui64Hz, 1 );

			 * Done using integers to avoid loss of precision over long durations, because nothing is more annoying than
			 *	pausing your game for 18 years only to come back to it to find that the audio is a jumbled mess.
			 */
			constexpr uint64_t ui64Multiplier = _tMasterDiv * _tApuDiv;

			uint64_t ui64RealTime = _ui64ApuCycle * ui64Multiplier;											// ui64RealTime / (double)_tMasterClock = actual time in seconds of the given APU cycle.
			uint64_t ui64SampleIdx = ui64RealTime * _ui64Hz / _tMasterClock + 1;							// For example, after 1 second (ui64RealTime == _tMasterClock), ui64SampleIdx will equal the Hz (_ui64Hz).
																											//	(+ 1) because we want the time of the next sample.
			uint64_t ui64ApuTimeAtSample = ui64SampleIdx * _tMasterClock / (_ui64Hz * ui64Multiplier);		// The APU cycle index that corresponds with that WAV sample.  This is the return value.

			// To determine the interpolation fraction we need to convert the new APU time back to a WAV sample.
			uint64_t ui64ApuTimeOfWav = ui64ApuTimeAtSample * ui64Multiplier * _ui64Hz;						// (* ui64Multiplier) means something is normalized via (/ (double)_tMasterClock).  We want to use this
																											//	to calculate the remainder, but it first needs to be tied to the Hz (* _ui64Hz).
			_fInterp = (ui64ApuTimeOfWav % _tMasterClock) / static_cast<float>(_tMasterClock);				// Use % to keep the values sanitized so that the conversion to float never loses precision over time.
																											//	This is the whole reason for doing everything in integer form.

			return ui64ApuTimeAtSample;
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
		 * Writing to 0x4000 (Duty, loop envelope/disable length counter, constant volume, envelope period/volume).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x00] = _ui8Val;
			paApu->m_pPulse1.SetSeq( GetDuty( _ui8Val >> 6 ) );
		}

		/**
		 * Writing to 0x4001 (Sweep unit: enabled, period, negative, shift count).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4001( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x01] = _ui8Val;
		}

		/**
		 * Writing to 0x4002 (Timer low).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4002( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x02] = _ui8Val;
			paApu->m_pPulse1.SetTimerLow( _ui8Val );
		}

		/**
		 * Writing to 0x4003 (Length counter load, timer high (also resets duty and starts envelope)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4003( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x03] = _ui8Val;
			paApu->m_pPulse1.SetTimerHigh( _ui8Val );
		}

		/**
		 * Writing to 0x4004 (Duty, loop envelope/disable length counter, constant volume, envelope period/volume).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4004( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x04] = _ui8Val;
			paApu->m_pPulse2.SetSeq( GetDuty( _ui8Val >> 6 ) );
		}

		/**
		 * Writing to 0x4005 (Sweep unit: enabled, period, negative, shift count).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4005( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x05] = _ui8Val;
		}

		/**
		 * Writing to 0x4006 (Timer low).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4006( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x06] = _ui8Val;
			paApu->m_pPulse2.SetTimerLow( _ui8Val );
		}

		/**
		 * Writing to 0x4007 (Length counter load, timer high (also resets duty and starts envelope)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x07] = _ui8Val;
			paApu->m_pPulse2.SetTimerHigh( _ui8Val );
		}

		/**
		 * Writing to 0x4008 (Length counter disable/linear counter control, linear counter reload value).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4008( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x08] = _ui8Val;
		}

		/**
		 * Writing to 0x400A (Timer low).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write400A( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x0A] = _ui8Val;
		}

		/**
		 * Writing to 0x4015 (Status: DMC interrupt, frame interrupt, length counter status: noise, triangle, pulse 2, pulse 1 (read)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write4015( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x15] = _ui8Val;
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
			// This is handled by having Set4017() set m_bModeSwitch, which will then be seen only on even ticks.
			paApu->m_dvRegisters3_4017.WriteWithDelay( _ui8Val );
		}

		/**
		 * Gets a duty sequence bit mask.
		 * 
		 * \param _ui8Duty The duty cycle index.
		 * \return Returns the bit sequence for the given pulse duty cycle.
		 **/
		static inline uint8_t							GetDuty( uint8_t _ui8Duty ) {
			static const uint8_t ui8Seqs[] = {
				0b10000000,
				0b11000000,
				0b11110000,
				0b00111111,
			};
			return ui8Seqs[_ui8Duty];
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
	//typedef CApu2A0X<LSN_APU_TYPE( NTSC )>				CNtscApu;

	/**
	 * A PAL PPU.
	 */
	//typedef CApu2A0X<LSN_APU_TYPE( PAL )>				CPalApu;

//#undef LSN_APU_TYPE

}	// namespace lsn
