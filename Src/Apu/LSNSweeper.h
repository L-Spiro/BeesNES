/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU sweep unit.  Sweeps a pitch up or down.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CSweeper
	 * \brief An APU sweep unit.
	 *
	 * Description: An APU sweep unit.  Sweeps a pitch up or down.
	 */
	class CSweeper {
	public :
		inline CSweeper( uint16_t &_ui16Reload ) :
			m_ui16ReloadRef( _ui16Reload ) {
		}
		inline ~CSweeper() {
		}


		// == Functions.
		/**
		 * Updates the envelope.
		 * 
		 * \param _bOnesCompliment Must be 0 (for two's compliment) or 1 (for one's compliment).
		 **/
		template <unsigned _uOnesCompliment>
		inline void									TickSweeper() {
			if ( m_ui8Timer == 0 && m_bEnabled && m_ui8Shift > 0 && !m_bMuted ) {
				m_ui16ReloadRef = NextRefVal<_uOnesCompliment>( m_ui16ReloadRef, m_ui8Shift, m_bNegated );
			}

			if ( m_ui8Timer == 0 || m_bNeedReload ) {
				m_ui8Timer = m_ui8Period;
				m_bNeedReload = false;
			}
			else {
				m_ui8Timer--;
			}

			//m_bMuted = (m_ui16ReloadRef < 8) || (!m_bNegated && m_ui16ReloadRef > 0x7FF);
			/*auto aTmp = NextRefVal<_uOnesCompliment>( m_ui16ReloadRef, m_ui8Shift, m_bNegated );
			m_bMuted = (m_ui16ReloadRef < 8) || (!m_bNegated && aTmp > 0x7FF);*/
		}

		/**
		 * Gets the next reload reference value.
		 * 
		 * \param _ui16ReloadRef The current reload reference value.
		 * \param _ui8Shift The shift amount.
		 * \param _bNegated The negated flag.
		 * \return Returns the adjusted reload reference value.
		 **/
		template <unsigned _uOnesCompliment>
		static inline uint16_t						NextRefVal( uint16_t _ui16ReloadRef, uint8_t _ui8Shift, bool _bNegated ) {
			uint16_t ui16Change = _ui16ReloadRef >> _ui8Shift;
			if ( _ui16ReloadRef >= 8 && ui16Change < 0x07FF ) {
				if ( _bNegated ) {
					_ui16ReloadRef += -ui16Change - _uOnesCompliment;
				}
				else {
					_ui16ReloadRef += ui16Change;
				}
			}
			return _ui16ReloadRef;
		}

		/**
		 * Updates the internal state based on external observations about the target reload value.
		 **/
		template <unsigned _uOnesCompliment>
		inline void									UpdateSweeperState() {
			//if ( m_bEnabled ) {
				//m_ui16Change = m_ui16ReloadRef >> m_ui8Shift;
				//m_bMuted = (m_ui16ReloadRef < 8) || (!m_bNegated && m_ui16ReloadRef > 0x7FF);
				auto aTmp = NextRefVal<_uOnesCompliment>( m_ui16ReloadRef, m_ui8Shift, m_bNegated );
				m_bMuted = (m_ui16ReloadRef < 8) || (!m_bNegated && aTmp > 0x7FF);
			//}
		}

		/**
		 * Sets the "enabled" flag.
		 * 
		 * \param _bEnabled Whether the sweep unit is enabled or not.
		 **/
		inline void									SetSweepEnabled( bool _bEnabled ) {
			m_bEnabled = _bEnabled;
		}

		/**
		 * Sets the "negated" flag.
		 * 
		 * \param _bEnabled Whether the sweep unit is negated or not.
		 **/
		inline void									SetSweepNegate( bool _bEnabled ) {
			m_bNegated = _bEnabled;
		}

		/**
		 * Sets the shift value.
		 * 
		 * \param _ui8Shift The shift value.
		 **/
		inline void									SetSweepShift( uint8_t _ui8Shift ) {
			m_ui8Shift = _ui8Shift;
		}

		/**
		 * Sets the period value.
		 * 
		 * \param _ui8Period The period value.
		 **/
		inline void									SetSweepPeriod( uint8_t _ui8Period ) {
			m_ui8Period = _ui8Period;
		}

		/**
		 * Dirties the sweeper.  An update of state will occur on the next call to TickSweeper().
		 **/
		inline void									DirtySweeper() {
			m_bNeedReload = true;
		}

		/**
		 * Gets the muted status of the sweeper unit.
		 * 
		 * \return Returns true if the sweep unit is muted.
		 **/
		inline bool									SweeperMuted() const {
			return m_bMuted;
		}

		/**
		 * Resets the unit to a known state.
		 **/
		inline void									ResetToKnown() {
			//m_ui16Change	= 0;
			m_ui8Shift		= 0x00;
			m_ui8Timer		= 0x00;
			m_ui8Period		= 0x00;
			m_bEnabled		= false;
			m_bMuted		= false;
			m_bNegated		= false;
			m_bNeedReload	= true;
		}


	protected :
		// == Members.
		/** A reference to the reload flag on which to operate and to track. */
		uint16_t &									m_ui16ReloadRef;
		/** The change amount, determined by the shift and the current value. */
		//uint16_t									m_ui16Change	= 0;
		/** The shift value set by a register. */
		uint8_t										m_ui8Shift		= 0x00;
		/** The current time within our period. */
		uint8_t										m_ui8Timer		= 0x00;
		/** The sweeper update period.  Set by a register. */
		uint8_t										m_ui8Period		= 0x00;
		/** Are we enabled?  Set by register. */
		bool										m_bEnabled		= false;
		/** Are we muted? */
		bool										m_bMuted		= false;
		/** Are we negated?  Set by register. */
		bool										m_bNegated		= false;
		/** Should we reload the timer?  Set as a side-effect of writing to the register */
		bool										m_bNeedReload	= true;

	};

}	// namespace lsn
