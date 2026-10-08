RWTexture2D<float4> backBufferUAV : register(u0);

Texture2D input : register(t0);
Texture2D<float> depthTexture : register(t1);

SamplerState samp : register(s0);

cbuffer TransformationBuffer : register(b0)
{
    float4x4 inverseProjection;
};

const static float blurScale = 4.0f;

float LinearizeValue(float depth, float minimum, float maximum)
{
    return (minimum * maximum) / (maximum - depth * (maximum - minimum));
}

float3 homogenize(float4 v)
{
    return v.xyz * (1.0f / v.w);
}

float3 perpendicular(float3 v)
{
    float3 av = abs(v);
    if (av.x < av.y)
        return (av.x < av.z) ? float3(0.0f, -v.z, v.y) : float3(-v.y, v.x, 0.0f);
    return (av.y < av.z) ? float3(-v.z, 0.0f, v.x) : float3(-v.y, v.x, 0.0f);    
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint2 dimensions;
    backBufferUAV.GetDimensions(dimensions.x, dimensions.y);
    float2 uv = float2((float) DTid.x / (float) dimensions.x, (float) DTid.y / (float) dimensions.y);

    float2 ndcXY = uv * 2.0f - 1.0f;

    float depth = input[DTid.xy].r;
    float4 ndc = float4(ndcXY, depth, 1.0f);
    
    float3 position = homogenize(mul(inverseProjection, ndc));

    float3 visualization = position * 0.5f + 0.5f;
    float depthVisualization = saturate(position.z / 100.0f);

    backBufferUAV[DTid.xy] = float4(depthVisualization.xxx, 1.0f);
}
