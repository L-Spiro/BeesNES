/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: VRC6 audio.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
//#include "../Utilities/LSNUtilities.h"


namespace lsn {

	/**
	 * Class CAudioVrc6
	 * \brief VRC6 audio.
	 *
	 * Description: VRC6 audio.
	 */
	class CAudioVrc6 {
	public :
		// == Types.
		typedef uint16_t (*								PfSwizzle)( uint16_t );


		// == Constructors.
		CAudioVrc6( PfSwizzle &_pfSiwzzle ) :
			m_pfSwizzleFunc( _pfSiwzzle ) {
			ResetFull( false );
		}
		virtual ~CAudioVrc6() {
		}


		


		// == Functions.
		/**
		 * Applies mapping to the CPU bus.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 */
		void											ApplyMap( CCpuBus * _pbCpuBus ) {
			// ================
			// BANK-SELECT
			// ================
			for ( uint32_t I = 0x8000; I <= 0xFFFF; ++I ) {
				uint16_t ui16ddr = m_pfSwizzleFunc( uint16_t( I ) );
				// VRC6 audio registers ($9000-$9003, $A000-$A002, $B000-$B002).
				if ( ui16ddr == 0x9000 || ui16ddr == 0xA000 || ui16ddr == 0xB000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::Write9000_A000_B000, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0x9001 || ui16ddr == 0xA001 || ui16ddr == 0xB001 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::Write9001_A001_B001, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0x9002 || ui16ddr == 0xA002 || ui16ddr == 0xB002 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::Write9002_A002_B002, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0x9003 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::Write9003, this, uint16_t( I ) );
				}
			}
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			m_fSample = TickAudioInternal();
		}

		/**
		 * Does a full power-on reset
		 **/
		void											ResetFull( bool _bToKnown = false ) {
			if ( _bToKnown ) {
				m_ui8Vrc6FreqCtrl = 0;
				for ( size_t I = 0; I < LSN_ELEMENTS( m_vVrc6Pulse ); ++I ) {
					m_vVrc6Pulse[I].ui8Control = 0;
					m_vVrc6Pulse[I].ui8FreqLow = 0;
					m_vVrc6Pulse[I].ui8FreqHigh = 0;
					m_vVrc6Pulse[I].ui16Divider = 0;
					m_vVrc6Pulse[I].ui8DutyStep = 0;
				}
				m_vVrc6Saw.ui8Rate = 0;
				m_vVrc6Saw.ui8FreqLow = 0;
				m_vVrc6Saw.ui8FreqHigh = 0;
				m_vVrc6Saw.ui16Divider = 0;
				m_vVrc6Saw.ui8Accumulator = 0;
				m_vVrc6Saw.ui8Step = 13;
			}
		}

		/**
		 * Does a soft reset.
		 **/
		void											ResetSoft() {
			/*std::memset( m_ui8Registers, 0, sizeof( m_ui8Registers ) );
			m_ui16Counter = 0;
			m_ui8Reg = 0;*/
		}

		/**
		 * Gets the extended-audio sample.
		 * 
		 * \return Returns the current sample.
		 **/
		inline float									Sample() {
			return m_fSample;
		}

		/**
		 * Post-process an output sample.  Applies the volume crunch.
		 * 
		 * \param _fSample The sample to modify.
		 * \return Returns the volume-crunched sample.
		 **/
		inline float									PostProcessSample( float _fSample, float /*_fHz*/ ) {
			return _fSample;
		}

	protected :
		// == Types.
		/** A VRC6 pulse channel. */
		struct LSN_VRC6_PULSE {
			uint8_t										ui8Control = 0;								/**< $9000/$A000: MDDD VVVV. */
			uint8_t										ui8FreqLow = 0;								/**< Low 8 bits of the 12-bit period. */
			uint8_t										ui8FreqHigh = 0;							/**< High 4 bits of period + E bit (bit 7). */
			uint16_t									ui16Divider = 0;							/**< Current timer divider value. */
			uint8_t										ui8DutyStep = 0;							/**< Duty sequence position [0..15]. */
		};

