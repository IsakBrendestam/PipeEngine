Texture2D highRes : register(t0);
Texture2D lowRes  : register(t1);

RWTexture2D<float4> output : register(u0);

SamplerState samp : register(s0);

float4 TentFilter(float2 uv, float2 texelSize)
{
	float4 result = 0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2(-1, -1),
        0) * 1.0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2( 0, -1),
        0) * 2.0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2( 1, -1),
        0) * 1.0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2(-1,  0),
        0) * 2.0;

    result += lowRes.SampleLevel(
        samp,
        uv,
        0) * 4.0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2(1, 0),
        0) * 2.0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2(-1, 1),
        0) * 1.0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2(0, 1),
        0) * 2.0;

    result += lowRes.SampleLevel(
        samp,
        uv + texelSize * float2(1, 1),
        0) * 1.0;

    return result / 16.0;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint2 outDim;
    output.GetDimensions(outDim.x, outDim.y);

    if (DTid.x >= outDim.x || DTid.y >= outDim.y)
        return;

    float2 uv = (float2(DTid.xy) + 0.5f) / float2(outDim);

	uint2 lowDim;
	lowRes.GetDimensions(lowDim.x, lowDim.y);

	float2 lowTexelSize = 1.0f/float2(lowDim);

    float4 low = TentFilter(
		uv,
		lowTexelSize
	);

    float4 high = highRes.Load(int3(DTid.xy, 0));

    output[DTid.xy] = low + high;
}