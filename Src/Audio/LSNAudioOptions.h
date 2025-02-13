/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Uses OpenAL or Core Audio to handle all for our audio needs.  As an emulator, we just need a single device, context, and source.
 *	This class owns and manages the objects and provides the interface we need to generate audio.
 */


#pragma once

#include <cstdint>

namespace lsn {

	/** Format types. */
	enum LSN_SAMPLE_FORMAT {
		LSN_SF_PCM,										/**< Signed integer formats. */
		LSN_SF_FLOAT									/**< A floating-point format. */
	};


	// == Types.
	/** The audio format. */
	struct LSN_AUDIO_FORMAT {
		/** The sample format. */
		LSN_SAMPLE_FORMAT								sfFormat = LSN_SF_PCM;
		/** The number of channels. */
		uint16_t										ui16Channels = 1;
		/** The number of bits in a sample. */
		uint16_t										ui16BitsPerChannel = 16;


		// == Operators.
		/**
		 * Inequality operator.
		 * 
		 * \param _fFormat The format against which to compare.
		 * \return Returns true if the given format does not equal this format.
		 **/
		bool											operator != ( const LSN_AUDIO_FORMAT &_fFormat ) const {
			return sfFormat != _fFormat.sfFormat ||
				ui16Channels != _fFormat.ui16Channels ||
				ui16BitsPerChannel != _fFormat.ui16BitsPerChannel;
		}
	};

	/** Audio-related options. */
	struct LSN_AUDIO_OPTIONS {
		uint32_t										ui32OutputHz = 44100;
		LSN_AUDIO_FORMAT								afFormat;
		
	};

}	// namespace lsn
