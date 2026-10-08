struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

SamplerState m_sampler : register(s0);

Texture2D m_metalnessMap : register(t4); // R Texture

float4 main(PixelShaderInput input) : SV_TARGET
{
    float metalness = m_metalnessMap.Sample(m_sampler, input.uv).b;
    return float4(metalness, metalness, metalness, 1);
}