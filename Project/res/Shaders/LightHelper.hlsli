
#define MAX_LIGHTS 25

#define PI 3.14159265359f

struct MaterialData
{
    float materialAmbient;
    float materialDiffuse;
    float materialSpecular;
    float materialShininess;
};

struct CameraData
{
    float4 cameraPosition;
    float4 cameraDirection;
};

struct PixelData
{
    float3 normal;
    float3 worldPos;
};


/****************************
       GammaCorrection
*****************************/

#define DEFAULT_GAMMA_FACTOR 2.2f

inline float4 InvGammaCorrection(float4 value, float gFactor)
{
    return pow(value, gFactor);
}

inline float4 GammaCorrection(float4 value, float gFactor)
{
    return pow(value, 1.0f / gFactor);
}

inline float3 GammaCorrection(float3 value, float gFactor)
{
    return pow(value, 1.0f / gFactor);
}

/****************************
Academy Color Encoding System
*****************************/

float3 ACESFilm(const float3 x)
{
    return clamp((x * (2.51f * x + 0.03f)) / (x * (2.43f * x + 0.59f) + 0.14f), 0.0f, 1.0f);
}

/****************************
        BRDF Functions
*****************************/

struct MaterialPBR
{
    float3 albedo;
    float roughness;
    float metallic;
};

struct CookTorranceInput
{
    float3 worldNormal;
    float3 viewDirection;
    float3 lightDirection;
};

float GeometrySchlickGGX(float NdotX, float roughness)
{
    float k = (roughness + 1.0);
    k = (k * k) / 8.0;
    return NdotX / (NdotX * (1.0 - k) + k);
}

void CookTorranceBRDF(CookTorranceInput input, MaterialPBR material, out float3 diffuse, out float3 specular, out float NdotL)
{
    float3 halfVector = normalize(input.viewDirection + input.lightDirection);

    // GGX Normal Distribution
    float a = material.roughness * material.roughness;
    float a2 = a * a;

    float NdotH = saturate(dot(input.worldNormal, halfVector));
    float denom = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
    float D = a2 / (PI * denom * denom);

    // Frensel (Schlick Approximation)
    float3 F0 = lerp(
        float3(0.04f, 0.04f, 0.04f),
        material.albedo,
        material.metallic
    );

    float VdotH = saturate(dot(input.viewDirection, halfVector));
    float3 F = F0 + (1.0f - F0) * (pow(1.0f - VdotH, 5.0f));

    // Geometry Function (Smith)
    float k  = (material.roughness + 1.0f);
    k = (k * k) / 8.0f;

    float NdotV = saturate(dot(input.worldNormal, input.viewDirection));
    NdotL = saturate(dot(input.worldNormal, input.lightDirection));

    float G = GeometrySchlickGGX(NdotV, material.roughness) * GeometrySchlickGGX(NdotL, material.roughness);

    // Cook-Torrance Specular
    float3 numerator = D * G * F;
    float denominator = 4.0f * NdotV * NdotL;

    specular = numerator / max(denominator, 0.001f);

    // Eergy conservation
    float3 kS = F;
    float3 kD = 1.0f - kS;
    kD *= (1.0f - material.metallic);

    // Diffuse
    diffuse = kD * material.albedo / PI;
}




/****************************
         Point Light
*****************************/

struct PointLightData
{
    float3 lightPosition;
    float lightAmbient;
    float3 lightColor;
    float lightIntencity;
    float lightSpecular;
    float3 padding;
};