		/** The VRC6 saw channel. */
		struct LSN_VRC6_SAW {
			uint8_t										ui8Rate = 0;								/**< $B000: ..AA AAAA accumulator rate. */
			uint8_t										ui8FreqLow = 0;								/**< Low 8 bits of the 12-bit period. */
			uint8_t										ui8FreqHigh = 0;							/**< High 4 bits of period + E bit (bit 7). */
			uint16_t									ui16Divider = 0;							/**< Current timer divider value. */
			uint8_t										ui8Accumulator = 0;							/**< 8-bit internal accumulator. */
			uint8_t										ui8Step = 13;								/**< 14-step sequence position, init so first clock resets. */
		};


		// == Members.
		PfSwizzle &										m_pfSwizzleFunc;							/**< The swizzle function. */
		LSN_VRC6_PULSE									m_vVrc6Pulse[2];							/** VRC6 pulse channels (Pulse 1 and Pulse 2). */
		LSN_VRC6_SAW									m_vVrc6Saw;									/** VRC6 saw channel. */
		float											m_fSample = 0.0f;							/**< The last sample output. */
		uint8_t											m_ui8Vrc6FreqCtrl = 0;						/** Global VRC6 frequency control ($9003: ..ABH). */
		

		// == Functions.
		/**
		 * VRC6 $9000/$A000/$B000 write (pulse duty/volume and saw accumulator rate).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 The CPU address being written.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write9000_A000_B000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudioVrc6 * pmThis = reinterpret_cast<CAudioVrc6 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			switch ( ui16ddr ) {
				case 0x9000 : {		// Pulse 1 control: MDDD VVVV.
					pmThis->m_vVrc6Pulse[0].ui8Control = _ui8Val;
					break;
				}
				case 0xA000 : {		// Pulse 2 control: MDDD VVVV.
					pmThis->m_vVrc6Pulse[1].ui8Control = _ui8Val;
					break;
				}
				case 0xB000 : {		// Saw accumulator rate ..AA AAAA.
					pmThis->m_vVrc6Saw.ui8Rate = (_ui8Val & 0b00111111);
					break;
				}
			}
		}

		/**
		 * VRC6 $9001/$A001/$B001 write (low 8 bits of 12-bit period).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 The CPU address being written.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write9001_A001_B001( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudioVrc6 * pmThis = reinterpret_cast<CAudioVrc6 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			switch ( ui16ddr ) {
				case 0x9001 : {		// Pulse 1 period low.
					pmThis->m_vVrc6Pulse[0].ui8FreqLow = _ui8Val;
					break;
				}
				case 0xA001 : {		// Pulse 2 period low.
					pmThis->m_vVrc6Pulse[1].ui8FreqLow = _ui8Val;
					break;
				}
				case 0xB001 : {		// Saw period low.
					pmThis->m_vVrc6Saw.ui8FreqLow = _ui8Val;
					break;
				}
			}
		}

		/**
		 * VRC6 $9002/$A002/$B002 write (high 4 bits of 12-bit period + enable E).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 The CPU address being written.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write9002_A002_B002( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudioVrc6 * pmThis = reinterpret_cast<CAudioVrc6 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			switch ( ui16ddr ) {
				case 0x9002 : {		// Pulse 1 freq high + enable.
					pmThis->SetPulseHigh( pmThis->m_vVrc6Pulse[0], _ui8Val );
					break;
				}
				case 0xA002 : {		// Pulse 2 freq high + enable.
					pmThis->SetPulseHigh( pmThis->m_vVrc6Pulse[1], _ui8Val );
					break;
				}
				case 0xB002 : {		// Saw freq high + enable.
					pmThis->SetSawHigh( _ui8Val );
					break;
				}
			}
		}

		/**
		 * VRC6 $9003 write (global frequency scaling and halt).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 The CPU address being written.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write9003( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudioVrc6 * pmThis = reinterpret_cast<CAudioVrc6 *>(_pvParm0);
			// Keep only the ..ABH bits.
			pmThis->m_ui8Vrc6FreqCtrl = (_ui8Val & 0b00000111);
		}

		/**
		 * Sets the high frequency/enable byte for a VRC6 pulse channel.
		 *
		 * \param _rCh The channel to modify.
		 * \param _ui8Val The new high byte value.
		 */
		inline void									SetPulseHigh( LSN_VRC6_PULSE &_rCh, uint8_t _ui8Val ) {
			bool bWasEnabled = ((_rCh.ui8FreqHigh & 0x80) != 0);
			_rCh.ui8FreqHigh = _ui8Val;
			bool bIsEnabled = ((_rCh.ui8FreqHigh & 0x80) != 0);
			if ( !bIsEnabled ) {
				// Disable: reset and halt duty so phase can be restarted when re-enabled.
				_rCh.ui8DutyStep = 0;
				_rCh.ui16Divider = 0;
			}
			else if ( !bWasEnabled && bIsEnabled ) {
				// Rising edge of enable: restart from beginning.
				_rCh.ui8DutyStep = 0;
				_rCh.ui16Divider = 0;
			}
		}

