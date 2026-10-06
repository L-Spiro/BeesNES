/*****************************************************************************/
/*
 * PAL/CRT - integer-only PAL video signal encoding / decoding emulation
 *
 *   by EMMIR 2018-2023
 *
 *   GitHub : https://github.com/LMP88959/PAL-CRT
 *   YouTube: https://www.youtube.com/@EMMIR_KC/videos
 *   Discord: https://discord.com/invite/hdYctSmyQJ
 */
/*****************************************************************************/

#include "pal_core.h"

#if (PAL_SYSTEM == PAL_SYSTEM_NES)
#include <stdlib.h>
#include <string.h>
#include <cmath>

/*  in PAL, the red and green emphasis bits swap meaning
 *  when compared to the NTSC version.
 *  If your emulator does this swapping already, set this to 0.
 *  Otherwise, set to 1.
 */
#define SWAP_RED_GREEN_EMPHASIS_BITS 0

/* Use UA6538 voltages or RP2C07 */
#define UA6538 0

/* generate the square wave for a given 9-bit pixel and phase
 */
static int
square_sample(int p, int phase, int alter, int ua6538)
{
    /*  white = 1450
     *  black = 0
     *  IREmax = 110
     *    
     *  ampIRE = round((mV / (white - black)) * 1024 * IREmax) 
     */
    /* From HardWareMan's UA6538 voltage measurements
     * RELATIVE TO BLACK (mV)
     *        LOW/EMPH          HIGH/EMPH
     *  L0   -125 / -208         450 /  234
     *  L1      0 / -116         934 /  600
     *  L2    350 /  159        1450 / 1009
     *  L3    975 /  634        1450 / 1009
     *  
     */
    static int IRE_UA6538[16] = {
     /* 0d     1d     2d      3d */
       -9710,  0,     27189,  75741,
     /* 0d     1d     2d      3d emphasized */
       -16158,-9011,  12352,  49251,
     /* 00     10     20      30 */
        34957, 72556, 112640, 112640,
     /* 00     10     20      30 emphasized */
        18178, 46610, 78382,  78382
    };
    /*  white = 1467
     *  black = 0
     *  IREmax = 110
     *    
     *  ampIRE = round((mV / (white - black)) * 1024 * IREmax) 
     */
    /* From HardWareMan's RP2C07 voltage measurements
     * RELATIVE TO BLACK (mV)
     *        LOW/EMPH          HIGH/EMPH
     *  L0   -175 / -266         609 /  334
     *  L1      0 / -133        1000 /  642
     *  L2    475 /  225        1467 / 1000
     *  L3   1067 /  700        1467 / 1000
     */
    static int IRE[16] = {
     /* 0d     1d     2d      3d */
       -13437, 0,     36472,  81927,
     /* 0d     1d     2d      3d emphasized */
       -20424,-10212, 17276,  53748,
     /* 00     10     20      30 */
        46761, 76783, 112640, 112640,
     /* 00     10     20      30 emphasized */
        25645, 49294, 76783,  76783
    };
#if SWAP_RED_GREEN_EMPHASIS_BITS
    /* red 0200, green 0100, blue 0400 */
    static int active[6] = {
        0300, 0200,
        0600, 0400,
        0500, 0100
    };
#else
    /* red 0100, green 0200, blue 0400 */
    static int active[6] = {
        0300, 0100,
        0500, 0400,
        0600, 0200
    };
#endif
    int hue, ohue;
    int e, l, v;

    hue = (p & 0x0f);

    /* last two columns are black */
    if (hue >= 0x0e) {
        return 0;
    }
    ohue = hue;
    if (alter) {
        hue = 0x0d - ((p + 2) & 0x0f);
        /* hue 0x0c maps to -1, which must wrap to 11 (C's % keeps the sign, which made phase 0 high) */
        if (hue < 0) {
            hue += 12;
        }
    }
    v = (((hue + phase) % 12) < 6);

    e = (((p & 0700) & active[(phase >> 1) % 6]) > 0);
    switch (ohue) {
        case 0x00: l = 1; break;
        case 0x0d: l = 0; break;
        default:   l = v; break;
    }

    if (ua6538) {
        return IRE_UA6538[(l << 3) + (e << 2) + ((p >> 4) & 3)];
    } else {
        return IRE[(l << 3) + (e << 2) + ((p >> 4) & 3)];
    }
}

