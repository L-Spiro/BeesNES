/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base class for all mappers.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Cpu/LSNCpuBase.h"
#include "../Roms/LSNRom.h"
#include "../System/LSNBussable.h"
#include "../System/LSNInterruptable.h"

namespace lsn {

	/**
	 * Class CMapperBase
	 * \brief A base class for all mappers.
	 *
	 * Description: A base class for all mappers.
	 */
	class CMapperBase {
	public :
		CMapperBase() :
			m_prRom( nullptr ),
			m_pcbCpu( nullptr ),
			m_pbPpuBus( nullptr ),
			m_stFixedOffset( 0 ),
			m_mmMirror( LSN_MM_HORIZONTAL ),
			m_ui8PgmBank( m_ui8PgmBanks[0] ),
			m_ui8ChrBank( m_ui8ChrBanks[0] ) {
		}
		virtual ~CMapperBase() {
		}


		// == Types.
		/**
		 * Class CVrcIrq
		 * \brief The VRC IRQ handler.
		 *
		 * Description: The VRC IRQ handler.
		 */
		class CVrcIrq {
		public :
			CVrcIrq() {
			}
			~CVrcIrq() {
			}


			// == Functions.
			/**
			 * Sets the low 4 bits of the latch.
			 * 
			 * \param _ui8Val The low 4 bits to set as the low 4 bits of the latch (reload) value.
			 **/
			inline void									SetLatch_Lo4( uint8_t _ui8Val ) {
				m_ui8Reload = (m_ui8Reload & 0xF0) | (_ui8Val & 0x0F);
			}

			/**
			 * Sets the high 4 bits of the latch.
			 * 
			 * \param _ui8Val The high 4 bits to set as the low 4 bits of the latch (reload) value.
			 **/
			inline void									SetLatch_Hi4( uint8_t _ui8Val ) {
				m_ui8Reload = (m_ui8Reload & 0x0F) | ((_ui8Val & 0x0F) << 4);
			}

			/**
			 * Sets the latch.
			 * 
			 * \param _ui8Val The value  to set as the latch (reload) value.
			 **/
			inline void									SetLatch( uint8_t _ui8Val ) {
				m_ui8Reload = _ui8Val;
			}

			/**
			 * Sets the control register.
			 * 
			 * \param _ui8Val The heigh 4 bits to set as the low 4 bits of the latch (reload) value.
			 * \param _pcbCpu A pointer to the CPU.
			 **/
			inline void									SetControl( uint8_t _ui8Val, CInterruptable * _piCpu ) {
				m_ui8Control = _ui8Val;

				m_i16Prescaler = 341;
				_piCpu->ClearIrq( LSN_IS_MAPPER );
				if ( _ui8Val & 0b00000010 ) {	// E.
					m_ui8Counter = m_ui8Reload;
				}
			}

			/**
			 * Acknowledges IRQ's.
			 * 
			 * \param _piCpu The IRQ handler.
			 **/
			inline void									AcknowledgeIrq( CInterruptable * _piCpu ) {
				_piCpu->ClearIrq( LSN_IS_MAPPER );
				m_ui8Control = (m_ui8Control & 0b00000101) | ((m_ui8Control & 0b00000001) << 1);
			}

			/**
			 * Performs a tick.
			 * 
			 * \param _piCpu The IRQ handler.
			 **/
			inline void									Tick( CInterruptable * _piCpu ) {
				if ( m_ui8Control & 0b00000010 ) {			// E bit set.
					bool bClockCounter = false;
					if ( !(m_ui8Control & 0b00000100) ) {	// M bit clear.
						m_i16Prescaler -= 3;
						if ( m_i16Prescaler <= 0 ) {
							m_i16Prescaler += 341;
							bClockCounter = true;
						}
					}
					else {									// M bit set.
						bClockCounter = true;
					}
					if ( bClockCounter ) {
						if LSN_UNLIKELY( m_ui8Counter == 0xFF ) {
							m_ui8Counter = m_ui8Reload;
							_piCpu->Irq( LSN_IS_MAPPER );
						}
						else {
							++m_ui8Counter;
						}
					}
				}
			}


