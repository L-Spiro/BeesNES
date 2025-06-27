/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Sunsoft 5B audio.
 */


#pragma once

#include "../LSNLSpiroNes.h"


namespace lsn {

	/**
	 * Class CAudio5b
	 * \brief Sunsoft 5B audio.
	 *
	 * Description: Sunsoft 5B audio.
	 */
	class CAudio5b {
	public :
		CAudio5b() {
		}
		virtual ~CAudio5b() {
		}


		// == Functions.


	protected :
		// == Members.
#pragma pack( push, 1 )
		union {
			uint8_t														m_ui8Registers[16];						/**< Each register. */
			struct {
				uint16_t												m_ui16Tone[3];							/**< Tone channels A, B, and C. */
				uint8_t													m_ui8NoisePeriod;						/**< 5-bit noise period. */
				uint8_t													m_ui8Disable;							/**< Noise and tone disablers. */
				uint8_t													m_ui8EnvAndVol[3];						/**< Envelopes and olume levels for tone channels. */
				uint16_t												m_ui16EnvPeriod;						/**< Envelope period. */
				uint8_t													m_ui8EnvShape;							/**< Envelope shape, continue, attack, alternate, and hold. */
				uint8_t													m_ui8Io[2];								/**< IO ports. */
			};
		};
#pragma pack( pop )
	};

}	// namespace lsn
