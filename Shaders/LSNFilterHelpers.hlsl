#ifndef LSN_FILTER_HELPERS_HLSL
#define LSN_FILTER_HELPERS_HLSL
// ============================================================================
// LSNFilterHelpers.hlsl
// Common filter helpers (linear domain) for D3D9 ps_2_0.
// ============================================================================

static const float LSN_PI = 3.141592502593994140625f;

float sinc( float x ) {
    x = abs( x );
    if ( x < 1e-5 ) { return 1.0; }
    return sin( LSN_PI * x ) / (LSN_PI * x);
}

float lanczos( float x, float a ) {
    x = abs( x );
    if ( x >= a ) { return 0.0; }
    return sinc( x ) * sinc( x / a );
}

// Mitchell–Netravali cubic with parameters B,C.
// Catmull-Rom is B=0, C=0.5.
float w_cubic_bc( float x, float B, float C ) {
    x = abs( x );
    float x2 = x*x;
    float x3 = x2*x;
    if ( x < 1.0 ) {
        return ((12.0 - 9.0*B - 6.0*C) * x3 +
                (-18.0 + 12.0*B + 6.0*C) * x2 +
                (6.0  - 2.0*B)) / 6.0;
    } else if ( x < 2.0 ) {
        return ((-B - 6.0*C) * x3 +
                (6.0*B + 30.0*C) * x2 +
                (-12.0*B - 48.0*C) * x +
                (8.0*B + 24.0*C)) / 6.0;
    }
    return 0.0;
}

float catmull_rom_filter_func( float x ) { return w_cubic_bc( x, 0.0, 0.5 ); }

float mitchell_filter_func( float x ) { return w_cubic_bc( x, 1.0 / 3.0, 1.0 / 3.0 ); }

float bspline_filter_func( float x ) { return w_cubic_bc( x, 1.0, 0.0 ); }

float cardinal_uniform_filter_func( float x ) { return w_cubic_bc( x, 0.0, 1.0 ); }

// ------------------------- Blackman window & variants -------------------------

// Blackman window using truncated coefficients (a = 0.16).
// dA0 = (1 - a)/2 = 0.42, dA1 = 0.5, dA2 = a/2 = 0.08.
// Window argument expected in [-1, 1] for full mainlobe (we pass normalized t).
float blackman_window( float x ) {
    const float a           = 0.16;
    const float a0          = (1.0 - a) * 0.5;  // 0.42
    const float a1          = 0.5;              // 0.5
    const float a2          = a * 0.5;          // 0.08
    return a0 + a1 * cos( LSN_PI * x ) + a2 * cos( 2.0 * LSN_PI * x );
}

// Blackman-filtered sinc with radius 3 (|t|<3), using blackman_window( t/3 ).
float blackman_filter_func( float x ) {
    x = abs( x );
    if ( x < 3.0 ) {
        return sinc( x ) * blackman_window( x / 3.0 );
    }
    return 0.0;
}

// Gaussian “sharp” variant: exp( -2 t^2 ) * sqrt(2/pi) windowed by Blackman over radius 1.25.
// sqrt(2/pi) ≈ 0.7978845608.
float gaussian_sharp_filter_func( float x ) {
    x = abs( x );
    if ( x < 1.25 ) {
        float g             = exp( -2.0 * x * x );
        float norm          = 0.79788458347320556640625f; //sqrt( 2.0 / LSN_PI )
        float w             = blackman_window( x / 1.25 );
        return g * norm * w;
    }
    return 0.0;
}

// Gaussian with sigma = 0.52, normalized: (1/(sigma*sqrt(2*pi))) * exp( -t^2/(2*sigma^2) )
// windowed by Blackman over radius 1.25.
float gaussian_filter_func( float x ) {
    x = abs( x );
    if ( x < 1.25 ) {
        const float sigma   = 0.52;
        float invSigma2     = 1.0 / (2.0 * sigma * sigma);
        float g             = exp( -(x * x) * invSigma2 );
        float norm          = 1.0 / (sigma * 2.5066282749176025390625f);   // 1 / (σ√(2π)); 2.5066282749176025390625f = sqrt( 2.0 * LSN_PI )
        float w             = blackman_window( x / 1.25 );
        return g * norm * w;
    }
    return 0.0;
}

// ------------------------- Kaiser window & Bessel I0 -------------------------

// Modified Bessel function of the first kind, order 0: I0(x).
// Series: I0(x) = sum_{k=0..∞} ( (x/2)^(2k) / (k!)^2 ).
// We implement a ps_2_0-friendly loop with early-out tolerance and a hard cap.
float bessel_i0( float x ) {
    const float eps         = 1.0e-6;       // Tolerance (relative).
    const int   KMAX        = 20;           // Hard cap for ps_2_0.
    float sum               = 1.0;          // k=0 term.
    float term              = 1.0;
    float xh                = x * 0.5;

    [loop]
    for ( int k = 1; k <= KMAX; ++k ) {
        // term_k = term_{k-1} * (xh^2 / k^2)
        float kf = (float)k;
        term *= ((xh * xh) / (kf * kf));
        sum += term;
        if ( term <= sum * eps ) { break; }
    }
    return sum;
}

// Kaiser helper: I0(alpha * sqrt(1 - (x/H)^2)) / I0(alpha), where H = half-width.
float kaiser_helper( float alpha, float halfWidth, float x ) {
    float r = x / halfWidth;
    r = saturate( r );                               // guard against slight over-range
    float num = bessel_i0( alpha * sqrt( max( 0.0, 1.0 - r * r ) ) );
    float den = bessel_i0( alpha );
    return (den > 0.0) ? (num / den) : 0.0;
}

// Kaiser filter (radius = 3), with attenuation A = 40 dB.
float kaiser_filter_func( float x ) {
    x = abs( x );
    if ( x < 3.0 ) {
        const float A       = 40.0;
        const float Am      = (A - 20.96);
        float alpha         = exp( log( 0.58417 * Am ) * 0.4 ) + 0.07886 * Am;
        return sinc( x ) * kaiser_helper( alpha, 3.0, x );
    }
    return 0.0;
}

// ------------------------- Misc -------------------------

// Sample at integer texel coordinate (center) given base and integer offsets.
// c0.zw must be [1/W, 1/H].
float4 SampleTexel( sampler2D sSrc, float2 base, int2 ofs, float2 invTex ) {
    float2 uv = (base + float2( ofs ) + 0.5) * invTex;
    uv = saturate( uv );
    return tex2D( sSrc, uv );
}

#endif // LSN_FILTER_HELPERS_HLSL
