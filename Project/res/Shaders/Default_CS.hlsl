cbuffer csVars : register(b0)
{
    double m_delta;
    double m_time;
}

RWTexture2D<float4> backBufferUAV : register(u0);

Texture2D input : register(t0);
SamplerState samp : register(s0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint2 dimensions;
    backBufferUAV.GetDimensions(dimensions.x, dimensions.y);
    float2 uv = float2((float) DTid.x / (float) dimensions.x, (float) DTid.y / (float) dimensions.y);
  
    backBufferUAV[DTid.xy] = float4(input.SampleLevel(samp, uv, 0).rgb, 1);
}
