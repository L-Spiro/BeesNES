/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The NSF header.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNRomConstants.h"

namespace lsn {

	// == Enumerations.
	/**
	 * \brief The NSF TV system.
	 */
	enum LSN_NSF_TV_SYSTEM {
		LSN_NTS_NTSC						= 0,						/**< NTSC. */
		LSN_NTS_PAL							= 1,						/**< PAL. */
		LSN_NTS_DUAL						= 2,						/**< Dual NTSC/PAL. */

		LSN_NTS_UNKNOWN						= -1,						/**< Unspecified. */
	};

	/**
	 * Struct LSN_NSF_HEADER
	 * \brief The NSF header.
	 *
	 * Description: The standard 128-byte NSF header structure.
	 */
	struct LSN_NSF_HEADER {
		char								cNesmString[5];				/**< 0-4: Constant "NESM" followed by $1A. */
		uint8_t								ui8Version;					/**< 5: Version (usually 1). */
		uint8_t								ui8TotalTracks;				/**< 6: Total tracks (1-256). */
		uint8_t								ui8StartingTrack;			/**< 7: Starting track (1-256). */
		uint8_t								ui8LoadAddress[2];			/**< 8-9: Load address of data ($8000-FFFF) (little-endian). */
		uint8_t								ui8InitAddress[2];			/**< A-B: Init address of data ($8000-FFFF) (little-endian). */
		uint8_t								ui8PlayAddress[2];			/**< C-D: Play address of data ($8000-FFFF) (little-endian). */
		char								cTrackName[32];				/**< E-2D: Track name, null-terminated. */
		char								cArtistName[32];			/**< 2E-4D: Artist name, null-terminated. */
		char								cCopyrightName[32];			/**< 4E-6D: Copyright holder, null-terminated. */
		uint8_t								ui8NtscSpeed[2];			/**< 6E-6F: Play speed, NTSC (1/1000000th sec per tick, usually 16639 or 16666) (little-endian). */
		uint8_t								ui8BankValues[8];			/**< 70-77: Bankswitch init values for $8000-$FFFF. */
		uint8_t								ui8PalSpeed[2];				/**< 78-79: Play speed, PAL (usually 19997) (little-endian). */
		uint8_t								ui8TvSystem;				/**< 7A: PAL/NTSC bits (0 = NTSC, 1 = PAL, 2 = Dual). */
		uint8_t								ui8SoundChip;				/**< 7B: Extra Sound Chip Support. */
		uint8_t								ui8Reserved[4];				/**< 7C-7F: Reserved. */


		// == Functions.
		/**
		 * Gets the format version.
		 *
		 * \return Returns the NSF format version.
		 */
		inline uint8_t						GetVersion() const;

		/**
		 * Gets the total number of tracks.
		 *
		 * \return Returns the total number of tracks.
		 */
		inline uint8_t						GetTotalTracks() const;

		/**
		 * Gets the starting track.
		 *
		 * \return Returns the starting track index (1-based).
		 */
		inline uint8_t						GetStartingTrack() const;

		/**
		 * Gets the load address.
		 *
		 * \return Returns the 16-bit load address.
		 */
		inline uint16_t						GetLoadAddress() const;

		/**
		 * Gets the init address.
		 *
		 * \return Returns the 16-bit init address.
		 */
		inline uint16_t						GetInitAddress() const;

		/**
		 * Gets the play address.
		 *
		 * \return Returns the 16-bit play address.
		 */
		inline uint16_t						GetPlayAddress() const;

		/**
		 * Gets the track name.
		 *
		 * \return Returns a constant pointer to the null-terminated track name.
		 */
		inline const char *					GetTrackName() const;

		/**
		 * Gets the artist name.
		 *
		 * \return Returns a constant pointer to the null-terminated artist name.
		 */
		inline const char *					GetArtistName() const;

		/**
		 * Gets the copyright name.
		 *
		 * \return Returns a constant pointer to the null-terminated copyright name.
		 */
		inline const char *					GetCopyrightName() const;

