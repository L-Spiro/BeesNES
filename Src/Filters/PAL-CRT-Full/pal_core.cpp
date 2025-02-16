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
#include "../../Utilities/LSNUtilities.h"

#include <immintrin.h>
#include <stdlib.h>
#include <string.h>

/* ensure negative values for x get properly modulo'd */
#define POSMOD(x, n)     (((x) % (n) + (n)) % (n))

static int sigpsin15[18] = { /* significant points on sine wave (15-bit) */
    0x0000,
    0x0c88,0x18f8,0x2528,0x30f8,0x3c50,0x4718,0x5130,0x5a80,
    0x62f0,0x6a68,0x70e0,0x7640,0x7a78,0x7d88,0x7f60,0x8000,
    0x7f60
};

static int
sintabil8(int n)
{
    int f, i, a, b;
    
    /* looks scary but if you don't change T14_2PI
     * it won't cause out of bounds memory reads
     */
    f = n >> 0 & 0xff;
    i = n >> 8 & 0xff;
    a = sigpsin15[i];
    b = sigpsin15[i + 1];
    return (a + ((b - a) * f >> 8));
}

/* 14-bit interpolated sine/cosine */
extern void
pal_sincos14(int *s, int *c, int n)
{
    int h;
    
    n &= T14_MASK;
    h = n & ((T14_2PI >> 1) - 1);
    
    if (h > ((T14_2PI >> 2) - 1)) {
        *c = -sintabil8(h - (T14_2PI >> 2));
        *s = sintabil8((T14_2PI >> 1) - h);
    } else {
        *c = sintabil8((T14_2PI >> 2) - h);
        *s = sintabil8(h);
    }
    if (n > ((T14_2PI >> 1) - 1)) {
        *c = -*c;
        *s = -*s;
    }
}

extern int
pal_bpp4fmt(int format)
{
    switch (format) {
        case PAL_PIX_FORMAT_RGB: 
        case PAL_PIX_FORMAT_BGR: 
            return 3;
        case PAL_PIX_FORMAT_ARGB:
        case PAL_PIX_FORMAT_RGBA:
        case PAL_PIX_FORMAT_ABGR:
        case PAL_PIX_FORMAT_BGRA:
            return 4;
        default:
            return 0;
    }
}

/*****************************************************************************/
/********************************* FILTERS ***********************************/
/*****************************************************************************/

/* convolution is much faster but the EQ looks softer, more authentic, and more analog */
#define USE_CONVOLUTION 1
#define USE_7_SAMPLE_KERNEL 1
#define USE_6_SAMPLE_KERNEL 0
#define USE_5_SAMPLE_KERNEL 0

#if USE_CONVOLUTION

/* NOT 3 band equalizer, faster convolution instead.
 * eq function names preserved to keep code clean
 */
static struct EQF {
    int h[7];
} eqY, eqU, eqV;

/* params unused to keep the function the same */
static void
init_eq(struct EQF *f,
        int f_lo, int f_hi, int rate,
        int g_lo, int g_mid, int g_hi)
{    
    memset(f, 0, sizeof(struct EQF));
}

static void
reset_eq(struct EQF *f)
{
    memset(f->h, 0, sizeof(f->h));
}

static int
eqf(struct EQF *f, int s)
{
    int i;
    int *h = f->h;

    for (i = 6; i > 0; i--) {
        h[i] = h[i - 1];
    }
    h[0] = s;
#if USE_7_SAMPLE_KERNEL
    /* index : 0 1 2 3 4 5 6 */
    /* weight: 1 4 7 8 7 4 1 */
    return (s + h[6] + ((h[1] + h[5]) * 4) + ((h[2] + h[4]) * 7) + (h[3] * 8)) >> 5;
#elif USE_6_SAMPLE_KERNEL
    /* index : 0 1 2 3 4 5 */
    /* weight: 1 3 4 4 3 1 */
    return (s + h[5] + 3 * (h[1] + h[4]) + 4 * (h[2] + h[3])) >> 4;
#elif USE_5_SAMPLE_KERNEL
    /* index : 0 1 2 3 4 */
    /* weight: 1 2 2 2 1 */
    return (s + h[4] + ((h[1] + h[2] + h[3]) << 1)) >> 3;
#else
    /* index : 0 1 2 3 */
    /* weight: 1 1 1 1*/
    return (s + h[3] + h[1] + h[2]) >> 2;
#endif
}

#else

#define HISTLEN     3
#define HISTOLD     (HISTLEN - 1) /* oldest entry */
#define HISTNEW     0             /* newest entry */

#define EQ_P        16 /* if changed, the gains will need to be adjusted */
#define EQ_R        (1 << (EQ_P - 1)) /* rounding */
/* three band equalizer */
static struct EQF {
    int lf, hf; /* fractions */
    int g[3]; /* gains */
    int fL[4];
    int fH[4];
    int h[HISTLEN]; /* history */
} eqY, eqU, eqV;

