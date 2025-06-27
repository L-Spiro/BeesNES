/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the APU.
 */


#pragma once

#include "../LSNLSpiroNes.h"


namespace lsn {

	/**
	 * Class CApuBase
	 * \brief The base class for the APU.
	 *
	 * Description: The base class for the APU.
	 */
	class CApuBase {
	public :
		CApuBase() {
		}
		virtual ~CApuBase() {
		}


		// == Functions.
		/**
		 * Sets the extension-audio sample.
		 * 
		 * \param _fSample The sample geneerated by extension audio to be mixed with the standard APU output.
		 **/
		virtual void							SetExtSample( float _fSample ) {
			m_fExtSample = _fSample;
		}


	protected :
		// == Members.
		float									m_fExtSample = 0.0f;							/**< Extension-audio sample. */
	};

}	// namespace lsn
