#include "Common.hlsli"

struct ScanlineSettings
{
    float3 scanlineSettings; // Strength, frequency, speed
    float distortion;
    uint2 windowSize;
    float vignette;
    float shiftRGB;
    double time;
    float temp; // Padding
};

float2 DistortionCRT(float2 uv, float strength)
{
    float2 centeredUV = uv * 2.0f - 1.0f;
    float r2 = dot(centeredUV, centeredUV);

    return uv + centeredUV * (strength * r2);
}

float Vignette(float2 uv, float strength)
{
    float2 coord = uv * (1.0f - uv.xy);
    float vig = coord.x * coord.y * 15.0f;
    return pow(vig, strength);
}

float3 ShiftRGB(float strength, float2 uv, InputTexture res)
{
    return float3(res.inputBuffer.SampleLevel(res.samp, uv - float2(strength, strength), 0).r,
                  res.inputBuffer.SampleLevel(res.samp, uv, 0).g,
                  res.inputBuffer.SampleLevel(res.samp, uv + float2(strength, strength), 0).b);
}

float Scanline(float2 uv, float strength, float frequency, float speed, float time)
{
    float scanline = sin((uv.y + time * (-speed)) * frequency * 3.14159f);
    scanline = 0.5f + 0.5f * scanline;
    return lerp(1.0f, scanline, strength);
}

float4 CRT(float2 uv, ScanlineSettings settings, InputTexture res)
{
    float2 dist_uv = DistortionCRT(uv, settings.distortion);
    
    if (any(dist_uv < 0.0f) || any(dist_uv > 1.0f))
        return float4(0, 0, 0, 1);
    
    float4 color = float4(ShiftRGB(settings.shiftRGB, dist_uv, res), 1);
    color *= Scanline(dist_uv, settings.scanlineSettings.x, settings.scanlineSettings.y, settings.scanlineSettings.z, settings.time);
    
    float vignette = Vignette(dist_uv, settings.vignette);

    return color * vignette;
}
