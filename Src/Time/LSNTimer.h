/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A high-precision timer class using C++11.
 */

#pragma once

#include <chrono>


namespace lsn {

	/**
	 * Class CTimer
	 * \brief A high-precision timer class using C++11.
	 *
	 * Description: A high-precision timer class using C++11.
	 */
	class CTimer {
	public :
		CTimer() :
			m_tpStartPoint(),
			m_tpEndPoint(),
			m_bRunning( false ) {}


		// == Functions.
		/**
		 * Starts or resumes the timer.
		 **/
		void																Start() {
			if ( !m_bRunning ) {
				m_tpStartPoint = std::chrono::high_resolution_clock::now();
				m_bRunning = true;
			}
		}

		/**
		 * Stops or pauses the timer.
		 **/
		void																Stop() {
			if ( m_bRunning ) {
				m_tpEndPoint = std::chrono::high_resolution_clock::now();
				m_bRunning = false;
			}
		}

		/**
		 * Resets the timer.
		 **/
		void																Reset() {
			m_tpStartPoint = std::chrono::high_resolution_clock::now();
			m_tpEndPoint = m_tpStartPoint;
			m_bRunning = false;
		}

		/**
		 * Returns the elapsed time in seconds.
		 * 
		 * \return Returns the elapsed time in seconds.
		 **/
		inline double														ElapsedSeconds() const {
			return Elapsed<std::chrono::duration<double>>();
		}

		/**
		 * Returns the elapsed time in milliseconds.
		 * 
		 * \return Returns the elapsed time in milliseconds.
		 **/
		inline double														ElapsedMilliseconds() const {
			return Elapsed<std::chrono::milliseconds>();
		}

		/**
		 * Returns the elapsed time in microseconds.
		 * 
		 * \return Returns the elapsed time in microseconds.
		 **/
		inline double														ElapsedMicroseconds() const {
			return Elapsed<std::chrono::microseconds>();
		}

		/**
		 * Returns the elapsed time in nanoseconds.
		 * 
		 * \return Returns the elapsed time in nanoseconds.
		 **/
		inline double														ElapsedNanoseconds() const {
			return Elapsed<std::chrono::nanoseconds>();
		}

	private :
		// == Members.
		std::chrono::time_point<std::chrono::high_resolution_clock>			m_tpStartPoint;
		std::chrono::time_point<std::chrono::high_resolution_clock>			m_tpEndPoint;
		bool																m_bRunning;


		// == Functions.
		template <typename _tnDuration>
		double																Elapsed() const {
			auto end_point = m_bRunning ? std::chrono::high_resolution_clock::now() : m_tpEndPoint;
			return static_cast<double>(std::chrono::duration_cast<_tnDuration>( end_point - m_tpStartPoint ).count());
		}
	};

}	// namespace lsn
