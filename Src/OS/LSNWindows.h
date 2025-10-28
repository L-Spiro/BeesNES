#pragma once

#ifdef LSN_USE_WINDOWS
#include <LSWWin.h>

#pragma comment( lib, "shlwapi.lib" )
#else
/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Windows macros and header.
 */


/*
// Just for reference.
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0  
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000  
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP  
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003  
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista  
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista  
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008  
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista  
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7  
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8  
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1  
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10  
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10  
*/

#define _WIN32_IE							0x0601
#define WINVER								0x0601
#define _WIN32_WINNT						0x0601
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#endif  // #ifdef LSN_USE_WINDOWS

#if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )
#define LSN_WINDOWS
#if defined( WIN32 ) || defined( _WIN32 )
#define LSN_WIN32
#endif	// #ifdef defined( WIN32 ) || defined( _WIN32 )
#ifdef _WIN64
#define LSN_WIN64
#endif	// #ifdef _WIN64
#endif	// #if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )


#ifdef LSN_WINDOWS

#include <cstdint>
#include <immintrin.h>
#include <stdexcept>


#define LSN_FASTCALL						__fastcall

#pragma intrinsic( _BitScanReverse )
#ifdef _WIN64
#pragma intrinsic( _BitScanReverse64 )
#endif  // #ifdef _WIN64


inline unsigned int                             CountLeadingZeros( uint32_t _ui32X ) {
#if defined( _MSC_VER )
    unsigned long ulIndex;
	auto ucIsNonZero = _BitScanReverse( &ulIndex, _ui32X );
	return ucIsNonZero ? (31 - static_cast<int>(ulIndex)) : 32;
#else
    return _ui32X != 0 ? __builtin_clz( _ui32X ) : 32;
#endif  // #if defined( _MSC_VER )
}

inline unsigned int                             CountLeadingZeros( uint64_t _ui64X ) {
#if defined( _MSC_VER )
    #if defined( _WIN64 )
        // Benchmark against (1000000*50) values.
        // _BitScanReverse64(): 0.01772186666666666 seconds
        // Manual version: 0.0004546 seconds. (!!!)
        #if 0
	        unsigned long ulIndex;
	        auto ucIsNonZero = _BitScanReverse64( &ulIndex, _ui64X );
	        return ucIsNonZero ? (63 - static_cast<int>(ulIndex)) : 64;
        #else
            unsigned long uiN = 0;
            if ( _ui64X == 0 ) { return 64; }
            if ( (_ui64X & 0xFFFFFFFF00000000ULL) == 0 ) { uiN += 32; _ui64X <<= 32; }
            if ( (_ui64X & 0xFFFF000000000000ULL) == 0 ) { uiN += 16; _ui64X <<= 16; }
            if ( (_ui64X & 0xFF00000000000000ULL) == 0 ) { uiN += 8;  _ui64X <<= 8; }
            if ( (_ui64X & 0xF000000000000000ULL) == 0 ) { uiN += 4;  _ui64X <<= 4; }
            if ( (_ui64X & 0xC000000000000000ULL) == 0 ) { uiN += 2;  _ui64X <<= 2; }
            if ( (_ui64X & 0x8000000000000000ULL) == 0 ) { uiN += 1; }

            return uiN;
        #endif  // #if 0
    #else
        if ( _ui64X == 0 ) { return 64; }
        unsigned int uiN = 0;

        #if 0
            // Benchmark against (1000000*50) values (x86 on x64 hardware). 
            // 0.0002651
            // 0.0002426333333333333
            if ( (_ui64X & 0xFFFFFFFF00000000ULL) == 0 ) { uiN += 32; _ui64X <<= 32; }
            if ( (_ui64X & 0xFFFF000000000000ULL) == 0 ) { uiN += 16; _ui64X <<= 16; }
            if ( (_ui64X & 0xFF00000000000000ULL) == 0 ) { uiN += 8;  _ui64X <<= 8; }
            if ( (_ui64X & 0xF000000000000000ULL) == 0 ) { uiN += 4;  _ui64X <<= 4; }
            if ( (_ui64X & 0xC000000000000000ULL) == 0 ) { uiN += 2;  _ui64X <<= 2; }
            if ( (_ui64X & 0x8000000000000000ULL) == 0 ) { uiN += 1; }
            return uiN;
        #else
            // Benchmark against (1000000*50) values (x86 on x64 hardware). 
            // 0.0002392 seconds.
            if ( _ui64X & 0xFFFFFFFF00000000ULL )        { uiN += 32; _ui64X >>= 32; }
	        if ( _ui64X & 0xFFFF0000 )                   { uiN += 16; _ui64X >>= 16; }
	        if ( _ui64X & 0xFFFFFF00 )                   { uiN += 8;  _ui64X >>= 8; }
	        if ( _ui64X & 0xFFFFFFF0 )                   { uiN += 4;  _ui64X >>= 4; }
	        if ( _ui64X & 0xFFFFFFFC )                   { uiN += 2;  _ui64X >>= 2; }
	        if ( _ui64X & 0xFFFFFFFE )                   { uiN += 1; }
	        return 63 - uiN;
        #endif  // #if 0
    #endif  // #if defined( _WIN64 )
#elif defined( __GNUC__ )
    return _ui64X != 0 ? __builtin_clzll( _ui64X ) : 64;
#endif  // #if defined( __GNUC__ )
}

