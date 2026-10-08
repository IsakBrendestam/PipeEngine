Texture2D input : register(t0);
SamplerState samp : register(s0);
RWTexture2D<float4> backBufferUAV : register(u0);

cbuffer ShaderParams : register(b0)
{
    float2 resolution;
    float dotSpacing;
    float dotScale;
    float3 angles;
    float temp; // Padding
}

float2 Rotate(float2 p, float angle)
{
    float rad = radians(angle);
    float s = sin(rad);
    float c = cos(rad);
    float2x2 r = float2x2(c, -s, s, c);
    return mul(p, r);
}

float SampleGridAverage(Texture2D tex, SamplerState samp, float2 centerUV, float2 texelSize, int kernel, int channelIndex)
{
    float sum = 0.0f;
    int count = 0;
    for (int y = -kernel; y <= kernel; ++y)
    {
        for (int x = -kernel; x <= kernel; ++x)
        {
            float2 offset = float2(x, y) * texelSize;
            sum += tex.SampleLevel(samp, centerUV + offset, 0)[channelIndex];
            count++;
        }
    }
    return sum / count;
}

float ChannelDot(float2 uv, float angle, int channelIndex)
{
    float2 texelSize = 1.0f / resolution;

    float2 pixelCoord = uv * resolution;
    float2 center = resolution * 0.5f;
    float2 gridCoord = Rotate((pixelCoord - center) / dotSpacing, angle);

    float2 cell = floor(gridCoord);
    float2 local = frac(gridCoord);

    float2 cellCenter = (cell + 0.5) * dotSpacing;
    float2 rotatedCellCenter = Rotate(cellCenter, -angle) + center;
    
    float2 centerUV = rotatedCellCenter / resolution;

    float average = SampleGridAverage(input, samp, centerUV, texelSize * dotSpacing / 2.0f, 1, channelIndex);

    float radius = average * 0.5f * dotScale;

    float2 offset = (local - 0.5f);
    float dist = length(offset);

    float mask = 1.0f - step(radius, dist);
    return mask;
}

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float2 uv = id.xy / resolution;

    float r = ChannelDot(uv, angles.r, 0);
    float g = ChannelDot(uv, angles.g, 1);
    float b = ChannelDot(uv, angles.b, 2);

    float3 color = float3(r, g, b);
    backBufferUAV[id.xy] = float4(color, 1.0f);
}
