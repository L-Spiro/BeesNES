/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Manages the DPCM state, rate decoding, and shift register.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Cpu/LSNCpuBase.h"


namespace lsn {

	/**
	 * Class CDmc
	 * \brief Handles the Delta Modulation Channel (DMC) for the APU.
	 *
	 * Description: Manages the DPCM state, rate decoding, and shift register.
	 */
	class CDmc /*: public CApuUnit*/ {
	public :
		CDmc();
		virtual ~CDmc();


		// == Functions.
		/**
		 * Resets the unit to a known state.
		 **/
		void									ResetToKnown();

		/**
		 * Ticks the DMC unit.
		 *
		 * \param _pCpu Pointer to the CPU base class to trigger subsequent DMA fetches.
		 */
		void									Tick( CCpuBase * _pCpu ) {
			if ( m_ui16Timer > 0 ) { m_ui16Timer--; }
			else {
				m_ui16Timer = m_ui16TimerPeriod;

				if ( !m_bSilent ) {
					if ( m_ui8ShiftRegister & 1 ) {
						if ( m_ui8OutputLevel <= 125 ) { m_ui8OutputLevel += 2; }
					}
					else {
						if ( m_ui8OutputLevel >= 2 ) { m_ui8OutputLevel -= 2; }
					}
				}

				m_ui8ShiftRegister >>= 1;
				m_ui8BitsRemaining--;
				if ( m_ui8BitsRemaining == 0 ) {
					m_ui8BitsRemaining = 8;
                
					if ( m_bBufferEmpty ) {
						m_bSilent = true;
					}
					else {
						m_bSilent = false;
						m_ui8ShiftRegister = m_ui8SampleBuffer;
						m_bBufferEmpty = true;
                    
						if ( m_ui16BytesRemaining > 0 ) {
							_pCpu->BeginDmcDma( true );
						}
					}
				}
			}
		}

		/**
		 * Called by the APU when the CPU completes the DMA fetch.
		 *
		 * \param _ui8Val The fetched sample byte from memory.
		 * \param _pCpu Pointer to the CPU to trigger an IRQ if the sample ends.
		 */
		inline void								ReceiveSample( uint8_t _ui8Val, CCpuBase * _pCpu ) {
			m_ui8SampleBuffer = _ui8Val;
			m_bBufferEmpty = false;

			m_ui16CurrentAddress = (m_ui16CurrentAddress + 1) | 0x8000;

			m_ui16BytesRemaining--;
			if ( m_ui16BytesRemaining == 0 ) {
				if ( m_bLoop ) {
					RestartSample();
				} 
				else if ( m_bIrqEnabled ) {
					_pCpu->Irq( LSN_IS_APU_DMC );
				}
			}
		}

		/**
		 * Handles writes to the $4010 register (IRQ Enable, Loop, Rate).
		 *
		 * \param _ui8Val The 8-bit value written to the register.
		 * \param _pCpu Pointer to the CPU to clear the IRQ if disabled.
		 */
		template <unsigned _uConsoleType>
		void									Write4010( uint8_t _ui8Val, CCpuBase * _pCpu ) {
			m_bIrqEnabled = (_ui8Val & 0b10000000) != 0;
			m_bLoop       = (_ui8Val & 0b01000000) != 0;
			m_ui16TimerPeriod = GetRatePeriod<_uConsoleType>( _ui8Val & 0x0F );

			if ( !m_bIrqEnabled && _pCpu ) {
				_pCpu->ClearIrq( LSN_IS_APU_DMC );
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
		 * Enables or disables the DMC channel. If enabled while bytes remaining is 0, 
		 * it restarts the sample.
		 *
		 * \param _bEnabled True to enable the DMC, false to disable.
		 * \param _pCpu Pointer to the CPU base class to trigger DMA if a fetch is immediately required.
		 */
		void									SetEnabled( bool _bEnabled, CCpuBase * _pCpu ) {
			if ( !_bEnabled ) {
				ClearBytesRemaining();
			}
			else {
				if ( m_ui16BytesRemaining == 0 ) {
					RestartSample();
					if ( m_bBufferEmpty ) {
						_pCpu->BeginDmcDma( false );
					}
				}
			}
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

		/**
		 * Clears the number of bytes remaining in the current sample transfer.
		 */
		inline void								ClearBytesRemaining() { m_ui16BytesRemaining = 0; }

		/**
		 * Checks if the DMC has asserted an IRQ directly from the CPU's state.
		 *
		 * \param _pCpu Pointer to the CPU tracking the IRQ.
		 * \return Returns true if an IRQ is currently asserted by the DMC.
		 */
		inline bool								IsIrqAsserted( CCpuBase * _pCpu ) const { 
			return _pCpu->GetIrqStatus( LSN_IS_APU_DMC ); 
		}

		/**
		 * Clears the DMC IRQ flag directly on the CPU.
		 * * \param _pCpu Pointer to the CPU tracking the IRQ.
		 */
		inline void								ClearIrq( CCpuBase * _pCpu ) { 
			_pCpu->ClearIrq( LSN_IS_APU_DMC ); 
		}

		/**
		 * Restarts the current sample by reloading the starting address and length.
		 */
		inline void								RestartSample() {
			m_ui16CurrentAddress = m_ui16SampleAddress;
			m_ui16BytesRemaining = m_ui16SampleLength;
		}


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
