/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The 2A0X series of APU's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../System/LSNTickable.h"

namespace lsn {

	/** APU timings. */
	enum LSN_APU_TIMINGS {
		LSN_AT_NTSC_MODE_0_STEP_0						= uint32_t( 3728.5 * 2.0 ),
		LSN_AT_NTSC_MODE_0_STEP_1						= uint32_t( 7456.5 * 2.0 ),
		LSN_AT_NTSC_MODE_0_STEP_2						= uint32_t( 11185.5 * 2.0 ),
		LSN_AT_NTSC_MODE_0_STEP_3_0						= uint32_t( 14914 * 2.0 ),
		LSN_AT_NTSC_MODE_0_STEP_3_1						= uint32_t( 14914.5 * 2.0 ),
		LSN_AT_NTSC_MODE_0_STEP_3_2						= uint32_t( 14915 * 2.0 ),

		LSN_AT_PAL_MODE_0_STEP_0						= uint32_t( 4156.5 * 2.0 ),
		LSN_AT_PAL_MODE_0_STEP_1						= uint32_t( 8313.5 * 2.0 ),
		LSN_AT_PAL_MODE_0_STEP_2						= uint32_t( 12469.5 * 2.0 ),
		LSN_AT_PAL_MODE_0_STEP_3_0						= uint32_t( 16626 * 2.0 ),
		LSN_AT_PAL_MODE_0_STEP_3_1						= uint32_t( 16626.5 * 2.0 ),
		LSN_AT_PAL_MODE_0_STEP_3_2						= uint32_t( 16627 * 2.0 ),


		LSN_AT_NTSC_MODE_1_STEP_0						= uint32_t( 3728.5 * 2.0 ),
		LSN_AT_NTSC_MODE_1_STEP_1						= uint32_t( 7456.5 * 2.0 ),
		LSN_AT_NTSC_MODE_1_STEP_2						= uint32_t( 11185.5 * 2.0 ),
		LSN_AT_NTSC_MODE_1_STEP_3						= uint32_t( 14914.5 * 2.0 ),
		LSN_AT_NTSC_MODE_1_STEP_4_0						= uint32_t( 18640.5 * 2.0 ),
		LSN_AT_NTSC_MODE_1_STEP_4_1						= uint32_t( 18641 * 2.0 ),

		LSN_AT_PAL_MODE_1_STEP_0						= uint32_t( 4156.5 * 2.0 ),
		LSN_AT_PAL_MODE_1_STEP_1						= uint32_t( 8313.5 * 2.0 ),
		LSN_AT_PAL_MODE_1_STEP_2						= uint32_t( 12469.5 * 2.0 ),
		LSN_AT_PAL_MODE_1_STEP_3						= uint32_t( 16626.5 * 2.0 ),
		LSN_AT_PAL_MODE_1_STEP_4_0						= uint32_t( 20782.5 * 2.0 ),
		LSN_AT_PAL_MODE_1_STEP_4_1						= uint32_t( 20783 * 2.0 ),
	};

	/**
	 * Class CApu2A0X
	 * \brief The 2A0X series of APU's.
	 *
	 * Description: The 2A0X series of APU's.
	 */
	template <unsigned _tM0S0, unsigned _tM0S1, unsigned _tM0S2, unsigned _tM0S4_0, unsigned _tM0S4_1, unsigned _tM0S4_2,
		unsigned _tM1S0, unsigned _tM1S1, unsigned _tM1S2, unsigned _tM1S4, unsigned _tM1S5_0, unsigned _tM1S5_1>
	class CApu2A0X : public CTickable {
	public :
		CApu2A0X( CCpuBus * _pbBus ) :
			m_pbBus( _pbBus ) {
		}
		~CApu2A0X() {
		}


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void									Tick() {
		}

		/**
		 * Applies the APU's memory mapping t the bus.
		 */
		void											ApplyMemoryMap() {
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


	protected :
		// == Types.
		/** A function pointer for the tick handlers. */
		typedef void (CApu2A0X:: *						PfTicks)();


		// == Members.
		/** The main bus. */
		CCpuBus *										m_pbBus;
		/** The current cycle function. */
		PfTicks											m_pftTick;


		// == Functions.
		/** Mode-0 step-0 tick function. */
		void											Tick_Mode0_Step0_Even() {
		}
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
#define LSN_APU_TYPE( REGION )							LSN_AT_ ## REGION ## _MODE_0_STEP_0, LSN_AT_ ## REGION ## _MODE_0_STEP_1, LSN_AT_ ## REGION ## _MODE_0_STEP_2,																				\
														LSN_AT_ ## REGION ## _MODE_0_STEP_3_0, LSN_AT_ ## REGION ## _MODE_0_STEP_3_1, LSN_AT_ ## REGION ## _MODE_0_STEP_3_2,																		\
														LSN_AT_ ## REGION ## _MODE_1_STEP_0, LSN_AT_ ## REGION ## _MODE_1_STEP_1, LSN_AT_ ## REGION ## _MODE_1_STEP_2, LSN_AT_ ## REGION ## _MODE_1_STEP_3,											\
														LSN_AT_ ## REGION ## _MODE_1_STEP_4_0, LSN_AT_ ## REGION ## _MODE_1_STEP_4_1

	/**
	 * An NTSC PPU.
	 */
	typedef CApu2A0X<LSN_APU_TYPE( NTSC )>													CNtscApu;

	/**
	 * A PAL PPU.
	 */
	typedef CApu2A0X<LSN_APU_TYPE( PAL )>													CPalApu;

#undef LSN_APU_TYPE

}	// namespace lsn
