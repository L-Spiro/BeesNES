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
#include "../../Utilities/LSNUtilities.h"

#include <immintrin.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

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
#define USE_7_SAMPLE_KERNEL 1
#define USE_6_SAMPLE_KERNEL 0
#define USE_5_SAMPLE_KERNEL 0

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

#define HISTLEN     (2 * (CRT_CB_FREQ / 4) + 1)
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

/**
 * Runs the 3-band equalizers of CRT_EQ_LANES scan lines at once, one line per lane, writing [sample][Y/I/Q][line] to _piYiq.
 *	Results are bit-identical to running eqf() on each line separately.
 * 
 * \param _piYiq The output buffer.
 * \param _ppsSig The CRT_EQ_LANES active-video signal pointers.
 * \param _piWave The CRT_EQ_LANES carriers, CRT_CB_FREQ values per line.
 * \param _iBright The brightness offset, in signal units.
 * \param _iLen The number of samples to filter.
 **/
static void
EqLines(int * _piYiq, const short * const * _ppsSig, const int (* _piWave)[CRT_CB_FREQ], int _iBright, int _iLen)
{
	constexpr int iShift = 9 + CRT_SIG_SHIFT;	/* (sig * wave) >> iShift is the demodulated chroma */
	constexpr int iStride = 3 * CRT_EQ_LANES;
#if defined( __AVX2__ ) && (CRT_EQ_LANES == 8) && !USE_CONVOLUTION
	if LSN_LIKELY( lsn::CUtilities::IsAvx2Supported() ) {
		/** The state of 8 equalizers. */
		struct LSN_EQ8 {
			__m256i							mLf, mHf, mG[3];		/**< Coefficients and gains. */
			__m256i							mL[4], mH[4];			/**< Filter stages. */
			__m256i							mHist[HISTLEN];			/**< Input history (ring buffer). */
			int								iHist;					/**< Index of the oldest history entry. */
		};
		LSN_EQ8 e8[3];
		const struct EQF * pEq[3] = { &eqY, &eqI, &eqQ };
		for (int C = 0; C < 3; C++) {
			e8[C].mLf = _mm256_set1_epi32(pEq[C]->lf);
			e8[C].mHf = _mm256_set1_epi32(pEq[C]->hf);
			for (int K = 0; K < 3; K++) { e8[C].mG[K] = _mm256_set1_epi32(pEq[C]->g[K]); }
			for (int K = 0; K < 4; K++) { e8[C].mL[K] = e8[C].mH[K] = _mm256_setzero_si256(); }
			for (int K = 0; K < HISTLEN; K++) { e8[C].mHist[K] = _mm256_setzero_si256(); }
			e8[C].iHist = 0;
		}
		const __m256i mR = _mm256_set1_epi32(EQ_R);
		auto Eq8 = [&](LSN_EQ8 &_e, __m256i _mS) -> __m256i {
			_e.mL[0] = _mm256_add_epi32(_e.mL[0], _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(_e.mLf, _mm256_sub_epi32(_mS, _e.mL[0])), mR), EQ_P));
			_e.mH[0] = _mm256_add_epi32(_e.mH[0], _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(_e.mHf, _mm256_sub_epi32(_mS, _e.mH[0])), mR), EQ_P));
			for (int K = 1; K < 4; K++) {
				_e.mL[K] = _mm256_add_epi32(_e.mL[K], _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(_e.mLf, _mm256_sub_epi32(_e.mL[K-1], _e.mL[K])), mR), EQ_P));
				_e.mH[K] = _mm256_add_epi32(_e.mH[K], _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(_e.mHf, _mm256_sub_epi32(_e.mH[K-1], _e.mH[K])), mR), EQ_P));
			}
			__m256i mR0 = _e.mL[3];
			__m256i mR1 = _mm256_sub_epi32(_e.mH[3], _e.mL[3]);
			__m256i mR2 = _mm256_sub_epi32(_e.mHist[_e.iHist], _e.mH[3]);
			_e.mHist[_e.iHist] = _mS;
			if (++_e.iHist == HISTLEN) { _e.iHist = 0; }
			return _mm256_add_epi32(_mm256_add_epi32(_mm256_srai_epi32(_mm256_mullo_epi32(mR0, _e.mG[0]), EQ_P),
				_mm256_srai_epi32(_mm256_mullo_epi32(mR1, _e.mG[1]), EQ_P)), _mm256_srai_epi32(_mm256_mullo_epi32(mR2, _e.mG[2]), EQ_P));
		};

		/* the carrier of each line, split so that (sig * wave) >> iShift can be done exactly in 32 bits:
		 *	wave = (hi << (iShift - 2)) + lo, and (sig * wave) >> iShift == ((sig * hi) + ((sig * lo) >> (iShift - 2))) >> 2
		 */
		__m256i mWaveHi[CRT_CB_FREQ], mWaveLo[CRT_CB_FREQ];
		for (int K = 0; K < CRT_CB_FREQ; K++) {
			LSN_ALN int iTmp[CRT_EQ_LANES];
			for (int I = 0; I < CRT_EQ_LANES; I++) { iTmp[I] = _piWave[I][K]; }
			__m256i mW = _mm256_load_si256(reinterpret_cast<const __m256i *>(iTmp));
			mWaveHi[K] = _mm256_srai_epi32(mW, iShift - 2);
			mWaveLo[K] = _mm256_and_si256(mW, _mm256_set1_epi32((1 << (iShift - 2)) - 1));
		}
		/* each lane reads its own line: gather 32 bits at each 16-bit sample and keep the low half */
		LSN_ALN int iOff[CRT_EQ_LANES];
		for (int I = 0; I < CRT_EQ_LANES; I++) { iOff[I] = (int)(_ppsSig[I] - _ppsSig[0]); }
		const __m256i mOff = _mm256_load_si256(reinterpret_cast<const __m256i *>(iOff));
		const int * piBase = reinterpret_cast<const int *>(_ppsSig[0]);
		const __m256i mBright = _mm256_set1_epi32(_iBright);
		int iW = 0, iWq = 3 * (CRT_CB_FREQ / 4);
		for (int I = 0; I < _iLen; I++) {
			__m256i mSig = _mm256_i32gather_epi32(piBase, _mm256_add_epi32(mOff, _mm256_set1_epi32(I)), 2);
			mSig = _mm256_srai_epi32(_mm256_slli_epi32(mSig, 16), 16);
			__m256i mI = _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(mSig, mWaveHi[iW]), _mm256_srai_epi32(_mm256_mullo_epi32(mSig, mWaveLo[iW]), iShift - 2)), 2);
			__m256i mQ = _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(mSig, mWaveHi[iWq]), _mm256_srai_epi32(_mm256_mullo_epi32(mSig, mWaveLo[iWq]), iShift - 2)), 2);
			int * piDst = _piYiq + I * iStride;
			_mm256_storeu_si256(reinterpret_cast<__m256i *>(piDst), _mm256_slli_epi32(Eq8(e8[0], _mm256_add_epi32(mSig, mBright)), 4 - CRT_SIG_SHIFT));
			_mm256_storeu_si256(reinterpret_cast<__m256i *>(piDst + CRT_EQ_LANES), _mm256_srai_epi32(Eq8(e8[1], mI), 3));
			_mm256_storeu_si256(reinterpret_cast<__m256i *>(piDst + 2 * CRT_EQ_LANES), _mm256_srai_epi32(Eq8(e8[2], mQ), 3));
			if (++iW == CRT_CB_FREQ) { iW = 0; }
			if (++iWq == CRT_CB_FREQ) { iWq = 0; }
		}
		return;
	}