		/**
		 * Gets the NTSC play speed.
		 *
		 * \return Returns the NTSC play speed in 1/1,000,000th of a second per tick.
		 */
		inline uint16_t						GetNtscSpeed() const;

		/**
		 * Gets the PAL play speed.
		 *
		 * \return Returns the PAL play speed in 1/1,000,000th of a second per tick.
		 */
		inline uint16_t						GetPalSpeed() const;

		/**
		 * Gets the initial bank value for a given index.
		 *
		 * \param _ui8Index The bank value index (0-7).
		 * \return Returns the initial bank value at the given index.
		 */
		inline uint8_t						GetBankValue( uint8_t _ui8Index ) const;

		/**
		 * Determines if the NSF uses bank switching.
		 *
		 * \return Returns true if bank switching is utilized, false otherwise.
		 */
		inline bool							UsesBankSwitching() const;

		/**
		 * Gets the TV system.
		 *
		 * \return Returns the TV system (NTSC, PAL, or Dual).
		 */
		inline LSN_NSF_TV_SYSTEM			GetTvSystem() const;

		/**
		 * Determines if VRC6 audio is supported.
		 *
		 * \return Returns true if VRC6 audio is supported.
		 */
		inline bool							HasVrc6() const;

		/**
		 * Determines if VRC7 audio is supported.
		 *
		 * \return Returns true if VRC7 audio is supported.
		 */
		inline bool							HasVrc7() const;

		/**
		 * Determines if FDS audio is supported.
		 *
		 * \return Returns true if FDS audio is supported.
		 */
		inline bool							HasFds() const;

		/**
		 * Determines if MMC5 audio is supported.
		 *
		 * \return Returns true if MMC5 audio is supported.
		 */
		inline bool							HasMmc5() const;

		/**
		 * Determines if Namco 163 audio is supported.
		 *
		 * \return Returns true if Namco 163 audio is supported.
		 */
		inline bool							HasNamco163() const;

		/**
		 * Determines if Sunsoft 5B audio is supported.
		 *
		 * \return Returns true if Sunsoft 5B audio is supported.
		 */
		inline bool							HasSunsoft5B() const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the format version.
	 *
	 * \return Returns the NSF format version.
	 */
	inline uint8_t LSN_NSF_HEADER::GetVersion() const {
		return ui8Version;
	}

	/**
	 * Gets the total number of tracks.
	 *
	 * \return Returns the total number of tracks.
	 */
	inline uint8_t LSN_NSF_HEADER::GetTotalTracks() const {
		return ui8TotalTracks;
	}

	/**
	 * Gets the starting track.
	 *
	 * \return Returns the starting track index (1-based).
	 */
	inline uint8_t LSN_NSF_HEADER::GetStartingTrack() const {
		return ui8StartingTrack;
	}

	/**
	 * Gets the load address.
	 *
	 * \return Returns the 16-bit load address.
	 */
	inline uint16_t LSN_NSF_HEADER::GetLoadAddress() const {
		return ui8LoadAddress[0] | (ui8LoadAddress[1] << 8);
	}

	/**
	 * Gets the init address.
	 *
	 * \return Returns the 16-bit init address.
	 */
	inline uint16_t LSN_NSF_HEADER::GetInitAddress() const {
		return ui8InitAddress[0] | (ui8InitAddress[1] << 8);
	}

	/**
	 * Gets the play address.
	 *
	 * \return Returns the 16-bit play address.
	 */
	inline uint16_t LSN_NSF_HEADER::GetPlayAddress() const {
		return ui8PlayAddress[0] | (ui8PlayAddress[1] << 8);
	}

	/**
	 * Gets the track name.
	 *
	 * \return Returns a constant pointer to the null-terminated track name.
	 */
	inline const char * LSN_NSF_HEADER::GetTrackName() const {
		return cTrackName;
	}

	/**
	 * Gets the artist name.
	 *
	 * \return Returns a constant pointer to the null-terminated artist name.
	 */
	inline const char * LSN_NSF_HEADER::GetArtistName() const {
		return cArtistName;
	}