		protected :
			// == Members.
			/** The prescaler. */
			int16_t										m_i16Prescaler = 341;
			/** The reload value. */
			uint8_t										m_ui8Reload = 0;
			/** The counter. */
			uint8_t										m_ui8Counter = 0;
			/** The control value. */
			uint8_t										m_ui8Control = 0;
		};

		/**
		 * Class CVrcIrq3
		 * \brief The VRC IRQ handler.
		 *
		 * Description: The VRC IRQ handler.
		 */
		class CVrcIrq3 {
		public :
			CVrcIrq3() {
				m_ui16Reload = 0;
				m_ui16Counter = 0;
			}
			~CVrcIrq3() {
			}


			// == Functions.
			/**
			 * Sets bits 0-3 of the reload value.
			 * 
			 * \param _ui8Val The low 4 bits to set as the low 4 bits of the latch (reload) value.
			 **/
			inline void									SetLatch_0_3( uint8_t _ui8Val ) {
				m_ui16Reload = (m_ui16Reload & 0xFFF0) | (_ui8Val & 0x0F);
			}

			/**
			 * Sets bits 4-7 of the reload value.
			 * 
			 * \param _ui8Val The high 4 bits to set as the low 4 bits of the latch (reload) value.
			 **/
			inline void									SetLatch_4_7( uint8_t _ui8Val ) {
				m_ui16Reload = (m_ui16Reload & 0xFF0F) | ((_ui8Val & 0x0F) << 4);
			}

			/**
			 * Sets bits 8-11 of the reload value.
			 * 
			 * \param _ui8Val The high 4 bits to set as the low 4 bits of the latch (reload) value.
			 **/
			inline void									SetLatch_8_11( uint8_t _ui8Val ) {
				m_ui16Reload = uint16_t( (m_ui16Reload & 0xF0FF) | (uint16_t( _ui8Val & 0x0F ) << 8) );
			}

			/**
			 * Sets bits 12-15 of the reload value.
			 * 
			 * \param _ui8Val The high 4 bits to set as the low 4 bits of the latch (reload) value.
			 **/
			inline void									SetLatch_12_15( uint8_t _ui8Val ) {
				m_ui16Reload = uint16_t( (m_ui16Reload & 0x0FFF) | (uint16_t( _ui8Val & 0x0F ) << 12) );
			}

			/**
			 * Sets the control register.
			 * 
			 * \param _ui8Val The heigh 4 bits to set as the low 4 bits of the latch (reload) value.
			 * \param _pcbCpu A pointer to the CPU.
			 **/
			inline void									SetControl( uint8_t _ui8Val, CInterruptable * _piCpu ) {
				m_ui8Control = _ui8Val;

				_piCpu->ClearIrq( LSN_IS_MAPPER );
				if ( _ui8Val & 0b00000010 ) {				// E.
					m_ui16Counter = m_ui16Reload;
				}
			}

			/**
			 * Acknowledges IRQ's.
			 * 
			 * \param _piCpu The IRQ handler.
			 **/
			inline void									AcknowledgeIrq( CInterruptable * _piCpu ) {
				_piCpu->ClearIrq( LSN_IS_MAPPER );
				m_ui8Control = (m_ui8Control & 0b00000101) | ((m_ui8Control & 0b00000001) << 1);
			}

			/**
			 * Performs a tick.
			 * 
			 * \param _piCpu The IRQ handler.
			 **/
			inline void									Tick( CInterruptable * _piCpu ) {
				if ( m_ui8Control & 0b00000010 ) {			// E bit set.
					if ( m_ui8Control & 0b00000100 ) {		// M bit set.
						//if LSN_UNLIKELY( m_ui8Counter[0] == 0xFF ) {
						if LSN_UNLIKELY( ++m_ui8Counter[0] == 0x00 ) {
							m_ui8Counter[0] = m_ui8Reload[0];
							_piCpu->Irq( LSN_IS_MAPPER );
						}
						/*else {
							++m_ui8Counter[0];
						}*/
					}
					else {
						//if LSN_UNLIKELY( m_ui16Counter == 0xFFFF ) {
						if LSN_UNLIKELY( ++m_ui16Counter == 0x0000 ) {
							m_ui16Counter = m_ui16Reload;
							_piCpu->Irq( LSN_IS_MAPPER );
						}
						/*else {
							++m_ui16Counter;
						}*/
					}
				}
			}


