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
			m_tVal( _tVal ),
			m_stDirty( 0 ) {
		}


		// == Functions.
		/**
		 * Returns the length of the internal array, which must be larger than 0.
		 *
		 * \return Returns max( 1, _tnType ).
		 */
		template <size_t _uDelayCycles>
		static constexpr size_t								ArraySize() { return _uDelayCycles > 0 ? _uDelayCycles : 1; }

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
		_tnType												WriteWithDelay( _tnType _tnValue ) {
			if constexpr ( _uDelayCycles == 0 ) {
				// No delay.  Bypass the delay functionality entirely.
				m_tVal = _tnValue;
			}
			else {
				// Shove the new vlue into the bottom of the stack.
				m_tBuffer[0] = _tnValue;
				m_stDirty = _uDelayCycles;
			}
			return _tnValue;
		}

		/**
		 * Advances values toward the top.
		 */
		void												Tick() {
			if constexpr ( _uDelayCycles != 0 ) {
				if ( m_stDirty ) {
					--m_stDirty;
					m_tVal = m_tBuffer[ArraySize<_uDelayCycles>()-1];
					if constexpr ( _uDelayCycles >= 2 ) {
						for ( size_t I = ArraySize<_uDelayCycles>() - 1; I--; ) {
							m_tBuffer[I+1] = m_tBuffer[I];
						}
					}
				}
			}
		}

		/**
		 * Sets the current value immediately without going through the delay.
		 *
		 * \param _tnValue The value to set immediately with no delay.
		 */
		void												SetValue( _tnType _tnValue ) {
			m_tVal = _tnValue;
			if constexpr ( _uDelayCycles != 0 ) {
				m_stDirty = 0;
				for ( size_t I = ArraySize<_uDelayCycles>(); I--; ) {
					m_tBuffer[I] = _tnValue;
				}
			}
		}

		/**
		 * Gets the current value, as affected by the delay.
		 *
		 * \return Returns the current value by constant reference.
		 */
		const _tnType &										Value() const {
			return m_tVal;
		}

		/**
		 * Gets the most recent value assigned.
		 *
		 * \return Returns the most recent value assigned by constant reference.
		 */
		const _tnType &										MostRecetValue() const {
			if constexpr ( _uDelayCycles == 0 ) {
				return m_tVal;
			}
			else {
				return m_tBuffer[0];
			}
		}

	protected :
		// == Members.
		/** The array of values through which changes must pass to reach the final value. */
		_tnType												m_tBuffer[ArraySize<_uDelayCycles>()];
		/** The value to change at the end of the period. */
		_tnType &											m_tVal;
		/** If non-zero, there is a value in the pipeline and the object should be updated each cycle until it reaches the top. */
		size_t												m_stDirty;
	};

}	// namespace lsn
