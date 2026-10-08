#include "ToneMappingHelper.hlsli"

#define REINHARD_MAPPING    1
#define ACES_MAPPING        2
#define UNCHARTED2_MAPPING  3

cbuffer settings : register(b0)
{
    int toneMap;
    float gammaFactor;
    int3 padding;
}

RWTexture2D<float4> backBufferUAV : register(u0);

Texture2D inputBuffer : register(t0);
SamplerState samp : register(s0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint2 dimensions;
    backBufferUAV.GetDimensions(dimensions.x, dimensions.y);
    
    float2 uv = (float2)DTid.xy / (float2)dimensions;
   
    float3 mapped;

    if (toneMap == REINHARD_MAPPING)
        mapped = ReinhardToneMap(inputBuffer[DTid.xy].xyz);
    else if (toneMap == ACES_MAPPING)
        mapped = ACESToneMap(inputBuffer[DTid.xy].xyz);
    else if (toneMap == UNCHARTED2_MAPPING)
        mapped = Uncharted2ToneMap(inputBuffer[DTid.xy].xyz);
    else
        mapped = inputBuffer[DTid.xy].xyz;

    // Final LDR output: clamp before gamma so untonemapped or negative values stay valid
    mapped = GammaCorrection(saturate(mapped), gammaFactor);
    backBufferUAV[DTid.xy] = float4(mapped, 1.0f);
}
