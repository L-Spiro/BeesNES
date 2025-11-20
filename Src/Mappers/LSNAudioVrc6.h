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
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::WriteVrc6_9000_A000_B000, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0x9001 || ui16ddr == 0xA001 || ui16ddr == 0xB001 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::WriteVrc6_9001_A001_B001, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0x9002 || ui16ddr == 0xA002 || ui16ddr == 0xB002 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::WriteVrc6_9002_A002_B002, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0x9003 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudioVrc6::WriteVrc6_9003, this, uint16_t( I ) );
				}
			}
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
		}

		/**
		 * Does a full power-on reset
		 **/
		void											ResetFull( bool /*_bToKnown = false*/ ) {

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
			return 0.0f;
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
		static void LSN_FASTCALL						WriteVrc6_9000_A000_B000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
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
		static void LSN_FASTCALL						WriteVrc6_9001_A001_B001( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
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
		static void LSN_FASTCALL						WriteVrc6_9002_A002_B002( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CAudioVrc6 * pmThis = reinterpret_cast<CAudioVrc6 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			switch ( ui16ddr ) {
				case 0x9002 : {		// Pulse 1 freq high + enable.
					//pmThis->Vrc6_SetPulseHigh( pmThis->m_vVrc6Pulse[0], _ui8Val );
					break;
				}
				case 0xA002 : {		// Pulse 2 freq high + enable.
					//pmThis->Vrc6_SetPulseHigh( pmThis->m_vVrc6Pulse[1], _ui8Val );
					break;
				}
				case 0xB002 : {		// Saw freq high + enable.
					//pmThis->Vrc6_SetSawHigh( _ui8Val );
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
		static void LSN_FASTCALL						WriteVrc6_9003( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudioVrc6 * pmThis = reinterpret_cast<CAudioVrc6 *>(_pvParm0);
			// Keep only the ..ABH bits.
			pmThis->m_ui8Vrc6FreqCtrl = (_ui8Val & 0b00000111);
		}
	};

}	// namespace lsn
