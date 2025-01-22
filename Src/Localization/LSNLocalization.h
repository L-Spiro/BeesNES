/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Localization.
 */

#pragma once

#ifdef LSN_EN_US
#include "LSNLocalization_en_us.h"
#endif	// #ifdef LSN_EN_US

#define LSN_STR_X( STR )				# STR
#define LSN_STR( STR )					LSN_STR_X( STR )
#define LSN_LSTR_X( STR )				L ## # STR
#define LSN_LSTR( STR )					LSN_LSTR_X( STR )
#define LSN_U8STR_X( STR )				u8 ## # STR
#define LSN_U8STR( STR )				LSN_U8STR_X( STR )
#define LSN_U16STR_X( STR )				u ## # STR
#define LSN_U16STR( STR )				LSN_U16STR_X( STR )
