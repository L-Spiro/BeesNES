/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for hardware that updates (ticks) at a constant rate (CPU, PPU, and APU).
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <vector>

namespace lsn {

	/**
	 * Class CTickable
	 * \brief The base class for hardware that updates (ticks) at a constant rate (CPU, PPU, and APU).
	 *
	 * Description: The base class for hardware that updates (ticks) at a constant rate (CPU, PPU, and APU).
	 */
	class CTickable {
	public :
		// == Types.
		/** A function pointer to the tick function.  Calling Obj->Tick() each cycle will cause the virtual
		 *	function to be found via look-up table, wasting instructions.  Instead, we can store a pointer
		 *	to the correct function and call it directly via a single dereference rather than via dereference
		 *	-> offset -> dereference. */
		typedef void (CTickable:: *			PfTickFunc)();


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void						Tick() {}
	};

}	// namespace lsn
