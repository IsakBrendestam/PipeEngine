
float3 FXAA(Texture2D tex, SamplerState samplerState, float2 uv, float2 inverseScreenSize)
{
    const float EDGE_THRESHOLD_MIN = 0.0312;
    const float EDGE_THRESHOLD_MAX = 0.125;
    const float SUBPIXEL_QUALITY    = 0.75;

    float3 rgbM = tex.SampleLevel(samplerState, uv, 0).rgb;

    float3 rgbNW = tex.SampleLevel(samplerState, uv + float2(-1.0, -1.0) * inverseScreenSize, 0).rgb;
    float3 rgbNE = tex.SampleLevel(samplerState, uv + float2( 1.0, -1.0) * inverseScreenSize, 0).rgb;
    float3 rgbSW = tex.SampleLevel(samplerState, uv + float2(-1.0,  1.0) * inverseScreenSize, 0).rgb;
    float3 rgbSE = tex.SampleLevel(samplerState, uv + float2( 1.0,  1.0) * inverseScreenSize, 0).rgb;

    float lumaM  = dot(rgbM,  float3(0.299, 0.587, 0.114));
    float lumaNW = dot(rgbNW, float3(0.299, 0.587, 0.114));
    float lumaNE = dot(rgbNE, float3(0.299, 0.587, 0.114));
    float lumaSW = dot(rgbSW, float3(0.299, 0.587, 0.114));
    float lumaSE = dot(rgbSE, float3(0.299, 0.587, 0.114));

    float lumaMin = min(
        lumaM,
        min(min(lumaNW, lumaNE), min(lumaSW, lumaSE))
    );

    float lumaMax = max(
        lumaM,
        max(max(lumaNW, lumaNE), max(lumaSW, lumaSE))
    );

    float lumaRange = lumaMax - lumaMin;

    if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX))
    {
        return rgbM;
    }

    float3 rgbAvg = 0.5 * (
        rgbNW + rgbNE +
        rgbSW + rgbSE
    );

    float3 rgbA = 0.5 * (
        rgbAvg + rgbM
    );

    float lumaA = dot(rgbA, float3(0.299, 0.587, 0.114));

    if (lumaA < lumaMin || lumaA > lumaMax)
    {
        return rgbA;
    }

    float3 rgbB = 0.25 * (
        rgbAvg + rgbM +
        tex.SampleLevel(
            samplerState,
            uv + float2(-1.0, 0.0) * inverseScreenSize,
            0
        ).rgb +
        tex.SampleLevel(
            samplerState,
            uv + float2(1.0, 0.0) * inverseScreenSize,
            0
        ).rgb
    );

    float lumaB = dot(rgbB, float3(0.299, 0.587, 0.114));

    if (lumaB < lumaMin || lumaB > lumaMax)
    {
        return rgbA;
    }

    return rgbB;
}


RWTexture2D<float4> backBufferUAV : register(u0);

Texture2D inputBuffer : register(t0);
SamplerState samp : register(s0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint2 dimensions;
    backBufferUAV.GetDimensions(dimensions.x, dimensions.y);

    if (DTid.x >= dimensions.x || DTid.y >= dimensions.y)
        return;
    
    float2 uv = (float2(DTid.xy) + 0.5f) / (float2)dimensions;
   
    backBufferUAV[DTid.xy] = float4(FXAA(
        inputBuffer,
        samp,
        uv,
        1.0f / float2(dimensions)
    ), 1.0f);
}
