struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 worldPos : WPOS;
    float2 uv : UV;
};

cbuffer TransformationBuffer : register(b0)
{
    float4x4 matWorld;
    float4x4 matViewProjection;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.position = float4(input.position, 1.0f);

    output.worldPos = mul(output.position, matWorld);
    output.position = mul(output.worldPos, matViewProjection);
    
    output.normal = float4(input.normal, 0);
    output.normal = mul(output.normal, matWorld);
    output.normal = normalize(output.normal);
    
    output.uv = input.uv;
    
    return output;
}