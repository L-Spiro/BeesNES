#include "LSNFilterCommon.hlsl"
#include "LSNFilterHelpers.hlsl"

// c0 = [W, H, 1/W, 1/H]
float4 main(float2 uv : TEXCOORD0) : COLOR {
    float2 texSz  = c0.xy;
    float2 invTex = c0.zw;
    float2 coord  = uv * texSz - 0.5;
    float2 base   = floor(coord);
    float2 f      = coord - base;

    float4 c00 = SampleTexel(sSrc, base, int2(0,0), invTex);
    float4 c10 = SampleTexel(sSrc, base, int2(1,0), invTex);
    float4 c01 = SampleTexel(sSrc, base, int2(0,1), invTex);
    float4 c11 = SampleTexel(sSrc, base, int2(1,1), invTex);

    float4 cx0 = lerp(c00, c10, f.x);
    float4 cx1 = lerp(c01, c11, f.x);
    return lerp(cx0, cx1, f.y);
}
