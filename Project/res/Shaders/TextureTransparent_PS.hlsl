#include "LightHelper.hlsli"

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

SamplerState m_sampler : register(s0);
Texture2D m_texture : register(t0);

StructuredBuffer<PointLightData> pointLgihts : register(t5);
StructuredBuffer<SpotLightData> spotLights : register(t6);
StructuredBuffer<DirectionalLightData> directionalLights : register(t7);

float4 CalculateLight(PixelShaderInput input)
{
    PixelData pixelData = { normalize(input.normal.xyz), input.worldPos.xyz };

    MaterialData materialData = { 1, 1, 1, 1 };
    
    float4 ambient, diffuse, specular;
    ambient = diffuse = specular = float4(0, 0, 0, 0);
    
    ambient = ambientLight * materialData.materialAmbient;
    
     // Loop over PointLights
    for (int i = 0; i < pointLightCount; i++)
    {
        float4 tempAmbient, tempDiffuse, tempSpecular;
        tempAmbient = tempDiffuse = tempSpecular = float4(0, 0, 0, 0);
        
        // Preforme lighting calculations
        CalculatePointLight(pixelData, materialData, pointLgihts[i], cameraData,
            tempAmbient, tempDiffuse, tempSpecular);
        
        diffuse += tempDiffuse;
        specular += tempSpecular;
    }
    
    return ambient + diffuse + specular;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Sample pixel color
    float4 pixelColor = m_texture.Sample(m_sampler, input.uv) * CalculateLight(input);
    
    // ACES Filter
    pixelColor.xyz = ACESFilm(pixelColor.xyz);
    
    // Gamma Correction
    pixelColor = GammaCorrection(pixelColor, DEFAULT_GAMMA_FACTOR);
    
    return pixelColor;
}