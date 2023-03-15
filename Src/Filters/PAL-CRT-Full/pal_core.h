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

#ifndef _PAL_CORE_H_
#define _PAL_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* pal_core.h
 *
 * The demodulator. This is also where you can define which system to emulate.
 * 
 */

/* library version */
#define PAL_MAJOR 1
#define PAL_MINOR 1
#define PAL_PATCH 2

#define PAL_SYSTEM_PAL 0 /* 'standard' PAL */
#define PAL_SYSTEM_NES 1 /* decode 6 or 9-bit NES pixels */

/* the system to be compiled */
#define PAL_SYSTEM PAL_SYSTEM_NES

#if (PAL_SYSTEM == PAL_SYSTEM_NES)
#include "pal_nes.h"
#elif (PAL_SYSTEM == PAL_SYSTEM_PAL)
#include "pal.h"
#else
#error No system defined
#endif

/* NOTE: this library does not use the alpha channel at all */
#define PAL_PIX_FORMAT_RGB  0  /* 3 bytes per pixel [R,G,B,R,G,B,R,G,B...] */
#define PAL_PIX_FORMAT_BGR  1  /* 3 bytes per pixel [B,G,R,B,G,R,B,G,R...] */
#define PAL_PIX_FORMAT_ARGB 2  /* 4 bytes per pixel [A,R,G,B,A,R,G,B...]   */
#define PAL_PIX_FORMAT_RGBA 3  /* 4 bytes per pixel [R,G,B,A,R,G,B,A...]   */
#define PAL_PIX_FORMAT_ABGR 4  /* 4 bytes per pixel [A,B,G,R,A,B,G,R...]   */
#define PAL_PIX_FORMAT_BGRA 5  /* 4 bytes per pixel [B,G,R,A,B,G,R,A...]   */

/* do bloom emulation (side effect: makes screen have black borders) */
#define PAL_DO_BLOOM    0  /* does not work for NES */
#define PAL_DO_VSYNC    1  /* look for VSYNC */
#define PAL_DO_HSYNC    1  /* look for HSYNC */

struct PAL_CRT {
    signed char analog[PAL_INPUT_SIZE];
    signed char inp[PAL_INPUT_SIZE]; /* CRT input, can be noisy */

    int outw, outh; /* output width/height */
    int out_format; /* output pixel format (one of the PAL_PIX_FORMATs) */
    unsigned char *out; /* output image */

    int brightness, contrast, saturation; /* common monitor settings */
    int black_point, white_point; /* user-adjustable */
    int scanlines; /* leave gaps between lines if necessary */
    int blend; /* blend new field onto previous image */
    int chroma_correction; /* aka Hanover bar correction */ 
    int chroma_lag; /* misalignment of Y and UV due to decoding delay */
    /* NOTE!!! chroma_lag is only defined between [-8, 8]
     * anything else provokes undefined behavior and might crash the program
     */
    unsigned v_fac; /* factor to stretch img vertically onto the output img */

    /* internal data */
    int ccf[PAL_VRES][4]; /* faster color carrier convergence */
    int cc_period; /* vertically */
    int hsync, vsync; /* keep track of sync over frames */
    int rn; /* seed for the 'random' noise */
};

/* Initializes the library. Sets up filters.
 *   w   - width of the output image
 *   h   - height of the output image
 *   f   - format of the output image
 *   out - pointer to output image data
 */
extern void pal_init(struct PAL_CRT *v, int w, int h, int f, unsigned char *out);

/* Updates the output image parameters
 *   w   - width of the output image
 *   h   - height of the output image
 *   f   - format of the output image
 *   out - pointer to output image data
 */
extern void pal_resize(struct PAL_CRT *v, int w, int h, int f, unsigned char *out);

/* Resets the PAL_CRT settings back to their defaults */
extern void pal_reset(struct PAL_CRT *v);

/* Modulates RGB image into an analog PAL signal
 *   s - struct containing settings to apply to this field
 */
extern void pal_modulate(struct PAL_CRT *v, struct PAL_SETTINGS *s);
    
/* Demodulates the PAL signal generated by pal_modulate()
 *   noise - the amount of noise added to the signal (0 - inf)
 */
extern void pal_demodulate(struct PAL_CRT *v, int noise);

/* Get the bytes per pixel for a certain PAL_PIX_FORMAT_
 * 
 *   format - the format to get the bytes per pixel for
 *   
 * returns 0 if the specified format does not exist
 */
extern int pal_bpp4fmt(int format);

/*****************************************************************************/
/*************************** FIXED POINT SIN/COS *****************************/
/*****************************************************************************/

#define T14_2PI           16384
#define T14_MASK          (T14_2PI - 1)
#define T14_PI            (T14_2PI / 2)

extern void pal_sincos14(int *s, int *c, int n);

#ifdef __cplusplus
}
#endif

#endif