/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Detects the processor feature set.
 */

#pragma once

#include <iostream>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#if defined( _MSC_VER )
#include <intrin.h>
#elif defined( __i386__ ) || defined( __x86_64__ )
#include <cpuid.h>
#else
//#include <x86intrin.h>
#endif  // #if defined( __i386__ ) || defined( __x86_64__ )


#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
#ifdef __GNUC__

void __cpuid( int * _piCpuInfo, int _iInfo ) {
	__asm__ __volatile__(
		"xchg %%ebx, %%edi;"
		"cpuid;"
		"xchg %%ebx, %%edi;"
		:"=a" (_piCpuInfo[0]), "=D" (_piCpuInfo[1]), "=c" (_piCpuInfo[2]), "=d" (_piCpuInfo[3])
		:"0" (_iInfo)
	);
}

unsigned long long _xgetbv( unsigned int _uiIndex ) {
	unsigned int eax, edx;
	__asm__ __volatile__(
		"xgetbv;"
		: "=a" (eax), "=d"(edx)
		: "c" (_uiIndex)
	);
	return ((unsigned long long)edx << 32) | eax;
}

void __cpuidex( int * _piCpuInfo, int _iInfo, int _iSubFunc ) {
    // _iInfo is the leaf, and _iSubFunc is the sub-leaf.
    __cpuid_count( _iInfo, _iSubFunc, _piCpuInfo[0], _piCpuInfo[1], _piCpuInfo[2], _piCpuInfo[3] );
}

#endif	// #ifdef __GNUC__
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )


namespace lsn {

	/**
	 * Class CFeatureSet
	 * \brief Detects the processor feature set.
	 *
	 * Description: Detects the processor feature set.
	 */
    class CFeatureSet {
        class                           CInstructionSet_Internal;

    public:
        // == Functions.
        static std::string              Vendor() { return m_iiCpuRep.m_sVendor; }
        static std::string              Brand() { return m_iiCpuRep.m_sBrand; }

        static inline bool              SSE3() { return m_iiCpuRep.m_bEcx1[0]; }
        static inline bool              PCLMULQDQ() { return m_iiCpuRep.m_bEcx1[1]; }
        static inline bool              MONITOR() { return m_iiCpuRep.m_bEcx1[3]; }
        static inline bool              SSSE3() { return m_iiCpuRep.m_bEcx1[9]; }
        static inline bool              FMA() { return m_iiCpuRep.m_bEcx1[12]; }
        static inline bool              CMPXCHG16B() { return m_iiCpuRep.m_bEcx1[13]; }
        static inline bool              SSE41() { return m_iiCpuRep.m_bEcx1[19]; }
        static inline bool              SSE42() { return m_iiCpuRep.m_bEcx1[20]; }
        static inline bool              MOVBE() { return m_iiCpuRep.m_bEcx1[22]; }
        static inline bool              POPCNT() { return m_iiCpuRep.m_bEcx1[23]; }
        static inline bool              AES() { return m_iiCpuRep.m_bEcx1[25]; }
        static inline bool              XSAVE() { return m_iiCpuRep.m_bEcx1[26]; }
        static inline bool              OSXSAVE() { return m_iiCpuRep.m_bEcx1[27]; }
        static inline bool              AVX() { return m_iiCpuRep.m_bEcx1[28]; }
        static inline bool              F16C() { return m_iiCpuRep.m_bEcx1[29]; }
        static inline bool              RDRAND() { return m_iiCpuRep.m_bEcx1[30]; }

        static inline bool              MSR() { return m_iiCpuRep.m_bEdx1[5]; }
        static inline bool              CX8() { return m_iiCpuRep.m_bEdx1[8]; }
        static inline bool              SEP() { return m_iiCpuRep.m_bEdx1[11]; }
        static inline bool              CMOV() { return m_iiCpuRep.m_bEdx1[15]; }
        static inline bool              CLFSH() { return m_iiCpuRep.m_bEdx1[19]; }
        static inline bool              MMX() { return m_iiCpuRep.m_bEdx1[23]; }
        static inline bool              FXSR() { return m_iiCpuRep.m_bEdx1[24]; }
        static inline bool              SSE() { return m_iiCpuRep.m_bEdx1[25]; }
        static inline bool              SSE2() { return m_iiCpuRep.m_bEdx1[26]; }

