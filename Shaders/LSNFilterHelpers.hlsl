// == LSN filter helpers (linear domain) ==

float sinc(float x) {
    x = abs(x);
    if (x < 1e-5) { return 1.0; }
    return sin(3.14159265358979323846 * x) / (3.14159265358979323846 * x);
}

float lanczos(float x, float a) {
    x = abs(x);
    if (x >= a) { return 0.0; }
    return sinc(x) * sinc(x / a);
}

// Mitchell–Netravali cubic with parameters B,C.
// Catmull-Rom is B=0, C=0.5.
float w_cubic_bc(float x, float B, float C) {
    x = abs(x);
    float x2 = x*x;
    float x3 = x2*x;
    if (x < 1.0) {
        return ((12.0 - 9.0*B - 6.0*C) * x3 +
                (-18.0 + 12.0*B + 6.0*C) * x2 +
                (6.0  - 2.0*B)) / 6.0;
    } else if (x < 2.0) {
        return ((-B - 6.0*C) * x3 +
                (6.0*B + 30.0*C) * x2 +
                (-12.0*B - 48.0*C) * x +
                (8.0*B + 24.0*C)) / 6.0;
    }
    return 0.0;
}

float w_catmull_rom(float x) { return w_cubic_bc(x, 0.0, 0.5); }

float w_mitchell(float x) { return w_cubic_bc(x, 1.0 / 3.0, 1.0 / 3.0); }

float w_bspline(float x) { return w_cubic_bc(x, 1.0, 0.0); }

float w_cardinal_uniform(float x) { return w_cubic_bc(x, 0.0, 1.0); }

// Sample at integer texel coordinate (center) given base and integer offsets.
// c0.zw must be [1/W, 1/H].
float4 SampleTexel(sampler2D sSrc, float2 base, int2 ofs, float2 invTex) {
    float2 uv = (base + float2(ofs) + 0.5) * invTex;
    uv = saturate(uv);
    return tex2D(sSrc, uv);
}
