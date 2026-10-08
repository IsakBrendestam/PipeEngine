#include "LightHelper.hlsli"
#include "Noise.hlsli"

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 worldPos : WPOS;
    float2 uv : UV;
};

cbuffer CameraBuffer : register(b0)
{
    CameraData cameraData;
}

cbuffer LightData : register(b1)
{
    int pointLightCount;
    int spotLightCount;
    int directionalLightCount;
    float ambientLight;
}

cbuffer Time : register(b2)
{
    double time;
    float2 noiseScale;
}

SamplerState m_sampler : register(s0);

Texture2D m_texture : register(t0); // sRGB texture

StructuredBuffer<PointLightData> pointLgihts : register(t5);
StructuredBuffer<SpotLightData> spotLights : register(t6);
StructuredBuffer<DirectionalLightData> directionalLights : register(t7);

const static float4 filloutColor = float4(0.96f, 0.83f, 0.26f, 1.0f);

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Sample pixel color (assumes sRGB texture)
    float4 pixelColor = filloutColor;
    
    // ACES Filter
    //pixelColor.xyz = ACESFilm(pixelColor.xyz);
    
    // Gamma Correction
    //pixelColor = GammaCorrection(pixelColor, DEFAULT_GAMMA_FACTOR);
    
    // Noise texture
    float noise = pNoise(input.uv + float2(0, time), noiseScale);
    pixelColor.rgb *= lerp(0.60f, 1.0f, noise);
    pixelColor.a *= noise;

    return pixelColor;
}