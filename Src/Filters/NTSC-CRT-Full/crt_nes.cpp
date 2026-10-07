/*****************************************************************************/
/*
 * NTSC/CRT - integer-only NTSC video signal encoding / decoding emulation
 *
 *   by EMMIR 2018-2023
 *   modifications for Mesen by Persune
 *   https://github.com/LMP88959/NTSC-CRT
 *
 *   YouTube: https://www.youtube.com/@EMMIR_KC/videos
 *   Discord: https://discord.com/invite/hdYctSmyQJ
 */
/*****************************************************************************/

#include "crt_core.h"

#if (CRT_SYSTEM == CRT_SYSTEM_NES)
#include "../../Utilities/LSNUtilities.h"

#include <immintrin.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>


/* amplified IRE = ((mV / 7.143) - 312 / 7.143) * 1024 */
/* https://www.nesdev.org/wiki/NTSC_video#Brightness_Levels */
static int IRE[16] = {
    /* 0d     1d     2d      3d */
    -12042, 0,     34406,  81427,
    /* 0d     1d     2d      3d emphasized */
    -17203,-8028,  19497,  57342,
    /* 00     10     20      30 */
    43581, 75693, 112965, 112965,
    /* 00     10     20      30 emphasized */
    26951, 52181, 83721,  83721
};
static int active[6] = {
    0300, 0100,
    0500, 0400,
    0600, 0200
};
/* generate the square wave for a given 9-bit pixel and phase */
static int
square_sample(int p, int phase)
{
#if 0
	// Decode the NES color.
	unsigned short ui16Color = (p & 0x0F);								// 0..15 "cccc".
	unsigned short ui16Level = (ui16Color >= 0xE) ? 1 : (p >> 4) & 3;	// 0..3  "ll".  For colors 14..15, level 1 is forced.
	unsigned short ui16Emphasis = (p >> 6);								// 0..7  "eee".

#define LSN_INCOLORPHASE( COLOR )					(((COLOR) + phase) % 12 < 6)
	// When de-emphasis bits are set, some parts of the signal are attenuated:
	// Colors [14..15] are not affected by de-emphasis.
	unsigned short ui16Atten = ((ui16Color < 0xE) &&
		(((ui16Emphasis & 1) && LSN_INCOLORPHASE( 0xC )) ||
		((ui16Emphasis & 2) && LSN_INCOLORPHASE( 0x4 )) ||
		((ui16Emphasis & 4) && LSN_INCOLORPHASE( 0x8 )))) ? 8 : 0;

	// The square wave for this color alternates between these two voltages:
	int fLow  = IRE[ui16Level+ui16Atten];
	int fHigh = (&IRE[4])[ui16Level+ui16Atten];
	if LSN_UNLIKELY( ui16Color == 0 ) { return fHigh; }						// For color 0, only high level is emitted.
	if LSN_UNLIKELY( ui16Color > 12 ) { return fLow; }						// For colors 13..15, only low level is emitted.

	return LSN_INCOLORPHASE( ui16Color ) ? fHigh : fLow;
#undef LSN_INCOLORPHASE
#else
    int hue;
    int e, l, v;

    hue = (p & 0x0f);

    /* last two columns are black */
    if LSN_UNLIKELY(hue >= 0x0e) {
        return 0;
    }

    v = (((hue + phase) % 12) < 6);

    /* red 0100, green 0200, blue 0400 */
    e = (((p & 0700) & active[(phase >> 1) % 6]) > 0);
    switch (hue) {
        case 0x00: l = 1; break;
        case 0x0d: l = 0; break;
        default:   l = v; break;
    }
    return IRE[(l << 3) + (e << 2) + ((p >> 4) & 3)];
#endif
}

/**
 * The table of band-limited signal samples, indexed by [9-bit pixel][phase % 12].
 * 
 * Each sample sums 4 of the 12 square-wave phases, but samples are only 3 phases apart and the line-to-line phase offsets (0, 4, 8) are not
 *	multiples of 3, so sampling the raw square wave aliases its upper harmonics onto the color carrier differently on each of the 3 line phases.
 *	That gave the same color a different hue and saturation on every 3rd line (visible as horizontal lines across flat areas).  Keeping only
 *	the DC and fundamental (all that a TV passes in the chroma band anyway) makes every line phase decode to the same color.
 **/
