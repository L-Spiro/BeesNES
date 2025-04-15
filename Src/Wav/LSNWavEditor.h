/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.  Helps to make OST's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNWavFile.h"

#include <atomic>
#include <map>
#include <string>
#include <vector>


namespace lsn {

	/**
	 * Class CWavEditor
	 * \brief Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.
	 *
	 * Description: Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.  Helps to make OST's.
	 */
	class CWavEditor {
	public :
		CWavEditor();
		~CWavEditor();


		// == Enumerations.
		/** Actual Hz types. */
		enum LSN_ACTUAL_HZ : int32_t {
			LSN_AH_NTSC													= int32_t( -LSN_PM_NTSC ),
			LSN_AH_PAL													= int32_t( -LSN_PM_PAL ),
			LSN_AH_DENDY												= int32_t( -LSN_PM_DENDY ),
			LSN_AH_PALM													= int32_t( -LSN_PM_PALM ),
			LSN_AH_PALN													= int32_t( -LSN_PM_PALN ),
			LSN_AH_BY_FILE												= int32_t( -100 ),
			LSN_AH_CUSTOM												= int32_t( -1000 ),
		};

		/** Mains-hum types. */
		enum LSN_MAINS_HUM {
			LSN_MH_XX_BLACK,
			LSN_MH_X_BLACK,
			LSN_MH_BLACK,
			LSN_MH_GREY_00,
			LSN_MH_GREY_10,
			LSN_MH_GREY_20EM,
			LSN_MH_WHITE,
			LSN_MH_RED,
			LSN_MH_GREEN,
			LSN_MH_BLUE,
		};

		/** White-noise types. */
		enum LSN_WHITE_NOISE {
			LSN_WN_GAUSSIAN,
			LSN_WN_UNIFORM,
		};

		/** Filter types. */
		enum LSN_FILTER {
			LSN_F_POLE,
			LSN_F_SINC,
		};

		/** Channel types. */
		enum LSN_CHANNELS {
			LSN_C_MONO													= 1,
			LSN_C_STEREO												= 2,
			LSN_C_SURROUND												= 3,
		};

		/** Volume type. */
		enum LSN_VOL_TYPE {
			LSN_VT_ABS,
			LSN_VT_NORM,
			LSN_VT_LOUDNESS,
		};


		// == Types.
		/** The output data. */
		struct LSN_OUTPUT {
			double														dAbsoluteVol = 5.0;								/**< Absolute volume. */
			double														dNormalizeTo = std::pow( 10, -5.5 / 20.0 );		/**< Normalize-to level. */
			double														dLoudness = std::pow( 10, -5.5 / 20.0 );		/**< Loudness level. */
			double														dMainsHumVol = 1.0;								/**< Mains Hum volume. */
			double														dWhiteNoiseVol = 1.0;							/**< White Noise volume. */
			double														dWhiteNoiseTemp = 290.0;						/**< White-noise temperature. */
			double														dWhiteNoiseRes = 50.0;							/**< White-noise resistance. */
			std::wstring												wsFolder;										/**< Path to the output folder. */
			uint32_t													ui32Hz = 48000;									/**< The final output Hz. */
			int32_t														i32MainsHumType = LSN_MH_BLACK;					/**< The color of mains hum. */
			int32_t														i32WhiteNoiseType = LSN_WN_GAUSSIAN;			/**< The type of white noise. */

			int32_t														i32VolType = LSN_VT_ABS;						/**< Volume type. */

			int32_t														i32Format = CWavFile::LSN_F_PCM;				/**< Output format. */
			uint16_t													ui16Bits = 32;									/**< PCM bits. */
			int32_t														i32Channels = LSN_C_MONO;						/**< Channel format. */

			bool														bMainsHum = false;								/**< Add Mains Hum? */
			bool														bWhiteNoise = false;							/**< Add White Noise? */

			bool														bDither = true;									/**< Dither 16-but output? */
			bool														bNumbered = false;								/**< Number the output files? */
		};

		/** A single WAV file. */
		struct LSN_WAV_FILE {
			std::wstring												wsPath;											/**< Path to the file. */
			CWavFile::LSN_FMT_CHUNK										fcFormat;										/**< The WAV-file format. */
			uint64_t													ui64Samples = 0;								/**< Total samples per channel. */
		};

		/** A WAV file. */
		struct LSN_WAV_FILE_SET {
			LSN_WAV_FILE												wfPath;											/**< Path to the file. */
			std::vector<LSN_WAV_FILE>									vExtensions;									/**< Additional files to append to the main file. */
			std::wstring												wsMetaPath;										/**< Path to the file's metadata. */
		};


		// == Functions.
		/**
		 * Sets the parameters for conversion.
		 * 
		 * \param _oOutput The output parameters.
		 * \param PARM DESC
		 * \return DESC
		 **/
		bool															SetParms( const LSN_OUTPUT &_oOutput ) {
			m_oOutput = _oOutput;
			return true;
		}

		/**
		 * Gets a pointer to a WAV file given an ID.
		 * 
		 * \param _ui32Id The ID of the WAV-file structure to find.
		 * \return Returns a pointer to the associated WAV-file structure or nullptr.
		 **/
		LSN_WAV_FILE *													WavById( uint32_t _ui32Id ) {
			auto aFound = m_mFileMapping.find( _ui32Id );
			if ( aFound == m_mFileMapping.end() ) { return nullptr; }
			return &aFound->second;
		}


	protected :
		// == Members.
		/** The output state. */
		LSN_OUTPUT														m_oOutput;
		/** The map of ID's to files. */
		std::map<uint32_t, LSN_WAV_FILE>								m_mFileMapping;

		/** Path ID's. */
		static std::atomic<uint32_t>									m_aPathId;
	};

}	// namespace lsn
