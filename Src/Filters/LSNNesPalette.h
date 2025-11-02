/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A NES palette.  Always has 512 entrie.  64-color inputs are copied to fill 512 enries.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Palette/LSNPalette.h"

#include <Float16/EEFloat16.h>

#include <filesystem>
#include <vector>


namespace lsn {

	/**
	 * Class CNesPalette
	 * \brief A NES palette.
	 *
	 * Description: A NES palette.  Always has 512 entrie.  64-color inputs are copied to fill 512 enries.
	 */
	class CNesPalette {
	public :
		CNesPalette();
		~CNesPalette();


		// == Enumerations.
		/** The CRT/gamma to apply when copying the palette. */
		enum LSN_GAMMA {
			LSN_G_NONE,								/**< Apply no gamma; use as-is. */
			LSN_G_AUTO,								/**< Uses the sRGB flag set during load.  If not set, nothing is done, otherwise an sRGB curve is applied. */
			LSN_G_CRT1,								/**< CUtilities::CrtProperToLinear()/CUtilities::LinearToCrtProper(). */
			LSN_G_CRT2,								/**< CUtilities::CrtProper2ToLinear()/CUtilities::LinearToCrtProper2(). */
			LSN_G_sRGB,								/**< CUtilities::sRGBtoLinear_Precise()/CUtilities::LinearTosRGB_Precise(). */
			LSN_G_SMPTE170M,						/**< CUtilities::SMPTE170MtoLinear_Precise()/CUtilities::LinearToSMPTE170M_Precise(). */
			LSN_G_DCIP3,							/**< CUtilities::DCIP3toLinear()/CUtilities::LinearToDCIP3(). */
			LSN_G_ADOBERGB,							/**< CUtilities::AdobeRGBtoLinear()/CUtilities::LinearToAdobeRGB(). */
			LSN_G_SMPTE240M,						/**< CUtilities::SMPTE240MtoLinear_Precise()/CUtilities::LinearToSMPTE240M_Precise(). */
			LSN_G_POW_1_96,							/**< std::pow( X, 1.96 )/std::pow( X, 1.0 / 1.96 ). */
			LSN_G_POW_2_0,							/**< std::pow( X, 2.0 )/std::pow( X, 1.0 / 2.0 ). */
			LSN_G_POW_2_2,							/**< std::pow( X, 2.22222222222222232090871330001391470432281494140625 )/std::pow( X, 1.0 / 2.22222222222222232090871330001391470432281494140625 ). */
			LSN_G_POW_2_35,							/**< std::pow( X, 2.35 )/std::pow( X, 1.0 / 2.35 ). */
			LSN_G_POW_2_4,							/**< std::pow( X, 2.4 )/std::pow( X, 1.0 / 2.4 ). */
			LSN_G_POW_2_5,							/**< std::pow( X, 2.5 )/std::pow( X, 1.0 / 2.5 ). */
			LSN_G_POW_2_7,							/**< std::pow( X, 2.7 )/std::pow( X, 1.0 / 2.7 ). */
			LSN_G_POW_2_8,							/**< std::pow( X, 2.8 )/std::pow( X, 1.0 / 2.8 ). */
		};


		// == Types.
		typedef struct { double x[3]; }				Float64_3;
		typedef struct { float x[3]; }				Float32_3;
		typedef struct { ee::CFloat16 x[3]; }		Float16_3;
		typedef std::vector<Float64_3>				PaletteF64;
		typedef std::vector<Float32_3>				PaletteF32;
		typedef std::vector<Float16_3>				PaletteF16;


		// == Functions.
		/**
		 * Loads a palette from a given file.
		 * 
		 * \param _pPath The path to the palette file to load (.PAL or .FPAL).
		 * \return Returns true if the file was opened and loaded.
		 **/
		bool										LoadFromFile( const std::filesystem::path &_pPath );

