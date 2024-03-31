/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Helps monitor the performance of a section of code.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Time/LSNClock.h"
#include <string>


namespace lsn {

	/**
	 * Class CPerformance
	 * \brief Helps monitor the performance of a section of code.
	 *
	 * Description: Helps monitor the performance of a section of code.
	 */
	class CPerformance {
	public :
		CPerformance( const char * _pcName ) :
			m_sName( _pcName ),
			m_ui64AccumTime( 0 ),
			m_ui64TimeNow( 0 ),
			m_ui32Calls( 0 ) {
		}
		~CPerformance() {
			if ( m_ui32Calls ) {
				char szBuffer[128];
				std::sprintf( szBuffer, ": [CRT Time: %.17f]\r\n", m_ui64AccumTime / double( m_ui32Calls ) / m_cPerfClock.GetResolution() * 1000.0 );
#ifdef LSN_WINDOWS
				::OutputDebugStringA( m_sName.c_str() );
				::OutputDebugStringA( szBuffer );
#else
				::fprintf( stderr, "%s\r\n%s", m_sName.c_str(), szBuffer );
#endif	// #ifdef LSN_WINDOWS
			}
		}


		// == Functions.
		/**
		 * Begins monitoring a section of code.
		 */
		LSN_FORCEINLINE void								Begin() {
			m_ui64TimeNow = m_cPerfClock.GetRealTick();
		}

		/**
		 * Stops monitoring a section of code.
		 */
		LSN_FORCEINLINE void								Stop() {
			m_ui64AccumTime += m_cPerfClock.GetRealTick() - m_ui64TimeNow;
			m_ui32Calls++;
		}

	protected :
		// == Members.
		/** The accumulated time. */
		uint64_t											m_ui64AccumTime;
		/** The time at which Begin() was called. */
		uint64_t											m_ui64TimeNow;
		/** The name of the performance monitor. */
		std::string											m_sName;
		/** The clock. */
		CClock												m_cPerfClock;
		/** The number of calls made. */
		uint32_t											m_ui32Calls;
	};

}	// namespace lsn
