/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The 2A0X series of APU's.
 */


#include "LSNApu2A0X.h"

namespace lsn {

	CApu2A0X::CApu2A0X( CCpuBus * _pbBus ) :
		m_pbBus( _pbBus ) {
	}
	CApu2A0X::~CApu2A0X() {
	}

	// == Functions.
	/**
	 * Performs a single cycle update.
	 */
	void CApu2A0X::Tick() {
	}

	/**
	 * Applies the APU's memory mapping t the bus.
	 */
	void CApu2A0X::ApplyMemoryMap() {
		// Apply the APU memory map to the bus.

		// None of the registers are readable except 0x4015.
		for ( uint32_t I = LSN_APU_START; I < LSN_APU_IO_START; ++I ) {
			if ( I != 0x4014 /*&& I != 0x4016*/	// Not registers.
				&& I != 0x4015 ) {			// Readable.
				m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
			}
		}
		// I/O registers normally disabled except in debug mode.
		for ( uint32_t I = LSN_APU_IO_START; I < (LSN_APU_IO_START + LSN_APU_IO); ++I ) {
			m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
		}
		// Unallocated space from 0x4020 to 0x40FF.
		for ( uint32_t I = (LSN_APU_IO_START + LSN_APU_IO); I <= (LSN_APU_START + 0xFF); ++I ) {
			m_pbBus->SetReadFunc( uint16_t( I ), CCpuBus::NoRead, this, uint16_t( I ) );
		}
	}

}	// namespace lsn
