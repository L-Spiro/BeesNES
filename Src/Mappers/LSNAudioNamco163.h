/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Namco 163 expansion audio.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <algorithm>
#include <cstring>


namespace lsn {

	/**
	 * Class CAudioNamco163
	 * \brief Namco 163 expansion audio.
	 *
	 * Description: Namco 163 expansion audio implementation.
	 */
	class CAudioNamco163 {
	public :
		// == Constructors.
		CAudioNamco163() :
			m_pui8Ram( nullptr ) {
			ResetFull( false );
		}
		virtual ~CAudioNamco163() {
		}


		// == Functions.
		/**
		 * Sets the pointer to the 128-byte internal RAM.
		 *
		 * \param _pui8Ram A pointer to the 128-byte RAM array managed by the mapper.
		 */
		inline void										SetRam( uint8_t * _pui8Ram ) {
			m_pui8Ram = _pui8Ram;
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			m_ui8CycleCount++;
			// Namco 163 updates one audio channel every 15 CPU cycles.
			if ( m_ui8CycleCount >= 15 ) {
				m_ui8CycleCount = 0;
				TickAudioInternal();
			}
		}

		/**
		 * Does a full power-on reset.
		 * 
		 * \param _bToKnown If true, forces RAM and registers to a known zero state.
		 **/
		void											ResetFull( bool _bToKnown = false ) {
			if ( _bToKnown ) {
				if ( m_pui8Ram ) {
					std::memset( m_pui8Ram, 0, RamSize() );
				}
				m_ui8Addr = 0;
				m_bAutoInc = false;
				m_ui8CycleCount = 0;
				m_ui8CurrentChannel = 0;
				m_fSample = 0.0f;
			}
		}

		/**
		 * Does a soft reset.
		 **/
		void											ResetSoft() {
		}

		/**
		 * Gets the extended-audio sample.
		 * 
		 * \return Returns the current sample.
		 **/
		inline float									Sample() {
			return m_fSample;
		}

		/**
		 * Post-process an output sample.  Applies the volume crunch.
		 * 
		 * \param _fSample The sample to modify.
		 * \param _fHz The current sampling rate.
		 * \return Returns the volume-crunched sample.
		 **/
		inline float									PostProcessSample( float _fSample, float /*_fHz*/ ) {
			return _fSample;
		}

		/**
		 * Writes to the Namco 163 internal address port ($F800-$FFFF).
		 *
		 * \param _ui8Val The value to write.
		 */
		inline void										WriteF800( uint8_t _ui8Val ) {
			m_ui8Addr = _ui8Val & 0x7F;
			m_bAutoInc = (_ui8Val & 0x80) != 0;
		}

		/**
		 * Reads from the Namco 163 data port ($4800-$4FFF).
		 *
		 * \param _ui8Ret The read value.
		 */
		inline void										Read4800( uint8_t &_ui8Ret ) {
			if ( m_pui8Ram ) {
				_ui8Ret = m_pui8Ram[m_ui8Addr];
			}
			if ( m_bAutoInc ) {
				m_ui8Addr = (m_ui8Addr + 1) & 0x7F;
			}
		}

		/**
		 * Writes to the Namco 163 data port ($4800-$4FFF).
		 *
		 * \param _ui8Val The value to write.
		 */
		inline void										Write4800( uint8_t _ui8Val ) {
			if ( m_pui8Ram ) {
				m_pui8Ram[m_ui8Addr] = _ui8Val;
			}
			if ( m_bAutoInc ) {
				m_ui8Addr = (m_ui8Addr + 1) & 0x7F;
			}
		}

		/**
		 * Gets a pointer to the internal RAM.
		 *
		 * \return Returns a pointer to the 128-byte internal RAM.
		 */
		inline uint8_t *								GetRam() {
			return m_pui8Ram;
		}