		/**
		 * Sets the high frequency/enable byte for the VRC6 saw channel.
		 *
		 * \param _ui8Val The new high byte value.
		 */
		inline void									SetSawHigh( uint8_t _ui8Val ) {
			bool bWasEnabled = ((m_vVrc6Saw.ui8FreqHigh & 0x80) != 0);
			m_vVrc6Saw.ui8FreqHigh = _ui8Val;
			bool bIsEnabled = ((m_vVrc6Saw.ui8FreqHigh & 0x80) != 0);
			if ( !bIsEnabled ) {
				// Disable: accumulator forced to zero while disabled.
				m_vVrc6Saw.ui8Accumulator = 0;
			}
			else if ( !bWasEnabled && bIsEnabled ) {
				// Rising edge of enable: mostly reset phase.
				m_vVrc6Saw.ui8Accumulator = 0;
				m_vVrc6Saw.ui8Step = 13;	// Next clock will be step 0, which resets accumulator.
			}
		}

		/**
		 * Combines the low and high frequency bytes into a 12-bit period.
		 *
		 * \param _ui8Low The low 8 bits of the period.
		 * \param _ui8High The high 4 bits of the period in bits [3..0].
		 * \return Returns the 12-bit period.
		 */
		static inline uint16_t						CombinePeriod( uint8_t _ui8Low, uint8_t _ui8High ) {
			return uint16_t( _ui8Low ) | (uint16_t( _ui8High & 0x0F ) << 8);
		}

		/**
		 * Applies the global frequency scaling (AB bits of $9003) to the given 12-bit period.
		 *
		 * \param _ui16Period The unscaled 12-bit period.
		 * \return Returns the scaled period value.
		 */
		inline uint16_t								ScaledPeriod( uint16_t _ui16Period ) const {
			uint8_t ui8Ctrl = m_ui8Vrc6FreqCtrl;
			// 256x overrides 16x.
			if ( ui8Ctrl & 0b00000100 ) { return uint16_t( _ui16Period >> 8 ); }
			if ( ui8Ctrl & 0b00000010 ) { return uint16_t( _ui16Period >> 4 ); }
			return _ui16Period;
		}

		/**
		 * Ticks a single VRC6 pulse channel by 1 CPU cycle (timer + duty).
		 *
		 * \param _rCh The channel to tick.
		 * \param _bHalt Whether the global halt flag (H) is set.
		 */
		inline void									TickPulseChannel( LSN_VRC6_PULSE &_rCh, bool _bHalt ) {
			if ( _bHalt ) { return; }
			// If channel disabled, timer/duty are halted.
			if ( (_rCh.ui8FreqHigh & 0x80) == 0 ) { return; }

			uint16_t ui16Period = CombinePeriod( _rCh.ui8FreqLow, _rCh.ui8FreqHigh ) & 0x0FFF;
			uint16_t ui16Effective = ScaledPeriod( ui16Period );

			if ( _rCh.ui16Divider == 0 ) {
				_rCh.ui16Divider = ui16Effective;
				// Duty generator: 16-step sequence.
				_rCh.ui8DutyStep = uint8_t( (_rCh.ui8DutyStep + 1) & 0x0F );
			}
			else {
				--_rCh.ui16Divider;
			}
		}

