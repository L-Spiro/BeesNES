/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Manages the DPCM state, rate decoding, and shift register.
 */

#include "LSNDmc.h"


namespace lsn {

	// == Members.
	const uint16_t CDmc::m_ui16Periods[LSN_PM_CONSOLE_TOTAL][16] = {						/**< Rate periods for each console type. */
		{ 428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54 },	// LSN_PM_NTSC
        { 398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118, 98, 78, 66, 50 },		// LSN_PM_PAL
		{ 428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54 },	// LSN_PM_DENDY
		{ 428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54 },	// LSN_PM_PALM
		{ 398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118, 98, 78, 66, 50 },		// LSN_PM_PALN
	};

	CDmc::CDmc() {
	}
	CDmc::~CDmc() {
	}

	// == Functions.
	/**
	 * Resets the unit to a known state.
	 **/
	void CDmc::ResetToKnown() {
		m_ui16SampleAddress = 0xC000;
        m_ui16SampleLength = 1;
        m_ui16CurrentAddress = 0;
        m_ui16BytesRemaining = 0;
        
        m_ui8OutputLevel = 0;
        m_ui8SampleBuffer = 0;
        m_ui8ShiftRegister = 0;
        m_ui8BitsRemaining = 8;

        m_ui16Timer = 0;
        m_ui16TimerPeriod = 0;

        m_bIrqEnabled = false;
        m_bLoop = false;
        m_bIrqAsserted = false;
        m_bBufferEmpty = true;
        m_bSilent = true;
	}


}	// namespace lsn