#if defined( _MSC_VER )
    #ifdef _WIN64
        #include <intrin.h>
        #pragma intrinsic( _udiv128 )
    #else
		#include <bit>
        inline uint64_t                         _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
            if ( _ui64Divisor == 0 ) [[unlikely]] { throw std::overflow_error( "Division by zero is not allowed." ); }
			if ( _ui64High >= _ui64Divisor ) [[unlikely]] { throw std::overflow_error( "The division would overflow the 64-bit quotient." ); }
	        if ( _ui64High == 0 ) {
		        if ( _pui64Remainder ) { (*_pui64Remainder) = _ui64Low % _ui64Divisor; }
		        return _ui64Low / _ui64Divisor;
	        }

            // == Knuth-style division in base b = 2^32.
            constexpr uint64_t ui64BitsPerWord = sizeof( uint64_t ) * 8;
            constexpr uint64_t ui64Base = 1ULL << (ui64BitsPerWord / 2);	// b = 2^32

            uint64_t ui64V = _ui64Divisor;
            uint64_t ui64Un64;												// High "digit" after normalization step.
            uint64_t ui64Un10;												// Low 64 after normalization step.

            // Normalize: shift left so that the top bit of ui64V is set.
            unsigned int uiShift = static_cast<unsigned int>(std::countl_zero( ui64V ));
            if ( uiShift > 0 ) {
                ui64V = ui64V << uiShift;
                ui64Un64 = (_ui64High << uiShift) | (_ui64Low >> (ui64BitsPerWord - uiShift));
                ui64Un10 = _ui64Low << uiShift;
            }
            else {
                // Avoid (x >> 64) UB for uiShift==0.
                ui64Un64 = _ui64High;
                ui64Un10 = _ui64Low;
            }

            // Split divisor into two 32-bit digits.
            const uint64_t ui64Vn1 = ui64V >> (ui64BitsPerWord / 2);
            const uint64_t ui64Vn0 = ui64V & 0xFFFFFFFFULL;

            // Split the low (normalized) 64 into two 32-bit digits.
            uint64_t ui64Un1 = ui64Un10 >> (ui64BitsPerWord / 2);
            uint64_t ui64Un0 = ui64Un10 & 0xFFFFFFFFULL;

            // First quotient digit q1.
            uint64_t ui64Q1 = ui64Un64 / ui64Vn1;
            uint64_t ui64Rhat = ui64Un64 - ui64Q1 * ui64Vn1;

            // Correct q1 (at most 2 decrements).
            while ( ui64Q1 >= ui64Base || ui64Q1 * ui64Vn0 > ui64Base * ui64Rhat + ui64Un1 ) {
                ui64Q1 -= 1;
                ui64Rhat += ui64Vn1;
                if ( ui64Rhat >= ui64Base ) { break; }
            }

            // Combine and subtract q1 * v.
            uint64_t ui64Un21 = ui64Un64 * ui64Base + ui64Un1 - ui64Q1 * ui64V;

            // Second quotient digit q0.
            uint64_t ui64Q0 = ui64Un21 / ui64Vn1;
            ui64Rhat = ui64Un21 - ui64Q0 * ui64Vn1;

            // Correct q0 (at most 2 decrements).
            while ( ui64Q0 >= ui64Base || ui64Q0 * ui64Vn0 > ui64Base * ui64Rhat + ui64Un0 ) {
                ui64Q0 -= 1;
                ui64Rhat += ui64Vn1;
                if ( ui64Rhat >= ui64Base ) { break; }
            }

            // Remainder (denormalize back).
            if ( _pui64Remainder ) {
                (*_pui64Remainder) = ((ui64Un21 * ui64Base + ui64Un0) - ui64Q0 * ui64V) >> uiShift;
            }

            // Quotient.
            return ui64Q1 * ui64Base + ui64Q0;

	        /*uint64_t ui64Q = 0;
	        uint64_t ui64R = _ui64High;

	        for ( int I = 63; I >= 0; --I ) {
		        ui64R = (ui64R << 1) | ((_ui64Low >> I) & 1);

		        if ( ui64R >= _ui64Divisor ) {
			        ui64R -= _ui64Divisor;
			        ui64Q |= 1ULL << I;
		        }
	        }

	        if ( _pui64Remainder ) { (*_pui64Remainder) = ui64R; }
	        return ui64Q;*/
        }

    #endif  // #ifdef _WIN64
