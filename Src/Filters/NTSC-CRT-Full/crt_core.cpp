/*****************************************************************************/
/*
 * NTSC/CRT - integer-only NTSC video signal encoding / decoding emulation
 * 
 *   by EMMIR 2018-2023
 *   
 *   YouTube: https://www.youtube.com/@EMMIR_KC/videos
 *   Discord: https://discord.com/invite/hdYctSmyQJ
 */
/*****************************************************************************/
#include "crt_core.h"

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
crt_sincos14_full(int *s, int *c, int n)
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
crt_bpp4fmt(int format)
{
    switch (format) {
        case CRT_PIX_FORMAT_RGB: 
        case CRT_PIX_FORMAT_BGR: 
            return 3;
        case CRT_PIX_FORMAT_ARGB:
        case CRT_PIX_FORMAT_RGBA:
        case CRT_PIX_FORMAT_ABGR:
        case CRT_PIX_FORMAT_BGRA:
            return 4;
        default:
            return 0;
    }
}

/*****************************************************************************/
/********************************* FILTERS ***********************************/
/*****************************************************************************/

/* convolution is much faster but the EQ looks softer, more authentic, and more analog */
#define USE_CONVOLUTION 0
//#define USE_7_SAMPLE_KERNEL 0
//#define USE_6_SAMPLE_KERNEL 0
//#define USE_5_SAMPLE_KERNEL 1

#if USE_CONVOLUTION

/* NOT 3 band equalizer, faster convolution instead.
 * eq function names preserved to keep code clean
 */
static struct EQF {
    int h[7];
} eqY, eqI, eqQ;

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
#if USE_7_SAMPLE
    /* index : 0 1 2 3 4 5 6 */
    /* weight: 1 4 7 8 7 4 1 */
    return (s + h[6] + ((h[1] + h[5]) * 4) + ((h[2] + h[4]) * 7) + (h[3] * 8)) >> 5;
#elif USE_6_SAMPLE
    /* index : 0 1 2 3 4 5 */
    /* weight: 1 3 4 4 3 1 */
    return (s + h[5] + 3 * (h[1] + h[4]) + 4 * (h[2] + h[3])) >> 4;
#elif USE_5_SAMPLE
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
} eqY, eqI, eqQ;

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
    
    crt_sincos14_full(&sn, &cs, T14_PI * f_lo / rate);
    if constexpr (EQ_P >= 15) {
        f->lf = 2 * (sn << (EQ_P - 15));
    } else {
        f->lf = 2 * (sn >> (15 - EQ_P));
    }
    crt_sincos14_full(&sn, &cs, T14_PI * f_hi / rate);
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
crt_resize_full(struct CRT *v, int w, int h, int f, unsigned char *out)
{    
    v->outw = w;
    v->outh = h;
    v->out_format = f;
    v->out = out;
}

extern void
crt_reset_full(struct CRT *v)
{
    v->hue = 0;
    v->saturation = 10;
    v->brightness = 0;
    v->contrast = 180;
    v->black_point = 0;
    v->white_point = 100;
    v->hsync = 0;
    v->vsync = 0;
}

extern void
crt_init_full(struct CRT *v, int w, int h, int f, unsigned char *out)
{
    memset(v, 0, sizeof(struct CRT));
    crt_resize_full(v, w, h, f, out);
    crt_reset_full(v);
    v->rn = 194;
    
    /* kilohertz to line sample conversion */
#define kHz2L(kHz) (CRT_HRES * (kHz * 100) / L_FREQ)
    
    /* band gains are pre-scaled as 16-bit fixed point
     * if you change the EQ_P define, you'll need to update these gains too
     */
    init_eq(&eqY, kHz2L(1500), kHz2L(3000), CRT_HRES, 65536, 8192, 9175);
    init_eq(&eqI, kHz2L(80),   kHz2L(1150), CRT_HRES, 65536, 65536, 1311);
    init_eq(&eqQ, kHz2L(80),   kHz2L(1000), CRT_HRES, 65536, 65536, 0);
}

/* search windows, in samples */
#define HSYNC_WINDOW 6
#define VSYNC_WINDOW 6

