Texture2D backbuffer : register(t0);
Texture2D bloom : register(t1);
RWTexture2D<float4> output : register(u0);

SamplerState samp : register(s0);

cbuffer BloomSettings : register(b0)
{
    float threshold;
    float bloomStrength;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint2 outDim;
    output.GetDimensions(outDim.x, outDim.y);
    float2 uv = (float2)DTid.xy / (float2)outDim;

    float3 origionalColor = backbuffer[DTid.xy].rgb;
    float3 bloomColor = bloom.SampleLevel(samp, uv, 0);

    output[DTid.xy] = float4(origionalColor + bloomColor * bloomStrength, 1.0f);
}