#elif defined( __x86_64__ ) || defined( _M_X64 )
    #include <immintrin.h>

    /**
     * \brief Unsigned 128-by-64 division using x86_64 DIVQ, returning a 64-bit quotient and (optionally) a 64-bit remainder.
     *
     * Computes ( (_ui64High: _ui64Low) / _ui64Divisor ). The hardware DIVQ instruction divides the 128-bit
     * value in RDX:RAX by the given 64-bit divisor, returning the 64-bit quotient in RAX and remainder in RDX.
     * To avoid the CPU raising a #DE fault, this performs the same precondition checks as MSVC's _udiv128:
     *  - Division by zero is rejected.
     *  - If _ui64High >= _ui64Divisor the quotient would not fit into 64 bits, so it is rejected as overflow.
     *
     * \param _ui64High The high 64 bits of the 128-bit unsigned dividend.
     * \param _ui64Low  The low 64 bits of the 128-bit unsigned dividend.
     * \param _ui64Divisor The 64-bit unsigned divisor.
     * \param _pui64Remainder Optional pointer that receives the unsigned remainder.
     * \return Returns the 64-bit unsigned quotient.
     * \throws std::overflow_error On division by zero or when the 64-bit quotient would overflow.
     */
    inline uint64_t                             _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
        uint64_t ui64Quot, ui64Rem;

        asm(
            "divq %4"
            : "=a"(ui64Quot), "=d"(ui64Rem)
            : "a"(_ui64Low), "d"(_ui64High), "r"(_ui64Divisor)
        );

        if ( _pui64Remainder ) {
            (*_pui64Remainder) = ui64Rem;
        }
        return ui64Quot;
    }
