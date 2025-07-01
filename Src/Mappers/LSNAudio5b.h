/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Sunsoft 5B audio.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"

#include <cassert>


#pragma warning( push )
#pragma warning( disable : 4201 )	// warning C4201: nonstandard extension used: nameless struct/union


namespace lsn {

#pragma pack( push, 1 )
	struct LSN_5B_REGS {
		uint16_t										ui16Tone[3];							/**< Tone channels A, B, and C. */
		uint8_t											ui8NoisePeriod;							/**< 5-bit noise period. */
		uint8_t											ui8Disable;								/**< Noise and tone disablers. */
		uint8_t											ui8EnvAndVol[3];						/**< Envelopes and olume levels for tone channels. */
		uint16_t										ui16EnvPeriod;							/**< Envelope period. */
		uint8_t											ui8EnvShape;							/**< Envelope shape, continue, attack, alternate, and hold. */
		uint8_t											ui8Io[2];								/**< IO ports. */
	};
#pragma pack( pop )

	/**
	 * Class CAudio5b
	 * \brief Sunsoft 5B audio.
	 *
	 * Description: Sunsoft 5B audio.
	 */
	class CAudio5b {
	public :
		CAudio5b() {
			ResetFull();
		}
		virtual ~CAudio5b() {
		}


		// == Functions.
		/**
		 * Applies mapping to the CPU bus.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 */
		void											ApplyMap( CCpuBus * _pbCpuBus ) {
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudio5b::SelectRegisterC000_DFFF, this, 0 );
			}
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudio5b::WriteRegisterE000_FFFF, this, 0 );
			}
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			// Easy to unwind loop.
			m_tTones[0].Tick( m_rRegs.ui16Tone[0] );
			m_tTones[1].Tick( m_rRegs.ui16Tone[1] );
			m_tTones[2].Tick( m_rRegs.ui16Tone[2] );

		}

		/**
		 * Does a full power-on reset
		 **/
		void											ResetFull( bool _bToKnown = false ) {
			if ( _bToKnown ) {
				std::memset( m_ui8Registers, 0, sizeof( m_ui8Registers ) );
				std::memset( m_tTones, 0, sizeof( m_tTones ) );
				//m_ui16Counter = 0;
				m_ui8Reg = 0;
			}
			m_rRegs.ui16Tone[0] &= 0x0FFF;
			m_rRegs.ui16Tone[1] &= 0x0FFF;
			m_rRegs.ui16Tone[2] &= 0x0FFF;
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
			float fRet = 0.0f;
			if ( !(m_rRegs.ui8Disable & 0b00000001) ) {
				fRet += float( m_tTones[0].bOnOff ) * (m_rRegs.ui8EnvAndVol[0] & 0xF);
			}
			if ( !(m_rRegs.ui8Disable & 0b00000010) ) {
				fRet += float( m_tTones[1].bOnOff ) * (m_rRegs.ui8EnvAndVol[1] & 0xF);
			}
			if ( !(m_rRegs.ui8Disable & 0b00000100) ) {
				fRet += float( m_tTones[2].bOnOff ) * (m_rRegs.ui8EnvAndVol[2] & 0xF);
			}

			return fRet * (1.0f / 63.0f);
		}

	protected :
		// == Types.
		/** The tone channels. */
		struct LSN_TONE {
			uint16_t									ui16Counter;							/**< The channel counter. */
			uint8_t										u8Divisor;								/**< The count to 16. */
			bool										bOnOff;									/**< Whether the pulse is 0 or non-0. */


			// == Functions.
			/**
			 * Advances the tone state by 1 cycle.
			 * 
			 * \param _ui1Period The tone period.
			 **/
			void										Tick( uint16_t _ui1Period ) {
				if LSN_UNLIKELY( (++u8Divisor & 0xF) == 0 ) {
					if LSN_UNLIKELY( ++ui16Counter >= _ui1Period ) {
						ui16Counter = 0;
						bOnOff = !bOnOff;
					}
				}
			}
		};


		// == Members.
		LSN_TONE										m_tTones[3];							/**< The tone channels. */
		union {
			uint8_t										m_ui8Registers[16];						/**< Each register. */
			LSN_5B_REGS									m_rRegs;								/**< Alternate names for registers. */
		};

		static_assert( offsetof( lsn::LSN_5B_REGS, ui16EnvPeriod ) == 11, "ui16EnvPeriod must be at offset 11." );
		static_assert( offsetof( lsn::LSN_5B_REGS, ui8Io ) == 14, "ui8Io must be at offset 14." );

		//uint16_t										m_ui16Counter = 0;						/**< The counter. */
		uint8_t											m_ui8Reg = 0;							/**< The register to which to write. */


		// == Functions.
		/**
		 * Selects an audio register.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectRegisterC000_DFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudio5b * pmThis = reinterpret_cast<CAudio5b *>(_pvParm0);
			pmThis->m_ui8Reg = _ui8Val;
		}
		/**
		 * Writes to the selected audio register.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						WriteRegisterE000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudio5b * pmThis = reinterpret_cast<CAudio5b *>(_pvParm0);
			if ( (pmThis->m_ui8Reg & 0b11110000) == 0 ) {
				uint8_t ui8Reg = pmThis->m_ui8Reg & 0b00001111;
				switch ( ui8Reg ) {
					case 0x01 : {							// Channel A high period.
						pmThis->m_ui8Registers[0x01] = _ui8Val & 0b00001111;
						break;
					}
					case 0x03 : {							// Channel B high period.
						pmThis->m_ui8Registers[0x03] = _ui8Val & 0b00001111;
						break;
					}
					case 0x05 : {							// Channel C high period.
						pmThis->m_ui8Registers[0x05] = _ui8Val & 0b00001111;
						break;
					}
					case 0x06 : {							// Noise period.
						pmThis->m_ui8Registers[0x06] = _ui8Val & 0b00011111;
						break;
					}
					case 0x07 : {							// Noise disable on channels C/B/A, Tone disable on channels c/b/a.
						pmThis->m_ui8Registers[0x07] = _ui8Val & 0b00111111;
						break;
					}
					case 0x08 : {							// Channel A envelope enable (E), volume (V).
						pmThis->m_ui8Registers[0x08] = _ui8Val & 0b00011111;
						break;
					}
					case 0x09 : {							// Channel B envelope enable (E), volume (V).
						pmThis->m_ui8Registers[0x09] = _ui8Val & 0b00011111;
						break;
					}
					case 0x0A : {							// Channel C envelope enable (E), volume (V).
						pmThis->m_ui8Registers[0x0A] = _ui8Val & 0b00011111;
						break;
					}
					case 0x0D : {							// Envelope reset and shape: continue (C), attack (A), alternate (a), hold (H).
						pmThis->m_ui8Registers[0x0D] = _ui8Val & 0b00001111;
						break;
					}
					default : {
						pmThis->m_ui8Registers[ui8Reg] = _ui8Val;
					}
				}
			}
		}
	};

}	// namespace lsn

#pragma warning( pop )
