Texture2D input : register(t0);
RWTexture2D<float4> output : register(u0);

SamplerState samp : register(s0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 inDim, outDim;
	input.GetDimensions(inDim.x, inDim.y);
	output.GetDimensions(outDim.x, outDim.y);
	
    if (DTid.x >= outDim.x || DTid.y >= outDim.y)
        return;

	uint2 inCoord = uint2(float2(DTid.xy) * (float2(inDim) / float2(outDim)));
    
	uint2 pixelsToSample = inDim / outDim;
	float4 result = float4(0.0, 0.0, 0.0, 0.0);
	
	for (uint y = 0; y < pixelsToSample.y; y++)
		for (uint x = 0; x < pixelsToSample.x; x++)
			result += input.SampleLevel(samp, float2(inCoord + uint2(x, y)) / float2(inDim), 0);
	
	output[DTid.xy] = result / (float)(pixelsToSample.x * pixelsToSample.y);
}