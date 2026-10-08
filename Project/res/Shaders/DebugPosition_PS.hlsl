struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    return float4(input.position.xyz, 1.0f);
}