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
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper024::SelectBankY000_YFFF, this, 0 );	// Treated as ROM.
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
		PfSwizzle										m_pfSwizzleFunc = nullptr;
		/** The PCB class. */
		CDatabase::LSN_PCB_CLASS						m_pcPcbClass;


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
		static inline CDatabase::LSN_PCB_CLASS			iNesToPcb( uint16_t _ui16Mapper, uint16_t _ui16SubMapper ) {
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
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank = 0>
		static void LSN_FASTCALL						SelectBankY000_YFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);
			pmThis->SetPgmBank<_uBank, PgmBankSize()>( _ui8Val & 0b1111 );
		}

		/**
		 * Selects a CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank = 0>
		static void LSN_FASTCALL						SelectChrBankX000_XFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper024 * pmThis = reinterpret_cast<CMapper024 *>(_pvParm0);
			pmThis->SetChrBank<_uBank, ChrBankSize()>( _ui8Val );
		}
	};

}	// namespace lsn
