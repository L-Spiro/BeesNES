/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the primary interface to the underlying graphics API.
 */


#pragma once

#include "../../LSNLSpiroNes.h"

namespace lsn {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// MACROS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define LSN_FILTER_POINT							1												/**< Point filter. */
#define LSN_FILTER_LINEAR							2												/**< Linear filter. */
#define	LSN_FILTER_TYPE_MASK						(0x3)											/**< Filter mask. */
#define	LSN_MIN_FILTER_SHIFT						(4)												/**< Min-filter shift. */
#define	LSN_MAG_FILTER_SHIFT						(2)												/**< Mag-fiter shift. */
#define	LSN_MIP_FILTER_SHIFT						(0)												/**< Min-filter shift. */
#define	LSN_COMPARISON_FILTERING_BIT				(0x80)											/**< Comparison bit. */
#define	LSN_ANISOTROPIC_FILTERING_BIT				(0x40)											/**< Anisotropic bit. */
#define LSN_ENC_BASIC_FILTER( MIN, MAG, MIP, COMPARISON )				\
	((COMPARISON) ? LSN_COMPARISON_FILTERING_BIT : 0) |					\
	(((MIN) & LSN_FILTER_TYPE_MASK) << LSN_MIN_FILTER_SHIFT) |			\
	(((MAG) & LSN_FILTER_TYPE_MASK) << LSN_MAG_FILTER_SHIFT) |			\
	(((MIP) & LSN_FILTER_TYPE_MASK) << LSN_MIP_FILTER_SHIFT)										/**< Encodes a basic (no anisotropy) filter. */
#define LSN_ENCODE_ANISOTROPIC_FILTER( COMPARISON )						\
	LSN_ANISOTROPIC_FILTERING_BIT |										\
	LSN_ENC_BASIC_FILTER( LSN_FILTER_LINEAR,							\
		LSN_FILTER_LINEAR,												\
		LSN_FILTER_LINEAR,												\
		COMPARISON )																				/**< Encodes an anisotropic filter. */
#define LSN_DECODE_MIN_FILTER( FILTER )									\
	(((FILTER) >> LSN_MIN_FILTER_SHIFT) & LSN_FILTER_TYPE_MASK)
#define LSN_DECODE_MAG_FILTER( FILTER )									\
	(((FILTER) >> LSN_MAG_FILTER_SHIFT) & LSN_FILTER_TYPE_MASK)
#define LSN_DECODE_MIP_FILTER( FILTER )									\
	(((FILTER) >> LSN_MIP_FILTER_SHIFT) & LSN_FILTER_TYPE_MASK) 
#define LSN_DECODE_IS_COMPARISON_FILTER( FILTER )						\
	((FILTER) & LSN_COMPARISON_FILTERING_BIT)							
#define LSN_DECODE_IS_ANISOTROPIC_FILTER( FILTER )						\
	(((FILTER) & LSN_ANISOTROPIC_FILTERING_BIT) &&						\
	(LSN_FILTER_LINEAR == LSN_DECODE_MIN_FILTER( FILTER )) &&			\
	(LSN_FILTER_LINEAR == LSN_DECODE_MAG_FILTER( FILTER )) &&			\
	(LSN_FILTER_LINEAR == LSN_DECODE_MIP_FILTER( FILTER )))

}	// namespace lsn