#endif	// #if defined( __AVX2__ ) && (CRT_EQ_LANES == 8) && !USE_CONVOLUTION
	for (int J = 0; J < CRT_EQ_LANES; J++) {
		struct EQF eY = eqY, eI = eqI, eQ = eqQ;
		const short * psSig = _ppsSig[J];
		reset_eq(&eY);
		reset_eq(&eI);
		reset_eq(&eQ);
		int iW = 0, iWq = 3 * (CRT_CB_FREQ / 4);
		for (int I = 0; I < _iLen; I++) {
			int * piDst = _piYiq + I * iStride + J;
			piDst[0] = eqf(&eY, psSig[I] + _iBright) << (4 - CRT_SIG_SHIFT);
			piDst[CRT_EQ_LANES] = eqf(&eI, (int)((long long)psSig[I] * _piWave[J][iW] >> iShift)) >> 3;
			piDst[2 * CRT_EQ_LANES] = eqf(&eQ, (int)((long long)psSig[I] * _piWave[J][iWq] >> iShift)) >> 3;
			if (++iW == CRT_CB_FREQ) { iW = 0; }
			if (++iWq == CRT_CB_FREQ) { iWq = 0; }
		}
	}
}

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

/**
 * The cosine and sine of the color carrier at each sample of a carrier cycle, scaled by 16384.
 **/
struct LSN_CARRIER_TABLE {
	LSN_CARRIER_TABLE() {
		for ( int I = 0; I < CRT_CB_FREQ; ++I ) {
			iCos[I] = int( std::lround( std::cos( 2.0 * 3.14159265358979323846 * I / CRT_CB_FREQ ) * 16384.0 ) );
			iSin[I] = int( std::lround( std::sin( 2.0 * 3.14159265358979323846 * I / CRT_CB_FREQ ) * 16384.0 ) );
		}
	}


	// == Members.
	int																	iCos[CRT_CB_FREQ];									/**< The cosine of the carrier at each sample, scaled by 16384. */
	int																	iSin[CRT_CB_FREQ];									/**< The sine of the carrier at each sample, scaled by 16384. */
};

