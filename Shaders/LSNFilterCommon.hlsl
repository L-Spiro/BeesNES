// Common interface for all final-pass filters (D3D9 ps_2_0).

// Source texture is the scanlined linear FP render target, bound to sampler 0.
sampler2D sSrc : register(s0);

// c0 = [W, H, 1/W, 1/H] of the source texture (after scanline factor applied).
float4 c0 : register(c0);
