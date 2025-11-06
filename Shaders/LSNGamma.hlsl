// ============================================================================
// LSNGamma.hlsl
// Common gamma transfer functions (linear <-> encoded) for D3D9 ps_2_0.
// Ported from LSN C++ implementations, including "_Precise" no-gap variants.
// All functions operate per-component; convenience float3 wrappers provided.
// ============================================================================

#ifndef GAMMA_HLSL
#define GAMMA_HLSL

// Clamp helper.
float LsnSaturate(float x) { return saturate(x); }
float3 LsnSaturate3(float3 x) { return saturate(x); }

// ======================= sRGB =======================
//
// Piecewise as per IEC 61966-2-1.

float sRGBtoLinear(float v) {
    // v in [0,1]
    return (v <= 0.04045) ? (v * (1.0 / 12.92))
                          : pow( (v + 0.055) * (1.0 / 1.055), 2.4 );
}

float LinearTosRGB(float v) {
    // v in [0,1]
    return (v <= 0.0031308) ? (v * 12.92)
                            : (1.055 * pow(v, 1.0 / 2.4) - 0.055);
}

// “Precise” no-gap variants.
float sRGBtoLinear_Precise(float v) {
    const float t  = 0.0392857142857142918601631720;           // ≈ 0.039285714…
    const float im = 1.0 / 12.9232101807878549948327417951;    // ≈ 0.0773993808
    return (v <= t) ? (v * im)
                    : pow( (v + 0.055) * (1.0 / 1.055), 2.4 );
}

float LinearTosRGB_Precise(float v) {
    const float t  = 0.0030399346397784318338231024370;        // Precise threshold.
    const float m  = 12.9232101807878549948327417951;
    return (v <= t) ? (v * m)
                    : (1.055 * pow(v, 1.0 / 2.4) - 0.055);
}

// Vector forms.
float3 sRGBtoLinear3(float3 c) {
    return float3(sRGBtoLinear(c.r), sRGBtoLinear(c.g), sRGBtoLinear(c.b));
}
float3 LinearTosRGB3(float3 c) {
    return float3(LinearTosRGB(c.r), LinearTosRGB(c.g), LinearTosRGB(c.b));
}
float3 sRGBtoLinear3_Precise(float3 c) {
    return float3(sRGBtoLinear_Precise(c.r), sRGBtoLinear_Precise(c.g), sRGBtoLinear_Precise(c.b));
}
float3 LinearTosRGB3_Precise(float3 c) {
    return float3(LinearTosRGB_Precise(c.r), LinearTosRGB_Precise(c.g), LinearTosRGB_Precise(c.b));
}

// ======================= SMPTE 170M (BT.601) =======================

float SMPTE170MtoLinear(float v) {
    return (v < 0.081) ? (v * (1.0 / 4.5))
                       : pow( (v + 0.099) * (1.0 / 1.099), 1.0 / 0.45 );
}
float LinearToSMPTE170M(float v) {
    return (v < 0.018) ? (v * 4.5)
                       : (1.099 * pow(v, 0.45) - 0.099);
}

float SMPTE170MtoLinear_Precise(float v) {
    const float t  = 0.0812428582986351593975271612180;
    const float a  = 1.09929682680944297568093048767;
    const float ao = 0.09929682680944297568093048767;
    return (v <= t) ? (v / 4.5)
                    : pow( (v + ao) / a, 1.0 / 0.45 );
}
float LinearToSMPTE170M_Precise(float v) {
    const float t  = 0.0180539685108078128139563744980;
    const float a  = 1.09929682680944297568093048767;
    const float ao = 0.09929682680944297568093048767;
    return (v <= t) ? (v * 4.5)
                    : (a * pow(v, 0.45) - ao);
}

