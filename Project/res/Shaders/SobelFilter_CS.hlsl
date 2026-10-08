RWTexture2D<float4> backBufferUAV : register(u0);

Texture2D input : register(t0);
SamplerState samp : register(s0);

float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{    
    uint2 dimensions;
    backBufferUAV.GetDimensions(dimensions.x, dimensions.y);

    if (DTid.x >= dimensions.x || DTid.y >= dimensions.y)
        return;


    float2 uv = float2((float) DTid.x / (float) dimensions.x, (float) DTid.y / (float) dimensions.y);

    float4 c[3][3];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int2 xy = DTid.xy + int2(j - 1, i - 1);
            c[i][j] = input[xy];
        }
    }

    float4 Gx = -1.0f * c[0][0] - 2.0f * c[1][0] - 1.0f * c[2][0] + 1.0f * c[0][2] + 2.0f * c[1][2] + 1.0f * c[2][2];
    float4 Gy = -1.0f * c[2][0] - 2.0f * c[2][1] - 1.0f * c[2][1] + 1.0f * c[0][0] + 2.0f * c[0][1] + 1.0f * c[0][2];

    float4 mag = sqrt(Gx * Gx + Gy * Gy);

    mag = saturate(CalcLuminance(mag.rgb));

    backBufferUAV[DTid.xy] = float4(mag.rgb, 1);
}
