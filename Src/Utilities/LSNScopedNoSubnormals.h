/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Functions for disabling/enabling floating-point denormals.
 */


#pragma once

#include <cstdint>

#if defined( _MSC_VER )
    #include <float.h>
#endif

#if defined( __SSE__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || (defined( _M_IX86_FP ) && _M_IX86_FP >= 1)
    #include <immintrin.h>
    #include <xmmintrin.h>
#endif

namespace lsn {

    /**
     * Returns true if this build target supports explicit subnormal control.
     */
    static constexpr bool		HasNoSubnormalsControl() {
#if defined( __aarch64__ ) || defined( _M_ARM64 ) || defined( __arm__ ) || defined( _M_ARM ) || defined( __SSE__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || (defined( _M_IX86_FP ) && _M_IX86_FP >= 1)
        return true;
#else
        return false;
#endif
    }

    /**
     * Enables "no subnormals" for the current thread.
     * x86/x64 -> MXCSR: set FTZ (bit 15) and DAZ (bit 6).
     * ARM/AArch64 -> FPCR/FPSCR: set FZ (bit 24) and (where available) FZ16 (bit 19).
     */
    static inline void			EnableNoSubnormals() {
#if defined( __aarch64__ ) || defined( _M_ARM64 )
        std::uint64_t ullFpcr = __builtin_aarch64_get_fpcr();
        ullFpcr |= (1ULL << 24) | (1ULL << 19);
        __builtin_aarch64_set_fpcr( ullFpcr );
#elif defined( __arm__ ) || defined( _M_ARM )
        std::uint32_t uiFpscr = __builtin_arm_get_fpscr();
        uiFpscr |= (1U << 24);
        __builtin_arm_set_fpscr( uiFpscr );
#elif defined( __SSE__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || (defined( _M_IX86_FP ) && _M_IX86_FP >= 1)
        unsigned int uiMxcsr = _mm_getcsr();
        uiMxcsr |= 0x8000U; // FTZ.
        uiMxcsr |= 0x0040U; // DAZ.
        _mm_setcsr( uiMxcsr );
#else
        // No-op.
#endif
    }

    /**
     * Disables "no subnormals" for the current thread (restores gradual underflow).
     */
    static inline void			DisableNoSubnormals() {
#if defined( __aarch64__ ) || defined( _M_ARM64 )
        std::uint64_t ullFpcr = __builtin_aarch64_get_fpcr();
        ullFpcr &= ~((1ULL << 24) | (1ULL << 19));
        __builtin_aarch64_set_fpcr( ullFpcr );
#elif defined( __arm__ ) || defined( _M_ARM )
        std::uint32_t uiFpscr = __builtin_arm_get_fpscr();
        uiFpscr &= ~(1U << 24);
        __builtin_arm_set_fpscr( uiFpscr );
#elif defined( __SSE__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || (defined( _M_IX86_FP ) && _M_IX86_FP >= 1)
        unsigned int uiMxcsr = _mm_getcsr();
        uiMxcsr &= ~0x8000U; // Clear FTZ.
        uiMxcsr &= ~0x0040U; // Clear DAZ.
        _mm_setcsr( uiMxcsr );
#else
        // No-op.
#endif
    }

    /**
     * Optional (Windows): also request CRT/x87 flush-to-zero for legacy code paths.
     * On x64 MSVC, SSE/AVX controls above are authoritative.
     */
    static inline void			EnableNoSubnormalsX87() {
#if defined( _MSC_VER )
        unsigned int uiOld{};
        _controlfp_s( &uiOld, _DN_FLUSH, _MCW_DN );
#endif
    }

    /**
     * RAII scope that enables "no subnormals" for the current thread and restores
     * the prior mode on destruction.
     */
    class CScopedNoSubnormals {
    public :
        /**
         * Captures the current control register and enables "no subnormals".
         */
        CScopedNoSubnormals() {
#if defined( __aarch64__ ) || defined( _M_ARM64 )
            m_ullSaved = __builtin_aarch64_get_fpcr();
            EnableNoSubnormals();
#elif defined( __arm__ ) || defined( _M_ARM )
            m_uiSaved = __builtin_arm_get_fpscr();
            EnableNoSubnormals();
#elif defined( __SSE__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || (defined( _M_IX86_FP ) && _M_IX86_FP >= 1)
            m_uiSaved = _mm_getcsr();
            EnableNoSubnormals();
#else
            // Nothing to save.
#endif
        }

        /**
         * Restores the previously captured control register.
         */
        ~CScopedNoSubnormals() {
#if defined( __aarch64__ ) || defined( _M_ARM64 )
            __builtin_aarch64_set_fpcr( m_ullSaved );
#elif defined( __arm__ ) || defined( _M_ARM )
            __builtin_arm_set_fpscr( m_uiSaved );
#elif defined( __SSE__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || (defined( _M_IX86_FP ) && _M_IX86_FP >= 1)
            _mm_setcsr( m_uiSaved );
#else
            // Nothing to restore.
#endif
        }

        CScopedNoSubnormals( const CScopedNoSubnormals &_sOther ) = delete;
        CScopedNoSubnormals &	operator = ( const CScopedNoSubnormals &_sOther ) = delete;

    private :
#if defined( __aarch64__ ) || defined( _M_ARM64 )
        std::uint64_t			m_ullSaved{};
#elif defined( __arm__ ) || defined( _M_ARM )
        std::uint32_t			m_uiSaved{};
#elif defined( __SSE__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || (defined( _M_IX86_FP ) && _M_IX86_FP >= 1)
        unsigned int			m_uiSaved{};
#endif
    };

} // namespace lsn