struct LSN_NES_SIGNAL_TABLE {
	LSN_NES_SIGNAL_TABLE() {
		constexpr double pi = 3.14159265358979323846;
		for ( int p = 0; p < 512; ++p ) {
			double box[12];
			double dc = 0.0, re = 0.0, im = 0.0;
			for ( int I = 0; I < 12; ++I ) {
				box[I] = double( square_sample( p, I + 0 ) + square_sample( p, I + 1 ) + square_sample( p, I + 2 ) + square_sample( p, I + 3 ) );
				dc += box[I];
				re += box[I] * std::cos( 2.0 * pi * I / 12.0 );
				im += box[I] * std::sin( 2.0 * pi * I / 12.0 );
			}
			dc /= 12.0;
			re *= 2.0 / 12.0;
			im *= 2.0 / 12.0;
			for ( int I = 0; I < 12; ++I ) {
				tsample[p][I] = int( std::lround( dc + re * std::cos( 2.0 * pi * I / 12.0 ) + im * std::sin( 2.0 * pi * I / 12.0 ) ) );
			}
		}
	}


	// == Members.
	int																	tsample[512][12];									/**< The band-limited sum of 4 square-wave phases for each pixel and starting phase. */
};

/**
 * Gets the band-limited sum of 4 square-wave phases for a given pixel and starting phase.
 * 
 * \param _iP The 9-bit pixel.
 * \param _iPhase The starting phase.
 * \return Returns the band-limited sum of square_sample( _iP, _iPhase + 0 ) through square_sample( _iP, _iPhase + 3 ).
 **/
static inline int
BandLimitedSample( int _iP, int _iPhase )
{
	static const LSN_NES_SIGNAL_TABLE table;
	return table.tsample[_iP&0x1FF][_iPhase%12];
}

/**
 * The NES's signal at its own resolution (12 phases per carrier cycle, 8 per pixel), band-limited and taken at the decoder's
 *	sample rate.
 *
 * Each pixel lasts 8 of the 12 color phases, so at 4 samples per carrier cycle pixel edges fall at 2.67-sample steps.  Taking one sample
 *	per 3 phases from the raw signal snapped every edge to the sample grid, so an object's shape changed as it moved by a pixel (bright
 *	highlights shimmered as things scrolled).  Instead, the signal is built per phase and low-pass filtered before it is sampled, so
 *	an edge between two samples is carried in their levels and the decoded image just shifts as the object moves.
 *
 * The filter is the 4-phase box that each sample summed before, followed by a 25-tap low-pass (taps -12..12) that passes DC and the
 *	carrier exactly and removes 2-6 times the carrier, so flat areas decode exactly as the band-limited table above, and everything
 *	that would alias at the decoder's rate (above 6.3 MHz) is gone.
 **/
struct LSN_NES_AA_FILTER {
	LSN_NES_AA_FILTER() {
		/* half of the symmetric low-pass, scaled by 4096 (taps 0..12) */
		static const int half[13] = { 918, 846, 622, 342, 83, -96, -171, -154, -83, 0, 61, 86, 53 };
		int low[25];
		for ( int I = 0; I < 25; ++I ) { low[I] = half[(I < 12) ? (12 - I) : (I - 12)]; }
		for ( int I = 0; I < LSN_TAPS; ++I ) {
			taps[I] = 0;
			for ( int J = 0; J < 4; ++J ) {
				if ( I - J >= 0 && I - J < 25 ) { taps[I] += low[I-J]; }
			}
		}
		for ( int P = 0; P < 512; ++P ) {
			for ( int I = 0; I < 12; ++I ) {
				/* the 12-phase signal is kept with 3 fewer bits so that the filter fits in 32 bits */
				tphase[P][I] = (square_sample( P, I ) + 4) >> 3;
			}
		}
	}


	// == Enumerations.
	enum {
		LSN_TAPS														= 28,												/**< The number of taps: the box (4) convolved with the low-pass (25). */
		LSN_FIRST														= -12,												/**< The phase offset of the first tap. */
	};


	// == Members.
	int																	taps[LSN_TAPS];									/**< The filter, scaled by 4096 * 4 (the box sums 4 phases). */
	int																	tphase[512][12];									/**< The signal of each pixel at each phase, divided by 8. */
};

#define NES_OPTIMIZED 1
/* toggle drawing of NES border
 * (normally not in visible region, but it depends on your emulator)
 * highly recommended to keep disabled for better performance if the border
 * is not visible in your emulator.
 */
#define NES_BORDER    0

