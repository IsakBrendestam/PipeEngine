struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

SamplerState m_sampler : register(s0);

Texture2D normalMap : register(t1); // RGB Texture

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 normal = normalMap.Sample(m_sampler, input.uv);
    return float4(normalize(mul(normal.xyz, input.tbn)), 1) * 0.5f + 0.5f;
}