		/**
		 * Gets a constant pointer to the internal RAM.
		 *
		 * \return Returns a constant pointer to the 128-byte internal RAM.
		 */
		inline const uint8_t *							GetRam() const {
			return m_pui8Ram;
		}

		/**
		 * Gets the size of the internal RAM array.
		 *
		 * \return Returns the internal RAM size (128 bytes).
		 */
		static constexpr size_t							RamSize() { return 128; }


	protected :
		// == Members.
		/** A pointer to the 128-byte internal RAM managed by the mapper. */
		uint8_t *										m_pui8Ram;
		/** Current address into the internal RAM [0..127]. */
		uint8_t											m_ui8Addr = 0;
		/** Address auto-increment flag. */
		bool											m_bAutoInc = false;
		/** Tracks CPU cycles to generate the 15-cycle channel update clock. */
		uint8_t											m_ui8CycleCount = 0;
		/** The current channel being processed during multiplexing. */
		uint8_t											m_ui8CurrentChannel = 0;
		/** The last sample output. */
		float											m_fSample = 0.0f;


		// == Functions.
		/**
		 * Internal helper for Tick(). Processes a single channel and updates the multiplexed output.
		 */
		inline void										TickAudioInternal() {
			if ( !m_pui8Ram ) { return; }

			// Bits 4-6 of $7F indicate (NumChannels - 1).
			uint8_t ui8NumChannels = ((m_pui8Ram[0x7F] >> 4) & 0x07) + 1;

			// Channels are processed in descending order, ending with 0.
			if ( m_ui8CurrentChannel == 0 || m_ui8CurrentChannel > ui8NumChannels ) {
				m_ui8CurrentChannel = ui8NumChannels;
			}
			m_ui8CurrentChannel--;

			// Channel 0 is at $78, Channel 1 is at $70 ... Channel 7 is at $40.
			uint8_t ui8Base = 0x40 + ((7 - m_ui8CurrentChannel) * 8);

			// 18-bit frequency.
			uint32_t ui32Freq = uint32_t( m_pui8Ram[ui8Base+0] ) | (uint32_t( m_pui8Ram[ui8Base+2] ) << 8) | ((uint32_t( m_pui8Ram[ui8Base+4] ) & 0x03) << 16);
			
			// 24-bit phase.
			uint32_t ui32Phase = uint32_t( m_pui8Ram[ui8Base+1] ) | (uint32_t( m_pui8Ram[ui8Base+3] ) << 8) | (uint32_t( m_pui8Ram[ui8Base+5] ) << 16);
			
			// Length is bits 2-7 of +4.
			uint32_t ui32Length = 256 - (m_pui8Ram[ui8Base+4] & 0xFC);

			// Advance phase and wrap.
			ui32Phase = (ui32Phase + ui32Freq) % (ui32Length << 16);

			// Write phase.
			m_pui8Ram[ui8Base+1] = uint8_t( ui32Phase & 0xFF );
			m_pui8Ram[ui8Base+3] = uint8_t( (ui32Phase >> 8) & 0xFF );
			m_pui8Ram[ui8Base+5] = uint8_t( (ui32Phase >> 16) & 0xFF );


			uint32_t ui32Offset = (ui32Phase >> 16);
			uint32_t ui32Address = uint32_t( m_pui8Ram[ui8Base+6] ) + ui32Offset;


			ui32Address &= 0xFF;

			uint8_t ui8SampleByte = m_pui8Ram[ui32Address>>1];
			uint8_t ui8Nibble = (ui32Address & 1) ? (ui8SampleByte >> 4) : (ui8SampleByte & 0x0F);

			// Convert from unsigned 4-bit [0..15] to signed [-8..+7].
			int32_t i32Sample = int32_t( ui8Nibble ) - 8;
			uint8_t ui8Vol = m_pui8Ram[ui8Base+7] & 0x0F;

			// Normalize.
			m_fSample = float( i32Sample * int32_t( ui8Vol ) ) / 120.0f;
		}
	};

}	// namespace lsn
