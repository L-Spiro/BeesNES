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

	/**
	 * Class CApu2A0X
	 * \brief The 2A0X series of APU's.
	 *
	 * Description: The 2A0X series of APU's.
	 */
	class CApu2A0X : public CTickable {
	public :
		CApu2A0X( CCpuBus * _pbBus );
		~CApu2A0X();


		// == Functions.
		/**
		 * Performs a single cycle update.
		 */
		virtual void						Tick();

		/**
		 * Applies the APU's memory mapping t the bus.
		 */
		void								ApplyMemoryMap();


	protected :
		// == Members.
		/** The main bus. */
		CCpuBus *							m_pbBus;
	};

}	// namespace lsn
