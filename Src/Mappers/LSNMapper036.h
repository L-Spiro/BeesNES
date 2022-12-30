/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 036 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper036
	 * \brief Mapper 036 implementation.
	 *
	 * Description: Mapper 036 implementation.
	 */
	class CMapper036 : public CMapperBase {
	public :
		CMapper036() {
		}
		virtual ~CMapper036() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 32 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			SetPgmBank<0, PgmBankSize()>( 0 );
			SetChrBank<0, ChrBankSize()>( 0 );
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );
			
			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			if ( m_prRom->vChrRom.size() ) {
				// PPU.
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x4000; I < 0x6000; ++I ) {
				if ( (I & 0xE100) == 0x4100 ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper036::CopyRr4100, this, 0 );
				}
				if ( (I & 0xE103) == 0x4100 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper036::SelectBank4100, this, 0 );	// Treated as ROM.
				}
				if ( (I & 0xE103) == 0x4101 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper036::SelectBank4101, this, 0 );	// Treated as ROM.
				}
				if ( (I & 0xE103) == 0x4102 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper036::SelectBank4102, this, 0 );	// Treated as ROM.
				}
				if ( (I & 0xE103) == 0x4103 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper036::SelectBank4103, this, 0 );	// Treated as ROM.
				}
				if ( (I & 0x8000) == 0x8000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper036::SelectBank8000, this, 0 );	// Treated as ROM.
				}
			}
			// CHR bank-select.
			for ( uint32_t I = 0x4000; I < 0x6000; ++I ) {
				if ( (I & 0xE200) == 0x4200 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper036::SelectBank4200, this, 0 );	// Treated as ROM.
				}
			}
		}


	protected :
		// == Members.
		/** Internal RR register. */
		uint8_t											m_ui8Rr;
		/** Internal PP register. */
		uint8_t											m_ui8Pp;
		/** Increment Mode. */
		bool											m_bIncrMode;
		/** Invert Mode. */
		bool											m_bInvMode;


		// == Functions.
		/**
		 * Reads from the CHR RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						CopyRr4100( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper036 * pmThis = reinterpret_cast<CMapper036 *>(_pvParm0);
			/**
			 *	Mask: $E100
			 *	 read $4100: [xxRR xxxx]
			 *				  |||| ||||
			 *				  ||++------ Copy internal register 'RR' to data bus.
			 *				  ++---++++- open bus
			 */
			_ui8Ret = (_ui8Ret & ~0b11001111) | ((pmThis->m_ui8Rr << 4) & 0b11001111);
		}

		/**
		 * Update RR.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4100( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper036 * pmThis = reinterpret_cast<CMapper036 *>(_pvParm0);
			if ( pmThis->m_bIncrMode ) {
				pmThis->m_ui8Rr++;
			}
			else if ( pmThis->m_bInvMode ) {
				pmThis->m_ui8Rr = ~pmThis->m_ui8Pp;
			}
			else {
				pmThis->m_ui8Rr = pmThis->m_ui8Pp;
			}
		}

		/**
		 * Update Invert Mode.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4101( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper036 * pmThis = reinterpret_cast<CMapper036 *>(_pvParm0);
			/** write $4101: [...V ....] - Invert Mode */
			pmThis->m_bInvMode = (_ui8Val >> 4) & 1;
		}

		/**
		 * Copy PP.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4102( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper036 * pmThis = reinterpret_cast<CMapper036 *>(_pvParm0);
			/** write $4102: [..PP ....] - Copy data bus to internal register 'PP'. Value is not yet exposed anywhere. */
			pmThis->m_ui8Pp = (_ui8Val >> 4) & 0x11;
		}

		/**
		 * Update Increment Mode.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4103( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper036 * pmThis = reinterpret_cast<CMapper036 *>(_pvParm0);
			/** write $4103: [...C ....] - Increment Mode */
			pmThis->m_bIncrMode = (_ui8Val >> 4) & 1;
		}

		/**
		 * Copoy RR to PRG bank pins.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper036 * pmThis = reinterpret_cast<CMapper036 *>(_pvParm0);
			/** write $8000: copy internal register 'RR' to PRG banking pins */
			pmThis->SetPgmBank<0, PgmBankSize()>( pmThis->m_ui8Rr );
		}

		/**
		 * Selects a CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4200( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper036 * pmThis = reinterpret_cast<CMapper036 *>(_pvParm0);
			/** write $4200: [.... CCCC] - Select 8 KiB CHR bank */
			pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val & 0b1111 );
		}
	};

}	// namespace lsn
