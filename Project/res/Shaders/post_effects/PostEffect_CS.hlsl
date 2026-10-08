#include "PostEffects.hlsli"

#define CRT_FLAG    (1 << 0)  // 0001
#define BENDAY_FLAG (1 << 1)  // 0010
#define SOBEL_FLAG  (1 << 2)  // 0100

cbuffer control : register(b0)
{
    uint effectFlag;
    uint temp[3]; // Padding
}

cbuffer scanline : register(b1)
{
    ScanlineSettings scanlineSettings;
}

cbuffer benday : register(b2)
{
    BenDaySettings benDaySettings;
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
    
    InputTexture res = {inputBuffer, samp};

    if (effectFlag & CRT_FLAG)
        backBufferUAV[DTid.xy] = CRT(uv, scanlineSettings, res);
    else if (effectFlag & BENDAY_FLAG)
        backBufferUAV[DTid.xy] = BenDay(DTid.xy, benDaySettings, res);
    else if (effectFlag & SOBEL_FLAG)
        backBufferUAV[DTid.xy] = Sobel(DTid.xy, res);
    else 
        backBufferUAV[DTid.xy] = inputBuffer[DTid.xy];
}
