/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: All of the image formats, along with conversion routines and functions for working with the formats (encoding, decoding, etc.)
 */

#include "LSNTextureAddressing.h"


namespace lsn {

	// == Members.
	/** Table of addressing functions, indexable via the LSN_TEXTURE_ADDRESSING enumeration. */
	CTextureAddressing::PfAddressingFunc CTextureAddressing::m_pfFuncs[LSN_TA_TOTAL] {
		CTextureAddressing::Wrap,
		CTextureAddressing::Mirror,
		CTextureAddressing::Clamp,
		CTextureAddressing::Border,
		CTextureAddressing::MirrorOnce,
		CTextureAddressing::NullBorder,
	};

}	// namespace lsn
