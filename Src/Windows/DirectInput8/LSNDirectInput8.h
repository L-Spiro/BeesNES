#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A wrapper around DirectInput8.
 */

#pragma once

//#include <dinput.h>

namespace lsn {

	/**
	 * Class CDirectInput8
	 * \brief A wrapper around DirectInput8.
	 *
	 * Description: A wrapper around DirectInput8.
	 */
	class CDirectInput8 {
	public :
		CDirectInput8();
		~CDirectInput8();

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