/* f_lo - low cutoff frequency
 * f_hi - high cutoff frequency
 * rate - sampling rate
 * g_lo, g_mid, g_hi - gains
 */
static void
init_eq(struct EQF *f,
        int f_lo, int f_hi, int rate,
        int g_lo, int g_mid, int g_hi)
{
    int sn, cs;
    
    memset(f, 0, sizeof(struct EQF));
        
    f->g[0] = g_lo;
    f->g[1] = g_mid;
    f->g[2] = g_hi;
    
    pal_sincos14(&sn, &cs, T14_PI * f_lo / rate);
    if constexpr (EQ_P >= 15) {
        f->lf = 2 * (sn << (EQ_P - 15));
    } else {
        f->lf = 2 * (sn >> (15 - EQ_P));
    }
    pal_sincos14(&sn, &cs, T14_PI * f_hi / rate);
    if constexpr (EQ_P >= 15) {
        f->hf = 2 * (sn << (EQ_P - 15));
    } else {
        f->hf = 2 * (sn >> (15 - EQ_P));
    }
}

static void
reset_eq(struct EQF *f)
{
    memset(f->fL, 0, sizeof(f->fL));
    memset(f->fH, 0, sizeof(f->fH));
    memset(f->h, 0, sizeof(f->h));
}

static int
eqf(struct EQF *f, int s)
{    
    int i, r[3];

    f->fL[0] += (f->lf * (s - f->fL[0]) + EQ_R) >> EQ_P;
    f->fH[0] += (f->hf * (s - f->fH[0]) + EQ_R) >> EQ_P;
    
    for (i = 1; i < 4; i++) {
        f->fL[i] += (f->lf * (f->fL[i - 1] - f->fL[i]) + EQ_R) >> EQ_P;
        f->fH[i] += (f->hf * (f->fH[i - 1] - f->fH[i]) + EQ_R) >> EQ_P;
    }
    
    r[0] = f->fL[3];
    r[1] = f->fH[3] - f->fL[3];
    r[2] = f->h[HISTOLD] - f->fH[3];

    for (i = 0; i < 3; i++) {
        r[i] = (r[i] * f->g[i]) >> EQ_P;
    }
  
    for (i = HISTOLD; i > 0; i--) {
        f->h[i] = f->h[i - 1];
    }
    f->h[HISTNEW] = s;
    
    return (r[0] + r[1] + r[2]);
}

#endif

/*****************************************************************************/
/***************************** PUBLIC FUNCTIONS ******************************/
/*****************************************************************************/

extern void
pal_resize(struct PAL_CRT *v, int w, int h, int f, unsigned char *out)
{    
    v->outw = w;
    v->outh = h;
    v->out_format = f;
    v->out = out;
}

extern void
pal_reset(struct PAL_CRT *v)
{
    v->saturation = 10;
    v->brightness = 0;
    v->contrast = 180;
    v->black_point = 0;
    v->white_point = 100;
    v->hsync = 0;
    v->vsync = 0;
}

extern void
pal_init(struct PAL_CRT *v, int w, int h, int f, unsigned char *out)
{
    memset(v, 0, sizeof(struct PAL_CRT));
    pal_resize(v, w, h, f, out);
    pal_reset(v);
    v->rn = 194;
    
    /* kilohertz to line sample conversion */
#define kHz2L(kHz) (PAL_HRES * (kHz * 100) / L_FREQ)
    
    /* band gains are pre-scaled as 16-bit fixed point
     * if you change the EQ_P define, you'll need to update these gains too
     */
    init_eq(&eqY, kHz2L(1890), kHz2L(3320), PAL_HRES, 65536, 8192, 9175);
    init_eq(&eqU, kHz2L(80),   kHz2L(1320), PAL_HRES, 65536, 65536, 1311);
    init_eq(&eqV, kHz2L(80),   kHz2L(1320), PAL_HRES, 65536, 65536, 1311);
}

/* search windows, in samples */
#define HSYNC_WINDOW 6
#define VSYNC_WINDOW 6

