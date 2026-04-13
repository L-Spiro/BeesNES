/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Manages the DPCM state, rate decoding, and shift register.
 */


#pragma once

#include "../LSNLSpiroNes.h"
//#include "LSNApuUnit.h"


namespace lsn {

	/**
	 * Class CNoise
	 * \brief Handles the Delta Modulation Channel (DMC) for the APU.
	 *
	 * Description: Manages the DPCM state, rate decoding, and shift register.
	 */
	class CDmc /*: public CApuUnit*/ {
		CDmc();
		virtual ~CDmc();


		// == Functions.
		/**
		 * Resets the unit to a known state.
		 **/
		void									ResetToKnown();

		/**
		 * Handles writes to the $4010 register (IRQ Enable, Loop, Rate).
		 *
		 * \param _ui8Val The 8-bit value written to the register.
		 */
		template <unsigned _uConsoleType>
		void									Write4010( uint8_t _ui8Val ) {
			m_bIrqEnabled = (_ui8Val & 0b10000000) != 0;
			m_bLoop       = (_ui8Val & 0b01000000) != 0;
			m_ui16TimerPeriod = GetRatePeriod<_uConsoleType>( _ui8Val & 0x0F );

			if ( !m_bIrqEnabled ) {
				m_bIrqAsserted = false;
			}
		}

		/**
		 * Handles writes to the $4011 register (Direct Load / Output Level).
		 *
		 * \param _ui8Val The 8-bit value written to the register.
		 */
		void									Write4011( uint8_t _ui8Val ) {
			m_ui8OutputLevel = _ui8Val & 0b01111111;
		}

		/**
		 * Handles writes to the $4012 register (Sample Address).
		 *
		 * \param _ui8Val The 8-bit value written to the register.
		 */
		void									Write4012( uint8_t _ui8Val ) {
			m_ui16SampleAddress = 0x4000 + (uint16_t( _ui8Val ) * 64);
		}

		/**
		 * Handles writes to the $4013 register (Sample Length).
		 *
		 * \param _ui8Val The 8-bit value written to the register.
		 */
		void									Write4013( uint8_t _ui8Val ) {
			m_ui16SampleLength = (uint16_t( _ui8Val ) * 16) + 1;
		}

		/**
		 * Gets the number of bytes remaining in the current sample transfer.
		 *
		 * \return Returns the number of bytes left to fetch.
		 */
		inline uint16_t							GetBytesRemaining() const { return m_ui16BytesRemaining; }

		/**
		 * Gets the current memory address the DMA should read from.
		 *
		 * \return Returns the 16-bit address for the next sample byte.
		 */
		inline uint16_t							GetCurrentAddress() const { return m_ui16CurrentAddress; }

		/**
		 * Gets the current 7-bit output level of the delta decoder.
		 *
		 * \return Returns the current DPCM output level.
		 */
		inline uint8_t							GetOutputLevel() const { return m_ui8OutputLevel; }


	protected :
		// == Members.
		uint16_t								m_ui16SampleAddress = 0xC000;								/**< The sample address. */
        uint16_t								m_ui16SampleLength = 1;										/**< The sample length. */
        uint16_t								m_ui16CurrentAddress = 0;									/**< The current address. */
        uint16_t								m_ui16BytesRemaining = 0;									/**< The number of bytes remaining. */
        
        uint8_t									m_ui8OutputLevel = 0;										/**< The current DMC output level. */
        uint8_t									m_ui8SampleBuffer = 0;										/**< The current sample buffer to decode. */
        uint8_t									m_ui8ShiftRegister = 0;										/**< The shift register. */
        uint8_t									m_ui8BitsRemaining = 8;										/**< The number of bits left to decode. */

        uint16_t								m_ui16Timer = 0;											/**< The rate timer. */
        uint16_t								m_ui16TimerPeriod = 0;										/**< The timer period. */

        bool									m_bIrqEnabled = false;										/**< Tracks the IRQ state. */
        bool									m_bLoop = false;											/**< Tracks the loop state. */
        bool									m_bIrqAsserted = false;										/**< Tracks the state of the IRQ assertion. */
        bool									m_bBufferEmpty = true;										/**< Determines if the buffer is empty or not. */
        bool									m_bSilent = true;											/**< If true, the no samples are being processed. */

		static const uint16_t					m_ui16Periods[LSN_PM_CONSOLE_TOTAL][16];					/**< Rate periods for each console type. */


		// == Functions.
		/**
		 * Gets the hardware timer period for a given rate index based on the cached region.
		 *
		 * \param _ui8Index The 4-bit rate index (0-15) written to $4010.
		 * \return Returns the timer countdown period (N-1) for the specified rate.
		 */
		template <unsigned _uConsoleType>
		inline uint16_t							GetRatePeriod( uint8_t _ui8Index ) const {
			return m_ui16Periods[_uConsoleType][_ui8Index] - 1;
		}
	};

}	// namespace lsn