	/**
	 * Gets the copyright name.
	 *
	 * \return Returns a constant pointer to the null-terminated copyright name.
	 */
	inline const char * LSN_NSF_HEADER::GetCopyrightName() const {
		return cCopyrightName;
	}

	/**
	 * Gets the NTSC play speed.
	 *
	 * \return Returns the NTSC play speed in 1/1,000,000th of a second per tick.
	 */
	inline uint16_t LSN_NSF_HEADER::GetNtscSpeed() const {
		return ui8NtscSpeed[0] | (ui8NtscSpeed[1] << 8);
	}

	/**
	 * Gets the PAL play speed.
	 *
	 * \return Returns the PAL play speed in 1/1,000,000th of a second per tick.
	 */
	inline uint16_t LSN_NSF_HEADER::GetPalSpeed() const {
		return ui8PalSpeed[0] | (ui8PalSpeed[1] << 8);
	}

	/**
	 * Gets the initial bank value for a given index.
	 *
	 * \param _ui8Index The bank value index (0-7).
	 * \return Returns the initial bank value at the given index.
	 */
	inline uint8_t LSN_NSF_HEADER::GetBankValue( uint8_t _ui8Index ) const {
		if ( _ui8Index < 8 ) {
			return ui8BankValues[_ui8Index];
		}
		return 0;
	}

	/**
	 * Determines if the NSF uses bank switching.
	 *
	 * \return Returns true if bank switching is utilized, false otherwise.
	 */
	inline bool LSN_NSF_HEADER::UsesBankSwitching() const {
		for ( uint8_t I = 0; I < 8; ++I ) {
			if ( ui8BankValues[I] != 0 ) { return true; }
		}
		return false;
	}

	/**
	 * Gets the TV system.
	 *
	 * \return Returns the TV system (NTSC, PAL, or Dual).
	 */
	inline LSN_NSF_TV_SYSTEM LSN_NSF_HEADER::GetTvSystem() const {
		switch ( ui8TvSystem & 0x03 ) {
			case 0 : { return LSN_NSF_TV_SYSTEM::LSN_NTS_NTSC; }
			case 1 : { return LSN_NSF_TV_SYSTEM::LSN_NTS_PAL; }
			case 2 :
			case 3 : { return LSN_NSF_TV_SYSTEM::LSN_NTS_DUAL; }
		}
		return LSN_NSF_TV_SYSTEM::LSN_NTS_UNKNOWN;
	}

	/**
	 * Determines if VRC6 audio is supported.
	 *
	 * \return Returns true if VRC6 audio is supported.
	 */
	inline bool LSN_NSF_HEADER::HasVrc6() const {
		return (ui8SoundChip & 0x01) == 0x01;
	}

	/**
	 * Determines if VRC7 audio is supported.
	 *
	 * \return Returns true if VRC7 audio is supported.
	 */
	inline bool LSN_NSF_HEADER::HasVrc7() const {
		return (ui8SoundChip & 0x02) == 0x02;
	}

	/**
	 * Determines if FDS audio is supported.
	 *
	 * \return Returns true if FDS audio is supported.
	 */
	inline bool LSN_NSF_HEADER::HasFds() const {
		return (ui8SoundChip & 0x04) == 0x04;
	}

	/**
	 * Determines if MMC5 audio is supported.
	 *
	 * \return Returns true if MMC5 audio is supported.
	 */
	inline bool LSN_NSF_HEADER::HasMmc5() const {
		return (ui8SoundChip & 0x08) == 0x08;
	}

	/**
	 * Determines if Namco 163 audio is supported.
	 *
	 * \return Returns true if Namco 163 audio is supported.
	 */
	inline bool LSN_NSF_HEADER::HasNamco163() const {
		return (ui8SoundChip & 0x10) == 0x10;
	}

	/**
	 * Determines if Sunsoft 5B audio is supported.
	 *
	 * \return Returns true if Sunsoft 5B audio is supported.
	 */
	inline bool LSN_NSF_HEADER::HasSunsoft5B() const {
		return (ui8SoundChip & 0x20) == 0x20;
	}

}	// namespace lsn