extern void
pal_demodulate(struct PAL_CRT *c, int noise)
{
    struct {
        int y, u, v;
    } outbuf[AV_LEN + 16], *out = outbuf + 8, *yuvA, *yuvB;
    int i, j = 0, line = 0, rn;
    signed char *sig;
    int s = 0;
    int field, ratio;
    int *ccr; /* color carrier signal */
    int huesn, huecs;
    int xnudge = -3, ynudge = 3;
    int bright = c->brightness - (BLACK_LEVEL + c->black_point);
    constexpr int bpp = 4;
	int pitch;
#if PAL_DO_BLOOM
    int prev_e; /* filtered beam energy per scan line */
    int max_e; /* approx maximum energy in a scan line */
#endif
    
    //bpp = pal_bpp4fmt(c->out_format);
    /*if LSN_UNLIKELY(bpp == 0) {
        return;
    }*/
    pitch = c->outw * bpp;
    
    rn = c->rn;
    for (i = 0; i < PAL_INPUT_SIZE; i++) {
        rn = (214019 * rn + 140327895);

        /* signal + noise */
        s = c->analog[i] + (((((rn >> 16) & 0xff) - 0x7f) * noise) >> 8);
        if LSN_UNLIKELY(s >  127) { s =  127; }
        if LSN_UNLIKELY(s < -127) { s = -127; }
        c->inp[i] = (signed char)s;
    }
    c->rn = rn;

    /* Look for vertical sync.
     * 
     * This is done by integrating the signal and
     * seeing if it exceeds a threshold. The threshold of
     * the vertical sync pulse is much higher because the
     * vsync pulse is a lot longer than the hsync pulse.
     * The signal needs to be integrated to lessen
     * the noise in the signal.
     */
    for (i = -VSYNC_WINDOW; i < VSYNC_WINDOW; i++) {
        line = POSMOD(c->vsync + i, PAL_VRES);
        sig = c->inp + line * PAL_HRES;
        s = 0;
        for (j = 0; j < PAL_HRES; j++) {
            s += sig[j];
            /* increase the multiplier to make the vsync
             * more stable when there is a lot of noise
             */
            if LSN_UNLIKELY(s <= (125 * SYNC_LEVEL)) {
                goto vsync_found;
            }
        }
    }
vsync_found:
#if PAL_DO_VSYNC
    c->vsync = line; /* vsync found (or gave up) at this line */
#else
    c->vsync = -3;
#endif
    /* if vsync signal was in second half of line, odd field */
    field = (j > (PAL_HRES / 2));

#if PAL_DO_BLOOM
    max_e = (128 + (noise / 2)) * AV_LEN;
    prev_e = (16384 / 8);
#endif
    /* ratio of output height to active video lines in the signal */
    ratio = (c->outh << 16) / PAL_LINES;
    ratio = (ratio + 32768) >> 16;
    
    field = (field * (ratio / 2));

    for (line = PAL_TOP; line < PAL_BOT; line++) {
        unsigned pos, ln;
        int scanL, scanR, dx;
        int L, R;
        unsigned char *cL, *cR;
        int wave[4];
        int dcu, dcv; /* decoded U, V */
        int xpos, ypos;
        int beg, end;
        int phasealign;
        int odd;
#if PAL_DO_BLOOM
        int line_w;
#endif
  
        beg = (line - PAL_TOP + 0) * (c->outh + c->v_fac) / PAL_LINES + field;
        end = (line - PAL_TOP + 1) * (c->outh + c->v_fac) / PAL_LINES + field;

        if LSN_UNLIKELY(beg >= c->outh) { continue; }
        if LSN_UNLIKELY(end > c->outh) { end = c->outh; }

        /* Look for horizontal sync.
         * See comment above regarding vertical sync.
         */
        ln = (POSMOD(line + c->vsync, PAL_VRES)) * PAL_HRES;
        sig = c->inp + ln + c->hsync;

        s = 0;
        for (i = -HSYNC_WINDOW; i < HSYNC_WINDOW; i++) {
            s += sig[SYNC_BEG + i];
            if LSN_UNLIKELY(s <= (4 * SYNC_LEVEL)) {
                break;
            }
        }
#if PAL_DO_HSYNC
        c->hsync = POSMOD(i + c->hsync, PAL_HRES);
#else
        c->hsync = 0;
#endif
        
        xpos = POSMOD(AV_BEG + c->hsync + xnudge, PAL_HRES);
        ypos = POSMOD(line + c->vsync + ynudge, PAL_VRES);
        pos = xpos + ypos * PAL_HRES;
        
        sig = c->inp + ln + c->hsync;
        odd = 0; /* PAL switch, odd line has SYNC in breezeway, even is blank */
        s = 0;
        for (i = 0; i < 8; i++) {
            s += sig[BW_BEG + i];
            if (s <= (4 * SYNC_LEVEL)) {
                odd = 1;
                break;
            }
        }
        ccr = c->ccf[ypos % c->cc_period];
        sig = c->inp + ln + (c->hsync & ~3);
        for (i = CB_BEG; i < CB_BEG + (CB_CYCLES * PAL_CB_FREQ); i++) {
            int p, n;
            p = ccr[i & 3] * 127 / 128; /* fraction of the previous */
            n = sig[i];                 /* mixed with the new sample */
            ccr[i & 3] = p + n;
        }
 
        phasealign = POSMOD(c->hsync, 4);

        if (!odd) {
            phasealign -= 1;
        }
        odd = odd ? -1 : 1;

        pal_sincos14(&huesn, &huecs, 90 * 8192 / 180 - OFFSET_25Hz(line));
        huesn >>= 7; /* make 8-bit */
        huecs >>= 7;
        
        /* amplitude of carrier = saturation, phase difference = hue */
        dcu = ccr[(phasealign + 1) & 3] - ccr[(phasealign + 3) & 3];
        dcv = ccr[(phasealign + 2) & 3] - ccr[(phasealign + 0) & 3];

        wave[0] = ((dcu * huecs - dcv * huesn) >> 8) * c->saturation;
        wave[1] = ((dcv * huecs + dcu * huesn) >> 8) * c->saturation;
        wave[2] = -wave[0];
        wave[3] = -wave[1];
       
        sig = c->inp + pos;
#if PAL_DO_BLOOM
        s = 0;
        for (i = 0; i < AV_LEN; i++) {
            s += sig[i]; /* sum up the scan line */
        }
        /* bloom emulation */
        prev_e = (prev_e * 123 / 128) + ((((max_e >> 1) - s) << 10) / max_e);
        line_w = (AV_LEN * 112 / 128) + (prev_e >> 9);

        dx = (line_w << 12) / c->outw;
        scanL = ((AV_LEN / 2) - (line_w >> 1) + 8) << 12;
        scanR = (AV_LEN - 1) << 12;
        
        L = (scanL >> 12);
        R = (scanR >> 12);
#else
        dx = ((AV_LEN - 1) << 12) / c->outw;
        scanL = 0;
        scanR = (AV_LEN - 1) << 12;
        L = 0;
        R = AV_LEN;
#endif
        reset_eq(&eqY);
        reset_eq(&eqU);
        reset_eq(&eqV);
        
        for (i = L; i < R; i++) {
            int dmU, dmV;
            int ou, ov;

            dmU = sig[i] * wave[(i + 0) & 3];
            dmV = sig[i] * wave[(i + 3) & 3] * odd;
            if LSN_LIKELY(c->chroma_correction) {
                static struct { int u, v; } delay_line[AV_LEN + 1];
                ou = dmU;
                ov = dmV;
                dmU = (delay_line[i].u + dmU) / 2;
                dmV = (delay_line[i].v + dmV) / 2;
                delay_line[i].u = ou;
                delay_line[i].v = ov;
            }
            out[i].y = eqf(&eqY, sig[i] + bright) << 4;
            out[i + c->chroma_lag].u = eqf(&eqU, dmU >> 9) >> 3;
            out[i + c->chroma_lag].v = eqf(&eqV, dmV >> 9) >> 3;
        }

        cL = c->out + (beg * pitch);
        cR = cL + pitch;
		pos = scanL;

#if defined(__AVX512F__)
		if ( lsn::CUtilities::IsAvx512FSupported() ) {
			// Preload constant vectors in 512-bit registers.
			__m512i vMaskFFF    = _mm512_set1_epi32(0xfff);
			__m512i vConst3     = _mm512_set1_epi32(3);
			__m512i vShift2     = _mm512_set1_epi32(2);
			__m512i vShift14    = _mm512_set1_epi32(14);
			__m512i vConst4669  = _mm512_set1_epi32(4669);
			__m512i vConst1622  = _mm512_set1_epi32(1622);
			__m512i vConst2380  = _mm512_set1_epi32(2380);
			__m512i vConst8311  = _mm512_set1_epi32(8311);
			__m512i vShift12    = _mm512_set1_epi32(12);
			__m512i vShift8     = _mm512_set1_epi32(8);
			__m512i vContrast   = _mm512_set1_epi32(c->contrast);
			__m512i vMax255     = _mm512_set1_epi32(255);

			// Precompute an index vector for dx increments:
			// { 0, dx, 2*dx, …, 15*dx }.
			__m512i idxInc = _mm512_setr_epi32(
				0,      dx,     2*dx,   3*dx,
				4*dx,   5*dx,   6*dx,   7*dx,
				8*dx,   9*dx,   10*dx,  11*dx,
				12*dx,  13*dx,  14*dx,  15*dx
			);

			// Process 16 pixels per iteration.
			for ( ; (int)pos < scanR && cL < cR; pos += dx * 16, cL += 16 * bpp ) {
				// Compute positions for 16 pixels: pos, pos+dx, …, pos+15*dx.
				__m512i vPos = _mm512_add_epi32(_mm512_set1_epi32(pos), idxInc);

				// Compute R = pos & 0xfff and L = 0xfff - R.
				__m512i vR = _mm512_and_epi32(vPos, vMaskFFF);
				__m512i vL = _mm512_sub_epi32(vMaskFFF, vR);

				// Compute sample index s = pos >> 12.
				__m512i vs = _mm512_srli_epi32(vPos, 12);

				// Compute gather indices for YUV.
				// Each YUV sample consists of three ints (y,u,v); for sample s, Y is at index = s*3.
				__m512i indexA = _mm512_mullo_epi32(vs, vConst3);
				__m512i indexB = _mm512_add_epi32(indexA, _mm512_set1_epi32(3));

				// Gather Y, U, V from sample A.
				__m512i A_y = _mm512_i32gather_epi32(indexA, reinterpret_cast<const int*>(out), 4);
				__m512i A_u = _mm512_i32gather_epi32(_mm512_add_epi32(indexA, _mm512_set1_epi32(1)),
													  reinterpret_cast<const int*>(out), 4);
				__m512i A_v = _mm512_i32gather_epi32(_mm512_add_epi32(indexA, _mm512_set1_epi32(2)),
													  reinterpret_cast<const int*>(out), 4);

				// Gather Y, U, V from sample B.
				__m512i B_y = _mm512_i32gather_epi32(indexB, reinterpret_cast<const int*>(out), 4);
				__m512i B_u = _mm512_i32gather_epi32(_mm512_add_epi32(indexB, _mm512_set1_epi32(1)),
													  reinterpret_cast<const int*>(out), 4);
				__m512i B_v = _mm512_i32gather_epi32(_mm512_add_epi32(indexB, _mm512_set1_epi32(2)),
													  reinterpret_cast<const int*>(out), 4);

				// Interpolate between samples:
				// y = ((A_y * L) >> 2) + ((B_y * R) >> 2)
				__m512i yA = _mm512_mullo_epi32(A_y, vL);
				__m512i yB = _mm512_mullo_epi32(B_y, vR);
				__m512i y_vec = _mm512_add_epi32(_mm512_srai_epi32(yA, 2),
												 _mm512_srai_epi32(yB, 2));

				// u = ((A_u * L) >> 14) + ((B_u * R) >> 14)
				__m512i uA = _mm512_mullo_epi32(A_u, vL);
				__m512i uB = _mm512_mullo_epi32(B_u, vR);
				__m512i u_vec = _mm512_add_epi32(_mm512_srai_epi32(uA, 14),
												 _mm512_srai_epi32(uB, 14));

				// v = ((A_v * L) >> 14) + ((B_v * R) >> 14)
				__m512i vA = _mm512_mullo_epi32(A_v, vL);
				__m512i vB = _mm512_mullo_epi32(B_v, vR);
				__m512i v_vec = _mm512_add_epi32(_mm512_srai_epi32(vA, 14),
												 _mm512_srai_epi32(vB, 14));

				// YUV to RGB conversion:
				// r = (((y + 4669*v) >> 12) * contrast) >> 8;
				__m512i r_term = _mm512_add_epi32(y_vec, _mm512_mullo_epi32(v_vec, vConst4669));
				__m512i r_tmp  = _mm512_srai_epi32(r_term, 12);
				__m512i r_vec  = _mm512_srai_epi32(_mm512_mullo_epi32(r_tmp, vContrast), 8);

				// g = (((y - 1622*u - 2380*v) >> 12) * contrast) >> 8;
				__m512i t1     = _mm512_mullo_epi32(u_vec, vConst1622);
				__m512i t2     = _mm512_mullo_epi32(v_vec, vConst2380);
				__m512i g_term = _mm512_sub_epi32(y_vec, _mm512_add_epi32(t1, t2));
				__m512i g_tmp  = _mm512_srai_epi32(g_term, 12);
				__m512i g_vec  = _mm512_srai_epi32(_mm512_mullo_epi32(g_tmp, vContrast), 8);

				// b = (((y + 8311*u) >> 12) * contrast) >> 8;
				__m512i b_term = _mm512_add_epi32(y_vec, _mm512_mullo_epi32(u_vec, vConst8311));
				__m512i b_tmp  = _mm512_srai_epi32(b_term, 12);
				__m512i b_vec  = _mm512_srai_epi32(_mm512_mullo_epi32(b_tmp, vContrast), 8);

				// Clamp negative values to zero.
				__m512i r_sat = _mm512_max_epi32(r_vec, _mm512_setzero_si512());
				__m512i g_sat = _mm512_max_epi32(g_vec, _mm512_setzero_si512());
				__m512i b_sat = _mm512_max_epi32(b_vec, _mm512_setzero_si512());

				r_sat = _mm512_min_epi32(r_sat, vMax255);
				g_sat = _mm512_min_epi32(g_sat, vMax255);
				b_sat = _mm512_min_epi32(b_sat, vMax255);

				if (c->blend) {
					// For blending, also clamp to 255.
					

					// Store computed R, G, B into temporary arrays.
					LSN_ALN int r_arr[16], g_arr[16], b_arr[16];
					_mm512_store_si512((__m512i*)r_arr, r_sat);
					_mm512_store_si512((__m512i*)g_arr, g_sat);
					_mm512_store_si512((__m512i*)b_arr, b_sat);

					int aa, bb;
					for (int k = 0; k < 16; k++) {
						aa = (r_arr[k] << 16) | (g_arr[k] << 8) | b_arr[k];
						bb = (cL[bpp*k+2] << 16) | (cL[bpp*k+1] << 8) | (cL[bpp*k+0]);
						// Blend: average the two colors using the 0xfefeff mask trick.
						bb = (((aa & 0xfefeff) >> 1) + ((bb & 0xfefeff) >> 1));
						cL[bpp*k+0] = static_cast<unsigned char>(bb & 0xff);
						cL[bpp*k+1] = static_cast<unsigned char>((bb >> 8) & 0xff);
						cL[bpp*k+2] = static_cast<unsigned char>((bb >> 16) & 0xff);
					}
				} else {
					// Non-blend branch:
					// Store the 32-bit integers to temporary int arrays.
					LSN_ALN int r_arr[16], g_arr[16], b_arr[16];
					_mm512_store_si512((__m512i*)r_arr, r_sat);
					_mm512_store_si512((__m512i*)g_arr, g_sat);
					_mm512_store_si512((__m512i*)b_arr, b_sat);

					// Write out the final 16 pixels by casting each int (in [0,255]) to uint8_t.
					for (int k = 0; k < 16; k++) {
						cL[bpp*k+0] = static_cast<unsigned char>(b_arr[k]);
						cL[bpp*k+1] = static_cast<unsigned char>(g_arr[k]);
						cL[bpp*k+2] = static_cast<unsigned char>(r_arr[k]);
					}

				}
			}
		}
#endif  // __AVX512F__


#if defined(__AVX2__)
		if LSN_LIKELY( lsn::CUtilities::IsAvx2Supported() ) {
			// Preload constant vectors.
			__m256i vMaskFFF    = _mm256_set1_epi32(0xfff);
			__m256i vConst3     = _mm256_set1_epi32(3);
			__m256i vShift2     = _mm256_set1_epi32(2);
			__m256i vShift14    = _mm256_set1_epi32(14);
			__m256i vConst4669  = _mm256_set1_epi32(4669);
			__m256i vConst1622  = _mm256_set1_epi32(1622);
			__m256i vConst2380  = _mm256_set1_epi32(2380);
			__m256i vConst8311  = _mm256_set1_epi32(8311);
			__m256i vShift12    = _mm256_set1_epi32(12);
			__m256i vShift8     = _mm256_set1_epi32(8);
			__m256i vContrast   = _mm256_set1_epi32(c->contrast);
			__m256i vMax255     = _mm256_set1_epi32(255);

			// Precompute an index vector for dx increments:
			// { 0, dx, 2*dx, …, 7*dx }.
			__m256i idxInc = _mm256_setr_epi32(0, dx, 2*dx, 3*dx, 4*dx, 5*dx, 6*dx, 7*dx);

			// Process eight pixels per iteration.
			for ( ; (int)pos < scanR && cL < cR; pos += dx * 8, cL += 8 * bpp ) {
				// Compute the positions for eight pixels: pos, pos+dx, …, pos+7*dx.
				__m256i vPos = _mm256_add_epi32(_mm256_set1_epi32(pos), idxInc);

				// Compute R = pos & 0xfff and L = 0xfff - R.
				__m256i vR = _mm256_and_si256(vPos, vMaskFFF);
				__m256i vL = _mm256_sub_epi32(vMaskFFF, vR);

				// Compute sample index s = pos >> 12.
				__m256i vs = _mm256_srli_epi32(vPos, 12);

				// Compute gather indices for YUV.
				// Each YUV sample consists of three ints (y,u,v) in memory.
				// For sample s, the Y is at index = s*3.
				__m256i indexA = _mm256_mullo_epi32(vs, vConst3);
				__m256i indexB = _mm256_add_epi32(indexA, _mm256_set1_epi32(3));

				// Gather Y, U, and V components from sample A.
				__m256i A_y = _mm256_i32gather_epi32(reinterpret_cast<const int*>(out), indexA, 4);
				__m256i A_u = _mm256_i32gather_epi32(reinterpret_cast<const int*>(out),
													 _mm256_add_epi32(indexA, _mm256_set1_epi32(1)), 4);
				__m256i A_v = _mm256_i32gather_epi32(reinterpret_cast<const int*>(out),
													 _mm256_add_epi32(indexA, _mm256_set1_epi32(2)), 4);

				// Gather Y, U, and V components from sample B.
				__m256i B_y = _mm256_i32gather_epi32(reinterpret_cast<const int*>(out), indexB, 4);
				__m256i B_u = _mm256_i32gather_epi32(reinterpret_cast<const int*>(out),
													 _mm256_add_epi32(indexB, _mm256_set1_epi32(1)), 4);
				__m256i B_v = _mm256_i32gather_epi32(reinterpret_cast<const int*>(out),
													 _mm256_add_epi32(indexB, _mm256_set1_epi32(2)), 4);

				// Interpolate between samples:
				// y = ((A_y * L) >> 2) + ((B_y * R) >> 2)
				__m256i yA = _mm256_mullo_epi32(A_y, vL);
				__m256i yB = _mm256_mullo_epi32(B_y, vR);
				__m256i y_vec = _mm256_add_epi32(_mm256_srai_epi32(yA, 2),
												 _mm256_srai_epi32(yB, 2));

				// u = ((A_u * L) >> 14) + ((B_u * R) >> 14)
				__m256i uA = _mm256_mullo_epi32(A_u, vL);
				__m256i uB = _mm256_mullo_epi32(B_u, vR);
				__m256i u_vec = _mm256_add_epi32(_mm256_srai_epi32(uA, 14),
												 _mm256_srai_epi32(uB, 14));

				// v = ((A_v * L) >> 14) + ((B_v * R) >> 14)
				__m256i vA = _mm256_mullo_epi32(A_v, vL);
				__m256i vB = _mm256_mullo_epi32(B_v, vR);
				__m256i v_vec = _mm256_add_epi32(_mm256_srai_epi32(vA, 14),
												 _mm256_srai_epi32(vB, 14));

				// YUV to RGB conversion:
				// r = (((y + 4669*v) >> 12) * contrast) >> 8;
				__m256i r_term = _mm256_add_epi32(y_vec, _mm256_mullo_epi32(v_vec, vConst4669));
				__m256i r_tmp  = _mm256_srai_epi32(r_term, 12);
				__m256i r_vec  = _mm256_srai_epi32(_mm256_mullo_epi32(r_tmp, vContrast), 8);

				// g = (((y - 1622*u - 2380*v) >> 12) * contrast) >> 8;
				__m256i t1     = _mm256_mullo_epi32(u_vec, vConst1622);
				__m256i t2     = _mm256_mullo_epi32(v_vec, vConst2380);
				__m256i g_term = _mm256_sub_epi32(y_vec, _mm256_add_epi32(t1, t2));
				__m256i g_tmp  = _mm256_srai_epi32(g_term, 12);
				__m256i g_vec  = _mm256_srai_epi32(_mm256_mullo_epi32(g_tmp, vContrast), 8);

				// b = (((y + 8311*u) >> 12) * contrast) >> 8;
				__m256i b_term = _mm256_add_epi32(y_vec, _mm256_mullo_epi32(u_vec, vConst8311));
				__m256i b_tmp  = _mm256_srai_epi32(b_term, 12);
				__m256i b_vec  = _mm256_srai_epi32(_mm256_mullo_epi32(b_tmp, vContrast), 8);

				// First, clamp any negative results to zero.
				__m256i r_sat = _mm256_max_epi32(r_vec, _mm256_setzero_si256());
				__m256i g_sat = _mm256_max_epi32(g_vec, _mm256_setzero_si256());
				__m256i b_sat = _mm256_max_epi32(b_vec, _mm256_setzero_si256());

				r_sat = _mm256_min_epi32(r_sat, vMax255);
				g_sat = _mm256_min_epi32(g_sat, vMax255);
				b_sat = _mm256_min_epi32(b_sat, vMax255);

				if LSN_LIKELY(c->blend) {
					// Blend branch: store computed R, G, B into temporary arrays,
					// then blend with the existing destination pixels.
					LSN_ALN int r_arr[8], g_arr[8], b_arr[8];
					_mm256_store_si256((__m256i*)r_arr, r_sat);
					_mm256_store_si256((__m256i*)g_arr, g_sat);
					_mm256_store_si256((__m256i*)b_arr, b_sat);

					int aa, bb;
					for (int k = 0; k < 8; k++) {
						aa = (r_arr[k] << 16) | (g_arr[k] << 8) | b_arr[k];

						bb = (cL[bpp*k+2] << 16) | (cL[bpp*k+1] << 8) | (cL[bpp*k+0]);

						// Blend: average the two colors using the 0xfefeff mask trick.
						bb = (((aa & 0xfefeff) >> 1) + ((bb & 0xfefeff) >> 1));

						cL[bpp*k+0] = static_cast<unsigned char>(bb & 0xff);
						cL[bpp*k+1] = static_cast<unsigned char>((bb >> 8) & 0xff);
						cL[bpp*k+2] = static_cast<unsigned char>((bb >> 16) & 0xff);
					}
				} else {
					// Non-blend branch:
					// Blend branch: store computed R, G, B into temporary arrays,
					// then blend with the existing destination pixels.
					LSN_ALN int r_arr[8], g_arr[8], b_arr[8];
					_mm256_store_si256((__m256i*)r_arr, r_sat);
					_mm256_store_si256((__m256i*)g_arr, g_sat);
					_mm256_store_si256((__m256i*)b_arr, b_sat);

					for (int k = 0; k < 8; k++) {
						cL[bpp*k+0] = (unsigned char)b_arr[k];
						cL[bpp*k+1] = (unsigned char)g_arr[k];
						cL[bpp*k+2] = (unsigned char)r_arr[k];
					}
				}
			}
		}
#endif  // __AVX2__


        for (; (int)pos < scanR && cL < cR; pos += dx) {
            int y, u, v;
            int r, g, b;
            int aa, bb;

            R = pos & 0xfff;
            L = 0xfff - R;
            s = pos >> 12;
            
            yuvA = out + s;
            yuvB = out + s + 1;
            
            /* interpolate between samples if needed */
            y = ((yuvA->y * L) >>  2) + ((yuvB->y * R) >>  2);
            u = ((yuvA->u * L) >> 14) + ((yuvB->u * R) >> 14);
            v = ((yuvA->v * L) >> 14) + ((yuvB->v * R) >> 14);

            /* YUV to RGB */
            r = (((y + 4669 * v) >> 12) * c->contrast) >> 8;
            g = (((y - 1622 * u - 2380 * v) >> 12) * c->contrast) >> 8;
            b = (((y + 8311 * u) >> 12) * c->contrast) >> 8;

            if LSN_UNLIKELY(r < 0) r = 0;
            if LSN_UNLIKELY(g < 0) g = 0;
            if LSN_UNLIKELY(b < 0) b = 0;
            if LSN_UNLIKELY(r > 255) r = 255;
            if LSN_UNLIKELY(g > 255) g = 255;
            if LSN_UNLIKELY(b > 255) b = 255;

            if LSN_LIKELY(c->blend) {
                aa = (r << 16 | g << 8 | b);

				bb = cL[2] << 16 | cL[1] << 8 | cL[0];
                /*switch (c->out_format) {
                    case PAL_PIX_FORMAT_RGB:
                    case PAL_PIX_FORMAT_RGBA:
                        bb = cL[0] << 16 | cL[1] << 8 | cL[2];
                        break;
                    case PAL_PIX_FORMAT_BGR: 
                    case PAL_PIX_FORMAT_BGRA:
                        bb = cL[2] << 16 | cL[1] << 8 | cL[0];
                        break;
                    case PAL_PIX_FORMAT_ARGB:
                        bb = cL[1] << 16 | cL[2] << 8 | cL[3];
                        break;
                    case PAL_PIX_FORMAT_ABGR:
                        bb = cL[3] << 16 | cL[2] << 8 | cL[1];
                        break;
                    default:
                        bb = 0;
                        break;
                }*/

                /* blend with previous color there */
                bb = (((aa & 0xfefeff) >> 1) + ((bb & 0xfefeff) >> 1));
#ifdef _WIN32
				cL[0] = bb >>  0 & 0xff;
				cL[1] = bb >>  8 & 0xff;
				cL[2] = bb >> 16 & 0xff;
#else 

				switch (c->out_format) {
					case PAL_PIX_FORMAT_RGB:
					case PAL_PIX_FORMAT_RGBA:
						cL[0] = bb >> 16 & 0xff;
						cL[1] = bb >>  8 & 0xff;
						cL[2] = bb >>  0 & 0xff;
						break;
					case PAL_PIX_FORMAT_BGR: 
					case PAL_PIX_FORMAT_BGRA:
						cL[0] = bb >>  0 & 0xff;
						cL[1] = bb >>  8 & 0xff;
						cL[2] = bb >> 16 & 0xff;
						break;
					case PAL_PIX_FORMAT_ARGB:
						cL[1] = bb >> 16 & 0xff;
						cL[2] = bb >>  8 & 0xff;
						cL[3] = bb >>  0 & 0xff;
						break;
					case PAL_PIX_FORMAT_ABGR:
						cL[1] = bb >>  0 & 0xff;
						cL[2] = bb >>  8 & 0xff;
						cL[3] = bb >> 16 & 0xff;
						break;
					default:
						break;
				}
#endif	// #ifdef _WIN32
            } else {
				cL[0] = (unsigned char)b;
				cL[1] = (unsigned char)g;
				cL[2] = (unsigned char)r;
                //bb = (r << 16 | g << 8 | b);
            }


			cL += bpp;
        }
        
        /* duplicate extra lines */
        for (s = beg + 1; s < (end - c->scanlines); s++) {
            memcpy(c->out + s * pitch, c->out + (s - 1) * pitch, pitch);
        }
    }
}
