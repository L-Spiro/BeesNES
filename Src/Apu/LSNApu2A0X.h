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
#include "../Audio/LSNAudioOptions.h"
#include "../Audio/LSNHpfFilter.h"
#include "../Audio/LSNPoleFilter.h"
#include "../Bus/LSNBus.h"
#include "../System/LSNInterruptable.h"
#include "../System/LSNTickable.h"
#include "../Wav/LSNWavFile.h"
#include "../Utilities/LSNDelayedValue.h"
#include "../Utilities/LSNRingBuffer.h"
#include "LSNApuBase.h"
#include "LSNApuUnit.h"
#include "LSNNoise.h"
#include "LSNPulse.h"
#include "LSNTriangle.h"

#include <filesystem>

#include <EEExpEval.h>

#define LSN_PULSE1_HALT_MASK							0b00100000
#define LSN_PULSE2_HALT_MASK							0b00100000
#define LSN_TRIANGLE_HALT_MASK							0b10000000
#define LSN_NOISE_HALT_MASK								0b00100000

#define LSN_PULSE1_ENABLED( THIS )						(((THIS)->m_ui8Registers[0x15] & 0b0001) != 0)
#define LSN_PULSE2_ENABLED( THIS )						(((THIS)->m_ui8Registers[0x15] & 0b0010) != 0)
#define LSN_TRIANGLE_ENABLED( THIS )					(((THIS)->m_ui8Registers[0x15] & 0b0100) != 0)
#define LSN_NOISE_ENABLED( THIS )						(((THIS)->m_ui8Registers[0x15] & 0b1000) != 0)

#define LSN_PULSE1_HALT									((m_ui8Registers[0x00] & LSN_PULSE1_HALT_MASK) != 0)
#define LSN_PULSE2_HALT									((m_ui8Registers[0x04] & LSN_PULSE2_HALT_MASK) != 0)
#define LSN_TRIANGLE_HALT								((m_ui8Registers[0x08] & LSN_TRIANGLE_HALT_MASK) != 0)
#define LSN_NOISE_HALT									((m_ui8Registers[0x0C] & LSN_NOISE_HALT_MASK) != 0)

#define LSN_PULSE1_USE_VOLUME							((m_ui8Registers[0x00] & 0b00010000) != 0)
#define LSN_PULSE2_USE_VOLUME							((m_ui8Registers[0x04] & 0b00010000) != 0)
#define LSN_NOISE_USE_VOLUME							((m_ui8Registers[0x0C] & 0b00010000) != 0)

#define LSN_PULSE1_ENV_DIVIDER( THIS )					((THIS)->m_ui8Registers[0x00] & 0b00001111)
#define LSN_PULSE2_ENV_DIVIDER( THIS )					((THIS)->m_ui8Registers[0x04] & 0b00001111)
#define LSN_NOISE_ENV_DIVIDER( THIS )					((THIS)->m_ui8Registers[0x0C] & 0b00001111)

// _bEven is false on 0 2 4 6 8, etc.  It goes by cycle count rather than cycle index.
#define LSN_APU_UPDATE									if constexpr ( !_bEven ) {																							\
															if ( m_bModeSwitch ) {																							\
																m_bModeSwitch = false;																						\
																if ( (m_dvRegisters3_4017.Value() & 0b10000000) != 0 ) {													\
																	 m_ui64StepCycles = _tM1S4_1 - 2;																		\
																	Tick_Mode1_Step4<_bEven, true>();																		\
																	m_pftTick = &CApu2A0X::Tick_Mode1_Step0<!_bEven, true>;													\
																	m_ui64StepCycles = 1;																					\
																}																											\
																else {																										\
																	m_ui64StepCycles = 0;																					\
																	Tick_Mode0_Step0<_bEven, false>();																		\
																}																											\
																if ( (m_dvRegisters3_4017.Value() & 0b01000000) != 0 ) {													\
																	m_piIrqTarget->ClearIrq( LSN_IS_APU );																	\
																}																											\
																return;																										\
															}																												\
															{																												\
																m_pPulse1.TickSequencer( LSN_PULSE1_ENABLED( this ) );														\
																m_pPulse2.TickSequencer( LSN_PULSE2_ENABLED( this ) );														\
																m_nNoise.TickSequencer( LSN_NOISE_ENABLED( this ) );														\
															}																												\
														}																													\
														m_tTriangle.TickSequencer( LSN_TRIANGLE_ENABLED( this ) );

#define LSN_4017_DELAY									(3+1)

