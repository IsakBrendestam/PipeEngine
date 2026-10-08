struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

SamplerState m_sampler : register(s0);

Texture2D m_roughnessMap : register(t3); // R Texture

float4 main(PixelShaderInput input) : SV_TARGET
{
    float roughness = m_roughnessMap.Sample(m_sampler, input.uv).g;
    return float4(roughness, roughness, roughness, 1);
}