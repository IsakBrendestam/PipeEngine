#include "Common.hlsli"

struct BenDaySettings
{
    float2 resolution;
    float dotSpacing;
    float dotScale;
    float3 angles;
    float temp; // Padding
};

float2 Rotate(float2 p, float angle)
{
    float rad = radians(angle);
    float s = sin(rad);
    float c = cos(rad);
    float2x2 r = float2x2(c, -s, s, c);
    return mul(p, r);
}

float SampleGridAverage(InputTexture input, float2 centerUV, float2 texelSize, int kernel, int channelIndex)
{
    float sum = 0.0f;
    int count = 0;
    for (int y = -kernel; y <= kernel; ++y)
    {
        for (int x = -kernel; x <= kernel; ++x)
        {
            float2 offset = float2(x, y) * texelSize;
            sum += input.inputBuffer.SampleLevel(input.samp, centerUV + offset, 0)[channelIndex];
            count++;
        }
    }
    return sum / count;
}

float ChannelDot(float2 uv, float angle, int channelIndex, BenDaySettings settings, InputTexture input)
{
    float2 texelSize = 1.0f / settings.resolution;

    float2 pixelCoord = uv * settings.resolution;
    float2 center = settings.resolution * 0.5f;
    float2 gridCoord = Rotate((pixelCoord - center) / settings.dotSpacing, angle);

    float2 cell = floor(gridCoord);
    float2 local = frac(gridCoord);

    float2 cellCenter = (cell + 0.5) * settings.dotSpacing;
    float2 rotatedCellCenter = Rotate(cellCenter, -angle) + center;
    
    float2 centerUV = rotatedCellCenter / settings.resolution;

    float average = SampleGridAverage(input, centerUV, texelSize * settings.dotSpacing / 2.0f, 1, channelIndex);

    float radius = average * 0.5f * settings.dotScale;

    float2 offset = (local - 0.5f);
    float dist = length(offset);

    float mask = 1.0f - step(radius, dist);
    return mask;
}

float4 BenDay(int2 xy, BenDaySettings settings, InputTexture input)
{
    float2 uv = xy / settings.resolution;

    float r = ChannelDot(uv, settings.angles.r, 0, settings, input);
    float g = ChannelDot(uv, settings.angles.g, 1, settings, input);
    float b = ChannelDot(uv, settings.angles.b, 2, settings, input);

    return float4(r, g, b, 1.0f);
}
