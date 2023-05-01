/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU sequencer.  Counts a timer down to determine the timing of an event.
 */


#include "LSNSequencer.h"


namespace lsn {

	CSequencer::CSequencer() :
		m_ui64SeqOff( 0 ) {
	}
	CSequencer::~CSequencer() {
	}

}	// namespace lsn