/* search windows, in samples */
#define HSYNC_WINDOW (6 * CRT_CB_FREQ / 4)
#define VSYNC_WINDOW 6

extern void
crt_demodulate_full(struct CRT *v, int noise)
{
	int i, j = 0, line = 0, rn;
	const int * pYiq; /* this line's decoded Y, I, Q in v->yiq */
	const int * pA, * pB;
	/* the lines to decode and how to decode them, filled in order (the sync and carrier state run from line to line) and then
	 * decoded CRT_EQ_LANES at a time
	 */
	struct LSN_LINE {
		const short * psSig;
		int iWave[CRT_CB_FREQ];
		int iBeg, iEnd, iDx, iScanL, iScanR;
	} lLines[CRT_LINES];
	int iLines = 0;
	short *sig;
	int s = 0;
	int field, ratio;
	int *ccr; /* color carrier signal */
	int huesn, huecs;
	/* the position (and so the carrier phase) of the first sample that is decoded matches that of the 4-samples-per-cycle layout */
	int xnudge = ((((HB_PPUpx + PS_PPUpx + LB_PPUpx) * (CRT_CC_LINE * 4 / 10) / LINE_PPUpx) - 3) * (CRT_CB_FREQ / 4)) - AV_BEG, ynudge = 3;
	int bright = v->brightness - (BLACK_LEVEL + v->black_point);
	constexpr int bpp = 4;
	int  pitch;
#if CRT_DO_BLOOM
	int prev_e; /* filtered beam energy per scan line */
	int max_e; /* approx maximum energy in a scan line */
#endif
    
	//bpp = 4;//crt_bpp4fmt(v->out_format);
	/*if (bpp == 0) {
		return;
	}*/
	pitch = v->outw * bpp;
    
	crt_sincos14_full(&huesn, &huecs, ((v->hue % 360) + 33) * 8192 / 180);
	huesn >>= 11; /* make 4-bit */
	huecs >>= 11;

	/* the noise is quantized to whole 8-bit signal steps as it always was (its look and strength depend on that), and it is scaled
	 *	by the square root of the sample-rate increase so that the same amount of it lands in the video band at any CRT_CB_FREQ
	 */
	const int iNoiseK = (CRT_CB_FREQ == 4) ? (noise * 256) : (int)(noise * 256.0 * std::sqrt(CRT_CB_FREQ / 4.0) + 0.5);
	rn = v->rn;
	i = 0;
#if defined( __AVX2__ )
	if LSN_LIKELY( lsn::CUtilities::IsAvx2Supported() ) {
		/* 16 samples at a time; the noise generator is stepped with jump-ahead constants, so the result is identical to the loop below */
		unsigned uMul[17], uAdd[17];
		uMul[0] = 1;
		uAdd[0] = 0;
		for (j = 1; j <= 16; j++) {
			uMul[j] = uMul[j - 1] * 214019u;
			uAdd[j] = uAdd[j - 1] * 214019u + 140327895u;
		}
		LSN_ALN unsigned uLanes[16];
		for (j = 0; j < 16; j++) {
			uLanes[j] = uMul[j + 1] * (unsigned)rn + uAdd[j + 1];
		}
		__m256i vRn0 = _mm256_load_si256(reinterpret_cast<const __m256i *>(uLanes));
		__m256i vRn1 = _mm256_load_si256(reinterpret_cast<const __m256i *>(uLanes + 8));
		const __m256i vMul16 = _mm256_set1_epi32((int)uMul[16]), vAdd16 = _mm256_set1_epi32((int)uAdd[16]);
		const __m256i vFf = _mm256_set1_epi32(0xff), v7f = _mm256_set1_epi32(0x7f), vNoise = _mm256_set1_epi32(iNoiseK);
		const __m256i vMax = _mm256_set1_epi32(127 << CRT_SIG_SHIFT), vMin = _mm256_set1_epi32(-(127 << CRT_SIG_SHIFT));
		for (; i + 16 <= CRT_INPUT_SIZE; i += 16) {
			__m256i vN0 = _mm256_slli_epi32(_mm256_srai_epi32(_mm256_mullo_epi32(_mm256_sub_epi32(_mm256_and_si256(_mm256_srli_epi32(vRn0, 16), vFf), v7f), vNoise), 16), CRT_SIG_SHIFT);
			__m256i vN1 = _mm256_slli_epi32(_mm256_srai_epi32(_mm256_mullo_epi32(_mm256_sub_epi32(_mm256_and_si256(_mm256_srli_epi32(vRn1, 16), vFf), v7f), vNoise), 16), CRT_SIG_SHIFT);
			__m256i vS0 = _mm256_add_epi32(_mm256_cvtepi16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i *>(v->analog + i))), vN0);
			__m256i vS1 = _mm256_add_epi32(_mm256_cvtepi16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i *>(v->analog + i + 8))), vN1);
			vS0 = _mm256_max_epi32(_mm256_min_epi32(vS0, vMax), vMin);
			vS1 = _mm256_max_epi32(_mm256_min_epi32(vS1, vMax), vMin);
			_mm256_storeu_si256(reinterpret_cast<__m256i *>(v->inp + i), _mm256_permute4x64_epi64(_mm256_packs_epi32(vS0, vS1), 0xD8));
			vRn0 = _mm256_add_epi32(_mm256_mullo_epi32(vRn0, vMul16), vAdd16);
			vRn1 = _mm256_add_epi32(_mm256_mullo_epi32(vRn1, vMul16), vAdd16);
			rn = (int)(uMul[16] * (unsigned)rn + uAdd[16]);
		}
	}
