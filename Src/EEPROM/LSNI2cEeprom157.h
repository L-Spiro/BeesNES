/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The EEPROM used by mapper 157.
 */
 
 #pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>


namespace lsn {

	/**
	 * \brief I²C 24C01 EEPROM for NES Mapper 157 (128 bytes), sub-cycle safe.
	 *
	 * Bit-banged via $6000 (SDA) and $6001 (SCL). Call Tick() once per CPU cycle.
	 */
	class CI2cEeprom157 {
	public :
		/**
		 * \brief Construct a 128-byte 24C01 EEPROM with default line levels and state.
		 */
		CI2cEeprom157() :
			m_vStorage( 128, 0 ),
			m_bSdaOut( true ),
			m_bScl( true ),
			m_bDevDriveLow( false ),
			m_bLastBusSda( true ),
			m_bLastBusScl( true ),
			m_bStarted( false ),
			m_bResponding( false ),
			m_ui8Shift( 0 ),
			m_ui8BitPos( 0 ),
			m_ePhase( LSN_P_DEV_ADDR ),
			m_eAck( LSN_A_NONE ),
			m_ui8AddrPtr( 0 ),
			m_ui8DevAddr7( 0x50 ),
			m_ui8MaskAddr( 0x7F )
		{
		}


		// == Functions
		/**
		 * \brief Advance one CPU cycle with the current host line levels.
		 *
		 * \param _bSdaOut Host SDA level (bit 0 of writes to $6000). True = released/high.
		 * \param _bScl    Host SCL level (bit 0 of writes to $6001). True = high.
		 * \return         BUS SDA level for reads from $6000 (bit 0). 1 = high, 0 = low.
		 */
		uint8_t                         Tick( bool _bSdaOut, bool _bScl ) {
			m_bSdaOut = _bSdaOut;
			m_bScl    = _bScl;

			const bool bBusSda = ComputeBusSda();
			const bool bBusScl = m_bScl;

			// START: SDA falls while SCL high.
			if ( m_bLastBusScl && m_bLastBusSda && !bBusSda ) {
				OnStart();
			}

			// STOP: SDA rises while SCL high.
			if ( m_bLastBusScl && !m_bLastBusSda && bBusSda ) {
				OnStop();
			}

			// Falling edge of SCL: prepare device-driven bit (data or ACK).
			if ( m_bLastBusScl && !bBusScl ) {
				OnSclFalling();
			}

			// Rising edge of SCL: sample data or master ACK/NACK.
			if ( !m_bLastBusScl && bBusScl ) {
				OnSclRising( bBusSda );
			}

			m_bLastBusSda = ComputeBusSda(); // Recompute in case device drive changed.
			m_bLastBusScl = bBusScl;

			return static_cast<uint8_t>(m_bLastBusSda ? 1U : 0U);
		}

		/**
		 * \brief Get a const view of EEPROM for save files.
		 *
		 * \return Reference to internal storage.
		 */
		const std::vector<uint8_t> &    GetData() const {
			return m_vStorage;
		}

		/**
		 * \brief Load EEPROM contents from a buffer.
		 *
		 * Excess input bytes are ignored.
		 *
		 * \param _vData Source buffer.
		 */
		void                            SetData( const std::vector<uint8_t> &_vData ) {
			std::fill( m_vStorage.begin(), m_vStorage.end(), 0 );
			for ( size_t I = 0; I < _vData.size() && I < m_vStorage.size(); ++I ) {
				m_vStorage[I] = _vData[I];
			}
		}


	protected :
		// == Types.
		/**
		 * \brief Protocol phases for 24C01 transactions.
		 *
		 * LSN_P_DEV_ADDR  : Device address byte (7-bit address + R/W bit).
		 * LSN_P_WORD_ADDR : Word-address byte (internal address pointer).
		 * LSN_P_WRITE_DATA: Payload byte(s) written by the master.
		 * LSN_P_READ_DATA : Payload byte(s) read by the master (current-address read or sequential).
		 */
		enum LSN_PHASE {
			LSN_P_DEV_ADDR,
			LSN_P_WORD_ADDR,
			LSN_P_WRITE_DATA,
			LSN_P_READ_DATA
		};

		/**
		 * \brief ACK ownership state.
		 *
		 * LSN_A_NONE   : No ACK/NACK phase active.
		 * LSN_A_DEV    : Device drives the ACK bit (after receiving a byte).
		 * LSN_A_MASTER : Master drives the ACK/NACK bit (after reading a byte).
		 */
		enum LSN_ACK {
			LSN_A_NONE,
			LSN_A_DEV,
			LSN_A_MASTER
		};


		// == Members.
		std::vector<uint8_t>            m_vStorage;        /**< 128-byte EEPROM array. */

		bool                            m_bSdaOut;         /**< CPU-driven SDA ($6000). */
		bool                            m_bScl;            /**< CPU-driven SCL ($6001). */

		bool                            m_bDevDriveLow;    /**< Device open-drain: true = drive low. */

		bool                            m_bLastBusSda;     /**< Last BUS SDA. */
		bool                            m_bLastBusScl;     /**< Last BUS SCL. */

		bool                            m_bStarted;        /**< Inside a START..STOP frame. */
		bool                            m_bResponding;     /**< Device address matched. */
		uint8_t                         m_ui8Shift;        /**< Shift register for RX. */
		uint8_t                         m_ui8BitPos;       /**< Bit index 0..7. */
		LSN_PHASE                       m_ePhase;          /**< Current protocol phase. */
		LSN_ACK                         m_eAck;            /**< Current ACK ownership. */

		uint8_t                         m_ui8AddrPtr;      /**< Internal address pointer (wraps). */

