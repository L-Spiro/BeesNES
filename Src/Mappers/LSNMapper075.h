/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 075 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper075
	 * \brief Mapper 075 implementation.
	 *
	 * Description: Mapper 075 implementation.
	 */
	class CMapper075 : public CMapperBase {
	public :
		CMapper075() {
		}
		virtual ~CMapper075() {
		}


		// == Enumerations.
		/** The mask. */
		enum LSN_MASK {
			LSN_M_MASK									= 0xFF,		// Or 0x0F.
		};


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _pbPpuBus );
			m_ui8PgmBank = 0;
			m_ui8PgmBank1 = 1;
			m_ui8PgmBank2 = 2;
			m_ui8ChrBank = 0;
			m_ui8ChrBank1 = 1;

		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

			// PGM bank 0.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_2000, this, uint16_t( I - 0x8000 ) );
			}
			// PGM bank 1.
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper075::PgmBank1Read_2000, this, uint16_t( I - 0xA000 ) );
			}
			// PGM bank 2.
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper075::PgmBank2Read_2000, this, uint16_t( I - 0xC000 ) );
			}

			// Set the reads of the fixed bank at the end.
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % m_prRom->vPrgRom.size() ) );
			}

			// CHR bank 0.
			for ( uint32_t I = 0x0000; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead_1000, this, uint16_t( I - 0x0000 ) );
			}
			// CHR bank 1.
			for ( uint32_t I = 0x1000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper075::ChrBank1Read_1000, this, uint16_t( I - 0x1000 ) );
			}

			// Select banks.
			for ( uint32_t I = 0x8000; I < 0x9000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper075::SelectBank8000_8FFF, this, 0 );	// Treated as ROM.
			}
			for ( uint32_t I = 0x9000; I < 0xA000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper075::SelectBank9000_9FFF, this, 0 );	// Treated as ROM.
			}
			for ( uint32_t I = 0xA000; I < 0xB000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper075::SelectBankA000_AFFF, this, 0 );	// Treated as ROM.
			}
			for ( uint32_t I = 0xC000; I < 0xD000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper075::SelectBankC000_CFFF, this, 0 );	// Treated as ROM.
			}
			for ( uint32_t I = 0xE000; I < 0xF000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper075::SelectBankE000_EFFF, this, 0 );	// Treated as ROM.
			}
			for ( uint32_t I = 0xF000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper075::SelectBankF000_FFFF, this, 0 );	// Treated as ROM.
			}


			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		/** PGM bank 2. */
		uint8_t											m_ui8PgmBank1;

		/** PGM bank 3. */
		uint8_t											m_ui8PgmBank2;

		/** CHR bank 2. */
		uint8_t											m_ui8ChrBank1;


		// == Functions.
		/**
		 * Reads from PGM ROM using m_ui8PgmBank1 to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBank1Read_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[_ui16Parm1+(pmThis->m_ui8PgmBank1*0x2000)];
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBank2 to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBank2Read_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[_ui16Parm1+(pmThis->m_ui8PgmBank2*0x2000)];
		}

		/**
		 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current 4-kilobyte bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrBank1Read_1000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBank1*0x1000+_ui16Parm1];
		}

		/**
		 * Select 8 KB PRG ROM at $8000.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_8FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			pmThis->m_ui8PgmBank = (_ui8Val & LSN_M_MASK) % (pmThis->m_prRom->vPrgRom.size() / 0x2000);
		}

		/**
		 * Select 8 KB PRG ROM at $A000.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankA000_AFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			pmThis->m_ui8PgmBank1 = (_ui8Val & LSN_M_MASK) % (pmThis->m_prRom->vPrgRom.size() / 0x2000);
		}

		/**
		 * Select 8 KB PRG ROM at $C000.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankC000_CFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			pmThis->m_ui8PgmBank2 = (_ui8Val & LSN_M_MASK) % (pmThis->m_prRom->vPrgRom.size() / 0x2000);
		}

		/**
		 * Mirroring  (0: Vertical; 1: Horizontal), High Bit of 4 KB CHR bank at PPU $0000, High Bit of 4 KB CHR bank at PPU $1000.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank9000_9FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---------
			 *	.... .BAM
			 *		  |||
			 *		  ||+- Mirroring  (0: Vertical; 1: Horizontal)
			 *		  |+-- High Bit of 4 KB CHR bank at PPU $0000
			 *		  +--- High Bit of 4 KB CHR bank at PPU $1000
			 */
			switch ( _ui8Val & 0b001 ) {
				case 0 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
				case 1 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
			}
			pmThis->m_ui8ChrBank = (pmThis->m_ui8ChrBank & 0b01111) | ((_ui8Val & 0b00010) << 3);
			pmThis->m_ui8ChrBank1 = (pmThis->m_ui8ChrBank1 & 0b01111) | ((_ui8Val & 0b00100) << 2);

			pmThis->m_ui8ChrBank = (pmThis->m_ui8ChrBank) % (pmThis->m_prRom->vChrRom.size() / 0x1000);
			pmThis->m_ui8ChrBank1 = (pmThis->m_ui8ChrBank1) % (pmThis->m_prRom->vChrRom.size() / 0x1000);
		}

		/**
		 * Low 4 bits of 4 KB CHR bank at PPU $0000.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankE000_EFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			pmThis->m_ui8ChrBank = (pmThis->m_ui8ChrBank & 0b10000) | (_ui8Val & 0b01111);
			pmThis->m_ui8ChrBank = (pmThis->m_ui8ChrBank) % (pmThis->m_prRom->vChrRom.size() / 0x1000);
		}

		/**
		 * Low 4 bits of 4 KB CHR bank at PPU $1000.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankF000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper075 * pmThis = reinterpret_cast<CMapper075 *>(_pvParm0);
			pmThis->m_ui8ChrBank1 = (pmThis->m_ui8ChrBank1 & 0b10000) | (_ui8Val & 0b01111);
			pmThis->m_ui8ChrBank1 = (pmThis->m_ui8ChrBank1) % (pmThis->m_prRom->vChrRom.size() / 0x1000);
		}
	};

}	// namespace lsn
