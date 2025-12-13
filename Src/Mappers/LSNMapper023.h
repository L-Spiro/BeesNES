/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 023 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Database/LSNDatabase.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper023
	 * \brief Mapper 023 implementation.
	 *
	 * Description: Mapper 023 implementation.
	 */
	class CMapper023 : public CMapperBase {
	public :
		CMapper023() {
		}
		virtual ~CMapper023() {
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
			std::memset( m_ui8ChrBanksVrc, 0, sizeof( m_ui8ChrBanksVrc ) );
			std::memset( m_ui8DefaultChrRam, 0, sizeof( m_ui8DefaultChrRam ) );
			std::memset( m_ui8ChrBanks, 0, sizeof( m_ui8ChrBanks ) );
			std::memset( m_ui8PgmBanks, 0, sizeof( m_ui8PgmBanks ) );
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

			/*for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &DefaultChrRamRead, this, uint16_t( I - 0x0000 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &DefaultChrRamWrite, this, uint16_t( I - 0x0000 ) );
			}*/

			m_pcPcbClass = iNesToPcb( m_prRom->riInfo.ui16Mapper, m_prRom->riInfo.ui16SubMapper );
			switch ( m_pcPcbClass ) {
				case CDatabase::LSN_PC_VRC4b : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC2c : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC2a : {
					m_pfSwizzleFunc = Swizzle_VRC2a_VRC2c_VRC4b_A1_A0;
					break;
				}
				case CDatabase::LSN_PC_VRC4f : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC2b : {
					m_pfSwizzleFunc = Swizzle_VRC2b_VRC4f_A0_A1;
					break;
				}
				case CDatabase::LSN_PC_VRC4a : {
					m_pfSwizzleFunc = Swizzle_VRC4a_A1_A2;
					break;
				}
				case CDatabase::LSN_PC_VRC4c : {
					m_pfSwizzleFunc = Swizzle_VRC4c_A6_A7;
					break;
				}
				case CDatabase::LSN_PC_VRC4d : {
					m_pfSwizzleFunc = Swizzle_VRC4d_A3_A2;
					break;
				}
				case CDatabase::LSN_PC_VRC4e : {
					m_pfSwizzleFunc = Swizzle_VRC4e_A2_A3;
					break;
				}
			}


			// ================
			// SWAPPABLE BANKS
			// ================
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x0400 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, ChrBankSize()>, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, ChrBankSize()>, this, uint16_t( I - 0x0C00 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<4, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<5, ChrBankSize()>, this, uint16_t( I - 0x1400 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<6, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
			}
			for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<7, ChrBankSize()>, this, uint16_t( I - 0x1C00 ) );
			}

			switch ( ClassifyVrc( m_pcPcbClass ) ) {
				case 2 : {
					ApplyMap_Vrc2( _pbCpuBus, _pbPpuBus );
					return;
				}
				case 4 : {
					ApplyMap_Vrc4( _pbCpuBus, _pbPpuBus );
					return;
				}
			}
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			m_viIrq.Tick( m_pInterruptable );
		}

	protected :
		// == Types.
		typedef uint16_t (*								PfSwizzle)( uint16_t );

		
		// == Members.
		/** WRAM. */
		std::vector<uint8_t>							m_vWram;
		/** VRC4 IRQ. */
		CVrcIrq											m_viIrq;
		/** The swizzle function. */
		PfSwizzle										m_pfSwizzleFunc = nullptr;
		/** The 2nd-to-last bank (VRC4). */
		size_t											m_s2ndToLast = 0;
		/** CHR banks. */
		uint8_t											m_ui8ChrBanksVrc[16];
		/** The PCB class. */
		CDatabase::LSN_PCB_CLASS						m_pcPcbClass;
		/** Microwire. */
		uint8_t											m_ui8MicroWire;
		/** Swap mode/WRAM contrl. */
		uint8_t											m_ui8SwapWram = 0;



		// == Functions.
		/**
		 * Applies VRC2 mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 **/
		void											ApplyMap_Vrc2( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			// ================
			// FIXED BANKS
			// ================
			// Set the reads of the fixed bank at the end.
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() * 2 ) - PgmBankSize() * 2;
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xC000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			if ( m_prRom->i32SaveRamSize ) {
				m_vWram.resize( 0x8000 - 0x6000 );
				for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper023::ReadWram<false>, this, uint16_t( I - 0x6000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::WriteWram<false>, this, uint16_t( I - 0x6000 ) );
				}
			}
			else {
				for ( uint32_t I = 0x6000; I < 0x7000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper023::ReadMicroWire6000_6FFF, this, uint16_t( I - 0x6000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::WriteMicroWire6000_6FFF, this, uint16_t( I - 0x6000 ) );
				}
				for ( uint32_t I = 0x7000; I < 0x8000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CCpuBus::NoRead, this, uint16_t( I - 0x7000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CCpuBus::NoWrite, this, uint16_t( I - 0x7000 ) );
				}
			}

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I <= 0x9FFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I <= 0xBFFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
			}

			// ================
			// BANK-SELECT
			// ================
			// $8000-$9FFF bank-select.
			for ( uint32_t I = 0x8000; I <= 0x8FFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectBank8000_8FFF, this, uint16_t( I ) );
			}
			// Mirroring.
			for ( uint32_t I = 0x9000; I <= 0x9FFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectMirror9000_9FFF<true>, this, uint16_t( I ) );
			}
			// $A000-$BFFF bank-select.
			for ( uint32_t I = 0xA000; I <= 0xAFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectBankA000_AFFF, this, uint16_t( I ) );
			}
			// CHR bank-select.
			for ( uint32_t I = 0xB000; I <= 0xBFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC2<0>, this, uint16_t( I ) );
			}
			for ( uint32_t I = 0xC000; I <= 0xCFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC2<2>, this, uint16_t( I ) );
			}
			for ( uint32_t I = 0xD000; I <= 0xDFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC2<4>, this, uint16_t( I ) );
			}
			for ( uint32_t I = 0xE000; I <= 0xEFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC2<6>, this, uint16_t( I ) );
			}

			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}

		/**
		 * Applies VRC4 mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 **/
		void											ApplyMap_Vrc4( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			// ================
			// FIXED BANKS
			// ================
			// Set the reads of the fixed bank at the end.
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize() * 1;
			m_s2ndToLast = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() * 2 ) - PgmBankSize() * 2;
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			m_vWram.resize( 0x8000 - 0x6000 );
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper023::ReadWram<true>, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::WriteWram<true>, this, uint16_t( I - 0x6000 ) );
			}
			/*if ( m_prRom->i32SaveRamSize == 8 * 1024 ) {
				for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper023::ReadWram<true>, this, uint16_t( I - 0x6000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::WriteWram<true>, this, uint16_t( I - 0x6000 ) );
				}
			}
			else {
				for ( uint32_t I = 0x6000; I < 0x7000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper023::ReadWram<true>, this, uint16_t( (I - 0x6000) & 0x7FF ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::WriteWram<true>, this, uint16_t( (I - 0x6000) & 0x7FF ) );
				}
				for ( uint32_t I = 0x7000; I < 0x8000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CCpuBus::NoRead, this, uint16_t( I - 0x6000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CCpuBus::NoWrite, this, uint16_t( I - 0x6000 ) );
				}
			}*/

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I <= 0x9FFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper023::Read8000_9FFF_or_C000_DFFF<0b00>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I <= 0xBFFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I <= 0xDFFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper023::Read8000_9FFF_or_C000_DFFF<0b10>, this, uint16_t( I - 0xC000 ) );
			}

			// ================
			// BANK-SELECT
			// ================
			// $8000-$9FFF bank-select.
			for ( uint32_t I = 0x8000; I <= 0x8FFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectBank8000_8FFF, this, uint16_t( I ) );
			}
			// Mirroring.
			for ( uint32_t I = 0x9000; I <= 0x9FFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectMirror9000_9FFF<false>, this, uint16_t( I ) );
			}
			// $A000-$BFFF bank-select.
			for ( uint32_t I = 0xA000; I <= 0xAFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectBankA000_AFFF, this, uint16_t( I ) );
			}
			// $F000-$FFFF IRQ.
			for ( uint32_t I = 0xF000; I <= 0xFFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::WriteIrqF000_FFFF, this, uint16_t( I ) );
			}
			// CHR bank-select.
			for ( uint32_t I = 0xB000; I <= 0xBFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC4<0>, this, uint16_t( I ) );
			}
			for ( uint32_t I = 0xC000; I <= 0xCFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC4<2>, this, uint16_t( I ) );
			}
			for ( uint32_t I = 0xD000; I <= 0xDFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC4<4>, this, uint16_t( I ) );
			}
			for ( uint32_t I = 0xE000; I <= 0xEFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper023::SelectChrBank_VRC4<6>, this, uint16_t( I ) );
			}

			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}

		/**
		 * Classifies a RVC PCB as 2, 4, 6, or 7.
		 * 
		 * \param _pcVrc The PCB type to classify.
		 * \return Returns the VRC* number.
		 **/
		static uint8_t									ClassifyVrc( CDatabase::LSN_PCB_CLASS _pcVrc ) {
			switch ( _pcVrc ) {
				case CDatabase::LSN_PC_VRC2c : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC2b : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC2a : { return 2; }
				case CDatabase::LSN_PC_VRC4f : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC4e : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC4d : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC4c : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC4b : {}			LSN_FALLTHROUGH
				case CDatabase::LSN_PC_VRC4a : { return 4; }
			}
			return 0;
		}

		/**
		 * Swizzles A1 A0 to A0 A1.
		 * 
		 * \param _ui16Addr The address to swizzle.
		 * \return Returns the swizzled address.
		 **/
		static uint16_t									Swizzle_VRC2a_VRC2c_VRC4b_A1_A0( uint16_t _ui16Addr ) {
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
		static uint16_t									Swizzle_VRC2b_VRC4f_A0_A1( uint16_t _ui16Addr ) {
			return (_ui16Addr & 0xF003);
		}

		/**
		 * Swizzles A1 A2 to A0 A1.
		 * 
		 * \param _ui16Addr The address to swizzle.
		 * \return Returns the swizzled address.
		 **/
		static uint16_t									Swizzle_VRC4a_A1_A2( uint16_t _ui16Addr ) {
			return (_ui16Addr & 0xF000) |
				((_ui16Addr >> 1) & 0b11);
		}

		/**
		 * Swizzles A6 A7 to A0 A1.
		 * 
		 * \param _ui16Addr The address to swizzle.
		 * \return Returns the swizzled address.
		 **/
		static uint16_t									Swizzle_VRC4c_A6_A7( uint16_t _ui16Addr ) {
			return (_ui16Addr & 0xF000) |
				((_ui16Addr >> 6) & 0b11);
		}

		/**
		 * Swizzles A3 A2 to A0 A1.
		 * 
		 * \param _ui16Addr The address to swizzle.
		 * \return Returns the swizzled address.
		 **/
		static uint16_t									Swizzle_VRC4d_A3_A2( uint16_t _ui16Addr ) {
			return (_ui16Addr & 0xF000) |
				((_ui16Addr >> 3) & 0b01) |
				((_ui16Addr >> 1) & 0b10);
		}

		/**
		 * Swizzles A2 A3 to A0 A1.
		 * 
		 * \param _ui16Addr The address to swizzle.
		 * \return Returns the swizzled address.
		 **/
		static uint16_t									Swizzle_VRC4e_A2_A3( uint16_t _ui16Addr ) {
			return (_ui16Addr & 0xF000) |
				((_ui16Addr >> 2) & 0b11);
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
				uint16_t					ui16SubMapper;
				CDatabase::LSN_PCB_CLASS	pcClass;
			}
			const mMappering[] = {
				{ 22,	0,	CDatabase::LSN_PC_VRC2a },
				{ 23,	3,	CDatabase::LSN_PC_VRC2b },
				{ 25,	3,	CDatabase::LSN_PC_VRC2c },

				{ 21,	1,	CDatabase::LSN_PC_VRC4a },
				{ 25,	1,	CDatabase::LSN_PC_VRC4b },
				{ 21,	2,	CDatabase::LSN_PC_VRC4c },
				{ 25,	2,	CDatabase::LSN_PC_VRC4d },
				{ 23,	2,	CDatabase::LSN_PC_VRC4e },
				{ 23,	1,	CDatabase::LSN_PC_VRC4f },
			};
			for ( auto I = std::size( mMappering ); I--; ) {
				if ( mMappering[I].ui16Mapper == _ui16Mapper && mMappering[I].ui16SubMapper == _ui16SubMapper ) { return mMappering[I].pcClass; }
			}
			return CDatabase::LSN_PC_VRC4f;
		}

		/**
		 * Selects a CHR bank for VRC2.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBnkIdx>
		static void LSN_FASTCALL						SelectChrBank_VRC2( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 ) & 0x0FFF;
			if ( ui16ddr == 0x0000 || ui16ddr == 0x0001 ) {
				if ( ui16ddr == 0x0000 ) {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+0] = _ui8Val & 0b1111;
				}
				else /*if ( ui16ddr == 0x0001 )*/ {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+1] = _ui8Val & 0b1111;
				}
				uint8_t ui8Tmp = pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+0] | (pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+1] << 4);
				if ( pmThis->m_pcPcbClass == CDatabase::LSN_PC_VRC2a ) {
					pmThis->SetChrBank<_uBnkIdx, ChrBankSize()>( ui8Tmp >> 1 );
				}
				else {
					pmThis->SetChrBank<_uBnkIdx, ChrBankSize()>( ui8Tmp );
				}
			}
			else if ( ui16ddr == 0x0002 || ui16ddr == 0x0003 ) {
				if ( ui16ddr == 0x0002 ) {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+2] = _ui8Val & 0b1111;
				}
				else /*if ( ui16ddr == 0x0003 )*/ {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+3] = _ui8Val & 0b1111;
				}
				uint8_t ui8Tmp = pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+2] | (pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+3] << 4);
				if ( pmThis->m_pcPcbClass == CDatabase::LSN_PC_VRC2a ) {
					pmThis->SetChrBank<_uBnkIdx+1, ChrBankSize()>( ui8Tmp >> 1 );
				}
				else {
					pmThis->SetChrBank<_uBnkIdx+1, ChrBankSize()>( ui8Tmp );
				}
			}
		}

		/**
		 * Selects a CHR bank for VRC2.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBnkIdx>
		static void LSN_FASTCALL						SelectChrBank_VRC4( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 ) & 0x0FFF;
			if ( ui16ddr == 0x0000 || ui16ddr == 0x0001 ) {
				if ( ui16ddr == 0x0000 ) {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+0] = _ui8Val & 0b01111;
				}
				else /*if ( ui16ddr == 0x0001 )*/ {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+1] = _ui8Val & 0b11111;
				}
				uint8_t ui8Tmp = pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+0] | (pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+1] << 4);
				pmThis->SetChrBank<_uBnkIdx, ChrBankSize()>( ui8Tmp );
			}
			else if ( ui16ddr == 0x0002 || ui16ddr == 0x0003 ) {
				if ( ui16ddr == 0x0002 ) {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+2] = _ui8Val & 0b01111;
				}
				else /*if ( ui16ddr == 0x0003 )*/ {
					pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+3] = _ui8Val & 0b11111;
				}
				uint8_t ui8Tmp = pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+2] | (pmThis->m_ui8ChrBanksVrc[_uBnkIdx*2+3] << 4);
				pmThis->SetChrBank<_uBnkIdx+1, ChrBankSize()>( ui8Tmp );
			}
		}

		/**
		 * Selects a bank for $8000-$9FFF (VRC2).
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectBank8000_8FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			if ( ui16ddr == 0x8000 || ui16ddr == 0x8001 || ui16ddr == 0x8002 || ui16ddr == 0x8003 ) {
				pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b11111 );
			}
		}

		/**
		 * Selects a bank for $A000-$BFFF (VRC2).
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectBankA000_AFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			if ( ui16ddr == 0xA000 || ui16ddr == 0xA001 || ui16ddr == 0xA002 || ui16ddr == 0xA003 ) {
				pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val & 0b11111 );
			}
		}

		/**
		 * Selects a mirroring mode.
		 * 
		 * \tparam _bVrc2 If VRC4, $9002 becomes a Swap Mode/WRAM controller and mirroring is only on $9000. 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		template <bool _bVrc2>
		static void LSN_FASTCALL						SelectMirror9000_9FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			if constexpr ( _bVrc2 ) {
				if ( ui16ddr == 0x9000 || ui16ddr == 0x9001 || ui16ddr == 0x9002 || ui16ddr == 0x9003 ) {
					switch ( _ui8Val & 0b01 ) {
						case 0b00 : {
							pmThis->m_mmMirror = LSN_MM_VERTICAL;
							break;
						}
						case 0b01 : {
							pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
							break;
						}
					}
				}
			}
			else {
				if ( ui16ddr == 0x9000 ) {
					switch ( _ui8Val & 0b11 ) {
						case 0b00 : {
							pmThis->m_mmMirror = LSN_MM_VERTICAL;
							break;
						}
						case 0b01 : {
							pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
							break;
						}
						case 0b10 : {
							pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
							break;
						}
						case 0b11 : {
							pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
							break;
						}
					}
				}
				else if ( ui16ddr == 0x9002 ) {
					pmThis->m_ui8SwapWram = _ui8Val;
				}
			}
		}

		/**
		 * Reads from the Microwire latch (VRC2).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadMicroWire6000_6FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			_ui8Ret = (_ui8Ret & 0b11111110) | (pmThis->m_ui8MicroWire & 0b00000001);
		}

		/**
		 * Writes to the Microwire latch (VRC2).
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteMicroWire6000_6FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			pmThis->m_ui8MicroWire = _ui8Val;
		}

		/**
		 * Reads from the swappable $8000-$9FFF/$C000-$DFFF bank (VRC4).  If the M bit matches the _uM value, the $8000 bank is read, otherwise the second-to-last fixed bank is used.
		 *
		 * \tparam _uM Matched against the M bit to decide on what to read.
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		template <unsigned _uM>
		static void LSN_FASTCALL						Read8000_9FFF_or_C000_DFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			if ( (pmThis->m_ui8SwapWram & 0b10) == _uM ) {
				CMapperBase::PgmBankRead<0, PgmBankSize()>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
			}
			else {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[_ui16Parm1+pmThis->m_s2ndToLast];
			}
		}

		/**
		 * Writes to the IRQ registers (VRC4).
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqF000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			uint16_t ui16ddr = pmThis->m_pfSwizzleFunc( _ui16Parm1 );
			if ( ui16ddr == 0xF000 ) {
				pmThis->m_viIrq.SetLatch_Lo4( _ui8Val );
			}
			else if ( ui16ddr == 0xF001 ) {
				pmThis->m_viIrq.SetLatch_Hi4( _ui8Val );
			}
			else if ( ui16ddr == 0xF002 ) {
				pmThis->m_viIrq.SetControl( _ui8Val, pmThis->m_pInterruptable );
			}
			else if ( ui16ddr == 0xF003 ) {
				pmThis->m_viIrq.AcknowledgeIrq( pmThis->m_pInterruptable );
			}
		}

		/**
		 * Reads WRAM $6000-$7FFF/$C000-$DFFF bank (VRC4).  If the M bit matches the _uM value, the $8000 bank is read, otherwise the second-to-last fixed bank is used.
		 *
		 * \tparam _uM Matched against the M bit to decide on what to read.
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		template <bool _bVrc4>
		static void LSN_FASTCALL						ReadWram( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			if constexpr ( _bVrc4 ) {
				if ( pmThis->m_ui8SwapWram & 0b01 ) {
					_ui8Ret = pmThis->m_vWram[_ui16Parm1];
				}
			}
			else {
				_ui8Ret = pmThis->m_vWram[_ui16Parm1];
			}
		}

		/**
		 * Writes to WRAM (VRC4).
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		template <bool _bVrc4>
		static void LSN_FASTCALL						WriteWram( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper023 * pmThis = reinterpret_cast<CMapper023 *>(_pvParm0);
			if constexpr ( _bVrc4 ) {
				if ( pmThis->m_ui8SwapWram & 0b01 ) {
					pmThis->m_vWram[_ui16Parm1] = _ui8Val;
				}
			}
			else {
				pmThis->m_vWram[_ui16Parm1] = _ui8Val;
			}
		}
	};

}	// namespace lsn