		uint8_t                         m_ui8DevAddr7;     /**< 0x50 for 24C01 (A2..A0=0). */
		uint8_t                         m_ui8MaskAddr;     /**< Size mask (0x7F for 128B). */

		
		// == Functions
		/**
		 * \brief Compute BUS SDA from open-drain master and device.
		 *
		 * \return True if the bus is high, false if low.
		 */
		bool                            ComputeBusSda() const {
			const bool bDevReleased = !m_bDevDriveLow;
			return m_bSdaOut && bDevReleased;
		}

		/**
		 * \brief Handle I²C START condition.
		 */
		void                            OnStart() {
			m_bStarted     = true;
			m_bResponding  = false;
			m_ePhase       = LSN_P_DEV_ADDR;
			m_eAck         = LSN_A_NONE;
			m_ui8Shift     = 0;
			m_ui8BitPos    = 0;
			m_bDevDriveLow = false; // Release.
		}

		/**
		 * \brief Handle I²C STOP condition.
		 */
		void                            OnStop() {
			m_bStarted     = false;
			m_bResponding  = false;
			m_ePhase       = LSN_P_DEV_ADDR;
			m_eAck         = LSN_A_NONE;
			m_ui8Shift     = 0;
			m_ui8BitPos    = 0;
			m_bDevDriveLow = false; // Release.
		}

		/**
		 * \brief Prepare device output on SCL falling edge.
		 */
		void                            OnSclFalling() {
			if ( !m_bStarted ) {
				m_bDevDriveLow = false;
				return;
			}

			if ( m_eAck == LSN_A_DEV ) {
				// Device ACK: pull SDA low during the 9th clock high if responding.
				m_bDevDriveLow = m_bResponding;
				return;
			}

			if ( m_ePhase == LSN_P_READ_DATA && m_eAck == LSN_A_NONE ) {
				const uint8_t ui8Byte = m_vStorage[m_ui8AddrPtr];
				const uint8_t ui8Bit  = static_cast<uint8_t>((ui8Byte >> (7 - m_ui8BitPos)) & 1U);
				m_bDevDriveLow = (ui8Bit == 0U);
			}
			else {
				m_bDevDriveLow = false; // Release in other phases.
			}
		}

		/**
		 * \brief Sample inputs and advance state on SCL rising edge.
		 *
		 * \param _bBusSda Current BUS SDA level.
		 */
		void                            OnSclRising( bool _bBusSda ) {
			if ( !m_bStarted ) { return; }

			if ( m_eAck == LSN_A_DEV ) {
				// End of device ACK bit.
				m_eAck      = LSN_A_NONE;
				m_ui8BitPos = 0;
				return;
			}

			if ( m_eAck == LSN_A_MASTER ) {
				const bool bAck = (_bBusSda == false);
				if ( bAck ) {
					m_ui8AddrPtr = static_cast<uint8_t>((m_ui8AddrPtr + 1) & m_ui8MaskAddr);
					m_ePhase = LSN_P_READ_DATA;
				}
				else {
					m_ePhase = LSN_P_DEV_ADDR; // NACK ends the read.
				}
				m_eAck         = LSN_A_NONE;
				m_bDevDriveLow = false;
				m_ui8BitPos    = 0;
				return;
			}

			// Receive path (address/word/data): sample BUS SDA.
			if ( m_ePhase == LSN_P_DEV_ADDR || m_ePhase == LSN_P_WORD_ADDR || m_ePhase == LSN_P_WRITE_DATA ) {
				m_ui8Shift = static_cast<uint8_t>((m_ui8Shift << 1) | (_bBusSda ? 1U : 0U));
				++m_ui8BitPos;

				if ( m_ui8BitPos == 8U ) {
					switch ( m_ePhase ) {
						case LSN_P_DEV_ADDR : {
							const uint8_t ui8Dev = static_cast<uint8_t>(m_ui8Shift >> 1);
							const bool    bRw    = ((m_ui8Shift & 1U) != 0U);
							m_bResponding = (ui8Dev == m_ui8DevAddr7);
							if ( m_bResponding ) {
								m_ePhase = bRw ? LSN_P_READ_DATA : LSN_P_WORD_ADDR;
								m_eAck   = LSN_A_DEV; // Device ACK.
							}
							else {
								// Not our address: leave line released (NACK by omission).
								m_ePhase = LSN_P_DEV_ADDR;
								m_eAck   = LSN_A_NONE;
							}
							break;
						}
						case LSN_P_WORD_ADDR : {
							m_ui8AddrPtr = static_cast<uint8_t>(m_ui8Shift & m_ui8MaskAddr);
							m_ePhase     = LSN_P_WRITE_DATA;
							m_eAck       = LSN_A_DEV;
							break;
						}
						case LSN_P_WRITE_DATA : {
							m_vStorage[m_ui8AddrPtr] = m_ui8Shift;
							m_ui8AddrPtr = static_cast<uint8_t>((m_ui8AddrPtr + 1) & m_ui8MaskAddr);
							m_eAck       = LSN_A_DEV;
							break;
						}
						default : {
							break;
						}
					}
					m_ui8BitPos = 0;
					m_ui8Shift  = 0;
				}
			}
			else if ( m_ePhase == LSN_P_READ_DATA ) {
				// We just drove a data bit; count bits and then yield to master ACK/NACK.
				++m_ui8BitPos;
				if ( m_ui8BitPos == 8U ) {
					m_ui8BitPos    = 0;
					m_bDevDriveLow = false;   // Release for master ACK/NACK.
					m_eAck         = LSN_A_MASTER;
				}
			}
		}
	};

}	// namespace lsn
