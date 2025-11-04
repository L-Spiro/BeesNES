/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A NES palette.  Always has 512 entrie.  64-color inputs are copied to fill 512 enries.
 */
 
 #include "LSNNesPalette.h"
 #include "../File/LSNStdFile.h"


namespace lsn {

	CNesPalette::CNesPalette() {
	}
	CNesPalette::~CNesPalette() {
	}

	// == Functions.
	/**
	 * Loads a palette from a given file.
	 * 
	 * \param _pPath The path to the palette file to load (.PAL or .FPAL).
	 * \return Returns true if the file was opened and loaded.
	 **/
	bool CNesPalette::LoadFromFile( const std::filesystem::path &_pPath ) {
		lsn::CStdFile sfFile;
		auto u16sTmp = _pPath.generic_u16string();
		if ( sfFile.Open( u16sTmp.c_str() ) ) {
			bool bTheyMightBeGiantFloats = false;
			bool bTheyMightNeedBeSrgb = false;
			std::u16string s16Tmp = CUtilities::ToLower( _pPath.filename().generic_u16string() );

			if ( CUtilities::ToLower( CUtilities::GetFileExtension( u16sTmp ) ) == u"fpal" || s16Tmp.find( u"float", 0 ) < s16Tmp.size() ) {
				bTheyMightBeGiantFloats = true;
			}

			if ( s16Tmp.find( u"applysrgb", 0 ) < s16Tmp.size() ) {
				bTheyMightNeedBeSrgb = true;
			}
			std::vector<uint8_t> vPal;
			if ( sfFile.LoadToMemory( vPal ) ) {
				SetPalette( vPal, bTheyMightBeGiantFloats, bTheyMightNeedBeSrgb, _pPath );
				return m_p64Palette.size() == 512;
			}
		}
		return false;
	}

	/**
	 * Sends a given palette to the console.
	 *
	 * \param _vPalette The loaded palette file.
	 * \param _bIsProbablyFloat When the size alone is not enough to determine the palette type, this is used to make the decision.
	 * \param _bApplySrgb If true, an sRGB curve is applied to the loaded palette.
	 * \param _pPath The path to the palette file to load (.PAL or .FPAL).
	 */
	void CNesPalette::SetPalette( const std::vector<uint8_t> &_vPalette, bool _bIsProbablyFloat, bool _bApplySrgb, const std::filesystem::path &_pPath ) {
		m_p64Palette.clear();
		PaletteF64 vTmp;
		if ( _bIsProbablyFloat && (1 << 6) * 3 * sizeof( double ) == _vPalette.size() ) {
			vTmp = Load32_64bitPalette_64_512<double>( _vPalette );
		}
		else {
			switch ( _vPalette.size() ) {
				case (1 << 6) * 3 * sizeof( uint8_t ) : { /* Drop through! */ }		LSN_FALLTHROUGH
				case (1 << 9) * 3 * sizeof( uint8_t ) : {
					vTmp = Load8bitPalette_64_512( _vPalette );
					break;
				}
				case (1 << 6) * 3 * sizeof( float ) : { /* Drop through! */ }		LSN_FALLTHROUGH
				case (1 << 9) * 3 * sizeof( float ) : {
					vTmp = Load32_64bitPalette_64_512<float>( _vPalette );
					break;
				}
				case (1 << 9) * 3 * sizeof( double ) : {
					vTmp = Load32_64bitPalette_64_512<double>( _vPalette );
					break;
				}
			}
		}
		if ( !vTmp.size() ) { return; }
		m_p64Palette = std::move( vTmp );
		m_pPath = _pPath;
		m_bApplySrgb = _bApplySrgb;
	}