#elif defined( __SIZEOF_INT128__ )
    /**
     * \brief Unsigned 128-by-64 division using native __uint128_t, returning a 64-bit quotient and (optionally) a 64-bit remainder.
     *
     * Computes ( (_ui64High: _ui64Low) / _ui64Divisor ) using __uint128_t arithmetic.
     * Enforces the same preconditions as other variants so behavior is consistent across platforms:
     *  - Division by zero is rejected.
     *  - If _ui64High >= _ui64Divisor then the 64-bit quotient would overflow and is rejected.
     *
     * \param _ui64High The high 64 bits of the 128-bit unsigned dividend.
     * \param _ui64Low  The low 64 bits of the 128-bit unsigned dividend.
     * \param _ui64Divisor The 64-bit unsigned divisor.
     * \param _pui64Remainder Optional pointer that receives the unsigned remainder.
     * \return Returns the 64-bit unsigned quotient.
     * \throws std::overflow_error On division by zero or when the 64-bit quotient would overflow.
     */
    inline uint64_t                             _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
        __uint128_t ui128Div = (static_cast<__uint128_t>(_ui64High) << 64) | _ui64Low;
        uint64_t ui64Quot = ui128Div / _ui64Divisor;
        if ( _pui64Remainder ) {
            (*_pui64Remainder) = ui128Div % _ui64Divisor;
        }
        return ui64Quot;
    }
#endif  // #if defined( _MSC_VER )


#if defined( _MSC_VER ) && defined( _WIN64 )
	#pragma intrinsic( _div128 )
#else
	/**
	 * \brief Performs signed 128-bit-by-64-bit division returning a 64-bit signed quotient and (optionally) a 64-bit signed remainder.
	 *
	 * Given a signed 128-bit dividend split into high and low 64-bit parts, and a signed 64-bit divisor,
	 * this routine computes the C/C++-semantics result where the quotient is truncated toward zero and the remainder
	 * has the same sign as the dividend with |remainder| < |divisor|.
	 * Internally it reduces to an unsigned divide via _udiv128() on absolute magnitudes and then fixes signs,
	 * detecting overflow of the 64-bit signed quotient.
	 *
	 * \param _i64HighDividend The signed high 64 bits of the 128-bit dividend (sign bit is taken from this).
	 * \param _i64LowDividend  The signed low 64 bits of the 128-bit dividend.
	 * \param _i64Divisor      The signed 64-bit divisor.
	 * \param _pi64Remainder   Optional pointer that receives the signed remainder (same sign as the dividend).
	 * \return Returns the signed 64-bit quotient.
	 * \throws std::overflow_error On division by zero or when the true quotient does not fit in a signed 64-bit integer.
	 */
	inline __int64								_div128( __int64 _i64HighDividend, __int64 _i64LowDividend, __int64 _i64Divisor, __int64 * _pi64Remainder ) {
		// Validate divisor.
		if ( _i64Divisor == 0 ) [[unlikely]] { throw std::overflow_error( "Division by zero is not allowed." ); }

		// Determine signs and compute absolute magnitudes without invoking UB on INT64_MIN.
		const bool bDividendNeg		= (_i64HighDividend < 0);
		const bool bDivisorNeg		= (_i64Divisor < 0);
		const bool bQuotNeg			= (bDividendNeg != bDivisorNeg);		// XOR: quotient sign.
		const bool bRemNeg			= bDividendNeg;							// Remainder sign matches dividend (C++ semantics).

		// Represent the 128-bit dividend as unsigned parts; bit patterns preserved on cast.
		uint64_t ui64Hi = static_cast<uint64_t>(_i64HighDividend);
		uint64_t ui64Lo = static_cast<uint64_t>(_i64LowDividend);

		// Two's-complement negate (128-bit) if the dividend is negative: (hi,lo) = - (hi,lo).
		auto Negate128 = []( uint64_t &_ui64Hi, uint64_t &_ui64Lo ) {
			uint64_t ui64NewLo = ~_ui64Lo + 1ULL;							// Add 1 to low after invert.
			_ui64Hi = ~_ui64Hi + (ui64NewLo == 0ULL ? 1ULL : 0ULL);			// Propagate carry into high if low wrapped.
			_ui64Lo = ui64NewLo;
		};
		if ( bDividendNeg ) {
			Negate128( ui64Hi, ui64Lo );
		}

		// Absolute value of the divisor as unsigned using two's-complement trick (works for INT64_MIN).
		uint64_t ui64Div = bDivisorNeg ? static_cast<uint64_t>(0ULL - static_cast<uint64_t>(_i64Divisor))
										: static_cast<uint64_t>(_i64Divisor);

		// Use existing unsigned 128/64 divide to get magnitude of quotient and remainder.
		uint64_t ui64Rem = 0ULL;
		const uint64_t ui64UQuot = _udiv128( ui64Hi, ui64Lo, ui64Div, &ui64Rem );

		// Check for signed 64-bit overflow of the quotient's final value.
		// If the result is non-negative, it must be <= INT64_MAX.
		// If the result is negative, magnitude may be up to 2^63 (i.e., 0x8000'0000'0000'0000).
		if ( !bQuotNeg ) {
			if ( ui64UQuot > static_cast<uint64_t>(INT64_MAX) ) [[unlikely]] {
				throw std::overflow_error( "The division would overflow the 64-bit signed quotient." );
			}
		}
		else {
			if ( ui64UQuot > (1ULL << 63) ) [[unlikely]] {
				throw std::overflow_error( "The division would overflow the 64-bit signed quotient." );
			}
		}

		// Form the signed quotient without invoking UB on INT64_MIN.
		// For negative: two's-complement via (0 - magnitude) in unsigned domain then cast.
		const __int64 i64Quot = bQuotNeg ? static_cast<__int64>(static_cast<uint64_t>(0ULL - ui64UQuot))
											: static_cast<__int64>(ui64UQuot);

		// Signed remainder (same sign as dividend).
		if ( _pi64Remainder ) {
			(*_pi64Remainder) = bRemNeg ? static_cast<__int64>(static_cast<uint64_t>(0ULL - ui64Rem))
											: static_cast<__int64>(ui64Rem);
		}

		return i64Quot;
	}