/* the optimized version is NOT the most optimized version, it just performs
 * some simple refactoring to prevent a few redundant computations
 */
#if NES_OPTIMIZED


/* this function is an optimization
 * basically factoring out the field setup since as long as CRT->analog
 * does not get cleared, all of this should remain the same every update
 */
static void
setup_field(struct CRT *v)
{
    int n;
 
    for (n = 0; n < CRT_VRES; n++) {
        int t; /* time */
        short *line = &v->analog[n * CRT_HRES];
 
        t = LINE_BEG;
 
        /* vertical sync scanlines */
        if LSN_UNLIKELY(n >= 259 && n <= CRT_VRES) {
           while (t < SYNC_BEG) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT); /* FP */
           while (t < PPUpx2pos(327)) line[t++] = SYNC_LEVEL * (1 << CRT_SIG_SHIFT); /* sync separator */
           while (t < CRT_HRES) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT); /* blank */
        } else {
            /* prerender/postrender/video scanlines */
            while (t < SYNC_BEG) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT); /* FP */
            while (t < BW_BEG) line[t++] = SYNC_LEVEL * (1 << CRT_SIG_SHIFT);  /* SYNC */
            while (t < CRT_HRES) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT);
        }
    }
}
 
extern void
crt_modulate_full(struct CRT *v, struct NTSC_SETTINGS *s)
{
    int x, y, xo, yo;
    int destw = AV_LEN;
    int desth = CRT_LINES;
    int n, phase;
    int iccf[3][CRT_CB_FREQ];
    int ccburst[3][CRT_CB_FREQ]; /* color phase for burst */
    int sn, cs;
    static int phasetab[4] = { 0, 4, 8 };
        
    if LSN_UNLIKELY(!s->field_initialized) {
        setup_field(v);
        s->field_initialized = 1;
    }

    for (y = 0; y < 3; y++) {
        xo = (y + s->dot_crawl_offset) * 120;
        for (x = 0; x < CRT_CB_FREQ; x++) {
            n = (s->hue + x * (360 / CRT_CB_FREQ) + xo + 33) % 360;
            crt_sincos14_full(&sn, &cs, n * 8192 / 180);
            ccburst[y][x] = sn;
        }
    }
    xo = AV_BEG;
    yo = CRT_TOP;
         
    /* align signal */
    xo -= (xo % CRT_CB_FREQ);
    
#if NES_BORDER
    for (n = CRT_TOP; n <= (CRT_BOT + 2); n++) {
        int t; /* time */
        short *line = &v->analog[n * CRT_HRES];
        
        t = LINE_BEG;
 
        phase = phasetab[(n + s->dot_crawl_offset) % 3] + 6;
        t = LAV_BEG;
        while (t < CRT_HRES) {
            int ire, p;
            p = s->border_color;
            if (t == LAV_BEG) p = 0xf0;
            ire = BLACK_LEVEL + v->black_point;
            ire += BandLimitedSample(p, phase);
            ire = ((ire * v->white_point / 100) + (1 << (11 - CRT_SIG_SHIFT))) >> (12 - CRT_SIG_SHIFT);
            line[t++] = ire;
            phase += 12 / CRT_CB_FREQ;
        }
    }
#endif
    /* CB_CYCLES of color burst at 3.579545 Mhz on every line outside of vertical sync, not just the picture lines (the decoder
     * also integrates the burst of the lines just above the picture, and a missing burst there made the top of the picture
     * decode with less saturation)
     */
    for (n = 0; n < 259; n++) {
        short *line = &v->analog[n * CRT_HRES];
        int t, cb;
        for (t = CB_BEG; t < CB_BEG + (CB_CYCLES * CRT_CB_FREQ); t++) {
            cb = ccburst[n % 3][t % CRT_CB_FREQ];
            line[t] = (short)(((BLANK_LEVEL << 15) + (cb * BURST_LEVEL) + (1 << (14 - CRT_SIG_SHIFT))) >> (15 - CRT_SIG_SHIFT));
            iccf[n % 3][t % CRT_CB_FREQ] = line[t];
        }
    }
    {
        constexpr int decim = 12 / CRT_CB_FREQ;                 /* phases per sample */
        constexpr int phases = AV_LEN * decim;                  /* phases of active video */
        constexpr int pre = -LSN_NES_AA_FILTER::LSN_FIRST;       /* phases before the active video that the filter reads */
        constexpr int post = LSN_NES_AA_FILTER::LSN_TAPS + LSN_NES_AA_FILTER::LSN_FIRST; /* and after it */
        static const LSN_NES_AA_FILTER aa_filter;
        /* the phases are kept as decim planes (plane r holds phases decim * m + r, counted from pre phases before the active video),
         * so that the filter for consecutive samples reads consecutive values */
        constexpr int planelen = (pre + phases + post) / decim + 8;
        static_assert((pre % decim) == 0, "The filter must start on a sample.");
        int plane[decim][planelen];
        const int black = BLACK_LEVEL + v->black_point;
        
        if (v->pix_w != s->w) {
            /* the first phase of each pixel (the phases of pixel P are those where (phase * w) / phases == P) */
            for (x = 0; x <= s->w; x++) {
                v->pix_start[x] = (x * phases + s->w - 1) / s->w;
            }
            v->pix_w = s->w;
        }
        /* outside of the active video the signal is blank */
        memset(plane, 0, sizeof(plane));
        for (y = 0; y < desth; y++) {
            int sy = (y * s->h) / desth;
            short *dst = &v->analog[xo + (y + yo) * CRT_HRES];
            
            if LSN_UNLIKELY(sy >= s->h) sy = s->h;
            if LSN_UNLIKELY(sy < 0) sy = 0;
 
            sy *= s->w;
            phase = phasetab[(y + yo + s->dot_crawl_offset) % 3] % 12;
            {
                /* walk the phases a pixel at a time */
                int r = 0, m = pre / decim;
                for (x = 0; x < s->w; x++) {
                    const int *row = aa_filter.tphase[s->data[x + sy] & 0x1FF];
                    for (int u = v->pix_start[x]; u < v->pix_start[x + 1]; u++) {
                        plane[r][m] = row[phase];
                        if (++phase == 12) { phase = 0; }
                        if (++r == decim) { r = 0; m++; }
                    }
                }
            }
            /* sum is the 4-phase sum of the signal, scaled by 4096 / 8 */
#define LSN_NES_LEVEL(SUM) (short)((((black + (((SUM) + (1 << 8)) >> 9)) * v->white_point / 100) + (1 << (11 - CRT_SIG_SHIFT))) >> (12 - CRT_SIG_SHIFT))
            x = 0;
#if defined( __AVX2__ )
            if LSN_LIKELY( lsn::CUtilities::IsAvx2Supported() ) {
                __m256i mmtaps[LSN_NES_AA_FILTER::LSN_TAPS];
                for (int k = 0; k < LSN_NES_AA_FILTER::LSN_TAPS; k++) { mmtaps[k] = _mm256_set1_epi32(aa_filter.taps[k]); }
                for (; x + 16 <= destw; x += 16) {
                    /* tap k reads plane k % decim at x + k / decim */
                    __m256i sum0 = _mm256_setzero_si256(), sum1 = _mm256_setzero_si256();
                    for (int r = 0; r < decim; r++) {
                        const int *pl = &plane[r][x];
                        for (int k = r; k < LSN_NES_AA_FILTER::LSN_TAPS; k += decim, pl++) {
                            sum0 = _mm256_add_epi32(sum0, _mm256_mullo_epi32(mmtaps[k], _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pl))));
                            sum1 = _mm256_add_epi32(sum1, _mm256_mullo_epi32(mmtaps[k], _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pl + 8))));
                        }
                    }
                    LSN_ALN int sum[16];
                    _mm256_store_si256(reinterpret_cast<__m256i *>(sum), sum0);
                    _mm256_store_si256(reinterpret_cast<__m256i *>(sum + 8), sum1);
                    for (int j = 0; j < 16; j++) {
                        dst[x + j] = LSN_NES_LEVEL(sum[j]);
                    }
                }
            }