        static inline bool              FSGSBASE() { return m_iiCpuRep.m_bEbx7[0]; }
        static inline bool              BMI1() { return m_iiCpuRep.m_bEbx7[3]; }
        static inline bool              HLE() { return m_iiCpuRep.m_bIsIntel && m_iiCpuRep.m_bEbx7[4]; }
        static inline bool              AVX2() { return m_iiCpuRep.m_bEbx7[5]; }
        static inline bool              BMI2() { return m_iiCpuRep.m_bEbx7[8]; }
        static inline bool              ERMS() { return m_iiCpuRep.m_bEbx7[9]; }
        static inline bool              INVPCID() { return m_iiCpuRep.m_bEbx7[10]; }
        static inline bool              RTM() { return m_iiCpuRep.m_bIsIntel && m_iiCpuRep.m_bEbx7[11]; }
        static inline bool              AVX512F() { return m_iiCpuRep.m_bEbx7[16]; }
        static inline bool              RDSEED() { return m_iiCpuRep.m_bEbx7[18]; }
        static inline bool              ADX() { return m_iiCpuRep.m_bEbx7[19]; }
        static inline bool              AVX512PF() { return m_iiCpuRep.m_bEbx7[26]; }
        static inline bool              AVX512ER() { return m_iiCpuRep.m_bEbx7[27]; }
        static inline bool              AVX512CD() { return m_iiCpuRep.m_bEbx7[28]; }
        static inline bool              SHA() { return m_iiCpuRep.m_bEbx7[29]; }
        static inline bool              AVX512BW() { return m_iiCpuRep.m_bEbx7[30]; }
        static inline bool              AVX512VL() { return m_iiCpuRep.m_bEbx7[31]; }

        static inline bool              PREFETCHWT1() { return m_iiCpuRep.m_bEcx7[0]; }

        static inline bool              LAHF() { return m_iiCpuRep.m_bEcx81[0]; }
        static inline bool              LZCNT() { return m_iiCpuRep.m_bIsIntel && m_iiCpuRep.m_bEcx81[5]; }
        static inline bool              ABM() { return m_iiCpuRep.m_bIsAmd && m_iiCpuRep.m_bEcx81[5]; }
        static inline bool              SSE4a() { return m_iiCpuRep.m_bIsAmd && m_iiCpuRep.m_bEcx81[6]; }
        static inline bool              XOP() { return m_iiCpuRep.m_bIsAmd && m_iiCpuRep.m_bEcx81[11]; }
        static inline bool              TBM() { return m_iiCpuRep.m_bIsAmd && m_iiCpuRep.m_bEcx81[21]; }

        static inline bool              SYSCALL() { return m_iiCpuRep.m_bIsIntel && m_iiCpuRep.m_bEdx81[11]; }
        static inline bool              MMXEXT() { return m_iiCpuRep.m_bIsAmd && m_iiCpuRep.m_bEdx81[22]; }
        static inline bool              RDTSCP() { return m_iiCpuRep.m_bIsIntel && m_iiCpuRep.m_bEdx81[27]; }
        static inline bool              _3DNOWEXT() { return m_iiCpuRep.m_bIsAmd && m_iiCpuRep.m_bEdx81[30]; }
        static inline bool              _3DNOW() { return m_iiCpuRep.m_bIsAmd && m_iiCpuRep.m_bEdx81[31]; }

    private :
        // == Members.
        static const                    CInstructionSet_Internal m_iiCpuRep;


