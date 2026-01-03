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

// Implementation for compilers that support __uint128_t (e.g., GCC, Clang)
inline uint64_t                             _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
	if ( _ui64Divisor == 0 ) {
		throw std::overflow_error( "_udiv128: Division by zero is not allowed." );
	}

	if ( _ui64High >= _ui64Divisor ) {
		throw std::overflow_error( "_udiv128: The division would overflow the 64-bit quotient." );
	}

	if ( _ui64High == 0 ) {
		if ( _pui64Remainder ) { (*_pui64Remainder) = _ui64Low % _ui64Divisor; }
		return _ui64Low / _ui64Divisor;
	}
	
	// Combine the high and low parts into a single __uint128_t value.
	__uint128_t ui128Dividend = static_cast<__uint128_t>(_ui64High) << 64 | _ui64Low;
	
	if ( _pui64Remainder ) {
		(*_pui64Remainder) = static_cast<uint64_t>(ui128Dividend % _ui64Divisor);
	}
	return static_cast<uint64_t>(ui128Dividend / _ui64Divisor);
}

// Implementation for compilers that support __int128 (e.g., GCC, Clang).
inline int64_t								_div128( int64_t _i64High, int64_t _i64Low, int64_t _i64Divisor, int64_t * _pi64Remainder ) {
	if ( _i64Divisor == 0 ) {
		throw std::overflow_error( "_div128: Division by zero is not allowed." );
	}

	// Combine the high and low parts into a single __int128 value.
	// Note: The low part is treated as an unsigned 64-bit chunk when OR'ing into the 128-bit value.
	const __int128 i128Dividend =
		(static_cast<__int128>(_i64High) << 64) |
		static_cast<__int128>(static_cast<unsigned __int128>(static_cast<uint64_t>(_i64Low)));

	// Compute quotient and remainder using truncation toward zero (C/C++ semantics).
	const __int128 i128Quot = i128Dividend / static_cast<__int128>(_i64Divisor);
	const __int128 i128Rem = i128Dividend % static_cast<__int128>(_i64Divisor);

	// The MSVC-style _div128 contract returns an int64 quotient; overflow must be reported.
	if ( i128Quot < static_cast<__int128>(INT64_MIN) || i128Quot > static_cast<__int128>(INT64_MAX) ) {
		throw std::overflow_error( "_div128: The division would overflow the 64-bit quotient." );
	}

	if ( _pi64Remainder ) {
		// Remainder is always in [-|divisor|+1, |divisor|-1], so it fits in int64_t.
		(*_pi64Remainder) = static_cast<int64_t>(i128Rem);
	}

	return static_cast<int64_t>(i128Quot);
}

/**
 * \brief Computes u64 * u64 -> u128, returning the low 64 bits and writing the high 64 bits.
 *
 * \param _ui64Multiplier The first operand.
 * \param _ui64Multiplicand The second operand.
 * \param _pui64ProductHi Receives the high 64 bits of the product.
 * \return Returns the low 64 bits of the product.
 **/
inline uint64_t								_umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, uint64_t * _pui64ProductHi ) {
	assert( _pui64ProductHi );

	__uint128_t ui128Tmp = static_cast<__uint128_t>(_ui64Multiplier) * static_cast<__uint128_t>(_ui64Multiplicand);
	(*_pui64ProductHi) = static_cast<uint64_t>(ui128Tmp >> 64);
	return static_cast<uint64_t>(ui128Tmp);
}

/**
 * \brief Computes s64 * s64 -> s128, returning the low 64 bits and writing the high 64 bits.
 *
 * \param _i64Multiplier The first operand.
 * \param _i64Multiplicand The second operand.
 * \param _pi64HighProduct Receives the high 64 bits of the product.
 * \return Returns the low 64 bits of the product.
 **/
inline int64_t LSN_FASTCALL					_mul128( int64_t _i64Multiplier, int64_t _i64Multiplicand, int64_t * _pi64HighProduct ) {
	assert( _pi64HighProduct );

	const __int128 i128Prod = static_cast<__int128>(_i64Multiplier) * static_cast<__int128>(_i64Multiplicand);
	(*_pi64HighProduct) = static_cast<int64_t>(i128Prod >> 64);
	return static_cast<int64_t>(i128Prod);
}

#endif  // #ifdef __APPLE__