#endif	// #if defined( __AVX2__ )
	for (; i < CRT_INPUT_SIZE; i++) {
		rn = (214019 * rn + 140327895);

		/* signal + noise */
		s = v->analog[i] + (((((rn >> 16) & 0xff) - 0x7f) * iNoiseK) >> 16) * (1 << CRT_SIG_SHIFT);
		if LSN_UNLIKELY(s >  (127 << CRT_SIG_SHIFT)) { s =  (127 << CRT_SIG_SHIFT); }
		if LSN_UNLIKELY(s < -(127 << CRT_SIG_SHIFT)) { s = -(127 << CRT_SIG_SHIFT); }
		v->inp[i] = (short)s;
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
			if (s <= (94 * (CRT_CB_FREQ / 4) * SYNC_LEVEL * (1 << CRT_SIG_SHIFT))) {
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
		int wave[CRT_CB_FREQ];
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
			if LSN_UNLIKELY(s <= ((7 * (CRT_CB_FREQ / 4) * SYNC_LEVEL * (1 << CRT_SIG_SHIFT)) / 2)) {
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
		sig = v->inp + ln + (v->hsync - (v->hsync % CRT_CB_FREQ)); /* burst @ 1/CB_FREQ sample rate */
		for (i = CB_BEG; i < CB_BEG + (CB_CYCLES * CRT_CB_FREQ); i++) {
			int p, n;
			p = ccr[i % CRT_CB_FREQ] * 127 / 128; /* fraction of the previous */
			n = sig[i];                 /* mixed with the new sample */
			ccr[i % CRT_CB_FREQ] = p + n;
		}
 
		phasealign = POSMOD(v->hsync, CRT_CB_FREQ);

		/* amplitude of carrier = saturation, phase difference = hue */
		dci = (ccr[(phasealign + 1 * (CRT_CB_FREQ / 4)) % CRT_CB_FREQ] - ccr[(phasealign + 3 * (CRT_CB_FREQ / 4)) % CRT_CB_FREQ]) >> CRT_SIG_SHIFT;
		dcq = (ccr[(phasealign + 2 * (CRT_CB_FREQ / 4)) % CRT_CB_FREQ] - ccr[(phasealign + 0 * (CRT_CB_FREQ / 4)) % CRT_CB_FREQ]) >> CRT_SIG_SHIFT;

		/* rotate them by the hue adjustment angle */
		wave[0] = ((dci * huecs - dcq * huesn) >> 4) * v->saturation;
		wave[1] = ((dcq * huecs + dci * huesn) >> 4) * v->saturation;
		if constexpr (CRT_CB_FREQ == 4) {
			wave[2] = -wave[0];
			wave[3] = -wave[1];
		}
		else {
			/* the carrier at every sample of a cycle (wave[0] holds the cosine term and wave[1] the sine term) */
			static const LSN_CARRIER_TABLE ctTable;
			const long long llCos = wave[0], llSin = wave[1];
			for (i = 0; i < CRT_CB_FREQ; i++) {
				wave[i] = (int)((llCos * ctTable.iCos[i] + llSin * ctTable.iSin[i] + (1 << 13)) >> 14);
			}
		}
        
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
		(void)L;
		(void)R;
		lLines[iLines].psSig = sig;
		memcpy(lLines[iLines].iWave, wave, sizeof(wave));
		lLines[iLines].iBeg = beg;
		lLines[iLines].iEnd = end;
		lLines[iLines].iDx = dx;
		lLines[iLines].iScanL = scanL;
		lLines[iLines].iScanR = scanR;
		iLines++;
	}

	for (int iGroup = 0; iGroup < iLines; iGroup += CRT_EQ_LANES) {
		const short * psSigs[CRT_EQ_LANES];
		int iWaves[CRT_EQ_LANES][CRT_CB_FREQ];
		for (int J = 0; J < CRT_EQ_LANES; J++) {
			/* a partial group repeats its first line in the unused lanes */
			const LSN_LINE & lThis = lLines[(iGroup + J < iLines) ? (iGroup + J) : iGroup];
			psSigs[J] = lThis.psSig;
			memcpy(iWaves[J], lThis.iWave, sizeof(iWaves[J]));
		}
		EqLines(v->yiq, psSigs, iWaves, bright * (1 << CRT_SIG_SHIFT), AV_LEN);
		memset(v->yiq + AV_LEN * 3 * CRT_EQ_LANES, 0, sizeof(int) * 3 * CRT_EQ_LANES);

	for (int iLane = 0; iLane < CRT_EQ_LANES && iGroup + iLane < iLines; iLane++) {
		unsigned pos;
		int scanR, dx;
		int L, R;
		unsigned char *cL, *cR;
		int beg, end;
		{
			const LSN_LINE & lThis = lLines[iGroup + iLane];
			beg = lThis.iBeg;
			end = lThis.iEnd;
			dx = lThis.iDx;
			scanR = lThis.iScanR;
			pos = lThis.iScanL;
		}
		pYiq = v->yiq + iLane;

		cL = v->out + (beg * pitch);
		cR = cL + pitch;



#if defined( __AVX512F__ )
		if ( lsn::CUtilities::IsAvx512FSupported() ) {
			// Preload constant vectors.
			__m512i vMaskFFf    = _mm512_set1_epi32(0xfff);
			__m512i vContrast   = _mm512_set1_epi32(v->contrast);
			__m512i vMax255     = _mm512_set1_epi32(255);
			__m512i vZero       = _mm512_setzero_si512();
			__m512i v3          = _mm512_set1_epi32(3 * CRT_EQ_LANES);
			__m512i vConst3879  = _mm512_set1_epi32(3879);
			__m512i vConst2556  = _mm512_set1_epi32(2556);
			__m512i vConst1126  = _mm512_set1_epi32(1126);
			__m512i vConst2605  = _mm512_set1_epi32(2605);
			__m512i vConst4530  = _mm512_set1_epi32(4530);
			__m512i vConst7021  = _mm512_set1_epi32(7021);

			// Precompute an index vector for multiplying dx:
			// 0, dx, 2*dx, ..., 15*dx.
			__m512i idxInc = _mm512_setr_epi32(
				0,      dx,     2*dx,   3*dx,
				4*dx,   5*dx,   6*dx,   7*dx,
				8*dx,   9*dx,   10*dx,  11*dx,
				12*dx,  13*dx,  14*dx,  15*dx
			);

			// Process 16 pixels per loop.
			auto scanRtotal = scanR - 16;
			auto cRtotal = cR - (16 * bpp);
			while ((int)pos <= scanRtotal && cL <= cRtotal) {
				// Compute positions for 16 pixels: pos, pos+dx, ..., pos+15*dx.
				__m512i vPos = _mm512_add_epi32(_mm512_set1_epi32(pos), idxInc);

				// Compute R = pos & 0xfff.
				__m512i vR = _mm512_and_epi32(vPos, vMaskFFf);
				// Compute L = 0xfff - R.
				__m512i vL = _mm512_sub_epi32(vMaskFFf, vR);
				// Compute sample index s = pos >> 12.
				__m512i vs = _mm512_srli_epi32(vPos, 12);

				// For each sample, load two YIQ samples.
				// Each YIQ occupies 3 ints; sample A's y is at index = s*3.
				__m512i indexA = _mm512_mullo_epi32(vs, v3);
				__m512i indexB = _mm512_add_epi32(indexA, _mm512_set1_epi32(3 * CRT_EQ_LANES));

				// Gather A.y, A.i, A.q.
				__m512i A_y = _mm512_i32gather_epi32(indexA, pYiq, 4);
				__m512i A_i = _mm512_i32gather_epi32(_mm512_add_epi32(indexA, _mm512_set1_epi32(CRT_EQ_LANES)),
														pYiq, 4);
				__m512i A_q = _mm512_i32gather_epi32(_mm512_add_epi32(indexA, _mm512_set1_epi32(2 * CRT_EQ_LANES)),
														pYiq, 4);

				// Gather B.y, B.i, B.q.
				__m512i B_y = _mm512_i32gather_epi32(indexB, pYiq, 4);
				__m512i B_i = _mm512_i32gather_epi32(_mm512_add_epi32(indexB, _mm512_set1_epi32(CRT_EQ_LANES)),
														pYiq, 4);
				__m512i B_q = _mm512_i32gather_epi32(_mm512_add_epi32(indexB, _mm512_set1_epi32(2 * CRT_EQ_LANES)),
														pYiq, 4);

				// Interpolate Y, I, and q.
				// y = ((A_y * L) >> 2) + ((B_y * R) >> 2)
				__m512i yA = _mm512_mullo_epi32(A_y, vL);
				__m512i yB = _mm512_mullo_epi32(B_y, vR);
				__m512i y_vec = _mm512_add_epi32(_mm512_srai_epi32(yA, 2), _mm512_srai_epi32(yB, 2));

				// I = ((A_i * L) >> 14) + ((B_i * R) >> 14)
				__m512i iA = _mm512_mullo_epi32(A_i, vL);
				__m512i iB = _mm512_mullo_epi32(B_i, vR);
				__m512i I_vec = _mm512_add_epi32(_mm512_srai_epi32(iA, 14), _mm512_srai_epi32(iB, 14));

				// q = ((A_q * L) >> 14) + ((B_q * R) >> 14)
				__m512i qA = _mm512_mullo_epi32(A_q, vL);
				__m512i qB = _mm512_mullo_epi32(B_q, vR);
				__m512i q_vec = _mm512_add_epi32(_mm512_srai_epi32(qA, 14), _mm512_srai_epi32(qB, 14));

				// Convert YIQ to RGB.
				// r = (((y + 3879*I + 2556*q) >> 12) * contrast) >> 8;
				__m512i r_term = _mm512_add_epi32(y_vec,
									_mm512_add_epi32(_mm512_mullo_epi32(I_vec, vConst3879),
														_mm512_mullo_epi32(q_vec, vConst2556)));
				__m512i r_tmp = _mm512_srai_epi32(r_term, 12);
				__m512i r_vec = _mm512_srai_epi32(_mm512_mullo_epi32(r_tmp, vContrast), 8);

				// g = (((y - 1126*I - 2605*q) >> 12) * contrast) >> 8;
				__m512i g_term = _mm512_sub_epi32(y_vec,
									_mm512_add_epi32(_mm512_mullo_epi32(I_vec, vConst1126),
														_mm512_mullo_epi32(q_vec, vConst2605)));
				__m512i g_tmp = _mm512_srai_epi32(g_term, 12);
				__m512i g_vec = _mm512_srai_epi32(_mm512_mullo_epi32(g_tmp, vContrast), 8);

				// b = (((y - 4530*I + 7021*q) >> 12) * contrast) >> 8;
				__m512i b_term = _mm512_add_epi32(
									_mm512_sub_epi32(y_vec, _mm512_mullo_epi32(I_vec, vConst4530)),
									_mm512_mullo_epi32(q_vec, vConst7021));
				__m512i b_tmp = _mm512_srai_epi32(b_term, 12);
				__m512i b_vec = _mm512_srai_epi32(_mm512_mullo_epi32(b_tmp, vContrast), 8);

				if LSN_LIKELY(v->blend) {
					// Clamp negative values to zero.
					r_vec = _mm512_max_epi32(r_vec, vZero);
					g_vec = _mm512_max_epi32(g_vec, vZero);
					b_vec = _mm512_max_epi32(b_vec, vZero);

					// Blend with the previous color there: (old * 6 + new * 10) >> 4, clamped to 255; alpha is kept.
					__m512i vOld = _mm512_loadu_si512(cL);
					__m512i vFf = _mm512_set1_epi32(0xff);
					__m512i v6 = _mm512_set1_epi32(6), v10 = _mm512_set1_epi32(10);
					__m512i sb_vec = _mm512_min_epi32(_mm512_srai_epi32(_mm512_add_epi32(_mm512_mullo_epi32(_mm512_and_si512(vOld, vFf), v6), _mm512_mullo_epi32(b_vec, v10)), 4), vMax255);
					__m512i sg_vec = _mm512_min_epi32(_mm512_srai_epi32(_mm512_add_epi32(_mm512_mullo_epi32(_mm512_and_si512(_mm512_srli_epi32(vOld, 8), vFf), v6), _mm512_mullo_epi32(g_vec, v10)), 4), vMax255);
					__m512i sr_vec = _mm512_min_epi32(_mm512_srai_epi32(_mm512_add_epi32(_mm512_mullo_epi32(_mm512_and_si512(_mm512_srli_epi32(vOld, 16), vFf), v6), _mm512_mullo_epi32(r_vec, v10)), 4), vMax255);
					_mm512_storeu_si512(cL, _mm512_or_si512(_mm512_and_si512(vOld, _mm512_set1_epi32(int(0xff000000))),
						_mm512_or_si512(sb_vec, _mm512_or_si512(_mm512_slli_epi32(sg_vec, 8), _mm512_slli_epi32(sr_vec, 16)))));
				} else {
					// Non-blend: perform a saturated conversion and store 16 pixels in B, G, R order.
					// Clamp negative values to zero.
					r_vec = _mm512_max_epi32(r_vec, vZero);
					g_vec = _mm512_max_epi32(g_vec, vZero);
					b_vec = _mm512_max_epi32(b_vec, vZero);

					// Clamp negative values to zero.
					r_vec = _mm512_min_epi32(r_vec, vMax255);
					g_vec = _mm512_min_epi32(g_vec, vMax255);
					b_vec = _mm512_min_epi32(b_vec, vMax255);

					// Store 16 pixels in B, G, R order, keeping alpha.
					__m512i vOld = _mm512_loadu_si512(cL);
					_mm512_storeu_si512(cL, _mm512_or_si512(_mm512_and_si512(vOld, _mm512_set1_epi32(int(0xff000000))),
						_mm512_or_si512(b_vec, _mm512_or_si512(_mm512_slli_epi32(g_vec, 8), _mm512_slli_epi32(r_vec, 16)))));
				}

				pos += dx * 16;
				cL  += 16 * bpp;
			}
			goto Finish;
		}
#endif  // #if defined( __AVX512F__ )

#if defined( __AVX2__ )
		if LSN_LIKELY( lsn::CUtilities::IsAvx2Supported() ) {
			// Preload constant vectors.
			__m256i vMaskFFf	= _mm256_set1_epi32(0xfff);
			__m256i vContrast	= _mm256_set1_epi32(v->contrast);
			__m256i vMax255		= _mm256_set1_epi32(255);
			__m256i vZero		= _mm256_setzero_si256();
			__m256i v3			= _mm256_set1_epi32(3 * CRT_EQ_LANES);
			__m256i vConst3879	= _mm256_set1_epi32(3879);
			__m256i vConst2556	= _mm256_set1_epi32(2556);
			__m256i vConst1126	= _mm256_set1_epi32(1126);
			__m256i vConst2605	= _mm256_set1_epi32(2605);
			__m256i vConst4530	= _mm256_set1_epi32(4530);
			__m256i vConst7021	= _mm256_set1_epi32(7021);

			// Precompute an index vector for multiplying dx.
			__m256i idxInc = _mm256_setr_epi32(0, dx, 2*dx, 3*dx, 4*dx, 5*dx, 6*dx, 7*dx);
		
			// Process 8 pixels per loop.
			auto scanRtotal = scanR - 8;
			auto cRtotal = cR - (8 * bpp);
			while ((int)pos <= scanRtotal && cL <= cRtotal) {
				// Compute the positions for 8 pixels: pos, pos+dx, ..., pos+7*dx.
				__m256i vPos = _mm256_add_epi32(_mm256_set1_epi32(pos), idxInc);

				// Compute R = pos & 0xfff.
				__m256i vR = _mm256_and_si256(vPos, vMaskFFf);
				// Compute L = 0xfff - R.
				__m256i vL = _mm256_sub_epi32(vMaskFFf, vR);
				// Compute sample index s = pos >> 12.
				__m256i vs = _mm256_srli_epi32(vPos, 12);

				// For each sample, we need to load two YIQ samples.
				// Since each YIQ is 3 ints, the first samplefs y is at index = s*3.
				__m256i indexA = _mm256_mullo_epi32(vs, v3);
				__m256i indexB = _mm256_add_epi32(indexA, _mm256_set1_epi32(3 * CRT_EQ_LANES));

				// Gather A.y, A.i, A.q.
				__m256i A_y = _mm256_i32gather_epi32(pYiq, indexA, 4);
				__m256i A_i = _mm256_i32gather_epi32(pYiq, _mm256_add_epi32(indexA, _mm256_set1_epi32(CRT_EQ_LANES)), 4);
				__m256i A_q = _mm256_i32gather_epi32(pYiq, _mm256_add_epi32(indexA, _mm256_set1_epi32(2 * CRT_EQ_LANES)), 4);

				// Gather B.y, B.i, B.q.
				__m256i B_y = _mm256_i32gather_epi32(pYiq, indexB, 4);
				__m256i B_i = _mm256_i32gather_epi32(pYiq, _mm256_add_epi32(indexB, _mm256_set1_epi32(CRT_EQ_LANES)), 4);
				__m256i B_q = _mm256_i32gather_epi32(pYiq, _mm256_add_epi32(indexB, _mm256_set1_epi32(2 * CRT_EQ_LANES)), 4);

				// Interpolate Y, I, and q.
				// y = ((A_y * L) >> 2) + ((B_y * R) >> 2)
				__m256i yA = _mm256_mullo_epi32(A_y, vL);
				__m256i yB = _mm256_mullo_epi32(B_y, vR);
				__m256i y_vec = _mm256_add_epi32(_mm256_srai_epi32(yA, 2), _mm256_srai_epi32(yB, 2));

				// I = ((A_i * L) >> 14) + ((B_i * R) >> 14)
				__m256i iA = _mm256_mullo_epi32(A_i, vL);
				__m256i iB = _mm256_mullo_epi32(B_i, vR);
				__m256i I_vec = _mm256_add_epi32(_mm256_srai_epi32(iA, 14), _mm256_srai_epi32(iB, 14));

				// q = ((A_q * L) >> 14) + ((B_q * R) >> 14)
				__m256i qA = _mm256_mullo_epi32(A_q, vL);
				__m256i qB = _mm256_mullo_epi32(B_q, vR);
				__m256i q_vec = _mm256_add_epi32(_mm256_srai_epi32(qA, 14), _mm256_srai_epi32(qB, 14));

				// Convert YIQ to RGB.
				// r = (((y + 3879*I + 2556*q) >> 12) * contrast) >> 8;
				__m256i r_term = _mm256_add_epi32(y_vec,
									_mm256_add_epi32(_mm256_mullo_epi32(I_vec, vConst3879),
													_mm256_mullo_epi32(q_vec, vConst2556)));
				__m256i r_tmp = _mm256_srai_epi32(r_term, 12);
				__m256i r_vec = _mm256_srai_epi32(_mm256_mullo_epi32(r_tmp, vContrast), 8);

				// g = (((y - 1126*I - 2605*q) >> 12) * contrast) >> 8;
				__m256i g_term = _mm256_sub_epi32(y_vec,
									_mm256_add_epi32(_mm256_mullo_epi32(I_vec, vConst1126),
													_mm256_mullo_epi32(q_vec, vConst2605)));
				__m256i g_tmp = _mm256_srai_epi32(g_term, 12);
				__m256i g_vec = _mm256_srai_epi32(_mm256_mullo_epi32(g_tmp, vContrast), 8);

				// b = (((y - 4530*I + 7021*q) >> 12) * contrast) >> 8;
				__m256i b_term = _mm256_add_epi32(
									_mm256_sub_epi32(y_vec, _mm256_mullo_epi32(I_vec, vConst4530)),
									_mm256_mullo_epi32(q_vec, vConst7021));
				__m256i b_tmp = _mm256_srai_epi32(b_term, 12);
				__m256i b_vec = _mm256_srai_epi32(_mm256_mullo_epi32(b_tmp, vContrast), 8);

				// Clamp negative values to zero.
				r_vec = _mm256_max_epi32(r_vec, vZero);
				g_vec = _mm256_max_epi32(g_vec, vZero);
				b_vec = _mm256_max_epi32(b_vec, vZero);

				__m256i vOld = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(cL));
				if LSN_LIKELY(v->blend) {
					// Blend with the previous color there: (old * 6 + new * 10) >> 4, clamped to 255; alpha is kept.
					__m256i vFf = _mm256_set1_epi32(0xff);
					__m256i v6 = _mm256_set1_epi32(6), v10 = _mm256_set1_epi32(10);
					b_vec = _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(_mm256_and_si256(vOld, vFf), v6), _mm256_mullo_epi32(b_vec, v10)), 4);
					g_vec = _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(_mm256_and_si256(_mm256_srli_epi32(vOld, 8), vFf), v6), _mm256_mullo_epi32(g_vec, v10)), 4);
					r_vec = _mm256_srai_epi32(_mm256_add_epi32(_mm256_mullo_epi32(_mm256_and_si256(_mm256_srli_epi32(vOld, 16), vFf), v6), _mm256_mullo_epi32(r_vec, v10)), 4);
				}
				// Clamp values to 255.
				r_vec = _mm256_min_epi32(r_vec, vMax255);
				g_vec = _mm256_min_epi32(g_vec, vMax255);
				b_vec = _mm256_min_epi32(b_vec, vMax255);
				// Write out 8 pixels in B, G, R order, keeping alpha.
				_mm256_storeu_si256(reinterpret_cast<__m256i *>(cL), _mm256_or_si256(_mm256_and_si256(vOld, _mm256_set1_epi32(int(0xff000000))),
					_mm256_or_si256(b_vec, _mm256_or_si256(_mm256_slli_epi32(g_vec, 8), _mm256_slli_epi32(r_vec, 16)))));

				pos += dx * 8;
				cL  += 8 * bpp;
			}
		}
#endif	// #if defined( __AVX2__ )

#if defined( __AVX512F__ )
	Finish :
#endif	// #if defined( __AVX512F__ )
		for (/*pos = scanL*/; (int)pos < scanR && cL < cR; pos += dx) {
			int y, I, q;
			int r, g, b;
			//int bb;

			R = pos & 0xfff;
			L = 0xfff - R;
			s = pos >> 12;
            
			pA = pYiq + s * (3 * CRT_EQ_LANES);
			pB = pA + (3 * CRT_EQ_LANES);
            
			/* interpolate between samples if needed */
			y = ((pA[0] * L) >>  2) + ((pB[0] * R) >>  2);
			I = ((pA[CRT_EQ_LANES] * L) >> 14) + ((pB[CRT_EQ_LANES] * R) >> 14);
			q = ((pA[2 * CRT_EQ_LANES] * L) >> 14) + ((pB[2 * CRT_EQ_LANES] * R) >> 14);
            
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
}
