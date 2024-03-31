/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: macOS/iOS/tvOS macros and header.
 */

#pragma once

#ifdef __APPLE__

#include <stdexcept>

#define LSN_APPLE

#ifdef __GNUC__
#include <math.h>
#endif	// #ifdef __GNUC__


#ifdef __GNUC__

#ifndef sincos
#define sincos		__sincos
#endif	// #ifndef sincos

#endif	// #ifdef __GNUC__

#if 0
inline uint64_t __emulu( unsigned int a, unsigned int b ) {
	return static_cast<uint64_t>(a) * static_cast<uint64_t>(b);
}
#endif

inline uint64_t _umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand,
	uint64_t * _pui64ProductHi ) {
	__uint128_t ui128Tmp = static_cast<__uint128_t>(_ui64Multiplier) * static_cast<__uint128_t>(_ui64Multiplicand);
	(*_pui64ProductHi) = static_cast<uint64_t>(ui128Tmp >> 64);
	return static_cast<uint64_t>(ui128Tmp);
#if 0
	
	// _ui64Multiplier   = ab = a * 2^32 + b
	// _ui64Multiplicand = cd = c * 2^32 + d
	// ab * cd = a * c * 2^64 + (a * d + b * c) * 2^32 + b * d
	uint64_t ui64A = _ui64Multiplier >> 32;
	uint64_t ui64B = static_cast<uint32_t>(_ui64Multiplier);
	uint64_t ui64C = _ui64Multiplicand >> 32;
	uint64_t ui64D = static_cast<uint32_t>(_ui64Multiplicand);

	uint64_t ui64Ad = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64D) );
	uint64_t ui64Bd = __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64D) );

	uint64_t ui64Abdc = ui64Ad + __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64C) );
	uint64_t ui64AbdcCarry = (ui64Abdc < ui64Ad);

	// _ui64Multiplier * _ui64Multiplicand = _pui64ProductHi * 2^64 + ui64ProductLo
	uint64_t ui64ProductLo = ui64Bd + (ui64Abdc << 32);
	uint64_t ui64ProductLoCarry = (ui64ProductLo < ui64Bd);
	(*_pui64ProductHi) = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64C) ) + (ui64Abdc >> 32) + (ui64AbdcCarry << 32) + ui64ProductLoCarry;

	return ui64ProductLo;
#endif
}

inline uint64_t _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
	if ( _ui64Divisor == 0 ) {
		throw std::overflow_error( "Division by zero is not allowed." );
	}

	if ( _ui64High >= _ui64Divisor ) {
		throw std::overflow_error( "The division would overflow the 64-bit quotient." );
	}

	if ( _ui64High == 0 ) {
		if ( _pui64Remainder ) { (*_pui64Remainder) = _ui64Low % _ui64Divisor; }
		return _ui64Low / _ui64Divisor;
	}
	
	// Combine the high and low parts into a single __uint128_t value.
	__uint128_t ui128Dividend = static_cast<__uint128_t>(_ui64High) << 64 | _ui64Low;
	
	(*_pui64Remainder) = static_cast<uint64_t>(ui128Dividend % _ui64Divisor);
	return static_cast<uint64_t>(ui128Dividend / _ui64Divisor);
#if 0
	uint64_t ui64Q = 0;
	uint64_t ui64R = _ui64High;

	for ( int I = 63; I >= 0; --I ) {
		ui64R = (ui64R << 1) | ((_ui64Low >> I) & 1);

		if ( ui64R >= _ui64Divisor ) {
			ui64R -= _ui64Divisor;
			ui64Q |= 1ULL << I;
		}
	}

	if ( _pui64Remainder ) { (*_pui64Remainder) = ui64R; }
	return ui64Q;
#endif
}

#endif  // #ifdef __APPLE__
