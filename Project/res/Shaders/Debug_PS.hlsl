struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

cbuffer DebugData : register(b3)
{
    uint DebugViewMode;
    float DebugScale;   // UV scale, depth scale, etc.
    float DebugBias;
    uint DebugFlags;
};

SamplerState m_sampler : register(s0);

Texture2D m_texture : register(t0); // sRGB texture


float4 main(PixelShaderInput input) : SV_TARGET
{
    // Sample pixel color (assumes sRGB texture)
    float4 pixelColor = m_texture.Sample(m_sampler, input.uv);
    return pixelColor;
}