#endif	// #if defined( __AVX2__ )
            for (; x < destw; x++) {
                int sum = 0;
                for (int k = 0; k < LSN_NES_AA_FILTER::LSN_TAPS; k++) {
                    sum += aa_filter.taps[k] * plane[k % decim][x + k / decim];
                }
                dst[x] = LSN_NES_LEVEL(sum);
            }
#undef LSN_NES_LEVEL
        }
    }
    
    for (x = 0; x < CRT_CB_FREQ; x++) {
        for (n = 0; n < 3; n++) {
            v->ccf[n][x] = iccf[n][x] << 7;
        }
    }
    v->cc_period = 3;
}
#else
/* NOT NES_OPTIMIZED */
extern void
crt_modulate_full(struct CRT *v, struct NTSC_SETTINGS *s)
{
    int x, y, xo, yo;
    int destw = AV_LEN;
    int desth = CRT_LINES;
    int n, phase;
    int iccf[3][CRT_CB_FREQ];
    int ccburst[3][CRT_CB_FREQ]; /* color phase for burst */
    int sn, cs;
    static int phasetab[4] = { 0, 4, 8 };

    for (y = 0; y < 3; y++) {
        xo = (y + s->dot_crawl_offset) * 120;
        for (x = 0; x < CRT_CB_FREQ; x++) {
            n = (s->hue + x * (360 / CRT_CB_FREQ) + xo + 33) % 360;
            crt_sincos14_full(&sn, &cs, n * 8192 / 180);
            ccburst[y][x] = sn;
        }
    }
    xo = AV_BEG;
    yo = CRT_TOP;
         
    /* align signal */
    xo -= (xo % CRT_CB_FREQ);
    
    for (n = 0; n < CRT_VRES; n++) {
        int t; /* time */
        short *line = &v->analog[n * CRT_HRES];
        
        t = LINE_BEG;

        /* vertical sync scanlines */
        if (n >= 259 && n <= CRT_VRES) {
           while (t < SYNC_BEG) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT); /* FP */
           while (t < PPUpx2pos(327)) line[t++] = SYNC_LEVEL * (1 << CRT_SIG_SHIFT); /* sync separator */
           while (t < CRT_HRES) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT); /* blank */
        } else {
            int cb;
            /* prerender/postrender/video scanlines */
            while (t < SYNC_BEG) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT); /* FP */
            while (t < BW_BEG) line[t++] = SYNC_LEVEL * (1 << CRT_SIG_SHIFT);  /* SYNC */
            while (t < CB_BEG) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT); /* BW + CB + BP */
            /* CB_CYCLES of color burst at 3.579545 Mhz */
            for (t = CB_BEG; t < CB_BEG + (CB_CYCLES * CRT_CB_FREQ); t++) {
                cb = ccburst[n % 3][t % CRT_CB_FREQ];
                line[t] = (short)(((BLANK_LEVEL << 15) + (cb * BURST_LEVEL) + (1 << (14 - CRT_SIG_SHIFT))) >> (15 - CRT_SIG_SHIFT));
                iccf[n % 3][t % CRT_CB_FREQ] = line[t];
            }
            while (t < LAV_BEG) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT);