		/**
		 * Sends a given palette to the console.
		 *
		 * \param _vPalette The loaded palette file.
		 * \param _bIsProbablyFloat When the size alone is not enough to determine the palette type, this is used to make the decision.
		 * \param _bApplySrgb If true, an sRGB curve is applied to the loaded palette.
		 * \param _pPath The path to the palette file to load (.PAL or .FPAL).
		 */
		void										SetPalette( const std::vector<uint8_t> &_vPalette, bool _bIsProbablyFloat, bool _bApplySrgb, const std::filesystem::path &_pPath );

		/**
		 * Gets the flag for whether the file contained a suggestion to convert to sRGB.
		 * 
		 * \return Return the flag for whether the file contained a suggestion to convert to sRGB.
		 **/
		bool										sRgbFlag() const { return m_bApplySrgb; }

		/**
		 * Gets a constant reference to the loaded palette.
		 * 
		 * \return Returns a constant reference to the loaded palette.
		 **/
		const PaletteF64 &							Palette() const { return m_p64Palette; }

		/**
		 * Fills the software palette with the loaded palette.
		 * 
		 * \param _ppSoftPal The software emulator palette to fill.
		 * \param _gCrtGamma The CRT gamma curve to apply.
		 * \param _gMonitorGamma The monitor/display gamma curve to apply.
		 * \return Returns true if the internal palette is not empty.
		 **/
		bool										FillSoftwarePalette( lsn::LSN_PALETTE * _ppSoftPal, LSN_GAMMA _gCrtGamma, LSN_GAMMA _gMonitorGamma );

		/**
		 * Loads a 64- or 512- entry 8-bit palette into a 512-entry floating-point palette.
		 * 
		 * \param _vPalFile The loaded palette file raw bytes.
		 * \return Returns a vector of 512 RGB 64-floats representing the colors of a palette.
		 **/
		static PaletteF64							Load8bitPalette_64_512( const std::vector<uint8_t> &_vPalFile );

		/**
		 * Loads a 64- or 512- entry 32-bit float palette into a 512-entry floating-point palette.
		 * 
		 * \param _vPalFile The loaded palette file raw bytes.
		 * \return Returns a vector of 512 RGB 64-floats representing the colors of a palette.
		 **/
		template <typename _tSrcType>
		static PaletteF64							Load32_64bitPalette_64_512( const std::vector<uint8_t> &_vPalFile ) {
			PaletteF64 vRet;
			try {
				vRet.resize( 512 );
				if ( !_vPalFile.size() ) { return vRet; }
				for ( size_t I = 0; I < 512; ++I ) {
					const _tSrcType * ptThisSrc = reinterpret_cast<const _tSrcType *>(&_vPalFile[(I*sizeof(_tSrcType)*3)%_vPalFile.size()]);
					vRet[I].x[0] = static_cast<double>(ptThisSrc[0]);
					vRet[I].x[1] = static_cast<double>(ptThisSrc[1]);
					vRet[I].x[2] = static_cast<double>(ptThisSrc[2]);
				}
			}
			catch ( ... ) {
				return PaletteF64();
			}
			return vRet;
		}

		/**
		 * Converts from a 64-bit palette to a 32-bit palette.
		 * 
		 * \param _p64Src the paletter to convert.
		 * \return Returns the converted palette.  If the size of the palette is not 512, there was an allocation error.
		 **/
		static PaletteF32							PaletteToF32( const PaletteF64 &_p64Src );

		/**
		 * Converts from a 64-bit palette to a 16-bit palette.
		 * 
		 * \param _p64Src the paletter to convert.
		 * \return Returns the converted palette.  If the size of the palette is not 512, there was an allocation error.
		 **/
		static PaletteF16							PaletteToF16( const PaletteF64 &_p64Src );

	protected :
		// == Members.
		PaletteF64									m_p64Palette;							/**< The loaded palette. */
		std::filesystem::path						m_pPath;								/**< The path to the loaded palette. */
		bool										m_bApplySrgb = false;					/**< Apply sRGB curves? */
	};

}	// namespace lsn
