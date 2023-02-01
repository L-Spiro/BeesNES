/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Delays the setting of a value for X ticks.
 */


#pragma once

#include "../LSNLSpiroNes.h"


namespace lsn {

	/**
	 * Class CDelayedValue
	 * \brief Delays the setting of a value for X ticks.
	 *
	 * Description: Delays the setting of a value for X ticks.
	 */
	template <typename _tnType, size_t _uDelayCycles>
	class CDelayedValue {
	public :
		CDelayedValue( _tnType &_tVal ) :
			m_tVal( _tVal ) {
		}


		// == Functions.
		/**
		 * Returns the length of the internal array, which must be larger than 0.
		 *
		 * \return Returns max( 1, _tnType ).
		 */
		constexpr size_t									ArraySize() const { return _uDelayCycles > 0 ? _uDelayCycles : 1; }

		/**
		 * Gets the actual delay.
		 *
		 * \return Returns _uDelayCycles.
		 */
		constexpr size_t									Delay() const { return _uDelayCycles; }

		/**
		 * Sets a value to be delay-copied into the final value passed in the constructor.
		 *
		 * \param _tnValue The new value to write with a delay.
		 * \return Returns the input value.
		 */
		_tnType												WriteWithDelay() {
		}


	protected :
		// == Members.
		/** The array of values through which changes must pass to reach the final value. */
		_tnType												m_tBuffer[ArraySize()];
		/** The value to change at the end of the period. */
		_tnType &											m_tVal;
	};

}	// namespace lsn
