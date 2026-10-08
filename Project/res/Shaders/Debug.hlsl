#ifndef DebugValue
#define DebugValue 3
#endif

#ifndef Test
#define Test 1
#endif

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

SamplerState m_sampler : register(s0);

Texture2D m_texture : register(t0); // sRGB texture
Texture2D normalMap : register(t1); // RGB Texture
Texture2D m_ambientOcclusion : register(t2); // RGB texture

float4 main(PixelShaderInput input) : SV_TARGET
{
    if (DebugValue == 0) {
        float4 pixelColor = m_texture.Sample(m_sampler, input.uv);
        return pixelColor;
    }

    if (DebugValue == 1) {
        float4 normal = normalMap.Sample(m_sampler, input.uv);
        return float4(normalize(mul(normal.xyz, input.tbn)), 1) * 0.5f + 0.5f;
    }

    if (DebugValue == 2) {
        float ambientOcclusion = m_ambientOcclusion.Sample(m_sampler, input.uv).r;
        return float4(ambientOcclusion, ambientOcclusion, ambientOcclusion, 1);
    }

    return float4(Test, Test, Test, Test);
}