extern void
crt_demodulate_full(struct CRT *v, int noise)
{
    struct {
        int y, i, q;
    } out[AV_LEN + 1], *yiqA, *yiqB;
    int i, j = 0, line = 0, rn;
    signed char *sig;
    int s = 0;
    int field, ratio;
    int *ccr; /* color carrier signal */
    int huesn, huecs;
    int xnudge = -3, ynudge = 3;
    int bright = v->brightness - (BLACK_LEVEL + v->black_point);
    int bpp, pitch;
#if CRT_DO_BLOOM
    int prev_e; /* filtered beam energy per scan line */
    int max_e; /* approx maximum energy in a scan line */
#endif
    
    bpp = crt_bpp4fmt(v->out_format);
    if (bpp == 0) {
        return;
    }
    pitch = v->outw * bpp;
    
    crt_sincos14_full(&huesn, &huecs, ((v->hue % 360) + 33) * 8192 / 180);
    huesn >>= 11; /* make 4-bit */
    huecs >>= 11;

    rn = v->rn;
    for (i = 0; i < CRT_INPUT_SIZE; i++) {
        rn = (214019 * rn + 140327895);

        /* signal + noise */
        s = v->analog[i] + (((((rn >> 16) & 0xff) - 0x7f) * noise) >> 8);
        if LSN_UNLIKELY(s >  127) { s =  127; }
        if LSN_UNLIKELY(s < -127) { s = -127; }
        v->inp[i] = (signed char)s;
    }
    v->rn = rn;

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
        line = POSMOD(v->vsync + i, CRT_VRES);
        sig = v->inp + line * CRT_HRES;
        s = 0;
        for (j = 0; j < CRT_HRES; j++) {
            s += sig[j];
            /* increase the multiplier to make the vsync
             * more stable when there is a lot of noise
             */
            if (s <= (94 * SYNC_LEVEL)) {
                goto vsync_found;
            }
        }
    }
vsync_found:
	if LSN_LIKELY( v->do_vsync ) {
		v->vsync = line; /* vsync found (or gave up) at this line */
	}
	else {
		v->vsync = -3;
	}
//#if CRT_DO_VSYNC
//    v->vsync = line; /* vsync found (or gave up) at this line */
//#else
//    v->vsync = -3;
//#endif
    /* if vsync signal was in second half of line, odd field */
    field = (j > (CRT_HRES / 2));
#if CRT_DO_BLOOM
    max_e = (128 + (noise / 2)) * AV_LEN;
    prev_e = (16384 / 8);
