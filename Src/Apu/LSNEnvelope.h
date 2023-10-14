/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU envelope.  Counts a timer down to determine the timing of an event.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CEnvelope
	 * \brief An APU envelope.
	 *
	 * Description: An APU envelope.  Counts a timer down to determine the timing of an event
	 */
	class CEnvelope {
	public :
		inline CEnvelope();
		inline ~CEnvelope();


		// == Functions.
		/**
		 * Updates the envelope.
		 * 
		 * \param _bUseVolume Determines if envelopes are used or just a solid volume.
		 * \param _bLoop The loop flag.
		 * \return Returns the envelope value after any updating that needs to be done.
		 **/
		inline uint8_t								TickEnvelope( bool _bUseVolume, bool _bLoop );

		/**
		 * Restarts the envelope.
		 **/
		inline void									RestartEnvelope();

		/**
		 * Sets the envelope volume.
		 * 
		 * \param _ui8Vol The new envelope volume.
		 **/
		inline void									SetEnvelopeVolume( uint8_t _ui8Vol );

		/**
		 * Gets the envelope output value.
		 * 
		 * \param _bUseVolume Determines if envelopes are used or just a solid volume.
		 * \return Returns the envelope output value.
		 **/
		inline uint8_t								GetEnvelopeOutput( bool _bUseVolume ) const;

		/**
		 * Resets the unit to a known state.
		 **/
		inline void									ResetToKnown();


	protected :
		// == Members.
		/** The final output value. */
		uint8_t										m_ui8Output					= 0;
		/** The decay count. */
		uint8_t										m_ui8DecayCounter			= 0;
		/** The divider count. */
		uint8_t										m_ui8DividerCounter			= 0;
		/** The volume. */
		uint8_t										m_ui8Volume					= 0;
		/** If set, re-initialize the state on the next tick. */
		bool										m_bRestart					= true;

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CEnvelope::CEnvelope() {
	}
	inline CEnvelope::~CEnvelope() {
	}

	// == Functions.
	/**
	 * Updates the envelope.
	 * 
	 * \param _bUseVolume Determines if envelopes are used or just a solid volume.
	 * \param _bLoop The loop flag.
	 * \return Returns the envelope value after any updating that needs to be done.
	 **/
	inline uint8_t CEnvelope::TickEnvelope( bool /*_bUseVolume*/, bool _bLoop ) {
		/**
		 * When clocked by the frame counter, one of two actions occurs: if the start flag is clear, the divider is clocked, otherwise the start flag is cleared, the decay level counter is loaded with 15, and the divider's period is immediately reloaded.
		 * When the divider is clocked while at 0, it is loaded with V and clocks the decay level counter. Then one of two actions occurs: If the counter is non-zero, it is decremented, otherwise if the loop flag is set, the decay level counter is loaded with 15.
		 * The envelope unit's volume output depends on the constant volume flag: if set, the envelope parameter directly sets the volume, otherwise the decay level is the current volume. The constant volume flag has no effect besides selecting the volume source; the decay level will still be updated when constant volume is selected.
		 */
		if ( m_bRestart ) {
			// otherwise the start flag is cleared, the decay level counter is loaded with 15, and the divider's period is immediately reloaded.
			m_bRestart = false;
			m_ui8DividerCounter = m_ui8Volume;
			m_ui8DecayCounter = 15;
		}
		else {
			// if the start flag is clear, the divider is clocked
			if ( m_ui8DividerCounter == 0 ) {
				// When the divider is clocked while at 0, it is loaded with V
				m_ui8DividerCounter = m_ui8Volume;

				// and clocks the decay level counter. Then one of two actions occurs:
				if ( m_ui8DecayCounter == 0 ) {
					// otherwise if the loop flag is set, the decay level counter is loaded with 15.
					if ( _bLoop ) { m_ui8DecayCounter = 15; }

				}
				// If the counter is non-zero, it is decremented
				else { m_ui8DecayCounter--; }
			}
			else { m_ui8DividerCounter--; }
		}

		// The envelope unit's volume output depends on the constant volume flag: if set, the envelope parameter directly sets the volume, otherwise the decay level is the current volume. The constant volume flag has no effect besides selecting the volume source; the decay level will still be updated when constant volume is selected.
		//m_ui8Output = _bUseVolume ? m_ui8Volume : m_ui8DecayCounter;
		m_ui8Output = m_ui8DecayCounter;

		return m_ui8Output;
	}

	/**
	 * Restarts the envelope.
	 **/
	inline void CEnvelope::RestartEnvelope() { m_bRestart = true; }

	/**
	 * Sets the envelope volume.
	 * 
	 * \param _ui8Vol The new envelope volume.
	 **/
	inline void CEnvelope::SetEnvelopeVolume( uint8_t _ui8Vol ) { m_ui8Volume = _ui8Vol; }

	/**
	 * Gets the envelope output value.
	 * 
	 * \param _bUseVolume Determines if envelopes are used or just a solid volume.
	 * \return Returns the envelope output value.
	 **/
	inline uint8_t CEnvelope::GetEnvelopeOutput( bool _bUseVolume ) const { return _bUseVolume ? m_ui8Volume : m_ui8Output; }

	/**
	 * Resets the unit to a known state.
	 **/
	inline void CEnvelope::ResetToKnown() {
		m_ui8Output					= 0;
		m_ui8DecayCounter			= 0;
		m_ui8DividerCounter			= 0;
		m_ui8Volume					= 0;
		m_bRestart					= true;
	}

}	// namespace lsn
