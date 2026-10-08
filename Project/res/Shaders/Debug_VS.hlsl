struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 worldPos : WPOS;
    float2 uv : UV;
    float3x3 tbn : TBN;
};

cbuffer TransformationBuffer : register(b0)
{
    float4x4 matWorld;
    float4x4 matViewProjection;
};

cbuffer MeshDataBuffer : register(b1)
{
    float3 aabbCenter;
    float aabbMaxExtent;
}

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    // Uniform scale
    float scale = 1.0f / aabbMaxExtent;
    output.worldPos = float4((input.position - aabbCenter) * scale, 1);
    output.worldPos = mul(output.worldPos, matWorld);
    output.position = mul(output.worldPos, matViewProjection);
     
    output.uv = input.uv;

    float3 T = normalize(mul(float4(input.tangent, 0), matWorld).xyz);
    float3 B = normalize(mul(float4(input.bitangent, 0), matWorld).xyz);
    float3 N = normalize(mul(float4(input.normal, 0), matWorld).xyz);
    output.tbn = float3x3(T, B, N);
    
    return output;
}