void CalculatePointLight(PixelData input, MaterialData material, PointLightData light, CameraData camera,
    out float4 ambient, out float4 diffuse, out float4 specular)
{
    // Ambient lightning
    ambient = light.lightAmbient * material.materialAmbient;
    
    // Diffuse Lighting
    float3 lightDistance = light.lightPosition - input.worldPos;
    float3 lightDirection = normalize(lightDistance);
    
    float d = length(lightDistance);
    float r2 = d * d;
    float attenuation = 1.0f / (1.0f + r2);

    float cosTheta = max(dot(input.normal, lightDirection), 0);
        
    diffuse = float4(light.lightColor, 1.0f) * 1.0f / max(r2, 0.0001f) * material.materialDiffuse * light.lightIntencity * cosTheta;
    
    // Specular Lighting
    float3 viewDir = normalize(camera.cameraPosition.xyz - input.worldPos);
    float3 halfVector = normalize(lightDirection + viewDir);
    float cosDelta = saturate(dot(input.normal, halfVector));
    
    specular = float4(light.lightColor, 1) * 1 / r2 * light.lightSpecular * material.materialSpecular * pow(cosDelta, max(material.materialShininess, 0.0001f));
    specular *= step(0.0f, cosTheta);
}

struct CookTorranceLightInput
{
    float3 worldPosition;
    float3 worldNormal;

    float3 cameraPosition;

    float3 lightPosition;
    float lightIntencity;
};


float3 CalculatePointLightCookTorrance(CookTorranceLightInput input, MaterialPBR material)
{
    // Light direction
    float3 lightDistance = input.lightPosition - input.worldPosition;
    float3 lightDirection = normalize(lightDistance);

    // Attenuation
    float d = length(lightDistance);
    float r2 = d * d;
    float attenuation = 1.0f / (1.0f + r2);

    // View direction
    float3 viewDirection = normalize(input.cameraPosition - input.worldPosition);

    CookTorranceInput inputCT = {
        input.worldNormal,
        viewDirection,
        lightDirection,
    };

    float3 diffuse, specular;
    float NdotL;
    CookTorranceBRDF(inputCT, material, diffuse, specular, NdotL);

    return (diffuse + specular) * input.lightIntencity * attenuation * max(NdotL, 0.0f);
}

/****************************
         SpotLight
*****************************/

struct SpotLightData
{
    PointLightData base;
    float3 lightDirection;
    float lightSpotFactor;
    float4x4 lightVp;
};

void CalculateSpotLight(PixelData input, MaterialData material, SpotLightData light, CameraData camera,
    out float4 ambient, out float4 diffuse, out float4 specular)
{
    // Ambient lightning
    ambient = light.base.lightAmbient * material.materialAmbient;
    
    // Diffuse Lighting
    float3 lightDistance = light.base.lightPosition - input.worldPos;
    float3 lightDirection = normalize(lightDistance);
    
    // Calculate Spot Factor
    float spot = pow(max(dot(-lightDirection, normalize(light.lightDirection)), 0), light.lightSpotFactor);
    
    float cosTheta = max(dot(input.normal, lightDirection), 0);
    float r2 = pow(length(lightDistance), 2);
        
    diffuse = float4(light.base.lightColor, 1) * 1 / r2 * spot * material.materialDiffuse * light.base.lightIntencity * cosTheta;
    
    // Specular Lighting
    float3 viewDir = normalize(camera.cameraPosition.xyz - input.worldPos);
    float3 lightViewDir = lightDirection + viewDir;
    float3 halfVector = lightViewDir / length(lightViewDir);
    float cosDelta = dot(input.normal, halfVector);
    
    specular = float4(light.base.lightColor, 1) * 1 / r2 * spot * light.base.lightSpecular * material.materialSpecular * pow(cosDelta, max(material.materialShininess, 0.0001f));
    
    specular = max(specular, 0);
    diffuse = max(diffuse, 0);
}

/****************************
      DirectionalLight
*****************************/

struct DirectionalLightData
{
    PointLightData base;
    float3 lightDirection;
    float padding;
    float4x4 lightVp;
};

