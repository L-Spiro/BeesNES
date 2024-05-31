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
	 * Class CDirectX9Gpu
	 * \brief The DirectX 9 GPU object.
	 *
	 * Description: The DirectX 9 GPU object.
	 */
	class CDirectX9Gpu : public CGpuBase {
	public :
		CDirectX9Gpu();
		~CDirectX9Gpu();

	protected :
		// == Members.
		lsw::LSW_HMODULE								m_hLib;									/**< The Direct3D DLL. */


	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
