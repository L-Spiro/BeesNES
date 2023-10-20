/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The 2A0X series of APU's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Audio/LSNAudio.h"
#include "../Audio/LSNPoleFilter.h"
#include "../Audio/LSNSampleBucket.h"
#include "../Audio/LSNSincFilter.h"
#include "../Bus/LSNBus.h"
#include "../System/LSNInterruptable.h"
#include "../System/LSNTickable.h"
#include "../Utilities/LSNDelayedValue.h"
#include "LSNApuUnit.h"
#include "LSNNoise.h"
#include "LSNPulse.h"
#include "LSNTriangle.h"


#define LSN_PULSE1_ENABLED( THIS )		(((THIS)->m_ui8Registers[0x15] & 0b0001) != 0)
#define LSN_PULSE2_ENABLED( THIS )		(((THIS)->m_ui8Registers[0x15] & 0b0010) != 0)
#define LSN_TRIANGLE_ENABLED( THIS )	(((THIS)->m_ui8Registers[0x15] & 0b0100) != 0)
#define LSN_NOISE_ENABLED( THIS )		(((THIS)->m_ui8Registers[0x15] & 0b1000) != 0)

#define LSN_PULSE1_HALT					((m_ui8Registers[0x00] & 0b00100000) != 0)
#define LSN_PULSE2_HALT					((m_ui8Registers[0x04] & 0b00100000) != 0)
#define LSN_TRIANGLE_HALT				((m_ui8Registers[0x08] & 0b10000000) != 0)
#define LSN_NOISE_HALT					((m_ui8Registers[0x0C] & 0b00100000) != 0)

#define LSN_PULSE1_USE_VOLUME			((m_ui8Registers[0x00] & 0b00010000) != 0)
#define LSN_PULSE2_USE_VOLUME			((m_ui8Registers[0x04] & 0b00010000) != 0)
#define LSN_NOISE_USE_VOLUME			((m_ui8Registers[0x0C] & 0b00010000) != 0)

#define LSN_PULSE1_ENV_DIVIDER( THIS )	((THIS)->m_ui8Registers[0x00] & 0b00001111)
#define LSN_PULSE2_ENV_DIVIDER( THIS )	((THIS)->m_ui8Registers[0x04] & 0b00001111)
#define LSN_NOISE_ENV_DIVIDER( THIS )	((THIS)->m_ui8Registers[0x0C] & 0b00001111)

// _bEven is false on 0 2 4 6 8, etc.  It goes by cycle count rather than cycle index.
#define LSN_APU_UPDATE					if constexpr ( !_bEven ) {												\
											if ( m_bModeSwitch ) {												\
												m_bModeSwitch = false;											\
												if ( (m_dvRegisters3_4017.Value() & 0b10000000) != 0 ) {		\
													 m_ui64StepCycles = _tM1S4_1 - 2;							\
													Tick_Mode1_Step4<_bEven, true>();							\
													m_pftTick = &CApu2A0X::Tick_Mode1_Step0<!_bEven, true>;		\
													m_ui64StepCycles = 1;										\
												}																\
												else {															\
													m_ui64StepCycles = 0;										\
													Tick_Mode0_Step0<_bEven, false>();							\
												}																\
												return;															\
											}																	\
											{																	\
												m_pPulse1.TickSequencer( LSN_PULSE1_ENABLED( this ) );			\
												m_pPulse2.TickSequencer( LSN_PULSE2_ENABLED( this ) );			\
												m_nNoise.TickSequencer( LSN_NOISE_ENABLED( this ) );			\
											}																	\
										}																		\
										m_tTriangle.TickSequencer( LSN_TRIANGLE_ENABLED( this ) );

#define LSN_4017_DELAY					(3+1)


namespace lsn {

