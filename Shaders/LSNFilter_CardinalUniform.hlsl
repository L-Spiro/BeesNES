#include "LSNFilterCommon.hlsl"
#include "LSNFilterHelpers.hlsl"

// c0 = [W, H, 1/W, 1/H]
float4 main(float2 uv : TEXCOORD0) : COLOR {
    float2 texSz  = c0.xy;
    float2 invTex = c0.zw;
    float2 coord  = uv * texSz - 0.5;
    float2 base   = floor(coord);
    float2 f      = coord - base;

    float4 sum  = 0;
    float  wsum = 0;

    [unroll] for (int j = -1; j <= 2; ++j) {
        float wy = w_cardinal_uniform(j - f.y);
        [unroll] for (int i = -1; i <= 2; ++i) {
            float wx = w_cardinal_uniform(i - f.x);
            float  w = wx * wy;
            sum  += SampleTexel(sSrc, base, int2(i,j), invTex) * w;
            wsum += w;
        }
    }
    return sum / max(wsum, 1e-6);
}