	/**
	 * Applies modifications to a single palette entry.
	 * 
	 * \param _fEntry The entry to modify.
	 * \param _gCrtGamma The CRT gamma curve to apply.
	 * \param _gMonitorGamma The monitor/display gamma curve to apply.
	 * \return Returns the modified entry.
	 **/
	CNesPalette::Float64_3 CNesPalette::UpdateEntry( const Float64_3 &_fEntry, LSN_GAMMA _gCrtGamma, LSN_GAMMA _gMonitorGamma ) {
		Float64_3 fDst;
		switch ( _gCrtGamma ) {
			case LSN_G_NONE : {
				fDst = _fEntry;
				break;
			}
			case LSN_G_AUTO : {
				if ( m_bApplySrgb ) {
					fDst.x[0] = CUtilities::sRGBtoLinear_Precise( _fEntry.x[0] );
					fDst.x[1] = CUtilities::sRGBtoLinear_Precise( _fEntry.x[1] );
					fDst.x[2] = CUtilities::sRGBtoLinear_Precise( _fEntry.x[2] );
				}
				else { fDst = _fEntry; }
				break;
			}
			case LSN_G_CRT1 : {
				fDst.x[0] = CUtilities::CrtProperToLinear( _fEntry.x[0], 1.0 );
				fDst.x[1] = CUtilities::CrtProperToLinear( _fEntry.x[1], 1.0, 0.0181 * 0.5 );
				fDst.x[2] = CUtilities::CrtProperToLinear( _fEntry.x[2], 1.0 );
				break;
			}
			case LSN_G_CRT2 : {
				fDst.x[0] = CUtilities::CrtProper2ToLinear( _fEntry.x[0] );
				fDst.x[1] = CUtilities::CrtProper2ToLinear( _fEntry.x[1] );
				fDst.x[2] = CUtilities::CrtProper2ToLinear( _fEntry.x[2] );
				break;
			}
			case LSN_G_sRGB : {
				fDst.x[0] = CUtilities::sRGBtoLinear_Precise( _fEntry.x[0] );
				fDst.x[1] = CUtilities::sRGBtoLinear_Precise( _fEntry.x[1] );
				fDst.x[2] = CUtilities::sRGBtoLinear_Precise( _fEntry.x[2] );
				break;
			}
			case LSN_G_SMPTE170M : {
				fDst.x[0] = CUtilities::SMPTE170MtoLinear_Precise( _fEntry.x[0] );
				fDst.x[1] = CUtilities::SMPTE170MtoLinear_Precise( _fEntry.x[1] );
				fDst.x[2] = CUtilities::SMPTE170MtoLinear_Precise( _fEntry.x[2] );
				break;
			}
			case LSN_G_DCIP3 : {
				fDst.x[0] = CUtilities::DCIP3toLinear( _fEntry.x[0] );
				fDst.x[1] = CUtilities::DCIP3toLinear( _fEntry.x[1] );
				fDst.x[2] = CUtilities::DCIP3toLinear( _fEntry.x[2] );
				break;
			}
			case LSN_G_ADOBERGB : {
				fDst.x[0] = CUtilities::AdobeRGBtoLinear( _fEntry.x[0] );
				fDst.x[1] = CUtilities::AdobeRGBtoLinear( _fEntry.x[1] );
				fDst.x[2] = CUtilities::AdobeRGBtoLinear( _fEntry.x[2] );
				break;
			}
			case LSN_G_SMPTE240M : {
				fDst.x[0] = CUtilities::SMPTE240MtoLinear_Precise( _fEntry.x[0] );
				fDst.x[1] = CUtilities::SMPTE240MtoLinear_Precise( _fEntry.x[1] );
				fDst.x[2] = CUtilities::SMPTE240MtoLinear_Precise( _fEntry.x[2] );
				break;
			}
			case LSN_G_POW_1_96 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 1.96 );
				fDst.x[1] = std::pow( _fEntry.x[1], 1.96 );
				fDst.x[2] = std::pow( _fEntry.x[2], 1.96 );
				break;
			}
			case LSN_G_POW_2_0 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 2.0 );
				fDst.x[1] = std::pow( _fEntry.x[1], 2.0 );
				fDst.x[2] = std::pow( _fEntry.x[2], 2.0 );
				break;
			}
			case LSN_G_POW_2_2 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 2.22222222222222232090871330001391470432281494140625 );
				fDst.x[1] = std::pow( _fEntry.x[1], 2.22222222222222232090871330001391470432281494140625 );
				fDst.x[2] = std::pow( _fEntry.x[2], 2.22222222222222232090871330001391470432281494140625 );
				break;
			}
			case LSN_G_POW_2_35 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 2.35 );
				fDst.x[1] = std::pow( _fEntry.x[1], 2.35 );
				fDst.x[2] = std::pow( _fEntry.x[2], 2.35 );
				break;
			}
			case LSN_G_POW_2_4 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 2.4 );
				fDst.x[1] = std::pow( _fEntry.x[1], 2.4 );
				fDst.x[2] = std::pow( _fEntry.x[2], 2.4 );
				break;
			}
			case LSN_G_POW_2_5 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 2.5 );
				fDst.x[1] = std::pow( _fEntry.x[1], 2.5 );
				fDst.x[2] = std::pow( _fEntry.x[2], 2.5 );
				break;
			}
			case LSN_G_POW_2_7 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 2.7 );
				fDst.x[1] = std::pow( _fEntry.x[1], 2.7 );
				fDst.x[2] = std::pow( _fEntry.x[2], 2.7 );
				break;
			}
			case LSN_G_POW_2_8 : {
				fDst.x[0] = std::pow( _fEntry.x[0], 2.8 );
				fDst.x[1] = std::pow( _fEntry.x[1], 2.8 );
				fDst.x[2] = std::pow( _fEntry.x[2], 2.8 );
				break;
			}
		}

		switch ( _gMonitorGamma ) {
			case LSN_G_NONE : { break; }
			case LSN_G_AUTO : {
				/*if ( m_bApplySrgb ) {
					fDst.x[0] = CUtilities::LinearTosRGB_Precise( fDst.x[0] );
					fDst.x[1] = CUtilities::LinearTosRGB_Precise( fDst.x[1] );
					fDst.x[2] = CUtilities::LinearTosRGB_Precise( fDst.x[2] );
				}*/
				break;
			}
			case LSN_G_CRT1 : {
				fDst.x[0] = CUtilities::LinearToCrtProper( fDst.x[0], 1.0 );
				fDst.x[1] = CUtilities::LinearToCrtProper( fDst.x[1], 1.0, 0.0181 * 0.5 );
				fDst.x[2] = CUtilities::LinearToCrtProper( fDst.x[2], 1.0 );
				break;
			}
			case LSN_G_CRT2 : {
				fDst.x[0] = CUtilities::LinearToCrtProper2( fDst.x[0] );
				fDst.x[1] = CUtilities::LinearToCrtProper2( fDst.x[1] );
				fDst.x[2] = CUtilities::LinearToCrtProper2( fDst.x[2] );
				break;
			}
			case LSN_G_sRGB : {
				fDst.x[0] = CUtilities::LinearTosRGB_Precise( fDst.x[0] );
				fDst.x[1] = CUtilities::LinearTosRGB_Precise( fDst.x[1] );
				fDst.x[2] = CUtilities::LinearTosRGB_Precise( fDst.x[2] );
				break;
			}
			case LSN_G_SMPTE170M : {
				fDst.x[0] = CUtilities::LinearToSMPTE170M_Precise( fDst.x[0] );
				fDst.x[1] = CUtilities::LinearToSMPTE170M_Precise( fDst.x[1] );
				fDst.x[2] = CUtilities::LinearToSMPTE170M_Precise( fDst.x[2] );
				break;
			}
			case LSN_G_DCIP3 : {
				fDst.x[0] = CUtilities::LinearToDCIP3( fDst.x[0] );
				fDst.x[1] = CUtilities::LinearToDCIP3( fDst.x[1] );
				fDst.x[2] = CUtilities::LinearToDCIP3( fDst.x[2] );
				break;
			}
			case LSN_G_ADOBERGB : {
				fDst.x[0] = CUtilities::LinearToAdobeRGB( fDst.x[0] );
				fDst.x[1] = CUtilities::LinearToAdobeRGB( fDst.x[1] );
				fDst.x[2] = CUtilities::LinearToAdobeRGB( fDst.x[2] );
				break;
			}
			case LSN_G_SMPTE240M : {
				fDst.x[0] = CUtilities::LinearToSMPTE240M_Precise( fDst.x[0] );
				fDst.x[1] = CUtilities::LinearToSMPTE240M_Precise( fDst.x[1] );
				fDst.x[2] = CUtilities::LinearToSMPTE240M_Precise( fDst.x[2] );
				break;
			}
			case LSN_G_POW_1_96 : {
				fDst.x[0] = std::pow( fDst.x[0], 1.0 / 1.96 );
				fDst.x[1] = std::pow( fDst.x[1], 1.0 / 1.96 );
				fDst.x[2] = std::pow( fDst.x[2], 1.0 / 1.96 );
				break;
			}
			case LSN_G_POW_2_0 : {
				fDst.x[0] = std::pow( fDst.x[0], 1.0 / 2.0 );
				fDst.x[1] = std::pow( fDst.x[1], 1.0 / 2.0 );
				fDst.x[2] = std::pow( fDst.x[2], 1.0 / 2.0 );
				break;
			}
			case LSN_G_POW_2_2 : {
				fDst.x[0] = std::pow( fDst.x[0], 0.45 );
				fDst.x[1] = std::pow( fDst.x[1], 0.45 );
				fDst.x[2] = std::pow( fDst.x[2], 0.45 );
				break;
			}
			case LSN_G_POW_2_35 : {
				fDst.x[0] = std::pow( fDst.x[0], 1.0 / 2.35 );
				fDst.x[1] = std::pow( fDst.x[1], 1.0 / 2.35 );
				fDst.x[2] = std::pow( fDst.x[2], 1.0 / 2.35 );
				break;
			}
			case LSN_G_POW_2_4 : {
				fDst.x[0] = std::pow( fDst.x[0], 1.0 / 2.4 );
				fDst.x[1] = std::pow( fDst.x[1], 1.0 / 2.4 );
				fDst.x[2] = std::pow( fDst.x[2], 1.0 / 2.4 );
				break;
			}
			case LSN_G_POW_2_5 : {
				fDst.x[0] = std::pow( fDst.x[0], 1.0 / 2.5 );
				fDst.x[1] = std::pow( fDst.x[1], 1.0 / 2.5 );
				fDst.x[2] = std::pow( fDst.x[2], 1.0 / 2.5 );
				break;
			}
			case LSN_G_POW_2_7 : {
				fDst.x[0] = std::pow( fDst.x[0], 1.0 / 2.7 );
				fDst.x[1] = std::pow( fDst.x[1], 1.0 / 2.7 );
				fDst.x[2] = std::pow( fDst.x[2], 1.0 / 2.7 );
				break;
			}
			case LSN_G_POW_2_8 : {
				fDst.x[0] = std::pow( fDst.x[0], 1.0 / 2.8 );
				fDst.x[1] = std::pow( fDst.x[1], 1.0 / 2.8 );
				fDst.x[2] = std::pow( fDst.x[2], 1.0 / 2.8 );
				break;
			}
		}

		return fDst;
	}

	/**
	 * Fills the software palette with the loaded palette.
	 * 
	 * \param _ppSoftPal The software emulator palette to fill.
	 * \param _gCrtGamma The CRT gamma curve to apply.
	 * \param _gMonitorGamma The monitor/display gamma curve to apply.
	 * \return Returns true if the internal palette is not empty.
	 **/
	bool CNesPalette::FillSoftwarePalette( lsn::LSN_PALETTE * _ppSoftPal, LSN_GAMMA _gCrtGamma, LSN_GAMMA _gMonitorGamma ) {
		if LSN_UNLIKELY( m_p64Palette.empty() ) { return false; }
		for ( size_t I = 0; I < 512; ++I ) {
			Float64_3 fSrc = UpdateEntry( m_p64Palette[I%m_p64Palette.size()], _gCrtGamma, _gMonitorGamma );

			_ppSoftPal->uVals[I].sRgb.ui8R = uint8_t( std::round( fSrc.x[2] * 255.0 ) );
			_ppSoftPal->uVals[I].sRgb.ui8G = uint8_t( std::round( fSrc.x[1] * 255.0 ) );
			_ppSoftPal->uVals[I].sRgb.ui8B = uint8_t( std::round( fSrc.x[0] * 255.0 ) );
		}
		return true;
	}

	/**
	 * Converts from a 64-bit palette to a 64-bit palette.
	 * 
	 * \param _gCrtGamma The CRT gamma curve to apply.
	 * \param _gMonitorGamma The monitor/display gamma curve to apply.
	 * \return Returns the converted palette.  If the size of the palette is not 512, there was an allocation error.
	 **/
	std::vector<CNesPalette::Float64_4> CNesPalette::PaletteToF64( LSN_GAMMA _gCrtGamma, LSN_GAMMA _gMonitorGamma ) {
		std::vector<CNesPalette::Float64_4> vRet;
		if ( m_p64Palette.size() ) {
			try {
				vRet.resize( 512 );
				for ( size_t I = 0; I < 512; ++I ) {
					auto fSrc = UpdateEntry( m_p64Palette[I%m_p64Palette.size()], _gCrtGamma, _gMonitorGamma );

					vRet[I].x[0] = fSrc.x[0];
					vRet[I].x[1] = fSrc.x[1];
					vRet[I].x[2] = fSrc.x[2];
					vRet[I].x[3] = 1.0;
				}
			}
			catch ( ... ) {
				return std::vector<CNesPalette::Float64_4>();
			}
		}
		return vRet;
	}

	/**
	 * Converts from a 64-bit palette to a 32-bit palette.
	 * 
	 * \param _gCrtGamma The CRT gamma curve to apply.
	 * \param _gMonitorGamma The monitor/display gamma curve to apply.
	 * \return Returns the converted palette.  If the size of the palette is not 512, there was an allocation error.
	 **/
	std::vector<CNesPalette::Float32_4>  CNesPalette::PaletteToF32( LSN_GAMMA _gCrtGamma, LSN_GAMMA _gMonitorGamma ) {
		std::vector<CNesPalette::Float32_4> vRet;
		if ( m_p64Palette.size() ) {
			try {
				vRet.resize( 512 );
				for ( size_t I = 0; I < 512; ++I ) {
					const Float64_3 fSrc = UpdateEntry( m_p64Palette[I%m_p64Palette.size()], _gCrtGamma, _gMonitorGamma );

					vRet[I].x[0] = float( fSrc.x[0] );
					vRet[I].x[1] = float( fSrc.x[1] );
					vRet[I].x[2] = float( fSrc.x[2] );
					vRet[I].x[3] = 1.0f;
				}
			}
			catch ( ... ) {
				return std::vector<CNesPalette::Float32_4>();
			}
		}
		return vRet;
	}

	/**
	 * Converts from a 64-bit palette to a 16-bit palette.
	 * 
	 * \param _gCrtGamma The CRT gamma curve to apply.
	 * \param _gMonitorGamma The monitor/display gamma curve to apply.
	 * \return Returns the converted palette.  If the size of the palette is not 512, there was an allocation error.
	 **/
	std::vector<CNesPalette::Float16_4> CNesPalette::PaletteToF16( LSN_GAMMA _gCrtGamma, LSN_GAMMA _gMonitorGamma ) {
		std::vector<CNesPalette::Float16_4> vRet;
		if ( m_p64Palette.size() ) {
			try {
				vRet.resize( 512 );
				for ( size_t I = 0; I < 512; ++I ) {
					const Float64_3 fSrc = UpdateEntry( m_p64Palette[I%m_p64Palette.size()], _gCrtGamma, _gMonitorGamma );

					vRet[I].x[0] = fSrc.x[0];
					vRet[I].x[1] = fSrc.x[1];
					vRet[I].x[2] = fSrc.x[2];
					vRet[I].x[3] = 1.0f;
				}
			}
			catch ( ... ) {
				return std::vector<CNesPalette::Float16_4>();
			}
		}
		return vRet;
	}

	/**
	 * Loads a 64- or 512- entry 8-bit palette into a 512-entry floating-point palette.
	 * 
	 * \param _vPalFile The loaded palette file raw bytes.
	 * \return Returns a vector of 512 RGB 64-floats representing the colors of a palette.
	 **/
	CNesPalette::PaletteF64 CNesPalette::Load8bitPalette_64_512( const std::vector<uint8_t> &_vPalFile ) {
		PaletteF64 vRet;
		try {
			vRet.resize( 512 );
			if ( !_vPalFile.size() ) { return vRet; }
			for ( size_t I = 0; I < 512; ++I ) {
				const uint8_t * pui8ThisSrc = reinterpret_cast<const uint8_t *>(&_vPalFile[(I*sizeof(uint8_t)*3)%_vPalFile.size()]);
				vRet[I].x[0] = pui8ThisSrc[0] / 255.0;
				vRet[I].x[1] = pui8ThisSrc[1] / 255.0;
				vRet[I].x[2] = pui8ThisSrc[2] / 255.0;
			}
		}
		catch ( ... ) {
			return PaletteF64();
		}
		return vRet;
	}

}	// namespace lsn