		protected :
			// == Members.
			/** The reload value. */
			union {
				uint16_t								m_ui16Reload;
				uint8_t									m_ui8Reload[2];
			};
			/** The counter. */
			union {
				uint16_t								m_ui16Counter;
				uint8_t									m_ui8Counter[2];
			};
			/** The control value. */
			uint8_t										m_ui8Control = 0;
		};


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			m_prRom = &_rRom;
			m_pcbCpu = _pcbCpuBase;
			m_pbPpuBus = _pbPpuBus;
			m_pInterruptable = _piInter;
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * /*_pbCpuBus*/, CPpuBus * _pbPpuBus ) {
			if ( m_prRom->vChrRom.size() == 0 ) {
				// RAM.
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &DefaultChrRamRead, this, uint16_t( I - 0x0000 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &DefaultChrRamWrite, this, uint16_t( I - 0x0000 ) );
				}
			}
			else {
				ApplyStdChrRom( _pbPpuBus );
			}
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {}

		/**
		 * Saves its battery-backed RAM to the file specified in m_prRom.u16SaveFilePrefix + u".sav".
		 * 
		 * \return Returns true if the operation completed as expected.  False indicates a file-access error.
		 **/
		virtual bool									SaveBatteryBacked() { return true; }

		/**
		 * Called to inform the mapper of a reset.
		 **/
		virtual void									Reset() {}

		/**
		 * Gets the extended audio sample.
		 * 
		 * \param _fApuSample The APU sample to mix with the extended-audio sample.
		 **/
		virtual float									GetExtAudio( float _fApuSample ) { return _fApuSample; }

		/**
		 * Applies any post affects to the audio sample.  The sample has been filtered and down-sampled to the output Hz by this point in the pipeline.
		 * 
		 * \param _fSample The sample to potentially modify.
		 * \param _fHz The output Hz.
		 * \return Returns the potentially modified sample.
		 **/
		virtual float									PostProcessAudioSample( float _fSample, float /*_fHz*/ ) { return _fSample; }

