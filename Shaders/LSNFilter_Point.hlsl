#include "LSNFilterCommon.hlsl"
#include "LSNFilterHelpers.hlsl"

// c0 = [W, H, 1/W, 1/H]
float4 main(float2 uv : TEXCOORD0) : COLOR {
    float2 texSz  = c0.xy;
    float2 invTex = c0.zw;
    // Center-based coordinate in texel space
    float2 coord  = uv * texSz - 0.5;
    float2 base   = floor(coord);
    float2 f      = coord - base;
    // Nearest neighbor
    int2   ofs    = int2( (f.x < 0.5) ? 0 : 1, (f.y < 0.5) ? 0 : 1 );
    return SampleTexel(sSrc, base, ofs, invTex);
}
