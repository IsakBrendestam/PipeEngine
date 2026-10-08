#include "LightHelper.hlsli"

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
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

SamplerState m_sampler : register(s0);

Texture2D m_texture : register(t0); // sRGB Texture
Texture2D normalMap : register(t1); // RGB Texture
Texture2D m_ambientOcclusionMap : register(t2); // R Texture
Texture2D m_roughnessMap : register(t3); // R Texture
Texture2D m_metalnessMap : register(t4); // R Texture

const static float roughnessFactor = 0.1f;
const static float metalnessFactor = 0.1f;

StructuredBuffer<PointLightData> pointLights : register(t5);
StructuredBuffer<SpotLightData> spotLights : register(t6);
StructuredBuffer<DirectionalLightData> directionalLights : register(t7);

float3 CalculateLight(PixelShaderInput input, float3 albedo)
{
    float3 normal = normalMap.Sample(m_sampler, input.uv).xyz;
    float3 normalLTS = normalize(normal * 2.0f - 1.0f);
    float3 worldNormal = normalize(mul(normalLTS, input.tbn));

    float ambientOcclusion = m_ambientOcclusionMap.Sample(m_sampler, input.uv).r;

    float roughness = m_roughnessMap.Sample(m_sampler, input.uv).g;
    roughness = max(roughness, 0.04f);

    float metalness = m_metalnessMap.Sample(m_sampler, input.uv).b;
    metalness *= metalnessFactor;
    
    float3 ambientColor = float3(1.0f, 1.0f, 1.0f);
    float3 ambient = ambientLight * ambientColor * ambientOcclusion;
    float3 finalColor = albedo * ambient;

    MaterialPBR material = {
        albedo,
        roughness,
        metalness
    };
    
     // Loop over PointLights
    for (int i = 0; i < pointLightCount; i++)
    {
        PointLightData light = pointLights[i];

        CookTorranceLightInput lightInput = {
            input.worldPos.xyz,
            worldNormal,
            cameraData.cameraPosition.xyz,
            light.lightPosition,
            light.lightIntencity
        };

        finalColor += CalculatePointLightCookTorrance(lightInput, material);
    }
    
    return finalColor;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Sample pixel color (assumes sRGB texture)
    float4 pixelColor = m_texture.Sample(m_sampler, input.uv);
    
    float3 color = CalculateLight(input, pixelColor.rgb);
    
    return float4(color, 1.0f);
}