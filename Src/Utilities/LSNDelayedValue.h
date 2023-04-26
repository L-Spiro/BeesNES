/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Delays the setting of a value for X ticks.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <functional>


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
		// == Types.
		/** The delayed value type. */
		typedef _tnType										Type;

		/** The callback function type. */
		typedef std::function<void ( void *, Type, Type )>	Callback;


		CDelayedValue( Callback _cCallback = nullptr, void * _pvCallbackParm = nullptr ) :
			m_stDirty( 0 ),
			m_cCallback( _cCallback ),
			m_pvCallbackParm( _pvCallbackParm ) {
			std::memset( m_bIsWrite, 0, sizeof( m_bIsWrite ) );
		}


		// == Functions.
		/**
		 * Returns the length of the internal array, which must be larger than 0.
		 *
		 * \return Returns _uDelayCycles + 1.
		 */
		template <size_t _uDelayCycles>
		static constexpr size_t								ArraySize() { return _uDelayCycles + 1; }

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
		 * \param _stManualDelay Allows overriding the position within the delay chain where the value is inserted.
		 * \return Returns the input value.
		 */
		_tnType												WriteWithDelay( _tnType _tnValue, size_t _stManualDelay = _uDelayCycles ) {
			if constexpr ( _uDelayCycles == 0 ) {
				// No delay.  Bypass the delay functionality entirely.
				if ( m_cCallback ) {
					m_cCallback( m_pvCallbackParm, _tnValue, Value() );
				}
			}
			// Shove the new value into the bottom of the stack.
			m_tBuffer[_uDelayCycles-_stManualDelay] = _tnValue;
			m_bIsWrite[_uDelayCycles-_stManualDelay] = true;
			m_stDirty = _uDelayCycles;

			return _tnValue;
		}

		/**
		 * Advances values toward the top.
		 */
		void												Tick() {
			if constexpr ( _uDelayCycles != 0 ) {
				if ( m_stDirty ) {
					--m_stDirty;
					if ( m_cCallback && m_bIsWrite[ArraySize<_uDelayCycles>()-2] ) {
						m_cCallback( m_pvCallbackParm, m_tBuffer[ArraySize<_uDelayCycles>()-2], Value() );
					}
					if constexpr ( _uDelayCycles >= 1 ) {
						for ( size_t I = ArraySize<_uDelayCycles>() - 1; I--; ) {
							m_tBuffer[I+1] = m_tBuffer[I];
							m_bIsWrite[I+1] = m_bIsWrite[I];
						}
						m_bIsWrite[0] = false;
					}
				}
			}
		}

		/**
		 * Sets the current value immediately without going through the delay.
		 *
		 * \param _tnValue The value to set immediately with no delay.
		 * \param _bTriggerCallback If true, the callback is triggered unless it is nullptr.
		 */
		void												SetValue( _tnType _tnValue, bool _bTriggerCallback = true ) {
			if ( _bTriggerCallback && m_cCallback ) {
				m_cCallback( m_pvCallbackParm, _tnValue, Value() );
			}
			if constexpr ( _uDelayCycles != 0 ) {
				m_stDirty = 0;
				for ( size_t I = ArraySize<_uDelayCycles>(); I--; ) {
					m_tBuffer[I] = _tnValue;
					m_bIsWrite[I] = false;
				}
			}
			else {
				m_tBuffer[0] = _tnValue;
			}
		}

		/**
		 * Gets the current value, as affected by the delay.
		 *
		 * \return Returns the current value by constant reference.
		 */
		const _tnType &										Value() const {
			return m_tBuffer[ArraySize<_uDelayCycles>()-1];
		}

		/**
		 * Gets the most recent value assigned.
		 *
		 * \return Returns the most recent value assigned by constant reference.
		 */
		const _tnType &										MostRecentValue() const {
			return m_tBuffer[0];
		}

	protected :
		// == Members.
		/** A callback function called when the final value actually gets set. */
		Callback											m_cCallback;
		/** The first parameter to pass to the callback. */
		void *												m_pvCallbackParm;
		/** The array of values through which changes must pass to reach the final value. */
		_tnType												m_tBuffer[ArraySize<_uDelayCycles>()];
		/** A companion array that allows us to track values through the delay to know when to trigger the callback.  The callback must only be triggered on actual writes to the target value. */
		bool												m_bIsWrite[ArraySize<_uDelayCycles>()];
		/** If non-zero, there is a value in the pipeline and the object should be updated each cycle until it reaches the top. */
		size_t												m_stDirty;
	};

}	// namespace lsn
