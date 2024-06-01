/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 9 GPU object.
 */


#pragma once

#ifdef LSN_DX9

#include "../../LSNLSpiroNes.h"
#include "../LSNGpuBase.h"
#include "Helpers/LSWHelpers.h"
#include "LSNDirectX9.h"

namespace lsn {

	/**
	 * Class CDirectX9Device
	 * \brief The DirectX 9 GPU object.
	 *
	 * Description: The DirectX 9 GPU object.
	 */
	class CDirectX9Device : public CGpuBase {
	public :
		CDirectX9Device();
		~CDirectX9Device();

	protected :
		// == Members.
		lsw::LSW_HMODULE								m_hLib;									/**< The Direct3D DLL. */


	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
