/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The user-adjustable settings of the EMMIR (LMP88959) NTSC-CRT and PAL-CRT filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Struct LSN_LMP88959_SETTINGS
	 * \brief The user-adjustable settings of the EMMIR (LMP88959) NTSC-CRT and PAL-CRT filters.
	 *
	 * Description: The user-adjustable settings of the EMMIR (LMP88959) NTSC-CRT and PAL-CRT filters.  The same structure is used by both;
	 *	the fields that apply to only one of them are noted.  The defaults are those of the NTSC software filter; use the factory functions
	 *	for the defaults of each filter.
	 */
	struct LSN_LMP88959_SETTINGS {
		int32_t												i32Hue = 20;												/**< The hue, in degrees (NTSC: the monitor hue; PAL: the encoder hue). */
		int32_t												i32Brightness = 4;											/**< The monitor brightness. */
		int32_t												i32Contrast = 180;											/**< The monitor contrast. */
		int32_t												i32Saturation = 17;											/**< The monitor saturation. */
		int32_t												i32BlackPoint = 4;											/**< The black point. */
		int32_t												i32WhitePoint = 75;											/**< The white point. */
		int32_t												i32Noise = 8;												/**< The amount of signal noise. */
		bool												bBlend = true;												/**< If true, each new field is blended onto the previous image. */
		uint32_t											ui32BorderColor = 0x22;										/**< The border color (a PPU palette index; NTSC only). */
		bool												bVSync = true;												/**< If true, the monitor locks onto the vertical sync pulse (NTSC only; otherwise the picture rolls). */
		bool												bHSync = true;												/**< If true, the monitor locks onto the horizontal sync pulse (NTSC only; otherwise the picture tears sideways). */
		bool												bChromaCorrection = true;									/**< Hanover-bar correction (PAL only). */
		int32_t												i32YOffset = 7;												/**< The vertical offset of the image, in lines (PAL only). */


		// == Functions.
		/**
		 * Gets the defaults of the NTSC software (CPU) filter.
		 *
		 * \return Returns the defaults of the NTSC software filter.
		 */
		static inline LSN_LMP88959_SETTINGS					NtscCpuDefaults() { return LSN_LMP88959_SETTINGS(); }

		/**
		 * Gets the defaults of the NTSC GPU filters.
		 *
		 * \return Returns the defaults of the NTSC GPU filters.
		 */
		static inline LSN_LMP88959_SETTINGS					NtscGpuDefaults() {
			LSN_LMP88959_SETTINGS lsRet;
			lsRet.bBlend = false;
			return lsRet;
		}

		/**
		 * Gets the defaults of the PAL software (CPU) filter.
		 *
		 * \return Returns the defaults of the PAL software filter.
		 */
		static inline LSN_LMP88959_SETTINGS					PalCpuDefaults() {
			LSN_LMP88959_SETTINGS lsRet;
			lsRet.i32Hue = 0;
			return lsRet;
		}

		/**
		 * Gets the defaults of the PAL GPU filters.
		 *
		 * \return Returns the defaults of the PAL GPU filters.
		 */
		static inline LSN_LMP88959_SETTINGS					PalGpuDefaults() {
			LSN_LMP88959_SETTINGS lsRet = PalCpuDefaults();
			lsRet.bBlend = false;
			return lsRet;
		}
	};

}	// namespace lsn
