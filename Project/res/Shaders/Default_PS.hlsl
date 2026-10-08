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

StructuredBuffer<PointLightData> pointLights : register(t5);
StructuredBuffer<SpotLightData> spotLights : register(t6);
StructuredBuffer<DirectionalLightData> directionalLights : register(t7);

float4 CalculateLight(PixelShaderInput input)
{
    float3 normal = normalMap.Sample(m_sampler, input.uv).xyz;
    float3 normalLTS = normalize(normal * 2.0f - 1.0f);

    PixelData pixelData = { normalize(mul(normalLTS, input.tbn)), input.worldPos.xyz };

    float ambientOcclusion = m_ambientOcclusionMap.Sample(m_sampler, input.uv).r;
    float roughness = m_roughnessMap.Sample(m_sampler, input.uv).r;

    MaterialData materialData = { 1, 1, 1, 1 };
    
    float4 ambient, diffuse, specular;
    ambient = diffuse = specular = float4(0, 0, 0, 0);
    
    ambient = ambientLight * materialData.materialAmbient * ambientOcclusion;
    
     // Loop over PointLights
    for (int i = 0; i < pointLightCount; i++)
    {
        float4 tempAmbient, tempDiffuse, tempSpecular;
        tempAmbient = tempDiffuse = tempSpecular = float4(0, 0, 0, 0);
        
        // Preforme lighting calculations
        CalculatePointLight(pixelData, materialData, pointLights[i], cameraData,
            tempAmbient, tempDiffuse, tempSpecular);
        
        ambient += tempAmbient;
        diffuse += tempDiffuse;
        specular += tempSpecular;
    }
    
    return ambient + diffuse + specular;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Sample pixel color (assumes sRGB texture)
    float4 pixelColor = m_texture.Sample(m_sampler, input.uv);
    
    pixelColor *= CalculateLight(input);
    
    // ACES Filter
    pixelColor.xyz = ACESFilm(pixelColor.xyz);
    
    // Gamma Correction
    pixelColor = GammaCorrection(pixelColor, DEFAULT_GAMMA_FACTOR);
    
    return pixelColor;
}