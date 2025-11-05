#include "LSNFilterCommon.hlsl"
#include "LSNFilterHelpers.hlsl"

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

    [unroll] for (int j = -3; j <= 3; ++j) {
        float wy = lanczos(j - f.y, A);
        [unroll] for (int i = -3; i <= 3; ++i) {
            float wx = lanczos(i - f.x, A);
            float  w = wx * wy;
            sum  += SampleTexel(sSrc, base, int2(i,j), invTex) * w;
            wsum += w;
        }
    }
    return sum / max(wsum, 1e-6);
}
