/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A time-keeping class.  Used to keep the master clock running in real-time.
 */


#include "LSNClock.h"

namespace lsn {

	// == Members.
#ifdef __GNUC__
	::mach_timebase_info_data_t CClock::m_mtidInfoData = { 0 };
#endif	// #ifdef __GNUC__

	// == Various constructors.
	CClock::CClock() {
#ifdef LSN_WINDOWS
		LARGE_INTEGER liTmp;
		::QueryPerformanceFrequency( &liTmp );
		m_ui64Resolution = liTmp.QuadPart;
#elif defined( __GNUC__ )
		if ( !m_mtidInfoData.denom ) {
			if ( KERN_SUCCESS == ::mach_timebase_info( &m_mtidInfoData ) ) {
				m_ui64Resolution = m_mtidInfoData.denom * 1000000000ULL;
			}
		}
#endif	// #ifdef LSN_WINDOWS

		SetStartingTick();
	}

	// == Functions.
	/**
	 * Gets the current clock tick.
	 *
	 * \return Returns the current clock tick.
	 */
	uint64_t CClock::GetRealTick() const {
#ifdef LSN_WINDOWS
		LARGE_INTEGER liTmp;
		::QueryPerformanceCounter( &liTmp );
		return liTmp.QuadPart;
#elif defined( __GNUC__ )
		return ::mach_absolute_time() * m_mtidInfoData.numer;
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Sets the starting clock tick to GetRealTick().
	 */
	void CClock::SetStartingTick() {
		m_ui64StartTime = GetRealTick();
	}

}	// namespace lsn
