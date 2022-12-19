/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for hardware that can perform DMA transfers.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CDmaSource
	 * \brief The base class for hardware that can perform DMA transfers.
	 *
	 * Description: The base class for hardware that can perform DMA transfers.
	 */
	class CDmaSource {
	public :
		// == Functions.
		/**
		 * Sets the target DMA address.
		 * 
		 * \param _ppuBus The PPU bus for writing DMA.
		 */
		virtual void						SetDmaTarget( CPpuBus * /*_ppuBus*/ ) {}
	};

}	// namespace lsn