/**
 * The table of band-limited signal samples, indexed by [UA6538][alternate line][9-bit pixel][phase % 12].
 * 
 * Each sample sums 6 of the 12 square-wave phases but samples are only 3 phases apart, and the line-to-line phase offsets (multiples of 2)
 *	are not multiples of 3, so sampling the raw square wave aliases its upper harmonics onto the color carrier differently on each line phase.
 *	That gave the same color a different hue and saturation on each line phase (visible as horizontal lines across flat areas).  Keeping
 *	only the DC and fundamental (all that a TV passes in the chroma band anyway) makes every line phase decode to the same color.
 **/
struct LSN_PAL_NES_SIGNAL_TABLE {
	LSN_PAL_NES_SIGNAL_TABLE() {
		constexpr double dPi = 3.14159265358979323846;
		for ( int U = 0; U < 2; ++U ) {
			for ( int A = 0; A < 2; ++A ) {
				for ( int P = 0; P < 512; ++P ) {
					double dBox[12];
					double dDc = 0.0, dRe = 0.0, dIm = 0.0;
					for ( int I = 0; I < 12; ++I ) {
						dBox[I] = 0.0;
						for ( int J = 0; J < 6; ++J ) {
							dBox[I] += double( square_sample( P, I + J, A, U ) );
						}
						dDc += dBox[I];
						dRe += dBox[I] * std::cos( 2.0 * dPi * I / 12.0 );
						dIm += dBox[I] * std::sin( 2.0 * dPi * I / 12.0 );
					}
					dDc /= 12.0;
					dRe *= 2.0 / 12.0;
					dIm *= 2.0 / 12.0;
					for ( int I = 0; I < 12; ++I ) {
						iSample[U][A][P][I] = int( std::lround( dDc + dRe * std::cos( 2.0 * dPi * I / 12.0 ) + dIm * std::sin( 2.0 * dPi * I / 12.0 ) ) );
					}
				}
			}
		}
	}


	// == Members.
	int																	iSample[2][2][512][12];								/**< The band-limited sum of 6 square-wave phases for each pixel and starting phase. */
};

/**
 * Gets the band-limited sum of 6 square-wave phases for a given pixel and starting phase.
 * 
 * \param _iP The 9-bit pixel.
 * \param _iPhase The starting phase.
 * \param _iAlter Non-zero on alternate (V-switched) lines.
 * \param _iUa6538 Non-zero to use UA6538 voltages.
 * \return Returns the band-limited sum of square_sample( _iP, _iPhase + 0, ... ) through square_sample( _iP, _iPhase + 5, ... ).
 **/
static inline int
BandLimitedSample( int _iP, int _iPhase, int _iAlter, int _iUa6538 )
{
	static const LSN_PAL_NES_SIGNAL_TABLE pnstTable;
	return pnstTable.iSample[_iUa6538?1:0][_iAlter?1:0][_iP&0x1FF][_iPhase%12];
}

/* this function is an optimization
 * basically factoring out the field setup since as long as PAL_CRT->analog
 * does not get cleared, all of this should remain the same every update
 */
static void
setup_field(struct PAL_CRT *v, struct PAL_SETTINGS *s)
{
    int n, y;
    
    for (y = 0; y < 6; y++) {
        s->altline[y] = ((y & 1) ? 1 : -1);
    };
 
    for (n = 0; n < PAL_VRES; n++) {
        int t; /* time */
        short *line = &v->analog[n * PAL_HRES];
 
        t = LINE_BEG;

        if (n <= 3 || (n >= 7 && n <= 9)) {
            /* equalizing pulses - small blips of sync, mostly blank */
            while (t < (4   * PAL_HRES / 100)) line[t++] = SYNC_LEVEL * (1 << PAL_SIG_SHIFT);
            while (t < (50  * PAL_HRES / 100)) line[t++] = BLANK_LEVEL * (1 << PAL_SIG_SHIFT);
            while (t < (54  * PAL_HRES / 100)) line[t++] = SYNC_LEVEL * (1 << PAL_SIG_SHIFT);
            while (t < (100 * PAL_HRES / 100)) line[t++] = BLANK_LEVEL * (1 << PAL_SIG_SHIFT);
        } else if (n >= 4 && n <= 6) {
            int offs[4] = { 46, 50, 96, 100 };
            /* vertical sync pulse - small blips of blank, mostly sync */
            while (t < (offs[0] * PAL_HRES / 100)) line[t++] = SYNC_LEVEL * (1 << PAL_SIG_SHIFT);
            while (t < (offs[1] * PAL_HRES / 100)) line[t++] = BLANK_LEVEL * (1 << PAL_SIG_SHIFT);
            while (t < (offs[2] * PAL_HRES / 100)) line[t++] = SYNC_LEVEL * (1 << PAL_SIG_SHIFT);
            while (t < (offs[3] * PAL_HRES / 100)) line[t++] = BLANK_LEVEL * (1 << PAL_SIG_SHIFT);
        } else {
            /* prerender/postrender/video scanlines */
            while (t < SYNC_BEG) line[t++] = BLANK_LEVEL * (1 << PAL_SIG_SHIFT); /* FP */
            while (t < BW_BEG) line[t++] = SYNC_LEVEL * (1 << PAL_SIG_SHIFT); /* SYNC */
            while (t < PAL_HRES) line[t++] = BLANK_LEVEL * (1 << PAL_SIG_SHIFT);
            if (s->altline[n & 3] == -1) {
                for (t = BW_BEG; t < CB_BEG; t++) {
                    line[t] = SYNC_LEVEL * (1 << PAL_SIG_SHIFT);
                }
            } else {
                for (t = BW_BEG; t < CB_BEG; t++) {
                    line[t] = BLANK_LEVEL * (1 << PAL_SIG_SHIFT);
                }
            }
        }
    }
}
 
