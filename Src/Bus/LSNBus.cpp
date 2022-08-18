/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The glue between each component of the system. The bus allows components to talk to
 *	each other and keeps track of floating values for the emulation of an open bus.
 * All memory accesses that would go across a real NES bus go across this. This means components
 *	with internal RAM can still manage their own RAM internally however they please.
 *
 * Ideally we would organize things so that components could "connect" to it and reserve address
 *	ranges for themselves etc., really keeping things nice and modular, but emulating real cycles
 *	will be taxing enough that anything else that can be optimized must be optimized, so this
 *	class hardcodes what addresses connect to what etc. and the logic behind the floating value.
 *
 * An outward-facing design decision is to have the entire block of system RAM contiguous in memory
 *	here to make it easier to parse by external readers (IE an external debugger).
 *
 * Since the memory is contiguous and directly part of this class, allocating this on the stack
 *	may cause a stack overflow.
 */


#include "LSNBus.h"
#include <cstring>


namespace lsn {

	// == Various constructors.
	CBus::CBus() :
		m_ui8LastRead( 0 ) {
	}
	CBus::~CBus() {
		ResetToKnown();
	}

	// == Functions.
	/**
	 * Resets the bus to a known state.
	 */
	void CBus::ResetToKnown() {
		std::memset( m_ui8Ram, 0, sizeof( m_ui8Ram ) );
		m_ui8LastRead = 0;
	}

	/**
	 * Performs an "analog" reset, allowing previous data to remain.
	 */
	void CBus::ResetAnalog() {
	}

}	// namespace lsn