	// == Enumerations.
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
	template <unsigned _tType, unsigned _tM0S0, unsigned _tM0S1, unsigned _tM0S2, unsigned _tM0S3_0, unsigned _tM0S3_1, unsigned _tM0S3_2,
		unsigned _tM1S0, unsigned _tM1S1, unsigned _tM1S2, unsigned _tM1S3, unsigned _tM1S4_0, unsigned _tM1S4_1,
		unsigned _tMasterClock, unsigned _tMasterDiv, unsigned _tApuDiv>
	class CApu2A0X : public CTickable {
	public :
		CApu2A0X( CCpuBus * _pbBus, CInterruptable * _piIrqTarget ) :
			m_pbBus( _pbBus ),
			m_ui64Cycles( 0 ),
			m_ui64StepCycles( 0 ),
			m_ui64LastBucketCycle( 0 ),
			m_piIrqTarget( _piIrqTarget ),
			m_dvRegisters3_4017( Set4017, this ) {
			m_pfPole90.CreateHpf( 90.0f / float( double( _tMasterClock ) / _tMasterDiv / _tApuDiv ) );
			m_pfPole440.CreateHpf( 440.0f / float( double( _tMasterClock ) / _tMasterDiv / _tApuDiv ) );
			m_pfPole14.CreateLpf( 14000.0f / float( double( _tMasterClock ) / _tMasterDiv / _tApuDiv ) );
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
			m_pPulse1.UpdateSweeperState();
			m_pPulse2.UpdateSweeperState();

			
			while ( (m_ui64LastBucketCycle - m_ui64Cycles) <= (LSN_SAMPLER_BUCKET_SIZE / 2 + 1) ) {
				// Determine the next APU cycle that corresponds with an output sample.
				float fInterp;
				uint64_t ui64ApuOutputCycle = ApuCycleOfNextSample( m_ui64LastBucketCycle + 1, CAudio::GetOutputFrequency(), fInterp );
				CAudio::RegisterBucket( ui64ApuOutputCycle, fInterp );
				m_ui64LastBucketCycle = ui64ApuOutputCycle;
			}
			/*{
				double dApuHz = (double)_tMasterClock / _tMasterDiv / _tApuDiv;
				double dTime = m_ui64Cycles / dApuHz;

				CAudio::AddSample( m_ui64Cycles, (float)std::sin( dTime * 2.0 * 3.1415926535897932384626433832795 * 440.0 ) );
			}*/
			float fPulse1 = (m_pPulse1.ProducingSound( LSN_PULSE1_ENABLED( this ) )) ? m_pPulse1.GetEnvelopeOutput( LSN_PULSE1_USE_VOLUME ) : 0.0f;
			float fPulse2 = (m_pPulse2.ProducingSound( LSN_PULSE2_ENABLED( this ) )) ? m_pPulse2.GetEnvelopeOutput( LSN_PULSE2_USE_VOLUME ) : 0.0f;
			float fFinalPulse = fPulse1 + fPulse2;
			if ( fFinalPulse ) {
				fFinalPulse = 95.88f / ((8128.0f / fFinalPulse) + 100.0f);
			}
			float fNoise = (m_nNoise.ProducingSound( LSN_NOISE_ENABLED( this ) )) ? m_nNoise.GetEnvelopeOutput( LSN_NOISE_USE_VOLUME ) : 0.0f;
			float fTriangle = m_tTriangle.Output();
			float fDmc = 0.0f;

			//fFinalPulse = fNoise = 0.0f;
			fNoise /= 12241.0f;
			fTriangle /= 8227.0f;
			fDmc /= 22638.0f;
			float fFinalTnd = 0.0f;
			if ( fNoise != 0.0f || fTriangle != 0.0f || fDmc != 0.0f ) {
				fFinalTnd = 159.79f / (1.0f / (fNoise + fTriangle + fDmc) + 100.0f);
			}
			
			double dFinal = static_cast<float>(m_pfPole90.Process( m_pfPole440.Process( m_pfPole14.Process( fFinalPulse + fFinalTnd ) ) ));

			float fLpf = (CAudio::GetOutputFrequency() / 2.0f) / float( double( _tMasterClock ) / _tMasterDiv / _tApuDiv );
			if ( fLpf < 0.5f ) {
				for ( auto I = LSN_ELEMENTS( m_pfOutputPole ); I--; ) {
					m_pfOutputPole[I].CreateLpf( fLpf );
				}
				for ( auto I = LSN_ELEMENTS( m_pfOutputPole ); I--; ) {
					dFinal = m_pfOutputPole[I].Process( dFinal );
				}
				//m_sfSincFilter.CreateLpf( double( _tMasterClock ) / _tMasterDiv / _tApuDiv, CAudio::GetOutputFrequency() / 2.0, size_t( 100 ), CSincFilter::SynthesizeHammingWindow );
				//dFinal = m_pfOutputPole.Process( m_pfOutputPole1.Process( m_pfOutputPole2.Process( m_pfOutputPole3.Process( dFinal ) ) ) );
				//dFinal = m_sfSincFilter.FilterSample( dFinal );
			}
			//m_fMaxSample = std::max( m_fMaxSample, fFinal );
			//m_fMinSample = std::min( m_fMinSample, fFinal );
			//float fRange = m_fMaxSample - m_fMinSample;
			//if ( fRange == 0.0f ) { fRange = 1.0f; }
			//float fCenter = (m_fMaxSample + m_fMinSample) / 2.0f;
			//CAudio::AddSample( m_ui64Cycles, (fFinal - fCenter) * (1.0f / fRange) * 1.0f );
			CAudio::AddSample( m_ui64Cycles, static_cast<float>(dFinal * 0.5) );

			++m_ui64Cycles;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void											ResetAnalog() {
			m_ui8Registers[0x15] = 0x00;
			m_ui64Cycles = 0;
			m_ui64StepCycles = 0;
			m_ui64LastBucketCycle = 0;
			CAudio::BeginEmulation();
			m_pftTick = &CApu2A0X::Tick_Mode0_Step0<false, false>;
			m_bModeSwitch = false;
			m_pPulse1.SetSeq( GetDuty( 0 ) );
			m_pPulse2.SetSeq( GetDuty( 0 ) );
			m_pPulse1.SetEnvelopeVolume( LSN_PULSE1_ENV_DIVIDER( this ) );
			m_pPulse2.SetEnvelopeVolume( LSN_PULSE2_ENV_DIVIDER( this ) );
			m_nNoise.SetEnvelopeVolume( LSN_NOISE_ENV_DIVIDER( this ) );
			m_fMaxSample = -INFINITY;
			m_fMinSample = INFINITY;
		}

		/**
		 * Resets the APU to a known state.
		 */
		void											ResetToKnown() {
			for ( auto I = 0x00; I <= 0x0F; ++I ) {
				m_ui8Registers[I] = 0x00;
			}
			for ( auto I = 0x10; I <= 0x13; ++I ) {
				m_ui8Registers[I] = 0x00;
			}
			m_ui8Registers[0x15] = 0x00;
			m_dvRegisters3_4017.SetValue( 0x00 );
			m_pPulse1.ResetToKnown();
			m_pPulse2.ResetToKnown();
			m_nNoise.ResetToKnown();
			m_tTriangle.ResetToKnown();
			ResetAnalog();
		}

		/**
		 * Applies the APU's memory mapping t the bus.
		 */
		void											ApplyMemoryMap() {
			// Apply the APU memory map to the bus.

			// None of the registers are readable except 0x4015.
			for ( uint32_t I = LSN_APU_START; I < LSN_APU_IO_START; ++I ) {
				if ( I != 0x4014 && I != 0x4016 && I != 0x4017	// Not registers.
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

			m_pbBus->SetWriteFunc( 0x4008, Write4008, this, 0 );
			m_pbBus->SetWriteFunc( 0x400A, Write400A, this, 0 );
			m_pbBus->SetWriteFunc( 0x400B, Write400B, this, 0 );

			m_pbBus->SetWriteFunc( 0x400C, Write400C, this, 0 );
			m_pbBus->SetWriteFunc( 0x400E, Write400E, this, 0 );
			m_pbBus->SetWriteFunc( 0x400F, Write400F, this, 0 );

			m_pbBus->SetReadFunc( 0x4015, Read4015, this, 0 );
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
		/** The cycle of the last-registered sample bucket.  When the APU cycle reachs this value, a new bucket should be registered. */
		uint64_t										m_ui64LastBucketCycle;
		/** The main bus. */
		CCpuBus *										m_pbBus;
		/** The IRQ target. */
		CInterruptable *								m_piIrqTarget;
		/** The current cycle function. */
		PfTicks											m_pftTick;
		/** The 90-Hz pole filter. */
		CPoleFilter										m_pfPole90;
		/** The 440-Hz pole filter. */
		CPoleFilter										m_pfPole440;
		/** The 14-Hz pole filter. */
		CPoleFilter										m_pfPole14;
		/** The output Hz filter. */
		CPoleFilter										m_pfOutputPole[4];
		/** The output (down-sampling) filter. */
		CSincFilter										m_sfSincFilter;
		/** Max output sample. */
		float											m_fMaxSample;
		/** Min output sample. */
		float											m_fMinSample;
		/** Pulse 1. */
		CPulse											m_pPulse1;
		/** Pulse 2. */
		CPulse											m_pPulse2;
		/** Noise. */
		CNoise											m_nNoise;
		/** Triangle. */
		CTriangle										m_tTriangle;
		/** Delayed writes. */
		DelayedVal										m_dvRegisters3_4017;
		/** Non-delayed registers. */
		uint8_t											m_ui8Registers[0x15+1];
		/** Set to true upon a write to $4017. */
		bool											m_bModeSwitch;
		


		// == Functions.
		/** Mode-0 step-0 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode0_Step0() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S0 ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode0_Step1<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step0<!_bEven, _bMode>;
			}
		}

		/** Mode-0 step-1 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode0_Step1() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S1 ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickSweeper<1>();
				m_pPulse2.TickSweeper<0>();

				m_pftTick = &CApu2A0X::Tick_Mode0_Step2<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step1<!_bEven, _bMode>;
			}
		}

		/** Mode-0 step-2 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode0_Step2() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S2 ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode0_Step3<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step2<!_bEven, _bMode>;
			}
		}

		/** Mode-0 step-3 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode0_Step3() {
			LSN_APU_UPDATE;
			if ( m_ui64StepCycles >= (_tM0S3_2 - 3) && (m_dvRegisters3_4017.Value() & 0b01000000) == 0 ) {
				//m_piIrqTarget->Irq();
			}

			if ( (m_ui64StepCycles + 1) == (_tM0S3_2 - 1) ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickSweeper<1>();
				m_pPulse2.TickSweeper<0>();
			}

			if ( ++m_ui64StepCycles == _tM0S3_2 ) {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step0<!_bEven, _bMode>;
				m_ui64StepCycles = 0;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step3<!_bEven, _bMode>;
			}
		}


		/** Mode-1 step-0 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode1_Step0() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S0 ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode1_Step1<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step0<!_bEven, _bMode>;
			}
		}

		/** Mode-1 step-1 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode1_Step1() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S1 ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickSweeper<1>();
				m_pPulse2.TickSweeper<0>();

				m_pftTick = &CApu2A0X::Tick_Mode1_Step2<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step1<!_bEven, _bMode>;
			}
		}

		/** Mode-1 step-2 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode1_Step2() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S2 ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode1_Step3<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step2<!_bEven, _bMode>;
			}
		}

		/** Mode-1 step-3 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode1_Step3() {
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S3 ) {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step4<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step3<!_bEven, _bMode>;
			}
		}

		/** Mode-1 step-4 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode1_Step4() {
			LSN_APU_UPDATE;

			if ( (m_ui64StepCycles + 1) == (_tM1S4_1 - 1) ) {
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickSweeper<1>();
				m_pPulse2.TickSweeper<0>();
			}

			if ( ++m_ui64StepCycles == _tM1S4_1 ) {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step0<!_bEven, _bMode>;
				m_ui64StepCycles = 0;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step4<!_bEven, _bMode>;
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
			//uint64_t ui64SampleIdx = ui64RealTime * _ui64Hz / _tMasterClock + 1;							// For example, after 1 second (ui64RealTime == _tMasterClock), ui64SampleIdx will equal the Hz (_ui64Hz).
			uint64_t ui64Hi;
			uint64_t ui64Low = _umul128( ui64RealTime, _ui64Hz, &ui64Hi );
			uint64_t ui64SampleIdx = _udiv128( ui64Hi, ui64Low, _tMasterClock, nullptr ) + 1;
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
			paApu->m_pPulse1.SetEnvelopeVolume( _ui8Val & 0b1111 );
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
			paApu->m_pPulse1.SetSweepEnabled( ((_ui8Val & 0b10000000) != 0) );
			paApu->m_pPulse1.SetSweepPeriod( (_ui8Val >> 4) & 0b111 );
			paApu->m_pPulse1.SetSweepNegate( ((_ui8Val & 0b1000) != 0) );
			paApu->m_pPulse1.SetSweepShift( _ui8Val & 0b111 );
			paApu->m_pPulse1.DirtySweeper();
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
			if ( LSN_PULSE1_ENABLED( paApu ) ) {
				paApu->m_pPulse1.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_pPulse1.RestartEnvelope();
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
			paApu->m_pPulse2.SetEnvelopeVolume( _ui8Val & 0b1111 );
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
			paApu->m_pPulse2.SetSweepEnabled( ((_ui8Val & 0b10000000) != 0) );
			paApu->m_pPulse2.SetSweepPeriod( (_ui8Val >> 4) & 0b111 );
			paApu->m_pPulse2.SetSweepNegate( ((_ui8Val & 0b1000) != 0) );
			paApu->m_pPulse2.SetSweepShift( _ui8Val & 0b111 );
			paApu->m_pPulse2.DirtySweeper();
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
			if ( LSN_PULSE2_ENABLED( paApu ) ) {
				paApu->m_pPulse2.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_pPulse2.RestartEnvelope();
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
			paApu->m_tTriangle.SetLinearCounter( _ui8Val & 0b01111111 );
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
			paApu->m_tTriangle.SetTimerLow( _ui8Val );
		}

		/**
		 * Writing to 0x400B (Length counter load, timer high (also reloads linear counter)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write400B( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x0B] = _ui8Val;
			paApu->m_tTriangle.SetTimerHigh( _ui8Val, false );
			if ( LSN_TRIANGLE_ENABLED( paApu ) ) {
				paApu->m_tTriangle.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_tTriangle.SetLinearReload();
		}

		/**
		 * Writing to 0x400C (Loop envelope/disable length counter, constant volume, envelope period/volume).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write400C( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x0C] = _ui8Val;
			paApu->m_nNoise.SetEnvelopeVolume( _ui8Val & 0b1111 );
		}

		/**
		 * Writing to 0x400E (Loop noise, noise period).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write400E( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x0E] = _ui8Val;
			paApu->m_nNoise.SetTimer( CApuUnit::NoiseTable<_tType>( (_ui8Val & 0b00001111) ) );
			paApu->m_nNoise.SetModeFlag( (_ui8Val & 0b10000000) != 0 );
		}

		/**
		 * Writing to 0x400F (Length counter load (also starts envelope)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write400F( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			paApu->m_ui8Registers[0x0F] = _ui8Val;
			if ( LSN_NOISE_ENABLED( paApu ) ) {
				paApu->m_nNoise.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_nNoise.RestartEnvelope();
		}

		/**
		 * A function usable for addresses that can't be read.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read4015( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CApu2A0X * paApu = reinterpret_cast<CApu2A0X *>(_pvParm0);
			// TODO: This register is internal to the CPU and so the external CPU data bus is disconnected when reading it. Therefore the returned value cannot be seen by external devices and the value does not affect open bus.
			_ui8Ret = _ui8Ret & 0b00100000;
			if ( paApu->m_pPulse1.GetLengthCounter() != 0 ) {
				_ui8Ret |= 0b0001;
			}
			if ( paApu->m_pPulse2.GetLengthCounter() != 0 ) {
				_ui8Ret |= 0b0010;
			}
			if ( paApu->m_tTriangle.GetLengthCounter() != 0 ) {
				_ui8Ret |= 0b0100;
			}
			if ( paApu->m_nNoise.GetLengthCounter() != 0 ) {
				_ui8Ret |= 0b1000;
			}
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

			if ( !LSN_PULSE1_ENABLED( paApu ) ) {
				paApu->m_pPulse1.SetLengthCounter( 0 );
			}
			if ( !LSN_PULSE2_ENABLED( paApu ) ) {
				paApu->m_pPulse2.SetLengthCounter( 0 );
			}
			if ( !LSN_TRIANGLE_ENABLED( paApu ) ) {
				paApu->m_tTriangle.SetLengthCounter( 0 );
			}
			if ( !LSN_NOISE_ENABLED( paApu ) ) {
				paApu->m_nNoise.SetLengthCounter( 0 );
			}
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
#define LSN_APU_TYPE( REGION )							LSN_AT_ ## REGION,																																											\
														LSN_AT_ ## REGION ## _MODE_0_STEP_0, LSN_AT_ ## REGION ## _MODE_0_STEP_1, LSN_AT_ ## REGION ## _MODE_0_STEP_2,																				\
														LSN_AT_ ## REGION ## _MODE_0_STEP_3_0, LSN_AT_ ## REGION ## _MODE_0_STEP_3_1, LSN_AT_ ## REGION ## _MODE_0_STEP_3_2,																		\
														LSN_AT_ ## REGION ## _MODE_1_STEP_0, LSN_AT_ ## REGION ## _MODE_1_STEP_1, LSN_AT_ ## REGION ## _MODE_1_STEP_2, LSN_AT_ ## REGION ## _MODE_1_STEP_3,											\
														LSN_AT_ ## REGION ## _MODE_1_STEP_4_0, LSN_AT_ ## REGION ## _MODE_1_STEP_4_1

#undef LSN_NOISE_ENV_DIVIDER
#undef LSN_TRIANGLE_ENABLED
#undef LSN_PULSE2_ENV_DIVIDER
#undef LSN_PULSE1_ENV_DIVIDER

#undef LSN_NOISE_USE_VOLUME
#undef LSN_TRIANGLE_HALT
#undef LSN_PULSE2_USE_VOLUME
#undef LSN_PULSE1_USE_VOLUME

#undef LSN_NOISE_HALT
#undef LSN_PULSE2_HALT
#undef LSN_PULSE1_HALT

#undef LSN_NOISE_ENABLED
#undef LSN_PULSE2_ENABLED
#undef LSN_PULSE1_ENABLED

}	// namespace lsn
