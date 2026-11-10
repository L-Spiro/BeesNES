#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The fields (settings) shown by the Video Options dialog:  what each one is, how it is shown, and how it is read from and
 *	written to the options.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Options/LSNOptions.h"

#include <ComboBox/LSWComboBox.h>

#include <string>

using namespace lsw;

namespace lsn {

	/**
	 * Class CVideoOptionsFields
	 * \brief The fields (settings) shown by the Video Options dialog.
	 *
	 * Description: The fields (settings) shown by the Video Options dialog:  what each one is, how it is shown, and how it is read from and
	 *	written to the options.
	 */
	class CVideoOptionsFields {
	public :
		// == Enumerations.
		/** The kinds of pages. */
		enum LSN_VIDEO_PAGE_KIND : uint32_t {
			LSN_VPK_HOST,																						/**< The Host Machine page. */
			LSN_VPK_PALETTE,																					/**< The Palette filter. */
			LSN_VPK_BLARGG,																						/**< Blargg's NTSC filter. */
			LSN_VPK_LSPIRO_NTSC,																				/**< The L. Spiro NTSC filters. */
			LSN_VPK_LSPIRO_PAL,																					/**< The L. Spiro PAL filters. */
			LSN_VPK_LMP_NTSC,																					/**< The LMP88959 NTSC filters. */
			LSN_VPK_LMP_PAL,																					/**< The LMP88959 PAL filters. */
			LSN_VPK_TOTAL,																						/**< The number of page kinds. */
		};

		/** The types of controls. */
		enum LSN_VIDEO_FIELD_TYPE : uint32_t {
			LSN_VFT_COMBO,																						/**< A drop-down list; the value is the item data of the selection. */
			LSN_VFT_FLOAT,																						/**< An edit holding a real number. */
			LSN_VFT_INT,																						/**< An edit holding an integer. */
			LSN_VFT_CHECK,																						/**< A check box (0 or 1). */
			LSN_VFT_DISPLAY,																					/**< A palette display (shows the palette; holds no value). */
		};

		/** The fields.  The control ID's of a field are LSN_VOWI_FIELD_FIRST + (field * 2) (label) and + 1 (control). */
		enum LSN_VIDEO_FIELD : uint32_t {
			// Host machine.
			LSN_VF_MONITOR_COLOR_SPACE,																			/**< The monitor's color space. */
			LSN_VF_MONITOR_GAMMA,																				/**< The monitor's gamma curve. */
			LSN_VF_WHITE_ADAPTATION,																			/**< White-point adaptation. */
			LSN_VF_GAMUT,																						/**< Out-of-gamut handling. */
			LSN_VF_OUTPUT_RANGE,																				/**< The output range. */

			// Common:  color correction.
			LSN_VF_TV_PRESET,																					/**< The TV preset (fills other fields; holds no value of its own). */
			LSN_VF_COLOR_SPACE,																					/**< The CRT's color space. */
			LSN_VF_DEMODULATOR,																					/**< The CRT's decoder chip (L. Spiro filters only). */
			LSN_VF_GAMMA,																						/**< The CRT's gamma curve. */
			LSN_VF_CRT_WHITE,																					/**< The white level of the CRT1 curve. */
			LSN_VF_CRT_BRIGHTNESS,																				/**< The brightness of the CRT1 curve. */
			// Common:  phosphor decay.
			LSN_VF_PHOSPHOR_ENABLED,																			/**< Phosphor decay on/off. */
			LSN_VF_PHOSPHOR_LEVEL,																				/**< The phosphor decay base level. */
			LSN_VF_PHOSPHOR_TIME,																				/**< The phosphor decay time. */
			LSN_VF_PHOSPHOR_RED,																				/**< The red phosphor multiplier (percent). */
			LSN_VF_PHOSPHOR_BLUE,																				/**< The blue phosphor multiplier (percent). */
			// Common:  internal scaling.
			LSN_VF_SCALE_X,																						/**< The horizontal internal scaling. */
			LSN_VF_SCALE_Y,																						/**< The vertical internal scaling. */
			LSN_VF_SCALE_DYNAMIC,																				/**< Dynamic internal scaling. */
			// Common:  convolution.
			LSN_VF_CONV_ENABLED,																				/**< Convolution on/off. */
			LSN_VF_CONV_LOW,																					/**< The convolution filter below 2.5 times. */
			LSN_VF_CONV_MEDIUM,																					/**< The convolution filter below 3.5 times. */
			LSN_VF_CONV_HIGH,																					/**< The convolution filter below 4.5 times. */
			LSN_VF_CONV_MAX,																					/**< The convolution filter at 4.5 times or above. */

