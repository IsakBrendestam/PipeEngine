struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 worldPos : WPOS;
    float4 color : COLOR;
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

    output.position = mul(output.position, matWorld);
    output.worldPos = output.position;
    output.position = mul(output.position, matViewProjection);
    
    output.normal = float4(input.normal, 0);
    output.normal = mul(output.normal, matWorld);
    output.normal = normalize(output.normal);
    
    output.color = float4(0, 1, 0, 1);
    return output;
}