/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The 2A0X series of APU's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../System/LSNTickable.h"

namespace lsn {

	/**
	 * Class CApu2A0X
	 * \brief The 2A0X series of APU's.
	 *
	 * Description: The 2A0X series of APU's.
	 */
	class CApu2A0X : public CTickable {
	public :
		CApu2A0X();
		~CApu2A0X();


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void						Tick() {}


	protected :
		// == Members.
	};

}	// namespace lsn