#endif
    /* ratio of output height to active video lines in the signal */
    ratio = (v->outh << 16) / CRT_LINES;
    ratio = (ratio + 32768) >> 16;
    
    field = (field * (ratio / 2));

    for (line = CRT_TOP; line < CRT_BOT; line++) {
        unsigned pos, ln;
        int scanL, scanR, dx;
        int L, R;
        unsigned char *cL, *cR;
        int wave[4];
        int dci, dcq; /* decoded I, Q */
        int xpos, ypos;
        int beg, end;
        int phasealign;
#if CRT_DO_BLOOM
        int line_w;
#endif
  
        beg = (line - CRT_TOP + 0) * (v->outh + v->v_fac) / CRT_LINES + field;
        end = (line - CRT_TOP + 1) * (v->outh + v->v_fac) / CRT_LINES + field;

        if LSN_UNLIKELY(beg >= v->outh) { continue; }
        if LSN_UNLIKELY(end > v->outh) { end = v->outh; }

        /* Look for horizontal sync.
         * See comment above regarding vertical sync.
         */
        ln = (POSMOD(line + v->vsync, CRT_VRES)) * CRT_HRES;
        sig = v->inp + ln + v->hsync;
        s = 0;
        for (i = -HSYNC_WINDOW; i < HSYNC_WINDOW; i++) {
            s += sig[SYNC_BEG + i];
            if LSN_UNLIKELY(s <= (4 * SYNC_LEVEL)) {
                break;
            }
        }

		if LSN_LIKELY( v->do_hsync ) {
			v->hsync = POSMOD(i + v->hsync, CRT_HRES);
		}
		else {
			v->hsync = 0;
		}
//#if CRT_DO_HSYNC
//        v->hsync = POSMOD(i + v->hsync, CRT_HRES);
//#else
//        v->hsync = 0;
//#endif
        
        xpos = POSMOD(AV_BEG + v->hsync + xnudge, CRT_HRES);
        ypos = POSMOD(line + v->vsync + ynudge, CRT_VRES);
        pos = xpos + ypos * CRT_HRES;
        
        ccr = v->ccf[ypos % v->cc_period];
        sig = v->inp + ln + (v->hsync & ~3); /* burst @ 1/CB_FREQ sample rate */
        for (i = CB_BEG; i < CB_BEG + (CB_CYCLES * CRT_CB_FREQ); i++) {
            int p, n;
            p = ccr[i & 3] * 127 / 128; /* fraction of the previous */
            n = sig[i];                 /* mixed with the new sample */
            ccr[i & 3] = p + n;
        }
 
        phasealign = POSMOD(v->hsync, 4);

        /* amplitude of carrier = saturation, phase difference = hue */
        dci = ccr[(phasealign + 1) & 3] - ccr[(phasealign + 3) & 3];
        dcq = ccr[(phasealign + 2) & 3] - ccr[(phasealign + 0) & 3];

        /* rotate them by the hue adjustment angle */
        wave[0] = ((dci * huecs - dcq * huesn) >> 4) * v->saturation;
        wave[1] = ((dcq * huecs + dci * huesn) >> 4) * v->saturation;
        wave[2] = -wave[0];
        wave[3] = -wave[1];
        
        sig = v->inp + pos;
#if CRT_DO_BLOOM
        s = 0;
        for (i = 0; i < AV_LEN; i++) {
            s += sig[i]; /* sum up the scan line */
        }
        /* bloom emulation */
        prev_e = (prev_e * 123 / 128) + ((((max_e >> 1) - s) << 10) / max_e);
        line_w = (AV_LEN * 112 / 128) + (prev_e >> 9);

        dx = (line_w << 12) / v->outw;
        scanL = ((AV_LEN / 2) - (line_w >> 1) + 8) << 12;
        scanR = (AV_LEN - 1) << 12;
        
        L = (scanL >> 12);
        R = (scanR >> 12);
#else
        dx = ((AV_LEN - 1) << 12) / v->outw;
        scanL = 0;
        scanR = (AV_LEN - 1) << 12;
        L = 0;
        R = AV_LEN;
#endif
        reset_eq(&eqY);
        reset_eq(&eqI);
        reset_eq(&eqQ);
        
        for (i = L; i < R; i++) {
            out[i].y = eqf(&eqY, sig[i] + bright) << 4;
            out[i].i = eqf(&eqI, sig[i] * wave[(i + 0) & 3] >> 9) >> 3;
            out[i].q = eqf(&eqQ, sig[i] * wave[(i + 3) & 3] >> 9) >> 3;
        }

        cL = v->out + (beg * pitch);
        cR = cL + pitch;

        for (pos = scanL; (int)pos < scanR && cL < cR; pos += dx) {
            int y, I, q;
            int r, g, b;
            //int bb;

            R = pos & 0xfff;
            L = 0xfff - R;
            s = pos >> 12;
            
            yiqA = out + s;
            yiqB = out + s + 1;
            
            /* interpolate between samples if needed */
            y = ((yiqA->y * L) >>  2) + ((yiqB->y * R) >>  2);
            I = ((yiqA->i * L) >> 14) + ((yiqB->i * R) >> 14);
            q = ((yiqA->q * L) >> 14) + ((yiqB->q * R) >> 14);
            
            /* YIQ to RGB */
            r = (((y + 3879 * I + 2556 * q) >> 12) * v->contrast) >> 8;
            g = (((y - 1126 * I - 2605 * q) >> 12) * v->contrast) >> 8;
            b = (((y - 4530 * I + 7021 * q) >> 12) * v->contrast) >> 8;

#define LSN_DECAY_HACK

            if LSN_UNLIKELY(r < 0) r = 0;
            if LSN_UNLIKELY(g < 0) g = 0;
            if LSN_UNLIKELY(b < 0) b = 0;
#ifndef LSN_DECAY_HACK
            if LSN_UNLIKELY(r > 255) r = 255;
            if LSN_UNLIKELY(g > 255) g = 255;
            if LSN_UNLIKELY(b > 255) b = 255;
#endif
            
#ifndef LSN_DECAY_HACK
            if (v->blend) {
                aa = (r << 16 | g << 8 | b);

                switch (v->out_format) {
                    case CRT_PIX_FORMAT_RGB:
                    case CRT_PIX_FORMAT_RGBA:
                        bb = cL[0] << 16 | cL[1] << 8 | cL[2];
                        break;
                    case CRT_PIX_FORMAT_BGR: 
                    case CRT_PIX_FORMAT_BGRA:
                        bb = cL[2] << 16 | cL[1] << 8 | cL[0];
                        break;
                    case CRT_PIX_FORMAT_ARGB:
                        bb = cL[1] << 16 | cL[2] << 8 | cL[3];
                        break;
                    case CRT_PIX_FORMAT_ABGR:
                        bb = cL[3] << 16 | cL[2] << 8 | cL[1];
                        break;
                    default:
                        bb = 0;
                        break;
                }

                /* blend with previous color there */
                bb = (((aa & 0xfefeff) >> 1) + ((bb & 0xfefeff) >> 1));
            } else {
                bb = (r << 16 | g << 8 | b);
            }

            switch (v->out_format) {
                case CRT_PIX_FORMAT_RGB:
                case CRT_PIX_FORMAT_RGBA:
                    cL[0] = bb >> 16 & 0xff;
                    cL[1] = bb >>  8 & 0xff;
                    cL[2] = bb >>  0 & 0xff;
                    break;
                case CRT_PIX_FORMAT_BGR: 
                case CRT_PIX_FORMAT_BGRA:
                    cL[0] = bb >>  0 & 0xff;
                    cL[1] = bb >>  8 & 0xff;
                    cL[2] = bb >> 16 & 0xff;
                    break;
                case CRT_PIX_FORMAT_ARGB:
                    cL[1] = bb >> 16 & 0xff;
                    cL[2] = bb >>  8 & 0xff;
                    cL[3] = bb >>  0 & 0xff;
                    break;
                case CRT_PIX_FORMAT_ABGR:
                    cL[1] = bb >>  0 & 0xff;
                    cL[2] = bb >>  8 & 0xff;
                    cL[3] = bb >> 16 & 0xff;
                    break;
                default:
                    break;
            }

#else
            if (v->blend) {
                int sr, sg, sb;
                
                /*bb = *(int *)cL;

                sr = (((bb >> 16 & 0xff) * 6 + (r * 10)) >> 4);
                sg = (((bb >> 8 & 0xff) * 6 + (g * 10)) >> 4);
                sb = (((bb >> 0 & 0xff) * 6 + (b * 10)) >> 4);*/
				sr = (((cL[2]) * 6 + (r * 10)) >> 4);
                sg = (((cL[1]) * 6 + (g * 10)) >> 4);
                sb = (((cL[0]) * 6 + (b * 10)) >> 4);
                
                if LSN_UNLIKELY(sr > 255) sr = 255;
                if LSN_UNLIKELY(sg > 255) sg = 255;
                if LSN_UNLIKELY(sb > 255) sb = 255;
                
                //*cL++ = (sr << 16 | sg << 8 | sb);
                cL[0] = (unsigned char)sb;
                cL[1] = (unsigned char)sg;
                cL[2] = (unsigned char)sr;
            } else {
                if LSN_UNLIKELY(r > 255) r = 255;
                if LSN_UNLIKELY(g > 255) g = 255;
                if LSN_UNLIKELY(b > 255) b = 255;
                //*cL = (r << 16 | g << 8 | b);
                cL[0] = (unsigned char)b;
                cL[1] = (unsigned char)g;
                cL[2] = (unsigned char)r;
            }
#endif  // #ifndef LSN_DECAY_HACK
            cL += bpp;
        }
        
        /* duplicate extra lines */
        for (s = beg + 1; s < (end - v->scanlines); s++) {
            memcpy(v->out + s * pitch, v->out + (s - 1) * pitch, pitch);
        }
    }
}
