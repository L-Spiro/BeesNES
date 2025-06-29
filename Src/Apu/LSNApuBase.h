/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the APU.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Mappers/LSNMapperBase.h"


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
		 * Sets the mapper to use for extension audio.
		 * 
		 * \param _pmbMapper The pointer to the current mapper or nullptr.
		 **/
		void									SetMapper( CMapperBase * _pmbMapper ) {
			m_pmbMapper = _pmbMapper;
		}


	protected :
		// == Members.
		CMapperBase *							m_pmbMapper = nullptr;
	};

}	// namespace lsn
