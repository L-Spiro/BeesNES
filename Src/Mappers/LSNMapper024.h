/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 024 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Database/LSNDatabase.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper024
	 * \brief Mapper 024 implementation.
	 *
	 * Description: Mapper 024 implementation.
	 */
	class CMapper024 : public CMapperBase {
	public :
		CMapper024() {
		}
		virtual ~CMapper024() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 1 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

			m_pcPcbClass = iNesToPcb( m_prRom->riInfo.ui16Mapper, m_prRom->riInfo.ui16SubMapper );
			switch ( m_pcPcbClass ) {
				case CDatabase::LSN_PC_VRC6a : {
					m_pfSwizzleFunc = Swizzle_VRC6a_A0_A1;
					break;
				}
				case CDatabase::LSN_PC_VRC6b : {
					m_pfSwizzleFunc = Swizzle_VRC6b_A1_A0;
					break;
				}
			}

			// ================
			// FIXED BANKS
			// ================
			// Set the reads of the fixed bank at the end.		
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			m_vWram.resize( 0x8000 - 0x6000 );
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ReadWram, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper024::WriteWram, this, uint16_t( I - 0x6000 ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize() * 2>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<0, 0, 0, 0x0000, 0x0000, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<1, 0, 1, 0x0400, 0x0000, ChrBankSize()>, this, uint16_t( I - 0x0400 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<2, 1, 2, 0x0000, 0x0000, ChrBankSize()>, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<3, 1, 3, 0x0400, 0x0000, ChrBankSize()>, this, uint16_t( I - 0x0C00 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<4, 2, 3, 0x0000, 0x0000, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<5, 2, 3, 0x0400, 0x0400, ChrBankSize()>, this, uint16_t( I - 0x1400 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<6, 3, 4, 0x0000, 0x0000, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
			}
			for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper024::ChrBankRead_0000_1FFF<7, 3, 4, 0x0400, 0x0400, ChrBankSize()>, this, uint16_t( I - 0x1C00 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// $8000-$9FFF bank-select.
			for ( uint32_t I = 0x8000; I <= 0xFFFF; ++I ) {
				uint16_t ui16ddr = m_pfSwizzleFunc( uint16_t( I ) );
				if ( ui16ddr == 0x8000 || ui16ddr == 0x8001 || ui16ddr == 0x8002 || ui16ddr == 0x8003 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper024::SelectBank8000_BFFF, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0xB003 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper024::SelectStyle, this, uint16_t( I ) );
				}
				else if ( ui16ddr == 0xC000 || ui16ddr == 0xC001 || ui16ddr == 0xC002 || ui16ddr == 0xC003 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper024::SelectBankC000_DFFF, this, uint16_t( I ) );
				}
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Types.
		typedef uint16_t (*								PfSwizzle)( uint16_t );

		
		// == Members.
		/** WRAM. */
		std::vector<uint8_t>							m_vWram;
		/** The swizzle function. */
		PfSwizzle										m_pfSwizzleFunc = Swizzle_VRC6a_A0_A1;
		/** The PCB class. */
		CDatabase::LSN_PCB_CLASS						m_pcPcbClass;
		/** PPU banking style/nametable control register ($B003). */
		uint8_t											m_ui8B003;


		// == Functions.
		/**
		 * Swizzles A1 A0 to A0 A1.
		 * 
		 * \param _ui16Addr The address to swizzle.
		 * \return Returns the swizzled address.
		 **/
		static uint16_t									Swizzle_VRC6b_A1_A0( uint16_t _ui16Addr ) {
			return (_ui16Addr & 0xF000) |
				((_ui16Addr & 0b10) >> 1) |
				((_ui16Addr & 0b01) << 1);
		}

		/**
		 * Swizzles A0 A1 to A0 A1.
		 * 
		 * \param _ui16Addr The address to swizzle.
		 * \return Returns the swizzled address.
		 **/
		static uint16_t									Swizzle_VRC6a_A0_A1( uint16_t _ui16Addr ) {
			return (_ui16Addr & 0xF003);
		}

		/**
		 * Converts a mapper and submapper to a PCB.
		 * 
		 * \param _ui16Mapper The mapper.
		 * \param _ui16SubMapper The submapper.
		 * \return Returns the PCB class colloquially assigned to the mapper/submapper combination.
		 **/
		static inline CDatabase::LSN_PCB_CLASS			iNesToPcb( uint16_t _ui16Mapper, uint16_t /*_ui16SubMapper*/ ) {
			static struct LSN_MAPPING {
				uint16_t					ui16Mapper;
				CDatabase::LSN_PCB_CLASS	pcClass;
			}
			const mMappering[] = {
				{ 24,	CDatabase::LSN_PC_VRC6a },
				{ 26,	CDatabase::LSN_PC_VRC6b },
			};
			for ( auto I = LSN_ELEMENTS( mMappering ); I--; ) {
				if ( mMappering[I].ui16Mapper == _ui16Mapper ) { return mMappering[I].pcClass; }
			}
			return CDatabase::LSN_PC_VRC6b;
		}

		/**
		 * Selects a bank for $8000-$BFFF.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectBank8000_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize() * 2>( _ui8Val & 0b1111 );
		}

		/**
		 * Selects a bank for $C000-$DFFF.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectBankC000_DFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);
			pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val & 0b11111 );
		}

		/**
		 * Selects the banking style.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectStyle( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);
			pmThis->m_ui8B003 = _ui8Val;
		}

		/**
		 * Reads from CHR ROM using m_ui8PgmBanks[_uReg] to select a bank among _uSize-sized banks. accounting for banking style (mode)
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 * \tparam _uReg0 The register index for Mode 0.
		 * \tparam _uReg1 The register index for Mode 1.
		 * \tparam _uReg2 The register index for Mode 2/3.
		 * \tparam _uMode2Offset An offset to apply in Mode 2/3.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uReg0, unsigned _uReg1, unsigned _uReg2, unsigned _uMode1Offset, unsigned _uMode2Offset, unsigned _uSize>
		static void LSN_FASTCALL						ChrBankRead_0000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);

			switch ( pmThis->m_ui8B003 & 0b00000011 ) {
				case 0 : {
					_ui8Ret = pmThis->m_prRom->vChrRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8ChrBanks[_uReg0])*_uSize)];
					break;
				}
				case 1 : {
					_ui8Ret = pmThis->m_prRom->vChrRom.data()[size_t(_ui16Parm1+_uMode1Offset)+(size_t(pmThis->m_ui8ChrBanks[_uReg1])*_uSize)];
					break;
				}
				default : {
					_ui8Ret = pmThis->m_prRom->vChrRom.data()[size_t(_ui16Parm1+_uMode2Offset)+(size_t(pmThis->m_ui8ChrBanks[_uReg2])*_uSize)];
					break;
				}
			}
		}

		/**
		 * Reads WRAM $6000-$7FFF.
		 *
		 * \tparam _uM Matched against the M bit to decide on what to read.
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadWram( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);
			_ui8Ret = pmThis->m_vWram[_ui16Parm1];
		}

		/**
		 * Writes to WRAM $6000-$7FFF.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteWram( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);
			pmThis->m_vWram[_ui16Parm1] = _ui8Val;
		}
	};

}	// namespace lsn