			// Palette.
			LSN_VF_PALETTE_LOAD,																				/**< The palette to load (holds the palette path). */
			LSN_VF_PALETTE_DISPLAY,																				/**< The palette display. */
			LSN_VF_PALETTE_SHOW_INDICES,																		/**< Show color indices in the palette display. */

			// Shared by the composite filters (always in this order).
			LSN_VF_HUE,																							/**< The hue (degrees). */
			LSN_VF_HUE_OFFSET,																					/**< The hue offset (degrees). */
			LSN_VF_BRIGHTNESS,																					/**< The brightness. */
			LSN_VF_CONTRAST,																					/**< The contrast. */
			LSN_VF_SATURATION,																					/**< The saturation. */
			LSN_VF_WHITE_LEVEL,																					/**< The white level. */
			LSN_VF_BLACK_LEVEL,																					/**< The black level. */

			// Blargg.
			LSN_VF_BLARGG_GAMMA,																				/**< Blargg's gamma (software only). */
			LSN_VF_BLARGG_SHARPNESS,																			/**< Blargg's sharpness. */
			LSN_VF_BLARGG_FRINGING,																				/**< Blargg's fringing. */
			LSN_VF_BLARGG_BLEED,																				/**< Blargg's bleed. */
			LSN_VF_BLARGG_ARTIFACTS,																			/**< Blargg's artifacts. */
			LSN_VF_BLARGG_MERGE_FIELDS,																			/**< Blargg's field merging. */

			// L. Spiro.
			LSN_VF_LSPIRO_BLEED,																				/**< The L. Spiro bleed. */
			LSN_VF_DEEP_PRESET,																					/**< The Deep Internals preset (fills other fields; holds no value of its own). */
			LSN_VF_CHROMA_FILTER,																				/**< The chrominance filter. */
			LSN_VF_CHROMA_PARM0,																				/**< The first chrominance filter parameter. */
			LSN_VF_CHROMA_PARM1,																				/**< The second chrominance filter parameter. */
			LSN_VF_CHROMA_KERNEL,																				/**< The chrominance kernel size. */
			LSN_VF_LUMA_FILTER,																					/**< The luminance filter. */
			LSN_VF_LUMA_PARM0,																					/**< The first luminance filter parameter. */
			LSN_VF_LUMA_PARM1,																					/**< The second luminance filter parameter. */
			LSN_VF_LUMA_KERNEL,																					/**< The luminance kernel size. */
			LSN_VF_VERTICAL_COMB,																				/**< The vertical comb filter (PAL only). */
			LSN_VF_NTSC_COMB,																					/**< The vertical comb filter (NTSC only). */
			LSN_VF_NOISE_TYPE,																					/**< The noise type. */
			LSN_VF_NOISE_AMOUNT,																				/**< The custom noise amount. */
			LSN_VF_SW_PHOSPHOR_LEVEL,																			/**< The software phosphor decay base level. */
			LSN_VF_SW_PHOSPHOR_TIME,																			/**< The software phosphor decay time. */

			// LMP88959.
			LSN_VF_LMP_NOISE,																					/**< The LMP88959 noise. */
			LSN_VF_LMP_BLEND,																					/**< The LMP88959 field blending. */
			LSN_VF_LMP_VSYNC,																					/**< The LMP88959 vertical sync (NTSC only). */
			LSN_VF_LMP_HSYNC,																					/**< The LMP88959 horizontal sync (NTSC only). */
			LSN_VF_LMP_CHROMA_CORRECTION,																		/**< The LMP88959 chroma correction (PAL only). */

			LSN_VF_TOTAL,																						/**< The number of fields. */
		};