float3 SMPTE170MtoLinear3(float3 c) {
    return float3(SMPTE170MtoLinear(c.r), SMPTE170MtoLinear(c.g), SMPTE170MtoLinear(c.b));
}
float3 LinearToSMPTE170M3(float3 c) {
    return float3(LinearToSMPTE170M(c.r), LinearToSMPTE170M(c.g), LinearToSMPTE170M(c.b));
}
float3 SMPTE170MtoLinear3_Precise(float3 c) {
    return float3(SMPTE170MtoLinear_Precise(c.r), SMPTE170MtoLinear_Precise(c.g), SMPTE170MtoLinear_Precise(c.b));
}
float3 LinearToSMPTE170M3_Precise(float3 c) {
    return float3(LinearToSMPTE170M_Precise(c.r), LinearToSMPTE170M_Precise(c.g), LinearToSMPTE170M_Precise(c.b));
}

// ======================= DCI-P3 (pure power 2.6) =======================

float DCIP3toLinear(float v)       { return pow(v, 2.6); }
float LinearToDCIP3(float v)       { return pow(v, 1.0 / 2.6); }
float3 DCIP3toLinear3(float3 c)    { return float3(DCIP3toLinear(c.r), DCIP3toLinear(c.g), DCIP3toLinear(c.b)); }
float3 LinearToDCIP33(float3 c)    { return float3(LinearToDCIP3(c.r), LinearToDCIP3(c.g), LinearToDCIP3(c.b)); }

// ======================= Adobe RGB (≈ 2.19921875) =======================

float AdobeRGBtoLinear(float v)    { return pow(v, 2.19921875); }
float LinearToAdobeRGB(float v)    { return pow(v, 1.0 / 2.19921875); }
float3 AdobeRGBtoLinear3(float3 c) { return float3(AdobeRGBtoLinear(c.r), AdobeRGBtoLinear(c.g), AdobeRGBtoLinear(c.b)); }
float3 LinearToAdobeRGB3(float3 c) { return float3(LinearToAdobeRGB(c.r), LinearToAdobeRGB(c.g), LinearToAdobeRGB(c.b)); }

// ======================= SMPTE 240M =======================

float SMPTE240MtoLinear(float v) {
    return (v < 0.0913) ? (v / 4.0)
                        : pow( (v + 0.1115) / 1.1115, 1.0 / 0.45 );
}
float LinearToSMPTE240M(float v) {
    return (v < 0.0228) ? (v * 4.0)
                        : (1.1115 * pow(v, 0.45) - 0.1115);
}

float SMPTE240MtoLinear_Precise(float v) {
    const float t  = 0.0912863421177801115380390228893;
    const float a  = 1.1115721959217312597711924126997;
    const float ao = 0.1115721959217312597711924126997;
    return (v < t) ? (v / 4.0)
                   : pow( (v + ao) / a, 1.0 / 0.45 );
}
float LinearToSMPTE240M_Precise(float v) {
    const float t  = 0.0228215855294450278845097557223;
    const float a  = 1.1115721959217312597711924126997;
    const float ao = 0.1115721959217312597711924126997;
    return (v < t) ? (v * 4.0)
                   : (a * pow(v, 0.45) - ao);
}

float3 SMPTE240MtoLinear3(float3 c) {
    return float3(SMPTE240MtoLinear(c.r), SMPTE240MtoLinear(c.g), SMPTE240MtoLinear(c.b));
}
float3 LinearToSMPTE240M3(float3 c) {
    return float3(LinearToSMPTE240M(c.r), LinearToSMPTE240M(c.g), LinearToSMPTE240M(c.b));
}
float3 SMPTE240MtoLinear3_Precise(float3 c) {
    return float3(SMPTE240MtoLinear_Precise(c.r), SMPTE240MtoLinear_Precise(c.g), SMPTE240MtoLinear_Precise(c.b));
}
float3 LinearToSMPTE240M3_Precise(float3 c) {
    return float3(LinearToSMPTE240M_Precise(c.r), LinearToSMPTE240M_Precise(c.g), LinearToSMPTE240M_Precise(c.b));
}

#endif // GAMMA_HLSL