        // == Types.
        class CInstructionSet_Internal {
        public :
            CInstructionSet_Internal() :
                m_iNumIds{ 0 },
                m_iNumExIds{ 0 },
                m_bIsIntel{ false },
                m_bIsAmd{ false },
                m_bEcx1{ 0 },
                m_bEdx1{ 0 },
                m_bEbx7{ 0 },
                m_bEcx7{ 0 },
                m_bEcx81{ 0 },
                m_bEdx81{ 0 },
                m_vData{},
                m_vExtData{} {
                //int cpuInfo[4] = {-1};
                std::array<int, 4> aCpuI;

                // Calling __cpuid with 0x0 as the function_id argument
                //	gets the number of the highest valid function ID.
                __cpuid( aCpuI.data(), 0 );
                m_iNumIds = aCpuI[0];

                for ( int I = 0; I <= m_iNumIds; ++I ) {
                    __cpuidex( aCpuI.data(), I, 0 );
                    m_vData.push_back( aCpuI );
                }

                // Capture vendor string.
                char cVendor[0x20];
                ::memset( cVendor, 0, sizeof( cVendor ) );
                (*reinterpret_cast<int*>(cVendor)) = m_vData[0][1];
                (*reinterpret_cast<int*>(cVendor + 4)) = m_vData[0][3];
                (*reinterpret_cast<int*>(cVendor + 8)) = m_vData[0][2];
                m_sVendor = cVendor;
                if ( m_sVendor == "GenuineIntel" ) {
                    m_bIsIntel = true;
                }
                else if ( m_sVendor == "AuthenticAMD" ) {
                    m_bIsAmd = true;
                }

                // Load bitset with flags for function 0x00000001.
                if ( m_iNumIds >= 1 ) {
                    m_bEcx1 = m_vData[1][2];
                    m_bEdx1 = m_vData[1][3];
                }

                // Load bitset with flags for function 0x00000007.
                if ( m_iNumIds >= 7 ) {
                    m_bEbx7 = m_vData[7][1];
                    m_bEcx7 = m_vData[7][2];
                }

                // Calling __cpuid with 0x80000000 as the function_id argument
                //	gets the number of the highest valid extended ID.
                __cpuid( aCpuI.data(), 0x80000000 );
                m_iNumExIds = aCpuI[0];

                char cBrand[0x40];
                ::memset( cBrand, 0, sizeof( cBrand ) );

                for ( int I = 0x80000000; I <= m_iNumExIds; ++I ) {
                    __cpuidex( aCpuI.data(), I, 0 );
                    m_vExtData.push_back( aCpuI );
                }

                // Load bitset with flags for function 0x80000001.
                if ( m_iNumExIds >= 0x80000001 ) {
                    m_bEcx81 = m_vExtData[1][2];
                    m_bEdx81 = m_vExtData[1][3];
                }

                // Interpret CPU cBrand string if reported.
                if ( m_iNumExIds >= 0x80000004 ) {
                    ::memcpy( cBrand, m_vExtData[2].data(), sizeof( aCpuI ) );
                    ::memcpy( cBrand + 16, m_vExtData[3].data(), sizeof( aCpuI ) );
                    ::memcpy( cBrand + 32, m_vExtData[4].data(), sizeof( aCpuI ) );
                    m_sBrand = cBrand;
                }
            };


            // == Members.
            int                             m_iNumIds;
            int                             m_iNumExIds;
            std::string                     m_sVendor;
            std::string                     m_sBrand;
            bool                            m_bIsIntel;
            bool                            m_bIsAmd;
            std::bitset<32>                 m_bEcx1;
            std::bitset<32>                 m_bEdx1;
            std::bitset<32>                 m_bEbx7;
            std::bitset<32>                 m_bEcx7;
            std::bitset<32>                 m_bEcx81;
            std::bitset<32>                 m_bEdx81;
            std::vector<std::array<int, 4>> m_vData;
            std::vector<std::array<int, 4>> m_vExtData;
        };
    };

}   // namespace lsn