		// == Types.
		/** Information about a field. */
		struct LSN_FIELD_INFO {
			LSN_VIDEO_FIELD_TYPE							vftType;										/**< The type of control. */
			const wchar_t *									pwcLabel;										/**< The label (or the check-box text). */
			const char *									pcToolTip;										/**< The tool tip (UTF-8). */
		};

		/** A group of fields. */
		struct LSN_FIELD_GROUP {
			const wchar_t *									pwcLabel;										/**< The label of the group box. */
			const LSN_VIDEO_FIELD *							pvfFields;										/**< The fields in the group. */
			size_t											sTotal;											/**< The number of fields in the group. */
			WORD											wDefaultsId = 0;								/**< The control ID of the group's own Restore Defaults button, or 0 for none. */
		};

		/** The settings a page edits (a region of global or per-game settings, plus the host-display settings). */
		struct LSN_FIELD_TARGET {
			LSN_FILTER_REGION_OPTIONS *						pfroFilters = nullptr;							/**< The filter settings of the region being edited. */
			LSN_PALETTE_OPTIONS *							ppoPalette = nullptr;							/**< The palette settings of the region being edited. */
			LSN_HOST_DISPLAY_OPTIONS *						phdoHost = nullptr;								/**< The host-display settings. */
			LSN_PPU_METRICS									pmRegion = LSN_PM_NTSC_M;						/**< The region being edited. */
			LSN_VIDEO_PAGE_KIND								vpkKind = LSN_VPK_HOST;							/**< The kind of page. */
		};

		/** A palette preset. */
		struct LSN_PALETTE_PRESET {
			const wchar_t *									pwcName;										/**< The name shown to the user. */
			const wchar_t *									pwcFile;										/**< The file name (in the default palette folder). */
			CNesPalette::LSN_GAMMA							gCrtGamma;										/**< The CRT gamma curve that goes with the palette. */
			float											fCrt1White;										/**< The CRT1 white level that goes with the palette. */
			float											fCrt1Brightness;								/**< The CRT1 brightness that goes with the palette. */
			CColorSpace::LSN_COLOR_SPACE					cspColorSpace;									/**< The color space that goes with the palette. */
			CColorSpace::LSN_COLOR_SPACE					cspNtscJColorSpace;								/**< The color space that goes with the palette in the NTSC-J region. */
		};


		// == Functions.
		/**
		 * Gets information about a field.
		 *
		 * \param _vfField The field.
		 * \return Returns information about the field.
		 */
		static const LSN_FIELD_INFO &						Info( LSN_VIDEO_FIELD _vfField );

