/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the primary interface to the underlying graphics API.
 */


#include "LSNGpuBase.h"

namespace lsn {

	CGpuBase::CGpuBase() {}
	CGpuBase::~CGpuBase() {}

	// == Functions.
	/**
	 * Creates an API device.
	 *
	 * \param _hWnd The window to which to attach.
	 * \param _sAdapter The adapter to use.
	 * \return Returns true if the device was created.
	 **/
	bool CGpuBase::Create( HWND /*_hWnd*/, const std::string &/*_sAdapter*/ ) { return false; }

}	// namespace lsn
