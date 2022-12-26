/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base claass for all mappers.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Roms/LSNRom.h"

namespace lsn {

	/**
	 * Class CMapperBase
	 * \brief A base claass for all mappers.
	 *
	 * Description: A base claass for all mappers.
	 */
	class CMapperBase {
	public :
		CMapperBase() :
			m_prRom( nullptr ),
			m_stFixedOffset( 0 ),
			m_mmMirror( LSN_MM_HORIZONTAL ),
			m_ui8PgmBank( m_ui8PgmBanks[0] ),
			m_ui8ChrBank( m_ui8ChrBanks[0] ) {
		}
		virtual ~CMapperBase() {
		}


		// == Enumerations.
		/** Mirroring modes. */
		enum LSN_MIRROR_MODE {
			LSN_MM_HORIZONTAL,
			LSN_MM_VERTICAL,
			LSN_MM_1_SCREEN_A,
			LSN_MM_1_SCREEN_B,
			LSN_MM_4_SCREENS,
		};


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom ) { m_prRom = &_rRom; }

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * /*_pbCpuBus*/, CPpuBus * /*_pbPpuBus*/ ) {}


	protected :
		// == Members.
		/** The ROM used to initialize this mapper. */
		LSN_ROM *										m_prRom;
		/** The offset of the fixed bank. */
		size_t											m_stFixedOffset;
		/** The PGM bank. */
		uint8_t &										m_ui8PgmBank;
		/** The CHR ROM bank. */
		uint8_t &										m_ui8ChrBank;
		/** Themirroring mode. */
		LSN_MIRROR_MODE									m_mmMirror;
		/** The PGM banks. */
		uint8_t											m_ui8PgmBanks[32];
		/** The CHR ROM banks. */
		uint8_t											m_ui8ChrBanks[32];


		// == Functions.
		/**
		 * Applies a controllable mirroring map.
		 *
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		void											ApplyControllableMirrorMap( CPpuBus * _pbPpuBus ) {
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				uint16_t ui16Root = ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
				//ui16Root += LSN_PPU_NAMETABLES;
				_pbPpuBus->SetReadFunc( uint16_t( I ), CMapperBase::Read_ControllableMirror, this, ui16Root );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), CMapperBase::Write_ControllableMirror, this, ui16Root );
			}
		}

		/**
		 * Applies a default CHR ROM map (read pointers set, writes disabled).
		 *
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		void											ApplyStdChrRom( CPpuBus * _pbPpuBus ) {
			if ( m_prRom && m_prRom->vChrRom.size() ) {
				m_ui8ChrBank = 0;
				for ( uint32_t I = LSN_PPU_PATTERN_TABLES; I < LSN_PPU_NAMETABLES; ++I ) {
					uint16_t ui16MappedAddr = ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE);
					ui16MappedAddr %= m_prRom->vChrRom.size();
					_pbPpuBus->SetReadFunc( uint16_t( I ), CMapperBase::ChrBankRead_2000, this, ui16MappedAddr | LSN_PPU_PATTERN_TABLES );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), CPpuBus::StdWrite, this, ui16MappedAddr | LSN_PPU_PATTERN_TABLES );
				}
			}
		}

		/**
		 * A standard mapper PGM trampoline read function.  Maps an address to a given byte in the ROM's PGM space.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						StdMapperCpuRead_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CCpuBus::LSN_TRAMPOLINE * ptTramp = reinterpret_cast<CCpuBus::LSN_TRAMPOLINE *>(_pvParm0);
			CMapperBase * pmBase = reinterpret_cast<CMapperBase *>(ptTramp->pvReaderParm0);
			_ui8Ret = pmBase->m_prRom->vPrgRom[_ui16Parm1];
		}

		/**
		 * A standard mapper PGM read function.  Maps an address to a given byte in the ROM's PGM space.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						StdMapperCpuRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmBase = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmBase->m_prRom->vPrgRom[_ui16Parm1];
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_8000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+(pmThis->m_ui8PgmBank*0x8000)];
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_4000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+(pmThis->m_ui8PgmBank*0x4000)];
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+(pmThis->m_ui8PgmBank*0x2000)];
		}

		/**
		 * Reads the fixed bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_Fixed( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+pmThis->m_stFixedOffset];
		}

		/**
		 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrBankRead_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBank*0x2000+_ui16Parm1];
		}

		/**
		 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current 4-kilobyte bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrBankRead_1000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBank*0x1000+_ui16Parm1];
		}

		/**
		 * Converts an address in the range from LSN_PPU_NAMETABLES to LSN_PPU_NAMETABLES_END into a mirrored address given the mirroring type.
		 *
		 * \param _ui16Addr The address to mirror.
		 * \param _mmMode The mirroring mode to apply
		 * \return Returns the mirrored address.
		 */
		static inline uint16_t							MirrorAddress( uint16_t _ui16Addr, LSN_MIRROR_MODE _mmMode ) {
			switch ( _mmMode ) {
				case LSN_MM_HORIZONTAL : {
					uint16_t ui16Root = (_ui16Addr /*- LSN_PPU_NAMETABLES*/);
					// Mirror $2400 to $2000 and $2C00 to $2800.
					ui16Root = (ui16Root & (LSN_PPU_NAMETABLES_SCREEN - 1)) + ((ui16Root >> 11/*/ (LSN_PPU_NAMETABLES_SCREEN * 2)*/) * (LSN_PPU_NAMETABLES_SCREEN * 1));
					return ui16Root | LSN_PPU_NAMETABLES;
				}
				case LSN_MM_VERTICAL : {
					uint16_t ui16Root = (_ui16Addr /*- LSN_PPU_NAMETABLES*/);
					ui16Root %= LSN_PPU_NAMETABLES_SCREEN * 2;									// Map $2800 to $2000 and $2C00 to $2400.
					return ui16Root | LSN_PPU_NAMETABLES;
				}
				case LSN_MM_1_SCREEN_A : {
					uint16_t ui16Root = (_ui16Addr /*- LSN_PPU_NAMETABLES*/);
					return (ui16Root & (LSN_PPU_NAMETABLES_SCREEN - 1)) | LSN_PPU_NAMETABLES;
				}
				case LSN_MM_1_SCREEN_B : {
					uint16_t ui16Root = (_ui16Addr /*- LSN_PPU_NAMETABLES*/);
					return (ui16Root & (LSN_PPU_NAMETABLES_SCREEN - 1)) | LSN_PPU_NAMETABLES | LSN_PPU_NAMETABLES_SCREEN;
				}
				case LSN_MM_4_SCREENS : {}
				default : {
					return _ui16Addr | LSN_PPU_NAMETABLES_SCREEN;
				}
			}
		}

		/**
		 * Handles reads from controllable mirror.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_ControllableMirror( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = _pui8Data[MirrorAddress( _ui16Parm1, pmThis->m_mmMirror )];
		}

		/**
		 * Handles writes to controllable mirror.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write_ControllableMirror( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_pui8Data[MirrorAddress( _ui16Parm1, pmThis->m_mmMirror )] = _ui8Val;
		}

	};

}	// namespace lsn