		/**
		 * Gets the label of a field for the current settings.  The labels of the kernel parameters name what the selected filter function
		 *	does with them (or say that they are unused); the other labels are fixed (Info()).
		 *
		 * \param _vfField The field.
		 * \param _ftTarget The settings being edited.
		 * \return Returns the label of the field.
		 */
		static const wchar_t *								Label( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Gets the groups shown in the left column of a page (the common settings, or the display settings of the Host Machine page).
		 *
		 * \param _vpkKind The kind of page.
		 * \param _sTotal Holds the returned number of groups.
		 * \return Returns the groups.
		 */
		static const LSN_FIELD_GROUP *						LeftGroups( LSN_VIDEO_PAGE_KIND _vpkKind, size_t &_sTotal );

		/**
		 * Gets the groups shown in the right column of a page (the filter-specific settings).
		 *
		 * \param _vpkKind The kind of page.
		 * \param _sTotal Holds the returned number of groups.
		 * \return Returns the groups.
		 */
		static const LSN_FIELD_GROUP *						RightGroups( LSN_VIDEO_PAGE_KIND _vpkKind, size_t &_sTotal );

		/**
		 * Gets the regional presets of a page.
		 *
		 * \param _vpkKind The kind of page.
		 * \param _sTotal Holds the returned number of presets.
		 * \return Returns the regions that have presets for the given kind of page (empty for the Host Machine page).
		 */
		static const LSN_PPU_METRICS *						Presets( LSN_VIDEO_PAGE_KIND _vpkKind, size_t &_sTotal );

		/**
		 * Gets the name of a region as shown in the regional-preset combo box.
		 *
		 * \param _pmRegion The region.
		 * \return Returns the name of the region.
		 */
		static const wchar_t *								RegionName( LSN_PPU_METRICS _pmRegion );

		/**
		 * Gets the name of a filter tab.
		 *
		 * \param _vpkKind The kind of page.
		 * \return Returns the name of the tab.
		 */
		static const wchar_t *								TabName( LSN_VIDEO_PAGE_KIND _vpkKind );

		/**
		 * Gets the palette presets.
		 *
		 * \param _sTotal Holds the returned number of presets.
		 * \return Returns the palette presets.
		 */
		static const LSN_PALETTE_PRESET *					PalettePresets( size_t &_sTotal );

		/**
		 * Fills the combo box of a field with its entries.
		 *
		 * \param _vfField The field.
		 * \param _ftTarget The settings being edited (some lists depend on them).
		 * \param _pcbCombo The combo box to fill.
		 */
		static void											FillCombo( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget, CComboBox * _pcbCombo );

		/**
		 * Gets the value of a field.  Combo boxes use the item data of their entries, check boxes 0 or 1, and the rest the value shown.
		 *
		 * \param _vfField The field.
		 * \param _ftTarget The settings being edited.
		 * \param _dValue Holds the returned value.
		 * \return Returns true if the field has a value on the given kind of page.
		 */
		static bool											GetValue( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget, double &_dValue );

		/**
		 * Sets the value of a field.  Settings that exist in both a software and a GPU version are both set.
		 *
		 * \param _vfField The field.
		 * \param _ftTarget The settings being edited.
		 * \param _dValue The value to set.
		 * \return Returns true if the value was set.
		 */
		static bool											SetValue( LSN_VIDEO_FIELD _vfField, LSN_FIELD_TARGET &_ftTarget, double _dValue );

		/**
		 * Determines whether a field can currently be edited (for example, the CRT1 parameters only when the CRT1 curve is selected).
		 *
		 * \param _vfField The field.
		 * \param _ftTarget The settings being edited.
		 * \return Returns true if the field can currently be edited.
		 */
		static bool											IsEditable( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Applies a TV preset:  sets the color space, the hue offset, the saturation (relative to the default saturation of the filter), and
		 *	(L. Spiro filters) the decoder chip.
		 *
		 * \param _cspPreset The color space that models the television.
		 * \param _ftTarget The settings being edited.
		 */
		static void											ApplyTvPreset( CColorSpace::LSN_COLOR_SPACE _cspPreset, LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Applies a palette preset:  sets the palette path, the CRT gamma curve, and the CRT1 parameters.
		 *
		 * \param _sPreset The index of the palette preset.
		 * \param _ftTarget The settings being edited.
		 */
		static void											ApplyPalettePreset( size_t _sPreset, LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Applies the settings that go with a palette loaded from a file:  the sRGB gamma curve and the sRGB color space.
		 *
		 * \param _ftTarget The settings being edited.
		 */
		static void											ApplyCustomPalette( LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Restores the defaults of the settings shown on a page.
		 *
		 * \param _ftTarget The settings being edited.
		 */
		static void											RestoreDefaults( LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Restores the defaults of the convolution settings shown on a page.
		 *
		 * \param _ftTarget The settings being edited.
		 */
		static void											RestoreConvolutionDefaults( LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Gets the range of the values of a field that would not break its filter.  Values outside of the range are clamped.
		 *
		 * \param _vfField The field.
		 * \param _dMin Holds the returned lowest value (in the units shown by the dialog).
		 * \param _dMax Holds the returned highest value (in the units shown by the dialog).
		 * \return Returns true if the field has a limited range.
		 */
		static bool											Range( LSN_VIDEO_FIELD _vfField, double &_dMin, double &_dMax );

		/**
		 * Gets the GPU settings shown on a page.
		 *
		 * \param _ftTarget The settings being edited.
		 * \return Returns the GPU settings that the common settings of the page edit, or nullptr for the Host Machine page.
		 */
		static LSN_GPU_FILTER_OPTIONS *						GpuOptions( const LSN_FIELD_TARGET &_ftTarget );

		/**
		 * Formats a value for an edit control.
		 *
		 * \param _vfField The field.
		 * \param _dValue The value.
		 * \return Returns the text to show.
		 */
		static std::wstring									FormatValue( LSN_VIDEO_FIELD _vfField, double _dValue );
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