extern void
pal_modulate(struct PAL_CRT *v, struct PAL_SETTINGS *s)
{
    int x, y, xo, yo;
    int destw = AV_LEN;
    int desth = PAL_LINES;
    int n, phase;
    int iccf[6][4];
    int ccburst[6][4]; /* color phase for burst */
    int sn, cs;
    int alter = 0;
        
    if (!s->field_initialized) {
        setup_field(v, s);
        s->field_initialized = 1;
    }
    for (y = 0; y < 6; y++) {
        int vert = y * (360 / 6);
        for (x = 0; x < 4; x++) {
            int ang;
            n = vert + x * 90 + 120;
            ang = n + (s->altline[y] * 45) + (s->altline[y] * s->hue);
            /* swinging burst */
            pal_sincos14(&sn, &cs, ang * 8192 / 180);
            ccburst[y][x] = sn;
        }
    }

    xo = AV_BEG  + s->xoffset;
    yo = PAL_TOP + s->yoffset;
       
    /* align signal */
    xo = (xo & ~3);
    /* color burst on every line outside of vertical sync, not just the picture lines (the decoder also integrates the burst of
     * the lines just above the picture, and a missing burst there made the top of the picture decode lines of differing
     * saturation and hue)
     */
    for (n = 10; n < PAL_VRES; n++) {
        short *line = &v->analog[n * PAL_HRES];
        int t, cb, nm6 = n % 6;
        for (t = CB_BEG; t < CB_BEG + (CB_CYCLES * PAL_CB_FREQ); t++) {
            cb = ccburst[nm6][t & 3];
            line[t] = (short)(((BLANK_LEVEL << 15) + (cb * BURST_LEVEL) + (1 << (14 - PAL_SIG_SHIFT))) >> (15 - PAL_SIG_SHIFT));
            iccf[nm6][t & 3] = line[t];
        }
    }
    /* no border on PAL according to https://www.nesdev.org/wiki/PAL_video */
    for (y = 0; y < desth; y++) {
        int nm6;
        int sy = (y * s->h) / desth;

        if (sy >= s->h) sy = s->h;
        if (sy < 0) sy = 0;
 
        n = (y + yo);
        nm6 = n % 6;

        sy *= s->w;

        phase = nm6 * 2;
        alter = s->altline[nm6] == -1;
        phase += alter ? 0 : 6;
		for (x = 0; x < destw; x++) {
            int ire, p;

            p = s->data[((x * s->w) / destw) + sy];
            ire = BLACK_LEVEL + v->black_point;

            ire += BandLimitedSample(p, phase, alter, s->ua6538);
            ire = ((ire * v->white_point / (110 * 6)) + (1 << (9 - PAL_SIG_SHIFT))) >> (10 - PAL_SIG_SHIFT);
            v->analog[(x + xo) + n * PAL_HRES] = (short)ire;
            phase += 3;
        }

        /*for (x = 0; x < destw; x++) {
            int ire, p;
            
            p = s->data[((x * s->w) / destw) + sy];
            ire = BLACK_LEVEL + v->black_point;
            
            ire += square_sample(p, phase + 0, alter, s->ua6538);
            ire += square_sample(p, phase + 1, alter, s->ua6538);
            ire += square_sample(p, phase + 2, alter, s->ua6538);
            ire += square_sample(p, phase + 3, alter, s->ua6538);
            ire = (ire * v->white_point / 110) >> 12;
            v->analog[(x + xo) + n * PAL_HRES] = (char)ire;
            phase += 3;
        }*/
    }
   
    for (x = 0; x < 4; x++) {
        for (n = 0; n < 6; n++) {
            v->ccf[n][x] = iccf[(n + 3) % 6][x] << 7;
        }
    }
    v->cc_period = 6;
}

#endif