void CalculateDirectionalLight(PixelData input, MaterialData material, DirectionalLightData light, CameraData camera,
    out float4 ambient, out float4 diffuse, out float4 specular)
{
      // Ambient lightning
    ambient = light.base.lightAmbient * material.materialAmbient;
    
    // Diffuse Lighting
    float3 lightDistance = light.base.lightPosition - input.worldPos;
    float3 lightDirection = normalize(lightDistance);
    
    // Calculate Spot Factor
    float spot = max(dot(-lightDirection, normalize(light.lightDirection)), 0);
    
    float cosTheta = max(dot(input.normal, lightDirection), 0);
    float r2 = pow(length(lightDistance), 2);
        
    diffuse = float4(light.base.lightColor, 1) * spot * material.materialDiffuse * light.base.lightIntencity * cosTheta;
    
    // Specular Lighting
    float3 viewDir = normalize(camera.cameraPosition.xyz - input.worldPos);
    float3 lightViewDir = lightDirection + viewDir;
    float3 halfVector = lightViewDir / length(lightViewDir);
    float cosDelta = dot(input.normal, halfVector);
    
    specular = float4(light.base.lightColor, 1) * spot * light.base.lightSpecular * material.materialSpecular * pow(cosDelta, max(material.materialShininess, 0.0001f));
    
    specular = max(specular, 0);
    diffuse = max(diffuse, 0);
}


/****************************
         Shadow
*****************************/

#define SHADOW_EPSILON 0
#define SHADOW_SIZE 2048.0f
#define SHADOW_THRESHOLD 0.001f

float CalculateShadow(float4 pixelPos, int lightIndex, SpotLightData light, SamplerState shadowSampler, Texture2DArray<float> shadowMaps)
{
    // Claculate Shading
    float4 lightClipSpace = mul(pixelPos, light.lightVp);
    float4 ndcPos = lightClipSpace / lightClipSpace.w;
    
    // y-multiplied by -0.5 to compensate for D3D11 format
    float3 shadowUV = float3(ndcPos.x * 0.5f + 0.5f, ndcPos.y * -0.5f + 0.5f, lightIndex);
    
    // Sample value from shadowMap
    const float dx = 1.0f / SHADOW_SIZE;
    
    const uint nOffsets = 5;
    float3 offsets[nOffsets] = { float3(-dx, dx, 0), float3(dx, dx, 0), float3(0, 0, 0), float3(-dx, -dx, 0), float3(dx, -dx, 0) };
    
    float shadowValue = 0;
    for (uint i = 0; i < nOffsets; i++)
        shadowValue += (shadowMaps.SampleLevel(shadowSampler, shadowUV + offsets[i], 0) + SHADOW_THRESHOLD < ndcPos.z) ? 0.0f : 1.0f;
    
    return shadowValue / nOffsets;
}

float CalculateShadowDir(float4 pixelPos, int lightIndex, DirectionalLightData light, SamplerState shadowSampler, Texture2DArray<float> shadowMaps)
{
    // Claculate Shading
    float4 lightClipSpace = mul(pixelPos, light.lightVp);
    float4 ndcPos = lightClipSpace / lightClipSpace.w;
    
    // y-multiplied by -0.5 to compensate for D3D11 format
    float3 shadowUV = float3(ndcPos.x * 0.5f + 0.5f, ndcPos.y * -0.5f + 0.5f, lightIndex);
    
    // Sample value from shadowMap
    const float dx = 1.0f / SHADOW_SIZE;
    
    const uint nOffsets = 5;
    float3 offsets[nOffsets] = { float3(-dx, dx, 0), float3(dx, dx, 0), float3(0, 0, 0), float3(-dx, -dx, 0), float3(dx, -dx, 0) };
    
    float shadowValue = 0;
    for (uint i = 0; i < nOffsets; i++)
        shadowValue += (shadowMaps.SampleLevel(shadowSampler, shadowUV + offsets[i], 0) + SHADOW_THRESHOLD < ndcPos.z) ? 0.0f : 1.0f;
    
    return shadowValue / nOffsets;
}