#if NES_BORDER
            if (n >= CRT_TOP && n <= (CRT_BOT + 2)) {
                phase = phasetab[(n + s->dot_crawl_offset) % 3] + 6;
                while (t < CRT_HRES) {
                    int ire, p;
                    p = s->border_color;
                    if (t == LAV_BEG) p = 0xf0;
                    ire = BLACK_LEVEL + v->black_point;
                    ire += BandLimitedSample(p, phase);
                    ire = ((ire * v->white_point / 100) + (1 << (11 - CRT_SIG_SHIFT))) >> (12 - CRT_SIG_SHIFT);
                    line[t++] = ire;
                    phase += 12 / CRT_CB_FREQ;
                }
            } else {
#endif
                while (t < CRT_HRES) line[t++] = BLANK_LEVEL * (1 << CRT_SIG_SHIFT);
#if NES_BORDER
            }
#endif
        }
    }

    for (y = 0; y < desth; y++) {
        int sy = (y * s->h) / desth;
        if (sy >= s->h) sy = s->h;
        if (sy < 0) sy = 0;
        
        sy *= s->w;
        phase = phasetab[(y + yo + s->dot_crawl_offset) % 3];
        for (x = 0; x < destw; x++) {
            int ire, p;
            
            p = s->data[((x * s->w) / destw) + sy];
            ire = BLACK_LEVEL + v->black_point;
            ire += BandLimitedSample(p, phase);
            ire = ((ire * v->white_point / 100) + (1 << (11 - CRT_SIG_SHIFT))) >> (12 - CRT_SIG_SHIFT);
            v->analog[(x + xo) + (y + yo) * CRT_HRES] = ire;
            phase += 12 / CRT_CB_FREQ;
        }
    }
    
    for (x = 0; x < CRT_CB_FREQ; x++) {
        for (n = 0; n < 3; n++) {
            v->ccf[n][x] = iccf[n][x] << 7;
        }
    }
    v->cc_period = 3;
}
#endif

#endif