		/**
		 * Applies a mirroring mode to a PPU bus.
		 *
		 * \param _ui16Mirror The mirroring mode to apply (a LSN_MIRROR_MODE value).
		 * \param _pbPpuBus A pointer to the PPU bus.
		 * \param _pvParm0 The pointer to pass to the read/write functions.
		 * \param _ui16NametableStart Start of the addresses to map.
		 * \param _ui16NametableEnd End of the range of addresses to map.
		 */
		static void										ApplyMirroring( uint16_t _ui16Mirror, CPpuBus * _pbPpuBus, void * _pvParm0,
			uint16_t _ui16NametableStart = LSN_PPU_NAMETABLES,
			uint16_t _ui16NametableEnd = LSN_PPU_MEM_FULL_SIZE ) {
			for ( uint32_t I = _ui16NametableStart; I < _ui16NametableEnd; ++I ) {
				uint16_t ui16Root = ((I - _ui16NametableStart) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
				uint16_t ui16Final = MirrorAddress( ui16Root, static_cast<LSN_MIRROR_MODE>(_ui16Mirror) );
				// MirrorAddress() is built for speed since it is used for run-time mirroring, so it
				//	always sets LSN_PPU_NAMETABLES.
				ui16Final &= ~LSN_PPU_NAMETABLES;
				ui16Final += _ui16NametableStart;
				_pbPpuBus->SetReadFunc( uint16_t( I ), CPpuBus::StdRead, _pvParm0, ui16Final );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), CPpuBus::StdWrite, _pvParm0, ui16Final );
			}
		}


	protected :
		// == Members.
		/** The PGM banks. */
		uint8_t											m_ui8PgmBanks[32];
		/** The CHR ROM banks. */
		uint8_t											m_ui8ChrBanks[32];
		/** If the ROM CHR size is 0, it uses CHR RAM instead. */
		uint8_t											m_ui8DefaultChrRam[8*1024];
		/** The ROM used to initialize this mapper. */
		LSN_ROM *										m_prRom;
		/** The CPU, for reading information such as cycle counts and for sending IRQ's. */
		CCpuBase *										m_pcbCpu;
		/** The interruptable object. */
		CInterruptable *								m_pInterruptable = nullptr;
		/** The PPU pointer, for accessing the PPU address bus. */
		CBussable *										m_pbPpuBus;
		/** The offset of the fixed bank. */
		size_t											m_stFixedOffset;
		/** The PGM bank. */
		uint8_t &										m_ui8PgmBank;
		/** The CHR ROM bank. */
		uint8_t &										m_ui8ChrBank;
		/** The mirroring mode. */
		LSN_MIRROR_MODE									m_mmMirror = LSN_MM_VERTICAL;


		// == Functions.
		/**
		 * Applies a controllable mirroring map.
		 *
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		void											ApplyControllableMirrorMap( CPpuBus * _pbPpuBus ) {
			for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_MEM_FULL_SIZE; ++I ) {
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
					_pbPpuBus->SetWriteFunc( uint16_t( I ), CPpuBus::NoWrite, this, ui16MappedAddr | LSN_PPU_PATTERN_TABLES );
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
			_ui8Ret = pmBase->m_prRom->vPrgRom.data()[_ui16Parm1];
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
			_ui8Ret = pmBase->m_prRom->vPrgRom.data()[_ui16Parm1];
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBanks[_uReg] to select a bank among _uSize-sized banks.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 * \tparam _uReg The register index.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uReg, unsigned _uSize>
		static void LSN_FASTCALL						PgmBankRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8PgmBanks[_uReg])*_uSize)];
		}

		/**
		 * Reads from CHR ROM using m_ui8PgmBanks[_uReg] to select a bank among _uSize-sized banks.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 * \tparam _uReg The register index.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uReg, unsigned _uSize>
		static void LSN_FASTCALL						ChrBankRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8ChrBanks[_uReg])*_uSize)];
		}

		/**
		 * Gets a bank by index.
		 *
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \param _sMemSize The size of the memory.
		 * \param _sBankSize The bank size.
		 * \return Returns the normalized bank index.
		 */
		static inline uint8_t							GetBank( int16_t _i16Bank, size_t _sMemSize, size_t _sBankSize ) {
			size_t stBanks = _sMemSize / _sBankSize;
			if ( stBanks ) {
				size_t stIdx;
				if ( _i16Bank < 0 ) {
					stIdx = stBanks - (-_i16Bank % stBanks);
				}
				else {
					stIdx = _i16Bank % stBanks;
				}
				return uint8_t( stIdx );
			}
			else {
				return 0;
			}
		}

		/**
		 * Gets a PGM bank by index.
		 *
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uSize The bank size.
		 * \return Returns the normalized bank index.
		 */
		template <unsigned _uSize>
		uint8_t											GetPgmBank( int16_t _i16Bank ) {
			size_t stBanks = m_prRom->vPrgRom.size() / _uSize;
			if ( stBanks ) {
				size_t stIdx;
				if ( _i16Bank < 0 ) {
					stIdx = stBanks - (-_i16Bank % stBanks);
				}
				else {
					stIdx = _i16Bank % stBanks;
				}
				return uint8_t( stIdx );
			}
			else {
				return 0;
			}
		}

		/**
		 * Sets a PGM bank by index.
		 *
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uReg The register index.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uReg, unsigned _uSize>
		void											SetPgmBank( int16_t _i16Bank ) {
			m_ui8PgmBanks[_uReg] = GetPgmBank<_uSize>( _i16Bank );
		}

		/**
		 * Sets a PGM bank by index.
		 *
		 * \param _ui16Reg The register index.
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uSize>
		void											SetPgmBank( uint16_t _ui16Reg, int16_t _i16Bank ) {
			m_ui8PgmBanks[_ui16Reg] = GetPgmBank<_uSize>( _i16Bank );
		}

		/**
		 * Sets a PGM bank by index.
		 *
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uReg The register index.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uReg, unsigned _uSize>
		void											SetPgmBank2x( int16_t _i16Bank ) {
			SetPgmBank<_uReg, _uSize>( _i16Bank );
			SetPgmBank<_uReg+1, _uSize>( _i16Bank + 1 );
		}

		/**
		 * Sets 2 PGM banks by index.
		 *
		 * \param _ui16Reg The register index.
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uSize>
		void											SetPgmBank2x( uint16_t _ui16Reg, int16_t _i16Bank ) {
			SetPgmBank<_uSize>( _ui16Reg, _i16Bank );
			SetPgmBank<_uSize>( _ui16Reg + 1, _i16Bank + 1 );
		}

		/**
		 * Gets a CHR bank by index.
		 *
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uSize The bank size.
		 * \return Returns the normalized bank index.
		 */
		template <unsigned _uSize>
		uint8_t											GetChrBank( int16_t _i16Bank ) {
			size_t stBanks = m_prRom->vChrRom.size() / _uSize;
			if ( stBanks ) {
				size_t stIdx;
				if ( _i16Bank < 0 ) {
					stIdx = stBanks - (-_i16Bank % stBanks);
				}
				else {
					stIdx = _i16Bank % stBanks;
				}
				return uint8_t( stIdx );
			}
			else {
				return 0;
			}
		}

		/**
		 * Sets a CHR bank by index.
		 *
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uReg The register index.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uReg, unsigned _uSize>
		void											SetChrBank( int16_t _i16Bank ) {
			m_ui8ChrBanks[_uReg] = GetChrBank<_uSize>( _i16Bank );
		}

		/**
		 * Sets a CHR bank by index.
		 *
		 * \param _ui16Reg The register index.
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uSize>
		void											SetChrBank( uint16_t _ui16Reg, int16_t _i16Bank ) {
			m_ui8ChrBanks[_ui16Reg] = GetChrBank<_uSize>( _i16Bank );
		}

		/**
		 * Sets a CHR bank by index.
		 *
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uReg The register index.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uReg, unsigned _uSize>
		void											SetChrBank2x( int16_t _i16Bank ) {
			SetChrBank<_uReg, _uSize>( _i16Bank );
			SetChrBank<_uReg+1, _uSize>( _i16Bank + 1 );
		}

		/**
		 * Sets a CHR bank by index.
		 *
		 * \param _ui16Reg The register index.
		 * \param _i16Bank The register value, which can be negative to set a bank from the end.
		 * \tparam _uSize The bank size.
		 */
		template <unsigned _uSize>
		void											SetChrBank2x( uint16_t _ui16Reg, int16_t _i16Bank ) {
			SetChrBank<_uSize>( _ui16Reg, _i16Bank );
			SetChrBank<_uSize>( _ui16Reg + 1, _i16Bank + 1 );
		}

		/**
		 * Sanitizes all bank registers without initializing their values.
		 * 
		 * \tparam _uPgmBankSize The PGM bank size.
		 * \tparam _uChrBankSize The CHR bank size.
		 */
		template <unsigned _uPgmBankSize, unsigned _uChrBankSize>
		void											SanitizeRegs() {
			if ( m_prRom ) {
				// Sanitize all register ranges.
				for ( auto I = LSN_ELEMENTS( m_ui8PgmBanks ); I--; ) {
					SetPgmBank<_uPgmBankSize>( uint16_t( I ), m_ui8PgmBanks[I] );
				}
				for ( auto I = LSN_ELEMENTS( m_ui8ChrBanks ); I--; ) {
					SetChrBank<_uChrBankSize>( uint16_t( I ), m_ui8ChrBanks[I] );
				}
			}
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_8000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			PgmBankRead<0, 0x8000>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_4000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			PgmBankRead<0, 0x4000>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
		}

		/**
		 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			PgmBankRead<0, 0x2000>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
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
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[_ui16Parm1+pmThis->m_stFixedOffset];
		}

		/**
		 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrBankRead_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			ChrBankRead<0, 0x2000>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
		}

		/**
		 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current 4-kilobyte bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrBankRead_1000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			ChrBankRead<0, 0x1000>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
		}

		/**
		 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current 2-kilobyte bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrBankRead_0800( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			ChrBankRead<0, 0x0800>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
		}

		/**
		 * Reads from the CHR RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						DefaultChrRamRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_ui8Ret = pmThis->m_ui8DefaultChrRam[_ui16Parm1];
		}

		/**
		 * Writes to the CHR RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						DefaultChrRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			pmThis->m_ui8DefaultChrRam[_ui16Parm1] = _ui8Val;
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
					// Mirror $2400 to $2000 and $2C00 to $2800 (and then $2800 to $2400).
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
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_ControllableMirror( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
			_pui8Data[MirrorAddress( _ui16Parm1, pmThis->m_mmMirror )] = _ui8Val;
		}

	};

}	// namespace lsn