#pragma warning( push )
#pragma warning( disable : 4324 )	// warning C4324: 'lsn::CApu2A0X<0,7457,14913,22371,29828,29829,29830,7457,14913,22371,29829,37281,37282,236250000,11,12,false>': structure was padded due to alignment specifier

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

	/** Register flags. */
	enum LSN_REG_FLAGS : uint64_t {
		LSN_RF_PULSE1									= (1 << 0),
		LSN_RF_PULSE2									= (1 << 1),
		LSN_RF_TRIANGLE									= (1 << 2),
		LSN_RF_NOISE									= (1 << 3),
		LSN_RF_DMC										= (1 << 4),
		LSN_RF_STATUS									= (1 << 5),
		LSN_RF_FRAME_COUNTER							= (1 << 6),

		LSN_RF_PULSE1_ON_OFF							= (1 << 7),
		LSN_RF_PULSE2_ON_OFF							= (1 << 8),
		LSN_RF_TRIANGKE_ON_OFF							= (1 << 9),
		LSN_RF_NOISE_ON_OFF								= (1 << 10),
	};

	/** Channel types. */
	enum LSN_CHANNELS {
		LSN_C_PULSE_0,
		LSN_C_PULSE_1,
		LSN_C_TRIANGLE,
		LSN_C_NOISE,
		LSN_C_DMC,
		LSN_C_TOTAL
	};

	/** Metadata types. */
	enum LSN_METADATA_TYPES {
		LSN_MT_REGISTER,
		LSN_MT_CHANNEL_ON_OFF,
	};

	/**
	 * Class CApu2A0X
	 * \brief The 2A0X series of APU's.
	 *
	 * Description: The 2A0X series of APU's.
	 */
	template <unsigned _tType, unsigned _tM0S0, unsigned _tM0S1, unsigned _tM0S2, unsigned _tM0S3_0, unsigned _tM0S3_1, unsigned _tM0S3_2,
		unsigned _tM1S0, unsigned _tM1S1, unsigned _tM1S2, unsigned _tM1S3, unsigned _tM1S4_0, unsigned _tM1S4_1,
		unsigned _tMasterClock, unsigned _tMasterDiv, unsigned _tApuDiv,
		bool _bSwapDuty>
	class CApu2A0X : public CTickable, public CApuBase {
		typedef CApu2A0X<_tType, _tM0S0, _tM0S1, _tM0S2, _tM0S3_0, _tM0S3_1, _tM0S3_2, _tM1S0, _tM1S1, _tM1S2, _tM1S3, _tM1S4_0, _tM1S4_1, _tMasterClock, _tMasterDiv, _tApuDiv, _bSwapDuty>
														CThisApu;
	public :
		CApu2A0X( CCpuBus * _pbBus, CInterruptable * _piIrqTarget ) :
			m_pbBus( _pbBus ),
			m_ui64Cycles( 0 ),
			m_ui64StepCycles( 0 ),
			m_ui64RawExportStartCycle( 0 ),
			m_piIrqTarget( _piIrqTarget ),
			m_dvRegisters3_4017( Set4017, this ),
			m_dvPulse1LengthCounter( Pulse1LengthCounter, this ),
			m_dvPulse2LengthCounter( Pulse2LengthCounter, this ),
			m_dvTriangleLengthCounter( TriangleLengthCounter, this ),
			m_dvNoiseLengthCounter( NoiseLengthCounter, this ),
			m_dvPulse1LengthCounterHalt( ChannelHaltLengthCounter<0x00, LSN_PULSE1_HALT_MASK>, this ),
			m_dvPulse2LengthCounterHalt( ChannelHaltLengthCounter<0x04, LSN_PULSE2_HALT_MASK>, this ),
			m_dvTriangleLengthCounterHalt( ChannelHaltLengthCounter<0x08, LSN_TRIANGLE_HALT_MASK>, this ),
			m_dvNoiseLengthCounterHalt( ChannelHaltLengthCounter<0x0C, LSN_NOISE_HALT_MASK>, this ),
			m_pwfRawStream( nullptr ),
			m_pwfOutStream( nullptr ),
			m_rbRingBuffer( 16 ),
			m_fSampleBoxLpf( 20000.0f ),
			m_fLpf( 20000.0f ),
			m_fHpf0( 194.0f ),
			m_fHpf1( 37.0f ),
			m_fHpf2( 37.0f ),
			m_fP1Vol( 1.0f ),
			m_fP2Vol( 1.0f ),
			m_fTVol( 1.0f ),
			m_fNVol( 1.0f ),
			m_fDmcVol( 1.0f ),
			m_fDmcRegVol( 0.0f ),
			m_bEnabled( true ) {

			m_pfLpf.CreateLpf( 20000.0f, HzAsFloat() );
			
		}
		~CApu2A0X() {
		}


		// == Types.
		typedef CDelayedValue<uint8_t, LSN_4017_DELAY>	DelayedVal;

		/** The register buffer. */
		LSN_ALIGN( 64 )
		struct LSN_REG_BUFFER {
			LSN_REG_BUFFER() {}
			LSN_REG_BUFFER( const LSN_REG_BUFFER &_rbOther ) {
				std::memcpy( ui8Registers, _rbOther.ui8Registers, sizeof( LSN_REG_BUFFER ) );
			}
			LSN_REG_BUFFER( LSN_REG_BUFFER && _rbOther ) {
				std::memcpy( ui8Registers, _rbOther.ui8Registers, sizeof( LSN_REG_BUFFER ) );
			}
			uint8_t										ui8Registers[0x17+1];
			double										dTime;
			LSN_METADATA_TYPES							mtType = LSN_MT_REGISTER;


			// == Operators.
			/**
			 * Assignment operator.
			 * 
			 * \param _rbOther The object to copy.
			 * \return Returns a reference to the copied object.
			 **/
			LSN_REG_BUFFER &							operator = ( const LSN_REG_BUFFER &_rbOther ) {
				std::memcpy( ui8Registers, _rbOther.ui8Registers, sizeof( LSN_REG_BUFFER ) );
				mtType = _rbOther.mtType;
				return (*this);
			}

			/**
			 * Assignment operator.
			 * 
			 * \param _pui8Other The object to copy.  Must be a byte array aligned to 64 bytes and must be 0x17+1 bytes long.
			 * \return Returns a reference to the copied object.
			 **/
			LSN_REG_BUFFER &							operator = ( const uint8_t * _pui8Other ) {
				std::memcpy( ui8Registers, _pui8Other, sizeof( ui8Registers ) );
				mtType = LSN_MT_REGISTER;
				return (*this);
			}


			// == Functions.
			/**
			 * Gets the Pulse 1 bits.
			 * 
			 * \return Returns the 32 Pulse 1 bits.
			 **/
			inline uint32_t								Pulse1() const { return (*reinterpret_cast<const uint32_t *>(&ui8Registers[0x00])); }

			/**
			 * Gets the Pulse 2 bits.
			 * 
			 * \return Returns the 32 Pulse 2 bits.
			 **/
			inline uint32_t								Pulse2() const { return (*reinterpret_cast<const uint32_t *>(&ui8Registers[0x04])); }

			/**
			 * Gets the Triangle bits.
			 * 
			 * \return Returns the 32 Triangle bits.
			 **/
			inline uint32_t								Triangle() const { return (*reinterpret_cast<const uint32_t *>(&ui8Registers[0x08])) & 0xFFFF00FF; }

			/**
			 * Gets the Noise bits.
			 * 
			 * \return Returns the 32 Noise bits.
			 **/
			inline uint32_t								Noise() const { return (*reinterpret_cast<const uint32_t *>(&ui8Registers[0x0C])) & 0b11111000100011110000000000111111; }

			/**
			 * Gets the DMC bits.
			 * 
			 * \return Returns the 32 DMC bits.
			 **/
			inline uint32_t								Dmc() const { return (*reinterpret_cast<const uint32_t *>(&ui8Registers[0x10])) & 0b11111111111111110111111111001111; }

			/**
			 * Gets the Status bits.
			 * 
			 * \return Returns the 5 Status bits.
			 **/
			inline uint8_t								Status() const { return ui8Registers[0x15] & 0b00011111; }

			/**
			 * Gets the Frame-Counter bits.
			 * 
			 * \return Returns the 2 Frame-Counter bits.
			 **/
			inline uint8_t								FrameCounter() const { return ui8Registers[0x17] & 0b11000000; }

			/**
			 * Sets to Pulse 1 on/off.
			 * 
			 * \param _bOn Whether noise was changed to on or not.
			 **/
			inline void									SetPulse1OnOff( bool _bOn ) {
				ui8Registers[0] = LSN_C_PULSE_0;
				ui8Registers[1] = _bOn;
				mtType = LSN_MT_CHANNEL_ON_OFF;
			}

			/**
			 * Sets to Pulse 2 on/off.
			 * 
			 * \param _bOn Whether noise was changed to on or not.
			 **/
			inline void									SetPulse2OnOff( bool _bOn ) {
				ui8Registers[0] = LSN_C_PULSE_1;
				ui8Registers[1] = _bOn;
				mtType = LSN_MT_CHANNEL_ON_OFF;
			}

			/**
			 * Sets to Noise on/off.
			 * 
			 * \param _bOn Whether noise was changed to on or not.
			 **/
			inline void									SetNoiseOnOff( bool _bOn ) {
				ui8Registers[0] = LSN_C_NOISE;
				ui8Registers[1] = _bOn;
				mtType = LSN_MT_CHANNEL_ON_OFF;
			}

			/**
			 * Compares against another set of registers using a bit mask to compare only specific components.
			 * 
			 * \param _rbOther The buffer against which to compare.
			 * \param _ui64Mask The bit mask indicating which channels/registers to compare.
			 * \return Returns true if the given buffer is equal to this one testing only the components specified by the mask.
			 **/
			inline bool									Equals( const LSN_REG_BUFFER &_rbOther, uint64_t _ui64Mask ) const {
				if ( (_ui64Mask & LSN_RF_PULSE1) && (Pulse1() != _rbOther.Pulse1()) ) { return false; }
				if ( (_ui64Mask & LSN_RF_PULSE2) && (Pulse2() != _rbOther.Pulse2()) ) { return false; }
				if ( (_ui64Mask & LSN_RF_TRIANGLE) && (Triangle() != _rbOther.Triangle()) ) { return false; }
				if ( (_ui64Mask & LSN_RF_NOISE) && (Noise() != _rbOther.Noise()) ) { return false; }
				if ( (_ui64Mask & LSN_RF_DMC) && (Dmc() != _rbOther.Dmc()) ) { return false; }
				if ( (_ui64Mask & LSN_RF_STATUS) && (Status() != _rbOther.Status()) ) { return false; }
				if ( (_ui64Mask & LSN_RF_FRAME_COUNTER) && (FrameCounter() != _rbOther.FrameCounter()) ) { return false; }
				return true;
			}

			/**
			 * Compares against another set of registers using a bit mask to compare only specific components.
			 * 
			 * \param _pui8Other The buffer against which to compare.
			 * \param _ui64Mask The bit mask indicating which channels/registers to compare.
			 * \return Returns true if the given buffer is equal to this one testing only the components specified by the mask.
			 **/
			inline bool									Equals( const uint8_t * _pui8Other, uint64_t _ui64Mask ) const {
				return Equals( (*reinterpret_cast<const LSN_REG_BUFFER *>(_pui8Other)), _ui64Mask );
			}
		};


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
			m_dvRegisters3_4017.Tick();

			m_dvPulse1LengthCounter.Tick();
			m_dvPulse2LengthCounter.Tick();
			m_dvTriangleLengthCounter.Tick();
			m_dvNoiseLengthCounter.Tick();

			m_dvPulse1LengthCounterHalt.Tick();
			m_dvPulse2LengthCounterHalt.Tick();
			m_dvTriangleLengthCounterHalt.Tick();
			m_dvNoiseLengthCounterHalt.Tick();

			(this->*m_pftTick)();
			m_pPulse1.UpdateSweeperState<1>();
			m_pPulse2.UpdateSweeperState<0>();
			
			

			if LSN_LIKELY( m_bEnabled ) {
				CAudio::InitSampleBox( m_fSampleBoxLpf, m_fHpf0,
					200,
					//CSampleBox::TransitionRangeToBandwidth( CSampleBox::TransitionRange( CAudio::GetOutputFrequency() ), CAudio::GetOutputFrequency() ) * 3,
					Hz(), CAudio::GetOutputFrequency() );
				CAudio::SampleBox().SetOutputCallback( PostHpf, this );
			

				/*bool bPulse1 = m_pPulse1.BasicallyOn( LSN_PULSE1_ENABLED( this ) );
				m_ChannelOutputting[LSN_C_PULSE_0] = bPulse1 != m_ChannelOutputtingStatus[LSN_C_PULSE_0];
				m_ChannelOutputtingStatus[LSN_C_PULSE_0] = bPulse1;
				bool bPulse2 = m_pPulse2.BasicallyOn( LSN_PULSE2_ENABLED( this ) );
				m_ChannelOutputting[LSN_C_PULSE_1] = bPulse2 != m_ChannelOutputtingStatus[LSN_C_PULSE_1];
				m_ChannelOutputtingStatus[LSN_C_PULSE_1] = bPulse2;*/
				bool bNoise = m_nNoise.ProducingSound( LSN_NOISE_ENABLED( this ) );
				m_ChannelOutputting[LSN_C_NOISE] = bNoise != m_ChannelOutputtingStatus[LSN_C_NOISE];
				m_ChannelOutputtingStatus[LSN_C_NOISE] = bNoise;
				bool bTriangle = m_tTriangle.ProducingSound( false );
				m_ChannelOutputting[LSN_C_TRIANGLE] = bTriangle != m_ChannelOutputtingStatus[LSN_C_TRIANGLE];
				m_ChannelOutputtingStatus[LSN_C_TRIANGLE] = bTriangle;

				float fPulse1 = (m_pPulse1.ProducingSound( LSN_PULSE1_ENABLED( this ) ) ? m_pPulse1.GetEnvelopeOutput( LSN_PULSE1_USE_VOLUME ) : 0.0f) * m_fP1Vol;
				// DEBUG.
				//fPulse1 = 0.0f;
				
				float fPulse2 = (m_pPulse2.ProducingSound( LSN_PULSE2_ENABLED( this ) ) ? m_pPulse2.GetEnvelopeOutput( LSN_PULSE2_USE_VOLUME ) : 0.0f) * m_fP2Vol;
				//fPulse2 = 0.0f;
				float fFinalPulse = fPulse1 + fPulse2;
				if ( fFinalPulse ) {
					fFinalPulse = 95.88f / ((8128.0f / fFinalPulse) + 100.0f);
				}
				
				float fNoise = (bNoise ? m_nNoise.GetEnvelopeOutput( LSN_NOISE_USE_VOLUME ) : 0.0f) * m_fNVol;
				float fTriangle = m_tTriangle.Output() * m_fTVol;
				float fDmc = m_fDmcRegVol * m_fDmcVol;

				//fFinalPulse = fNoise = fTriangle = 0.0f;

				// DEBUG.
				//fFinalPulse = fNoise = 0.0f;
				//fNoise = fTriangle = 0.0f;
				//fFinalPulse = fTriangle = 0.0f;

				//static uint64_t ui64CycleCnt = 0;
				//static bool bMadeSound = false;
				//static bool bWasJustMakingSound = false;
				//static uint32_t ui32OnOffCnt = 0;
				//static uint32_t ui32PrintSilenceCnt = 0;
				//if ( m_pPulse2.ProducingSound( LSN_PULSE2_ENABLED( this ) ) ) {
				//	if ( ui32PrintSilenceCnt >= 43520 && !fPulse2 ) {
				//		m_pPulse2.ProducingSound( LSN_PULSE2_ENABLED( this ) );
				//	}
				//	else if ( ui32PrintSilenceCnt >= 44880 && fPulse2 ) {
				//		m_pPulse2.ProducingSound( LSN_PULSE2_ENABLED( this ) );
				//	}
				//	ui32OnOffCnt += (bWasJustMakingSound == false) ? 1 : 0;
				//	bMadeSound = true;
				//	++ui64CycleCnt;
				//	bWasJustMakingSound = true;

				//	if ( ui32OnOffCnt > 120 /*&& (ui64CycleCnt % 1000) == 0*/ ) {
				//		::OutputDebugStringA( (std::string( "Noise: " ) + std::to_string( ui64CycleCnt ) + " " + std::to_string( ui32OnOffCnt ) + " " + std::to_string( fPulse2 ) + "\r\n").c_str() );
				//		//::OutputDebugStringA( "." );
				//	}
				//}
				//if ( bMadeSound && !m_pPulse2.ProducingSound( LSN_PULSE2_ENABLED( this ) ) ) {
				//	ui32OnOffCnt += (bWasJustMakingSound == true) ? 1 : 0;
				//	if ( ui32OnOffCnt > 120 ) {
				//		//::OutputDebugStringA( (std::string( "Silence: " ) + std::to_string( ui64CycleCnt ) + " " + std::to_string( ui32OnOffCnt ) + "\r\n").c_str() );
				//		//::OutputDebugStringA( " " );
				//		++ui32PrintSilenceCnt;
				//		if ( ui32PrintSilenceCnt >= 43520 ) {
				//		}
				//	}
				//	bWasJustMakingSound = false;
				//}


				fNoise /= 12241.0f;
				fTriangle /= 8227.0f;
				fDmc /= 22638.0f;
				float fFinalTnd = 0.0f;
				float fDenom = fNoise + fTriangle + fDmc;
				if ( fDenom ) {
					fFinalTnd = 159.79f / (1.0f / fDenom + 100.0f);
				}
			
				float fFinal = fFinalPulse + fFinalTnd;
				if LSN_LIKELY( m_pmbMapper ) {
					fFinal = m_pmbMapper->GetExtAudio( fFinal );
				}

				if LSN_UNLIKELY( m_pwfRawStream ) {
					m_pwfRawStream->AddStreamSample( fFinal );
				}

				fFinal = float( m_pfLpf.Process( fFinal ) );
				{
					for ( auto I = LSN_ELEMENTS( m_pfOutputPole ); I--; ) {
						fFinal = float( m_pfOutputPole[I].Process( fFinal ) );
					}
				}

				CAudio::AddSample( fFinal );
			}

			m_bRegModified = false;
			++m_ui64Cycles;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void											ResetAnalog() {
			m_ui8Registers[0x15] = 0x00;
			m_ui64Cycles = 0;
			m_ui64StepCycles = 0;
			m_ui64RawExportStartCycle = 0;
			CAudio::BeginEmulation();
			m_pftTick = &CApu2A0X::Tick_Mode0_Step0<false, false>;
			m_bModeSwitch = false;
			m_pPulse1.SetSeq( GetDuty( 0 ) );
			m_pPulse2.SetSeq( GetDuty( 0 ) );
			m_pPulse1.SetEnvelopeVolume( LSN_PULSE1_ENV_DIVIDER( this ) );
			m_pPulse2.SetEnvelopeVolume( LSN_PULSE2_ENV_DIVIDER( this ) );
			m_nNoise.SetEnvelopeVolume( LSN_NOISE_ENV_DIVIDER( this ) );
			Write4015( this, 0, 0, 0 );
			m_i64TicksToLenCntr = _tM0S0;
			if ( !m_bLetterless || _tType != LSN_AT_NTSC ) {
				m_dvRegisters3_4017.SetValue( m_ui8Last4017 | 0b01000000 );
			}

			m_bRegModified = false;

			m_ChannelOutputtingStatus[LSN_C_PULSE_0] = m_pPulse1.ProducingSound( LSN_PULSE1_ENABLED( this ) );
			m_ChannelOutputtingStatus[LSN_C_PULSE_1] = m_pPulse1.ProducingSound( LSN_PULSE2_ENABLED( this ) );
			m_ChannelOutputtingStatus[LSN_C_NOISE] = m_nNoise.ProducingSound( LSN_NOISE_ENABLED( this ) );

			m_ChannelOutputting[LSN_C_PULSE_0] = false;
			m_ChannelOutputting[LSN_C_PULSE_1] = false;
			m_ChannelOutputting[LSN_C_NOISE] = false;
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
			//m_ui8Registers[0x15] = 0x00;
			//m_dvRegisters3_4017.SetValue( 0x00 );
			m_pPulse1.ResetToKnown();
			m_pPulse2.ResetToKnown();
			m_nNoise.ResetToKnown();
			m_tTriangle.ResetToKnown();
			ResetAnalog();
			m_fDmcRegVol = 0.0f;
			m_dvRegisters3_4017.SetValue( 0x00 );
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

			m_pbBus->SetWriteFunc( 0x4010, Write4010, this, 0 );
			m_pbBus->SetWriteFunc( 0x4011, Write4011, this, 0 );
			m_pbBus->SetWriteFunc( 0x4012, Write4012, this, 0 );
			m_pbBus->SetWriteFunc( 0x4013, Write4013, this, 0 );

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

		/**
		 * Gets the audio frequency expressed as a double.
		 * 
		 * \return Returns _tMasterClock / _tMasterDiv / _tApuDiv.
		 **/
		inline constexpr double							Hz() const { return double( _tMasterClock ) / _tMasterDiv / _tApuDiv; }

		/**
		 * Gets the audio frequency expressed as a float.
		 * 
		 * \return Returns _tMasterClock / _tMasterDiv / _tApuDiv.
		 **/
		inline constexpr float							HzAsFloat() const { return float( Hz() ); }

		/**
		 * Sets the audio options.
		 * 
		 * \param _aoOptions The options to set.
		 **/
		void											SetOptions( const LSN_AUDIO_OPTIONS &_aoOptions ) {
			m_fVolume = _aoOptions.fVolume * _aoOptions.apCharacteristics.fVolume;
			if ( _aoOptions.apCharacteristics.bInvert ) {
				m_fVolume *= -1.0f;
			}
			m_bEnabled = m_fVolume != 0.0f && _aoOptions.bEnabled;

			float dMaxLpf = _aoOptions.ui32OutputHz / 2.0f + 100.0f;
			m_fLpf = _aoOptions.apCharacteristics.bLpfEnable ? std::min( dMaxLpf, _aoOptions.apCharacteristics.fLpf ) : dMaxLpf;
			m_fHpf0 = _aoOptions.apCharacteristics.bHpf0Enable ? _aoOptions.apCharacteristics.fHpf0 : 20.0f;
			m_fHpf1 = _aoOptions.apCharacteristics.fHpf1;
			m_fHpf2 = _aoOptions.apCharacteristics.fHpf2;

			m_pfLpf.CreateLpf( m_fLpf, HzAsFloat() );
			for ( auto I = LSN_ELEMENTS( m_pfOutputPole ); I--; ) {
				//float fLpf = (std::min( CAudio::GetOutputFrequency() / 2.0f, 20000.0f ) + I * 10.0f);
				float fLpf = _aoOptions.ui32OutputHz / 2.0f + 100.0f;
				m_pfOutputPole[I].CreateLpf( fLpf, HzAsFloat() );
			}

			m_hfHpfFilter1.SetEnabled( _aoOptions.apCharacteristics.bHpf1Enable );
			m_hfHpfFilter2.SetEnabled( _aoOptions.apCharacteristics.bHpf2Enable );
			m_fSampleBoxLpf = (22000.0f / 22050.0f) * (_aoOptions.ui32OutputHz / 2.0f);

			m_fP1Vol = _aoOptions.apCharacteristics.fP1Volume;
			m_fP2Vol = _aoOptions.apCharacteristics.fP2Volume;
			m_fTVol = _aoOptions.apCharacteristics.fTVolume;
			m_fNVol = _aoOptions.apCharacteristics.fNVolume;
			m_fDmcVol = _aoOptions.apCharacteristics.fDmcVolume;
			m_bLetterless = _aoOptions.apCharacteristics.bRp2A02;
			if ( m_bLetterless ) {
				m_nNoise.SetModeFlag( false );
			}
		}

		/**
		 * Sets the raw stream-to-file pointer.
		 * 
		 * \param _pfStream The pointer to set for streaming the raw signal to a file.
		 **/
		inline void										SetRawStream( CWavFile * _pfStream ) {
			m_pwfRawStream = _pfStream;
		}

		/**
		 * Sets the output stream-to-file pointer.
		 * 
		 * \param _pfStream The pointer to set for streaming the output signal to a file.
		 **/
		inline void										SetOutStream( CWavFile * _pfStream ) {
			m_pwfOutStream = _pfStream;
		}

		/**
		 * Sets as inactive (another system is being played).
		 **/
		virtual void									SetAsInactive() {
			m_pwfOutStream = m_pwfRawStream = nullptr;
		}

		/**
		 * Gets the channel-switched-on-off settings for each channel.
		 * 
		 * \return Returns the channel-switched-on-off settings for each channel.
		 **/
		const bool *									GetChannelOnOff() const { return m_ChannelOutputting; }

		/**
		 * Gets the $4010 register value.
		 * 
		 * \return Returns the $4010 register value.
		 **/
		inline uint8_t									Get4010() const { return m_ui8Registers[0x10]; }

		/**
		 * Gets the $4012 register value.
		 * 
		 * \return Returns the $4012 register value.
		 **/
		inline uint8_t									Get4012() const { return m_ui8Registers[0x12]; }

		/**
		 * Gets the $4013 register value.
		 * 
		 * \return Returns the $4013 register value.
		 **/
		inline uint8_t									Get4013() const { return m_ui8Registers[0x13]; }

		/**
		 * The add-metadata function.  Fetches the current registers from a ring buffer (because the associated samples may be out-of-date).
		 * 
		 * \param _pvParm A pointer to an object of this class.
		 * \param _sStream A reference to the streaming options.
		 **/
		//static bool LSN_STDCALL							AddMetaDataFunc_RingBuffer( void * _pvParm, CWavFile::LSN_STREAMING &_sStream ) {
		//	CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm);

		//	auto rbTmp = paApu->m_rbRingBuffer.Pop_Ref();
		//	if ( !paApu->m_rbLastRegBufferOut.Equals( rbTmp, _sStream.ui64MetaParm ) || _sStream.ui64MetaWritten == 0 ) {
		//		// Add the buffer to be consumed by the thread later.
		//		//paApu->m_vRegBuffersOut.push_back( rbTmp );
		//		_sStream.vMetaBuffer.insert( _sStream.vMetaBuffer.end(), rbTmp.ui8Registers, rbTmp.ui8Registers + sizeof( LSN_REG_BUFFER ) );

		//		paApu->m_rbLastRegBufferOut = rbTmp;
		//		++_sStream.ui64MetaWritten;
		//		return true;
		//	}
		//	else {
		//		paApu->m_rbRingBuffer.Pop_Discard();
		//		return false;
		//	}
		//}

		/**
		 * The add-metadata function for raw streams.  Fetches from the current registers, since the associated sample stream is always current.
		 * 
		 * \param _pvParm A pointer to an object of this class.
		 * \param _sStream A reference to the streaming options.
		 **/
		static bool LSN_STDCALL							AddMetaDataFunc_Raw( void * _pvParm, CWavFile::LSN_STREAMING &_sStream ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm);
			
			bool bRet = false;
			if ( !paApu->m_rbLastRegBufferRaw.Equals( paApu->m_ui8Registers, _sStream.ui64MetaParm ) || _sStream.ui64MetaWritten == 0 ) {
				if LSN_UNLIKELY( _sStream.ui64MetaWritten == 0 ) { paApu->m_ui64RawExportStartCycle = paApu->m_ui64Cycles; }
				
				size_t sIdx = _sStream.vMetaBuffer.size();
				_sStream.vMetaBuffer.insert( _sStream.vMetaBuffer.end(), paApu->m_ui8Registers, paApu->m_ui8Registers + sizeof( LSN_REG_BUFFER ) );
				(*reinterpret_cast<LSN_REG_BUFFER *>(&_sStream.vMetaBuffer[sIdx])).dTime = (paApu->m_ui64Cycles - paApu->m_ui64RawExportStartCycle) / std::ceil( paApu->Hz() );
				(*reinterpret_cast<LSN_REG_BUFFER *>(&_sStream.vMetaBuffer[sIdx])).mtType = LSN_MT_REGISTER;

				paApu->m_rbLastRegBufferRaw = paApu->m_ui8Registers;

				bRet = true;
			}
			if LSN_UNLIKELY( (_sStream.ui64MetaParm & LSN_RF_PULSE1_ON_OFF) && paApu->GetChannelOnOff()[LSN_C_PULSE_0] ) {
				if LSN_UNLIKELY( _sStream.ui64MetaWritten == 0 ) { paApu->m_ui64RawExportStartCycle = paApu->m_ui64Cycles; }

				LSN_REG_BUFFER rbTmp;
				rbTmp.dTime = (paApu->m_ui64Cycles - paApu->m_ui64RawExportStartCycle) / std::ceil( paApu->Hz() );
				rbTmp.SetPulse1OnOff( paApu->m_ChannelOutputtingStatus[LSN_C_PULSE_0] );

				_sStream.vMetaBuffer.insert( _sStream.vMetaBuffer.end(), reinterpret_cast<uint8_t *>(&rbTmp), reinterpret_cast<uint8_t *>(&rbTmp) + sizeof( LSN_REG_BUFFER ) );
				bRet = true;
			}
			if LSN_UNLIKELY( (_sStream.ui64MetaParm & LSN_RF_PULSE2_ON_OFF) && paApu->GetChannelOnOff()[LSN_C_PULSE_1] ) {
				if LSN_UNLIKELY( _sStream.ui64MetaWritten == 0 ) { paApu->m_ui64RawExportStartCycle = paApu->m_ui64Cycles; }

				LSN_REG_BUFFER rbTmp;
				rbTmp.dTime = (paApu->m_ui64Cycles - paApu->m_ui64RawExportStartCycle) / std::ceil( paApu->Hz() );
				rbTmp.SetPulse2OnOff( paApu->m_ChannelOutputtingStatus[LSN_C_PULSE_1] );

				_sStream.vMetaBuffer.insert( _sStream.vMetaBuffer.end(), reinterpret_cast<uint8_t *>(&rbTmp), reinterpret_cast<uint8_t *>(&rbTmp) + sizeof( LSN_REG_BUFFER ) );
				bRet = true;
			}
			if LSN_UNLIKELY( (_sStream.ui64MetaParm & LSN_RF_NOISE_ON_OFF) && paApu->GetChannelOnOff()[LSN_C_NOISE] ) {
				if LSN_UNLIKELY( _sStream.ui64MetaWritten == 0 ) { paApu->m_ui64RawExportStartCycle = paApu->m_ui64Cycles; }

				LSN_REG_BUFFER rbTmp;
				rbTmp.dTime = (paApu->m_ui64Cycles - paApu->m_ui64RawExportStartCycle) / std::ceil( paApu->Hz() );
				rbTmp.SetNoiseOnOff( paApu->m_ChannelOutputtingStatus[LSN_C_NOISE] );

				_sStream.vMetaBuffer.insert( _sStream.vMetaBuffer.end(), reinterpret_cast<uint8_t *>(&rbTmp), reinterpret_cast<uint8_t *>(&rbTmp) + sizeof( LSN_REG_BUFFER ) );
				bRet = true;
			}
			return bRet;
		}

		/**
		 * The thread callback for actually writing the values to the file.
		 * 
		 * \param _pwfWavFile A pointer to the underlying CWavFile object.
		 * \param _pvParm A pointer to an object of this class.
		 * \param _sStream A reference to the streaming options.
		 **/
		static void LSN_STDCALL							MetaDataThreadFunc( CWavFile * /*_pwfWavFile*/, void * /*_pvParm*/, CWavFile::LSN_STREAMING &_sStream,
			std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>> &_vInput, std::vector<uint8_t> &_vTmpBuffer ) {
			_vTmpBuffer.clear();
			//CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm);

			size_t sTotal = _vInput.size() / sizeof( LSN_REG_BUFFER );
			LSN_REG_BUFFER * prbInput = reinterpret_cast<LSN_REG_BUFFER *>(_vInput.data());

			LSN_REG_BUFFER * prbLast = _sStream.vMetaThreadScratch.size() ? reinterpret_cast<LSN_REG_BUFFER *>(_sStream.vMetaThreadScratch.data()) : nullptr;
			for ( size_t I = 0; I < sTotal; ++I ) {
				std::string sBinaryTmp;
				sBinaryTmp = std::to_string( _sStream.ui64MetaThreadParm ) + ":";
				if ( prbInput[I].mtType == LSN_MT_REGISTER ) {
					if ( !prbLast ||
						((_sStream.ui64MetaParm & LSN_RF_PULSE1) && prbLast->Pulse1() != prbInput[I].Pulse1()) ) {
					
						sBinaryTmp += " Pu1: " + ee::CExpEval::ToBinary( prbInput[I].Pulse1(), 4 * 8 );
					}
					if ( !prbLast ||
						((_sStream.ui64MetaParm & LSN_RF_PULSE2) && prbLast->Pulse2() != prbInput[I].Pulse2()) ) {
					
						sBinaryTmp += " Pu2: " + ee::CExpEval::ToBinary( prbInput[I].Pulse2(), 4 * 8 );
					}
					if ( !prbLast ||
						((_sStream.ui64MetaParm & LSN_RF_TRIANGLE) && prbLast->Triangle() != prbInput[I].Triangle()) ) {
					
						sBinaryTmp += " Tri: " + ee::CExpEval::ToBinary( prbInput[I].Triangle(), 4 * 8 );
					}
					if ( !prbLast ||
						((_sStream.ui64MetaParm & LSN_RF_NOISE) && prbLast->Noise() != prbInput[I].Noise()) ) {
					
						sBinaryTmp += " Noi: " + ee::CExpEval::ToBinary( prbInput[I].Noise(), 4 * 8 );
					}
					if ( !prbLast ||
						((_sStream.ui64MetaParm & LSN_RF_DMC) && prbLast->Dmc() != prbInput[I].Dmc()) ) {
					
						sBinaryTmp += " DMC: " + ee::CExpEval::ToBinary( prbInput[I].Dmc(), 4 * 8 );
					}
					if ( !prbLast ||
						((_sStream.ui64MetaParm & LSN_RF_STATUS) && prbLast->Status() != prbInput[I].Status()) ) {
					
						sBinaryTmp += " Sts: " + ee::CExpEval::ToBinary( prbInput[I].Status(), 1 * 8 );
					}
					if ( !prbLast ||
						((_sStream.ui64MetaParm & LSN_RF_FRAME_COUNTER) && prbLast->FrameCounter() != prbInput[I].FrameCounter()) ) {
					
						sBinaryTmp += " FrC: " + ee::CExpEval::ToBinary( prbInput[I].FrameCounter(), 1 * 8 );
					}
				}
				else if ( prbInput[I].mtType == LSN_MT_CHANNEL_ON_OFF ) {
					switch ( prbInput[I].ui8Registers[0] ) {
						case LSN_C_PULSE_0 : {
							sBinaryTmp = " Pu1: " + std::string( prbInput[I].ui8Registers[1] ? "On" : "Off" );
							break;
						}
						case LSN_C_PULSE_1 : {
							sBinaryTmp = " Pu2: " + std::string( prbInput[I].ui8Registers[1] ? "On" : "Off" );
							break;
						}
						case LSN_C_NOISE : {
							sBinaryTmp = " Noi: " + std::string( prbInput[I].ui8Registers[1] ? "On" : "Off" );
							break;
						}
					}
				}
				//if ( sBinaryTmp.size() ) {
					std::string sFinal = std::format( "{0:.27f}\t{0:.27f}\t", prbInput[I].dTime ) + "[" + sBinaryTmp + "]\r\n";
					_vTmpBuffer.insert( _vTmpBuffer.end(), sFinal.data(), sFinal.data() + sFinal.size() );

					prbLast = &prbInput[I];
					++_sStream.ui64MetaThreadParm;
				//}
			}
			_sStream.vMetaThreadScratch.resize( sizeof( LSN_REG_BUFFER ) );
			(*reinterpret_cast<LSN_REG_BUFFER *>(_sStream.vMetaThreadScratch.data())) = (*prbLast);
			_sStream.sfMetaFile.WriteToFile( _vTmpBuffer );
		}

		

	protected :
		// == Types.
		/** A function pointer for the tick handlers. */
		typedef void (CApu2A0X:: *						PfTicks)();
		typedef CPoleFilter								CPoleFilterLpf;
		typedef CHpfFilter								CPoleFilterHpf;


		// == Members.
		/** The cycle counter. */
		uint64_t										m_ui64Cycles;
		/** The step cycle counter. */
		uint64_t										m_ui64StepCycles;
		/** Approximate number of ticks until length counters are ticked. */
		int64_t											m_i64TicksToLenCntr;
		/** The starting cycle of raw WAV exports. */
		uint64_t										m_ui64RawExportStartCycle;
		/** The main bus. */
		CCpuBus *										m_pbBus;
		/** The IRQ target. */
		CInterruptable *								m_piIrqTarget;
		/** WAV streamer for the source signal. */
		CWavFile *										m_pwfRawStream;
		/** WAV streamer for the output signal. */
		CWavFile *										m_pwfOutStream;
		/** The current cycle function. */
		PfTicks											m_pftTick;
		/** Final volume multiplier. */
		float											m_fVolume = (-1.0f);		
		/** The 37-Hz pole filter. */
		CPoleFilterHpf									m_pfPole37;
		/** The 14-Hz pole filter. */
		CPoleFilterLpf									m_pfLpf;
		/** The output Hz filter. */
		CPoleFilterLpf									m_pfOutputPole[1];
		/** A sanitization HPF. */
		CHpfFilter										m_hfHpfFilter1;
		/** A sanitization HPF. */
		CHpfFilter										m_hfHpfFilter2;
		/** The sample-box LPF frequency. */
		float											m_fSampleBoxLpf;
		/** The LPF frequency. */
		float											m_fLpf;
		/** The HPF0 frequency. */
		float											m_fHpf0;
		/** The HPF1 frequency. */
		float											m_fHpf1;
		/** The HPF2 frequency. */
		float											m_fHpf2;
		/** The Pulse 1 master volume. */
		float											m_fP1Vol;
		/** The Pulse 2 master volume. */
		float											m_fP2Vol;
		/** The Triangle master volume. */
		float											m_fTVol;
		/** The Noise master volume. */
		float											m_fNVol;
		/** The Dmc master volume. */
		float											m_fDmcVol;
		/** DMC voluem (from register 4011, not settings). */
		float											m_fDmcRegVol;
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
		LSN_ALIGN( 64 )
		uint8_t											m_ui8Registers[0x17+1];
		/** Ring-buffer of register buffers for metadata streaming. */
		CRingBuffer<LSN_REG_BUFFER>						m_rbRingBuffer;
		/** The last set of registers seen during metadata streaming from the output stream. */
		LSN_REG_BUFFER									m_rbLastRegBufferOut;
		/** The last set of registers seen during metadata streaming from raw values. */
		LSN_REG_BUFFER									m_rbLastRegBufferRaw;
		/** The buffered array of metadata chunks to eventually be converted to text and flushed to disk for output capture. */
		std::vector<LSN_REG_BUFFER, CAlignmentAllocator<LSN_REG_BUFFER, 64>>
														m_vRegBuffersOut;
		/** The buffered array of metadata chunks to eventually be converted to text and flushed to disk for raw capture. */
		std::vector<LSN_REG_BUFFER, CAlignmentAllocator<LSN_REG_BUFFER, 64>>
														m_vRegBuffersRaw;
		/** Set to true upon a write to $4017. */
		bool											m_bModeSwitch;
		/** Audio setting: Enabled. */
		bool											m_bEnabled = true;
		/** Register was written this cycle. */
		bool											m_bRegModified = false;
		/** The old letterless buggy RP2A03 version. */
		bool											m_bLetterless = false;
		/** On/off status tracker for each channel. */
		bool											m_ChannelOutputting[5];
		/** Lats on/off status for each channel. */
		bool											m_ChannelOutputtingStatus[5];

		/** The last value written to $4017. */
		uint8_t											m_ui8Last4017 = 0;
		
		/** Length-counter delay for Pulse 1. */
		CDelayedValue<uint8_t, 2>						m_dvPulse1LengthCounter;
		/** Length-counter delay for Pulse 2. */
		CDelayedValue<uint8_t, 2>						m_dvPulse2LengthCounter;
		/** Length-counter delay for Triangle. */
		CDelayedValue<uint8_t, 2>						m_dvTriangleLengthCounter;
		/** Length-counter delay for Noise. */
		CDelayedValue<uint8_t, 2>						m_dvNoiseLengthCounter;

		/** Halt delay for Pulse 1. */
		CDelayedValue<uint8_t, 2>						m_dvPulse1LengthCounterHalt;
		/** Halt delay for Pulse 2. */
		CDelayedValue<uint8_t, 2>						m_dvPulse2LengthCounterHalt;
		/** Halt delay for Triangle. */
		CDelayedValue<uint8_t, 2>						m_dvTriangleLengthCounterHalt;
		/** Halt delay for Noise. */
		CDelayedValue<uint8_t, 2>						m_dvNoiseLengthCounterHalt;


		// == Functions.
		/** Mode-0 step-0 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode0_Step0() {
			m_i64TicksToLenCntr = static_cast<int64_t>(_tM0S0);
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S0 ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode0_Step1<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step0<!_bEven, _bMode>;
			}
		}

		/** Mode-0 step-1 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode0_Step1() {
			m_i64TicksToLenCntr = static_cast<int64_t>(_tM0S1 - m_ui64StepCycles - 1);
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S1 ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

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
			m_i64TicksToLenCntr = static_cast<int64_t>(_tM0S2);
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM0S2 ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode0_Step3<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode0_Step2<!_bEven, _bMode>;
			}
		}

		/** Mode-0 step-3 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode0_Step3() {
			m_i64TicksToLenCntr = static_cast<int64_t>((_tM0S3_2 - 1) - m_ui64StepCycles - 1);
			LSN_APU_UPDATE;
			if ( m_ui64StepCycles >= (_tM0S3_2 - 3) && (m_dvRegisters3_4017.Value() & 0b01000000) == 0 ) {
				m_piIrqTarget->Irq( LSN_IS_APU );
			}

			if ( (m_ui64StepCycles + 1) == (_tM0S3_2 - 1) ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

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
			m_i64TicksToLenCntr = static_cast<int64_t>(_tM1S0);
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S0 ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode1_Step1<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step0<!_bEven, _bMode>;
			}
		}

		/** Mode-1 step-1 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode1_Step1() {
			m_i64TicksToLenCntr = static_cast<int64_t>(_tM1S1 - m_ui64StepCycles - 1);
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S1 ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

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
			m_i64TicksToLenCntr = static_cast<int64_t>(_tM1S2);
			LSN_APU_UPDATE;

			if ( ++m_ui64StepCycles == _tM1S2 ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );
				
				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

				m_pftTick = &CApu2A0X::Tick_Mode1_Step3<!_bEven, _bMode>;
			}
			else {
				m_pftTick = &CApu2A0X::Tick_Mode1_Step2<!_bEven, _bMode>;
			}
		}

		/** Mode-1 step-3 tick function. */
		template <bool _bEven, bool _bMode>
		void											Tick_Mode1_Step3() {
			m_i64TicksToLenCntr = static_cast<int64_t>(_tM1S3);
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
			m_i64TicksToLenCntr = static_cast<int64_t>((_tM1S4_1 - 1) - m_ui64StepCycles - 1);
			LSN_APU_UPDATE;

			if ( (m_ui64StepCycles + 1) == (_tM1S4_1 - 1) ) {
				m_tTriangle.TickLinearCounter( LSN_TRIANGLE_HALT );

				m_pPulse1.TickLengthCounter( LSN_PULSE1_ENABLED( this ), LSN_PULSE1_HALT );
				m_pPulse2.TickLengthCounter( LSN_PULSE2_ENABLED( this ), LSN_PULSE2_HALT );
				m_nNoise.TickLengthCounter( LSN_NOISE_ENABLED( this ), LSN_NOISE_HALT );
				m_tTriangle.TickLengthCounter( LSN_TRIANGLE_ENABLED( this ), LSN_TRIANGLE_HALT );

				m_pPulse1.TickEnvelope( LSN_PULSE1_USE_VOLUME, LSN_PULSE1_HALT );
				m_pPulse2.TickEnvelope( LSN_PULSE2_USE_VOLUME, LSN_PULSE2_HALT );
				m_nNoise.TickEnvelope( LSN_NOISE_USE_VOLUME, LSN_NOISE_HALT );

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
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm);
			paApu->m_bModeSwitch = true;
		}

		/**
		 * Writing to 0x4000 (Duty, loop envelope/disable length counter, constant volume, envelope period/volume).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			//paApu->m_ui8Registers[0x00] = _ui8Val;
			paApu->m_ui8Registers[0x00] = (_ui8Val & ~LSN_PULSE1_HALT_MASK) | (paApu->m_ui8Registers[0x00] & LSN_PULSE1_HALT_MASK);
			paApu->m_dvPulse1LengthCounterHalt.WriteWithDelay( _ui8Val & LSN_PULSE1_HALT_MASK );
			paApu->m_pPulse1.SetSeq( GetDuty( _ui8Val >> 6 ) );
			paApu->m_pPulse1.SetEnvelopeVolume( _ui8Val & 0b1111 );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4001 (Sweep unit: enabled, period, negative, shift count).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4001( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x01] = _ui8Val;
			paApu->m_pPulse1.SetSweepEnabled( ((_ui8Val & 0b10000000) != 0) );
			paApu->m_pPulse1.SetSweepPeriod( (_ui8Val >> 4) & 0b111 );
			paApu->m_pPulse1.SetSweepNegate( ((_ui8Val & 0b1000) != 0) );
			paApu->m_pPulse1.SetSweepShift( _ui8Val & 0b111 );
			paApu->m_pPulse1.DirtySweeper();
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4002 (Timer low).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4002( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x02] = _ui8Val;
			paApu->m_pPulse1.SetTimerLow( _ui8Val );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4003 (Length counter load, timer high (also resets duty and starts envelope)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4003( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x03] = _ui8Val;
			paApu->m_pPulse1.SetTimerHigh( _ui8Val );
			if ( LSN_PULSE1_ENABLED( paApu ) ) {
				//if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_pPulse1.GetLengthCounter()) ) {
				//	paApu->m_pPulse1.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
				//}
				paApu->m_dvPulse1LengthCounter.WriteWithDelay( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_pPulse1.RestartEnvelope();
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4004 (Duty, loop envelope/disable length counter, constant volume, envelope period/volume).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4004( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			//paApu->m_ui8Registers[0x04] = _ui8Val;
			paApu->m_ui8Registers[0x04] = (_ui8Val & ~LSN_PULSE2_HALT_MASK) | (paApu->m_ui8Registers[0x04] & LSN_PULSE2_HALT_MASK);
			paApu->m_dvPulse2LengthCounterHalt.WriteWithDelay( _ui8Val & LSN_PULSE2_HALT_MASK );
			paApu->m_pPulse2.SetSeq( GetDuty( _ui8Val >> 6 ) );
			paApu->m_pPulse2.SetEnvelopeVolume( _ui8Val & 0b1111 );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4005 (Sweep unit: enabled, period, negative, shift count).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4005( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x05] = _ui8Val;
			paApu->m_pPulse2.SetSweepEnabled( ((_ui8Val & 0b10000000) != 0) );
			paApu->m_pPulse2.SetSweepPeriod( (_ui8Val >> 4) & 0b111 );
			paApu->m_pPulse2.SetSweepNegate( ((_ui8Val & 0b1000) != 0) );
			paApu->m_pPulse2.SetSweepShift( _ui8Val & 0b111 );
			paApu->m_pPulse2.DirtySweeper();
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4006 (Timer low).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4006( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x06] = _ui8Val;
			paApu->m_pPulse2.SetTimerLow( _ui8Val );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4007 (Length counter load, timer high (also resets duty and starts envelope)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x07] = _ui8Val;
			paApu->m_pPulse2.SetTimerHigh( _ui8Val );
			if ( LSN_PULSE2_ENABLED( paApu ) ) {
				//if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_pPulse2.GetLengthCounter()) ) {
				//	paApu->m_pPulse2.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
				//}
				paApu->m_dvPulse2LengthCounter.WriteWithDelay( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_pPulse2.RestartEnvelope();
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4008 (Length counter disable/linear counter control, linear counter reload value).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4008( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			//paApu->m_ui8Registers[0x08] = _ui8Val;
			paApu->m_ui8Registers[0x08] = (_ui8Val & ~LSN_TRIANGLE_HALT_MASK) | (paApu->m_ui8Registers[0x08] & LSN_TRIANGLE_HALT_MASK);
			paApu->m_dvTriangleLengthCounterHalt.WriteWithDelay( _ui8Val & LSN_TRIANGLE_HALT_MASK );
			paApu->m_tTriangle.SetLinearCounter( _ui8Val & 0b01111111 );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x400A (Timer low).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write400A( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x0A] = _ui8Val;
			paApu->m_tTriangle.SetTimerLow( _ui8Val );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x400B (Length counter load, timer high (also reloads linear counter)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write400B( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x0B] = _ui8Val;
			paApu->m_tTriangle.SetTimerHigh( _ui8Val, false );
			if ( LSN_TRIANGLE_ENABLED( paApu ) ) {
				//if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_tTriangle.GetLengthCounter()) ) {
				//	paApu->m_tTriangle.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
				//}
				paApu->m_dvTriangleLengthCounter.WriteWithDelay( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_tTriangle.SetLinearReload();
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x400C (Loop envelope/disable length counter, constant volume, envelope period/volume).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write400C( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			//paApu->m_ui8Registers[0x0C] = _ui8Val;
			paApu->m_ui8Registers[0x0C] = (_ui8Val & ~LSN_NOISE_HALT_MASK) | (paApu->m_ui8Registers[0x0C] & LSN_NOISE_HALT_MASK);
			paApu->m_dvNoiseLengthCounterHalt.WriteWithDelay( _ui8Val & LSN_NOISE_HALT_MASK );
			paApu->m_nNoise.SetEnvelopeVolume( _ui8Val & 0b1111 );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x400E (Loop noise, noise period).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write400E( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x0E] = _ui8Val;
			if LSN_UNLIKELY( paApu->m_bLetterless ) {
				paApu->m_nNoise.SetTimer( CApuUnit::OldNoiseTable<0>( (_ui8Val & 0b00001111) ) );
				paApu->m_nNoise.SetModeFlag( false );
			}
			else {
				paApu->m_nNoise.SetTimer( CApuUnit::NoiseTable<_tType>( (_ui8Val & 0b00001111) ) );
				paApu->m_nNoise.SetModeFlag( (_ui8Val & 0b10000000) != 0 );
			}
			
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x400F (Length counter load (also starts envelope)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write400F( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x0F] = _ui8Val;
			if ( LSN_NOISE_ENABLED( paApu ) ) {
				//if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_nNoise.GetLengthCounter()) ) {
				//	paApu->m_nNoise.SetLengthCounter( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
				//}
				paApu->m_dvNoiseLengthCounter.WriteWithDelay( CApuUnit::LenTable( (_ui8Val /*& 0b11111000*/) >> 3 ) );
			}
			paApu->m_nNoise.RestartEnvelope();
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4010 (Flags and Rate).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4010( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x10] = _ui8Val;
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4011 (Direct load).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4011( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x11] = _ui8Val;
			paApu->m_fDmcRegVol = float( _ui8Val & 0b01111111 );
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4012 (Sample address).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4012( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x12] = _ui8Val;
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4013 (Sample address).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4013( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x13] = _ui8Val;
			paApu->m_bRegModified = true;
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
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
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

			if ( paApu->m_piIrqTarget->GetIrqStatus( LSN_IS_APU ) ) {
				_ui8Ret |= 0b01000000;
				paApu->m_piIrqTarget->ClearIrq( LSN_IS_APU );
			}
		}

		/**
		 * Writing to 0x4015 (Status: DMC interrupt, frame interrupt, length counter status: noise, triangle, pulse 2, pulse 1 (read)).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4015( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			paApu->m_ui8Registers[0x15] = _ui8Val;

			if ( !LSN_PULSE1_ENABLED( paApu ) ) {
				paApu->m_pPulse1.SetLengthCounter( 0 );
				//paApu->m_dvPulse1LengthCounter.WriteWithDelay( 0 );
			}
			if ( !LSN_PULSE2_ENABLED( paApu ) ) {
				paApu->m_pPulse2.SetLengthCounter( 0 );
				//paApu->m_dvPulse2LengthCounter.WriteWithDelay( 0 );
			}
			if ( !LSN_TRIANGLE_ENABLED( paApu ) ) {
				paApu->m_tTriangle.SetLengthCounter( 0 );
				//paApu->m_dvTriangleLengthCounter.WriteWithDelay( 0 );
			}
			if ( !LSN_NOISE_ENABLED( paApu ) ) {
				paApu->m_nNoise.SetLengthCounter( 0 );
				//paApu->m_dvNoiseLengthCounter.WriteWithDelay( 0 );
			}
			if LSN_LIKELY( paApu->m_piIrqTarget ) {
				paApu->m_piIrqTarget->ClearIrq( LSN_IS_APU_DMC );
			}
			paApu->m_bRegModified = true;
		}

		/**
		 * Writing to 0x4017 (Frame counter: 5-frame sequence, disable frame interrupt).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4017( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvParm0);
			// * If the write occurs during an APU cycle, the effects occur 3 CPU cycles after the $4017 write cycle, and if the write occurs between APU cycles, the effects occurs 4 CPU cycles after the write cycle.
			// "During" an APU cycle means every even CPU cycle.  "Between" APU cycles means every odd CPU cycle.
			// This is handled by having Set4017() set m_bModeSwitch, which will then be seen only on even ticks.
			paApu->m_ui8Registers[0x17] = _ui8Val;
			paApu->m_dvRegisters3_4017.WriteWithDelay( _ui8Val );
			paApu->m_ui8Last4017 = _ui8Val;
			paApu->m_bRegModified = true;
		}

		/**
		 * Gets a duty sequence bit mask.
		 * 
		 * \param _ui8Duty The duty cycle index.
		 * \return Returns the bit sequence for the given pulse duty cycle.
		 **/
		static inline uint8_t							GetDuty( uint8_t _ui8Duty ) {
			if constexpr ( _bSwapDuty ) {
				static const uint8_t ui8Seqs[] = {
					0b10000000,
					0b11110000,
					0b11000000,
					0b00111111,
				};
				return ui8Seqs[_ui8Duty];
			}
			else {
				static const uint8_t ui8Seqs[] = {
					0b10000000,
					0b11000000,
					0b11110000,
					0b00111111,
				};
				return ui8Seqs[_ui8Duty];
			}
		}

		/**
		 *  Applies a 2nd HPF to the output.
		 *
		 * \param _pvThis The this pointer.
		 * \param _fSample The input sample.
		 * \param _ui32Hz The output Hz.
		 * \return Returns _fSample.
		 */
		static float									PostHpf( void * _pvThis, float _fSample, uint32_t _ui32Hz ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvThis);
			auto fHz = float( _ui32Hz );
			if LSN_LIKELY ( paApu->m_hfHpfFilter1.CreateHpf( paApu->m_fHpf1, fHz ) ) {
				if LSN_LIKELY( paApu->m_hfHpfFilter2.CreateHpf( paApu->m_fHpf2, fHz ) ) {
					auto fSample = float( paApu->m_hfHpfFilter1.Process( _fSample ) );

					if LSN_LIKELY( paApu->m_pmbMapper ) {
						fSample = paApu->m_pmbMapper->PostProcessAudioSample( fSample, fHz );
					}
					fSample = float( paApu->m_hfHpfFilter2.Process( fSample ) );
					fSample *= paApu->m_fVolume;
					if LSN_UNLIKELY( paApu->m_pwfOutStream ) {
						paApu->m_pwfOutStream->AddStreamSample( fSample );
					}
					return fSample;
				}
			}
			return _fSample * paApu->m_fVolume;
		}

		/**
		 * Updates the length counter for Pulse 1.
		 * 
		 * \param _pvObj A pointer to this object.
		 * \param _ui8NewVal The new value to set.
		 * \param _ui8OldVal The previous value.
		 **/
		static void										Pulse1LengthCounter( void * _pvObj, uint8_t _ui8NewVal, uint8_t /*_ui8OldVal*/ ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvObj);
			if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_pPulse1.GetLengthCounter()) ) {
				paApu->m_pPulse1.SetLengthCounter( _ui8NewVal );
			}
			//paApu->m_pPulse1.SetLengthCounter( _ui8NewVal );
		}

		/**
		 * Updates the length counter for Pulse 2.
		 * 
		 * \param _pvObj A pointer to this object.
		 * \param _ui8NewVal The new value to set.
		 * \param _ui8OldVal The previous value.
		 **/
		static void										Pulse2LengthCounter( void * _pvObj, uint8_t _ui8NewVal, uint8_t /*_ui8OldVal*/ ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvObj);
			if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_pPulse2.GetLengthCounter()) ) {
				paApu->m_pPulse2.SetLengthCounter( _ui8NewVal );
			}
			//paApu->m_pPulse2.SetLengthCounter( _ui8NewVal );
		}

		/**
		 * Updates the length counter for Triangle.
		 * 
		 * \param _pvObj A pointer to this object.
		 * \param _ui8NewVal The new value to set.
		 * \param _ui8OldVal The previous value.
		 **/
		static void										TriangleLengthCounter( void * _pvObj, uint8_t _ui8NewVal, uint8_t /*_ui8OldVal*/ ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvObj);
			if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_tTriangle.GetLengthCounter()) ) {
				paApu->m_tTriangle.SetLengthCounter( _ui8NewVal );
			}
			//paApu->m_tTriangle.SetLengthCounter( _ui8NewVal );
		}

		/**
		 * Updates the length counter for Noise.
		 * 
		 * \param _pvObj A pointer to this object.
		 * \param _ui8NewVal The new value to set.
		 * \param _ui8OldVal The previous value.
		 **/
		static void										NoiseLengthCounter( void * _pvObj, uint8_t _ui8NewVal, uint8_t /*_ui8OldVal*/ ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvObj);
			if ( !(paApu->m_i64TicksToLenCntr == 0 && paApu->m_nNoise.GetLengthCounter()) ) {
				paApu->m_nNoise.SetLengthCounter( _ui8NewVal );
			}
			//paApu->m_nNoise.SetLengthCounter( _ui8NewVal );
		}

		/**
		 * Halts length counter for a channel.
		 * 
		 * \param _pvObj A pointer to this object.
		 * \param _ui8NewVal The new value to set.
		 * \param _ui8OldVal The previous value.
		 **/
		template <unsigned _uReg, uint8_t _ui8Flag>
		static void										ChannelHaltLengthCounter( void * _pvObj, uint8_t _ui8NewVal, uint8_t /*_ui8OldVal*/ ) {
			CThisApu * paApu = reinterpret_cast<CThisApu *>(_pvObj);
			if ( _ui8NewVal ) {
				paApu->m_ui8Registers[_uReg] |= _ui8Flag;
			}
			else {
				paApu->m_ui8Registers[_uReg] &= ~_ui8Flag;
			}
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

#pragma warning( pop )
