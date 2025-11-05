#include "LSNFilterCommon.hlsl"
#include "LSNFilterHelpers.hlsl"

float blackman(float x, float a) {
    // x in [0,a]; symmetric about 0
    float t = x / a;
    return 0.42 - 0.5 * cos(2.0*3.14159265*t) + 0.08 * cos(4.0*3.14159265*t);
}

float w_sinc_blackman(float x, float a) {
    x = abs(x);
    if (x >= a) { return 0.0; }
    return sinc(x) * blackman(x, a);
}

// c0 = [W, H, 1/W, 1/H]; radius = 3
float4 main(float2 uv : TEXCOORD0) : COLOR {
    const float A = 3.0;
    float2 texSz  = c0.xy;
    float2 invTex = c0.zw;
    float2 coord  = uv * texSz - 0.5;
    float2 base   = floor(coord);
    float2 f      = coord - base;

    float4 sum  = 0;
    float  wsum = 0;

    [loop] for (int j = -3; j <= 3; ++j) {
        float wy = w_sinc_blackman(j - f.y, A);
        [loop] for (int i = -3; i <= 3; ++i) {
            float wx = w_sinc_blackman(i - f.x, A);
            float  w = wx * wy;
            sum  += SampleTexel(sSrc, base, int2(i,j), invTex) * w;
            wsum += w;
        }
    }
    return sum / max(wsum, 1e-6);
}
