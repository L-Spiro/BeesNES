/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The 2C0X series of PPU's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../System/LSNTickable.h"

namespace lsn {

	/**
	 * Class CPpu2C0X
	 * \brief The 2C0X series of PPU's.
	 *
	 * Description: The 2C0X series of PPU's.
	 */
	class CPpu2C0X : public CTickable {
	public :
		CPpu2C0X();
		~CPpu2C0X();


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void						Tick() {}
	};

}	// namespace lsn