		/**
		 * Computes the 4-bit output of a VRC6 pulse channel.
		 *
		 * \param _rCh The channel to sample.
		 * \return Returns the 4-bit pulse output in the range [0..15].
		 */
		static inline uint8_t						PulseOutput( const LSN_VRC6_PULSE &_rCh ) {
			// E bit clears output immediately when low.
			if ( (_rCh.ui8FreqHigh & 0x80) == 0 ) { return 0; }

			uint8_t ui8Vol = (_rCh.ui8Control & 0x0F);
			if ( ui8Vol == 0 ) { return 0; }

			// Mode bit: when set, ignore duty and output constant volume.
			if ( _rCh.ui8Control & 0x80 ) { return ui8Vol; }

			uint8_t ui8Duty = ((_rCh.ui8Control >> 4) & 0x07);
			// Duty generator: step is in [0..15]. Output volume when step <= D, else 0.
			if ( _rCh.ui8DutyStep <= ui8Duty ) { return ui8Vol; }
			return 0;
		}

		/**
		 * Ticks the VRC6 saw channel by 1 CPU cycle (timer + accumulator).
		 *
		 * \param _bHalt Whether the global halt flag (H) is set.
		 */
		inline void									TickSaw( bool _bHalt ) {
			if ( _bHalt ) { return; }

			uint16_t ui16Period = CombinePeriod( m_vVrc6Saw.ui8FreqLow, m_vVrc6Saw.ui8FreqHigh ) & 0x0FFF;
			uint16_t ui16Effective = ScaledPeriod( ui16Period );

			if ( m_vVrc6Saw.ui16Divider == 0 ) {
				m_vVrc6Saw.ui16Divider = ui16Effective;

				// Divider reached zero: clock the 14-step accumulator sequence.
				m_vVrc6Saw.ui8Step = uint8_t( (m_vVrc6Saw.ui8Step + 1) % 14 );

				// Accumulator reacts every 2 clocks; even steps modify the accumulator.
				if ( (m_vVrc6Saw.ui8Step & 0x01) == 0 ) {
					if ( m_vVrc6Saw.ui8Step == 0 ) {
						// 14th step: reset accumulator.
						m_vVrc6Saw.ui8Accumulator = 0;
					}
					else {
						// Even step (2,4,6,8,10,12): add A to accumulator.
						m_vVrc6Saw.ui8Accumulator = uint8_t( m_vVrc6Saw.ui8Accumulator + (m_vVrc6Saw.ui8Rate & 0x3F) );
					}
				}
			}
			else { --m_vVrc6Saw.ui16Divider; }

			// If disabled, accumulator is forced to zero until E is set again.
			if ( (m_vVrc6Saw.ui8FreqHigh & 0x80) == 0 ) { m_vVrc6Saw.ui8Accumulator = 0; }
		}

		/**
		 * Computes the 5-bit output of the VRC6 saw channel.
		 *
		 * \return Returns the 5-bit saw output in the range [0..31].
		 */
		inline uint8_t								SawOutput() const {
			if ( (m_vVrc6Saw.ui8FreqHigh & 0x80) == 0 ) { return 0; }
			// Output high 5 bits of accumulator.
			return uint8_t( m_vVrc6Saw.ui8Accumulator >> 3 );
		}

		/**
		 * Internal helper for Tick().
		 *
		 * \return Returns the linear DAC output mapped from [0..63] to [0.0f..1.0f].
		 */
		inline float								TickAudioInternal() {
			const bool bHalt = ( (m_ui8Vrc6FreqCtrl & 0x01) != 0 );

			// Advance oscillators.
			TickPulseChannel( m_vVrc6Pulse[0], bHalt );
			TickPulseChannel( m_vVrc6Pulse[1], bHalt );
			TickSaw( bHalt );

			// Sample outputs.
			uint8_t ui8P0 = PulseOutput( m_vVrc6Pulse[0] );
			uint8_t ui8P1 = PulseOutput( m_vVrc6Pulse[1] );
			uint8_t ui8Saw = SawOutput();

			// Final mix is a 6-bit DAC summing the 2Å~4-bit pulses and 5-bit saw.
			uint32_t ui32Mix = uint32_t( ui8P0 ) + uint32_t( ui8P1 ) + uint32_t( ui8Saw );
			// Map [0..63] (actual max 61) to [0.0f..1.0f].
			return float( ui32Mix ) * (1.0f / 63.0f);
		}
	};

}	// namespace lsn
