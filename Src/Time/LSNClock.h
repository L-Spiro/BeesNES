/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A time-keeping class.  Used to keep the master clock running in real-time.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../OS/LSNOs.h"

#ifdef __GNUC__
#include <mach/mach_time.h>
#endif	// #ifdef __GNUC__

namespace lsn {

	/**
	 * Class CBus
	 * \brief A time-keeping class.  Used to keep the master clock running in real-time.
	 *
	 * Description: A time-keeping class.  Used to keep the master clock running in real-time.
	 */
	class CClock {
	public :
		// == Various constructors.
		CClock();


		// == Functions.
		/**
		 * Gets the clock resolution.
		 *
		 * \return Returns the resolution.
		 */
		inline uint64_t							GetResolution() const;

		/**
		 * Gets the clock start tick.
		 *
		 * \return Returns the start tick.
		 */
		inline uint64_t							GetStartTick() const;

		/**
		 * Gets the current clock tick.
		 *
		 * \return Returns the current clock tick.
		 */
		uint64_t								GetRealTick() const;

		/**
		 * Sets the starting clock tick to GetRealTick().
		 */
		void									SetStartingTick();

	protected :
		// == Members.
		uint64_t								m_ui64Resolution = 0;							/**< The resolution of the clock. */
		uint64_t								m_ui64StartTime = 0;							/**< The starting clock time. */
		
#ifdef __GNUC__
		static ::mach_timebase_info_data_t		m_mtidInfoData;									/**< Time resoution. */
#endif	// #ifdef __GNUC__
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the clock resolution.
	 *
	 * \return Returns the resolution.
	 */
	inline uint64_t CClock::GetResolution() const { return m_ui64Resolution; }

	/**
	 * Gets the clock start tick.
	 *
	 * \return Returns the start tick.
	 */
	inline uint64_t CClock::GetStartTick() const { return m_ui64StartTime; }

}	// namespace lsn