#endif	// #if defined( _MSC_VER ) && defined( _WIN64 )


#if defined( _MSC_VER )
	#if defined( _WIN64 )
		#pragma intrinsic( _umul128 )
	#else
		// Mercilessly ripped from: https://stackoverflow.com/a/46924301
		//#include <intrin.h>
	
		/**
		 * \brief Computes a full-width 64Å~64 Å® 128-bit unsigned product, returning the low 64 bits and writing the high 64 bits via pointer.
		 *
		 * On x86_64 targets this maps to a single `mulq` instruction (`RDX:RAX Å© RAX * r/m64`). On compilers that support
		 * `__uint128_t`, it uses native 128-bit arithmetic to produce the exact product. This routine performs a pure unsigned
		 * multiplication; it does not perform any parameter validation and assumes `_pui64ProductHi` is a valid, writable pointer.
		 * No exceptions are thrown.
		 *
		 * \param _ui64Multiplier The left 64-bit unsigned multiplicand.
		 * \param _ui64Multiplicand The right 64-bit unsigned multiplicand.
		 * \param _pui64ProductHi Pointer that receives the upper 64 bits of the 128-bit product.
		 * \return The lower 64 bits of the 128-bit product.
		 *
		 * \note Semantics match those of `UnsignedMultiply128` on Windows: `(high, low) = _umul128( a, b, &high )`.
		 * \note This function is thread-safe and has no side effects beyond writing through `_pui64ProductHi`.
		 */
		inline uint64_t LSN_FASTCALL                    _umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, 
			uint64_t *_pui64ProductHi ) {
		#if 0
			return UnsignedMultiply128( _ui64Multiplier, _ui64Multiplicand, _pui64ProductHi );
		#else
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
		#endif  // 1
		}
	#endif	// #if defined( _WIN64 )
