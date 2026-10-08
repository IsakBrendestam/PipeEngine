struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

SamplerState m_sampler : register(s0);

Texture2D m_ambientOcclusion : register(t2); // RGB texture

float4 main(PixelShaderInput input) : SV_TARGET
{
    float ambientOcclusion = m_ambientOcclusion.Sample(m_sampler, input.uv).r;
    return float4(ambientOcclusion, ambientOcclusion, ambientOcclusion, 1);
}