#elif defined( __x86_64__ ) || defined( _M_X64 )
    //#include <stdint.h>

    /**
	 * \brief Computes a full-width 64Å~64 Å® 128-bit unsigned product, returning the low 64 bits and writing the high 64 bits via pointer.
	 *
	 * On x86_64 targets this maps to a single `mulq` instruction (`RDX:RAX Å© RAX * r/m64`). On compilers that support
	 * `__uint128_t`, it uses native 128-bit arithmetic to produce the exact product. This routine performs a pure unsigned
	 * multiplication; it does not perform any parameter validation and assumes `_pui64ProductHi` is a valid, writable pointer.
	 * No exceptions are thrown.
	 *
	 * \param _ui64Multiplier The left 64-bit unsigned multiplicand.
	 * \param _ui64Multiplicand The right 64-bit unsigned multiplicand.
	 * \param _pui64ProductHi Pointer that receives the upper 64 bits of the 128-bit product.
	 * \return The lower 64 bits of the 128-bit product.
	 *
	 * \note Semantics match those of `UnsignedMultiply128` on Windows: `(high, low) = _umul128( a, b, &high )`.
	 * \note This function is thread-safe and has no side effects beyond writing through `_pui64ProductHi`.
	 */
    inline uint64_t LSN_FASTCALL                    _umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, 
        uint64_t * _pui64ProductHi ) {
        uint64_t ui64Lo, ui64Hi;
        asm(
            "mulq %3"
            : "=&a"(ui64Lo), "=&d"(ui64Hi)
            : "a"(_ui64Multiplier), "r"(_ui64Multiplicand)
        );
        (*_pui64ProductHi) = ui64Hi;
        return ui64Lo;
    }

#elif defined( __SIZEOF_INT128__ )
    /**
	 * \brief Computes a full-width 64Å~64 Å® 128-bit unsigned product, returning the low 64 bits and writing the high 64 bits via pointer.
	 *
	 * On x86_64 targets this maps to a single `mulq` instruction (`RDX:RAX Å© RAX * r/m64`). On compilers that support
	 * `__uint128_t`, it uses native 128-bit arithmetic to produce the exact product. This routine performs a pure unsigned
	 * multiplication; it does not perform any parameter validation and assumes `_pui64ProductHi` is a valid, writable pointer.
	 * No exceptions are thrown.
	 *
	 * \param _ui64Multiplier The left 64-bit unsigned multiplicand.
	 * \param _ui64Multiplicand The right 64-bit unsigned multiplicand.
	 * \param _pui64ProductHi Pointer that receives the upper 64 bits of the 128-bit product.
	 * \return The lower 64 bits of the 128-bit product.
	 *
	 * \note Semantics match those of `UnsignedMultiply128` on Windows: `(high, low) = _umul128( a, b, &high )`.
	 * \note This function is thread-safe and has no side effects beyond writing through `_pui64ProductHi`.
	 */
    inline uint64_t LSN_FASTCALL                    _umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, 
        uint64_t * _pui64ProductHi ) {
        __uint128_t ui128Prod = static_cast<__uint128_t>(_ui64Multiplier) * static_cast<__uint128_t>(_ui64Multiplicand);
        (*_pui64ProductHi) = static_cast<uint64_t>(ui128Prod >> 64);
        return static_cast<uint64_t>(ui128Prod);
    }
#endif  // #if defined( _MSC_VER ) && defined( _WIN64 )


extern "C" {

#if defined( _M_AMD64 )
extern void sincos( double _dAngle, double * _pdSin, double * _pdCos );
extern void sincosf( float _fAngle, float * _pfSin, float * _pfCos );
#else

// 32 bit implementation in inline assembly
inline void                                     sincos( double _dAngle, double * _pdSin, double * _pdCos ) {
	double dSin, dCos;
	__asm {
		fld QWORD PTR[_dAngle]
		fsincos
		fstp QWORD PTR[dCos]
		fstp QWORD PTR[dSin]
		fwait
	}
	(*_pdSin) = dSin;
	(*_pdCos) = dCos;
}

inline void                                     sincosf( float _fAngle, float * _pfSin, float * _pfCos ) {
    float fSinT, fCosT;
    __asm {
        fld DWORD PTR[_fAngle]		// Load the 32-bit float into the FPU stack.
        fsincos						// Compute cosine and sine.
        fstp DWORD PTR[fCosT]		// Store the cosine value.
        fstp DWORD PTR[fSinT]		// Store the sine value.
        fwait						// Wait for the FPU to finish.
    }
    (*_pfSin) = fSinT;
    (*_pfCos) = fCosT;
}

#endif

}	// extern "C"

#endif	// #ifdef LSN_WINDOWS
